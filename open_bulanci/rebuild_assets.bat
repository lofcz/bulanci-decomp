@echo off
rem Simple master batch file to rebuild all assets from source.
rem Runs open_bulanci/asset_pipeline/rebuild_assets.py which parallelises
rem asm_to_luau.py (goto-free Luau from the curated *.script.asm) +
rem build_assets.py (registry-driven extraction
rem of every `ship = true` resource), then packs everything via
rem pack_assets.py.
python "%~dp0asset_pipeline\rebuild_assets.py"
pause
