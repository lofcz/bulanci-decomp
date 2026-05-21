"""Cross-check the closed-loop motion-path tracks discovered in
trace.jsonl against the unpacker's emitted `motionPath` block for
sprites 65801..65803. If we got both the trace AND the unpacker
right, each runtime track must be byte-identical to exactly one
sprite resource."""

import json
from pathlib import Path

TRACE_TRACKS = {
    "A_22": [(1, 2), (-1, 1), (-2, 1), (-1, 0), (-4, -1), (-3, -1),
             (3, -1), (1, 0), (1, 1), (3, 3), (5, 2), (3, -3),
             (2, -4), (-2, -2), (-3, -3), (-5, -2), (-2, -1),
             (-2, 0), (-1, 3), (1, 1), (2, 2), (4, 2)],
    "B_9":  [(1, 2), (3, 1), (3, 0), (2, -3), (-1, -4),
             (-3, -1), (-4, 0), (-2, 2), (1, 3)],
    "C_24": [(1, 1), (2, 3), (3, 3), (3, 2), (2, 2), (-2, 0),
             (-3, -1), (-8, -3), (-7, -4), (-6, -2), (-2, 0),
             (-1, 1), (0, 1), (3, 2), (6, 0), (3, 0), (1, -3),
             (0, -6), (-1, -6), (0, -3), (3, 2), (2, 4), (1, 4), (0, 3)],
}


def main():
    for sid in (65801, 65802, 65803):
        path = Path(f"unpacked/overlay/res_{sid:010d}_52_BitmapSprite.atlas.json")
        with path.open("r", encoding="utf-8") as f:
            data = json.load(f)
        mp = data["motionPath"]["perFrameAbsolutePosition"]
        deltas = [(e["dx"], e["dy"]) for e in mp if e["index"] > 0]
        bbox = data["motionPath"]["boundingBox"]
        print(f"sprite {sid}: frameCount={data['frameCount']}, "
              f"motion deltas={len(deltas)}, bbox={bbox}")
        for label, td in TRACE_TRACKS.items():
            if td == deltas:
                print(f"   ✓ matches trace track {label}")


if __name__ == "__main__":
    main()
