# Decompilation pipeline

This document describes how the bulanci-decomp pipeline is wired and how
to regenerate every artifact end-to-end. The setup mirrors
[regratka/mkp](https://decomp.dev/regratka/mkp), with two deliberate
differences explained at the bottom.

## Layout

```
orig/             # original binaries (committed, freeware) + hashes file
config/bulanci/   # source-of-truth CSV configuration for the bulanci unit
src/bulanci/      # stub C++ source emitted by scripts/generate_sources.py
include/bulanci/  # stub C++ headers emitted by the same script
include/globals.h # shared typedefs (uchar/ushort/uint, BOOL, NULL, ...)
build/            # generated target/base .obj files (gitignored)
scripts/          # Python build orchestration
scripts/ghidra/   # Ghidra Java headless scripts
scripts/internal/ # helpers that talk to the Ghidra MCP HTTP API
.github/workflows # CI that uploads report.json as `bulanci_report`
tools/msvc8/      # vendored Visual Studio 2005 RTM toolchain (gitignored)
objdiff.json      # objdiff configuration (generated)
report.json       # objdiff progress report (committed; CI uploads it)
decomp-goal.toml  # decomp-goal-harness adapter entry point
```

## Identified toolchain

The binary's PE Rich Header pins the compile / link toolchain to
**Visual Studio 2005 RTM (VC8, link 8.00.50727, cl 14.00.50727)**.
The build date stamp (`0x44A4D775` = 2006-06-02) places it between the
RTM (Nov 2005) and SP1 (Dec 2006) releases, so SP1 is ruled out.

Re-verify any time with:

```cmd
python scripts\internal\dump_rich_header.py orig\bulanci.exe
```

A matching build is only possible with the exact RTM toolchain — see
[`tools/msvc8/README.md`](../tools/msvc8/README.md) for vendoring
instructions.

## Prerequisites

| Tool                          | How                                                                                              |
|-------------------------------|--------------------------------------------------------------------------------------------------|
| `bulanci.exe`                 | committed at `orig/bulanci.exe`                                                                  |
| Python 3.10+                  | `python --version`                                                                               |
| `pip install pefile`          | only for `scripts/internal/dump_rich_header.py`                                                  |
| Ghidra 12.0.x                 | install + set `GHIDRA_HOME` so headless scripts can find it                                      |
| ghidra-delinker-extension     | drop the matching zip from [boricj/ghidra-delinker-extension](https://github.com/boricj/ghidra-delinker-extension/releases) into `%APPDATA%\ghidra\<ver>\Extensions\` |
| ghidra-mcp plugin             | needed only by the MCP-based mapping exporter; HTTP plugin on `:8089`                            |
| `tools/objdiff-cli.exe`       | from <https://github.com/encounter/objdiff/releases>                                             |
| `tools/msvc8/` (optional)     | Visual Studio 2005 RTM toolchain - required only when you want progress > 0%                     |
| `uv`                          | for `decomp-goal-harness` (<https://github.com/astral-sh/uv>)                                    |

The delinker extension provides `CoffRelocatableObjectExporter` and the
`RelocationTableSynthesizerAnalyzer` that `scripts/ghidra/ExportDelinker.java`
relies on. Stock Ghidra does not ship them.

## Regenerate mapping.csv + units_listing.csv

`config/bulanci/mapping.csv` is the source-of-truth function table.
Two regeneration paths are supported:

* **Headless Ghidra (canonical).** `scripts/ghidra/GenerateMapping.java`
  uses `func.getName(true)` which preserves the full namespace path
  (e.g. `std::exception::exception`). This is the only path that
  captures class memberships - required for the namespace promotion
  loop below. Requires Ghidra to be CLOSED so the project lock is
  free. Takes ~10 seconds against the analysed project.

  ```bash
  python scripts/update_mapping.py "%CD%" --project bulanci ^
      --program bulanci.exe --output config\bulanci\mapping.csv
  python scripts/internal/seed_units_listing.py
  ```

* **MCP shortcut (no namespaces).** `scripts/internal/export_mapping_via_mcp.py`
  talks to the running Ghidra MCP plugin and produces the same CSV in
  seconds while the GUI stays open, **but** the MCP endpoints currently
  expose only bare function names (`FUN_xxx`), so all functions end up
  at file scope. Use this only when you specifically don't care about
  namespaces (early-stage mapping refresh).

  ```bash
  python scripts/internal/export_mapping_via_mcp.py
  python scripts/internal/seed_units_listing.py
  ```

`seed_units_listing.py` groups every function by its top-level
namespace, writing one unit per group into `units_listing.csv`. Each
unit row carries every nested namespace beneath the top-level (e.g.
`std,std::bad_alloc,std::exception`) so `helpers.has_functions` can
match them exactly. Functions at file scope are promoted to
`_Globals::FUN_xxx` so the C++ stub generator has a real namespace to
bind to.

### Namespace promotion loop

As you name and group functions inside Ghidra (e.g. `CGame::Init`,
`CBulanci::Update`), re-running the headless path will fragment the
single `_Globals` super-unit into per-class translation units
automatically:

1. In Ghidra, rename a function and/or move it into a class namespace.
2. Save the project (`Ctrl+S`).
3. Close the Ghidra GUI (the headless step needs the project lock).
4. Re-run:
   ```bash
   python scripts/update_mapping.py "%CD%" --project bulanci ^
       --program bulanci.exe --output config\bulanci\mapping.csv
   python scripts/internal/seed_units_listing.py
   python scripts/configure.py
   .\progress_update.bat
   ```
5. The new namespaces appear as separate units in `objdiff.json` and
   `report.json` will reflect them on the next CI upload.

`seed_units_listing.py` only promotes to `_Globals` when no `::` is
present in the qualified name. Anything you've already namespaced in
Ghidra is kept exactly as-is.

## Regenerate sources / objdiff / ninja

```bash
python scripts/configure.py                       # auto-detects Ghidra mode
python scripts/configure.py --skip-ghidra         # skip the ExportDelinker step
python scripts/configure.py --ghidra-mode tempdir # force tempdir reimport
```

The Ghidra step is run via `scripts/export_ghidra_objs.py` and has two
modes:

* **`existing`** (default when `bulanci.gpr` is present and unlocked):
  reuses the repo-root Ghidra project, skips auto-analysis, finishes in
  seconds. **Requires the Ghidra GUI to be closed** so the project lock
  is free.
* **`tempdir`** (fallback): imports the binary into a throwaway temp
  project and runs full auto-analysis. Always works regardless of GUI
  state; takes ~1.5 minutes for `bulanci.exe`.

Force a mode with `--ghidra-mode {existing,tempdir,auto}` or
`BULANCI_GHIDRA_MODE=...` in the environment.

After Ghidra finishes, the pipeline writes:

* `src/bulanci/<unit>.cpp` + `include/bulanci/<unit>.h` per unit
* `build/orig/bulanci/<unit>.obj` — relocatable COFF target object,
  produced by `ExportDelinker.java` via the delinker extension. One
  `.obj` per top-level namespace (currently `_Globals`, `ATL`,
  `_LocaleUpdate`, `exception`, `std`, `type_info`).
* `build.ninja` — defaults `cl` to `tools\msvc8\Bin\cl.exe`; override
  with `BULANCI_CL=path/to/cl.exe`
* `objdiff.json` — `build_base` auto-flips to `true` when `cl.exe`
  exists; `base_path` becomes non-null per unit once `ninja` has built
  the stub

### RTTI duplicate-symbol handling

`bulanci.exe` is RTTI-rich C++: Ghidra's auto-analysis discovers ~100
classes (CGaming, CBitmap, CBulanci, ...) with multiple per-inheritance
`vftable` labels sharing the same qualified name. The
`CoffRelocatableObjectExporter` aborts when its external symbol table
has duplicate names, so `ExportDelinker.java` runs an in-memory pass
that suffixes every colliding symbol with its address (e.g.
`CGaming::vftable_0042a000`). On a typical run this renames ~1140
symbols. The renames live in the Ghidra session memory only; the saved
project is untouched.

## Regenerate report.json

```bash
progress_update.bat
```

This shells out to `tools/objdiff-cli.exe report generate`. The CI
workflow at `.github/workflows/progress.yml` does **not** rebuild
`report.json`; it only uploads the committed file as the
`bulanci_report` artifact that decomp.dev's GitHub app reads. Keep
`report.json` checked in.

## decomp-goal-harness loop

```bash
uv run decomp-goal run --repo C:\path\to\bulanci --unit _Globals
```

The harness reads `decomp-goal.toml`, which calls `scripts/oracle.py`:

* `--build` reruns `objdiff-cli` and refreshes `report.json`
* `--score --unit U` extracts the unit's measures into the harness's
  `{matched, score, exact_bytes, total_bytes, matching_prefix_bytes,
  matching_prefix_percent, first_mismatch_offset}` schema
* `--diff --unit U` shells out to `objdiff-cli diff` when available, or
  prints a small text summary otherwise

## Differences vs MKP

1. **Different MSVC.** MKP vendors Visual C++ 6.6 (`tools/msvc66/`);
   bulanci.exe was linked with VS2005 RTM (`tools/msvc8/`). The
   identification is from the PE Rich Header and OptionalHeader, not
   guessed.
2. **MCP-driven mapping export.** MKP runs `GenerateMapping.java` under
   Ghidra headless, which requires a fresh import + analysis (~minutes).
   We additionally ship
   [`scripts/internal/export_mapping_via_mcp.py`](../scripts/internal/export_mapping_via_mcp.py),
   which talks to the already-running Ghidra MCP plugin and produces the
   same CSV in seconds. The headless `GenerateMapping.java` is kept as a
   no-MCP fallback.
