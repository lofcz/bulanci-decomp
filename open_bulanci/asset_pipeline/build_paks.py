"""Compile folder paks into distributable binary paks (pack v2).

Dev layout (git-diffable):

    open_bulanci/paks/profile.json        # active pak set + load order
    open_bulanci/paks/<id>/pak.json       # per-pak manifest ("package.json")
    open_bulanci/paks/<id>/<files...>     # hash-named overrides + new assets
    open_bulanci/paks/<id>/scenes/*.luau  # scene/behavior modules

Dist layout (this script's output):

    open_bulanci/paks/dist/<id>.pak       # binary pack v2 (self-describing)
    open_bulanci/paks/profile.shipped.json# the load order, pointing at .pak files

Binary pack v2 frame (little-endian), parsed by
`open_bulanci/core/src/assets.rs::parse_pack`:

    u32  magic   = 0x504B4C42 ('BLKP')
    u32  version = 2
    u32  header_len
    u8[] header                 # UTF-8 JSON: {"id","kind","order"}
    u32  count
    repeat count times:
      u64  hash   (little-endian; same blake2b-64 the runtime keys on)
      u32  data_len
      u8[] data

Two invariants this enforces, matching the rest of the pipeline:

  * **Names resolve to hashes at compile time.** A pak's `assets` table is
    already keyed by the 16-hex handle hash, and scenes resolve to
    blake2b("scenes/<name>"); the emitted binary carries only hashes +
    bytes, never slugs/paths.
  * **Luau ships as stripped bytecode.** `.luau`/`.lua` content is compiled
    via the `luau_compile` helper (`core/src/bin/luau_compile.rs`,
    debug_level=0) so no identifiers leak. If the helper can't be built
    (no toolchain), the source is embedded with a loud warning.

Usage::

    python open_bulanci/asset_pipeline/build_paks.py            # all mod paks
    python open_bulanci/asset_pipeline/build_paks.py --pak modX  # one pak
    python open_bulanci/asset_pipeline/build_paks.py --no-luau   # skip bytecode
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[2]
_PAKS_DIR = _REPO_ROOT / "open_bulanci" / "paks"
_DIST_DIR = _PAKS_DIR / "dist"
_PROFILE = _PAKS_DIR / "profile.json"
_SHIPPED_PROFILE = _PAKS_DIR / "profile.shipped.json"

_PACK_MAGIC = 0x504B4C42
_PACK_VERSION_V2 = 2

_LUAU_EXTS = {".luau", ".lua"}
# Metadata files that describe a pak but are not shipped content.
_SKIP_NAMES = {"pak.json"}


def _hash_raw(path: str) -> int:
    """blake2b-64(path), big-endian — identical to the runtime's
    `hash_path(&str)` and to `pack_assets._hash_raw_path`."""
    return int.from_bytes(hashlib.blake2b(path.encode("utf-8"), digest_size=8).digest(), "big")


def _is_hex16(stem: str) -> bool:
    return len(stem) == 16 and all(c in "0123456789abcdefABCDEF" for c in stem)


# --- Luau bytecode compilation ------------------------------------------

class _LuauCompiler:
    """Locates (and, if needed, builds) the `luau_compile` helper, then
    compiles Luau source to stripped bytecode. Degrades gracefully to
    embedding source if the toolchain isn't available."""

    def __init__(self, enabled: bool, keep_debug: bool) -> None:
        self.enabled = enabled
        self.keep_debug = keep_debug
        self.exe: Path | None = None
        self.failed = False
        if enabled:
            self.exe = self._locate_or_build()
            if self.exe is None:
                print("  [luau] helper unavailable - embedding Luau as SOURCE (names will leak)")

    def _candidate_paths(self) -> list[Path]:
        exe_name = "luau_compile.exe" if sys.platform == "win32" else "luau_compile"
        out = []
        for profile in ("release", "debug"):
            out.append(_REPO_ROOT / "open_bulanci" / "target" / profile / exe_name)
        return out

    def _locate_or_build(self) -> Path | None:
        for cand in self._candidate_paths():
            if cand.is_file():
                return cand
        # Try to build it once.
        print("  [luau] building luau_compile helper (cargo build -p bulanci_core --bin luau_compile --release)")
        try:
            subprocess.run(
                ["cargo", "build", "-p", "bulanci_core", "--bin", "luau_compile", "--release"],
                cwd=_REPO_ROOT / "open_bulanci",
                check=True,
            )
        except (subprocess.CalledProcessError, FileNotFoundError) as e:
            print(f"  [luau] build failed: {e}")
            return None
        for cand in self._candidate_paths():
            if cand.is_file():
                return cand
        return None

    def compile(self, src_path: Path) -> bytes:
        """Return bytecode for `src_path`, or its raw source on failure."""
        if not self.enabled or self.exe is None:
            return src_path.read_bytes()
        with tempfile.TemporaryDirectory() as td:
            out = Path(td) / "out.luac"
            cmd = [str(self.exe)]
            if self.keep_debug:
                cmd.append("--keep-debug")
            cmd += [str(src_path), str(out)]
            try:
                subprocess.run(cmd, check=True)
                return out.read_bytes()
            except subprocess.CalledProcessError as e:
                print(f"  [luau] compile {src_path.name} failed ({e}); embedding source")
                self.failed = True
                return src_path.read_bytes()


# --- pak collection ------------------------------------------------------

def _load_json(p: Path) -> dict:
    return json.loads(p.read_text(encoding="utf-8"))


def _dump_control_registry() -> None:
    """Refresh `ghidra_analysis/asset_catalog/control-registry.json` from the
    engine's reflected control model so the scene editor's toolbox/property
    grid never drifts. Best-effort: a missing cargo toolchain is a warning,
    not a build failure (the JSON is committed and only changes when the
    control structs do)."""
    try:
        subprocess.run(
            ["cargo", "run", "-q", "-p", "bulanci_core", "--bin", "dump-registry"],
            cwd=_REPO_ROOT / "open_bulanci",
            check=True,
        )
        print("  [registry] refreshed control-registry.json")
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        print(f"  [registry] skipped control-registry refresh ({e})")


def _file_to_hash_map(manifest: dict) -> dict[str, int]:
    """Invert the manifest's hash-keyed tables into a `relfile -> hash`
    lookup so loose files can be matched back to their declared hash."""
    out: dict[str, int] = {}
    for hash_hex, asset in (manifest.get("assets") or {}).items():
        try:
            out[asset["file"].replace("\\", "/")] = int(hash_hex, 16)
        except (KeyError, ValueError):
            continue
    for name, scene in (manifest.get("scenes") or {}).items():
        # Legacy single-module scene: the .luau resolves to scenes/<name>.
        f = scene.get("file")
        if f:
            h = scene.get("hash")
            out[f.replace("\\", "/")] = int(h, 16) if h else _hash_raw(f"scenes/{name}")
        # Declarative scene: the doc + each script resolve to their literal
        # path hash (the same key the SceneManager reads them by). Listing
        # them is optional for hashing (the loose-file fallback computes the
        # same value), but it pins them so the editor can enumerate scenes.
        doc = scene.get("doc")
        if doc:
            dposix = doc.replace("\\", "/")
            out[dposix] = _hash_raw(dposix)
        for script in scene.get("scripts") or []:
            sposix = script.replace("\\", "/")
            out[sposix] = _hash_raw(sposix)
    # Levels mirror declarative scenes: the *.level.json doc + any gamemode
    # scripts resolve to their literal path hash (what `LevelSurface::load`
    # reads them by). Pinning them lets the editor enumerate levels; the
    # loose-file fallback computes the same hash regardless.
    for level in (manifest.get("levels") or {}).values():
        doc = level.get("doc")
        if doc:
            dposix = doc.replace("\\", "/")
            out[dposix] = _hash_raw(dposix)
        for script in level.get("scripts") or []:
            sposix = script.replace("\\", "/")
            out[sposix] = _hash_raw(sposix)
    return out


def _resolve_hash(rel: str, file_map: dict[str, int]) -> int:
    if rel in file_map:
        return file_map[rel]
    stem = Path(rel).stem
    if _is_hex16(stem):
        return int(stem, 16)
    return _hash_raw(rel)


def compile_pak(pak_dir: Path, pak_id: str, kind: str, order: int, luau: _LuauCompiler) -> Path:
    manifest = _load_json(pak_dir / "pak.json") if (pak_dir / "pak.json").is_file() else {}
    file_map = _file_to_hash_map(manifest)

    entries: dict[int, tuple[bytes, str]] = {}
    for fp in sorted(pak_dir.rglob("*")):
        if not fp.is_file():
            continue
        rel = fp.relative_to(pak_dir).as_posix()
        if fp.name in _SKIP_NAMES or rel.endswith(".schema.json"):
            continue
        if fp.suffix.lower() in _LUAU_EXTS:
            data = luau.compile(fp)
        else:
            data = fp.read_bytes()
        h = _resolve_hash(rel, file_map)
        if h in entries and entries[h][1] != rel:
            raise RuntimeError(
                f"[{pak_id}] hash collision 0x{h:016x}: {rel!r} and {entries[h][1]!r}"
            )
        entries[h] = (data, rel)

    header = json.dumps({"id": pak_id, "kind": kind, "order": order}, separators=(",", ":")).encode("utf-8")
    blob = bytearray()
    blob.extend(struct.pack("<II", _PACK_MAGIC, _PACK_VERSION_V2))
    blob.extend(struct.pack("<I", len(header)))
    blob.extend(header)
    blob.extend(struct.pack("<I", len(entries)))
    for h in sorted(entries):
        data, _rel = entries[h]
        blob.extend(struct.pack("<QI", h, len(data)))
        blob.extend(data)

    _DIST_DIR.mkdir(parents=True, exist_ok=True)
    out = _DIST_DIR / f"{pak_id}.pak"
    out.write_bytes(blob)
    print(f"  -> {out.relative_to(_REPO_ROOT)} ({len(entries)} entries, {len(blob)} bytes)")
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--pak", help="compile only this pak id (default: all mod paks)")
    ap.add_argument("--no-luau", action="store_true", help="embed Luau as source instead of bytecode")
    ap.add_argument("--keep-debug", action="store_true", help="keep Luau debug names (dev only)")
    ap.add_argument("--no-registry", action="store_true", help="skip refreshing the editor control registry")
    args = ap.parse_args()

    if not _PROFILE.is_file():
        print(f"no profile at {_PROFILE}; nothing to compile")
        return 0
    profile = _load_json(_PROFILE)
    luau = _LuauCompiler(enabled=not args.no_luau, keep_debug=args.keep_debug)

    shipped_paks = []
    order = 0
    for entry in profile.get("paks") or []:
        pid = entry["id"]
        kind = entry.get("kind", "mod")
        order += 1
        if kind == "master":
            # Master ships as the embedded `assets.pack` (built by
            # pack_assets.py); record it in the shipped profile only.
            shipped_paks.append({"id": pid, "kind": "master", "order": order})
            continue
        if args.pak and pid != args.pak:
            continue
        if not entry.get("enabled", True):
            continue
        rel = entry.get("path", pid)
        pak_dir = _PAKS_DIR / rel
        if not pak_dir.is_dir():
            print(f"  [{pid}] skipped: {pak_dir} not found")
            continue
        print(f"compiling pak '{pid}' (order {order}) from {pak_dir.relative_to(_REPO_ROOT)}")
        out = compile_pak(pak_dir, pid, kind, order, luau)
        shipped_paks.append({
            "id": pid,
            "kind": kind,
            "order": order,
            "file": f"dist/{out.name}",
        })

    if not args.pak:
        _SHIPPED_PROFILE.write_text(
            json.dumps({"schema": 1, "comment": "Generated by build_paks.py - load order for the shipped binary paks.", "paks": shipped_paks}, indent=2) + "\n",
            encoding="utf-8",
        )
        print(f"  -> {_SHIPPED_PROFILE.relative_to(_REPO_ROOT)}")

    if luau.enabled and luau.exe is None:
        print("WARNING: Luau shipped as source (no compiler). Names are NOT stripped.")

    # Keep the editor's reflected control descriptor in sync on a full build.
    if not args.no_registry and not args.pak:
        _dump_control_registry()
    return 0


if __name__ == "__main__":
    sys.exit(main())
