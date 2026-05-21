"""Bake the three idle-twitch motion paths into the generic production atlas format.

CGunMouse_ctor @ 0x00426060 registers three BitmapSprite IDs with
its track manager: DAT_004af900 = { 0x1010b, 0x1010a, 0x10109 }
i.e. 65803, 65802, 65801 (track 0 = 65803, first played at activate).

The unpacker (tools/bulanci_unpack/bulanci_unpack.py) already lifts
each one into:
    unpacked/overlay/res_0000065801_52_BitmapSprite.atlas.png   (frame strip)
    unpacked/overlay/res_0000065801_52_BitmapSprite.atlas.json  (full sidecar)

This script structures those raw unpacked artifacts into the new generic
production format:
    open_bulanci/assets/atlases/<name>/atlas.json
    open_bulanci/assets/atlases/<name>/atlas.png
    open_bulanci/assets/cursor/cursor.json

Where:
    - index 0 (65803) -> cursor_idle_a
    - index 1 (65802) -> cursor_idle_b
    - index 2 (65801) -> cursor_idle_c
"""

from __future__ import annotations

import json
import shutil
from pathlib import Path

TRACKS = [
    {"id": 65803, "name": "cursor_idle_a"},
    {"id": 65802, "name": "cursor_idle_b"},
    {"id": 65801, "name": "cursor_idle_c"},
]


def _slim_track(sprite_id: int, name: str, atlas_json: dict) -> dict:
    """Transform raw unpack JSON into the new generic production Atlas schema."""
    frame_count = atlas_json["frameCount"]
    motion = atlas_json["motionPath"]["perFrameAbsolutePosition"]
    deltas = [[entry["dx"], entry["dy"]] for entry in motion[1 : frame_count + 1]]
    assert len(deltas) == frame_count, (
        f"{sprite_id}: motionPath length {len(deltas)} != frameCount {frame_count}"
    )

    # Closed-loop invariant check: cumulative sum must land at (0, 0).
    sx = sum(d[0] for d in deltas)
    sy = sum(d[1] for d in deltas)
    assert (sx, sy) == (0, 0), (
        f"{sprite_id}: deltas do not close to origin (cumulative = ({sx}, {sy}))"
    )

    frames = []
    for dx, dy in deltas:
        # Every frame is listed sequentially, carrying its motion inside `meta`.
        frames.append({
            "meta": {
                "motion": [dx, dy]
            }
        })

    return {
        "schemaVersion": 1,
        "name": name,
        "image": "atlas.png",
        "frame": {
            "width": atlas_json["frameWidth"],
            "height": atlas_json["frameHeight"],
        },
        "origin": {
            "x": atlas_json["frameWidth"] // 2,
            "y": atlas_json["frameHeight"] // 2,
        },
        "frames": frames,
        "meta": {
            "engineSpriteId": sprite_id,
        },
    }


def main() -> None:
    repo_root = Path(__file__).parent.parent
    unpacked = repo_root / "unpacked" / "overlay"
    assets_dir = repo_root / "open_bulanci" / "assets"

    # 1. Clean up old build folder to prevent stale track_*.png / manifest.json files
    old_cursor_dir = assets_dir / "cursor"
    if old_cursor_dir.is_dir():
        for item in old_cursor_dir.iterdir():
            if item.name not in ("cursor.json", "cursor.bin"):
                item.unlink()

    # Create directories
    atlases_dir = assets_dir / "atlases"
    atlases_dir.mkdir(parents=True, exist_ok=True)
    old_cursor_dir.mkdir(parents=True, exist_ok=True)

    composition_tracks = []

    for item in TRACKS:
        sprite_id = item["id"]
        atlas_name = item["name"]

        prefix = f"res_{sprite_id:010d}_52_BitmapSprite"
        src_png = unpacked / f"{prefix}.atlas.png"
        src_json = unpacked / f"{prefix}.atlas.json"
        if not src_png.is_file() or not src_json.is_file():
            raise FileNotFoundError(
                f"missing unpacked artefacts for sprite {sprite_id}; "
                f"re-run tools/bulanci_unpack first"
            )

        # Create atlas folder
        atlas_dir = atlases_dir / atlas_name
        atlas_dir.mkdir(parents=True, exist_ok=True)

        # Copy png
        dst_png = atlas_dir / "atlas.png"
        shutil.copyfile(src_png, dst_png)

        # Load and transform json
        raw_json = json.loads(src_json.read_text(encoding="utf-8"))
        atlas_out = _slim_track(sprite_id, atlas_name, raw_json)

        # Write atlas.json
        dst_json = atlas_dir / "atlas.json"
        dst_json.write_text(
            json.dumps(atlas_out, indent=2) + "\n", encoding="utf-8"
        )

        composition_tracks.append({"atlas": atlas_name})

    # Write cursor.json composition file
    composition = {
        "schemaVersion": 1,
        "idleTracks": composition_tracks,
    }
    (old_cursor_dir / "cursor.json").write_text(
        json.dumps(composition, indent=2) + "\n", encoding="utf-8"
    )

    print(f"Bake complete. Wrote {len(TRACKS)} atlases to {atlases_dir} and cursor to {old_cursor_dir}.")


if __name__ == "__main__":
    main()
