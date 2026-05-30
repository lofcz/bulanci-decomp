import hashlib
import os
import struct
import json
from pathlib import Path
import msgpack

# Files that get rewritten / specially handled when packing.  They
# never appear under `assets/` by these names; the pipeline produces
# them on the fly so a stray copy in the source tree can't accidentally
# shadow the canonical one.
_SLIM_MANIFEST   = "manifest.slim.json"   # source file
# Files that are present in `open_bulanci/assets/` for *dev* tooling
# but must NEVER be embedded in the shipped binary — their bytes would
# leak slugs/folders into `.rodata`.  zstd at level 9 doesn't reliably
# compress short ASCII strings, so the only safe thing is to keep them
# out of the pack entirely.
_DEV_ONLY_FILES = frozenset({
    "manifest.json",                       # fat dev manifest
    _SLIM_MANIFEST,                        # paths inside it would leak
})


def _hash_raw_path(path: str) -> int:
    """blake2b-64 of a raw source path.  Used **only** for files that
    don't have a typed handle (poems, transpiled Lua levels) — those
    are looked up via `vfs.read(path)` at runtime, which hashes the
    string locally with the same function.  Files that *do* have a
    typed handle are keyed by their handle's hash (computed by
    `build_typed_handles.py::hash_path("<folder>/<slug>")`), which we
    pull straight out of the slim manifest below.
    """
    return int.from_bytes(
        hashlib.blake2b(path.encode("utf-8"), digest_size=8).digest(),
        "big",
    )


def _strip_atlas_identifiers(data: dict) -> dict:
    """Strip user-meaningful name/image strings from an atlas /
    cursor-composition before it goes into the pack.

    The atlas/cursor msgpack blobs originally carry `name` (the slug)
    and `image` (e.g. `"atlas.png"`), and cursor compositions carry
    `idleTracks[*].atlas` (a slug list).  Leaving any of those in the
    pack would re-leak slugs into the compressed binary blob — zstd at
    level 9 happily passes short ASCII strings through unchanged.
    The runtime never consults these fields (the atlas / cursor
    loaders take typed handles for every part), so we drop them.
    """
    if not isinstance(data, dict):
        return data
    out = dict(data)
    out.pop("name",  None)
    out.pop("image", None)
    if isinstance(out.get("idleTracks"), list):
        out["idleTracks"] = [
            {k: v for k, v in entry.items() if k != "atlas"}
            for entry in out["idleTracks"]
        ]
    return out


# Hash-keyed binary pack v1:
#
#   u32  magic   = 0x504B4C42 ('BLKP' little-endian – "BuLanci asset PacK")
#   u32  version = 1
#   u32  count
#   repeat count times:
#     u64  blake2b_64(path)   little-endian, same as AssetHandle hashes
#     u32  data_len
#     u8[] data
#
# No paths.  No slugs.  No manifest.  Every key is the same 64-bit
# digest the codegen baked into the binary as an `AssetHandle`, so
# loading is one HashMap lookup with zero string round-trips and
# zero opportunity for `strings(1)` to recover an asset name.
_PACK_MAGIC   = 0x504B4C42
_PACK_VERSION = 1


def _collect_pack_entries(assets_dir: Path) -> list[tuple[str, bytes]]:
    """Walk `assets/` and produce the (path, bytes) tuples that should
    end up in the pack.  Paths are normalised to forward slashes and
    are post-transcoding (atlas/cursor `.json` becomes `.bin`).
    """
    entries: list[tuple[str, bytes]] = []
    for root, _dirs, files in os.walk(assets_dir):
        for f in files:
            fp = Path(root) / f
            rel = fp.relative_to(assets_dir)
            rel_str = str(rel).replace("\\", "/")
            if rel_str in _DEV_ONLY_FILES:
                continue
            is_atlas_or_cursor_json = (
                (rel_str.startswith("atlases/") or rel_str.startswith("cursor/"))
                and rel_str.endswith(".json")
            )
            if is_atlas_or_cursor_json:
                bin_path = rel_str[:-5] + ".bin"
                json_data = json.loads(fp.read_text(encoding="utf-8"))
                stripped  = _strip_atlas_identifiers(json_data)
                entries.append((bin_path, msgpack.packb(stripped, use_bin_type=True)))
                print(f"  Transcoding: {rel_str} -> {bin_path} (stripped)")
            else:
                entries.append((rel_str, fp.read_bytes()))
    return entries


def _load_manifest_path_to_hash(slim_path: Path) -> dict[str, int]:
    """Return a `source_path -> AssetHandle.hash` map built from the
    slim manifest.  The manifest stores `hash_hex -> source_path`, but
    for packing we need the reverse lookup so we can stamp every
    typed asset with the exact hash the codegen baked into the binary.

    The paths in the manifest still reference editable `.json` files
    for atlases / cursor compositions; the pack-time transcode step
    rewrites them to `.bin`, so we mirror that mapping here.
    """
    if not slim_path.is_file():
        raise FileNotFoundError(
            f"{slim_path} is missing — run "
            f"`python open_bulanci/asset_pipeline/build_typed_handles.py` first.",
        )
    slim: dict[str, str] = json.loads(slim_path.read_text(encoding="utf-8"))
    by_path: dict[str, int] = {}
    for hash_hex, src in slim.items():
        h = int(hash_hex, 16)
        if (src.startswith("atlases/") or src.startswith("cursor/")) and src.endswith(".json"):
            by_path[src[:-5] + ".bin"] = h
        else:
            by_path[src] = h
    return by_path


def pack_assets() -> Path:
    root_dir   = Path(__file__).parent.parent.parent
    assets_dir = root_dir / "open_bulanci" / "assets"
    output_pack = root_dir / "open_bulanci" / "assets.pack"
    slim_path   = assets_dir / _SLIM_MANIFEST

    print(f"Packing assets in {assets_dir} into {output_pack}...")

    entries        = _collect_pack_entries(assets_dir)
    manifest_hash  = _load_manifest_path_to_hash(slim_path)

    # Cross-check coverage: every typed handle that names a *file*
    # must resolve to something we actually packed.  AudioBank "parent"
    # handles use a synthetic `audio/<slug>/` directory path — there's
    # no file there, the runtime only ever loads the per-sample
    # siblings, so skip those when verifying.  Catching real mismatches
    # here turns a silent runtime placeholder fallback into a noisy
    # build failure.
    packed_paths = {p for p, _ in entries}
    missing = sorted(
        p for p in (set(manifest_hash) - packed_paths)
        if not p.endswith("/")
    )
    if missing:
        raise RuntimeError(
            "Typed handles reference paths that are NOT in the pack:\n  "
            + "\n  ".join(missing)
        )

    # Stamp each entry with its hash.  Typed assets inherit the hash
    # the codegen computed from `<folder>/<slug>`; everything else
    # (poems, transpiled Lua levels, etc.) gets a synthetic blake2b
    # of its source path so the runtime's `vfs.read(&str)` shim can
    # find it without baking a path table.
    hashed: list[tuple[int, bytes, str]] = []
    for path, data in entries:
        h = manifest_hash.get(path)
        if h is None:
            h = _hash_raw_path(path)
        hashed.append((h, data, path))

    # Detect collisions — blake2b-64 has ~2^-32 collision probability
    # per pair, but we'd rather discover one at build time than chase
    # an asset that resolves to the wrong bytes at runtime.
    seen: dict[int, str] = {}
    for h, _, path in hashed:
        if h in seen and seen[h] != path:
            raise RuntimeError(
                f"Hash collision: '{path}' and '{seen[h]}' both map to "
                f"0x{h:016x}. Pick a different path for one of them."
            )
        seen[h] = path

    hashed.sort(key=lambda kv: kv[0])

    out = bytearray()
    out.extend(struct.pack("<III", _PACK_MAGIC, _PACK_VERSION, len(hashed)))
    for h, data, _path in hashed:
        out.extend(struct.pack("<QI", h, len(data)))
        out.extend(data)

    output_pack.write_bytes(out)
    print(
        f"Packed {len(hashed)} files ({len(out)} bytes, hash-keyed, "
        f"magic=0x{_PACK_MAGIC:08x} v{_PACK_VERSION})."
    )
    return output_pack


if __name__ == "__main__":
    pack_assets()
