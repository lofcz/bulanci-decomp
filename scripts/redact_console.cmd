@echo off
REM Run a command with console output redacted for screen recordings.
REM Usage: scripts\redact_console.cmd <command> [args...]
REM   e.g. scripts\redact_console.cmd scripts\frida\run_force_bedtime.cmd
REM   e.g. scripts\redact_console.cmd frida -l hook.js -f orig\bulanci.exe
REM
REM Optional env: REDACT_CONSOLE_REDACT (comma-separated), REDACT_CONSOLE_AS
setlocal EnableDelayedExpansion
if "%~1"=="" (
    echo Usage: %~nx0 ^<command^> [args...]
    echo   e.g. %~nx0 scripts\frida\run_force_bedtime.cmd
    exit /b 2
)
set "WRAPPER=%~dp0redact_console.ps1"
powershell -NoProfile -ExecutionPolicy Bypass -File "!WRAPPER!" %*
exit /b !ERRORLEVEL!
