"""Generate strongly-typed Rust asset accessors from registry.json.

Project-level layout: assets in registry.json are tagged with a
human-meaningful `folder = "menu/widgets/radio"` path that's purely
for organisation in the catalog gallery + this generator.

Runtime layout: the shipped binary references each asset only by a
stable 64-bit hash of its `folder/slug` — the actual path string is
*not* embedded.  That keeps the original asset names and our project
structure out of the binary while still giving Rust callers a typed,
auto-completing module tree like::

    use crate::generated::assets;

    let bank: AssetHandle<AudioBank> = assets::menu::sfx::MENU_SFX_BANK;
    let bytes = asset_manager.load(bank)?;          // returns AudioBank

This script writes two artefacts:

    * open_bulanci/client/src/generated/assets.rs
        — `pub mod` tree of `pub const NAME: AssetHandle<Class> = …;`
          definitions.  Hand-written code references these constants.

    * open_bulanci/assets/manifest.json
        — `{ "<hash_hex>": { class, folder, slug, source } }` table
          the asset manager loads at startup to resolve a handle to
          a file on disk (or, eventually, to an entry in our own
          packed VFS).

Run it whenever registry.json changes::

    python open_bulanci/asset_pipeline/build_typed_handles.py
"""
from __future__ import annotations

import argparse
import hashlib
import json
import keyword
import re
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

# Allow `python build_typed_handles.py` to import the sibling `lib/`.
sys.path.insert(0, str(Path(__file__).parent))
from lib.registry import (                                            # noqa: E402
    ManualAsset, NamedAsset, load_folders, load_manual_assets,
    load_named_assets,
)
from lib.catalog import index_by_id, load_catalog                     # noqa: E402

_REPO_ROOT = Path(__file__).resolve().parents[2]
_OUT_RUST  = _REPO_ROOT / "open_bulanci" / "client" / "src" / "generated" / "assets.rs"
# Two manifests with distinct audiences:
#   * `manifest.json` — full, fat record (class / folder / slug /
#     source path).  Used by the gallery, the typed-handle codegen
#     itself, and any "what is asset 0xHASH" debug query.  NEVER
#     ends up inside the shipped binary.
#   * `manifest.slim.json` — just `"hash_hex": "source_path"`.
#     `pack_assets.py` injects this as `__manifest.json` inside
#     `assets.pack`, so the runtime gets a hash → path table without
#     leaking slug or folder names into `.rodata`.  See
#     `client/src/asset/loader.rs` for the loader side.
_OUT_MANIFEST      = _REPO_ROOT / "open_bulanci" / "assets" / "manifest.json"
_OUT_MANIFEST_SLIM = _REPO_ROOT / "open_bulanci" / "assets" / "manifest.slim.json"
# Modder-facing path → hash reference.  This DOES list every shipped
# asset's source path next to its 64-bit handle hash — it's the public
# modding API and ships in the release bundle next to the executable.
# The binary itself stays opaque (no slugs in `.rodata`); modders who
# want to override `images/bg_menu.jpg` look up the hash here and drop
# their replacement under `mods/<hash_hex>` or `mods/images/bg_menu.jpg`
# (the watcher accepts either form).  The file lives in the repo so
# pack_assets.py can copy it alongside `bulanci_client.exe` at release
# bundling time.
_OUT_MODS_REF = _REPO_ROOT / "open_bulanci" / "assets" / "mods.reference.json"

# Maps catalog className -> Rust marker type emitted in the generated
# module.  These markers must exist in the hand-written runtime code
# (e.g. `open_bulanci/client/src/asset/types.rs`) — the generator only
# *references* them.
_CLASS_TO_RUST: dict[str, str] = {
    "BitmapJPEG":         "BitmapJpeg",
    "BitmapSpecial":      "BitmapSpecial",
    "BitmapSprite":       "BitmapSprite",
    "BitmapJpegAnim":     "BitmapJpegAnim",
    "Mp3":                "Mp3",
    "AudioBank":          "AudioBank",
    "Font":               "Font",
    "Poem":               "Poem",
    "Script":             "Script",
    "HistoryScript":      "HistoryScript",
    "DsmInner":           "DsmInner",
    # Sibling classes — see `_collect_entries` for where they're emitted.
    "BitmapSpritePng":    "BitmapSpritePng",
    "FontJson":           "FontJson",
    # Manual-only classes (no catalog backing).
    "CursorComposition":  "CursorComposition",
}

# Per-sample handles for AudioBank entries get this synthetic class.
# The runtime resolves them to `audio/<sample_slug>.wav` produced by
# the `build_assets.py` polyphase-resampling step.  Must also exist
# as a marker in `asset/types.rs`.
_AUDIO_SAMPLE_RUST = "AudioSample"

# `<folder>/<slug>` is the canonical name we hash.  Keep this stable
# — changing the hash function would break every shipping binary.
def hash_path(folder: str | None, slug: str) -> int:
    """Stable 64-bit hash of an asset's project path.  blake2b is
    fast, well-distributed, and lets us pin a `digest_size=8`."""
    key = f"{folder or '_root'}/{slug}".encode("utf-8")
    return int.from_bytes(hashlib.blake2b(key, digest_size=8).digest(), "big")


# --- Rust identifier sanitisation -----------------------------------------
_RUST_RAW_OK = re.compile(r"^[a-zA-Z_][a-zA-Z0-9_]*$")

def _sanitise_ident(name: str, *, kind: str) -> str:
    """Map a folder/slug fragment to a valid Rust identifier.

    * Lowercases (modules) / uppercases (constants).
    * Replaces every disallowed char with `_`.
    * Prefixes leading digits with `_`.
    * Appends `_` to Rust keywords so we never need raw identifiers."""
    cleaned = re.sub(r"[^a-zA-Z0-9_]", "_", name)
    if not cleaned or cleaned[0].isdigit():
        cleaned = f"_{cleaned}"
    if kind == "module":
        cleaned = cleaned.lower()
    elif kind == "const":
        cleaned = cleaned.upper()
    # keyword.iskeyword covers only Python kw; carry a Rust subset too.
    if cleaned.lower() in _RUST_KEYWORDS or keyword.iskeyword(cleaned.lower()):
        cleaned += "_"
    assert _RUST_RAW_OK.match(cleaned), f"sanitisation failed for {name!r} -> {cleaned!r}"
    return cleaned

# 2024-edition Rust keywords + a few reserved ones.  Worth keeping
# explicit so the codegen is deterministic across Python versions.
_RUST_KEYWORDS = {
    "as", "break", "const", "continue", "crate", "else", "enum", "extern",
    "false", "fn", "for", "if", "impl", "in", "let", "loop", "match", "mod",
    "move", "mut", "pub", "ref", "return", "self", "static", "struct",
    "super", "trait", "true", "type", "unsafe", "use", "where", "while",
    "async", "await", "dyn", "abstract", "become", "box", "do", "final",
    "macro", "override", "priv", "typeof", "unsized", "virtual", "yield",
    "try", "union",
}


# --- main code generation -------------------------------------------------

@dataclass
class _HandleEntry:
    """Everything the codegen needs about a single ship-eligible asset
    (or a single AudioBank sample / atlas-sibling / font-sibling)."""
    folder:     str | None    # project folder path
    slug:       str           # leaf identifier (with sibling suffix for siblings)
    class_name: str           # original catalog className (or "AudioSample" / "BitmapSpritePng" / "FontJson" / manual class)
    rust_class: str           # mapped Rust marker type
    hash_id:    int
    notes:      str | None = None
    # Original parent — only set for AudioBank samples (helps with
    # error messages / manifest hints).
    parent_slug: str | None = None
    # When set, overrides `_source_path_for` (used by manual_assets +
    # sibling handles whose source isn't derivable from class alone).
    source_override: str | None = None


def _collect_entries(
    assets: list[NamedAsset],
    manual: list[ManualAsset],
) -> list[_HandleEntry]:
    cat = load_catalog()
    by_id = index_by_id(cat)
    out: list[_HandleEntry] = []
    for a in assets:
        if not a.ship:
            continue
        row = by_id.get(a.resource_id)
        if row is None:
            print(f"  warning: registry slug {a.slug!r} has no catalog entry, skipping")
            continue
        cls = row["className"]
        rust_cls = _CLASS_TO_RUST.get(cls)
        if rust_cls is None:
            print(f"  warning: no Rust marker for class {cls!r} ({a.slug}); skipping")
            continue
        out.append(_HandleEntry(
            folder=a.folder,
            slug=a.slug,
            class_name=cls,
            rust_class=rust_cls,
            hash_id=hash_path(a.folder, a.slug),
            notes=a.notes,
        ))
        # AudioBank samples become their own typed handles so the
        # game code can reference an individual SFX without going
        # through the bank.  Sample path = `audio/<sample_slug>.wav`
        # (matches `build_assets.py`'s polyphase output).
        if cls == "AudioBank":
            for _idx, sample_slug in sorted(a.samples.items()):
                out.append(_HandleEntry(
                    folder=a.folder,
                    slug=sample_slug,
                    class_name="AudioSample",
                    rust_class=_AUDIO_SAMPLE_RUST,
                    hash_id=hash_path(a.folder, sample_slug),
                    notes=f"sample from {a.slug!r}",
                    parent_slug=a.slug,
                ))
        # Compound classes need a paired sibling handle so the runtime
        # can fetch every part of the asset by handle alone — no
        # filesystem path-munging at the call site.
        if cls == "BitmapSprite":
            # BitmapSprite = atlas folder with `atlas.json` + `atlas.png`.
            # The base handle resolves to the JSON, the `_png` sibling
            # to the image.  The sibling slug stays uppercase-friendly
            # so the emitted Rust const reads `CURSOR_IDLE_A_PNG`.
            sibling_slug = f"{a.slug}_png"
            out.append(_HandleEntry(
                folder=a.folder,
                slug=sibling_slug,
                class_name="BitmapSpritePng",
                rust_class=_CLASS_TO_RUST["BitmapSpritePng"],
                hash_id=hash_path(a.folder, sibling_slug),
                notes=f"PNG image for atlas {a.slug!r}",
                parent_slug=a.slug,
                source_override=f"atlases/{a.slug}/atlas.png",
            ))
        elif cls == "Font":
            sibling_slug = f"{a.slug}_json"
            out.append(_HandleEntry(
                folder=a.folder,
                slug=sibling_slug,
                class_name="FontJson",
                rust_class=_CLASS_TO_RUST["FontJson"],
                hash_id=hash_path(a.folder, sibling_slug),
                notes=f"glyph metrics JSON for font {a.slug!r}",
                parent_slug=a.slug,
                source_override=f"fonts/bitmap/{a.slug}.json",
            ))

    # Manual entries.  No catalog lookup — registry author supplies
    # both class and source path.  Validated against `_CLASS_TO_RUST`
    # because the runtime needs the marker type to exist.
    for m in manual:
        rust_cls = _CLASS_TO_RUST.get(m.cls)
        if rust_cls is None:
            print(f"  warning: manual asset {m.slug!r} declares unknown class {m.cls!r}; skipping")
            continue
        out.append(_HandleEntry(
            folder=m.folder,
            slug=m.slug,
            class_name=m.cls,
            rust_class=rust_cls,
            hash_id=hash_path(m.folder, m.slug),
            notes=m.notes,
            source_override=m.source,
        ))
    return out


def _validate(entries: list[_HandleEntry]) -> None:
    """Ensure no two assets collide on hash or on folder/slug pair."""
    by_hash: dict[int, _HandleEntry] = {}
    by_path: dict[tuple[str | None, str], _HandleEntry] = {}
    for e in entries:
        key = (e.folder, e.slug)
        if key in by_path:
            other = by_path[key]
            raise ValueError(
                f"duplicate (folder, slug) pair: {key!r} used by both "
                f"{other.slug!r} and {e.slug!r}",
            )
        by_path[key] = e
        if e.hash_id in by_hash:
            other = by_hash[e.hash_id]
            raise ValueError(
                f"hash collision (vanishingly unlikely) on 0x{e.hash_id:016x}: "
                f"{other.slug!r} vs {e.slug!r}",
            )
        by_hash[e.hash_id] = e


# --- Rust module-tree emitter --------------------------------------------

class _Tree:
    """Nested dict shaped like `{ "menu": { "widgets": { "radio": [entries...] }}}`."""

    def __init__(self) -> None:
        self.children: dict[str, _Tree]   = {}
        self.entries:  list[_HandleEntry] = []

    def insert(self, e: _HandleEntry) -> None:
        node = self
        parts = (e.folder or "").split("/") if e.folder else []
        for raw_part in parts:
            part = _sanitise_ident(raw_part, kind="module")
            node = node.children.setdefault(part, _Tree())
        node.entries.append(e)


def _build_tree(entries: list[_HandleEntry]) -> _Tree:
    root = _Tree()
    for e in entries:
        root.insert(e)
    return root


def _emit_tree(tree: _Tree, depth: int, lines: list[str]) -> None:
    pad = "    " * depth
    # Sort for deterministic output.
    for e in sorted(tree.entries, key=lambda x: x.slug):
        const_name = _sanitise_ident(e.slug, kind="const")
        comment    = e.notes or e.class_name
        lines.append(f"{pad}/// {comment}")
        lines.append(
            f"{pad}pub const {const_name}: AssetHandle<{e.rust_class}> "
            f"= AssetHandle::new(0x{e.hash_id:016x}u64);",
        )
    for name, child in sorted(tree.children.items()):
        lines.append("")
        lines.append(f"{pad}pub mod {name} {{")
        lines.append(f"{pad}    use super::*;")
        _emit_tree(child, depth + 1, lines)
        lines.append(f"{pad}}}")


_PREAMBLE = """\
// !!! AUTO-GENERATED by open_bulanci/asset_pipeline/build_typed_handles.py
// !!! DO NOT EDIT.  Run the script to regenerate.
//
// Strongly-typed asset handles produced from registry.json's
// `folder` + `slug` fields.  The raw `u64` is a blake2b digest of
// "<folder>/<slug>" — neither the slug nor the folder path is
// embedded in the shipped binary.  The runtime looks the hash up in
// `open_bulanci/assets/manifest.json` to find the actual file.
//
// To rename or move an asset:
//   1. Update registry.json (slug / folder).
//   2. Re-run `python open_bulanci/asset_pipeline/build_typed_handles.py`.
//   3. Update any Rust call sites that referenced the old constant.

#![allow(dead_code, non_upper_case_globals)]
use crate::asset::handle::AssetHandle;
use crate::asset::types::*;
"""


def _emit_rust(entries: list[_HandleEntry]) -> str:
    lines: list[str] = []
    lines.append(_PREAMBLE)
    tree = _build_tree(entries)
    _emit_tree(tree, depth=0, lines=lines)
    return "\n".join(lines).rstrip() + "\n"


# --- manifest emitters ---------------------------------------------------

def _emit_manifest(entries: list[_HandleEntry]) -> str:
    """The *fat* manifest — keyed by 16-hex-digit hash, includes class
    name, folder, slug, and the resolved source path.  Used by the
    gallery + dev tooling; **never embedded in the shipped binary**.

    The runtime equivalent that *does* ship lives in
    `manifest.slim.json` (emitted next to this file by
    `_emit_manifest_slim`) and gets packed inside `assets.pack` so
    that even the path strings live inside the zstd blob instead of
    `.rodata`."""
    obj: dict[str, dict[str, object]] = {}
    for e in entries:
        obj[f"{e.hash_id:016x}"] = {
            "class":  e.class_name,
            "folder": e.folder,
            "slug":   e.slug,
            "source": _source_path_for(e),
        }
    return json.dumps(obj, indent=2, sort_keys=True) + "\n"


def _emit_manifest_slim(entries: list[_HandleEntry]) -> str:
    """Compact manifest the runtime actually consumes.

    Format: `{ "<hash_hex>": "<source_path>" }`.  No class name (the
    typed handle's marker carries that at compile time), no slug, no
    folder — those are dev-only metadata.

    Sorted by hash so `git diff` flags only real changes."""
    obj: dict[str, str] = {
        f"{e.hash_id:016x}": _source_path_for(e) for e in entries
    }
    return json.dumps(obj, indent=2, sort_keys=True) + "\n"


def _emit_mods_reference(entries: list[_HandleEntry]) -> str:
    """Path → hash reference shipped next to the release binary.

    The runtime never reads this file — it exists for modders, who
    look up `images/bg_menu.jpg` to learn that the corresponding
    handle hash is `b3ad…` and drop their replacement either at
    `mods/b3adXXXXXXXXXXXX.jpg` (hash-named, language-agnostic) or
    at `mods/images/bg_menu.jpg` (path-mirroring, friendlier).  The
    file watcher accepts both forms.

    Pack-time transcoding (`.json` → `.bin` for atlas / cursor
    composition) is rewritten here too so the mod path matches the
    name the runtime actually expects to override.
    """
    pack_paths: dict[str, str] = {}
    for e in entries:
        src = _source_path_for(e)
        if (src.startswith("atlases/") or src.startswith("cursor/")) and src.endswith(".json"):
            pack_paths[src[:-5] + ".bin"] = f"{e.hash_id:016x}"
        else:
            pack_paths[src] = f"{e.hash_id:016x}"
    return json.dumps({
        "_comment": (
            "Modder reference. Each key is a path inside the shipped pack; "
            "the value is the 64-bit handle hash (hex) the runtime uses to "
            "key it. Drop replacement files into mods/<path> next to the "
            "executable, or into mods/<hash>.<ext> if you prefer hash-named "
            "mods. The runtime resolves either form via this same hash. "
            "Atlas / cursor composition originals are JSON in the repo but "
            "ship as MsgPack (.bin) inside the pack — the path here is the "
            ".bin form."
        ),
        "_schema_version": 1,
        "paths": pack_paths,
    }, indent=2, sort_keys=True) + "\n"


def _source_path_for(e: _HandleEntry) -> str:
    """Mirror `build_assets.py`'s destination layout.

    Manual / sibling entries set `source_override` so we don't need to
    encode their layout in this table — the registry author decides."""
    if e.source_override is not None:
        return e.source_override
    cls  = e.class_name
    slug = e.slug
    if cls == "BitmapJPEG":         return f"images/{slug}.jpg"
    if cls == "BitmapSpecial":      return f"images/{slug}.png"
    if cls == "BitmapSprite":       return f"atlases/{slug}/atlas.json"
    if cls == "BitmapJpegAnim":     return f"anims/{slug}/"
    if cls == "Mp3":                return f"audio/{slug}.mp3"
    if cls == "AudioBank":          return f"audio/{slug}/"
    if cls == "AudioSample":        return f"audio/{slug}.wav"
    # Bitmap fonts ship under `fonts/bitmap/` because the unpacker /
    # `build_assets.py` keeps them in their own folder (away from any
    # future TTF / SDF font we might add at `fonts/<name>.ttf`).
    if cls == "Font":               return f"fonts/bitmap/{slug}.png"
    return f"misc/{slug}"


# --- entry point ---------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rust-out",          type=Path, default=_OUT_RUST)
    parser.add_argument("--manifest-out",      type=Path, default=_OUT_MANIFEST)
    parser.add_argument("--manifest-slim-out", type=Path, default=_OUT_MANIFEST_SLIM)
    parser.add_argument("--mods-ref-out",      type=Path, default=_OUT_MODS_REF)
    parser.add_argument("--check",  action="store_true",
                        help="just validate registry.json; don't write files.")
    args = parser.parse_args()

    assets  = load_named_assets()
    manual  = load_manual_assets()
    folders = load_folders()          # currently consulted only for notes
    entries = _collect_entries(assets, manual)
    _validate(entries)

    print(f"  {len(entries)} ship-eligible handles across "
          f"{len({e.folder for e in entries})} folders "
          f"({len(folders)} have folder-level notes)")

    if args.check:
        return 0

    args.rust_out.parent.mkdir(parents=True, exist_ok=True)
    args.rust_out.write_text(_emit_rust(entries), encoding="utf-8")
    print(f"  -> {args.rust_out.relative_to(_REPO_ROOT)}")

    args.manifest_out.parent.mkdir(parents=True, exist_ok=True)
    args.manifest_out.write_text(_emit_manifest(entries), encoding="utf-8")
    print(f"  -> {args.manifest_out.relative_to(_REPO_ROOT)}")

    args.manifest_slim_out.parent.mkdir(parents=True, exist_ok=True)
    args.manifest_slim_out.write_text(_emit_manifest_slim(entries), encoding="utf-8")
    print(f"  -> {args.manifest_slim_out.relative_to(_REPO_ROOT)}")

    args.mods_ref_out.parent.mkdir(parents=True, exist_ok=True)
    args.mods_ref_out.write_text(_emit_mods_reference(entries), encoding="utf-8")
    print(f"  -> {args.mods_ref_out.relative_to(_REPO_ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
