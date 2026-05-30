"""Registry-driven asset extraction.

This script is the single entry point that turns *named* master-pack
resources into runtime artefacts under `open_bulanci/assets/`.  It
replaces the older per-feature scripts (`build_menu_assets.py`,
`build_cursor_atlas.py`); both their hand-maintained Python dicts have
been folded into `ghidra_analysis/asset_catalog/registry.json`.

How it works
------------
1. Load `registry.json` (the canonical names) and `catalog.json` (the
   className for each resource id).
2. For every `assets["0x..."]` entry with `ship = true`:
   * look up the catalog className,
   * dispatch to the matching extractor in `lib/extractors.py`.
3. Bake the cursor composition file (`assets/cursor/cursor.json`) from
   the named `cursor_idle_*` tracks.

Run `python open_bulanci/asset_pipeline/build_assets.py` (or, through
the master orchestrator, `rebuild_assets.py`).
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from lib import extractors as ex                            # noqa: E402
from lib.catalog import load_catalog, index_by_id            # noqa: E402
from lib.registry import load_named_assets, NamedAsset      # noqa: E402


# Slugs the cursor composition consumes, in playback order.  Track 0
# is the first played when the cursor activates (CGunMouse_ctor at
# 0x00426060 hands DAT_004af900 = { 0x1010b, 0x1010a, 0x10109 } to its
# track manager).
CURSOR_TRACK_ORDER = ("cursor_idle_a", "cursor_idle_b", "cursor_idle_c")


def _bake_cursor_composition(named_by_slug: dict[str, NamedAsset],
                             assets_dir: Path) -> None:
    """Write `cursor/cursor.json` listing the idle-twitch tracks."""
    tracks = []
    for slug in CURSOR_TRACK_ORDER:
        if slug not in named_by_slug:
            raise RuntimeError(
                f"cursor composition needs '{slug}' in registry.json "
                f"with ship=true"
            )
        tracks.append({"atlas": slug})

    cursor_dir = assets_dir / "cursor"
    cursor_dir.mkdir(parents=True, exist_ok=True)
    # Clean any stale per-build cruft (older script wrote track_*.png).
    for item in cursor_dir.iterdir():
        if item.name not in ("cursor.json", "cursor.bin"):
            item.unlink()

    composition = {
        "schemaVersion": 1,
        "idleTracks":    tracks,
    }
    (cursor_dir / "cursor.json").write_text(
        json.dumps(composition, indent=2) + "\n", encoding="utf-8"
    )


def main() -> int:
    repo_root  = Path(__file__).resolve().parents[2]
    unpacked   = repo_root / "unpacked" / "overlay"
    assets_dir = repo_root / "open_bulanci" / "assets"

    by_id = index_by_id(load_catalog())
    named = load_named_assets()
    shippable = [a for a in named if a.ship]

    if not shippable:
        print("[build_assets] No `ship = true` entries in registry.json — nothing to do.")
        return 0

    # Re-export the cursor tracks with motion-path verification on
    # (the runtime depends on the closed-loop invariant; see the
    # cursor builder commentary).
    cursor_slugs = set(CURSOR_TRACK_ORDER)

    written: list[Path] = []
    for asset in shippable:
        entry = by_id.get(asset.resource_id)
        if entry is None:
            raise RuntimeError(
                f"registry entry {asset.id_hex} ({asset.slug!r}) is "
                f"missing from catalog.json — re-run "
                f"build_asset_catalog.py?"
            )
        class_name = entry["className"]
        kwargs = None
        if class_name == "BitmapSprite" and asset.slug in cursor_slugs:
            kwargs = {"include_motion": True, "require_closed_loop": True}
        elif class_name == "BitmapSprite":
            # Menu buttons (and any future non-cursor sprites) ship with
            # motion data too — the runtime is free to ignore it.
            kwargs = {"include_motion": True, "require_closed_loop": False}

        paths = ex.extract_named(
            asset, class_name, unpacked, assets_dir,
            bitmap_sprite_kwargs=kwargs,
        )
        for p in paths:
            rel = p.relative_to(repo_root)
            print(f"[build_assets] {class_name:14s}  {asset.slug:24s} -> {rel}")
            written.append(p)

    # Cursor composition (depends on the cursor_idle_* atlases just
    # produced).  Skipped silently when none of those slugs ship.
    named_by_slug = {a.slug: a for a in shippable}
    if cursor_slugs <= named_by_slug.keys():
        _bake_cursor_composition(named_by_slug, assets_dir)
        print(f"[build_assets] composition    cursor.json              "
              f"-> {(assets_dir / 'cursor' / 'cursor.json').relative_to(repo_root)}")

    print(f"[build_assets] Done. Wrote {len(written)} files for "
          f"{len(shippable)} named resources.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
