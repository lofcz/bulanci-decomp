"""Copy menu assets from unpacked/overlay to open_bulanci/assets/images."""

import shutil
import json
import sys
from pathlib import Path

# Local helper for the 22050 -> 48000 Hz resample applied to SFX before
# they ship to the runtime. See `RESAMPLE_NOTE` below for why this exists.
sys.path.insert(0, str(Path(__file__).parent.resolve()))
from resample_sfx import read_wav_mono16, resample_int16, write_wav_mono16  # noqa: E402

RESAMPLE_NOTE = """
Why SFX are resampled to 48 kHz before shipping
-----------------------------------------------
Retail stores its SFX bank at 22050 Hz (`extract_all_banks.py` preserves
that as-is in `unpacked/...extracted_all_samples/`).  At playback time
retail goes through DirectSound, which hands the 22050 Hz buffer to the
Windows shared-mode mixer; that mixer uses a high-order polyphase /
sinc resampler to convert to the device's default rate (48 kHz on
virtually every modern Windows install).

Our runtime uses `rodio 0.17`, whose `SampleRateConverter` implements
**unfiltered linear interpolation between integer-sample pairs**
(see `rodio/src/conversions/sample_rate.rs:139-200`).  Linear
interpolation has the spectral response of a triangular window, so any
percussive content in the source produces large amounts of aliasing in
the upper half of the output spectrum.  For `sfx_hover.wav` this shows
up as a clearly audible high-pitched ~5 kHz "intro" during the first
~100 ms of every playback — the FFT diff against a retail WASAPI
capture shows 1000-12000x more energy in the 17-20 kHz band, which the
ear perceives as a transient whine on top of the bass-heavy hover.

The fix is to do the resampling **once, offline, with a proper
band-limited polyphase filter** (scipy's `resample_poly`, Kaiser
window).  The 48 kHz asset is shipped to the runtime; rodio sees
`from == to == 48000` at the sample-rate-converter constructor
(`sample_rate.rs:70-72` and `:129-132`), short-circuits the linear
interpolation entirely, and feeds the raw samples to cpal/WASAPI.

This keeps `unpacked/...extracted_all_samples/sample_00.wav` byte-
identical to retail (the AudioBank PCM, with a 22050 Hz WAV header),
and only the shipped `open_bulanci/assets/audio/*.wav` files differ.

See `ghidra_analysis/gameplay/main_menu_hover_audio.md` §8 for the full
analysis (FFT comparisons, offline pipeline tests in `audio.rs`).
"""

RUNTIME_SAMPLE_RATE = 48000

ASSETS = {
    65850: "intro_splash.jpg",
    65657: "radio_unselected.png",
    65655: "radio_unselected_hover.png",
    65658: "radio_selected.png",
    65656: "radio_selected_hover.png",
    65659: "radio_active.png",
    65567: "btn_dialog_normal.png",
    65566: "btn_dialog_hover.png",
    65568: "btn_dialog_pressed.png",
}

BUTTON_TRACKS = [
    {"id": 65748, "name": "menu_button_off"},
    {"id": 65749, "name": "menu_button_on"},
]

def _slim_button_track(sprite_id: int, name: str, atlas_json: dict) -> dict:
    """Transform raw unpack JSON into the new generic production Atlas schema."""
    frame_count = atlas_json["frameCount"]
    frames = [{} for _ in range(frame_count)]
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

def main():
    repo_root = Path(__file__).parent.parent.resolve()
    unpacked_dir = repo_root / "unpacked" / "overlay"
    dst_dir = repo_root / "open_bulanci" / "assets" / "images"
    atlases_dir = repo_root / "open_bulanci" / "assets" / "atlases"

    dst_dir.mkdir(parents=True, exist_ok=True)
    atlases_dir.mkdir(parents=True, exist_ok=True)

    print("[menu_assets] Copying splash/radio/checkbox assets...")
    for sprite_id, target_name in ASSETS.items():
        if target_name.endswith(".jpg"):
            src_file = unpacked_dir / f"res_{sprite_id:010d}_21_BitmapJPEG.jpg"
        else:
            src_file = unpacked_dir / f"res_{sprite_id:010d}_28_BitmapSpecial.png"
        if not src_file.is_file():
            raise FileNotFoundError(
                f"Missing unpacked asset: {src_file}. "
                f"Please run bulanci_unpack first."
            )
        dst_file = dst_dir / target_name
        print(f"  {src_file.name} -> {dst_file.relative_to(repo_root)}")
        shutil.copyfile(src_file, dst_file)

    print("[menu_assets] Baking menu button atlases...")
    for track in BUTTON_TRACKS:
        sprite_id = track["id"]
        atlas_name = track["name"]

        prefix = f"res_{sprite_id:010d}_52_BitmapSprite"
        src_png = unpacked_dir / f"{prefix}.atlas.png"
        src_json = unpacked_dir / f"{prefix}.atlas.json"
        
        if not src_png.is_file() or not src_json.is_file():
            raise FileNotFoundError(
                f"Missing unpacked artefacts for sprite {sprite_id}; "
                f"please re-run tools/bulanci_unpack first."
            )

        # Create atlas folder
        atlas_dir = atlases_dir / atlas_name
        atlas_dir.mkdir(parents=True, exist_ok=True)

        # Copy png as atlas.png
        dst_png = atlas_dir / "atlas.png"
        print(f"  {src_png.name} -> {dst_png.relative_to(repo_root)}")
        shutil.copyfile(src_png, dst_png)

        # Load and transform json
        raw_json = json.loads(src_json.read_text(encoding="utf-8"))
        atlas_out = _slim_button_track(sprite_id, atlas_name, raw_json)

        # Write atlas.json
        dst_json = atlas_dir / "atlas.json"
        print(f"  {src_json.name} -> {dst_json.relative_to(repo_root)}")
        dst_json.write_text(
            json.dumps(atlas_out, indent=2) + "\n", encoding="utf-8"
        )

    # Idempotent extraction and setup of menu button audio assets from unpacked resources
    audio_dst_dir = repo_root / "open_bulanci" / "assets" / "audio"
    audio_dst_dir.mkdir(parents=True, exist_ok=True)

    print("[menu_assets] Copying menu audio files...")
    # Retail CMenu::SetDayNightBg passes the is-night flag into
    # CMenu_LoadBackgroundMusic: false -> 0x10149, true -> 0x1014a.
    ambient_mappings = {
        "res_0000065865_48_Mp3.mp3": "day_ambient.mp3",
        "res_0000065866_48_Mp3.mp3": "night_ambient.mp3",
    }
    for src_name, dst_name in ambient_mappings.items():
        src_path = unpacked_dir / src_name
        if not src_path.is_file():
            raise FileNotFoundError(f"Missing unpacked ambient audio file: {src_path}")
        dst_path = audio_dst_dir / dst_name
        print(f"  {src_path.name} -> {dst_path.relative_to(repo_root)}")
        shutil.copyfile(src_path, dst_path)

    print("[menu_assets] Copying menu button audio files...")
    # Map original unpacked samples to target assets
    # 65874 is the index ID containing menu button SFX voices and the hover sound
    audio_bank_src = unpacked_dir / "extracted_all_samples" / "bank_65874"
    if not audio_bank_src.is_dir():
        raise FileNotFoundError(
            f"Missing unpacked audio bank directory: {audio_bank_src}. "
            f"Please run scripts/extract_all_banks.py first."
        )

    audio_mappings = {
        "sample_00.wav": "sfx_hover.wav",
        "sample_14.wav": "sfx_radio_click.wav",
        "sample_24.wav": "sfx_history.wav",
        "sample_25.wav": "sfx_quit.wav",
        "sample_26.wav": "sfx_force_exit.wav",
        "sample_28.wav": "sfx_alt_exit.wav",
        "sample_27.wav": "sfx_start.wav",
    }

    for src_sample, dst_name in audio_mappings.items():
        src_path = audio_bank_src / src_sample
        if not src_path.is_file():
            raise FileNotFoundError(f"Missing unpacked sample audio file: {src_path}")
        dst_path = audio_dst_dir / dst_name
        samples, src_rate = read_wav_mono16(src_path)
        if src_rate == RUNTIME_SAMPLE_RATE:
            # Pass-through (unlikely for the retail bank but cheap to check)
            print(f"  {src_path.name} -> {dst_path.relative_to(repo_root)} [{src_rate} Hz, unchanged]")
            shutil.copyfile(src_path, dst_path)
        else:
            resampled = resample_int16(samples, src_rate, RUNTIME_SAMPLE_RATE)
            write_wav_mono16(dst_path, resampled, RUNTIME_SAMPLE_RATE)
            print(
                f"  {src_path.name} ({src_rate} Hz) -> "
                f"{dst_path.relative_to(repo_root)} [{RUNTIME_SAMPLE_RATE} Hz, polyphase]"
            )

    print("[menu_assets] Finished successfully.")

if __name__ == "__main__":
    main()
