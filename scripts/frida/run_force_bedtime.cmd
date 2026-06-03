@echo off
REM
REM run_force_bedtime.cmd — launch Bulanci under Frida with the
REM bulanci_force_bedtime.js hook (pinned srand + forced first-6 _rand
REM to 0 → deterministic Variant A with all 5 entities present on
REM "Na dobrou noc" / "Bedtime story").
REM
REM Usage:
REM   scripts\frida\run_force_bedtime.cmd                                  (defaults: orig, seed 0x12345678)
REM   scripts\frida\run_force_bedtime.cmd instrumented                    (use bulanci_insturmented.exe)
REM   scripts\frida\run_force_bedtime.cmd orig 0xCAFEBABE                 (custom seed)
REM   scripts\frida\run_force_bedtime.cmd --attach <pid>                  (attach to running Bulanci)
REM   scripts\frida\run_force_bedtime.cmd --list                          (list Bulanci PIDs, then prompt)
REM
REM What the player does:
REM   After Frida attaches, pick "Na dobrou noc" from the level menu in-game.
REM   The script forces the layout deterministically; the player still drives
REM   the menu (no menu injection, no event spoofing).
REM
setlocal EnableDelayedExpansion

set "EXE_DIR=%~dp0..\..\orig"
set "SCRIPT=%~dp0bulanci_force_bedtime.js"
set "FRIDA_ARGS=-l "%SCRIPT%""
set "TARGET_KIND=orig"
set "SEED=0x12345678"
set "ATTACH_PID="
set "SPAWN_NEW=1"

REM ──────── parse args ────────
:parse_args
if "%~1"=="" goto run
if /I "%~1"=="--help" goto usage
if /I "%~1"=="-h" goto usage
if /I "%~1"=="--attach" (
    set "SPAWN_NEW=0"
    set "ATTACH_PID=%~2"
    shift /1
    shift /1
    goto parse_args
)
if /I "%~1"=="--list" (
    echo [run_force_bedtime] Bulanci processes:
    tasklist /FI "IMAGENAME eq bulanci.exe" /NH
    exit /b 0
)
if /I "%~1"=="orig" (
    set "TARGET_KIND=orig"
    shift /1
    goto parse_args
)
if /I "%~1"=="instrumented" (
    set "TARGET_KIND=instrumented"
    shift /1
    goto parse_args
)
REM Anything else: if it looks like a hex seed, use it; else echo error.
echo %~1 | findstr /R "^0x[0-9A-Fa-f][0-9A-Fa-f]*$" >nul
if !ERRORLEVEL!==0 (
    set "SEED=%~1"
    shift /1
    goto parse_args
)
echo [run_force_bedtime] unknown arg: %~1
goto usage

:run
if /I "%TARGET_KIND%"=="instrumented" (
    set "TARGET=%EXE_DIR%\bulanci_insturmented.exe"
) else (
    set "TARGET=%EXE_DIR%\bulanci.exe"
)

if not exist "%TARGET%" (
    echo [run_force_bedtime] ERROR: %TARGET% not found.
    exit /b 1
)
if not exist "%SCRIPT%" (
    echo [run_force_bedtime] ERROR: %SCRIPT% not found.
    exit /b 1
)

echo [run_force_bedtime] target : !TARGET!
echo [run_force_bedtime] script : !SCRIPT!
echo [run_force_bedtime] seed   : !SEED!

REM Pass the seed into the Frida script via env var so the user can re-run
REM with a different seed by re-running the .cmd.  The script reads
REM BULANCI_FORCE_SEED at startup (see CONFIG.seed).
set "BULANCI_FORCE_SEED=!SEED!"

if "!SPAWN_NEW!"=="1" (
    echo [run_force_bedtime] spawning new Bulanci under Frida...
    frida !FRIDA_ARGS! -f "!TARGET!"
) else (
    if "!ATTACH_PID!"=="" (
        echo [run_force_bedtime] ERROR: --attach needs a PID.
        exit /b 1
    )
    echo [run_force_bedtime] attaching to PID !ATTACH_PID!...
    frida !FRIDA_ARGS! -p "!ATTACH_PID!"
)
exit /b %ERRORLEVEL%

:usage
echo.
echo Usage:
echo   %~nx0  [orig^|instrumented] [0xSEED]
echo   %~nx0  --attach ^<pid^> [orig^|instrumented] [0xSEED]
echo   %~nx0  --list
echo   %~nx0  --help
echo.
echo Examples:
echo   %~nx0                          REM spawn orig bulanci.exe, seed 0x12345678
echo   %~nx0 instrumented              REM spawn bulanci_insturmented.exe
echo   %~nx0 orig 0xCAFEBABE          REM custom seed
echo   %~nx0 --list                   REM show running Bulanci PIDs
echo   %~nx0 --attach 1234            REM attach to PID 1234
exit /b 1
