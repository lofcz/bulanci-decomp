@echo off
REM
REM run_audio_logger.cmd — launch Bulanci (orig or instrumented) under
REM Frida with the bulanci_audio_logger.js script.
REM
REM Usage:
REM   scripts\frida\run_audio_logger.cmd                                  (defaults to orig)
REM   scripts\frida\run_audio_logger.cmd instrumented
REM   scripts\frida\run_audio_logger.cmd orig hit.jsonl
REM   scripts\frida\run_audio_logger.cmd orig -l scripts\frida\menu_hover_audio_trace.js
REM   scripts\frida\run_audio_logger.cmd instrumented hit.jsonl -l scripts\frida\menu_hover_audio_trace.js
REM
REM Chaining:
REM   Any `-l <script.js>` switches passed *after* the positional args
REM   are loaded into Frida in order, *after* the audio logger.  Chaining
REM   is purely additive — every script's JSONL output goes to the same
REM   stream, so you can grep the whole timeline at once.
REM
setlocal EnableDelayedExpansion

set "EXE_DIR=%~dp0..\..\orig"
set "SCRIPT=%~dp0bulanci_audio_logger.js"
set "FRIDA_SCRIPTS=-l "%SCRIPT%""
set "OUT="
set "TARGET_KIND=orig"

REM ──────── parse positional args (kind, output)
:parse_positional
if "%~1"=="" goto after_positional
if /I "%~1"=="instrumented" (
    set "TARGET_KIND=instrumented"
    shift
    goto parse_positional
)
if /I "%~1"=="orig" (
    set "TARGET_KIND=orig"
    shift
    goto parse_positional
)
REM Anything else here is treated as the output file (first hit wins).
if not defined OUT (
    set "OUT=%~1"
    shift
    goto parse_positional
)
REM Second positional after OUT — fall through to extra-args collector.
goto collect_extra

:after_positional
:collect_extra
if "%~1"=="" goto run
set "FRIDA_SCRIPTS=%FRIDA_SCRIPTS% -l "%~1""
shift
goto collect_extra

:run
if /I "%TARGET_KIND%"=="instrumented" (
    set "TARGET=%EXE_DIR%\bulanci_insturmented.exe"
) else (
    set "TARGET=%EXE_DIR%\bulanci.exe"
)

echo [run_audio_logger] target  : !TARGET!
echo [run_audio_logger] scripts : !FRIDA_SCRIPTS!
if defined OUT (
    echo [run_audio_logger] output  : !OUT!
    frida !FRIDA_SCRIPTS! -f "!TARGET!" -o "!OUT!"
) else (
    frida !FRIDA_SCRIPTS! -f "!TARGET!"
)
