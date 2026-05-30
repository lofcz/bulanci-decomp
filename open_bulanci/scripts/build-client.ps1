# Build the native bulanci_client binary with the dev-shell toolchain
# (MSVC + libclang) configured. Used by the "win64 (debug)" launch config's
# preLaunchTask so F5 / DebugMCP always debugs freshly compiled code instead of
# a stale prebuilt exe. Also runnable by hand:
#
#     .\scripts\build-client.ps1            # debug (default)
#     .\scripts\build-client.ps1 -Profile release
param(
    [ValidateSet('debug', 'release')]
    [string]$Profile = 'debug'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot            # open_bulanci/

# Load MSVC INCLUDE/LIB + LIBCLANG_PATH (+ emsdk, harmless here).
. (Join-Path $PSScriptRoot 'dev-shell.ps1')

Push-Location $root
try {
    if ($Profile -eq 'release') {
        cargo build -p bulanci_client --release
    }
    else {
        cargo build -p bulanci_client
    }
    if ($LASTEXITCODE -ne 0) { throw "native build failed (exit $LASTEXITCODE)" }
}
finally {
    Pop-Location
}
