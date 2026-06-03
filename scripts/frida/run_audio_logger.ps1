<#
.SYNOPSIS
  Launch Bulanci (orig or instrumented) under Frida with the audio logger.

.DESCRIPTION
  Mirrors scripts/frida/run_audio_logger.cmd but in PowerShell so the
  usage reads naturally in this repo. Chaining is supported by passing
  extra `-l <script>` switches after the positional args.

.PARAMETER Target
  'orig' (default) or 'instrumented' — which Bulanci.exe to launch.

.PARAMETER OutputFile
  If set, the JSONL stream is redirected to this file via `frida -o`.

.EXAMPLE
  PS> .\run_audio_logger.ps1

.EXAMPLE
  PS> .\run_audio_logger.ps1 -Target instrumented -OutputFile hit.jsonl
#>
[CmdletBinding()]
param(
    [ValidateSet("orig", "instrumented")]
    [string]$Target = "orig",
    [string]$OutputFile
)

$ErrorActionPreference = "Stop"

$repoRoot   = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$origDir    = Join-Path $repoRoot "orig"
$scriptPath = Join-Path $PSScriptRoot "bulanci_audio_logger.js"

$exe = if ($Target -eq "instrumented") {
    Join-Path $origDir "bulanci_insturmented.exe"
} else {
    Join-Path $origDir "bulanci.exe"
}

if (-not (Test-Path $exe))    { throw "Bulanci binary not found: $exe" }
if (-not (Test-Path $scriptPath)) { throw "Frida script not found: $scriptPath" }

$fridaArgs = @("-l", $scriptPath, "-f", $exe)
if ($OutputFile) { $fridaArgs += @("-o", $OutputFile) }

Write-Host "[run_audio_logger] target : $exe"
Write-Host "[run_audio_logger] script : $scriptPath"
if ($OutputFile) { Write-Host "[run_audio_logger] output : $OutputFile" }

# Pick up any extra `-l <script.js>` switches the caller added (chaining).
$extra = $args
foreach ($a in $extra) {
    $fridaArgs += ,$a
    Write-Host "[run_audio_logger] extra  : $a"
}

& frida @fridaArgs
