# Ninja build tool

The decomp pipeline emits `build.ninja` and expects to run
`tools\ninja\ninja.exe` to compile the C++ stubs against the vendored
`tools/msvc8/` toolchain. Put the executable here so the path used
across the repo (and in CI) is stable.

## How to stage

1. Download a Windows zip from
   <https://github.com/ninja-build/ninja/releases>. Any 1.10+ version
   works; the project has been tested with 1.13.2.
2. Extract `ninja.exe` directly into this directory:

   ```text
   tools/
     ninja/
       README.md   <- this file
       ninja.exe   <- the binary you just extracted
   ```

3. Smoke-test from the repo root:

   ```cmd
   .\tools\ninja\ninja.exe --version
   ```

## Why vendored

`ninja.exe` is a few hundred KB, statically linked, and the project's
build flow assumes a known path so `progress_update.bat` and the
CI workflow do not have to probe `PATH`. The binary itself is
gitignored (see `.gitignore`) - only this README is checked in.
