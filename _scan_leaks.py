"""Scan the release binary for asset-name / path literals that would leak
into `.rodata`.  Reads the slim manifest as the ground truth of every
known slug + path, then greps the exe bytes for either form.

Exits 1 if anything matches — the binary should be opaque w.r.t. asset
names.  The packed assets blob lives in `open_bulanci/assets.pack`, NOT
inside the exe, so even compressed slug strings in the pack don't count.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent
EXE = REPO / "open_bulanci" / "target" / "release" / "bulanci_client.exe"
MANIFEST = REPO / "open_bulanci" / "assets" / "manifest.slim.json"


def main() -> int:
    if not EXE.is_file():
        print(f"ERROR: missing {EXE}", file=sys.stderr)
        return 2
    if not MANIFEST.is_file():
        print(f"ERROR: missing {MANIFEST}", file=sys.stderr)
        return 2

    blob = EXE.read_bytes()
    paths = json.loads(MANIFEST.read_text(encoding="utf-8"))

    # Generic engine-domain stems that aren't asset slugs — they're
    # the *type* of the asset (every BitmapSprite lives in an
    # `atlases/<slug>/atlas.{json,bin}` file, the cursor subsystem
    # ships its composition at `cursor/cursor.{json,bin}`).  Those
    # tokens legitimately appear in Rust source paths, anyhow!
    # contexts, and module names; flagging them here would force us
    # to rename engine concepts to chase a false positive.
    GENERIC_STEMS = frozenset({"atlas", "cursor", "manifest", "audio", "images"})

    candidates: set[str] = set()
    for path_str in paths.values():
        # Both the full path and the leaf slug should be absent.
        # E.g. `images/bg_menu.jpg` -> add 'images/bg_menu.jpg' and
        #      'bg_menu' (without extension).
        candidates.add(path_str)
        leaf = Path(path_str).name
        candidates.add(leaf)
        stem = Path(path_str).stem
        if stem and stem != leaf and stem not in GENERIC_STEMS:
            candidates.add(stem)
        # Folder prefix on its own is benign but flag if a full
        # `audio/X.wav` slips in.
    # Also pin: the slug-only forms used to live in `.rodata`.
    extra_slugs = [
        "sfx_hover", "sfx_start", "sfx_history", "sfx_quit",
        "sfx_force_exit", "sfx_alt_exit", "sfx_radio_click",
        "day_ambient", "night_ambient",
        "bg_menu", "bg_hero", "bg_start", "bg_history", "bg_quit",
        "intro_splash",
        "btn_dialog_hover", "btn_dialog_normal", "btn_dialog_pressed",
        "radio_active", "radio_selected", "radio_selected_hover",
        "radio_unselected", "radio_unselected_hover",
        "icon_start", "icon_start_hi", "icon_history", "icon_history_hi",
        "icon_quit", "icon_quit_hi",
        "cursor_idle_a", "cursor_idle_b", "cursor_idle_c",
        "cursor_reticle", "cursor_composition",
        "menu_button_off", "menu_button_on",
        "font_small", "font_medium", "font_large",
        "menu_sfx_bank",
    ]
    candidates.update(extra_slugs)

    leaks: list[tuple[str, int]] = []
    for cand in sorted(candidates):
        needle = cand.encode("utf-8")
        # Cheap path: substring search inside the byte blob.
        offset = blob.find(needle)
        if offset >= 0:
            leaks.append((cand, offset))

    if not leaks:
        print(f"OK: scanned {len(candidates)} candidate strings, "
              f"none found in {EXE.relative_to(REPO)} "
              f"({len(blob):,} bytes)")
        return 0

    print(f"LEAK: {len(leaks)} asset literal(s) found in "
          f"{EXE.relative_to(REPO)}:")
    for cand, off in leaks:
        # Show 32 bytes of context.
        ctx = blob[max(0, off-8):off+len(cand)+16]
        printable = "".join(chr(b) if 32 <= b < 127 else "." for b in ctx)
        print(f"  {off:>8x}  {cand!r:40s}  ...{printable!r}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
