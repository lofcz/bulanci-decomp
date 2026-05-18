@ECHO OFF
SETLOCAL

ECHO Started Update

ECHO Updating report.json...
tools\objdiff-cli.exe report generate -o report.json -f json-pretty
IF ERRORLEVEL 1 (
    ECHO objdiff-cli failed. Make sure tools\objdiff-cli.exe exists.
    ECHO Download from https://github.com/encounter/objdiff/releases
    EXIT /B 1
)

ECHO Updating changes based on baseline.json...
IF EXIST baseline.json (
    tools\objdiff-cli.exe report changes baseline.json report.json -f json-pretty -o changes.json
) ELSE (
    ECHO No baseline.json yet; skipping changes diff.
)

ENDLOCAL
