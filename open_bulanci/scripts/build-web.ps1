# Build the web (wasm32-unknown-emscripten) client and deploy the artifacts
# into web/. Build-only (no server) so it can be driven both by run-web.ps1 and
# by the editor's auto-build Vite plugin (see gallery/vite.config.ts), which
# rebuilds the embedded engine whenever Rust changes. Runnable by hand too:
#
#     .\scripts\build-web.ps1                  # release (default)
#     .\scripts\build-web.ps1 -Profile debug   # faster relink during dev
param(
    [ValidateSet('debug', 'release')]
    [string]$Profile = 'release'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot            # open_bulanci/

# Load the emscripten SDK (emcc on PATH) + MSVC/libclang for the C build steps.
. (Join-Path $PSScriptRoot 'dev-shell.ps1')

Push-Location $root
try {
    if ($Profile -eq 'release') {
        cargo build -p bulanci_client --target wasm32-unknown-emscripten --release
        $sub = 'release'
    }
    else {
        cargo build -p bulanci_client --target wasm32-unknown-emscripten
        $sub = 'debug'
    }
    if ($LASTEXITCODE -ne 0) { throw "wasm build failed (exit $LASTEXITCODE)" }

    $out = Join-Path $root "target/wasm32-unknown-emscripten/$sub"
    $web = Join-Path $root 'web'
    Copy-Item (Join-Path $out 'bulanci_client.js'), (Join-Path $out 'bulanci_client.wasm') `
        -Destination $web -Force
    Write-Host "[build-web] deployed bulanci_client.js/.wasm -> $web ($Profile)" -ForegroundColor Green
}
finally {
    Pop-Location
}
