"""Generic, class-driven asset extractors.

Each extractor reads a single asset from `unpacked/overlay/` (lifted by
`tools/bulanci_unpack`) and writes the runtime-ready artefact into
`open_bulanci/assets/<bucket>/`.

The extractors are *atomic* — they handle one resource at a time and
know nothing about which set of resources the caller cares about.
`build_assets.py` is responsible for iterating the registry and
choosing the right extractor per class.

Conventions
-----------
* Output paths are derived purely from `(className, slug)`:
      BitmapJPEG     -> images/<slug>.jpg
      BitmapSpecial  -> images/<slug>.png
      BitmapSprite   -> atlases/<slug>/{atlas.png, atlas.json}
      Mp3            -> audio/<slug>.mp3
      AudioBank      -> audio/<sample-slug>.wav  (one per indexed sample)

* SFX banks are 22 050 Hz on disk; the runtime decodes everything at
  48 000 Hz to keep rodio's `SampleRateConverter` in the `from == to`
  pass-through path (see `resample_sfx.py` for why).
"""
from __future__ import annotations

import json
import shutil
import sys
from pathlib import Path

# Local import — resample_sfx ships next to this lib package.
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from resample_sfx import read_wav_mono16, resample_int16, write_wav_mono16  # noqa: E402

RUNTIME_SAMPLE_RATE = 48000

CLASS_ID = {
    "BitmapJPEG":    21,
    "BitmapSpecial": 28,
    "BitmapSprite":  52,
    "Mp3":           48,
    "AudioBank":     43,
    "Font":          54,
}


def _stem(rid: int, class_id: int, class_name: str) -> str:
    """Canonical unpacker filename stem (no extension)."""
    return f"res_{rid:010d}_{class_id}_{class_name}"


# ---------------------------------------------------------------------------
# Class-specific extractors
# ---------------------------------------------------------------------------


def extract_bitmap_jpeg(rid: int, slug: str,
                        unpacked: Path, dst_images: Path) -> Path:
    src = unpacked / f"{_stem(rid, CLASS_ID['BitmapJPEG'], 'BitmapJPEG')}.jpg"
    if not src.is_file():
        raise FileNotFoundError(f"missing {src}; run tools/bulanci_unpack first")
    dst_images.mkdir(parents=True, exist_ok=True)
    dst = dst_images / f"{slug}.jpg"
    shutil.copyfile(src, dst)
    return dst


def extract_bitmap_special(rid: int, slug: str,
                           unpacked: Path, dst_images: Path) -> Path:
    src = unpacked / f"{_stem(rid, CLASS_ID['BitmapSpecial'], 'BitmapSpecial')}.png"
    if not src.is_file():
        raise FileNotFoundError(f"missing {src}; run tools/bulanci_unpack first")
    dst_images.mkdir(parents=True, exist_ok=True)
    dst = dst_images / f"{slug}.png"
    shutil.copyfile(src, dst)
    return dst


def extract_font(rid: int, slug: str,
                 unpacked: Path, dst_fonts: Path) -> Path:
    """Copy a CDSFont (class id 54) PNG + glyph-metrics JSON pair.

    Source artefacts are produced by `tools/bulanci_unpack` as
    `res_<rid>_54_Font.{png, font.json}`.  We rename to
    `<slug>.{png, json}` and drop them under
    `assets/fonts/bitmap/` so the runtime's
    `BitmapFontProvider::from_handles` can load both halves through
    typed handles (the `.png` via the `Font` marker, the `.json`
    via the auto-emitted `FontJson` sibling).
    """
    stem = _stem(rid, CLASS_ID["Font"], "Font")
    src_png  = unpacked / f"{stem}.png"
    src_json = unpacked / f"{stem}.font.json"
    if not src_png.is_file() or not src_json.is_file():
        raise FileNotFoundError(
            f"missing unpacked artifacts for font {rid:#x}; "
            f"run tools/bulanci_unpack first"
        )
    dst_fonts.mkdir(parents=True, exist_ok=True)
    dst_png  = dst_fonts / f"{slug}.png"
    dst_json = dst_fonts / f"{slug}.json"
    shutil.copyfile(src_png,  dst_png)
    shutil.copyfile(src_json, dst_json)
    return dst_png


def extract_mp3(rid: int, slug: str,
                unpacked: Path, dst_audio: Path) -> Path:
    src = unpacked / f"{_stem(rid, CLASS_ID['Mp3'], 'Mp3')}.mp3"
    if not src.is_file():
        raise FileNotFoundError(f"missing {src}; run tools/bulanci_unpack first")
    dst_audio.mkdir(parents=True, exist_ok=True)
    dst = dst_audio / f"{slug}.mp3"
    shutil.copyfile(src, dst)
    return dst


def extract_bitmap_sprite_atlas(rid: int, slug: str,
                                unpacked: Path, dst_atlases: Path,
                                *, include_motion: bool = True,
                                require_closed_loop: bool = False) -> Path:
    """Bake a BitmapSprite into the production atlas layout.

    Writes `<dst_atlases>/<slug>/atlas.png` and `atlas.json`.  Per-frame
    motion vectors from the unpacker's `motionPath` are embedded under
    `frames[i].meta.motion` whenever `include_motion=True`.

    `require_closed_loop=True` asserts that the cumulative motion sums
    to (0, 0); used by the cursor builder to catch corrupted unpacks.
    """
    stem = _stem(rid, CLASS_ID["BitmapSprite"], "BitmapSprite")
    src_png  = unpacked / f"{stem}.atlas.png"
    src_json = unpacked / f"{stem}.atlas.json"
    if not src_png.is_file() or not src_json.is_file():
        raise FileNotFoundError(
            f"missing unpacked artifacts for sprite {rid:#x}; "
            f"run tools/bulanci_unpack first"
        )

    raw = json.loads(src_json.read_text(encoding="utf-8"))
    frame_count = raw["frameCount"]
    motion = (raw.get("motionPath") or {}).get("perFrameAbsolutePosition") or []

    # Pre-collect deltas so we can decide whether to emit motion at all
    # (skip-when-all-zero keeps purely-static atlases like the menu
    # button tracks free of decorative noise).
    deltas: list[tuple[int, int]] = []
    if include_motion:
        # The unpacker stores motion starting at index 1 (index 0 is the
        # static spawn position); take the next `frame_count` entries.
        for i in range(frame_count):
            if 1 + i < len(motion):
                mv = motion[1 + i]
                deltas.append((int(mv["dx"]), int(mv["dy"])))
            else:
                deltas.append((0, 0))
    emit_motion = include_motion and any(d != (0, 0) for d in deltas)

    if require_closed_loop and emit_motion:
        sx = sum(d[0] for d in deltas)
        sy = sum(d[1] for d in deltas)
        assert (sx, sy) == (0, 0), (
            f"sprite {rid:#x}: deltas do not close to origin "
            f"(cumulative = ({sx}, {sy}))"
        )

    frames: list[dict] = []
    for i in range(frame_count):
        if emit_motion:
            dx, dy = deltas[i]
            frames.append({"meta": {"motion": [dx, dy]}})
        else:
            frames.append({})

    atlas_doc = {
        "schemaVersion": 1,
        "name":  slug,
        "image": "atlas.png",
        "frame": {
            "width":  raw["frameWidth"],
            "height": raw["frameHeight"],
        },
        "origin": {
            "x": raw["frameWidth"]  // 2,
            "y": raw["frameHeight"] // 2,
        },
        "frames": frames,
        "meta": {"engineSpriteId": rid},
    }

    atlas_dir = dst_atlases / slug
    atlas_dir.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(src_png, atlas_dir / "atlas.png")
    (atlas_dir / "atlas.json").write_text(
        json.dumps(atlas_doc, indent=2) + "\n", encoding="utf-8"
    )
    return atlas_dir


def extract_bank_sample(bank_rid: int, sample_idx: int, slug: str,
                        unpacked: Path, dst_audio: Path,
                        *, target_rate: int = RUNTIME_SAMPLE_RATE) -> Path:
    """Resample one AudioBank slice into a runtime-ready WAV.

    Source path is `unpacked/extracted_all_samples/bank_<rid>/sample_NN.wav`
    (produced by `scripts/extract_all_banks.py`).  If `src_rate` already
    matches `target_rate` the file is copied verbatim, otherwise it is
    upsampled via the polyphase filter in `resample_sfx.py`.
    """
    bank_dir = unpacked / "extracted_all_samples" / f"bank_{bank_rid}"
    src = bank_dir / f"sample_{sample_idx:02d}.wav"
    if not src.is_file():
        raise FileNotFoundError(
            f"missing bank sample: {src}. "
            f"run scripts/extract_all_banks.py first"
        )
    samples, src_rate = read_wav_mono16(src)
    dst_audio.mkdir(parents=True, exist_ok=True)
    dst = dst_audio / f"{slug}.wav"
    if src_rate == target_rate:
        shutil.copyfile(src, dst)
    else:
        write_wav_mono16(dst, resample_int16(samples, src_rate, target_rate),
                         target_rate)
    return dst


# ---------------------------------------------------------------------------
# High-level dispatch
# ---------------------------------------------------------------------------


# className -> (extractor_fn, dst-subdir, label)
_SIMPLE_DISPATCH = {
    "BitmapJPEG":    (extract_bitmap_jpeg,    "images"),
    "BitmapSpecial": (extract_bitmap_special, "images"),
    "Mp3":           (extract_mp3,            "audio"),
    "Font":          (extract_font,           "fonts/bitmap"),
}


def extract_named(asset, class_name: str,
                  unpacked: Path, assets_dir: Path,
                  *, bitmap_sprite_kwargs: dict | None = None) -> list[Path]:
    """Dispatch a `NamedAsset` to the right extractor(s).

    Returns the list of written files (one path for most classes; one
    per sample for AudioBank).
    """
    if class_name in _SIMPLE_DISPATCH:
        fn, sub = _SIMPLE_DISPATCH[class_name]
        return [fn(asset.resource_id, asset.slug, unpacked, assets_dir / sub)]

    if class_name == "BitmapSprite":
        kw = bitmap_sprite_kwargs or {}
        return [extract_bitmap_sprite_atlas(
            asset.resource_id, asset.slug, unpacked, assets_dir / "atlases", **kw,
        )]

    if class_name == "AudioBank":
        if not asset.samples:
            raise RuntimeError(
                f"AudioBank {asset.id_hex} marked ship=true but has no "
                f"[asset.\"{asset.id_hex}\".samples] table"
            )
        return [
            extract_bank_sample(
                asset.resource_id, idx, sample_slug,
                unpacked, assets_dir / "audio",
            )
            for idx, sample_slug in sorted(asset.samples.items())
        ]

    raise RuntimeError(
        f"no extractor for class {class_name!r} (asset {asset.id_hex} "
        f"slug={asset.slug!r}); add one to lib/extractors.py or remove "
        f"ship=true from the registry entry"
    )
