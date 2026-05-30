# open_bulanci dev shell — configures MSVC + libclang (+ emscripten) so cargo
# can build the raylib + mlua toolchain (native and wasm) on Windows.
#
# Dot-source it so the env sticks in your current shell:
#     . .\scripts\dev-shell.ps1
#
# Then:
#     cargo build -p bulanci_client                                     # native
#     cargo build -p bulanci_client --target wasm32-unknown-emscripten  # web
#
# Why each piece is needed:
#  * MSVC vcvars   -> INCLUDE (libclang needs stdbool.h; the installed LLVM
#                     ships no clang builtin headers) and LIB (host build
#                     scripts link with link.exe / kernel32.lib).
#  * LIBCLANG_PATH -> bindgen (raylib-sys) loads libclang.dll from here.
#  * emsdk         -> puts emcc on PATH for the wasm32-unknown-emscripten target
#                     (raylib-sys auto-detects the SDK under %USERPROFILE%).

function Import-BatchEnv([string]$Bat) {
    if (-not (Test-Path $Bat)) { Write-Warning "[dev-shell] not found: $Bat"; return $false }
    cmd /c "`"$Bat`" >nul 2>&1 && set" | ForEach-Object {
        if ($_ -match '^([^=]+)=(.*)$') { Set-Item -Path ("env:" + $matches[1]) -Value $matches[2] }
    }
    return $true
}

# 1) MSVC environment (INCLUDE + LIB)
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vcvars  = 'C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
if (Test-Path $vswhere) {
    $vcInstall = & $vswhere -latest -products * -property installationPath 2>$null | Select-Object -First 1
    if ($vcInstall) {
        $candidate = Join-Path $vcInstall 'VC\Auxiliary\Build\vcvars64.bat'
        if (Test-Path $candidate) { $vcvars = $candidate }
    }
}
if (Import-BatchEnv $vcvars) { Write-Host "[dev-shell] MSVC env loaded" -ForegroundColor Green }

# 2) libclang for bindgen
$llvm = 'C:\Program Files\LLVM\bin'
if (Test-Path (Join-Path $llvm 'libclang.dll')) {
    $env:LIBCLANG_PATH = $llvm
    Write-Host "[dev-shell] LIBCLANG_PATH = $llvm" -ForegroundColor Green
} else {
    Write-Warning "[dev-shell] libclang.dll not found under $llvm"
}

# 3) Emscripten (web target only)
$emsdkEnv = Join-Path $env:USERPROFILE '.local\share\emsdk\emsdk_env.bat'
if (Import-BatchEnv $emsdkEnv) { Write-Host "[dev-shell] emsdk env loaded (EMSDK=$env:EMSDK)" -ForegroundColor Green }

Write-Host "[dev-shell] ready." -ForegroundColor Cyan
