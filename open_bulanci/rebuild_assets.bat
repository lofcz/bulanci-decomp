@echo off
rem Simple master batch file to rebuild all assets from source
rem This bat script runs python scripts/rebuild_assets.py which parallelizes transpiling and cursor generation
python "%~dp0..\scripts\rebuild_assets.py"
pause
