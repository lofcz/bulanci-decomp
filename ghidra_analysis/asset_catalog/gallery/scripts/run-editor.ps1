# Run the Bulanci asset studio (the Vite+ gallery editor) dev server and leave it
# serving so the "editor (web)" launch config can open it in a browser tab.
#
# The dev server runs through Vite+ (`vp dev`) and is started *detached* (in its
# own console, so the HMR/build logs stay visible); we poll until the port is
# live so the VS Code task exits cleanly instead of hanging as a never-ending
# background task. Runnable by hand:
#
#     .\scripts\run-editor.ps1            # serve on :5173
#     .\scripts\run-editor.ps1 -Port 5180
param(
    [int]$Port = 5173
)

$ErrorActionPreference = 'Stop'
$gallery = Split-Path -Parent $PSScriptRoot         # .../asset_catalog/gallery

# Vite+ is the project toolchain (`vp`). It wraps the package manager declared
# by `packageManager` (bun here); fall back to bun/npm only if `vp` is missing.
$useVp = [bool](Get-Command vp -ErrorAction SilentlyContinue)
$pm    = if (Get-Command bun -ErrorAction SilentlyContinue) { 'bun' } else { 'npm' }

if (-not (Test-Path (Join-Path $gallery 'node_modules'))) {
    Push-Location $gallery
    try {
        if ($useVp) {
            Write-Host "[run-editor] installing deps with vp install..." -ForegroundColor Cyan
            & vp install
        } else {
            Write-Host "[run-editor] installing deps with $pm..." -ForegroundColor Cyan
            & $pm install
        }
        if ($LASTEXITCODE -ne 0) { throw "dependency install failed (exit $LASTEXITCODE)" }
    }
    finally {
        Pop-Location
    }
}

# Free a stale dev server on the port, then start the dev server detached.
$listeners = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue |
    Select-Object -ExpandProperty OwningProcess -Unique
foreach ($procId in $listeners) {
    Stop-Process -Id $procId -Force -ErrorAction SilentlyContinue
    Write-Host "[run-editor] freed port $Port (stopped PID $procId)" -ForegroundColor Yellow
}

if ($useVp) {
    $dev = Start-Process -FilePath 'vp' -ArgumentList @('dev', '--port', "$Port") `
        -WorkingDirectory $gallery -PassThru
} else {
    $dev = Start-Process -FilePath $pm -ArgumentList @('run', 'dev', '--', '--port', "$Port") `
        -WorkingDirectory $gallery -PassThru
}

for ($i = 0; $i -lt 100; $i++) {
    Start-Sleep -Milliseconds 200
    if (Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue) { break }
}
if (-not (Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue)) {
    throw "Vite dev server did not come up on port $Port"
}
Write-Host "[run-editor] asset studio at http://localhost:$Port (PID $($dev.Id)); stop with: Stop-Process -Id $($dev.Id)" -ForegroundColor Green
