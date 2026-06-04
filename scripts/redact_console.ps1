# Run any command with stdout/stderr streamed to the console, redacting sensitive strings.
# Child argv and cwd are unchanged — only printed lines are filtered (for screen recordings).
#
# Usage:
#   scripts\redact_console.ps1 <command> [args...]
#   scripts\redact_console.ps1 -Redact mstagl-dev,secret frida -l hook.js -f game.exe
#   scripts\redact_console.cmd scripts\frida\run_force_bedtime.cmd
#
# Environment (optional defaults):
#   REDACT_CONSOLE_REDACT  comma-separated substrings (default: %USERNAME%)
#   REDACT_CONSOLE_AS      replacement text (default: [redacted])

function Show-Usage {
    Write-Host @'
Usage:
  scripts\redact_console.ps1 [-Redact <a,b,...>] [-RedactAs <text>] <command> [args...]
  scripts\redact_console.cmd <command> [args...]

Runs <command> with unmodified arguments and working directory. Stdout/stderr are
copied to this console with each -Redact substring replaced by -RedactAs.

Examples:
  scripts\redact_console.ps1 frida -l scripts\frida\hook.js -f orig\game.exe
  scripts\redact_console.ps1 -Redact mstagl-dev python tools\dump.py
  scripts\redact_console.cmd scripts\frida\run_force_bedtime.cmd orig A1111

Defaults:
  -Redact    $env:USERNAME, or REDACT_CONSOLE_REDACT (comma-separated)
  -RedactAs  [redacted], or REDACT_CONSOLE_AS
'@
}

$Redact = @()
$RedactAs = $(if ($env:REDACT_CONSOLE_AS) { $env:REDACT_CONSOLE_AS } else { '[redacted]' })
$ExeAndArgs = @()

$i = 0
while ($i -lt $args.Count) {
    $a = $args[$i]
    if ($a -eq '-Redact' -or $a -eq '-R') {
        $i++
        if ($i -ge $args.Count) { Write-Error '-Redact requires a value'; exit 2 }
        $Redact += ($args[$i] -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ })
        $i++
        continue
    }
    if ($a -eq '-RedactAs') {
        $i++
        if ($i -ge $args.Count) { Write-Error '-RedactAs requires a value'; exit 2 }
        $RedactAs = $args[$i]
        $i++
        continue
    }
    if ($a -eq '--') {
        $i++
        break
    }
    break
}

if ($i -lt $args.Count) {
    $ExeAndArgs = $args[$i..($args.Count - 1)]
}

if (-not $ExeAndArgs -or $ExeAndArgs.Count -eq 0) {
    Show-Usage
    exit 2
}

if (-not $Redact -or $Redact.Count -eq 0) {
    if ($env:REDACT_CONSOLE_REDACT) {
        $Redact = $env:REDACT_CONSOLE_REDACT -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ }
    }
    if (-not $Redact -or $Redact.Count -eq 0) {
        if ($env:USERNAME) { $Redact = @($env:USERNAME) }
    }
}

if (-not $Redact -or $Redact.Count -eq 0) {
    Write-Error 'No -Redact strings and no USERNAME; set REDACT_CONSOLE_REDACT or pass -Redact.'
    exit 2
}

$env:PYTHONUNBUFFERED = '1'

function Write-RedactedLine([string]$Line) {
    if ($null -eq $Line -or $Line.Length -eq 0) { return }
    foreach ($needle in $Redact) {
        if ($needle) { $Line = $Line.Replace($needle, $RedactAs) }
    }
    [Console]::WriteLine($Line)
}

$exe = $ExeAndArgs[0]
$cmdArgs = @()
if ($ExeAndArgs.Count -gt 1) {
    $cmdArgs = $ExeAndArgs[1..($ExeAndArgs.Count - 1)]
}

& $exe @cmdArgs 2>&1 | ForEach-Object -Process {
    if ($_ -is [System.Management.Automation.ErrorRecord]) {
        $line = $_.ToString()
    } else {
        $line = [string]$_
    }
    Write-RedactedLine $line
}

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
exit 0
