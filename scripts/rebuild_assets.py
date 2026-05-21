"""Master script to rebuild all Bulanci assets from source in parallel.

This script runs:
1. scripts/transpile_to_lua.py (in parallel)
2. scripts/build_cursor_atlas.py (in parallel)

Once both of these independent tasks finish successfully, it runs:
3. scripts/pack_assets.py (to bundle and transcode everything)
"""

import sys
import subprocess
from pathlib import Path
import time

def run_subprocess(script_path: Path) -> subprocess.Popen:
    print(f"[rebuild] Spawning: {script_path.name}")
    return subprocess.Popen(
        [sys.executable, str(script_path)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

def main() -> int:
    start_time = time.time()
    scripts_dir = Path(__file__).parent.resolve()
    
    transpile_script = scripts_dir / "transpile_to_lua.py"
    cursor_script = scripts_dir / "build_cursor_atlas.py"
    pack_script = scripts_dir / "pack_assets.py"
    
    # 1. Run transpiler and cursor builder in parallel
    p_transpile = run_subprocess(transpile_script)
    p_cursor = run_subprocess(cursor_script)
    
    # Wait for both processes to finish
    out_transpile, err_transpile = p_transpile.communicate()
    out_cursor, err_cursor = p_cursor.communicate()
    
    success = True
    
    # Check transpile results
    if p_transpile.returncode != 0:
        print(f"[rebuild] ERROR: {transpile_script.name} failed with exit code {p_transpile.returncode}")
        print(err_transpile)
        success = False
    else:
        print(f"[rebuild] SUCCESS: {transpile_script.name}")
        # Print a small summary of output
        lines = out_transpile.strip().splitlines()
        if lines:
            print(f"  {lines[0]}")
            if len(lines) > 1:
                print(f"  ...")
                print(f"  {lines[-1]}")

    # Check cursor results
    if p_cursor.returncode != 0:
        print(f"[rebuild] ERROR: {cursor_script.name} failed with exit code {p_cursor.returncode}")
        print(err_cursor)
        success = False
    else:
        print(f"[rebuild] SUCCESS: {cursor_script.name}")
        lines = out_cursor.strip().splitlines()
        for line in lines:
            print(f"  {line}")
            
    if not success:
        print("[rebuild] Rebuild failed during parallel tasks.")
        return 1
        
    # 2. Run pack_assets sequentially (depends on outputs of 1 & 2)
    print(f"[rebuild] Parallel tasks finished. Repacking and transcoding assets...")
    p_pack = subprocess.run(
        [sys.executable, str(pack_script)],
        capture_output=True,
        text=True
    )
    
    if p_pack.returncode != 0:
        print(f"[rebuild] ERROR: {pack_script.name} failed with exit code {p_pack.returncode}")
        print(p_pack.stderr)
        return 1
    else:
        print(f"[rebuild] SUCCESS: {pack_script.name}")
        lines = p_pack.stdout.strip().splitlines()
        for line in lines:
            print(f"  {line}")
            
    elapsed = time.time() - start_time
    print(f"[rebuild] All assets rebuilt and transcoded successfully in {elapsed:.2f} seconds!")
    return 0

if __name__ == "__main__":
    sys.exit(main())
