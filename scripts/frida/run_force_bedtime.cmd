@echo off
REM
REM run_force_bedtime.cmd — launch Bulanci under Frida with the
REM bulanci_force_bedtime_v2.js hook (resets on the per-match srand and forces
REM the first 5 CDSScript_Rand *results* to a user-specified layout on
REM "Na dobrou noc" / "Bedtime story"). v2 forces the script Rand opcode
REM result directly; the old v1 forced raw _rand() and collapsed every roll
REM to its minimum (variant A + all props absent), so it is not used here.
REM
REM Usage:
REM   scripts\frida\run_force_bedtime.cmd                                  (defaults: orig, layout A1111, seed 0x12345678)
REM   scripts\frida\run_force_bedtime.cmd instrumented                    (use bulanci_insturmented.exe)
REM   scripts\frida\run_force_bedtime.cmd orig A1010 0xCAFEBABE          (custom layout + seed)
REM   scripts\frida\run_force_bedtime.cmd --attach ^<pid^>                (attach to running Bulanci)
REM   scripts\frida\run_force_bedtime.cmd --list                          (list Bulanci PIDs, then prompt)
REM
REM Layout spec (5 chars = variant + 4 presence bits):
REM   char 1  variant : A or B  -> picks the A/B placement/coordinate table
REM   char 2  bunny   : 1=present, 0=absent
REM   char 3  mouse   : 1=present, 0=absent
REM   char 4  bird    : 1=present, 0=absent
REM   char 5  bush    : 1=present, 0=absent
REM   Butterfly is always present (no roll).
REM   Examples:
REM     A1111  variant A, all 4 entities present  (max entities)
REM     A0000  variant A, no 4 entities         (butterfly only)
REM     B1010  variant B, bunny+bird, no mouse, no bush
REM
REM The .cmd writes the layout to bulanci_force_bedtime.cfg next to the
REM .js. The Frida script reads it on startup. (Frida 17.10.1 has no
REM Process.env in the JS API, so env vars don't reach the script —
REM config file is the reliable path.)
REM
REM What the player does:
REM   After Frida attaches, pick "Na dobrou noc" from the level menu in-game.
REM   The script forces the layout deterministically; the player still drives
REM   the menu (no menu injection, no event spoofing).
REM
setlocal EnableDelayedExpansion

set "EXE_DIR=%~dp0..\..\orig"
set "SCRIPT=%~dp0bulanci_force_bedtime_v2.js"
set "CFG=%~dp0bulanci_force_bedtime.cfg"
set "FRIDA_ARGS=-l "%SCRIPT%""
set "TARGET_KIND=orig"
set "SEED=0x12345678"
set "LAYOUT=A1111"
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
REM Layout: 1 char (A|B) + 4 bits (0|1) = 5 chars total, e.g. A1111 / B1010
set "_T=%~1"
if "!_T:~0,1!"=="A" goto check_layout_tail
if "!_T:~0,1!"=="B" goto check_layout_tail
goto not_layout
:check_layout_tail
if "!_T:~1,1!"=="0" goto check_layout_mid
if "!_T:~1,1!"=="1" goto check_layout_mid
goto not_layout
:check_layout_mid
if "!_T:~2,1!"=="0" goto check_layout_mid2
if "!_T:~2,1!"=="1" goto check_layout_mid2
goto not_layout
:check_layout_mid2
if "!_T:~3,1!"=="0" goto check_layout_end
if "!_T:~3,1!"=="1" goto check_layout_end
goto not_layout
:check_layout_end
if "!_T:~4,1!"=="0" goto layout_ok
if "!_T:~4,1!"=="1" goto layout_ok
goto not_layout
:layout_ok
if "!_T:~5!"=="" (
    set "LAYOUT=!_T!"
    shift /1
    goto parse_args
)
:not_layout
REM Seed: 0xHEX
echo %~1 | findstr /R /I "^0x[0-9A-Fa-f][0-9A-Fa-f]*$" >nul
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

set "TARGET_DISP=!TARGET:mstagl-dev=[redacted]!"
set "SCRIPT_DISP=!SCRIPT:mstagl-dev=[redacted]!"

if not exist "%TARGET%" (
    echo [run_force_bedtime] ERROR: !TARGET_DISP! not found.
    exit /b 1
)
if not exist "%SCRIPT%" (
    echo [run_force_bedtime] ERROR: !SCRIPT_DISP! not found.
    exit /b 1
)

echo [run_force_bedtime] target : !TARGET_DISP!
echo [run_force_bedtime] script : !SCRIPT_DISP!
echo [run_force_bedtime] layout : !LAYOUT!   ^(variant + bunny/mouse/bird/bush^)
echo [run_force_bedtime] seed   : !SEED!

REM Write the config file the Frida script reads. We write it to the
REM current working directory (cwd) — that's where frida spawns the
REM target, so the script's `new File('bulanci_force_bedtime.cfg')`
REM resolves it. The .cmd should be invoked from the repo root
REM (e.g. C:\...\bulanci) for this to work; if invoked from a different
REM dir, the script falls back to its hardcoded default.
REM
REM If a cfg already exists in cwd we leave it untouched (the LAYOUT arg is
REM ignored) so hand-edited layouts survive re-runs. Delete the cfg to
REM regenerate it from the LAYOUT argument.
if exist "bulanci_force_bedtime.cfg" goto cfg_exists
> "bulanci_force_bedtime.cfg" echo ; bedtime layout: 5 chars = variant,bunny,mouse,bird,bush
>> "bulanci_force_bedtime.cfg" echo ;   char 1 variant : A or B picks the A/B placement table
>> "bulanci_force_bedtime.cfg" echo ;   char 2 bunny   : 1=present 0=absent
>> "bulanci_force_bedtime.cfg" echo ;   char 3 mouse   : 1=present 0=absent
>> "bulanci_force_bedtime.cfg" echo ;   char 4 bird    : 1=present 0=absent
>> "bulanci_force_bedtime.cfg" echo ;   char 5 bush    : 1=present 0=absent
>> "bulanci_force_bedtime.cfg" echo ;   butterfly is ALWAYS present - no roll
>> "bulanci_force_bedtime.cfg" echo !LAYOUT!
echo [run_force_bedtime] wrote .\bulanci_force_bedtime.cfg (=!LAYOUT!)
goto cfg_done
:cfg_exists
echo [run_force_bedtime] cfg exists, not regenerated ^(delete .\bulanci_force_bedtime.cfg to rebuild from LAYOUT^)
:cfg_done

REM Best-effort env-var pass-through (Frida 17 doesn't surface env to JS,
REM but we set it anyway for documentation / future use).
set "BULANCI_FORCE_LAYOUT=!LAYOUT!"
set "BULANCI_FORCE_SEED=!SEED!"

REM Frida console lines redacted via scripts\redact_console.ps1 (argv/cwd unchanged).
set "REDACT_PS1=%~dp0..\redact_console.ps1"
if "!SPAWN_NEW!"=="1" (
    echo [run_force_bedtime] spawning new Bulanci under Frida...
    powershell -NoProfile -ExecutionPolicy Bypass -File "!REDACT_PS1!" -- frida !FRIDA_ARGS! -f "!TARGET!"
) else (
    if "!ATTACH_PID!"=="" (
        echo [run_force_bedtime] ERROR: --attach needs a PID.
        exit /b 1
    )
    echo [run_force_bedtime] attaching to PID !ATTACH_PID!...
    powershell -NoProfile -ExecutionPolicy Bypass -File "!REDACT_PS1!" -- frida !FRIDA_ARGS! -p "!ATTACH_PID!"
)
exit /b %ERRORLEVEL%

:usage
echo.
echo Usage:
echo   %~nx0  [orig^|instrumented] [LAYOUT] [0xSEED]
echo   %~nx0  --attach ^<pid^> [orig^|instrumented] [LAYOUT] [0xSEED]
echo   %~nx0  --list
echo   %~nx0  --help
echo.
echo LAYOUT: A^|B + 4 bits for ^(bunny, mouse, bird, bush^). 1=present, 0=absent.
echo   Example: A1111  ^(variant A, all 4 entities^)
echo   Example: A0000  ^(variant A, no entities, only butterfly^)
echo   Example: B1010  ^(variant B, bunny + bird, no mouse / no bush^)
echo.
echo Examples:
echo   %~nx0                          REM spawn orig, layout A1111, seed 0x12345678
echo   %~nx0 instrumented              REM spawn bulanci_insturmented.exe
echo   %~nx0 orig A0000                REM variant A, only butterfly
echo   %~nx0 orig B1010 0xCAFEBABE    REM variant B, bunny+bird, custom seed
echo   %~nx0 --list                   REM show running Bulanci PIDs
echo   %~nx0 --attach 1234            REM attach to PID 1234
exit /b 1
