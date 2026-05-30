"""Master script to rebuild all Bulanci assets from source.

Order of operations
-------------------
1. In parallel:
   * `transpile_to_lua.py`       — re-emit `.lua` for every master-pack Script.
   * `build_assets.py`           — extract every `ship = true` resource listed
                                   in `registry.json` into `open_bulanci/assets/`.
   * `build_typed_handles.py`    — emit `client/src/generated/assets.rs`
                                   + `assets/manifest.json` (typed Rust
                                   accessors keyed by folder/slug hashes;
                                   see FOLDERS.md).
2. Sequentially:
   * `pack_assets.py`            — bundle the asset tree into `assets.pack`.

`build_assets.py` is registry-driven and replaces the older per-feature
scripts (`build_menu_assets.py`, `build_cursor_atlas.py`); their
hand-maintained ID dicts were folded into
`ghidra_analysis/asset_catalog/registry.json`.
"""
from __future__ import annotations

import subprocess
import sys
import time
from pathlib import Path


def _spawn(script: Path) -> subprocess.Popen:
    print(f"[rebuild] Spawning: {script.name}")
    return subprocess.Popen(
        [sys.executable, str(script)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding="utf-8",
    )


def _print_result(name: str, returncode: int, stdout: str, stderr: str) -> bool:
    if returncode != 0:
        print(f"[rebuild] ERROR: {name} failed with exit code {returncode}")
        if stderr.strip():
            print(stderr)
        return False
    print(f"[rebuild] SUCCESS: {name}")
    for line in stdout.strip().splitlines():
        print(f"  {line}")
    return True


def main() -> int:
    start = time.time()
    pipeline = Path(__file__).resolve().parent

    transpile = pipeline / "transpile_to_lua.py"
    assets    = pipeline / "build_assets.py"
    handles   = pipeline / "build_typed_handles.py"
    pack      = pipeline / "pack_assets.py"

    p_transpile = _spawn(transpile)
    p_assets    = _spawn(assets)
    p_handles   = _spawn(handles)

    out_t, err_t = p_transpile.communicate()
    out_a, err_a = p_assets.communicate()
    out_h, err_h = p_handles.communicate()

    success = True
    success &= _print_result(transpile.name, p_transpile.returncode, out_t, err_t)
    success &= _print_result(assets.name,    p_assets.returncode,    out_a, err_a)
    success &= _print_result(handles.name,   p_handles.returncode,   out_h, err_h)
    if not success:
        print("[rebuild] Rebuild failed during parallel tasks.")
        return 1

    print(f"[rebuild] Parallel tasks finished. Repacking and transcoding assets...")
    p_pack = subprocess.run(
        [sys.executable, str(pack)],
        capture_output=True, text=True, encoding="utf-8",
    )
    if not _print_result(pack.name, p_pack.returncode, p_pack.stdout, p_pack.stderr):
        return 1

    elapsed = time.time() - start
    print(f"[rebuild] All assets rebuilt and transcoded successfully in "
          f"{elapsed:.2f} seconds!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
