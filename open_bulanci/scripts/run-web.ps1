# Build the web (wasm32-unknown-emscripten) client, deploy the artifacts into
# web/, and serve them over HTTP. Used by the "wasm (web)" launch config's
# preLaunchTask: it builds, starts the server *detached*, waits until the port
# is accepting connections, then exits so the launch opens the game tab against
# a ready server. Also runnable by hand:
#
#     .\scripts\run-web.ps1            # build + serve on :8765
#     .\scripts\run-web.ps1 -Port 9000
param(
    [int]$Port = 8765
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot            # open_bulanci/

# Load the emscripten SDK (emcc on PATH) + MSVC/libclang for the C build steps.
. (Join-Path $PSScriptRoot 'dev-shell.ps1')

# ---- Compile + deploy. ----
Push-Location $root
try {
    cargo build -p bulanci_client --target wasm32-unknown-emscripten --release
    if ($LASTEXITCODE -ne 0) { throw "wasm build failed (exit $LASTEXITCODE)" }

    $out = Join-Path $root 'target/wasm32-unknown-emscripten/release'
    $web = Join-Path $root 'web'
    Copy-Item (Join-Path $out 'bulanci_client.js'), (Join-Path $out 'bulanci_client.wasm') `
        -Destination $web -Force
    Write-Host "[run-web] deployed bulanci_client.js/.wasm -> $web" -ForegroundColor Green
}
finally {
    Pop-Location
}

# ---- Free a stale server on the port, then serve detached. ----
# Starting the server detached (instead of blocking here) lets this task exit so
# the launch config's browser step runs. We then poll until the port is live so
# the tab never opens before the server is ready.
$listeners = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue |
    Select-Object -ExpandProperty OwningProcess -Unique
foreach ($procId in $listeners) {
    Stop-Process -Id $procId -Force -ErrorAction SilentlyContinue
    Write-Host "[run-web] freed port $Port (stopped PID $procId)" -ForegroundColor Yellow
}

$web = Join-Path $root 'web'
$server = Start-Process -FilePath 'python' `
    -ArgumentList @('-m', 'http.server', "$Port") `
    -WorkingDirectory $web -WindowStyle Hidden -PassThru

for ($i = 0; $i -lt 50; $i++) {
    Start-Sleep -Milliseconds 200
    if (Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue) { break }
}
if (-not (Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue)) {
    throw "server did not come up on port $Port"
}
Write-Host "[run-web] serving http://localhost:$Port (PID $($server.Id)); stop with: Stop-Process -Id $($server.Id)" -ForegroundColor Green
