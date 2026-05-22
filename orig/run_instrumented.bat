@echo off
setlocal
echo Starting Bulanci in Windowed Mode with Frida patches...
set FRIDA_SCRIPTS=-l patch_window.js

if "%~1"=="--no-main-menu-bg" goto no_main_menu_bg
goto run

:no_main_menu_bg
    echo Muting retail main-menu background audio...
    set FRIDA_SCRIPTS=%FRIDA_SCRIPTS% -l no_main_menu_bg.js
    shift

:run
frida %FRIDA_SCRIPTS% -f bulanci_insturmented.exe

