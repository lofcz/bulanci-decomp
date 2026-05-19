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
| `tools/ninja/ninja.exe`       | from <https://github.com/ninja-build/ninja/releases>; drives the stub recompile                  |
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

### One-time SDK type setup (DirectX + Win32)

Without matching struct / COM-interface types in Ghidra's data-type
manager, every Win32 / DirectX call decompiles to inscrutable
`(**(code **)(*pDD + 0x24))(...)` arithmetic and anonymous offset
math like `*(int*)(arg + 0x18)`.

Two vendored `.gdt` archives, both committed to the tree, fix this:

| Archive | Source                                             | Coverage |
|---------|----------------------------------------------------|----------|
| `tools/dxsdk_feb2007/gdt/directx_feb2007.gdt` | DX SDK Feb 2007 (`ddraw`, `dsound`, `dinput`) | DirectX COM vtables |
| `tools/msvc8/gdt/win32_msvc8.gdt`             | MSVC8 Platform SDK (`windows.h`, `objbase.h`, `mmsystem.h`) | Win32 API surface |

Apply both at once:

```pwsh
:: Ghidra must be closed for --mode existing
python scripts\apply_gdt.py --mode existing
```

After applying you should see the decompiler render DirectX calls as
`pDS->lpVtbl->CreateSoundBuffer(pDS, &desc, &pBuf, NULL)` and Win32
calls with their proper struct args (`STARTUPINFOA`, `OVERLAPPED`,
`WIN32_FIND_DATAA`, `WNDCLASSEXA`, ...), which makes everything under
`Engine.DS.*` and the kernel32 / user32 call sites legible.

To rebuild the archives (e.g. after an SDK version bump):

```pwsh
python scripts\build_gdt.py                     :: --target all, default
python scripts\build_gdt.py --target win32       :: build only one
python scripts\build_gdt.py --phase verify       :: just spot-check existing .gdt
```

`build_gdt.py` runs `cl /E /TC /FI dx_prelude.h` on each wrapper TU
(`tools/dxsdk_feb2007/build/dx_main.c`,
`tools/msvc8/build/win32_main.c`), post-processes the flattened `.i`
through `scripts/internal_clean_i.py`-style regexes, and feeds the
result to Ghidra's CParser via
`scripts/ghidra/ParseHeadersToGdt.java`. Phases (`preprocess`,
`clean`, `parse`, `verify`) are independent and re-runnable.

### One-time FidDb setup

The first promotion pass (`ApplyFidDb.java`) uses Ghidra's FunctionID
database to recognise MSVC CRT / STL / ATL / MFC entry points. Stock
Ghidra ships the FID infrastructure but not the data files - clone the
NSA-published [`ghidra-data`](https://github.com/NationalSecurityAgency/ghidra-data)
repository alongside this project and register the relevant DB once:

```bash
git clone https://github.com/NationalSecurityAgency/ghidra-data \
    ../ghidra-data
$env:GHIDRA_DATA = "$(Resolve-Path ..\ghidra-data)"
python scripts\setup_fiddb.py
```

`vsOlder_x86.fidb` covers VS6 - VS2010 (including bulanci's VS2005
RTM toolchain). The script runs against a throwaway Ghidra project so
it works while the real `bulanci.gpr` is open in the GUI.

### Automated namespace promotion

Before doing any manual renaming, run the six chained Ghidra passes
that mechanically attribute functions to classes. Close the Ghidra
GUI first (these passes write to the project):

```bash
python scripts/promote_namespaces.py
```

The chain (see `scripts/promote_namespaces.py` for flags):

1. **`ApplyFidDb.java`** - if FidDb files are attached, applies
   single-match results to default-named functions. No-op until
   `setup_fiddb.py` has been run once. Names CRT helpers Ghidra's
   RTTI pass cannot reach.
2. **`AssignEntryPoints.java`** - tags the PE entry as
   `_mainCRTStartup`, then walks `RegisterClass*` / `DialogBox*`
   argument slots to anchor `WinMain` / `WndProc` / `DialogProc`.
3. **`PromoteVftableMembers.java`** - reparents every RTTI vftable
   member into its class namespace (~920 functions on `bulanci.exe`).
4. **`AssignByStringRefs.java`** - matches defined strings (including
   the MSVC RTTI mangled form `.?AVClass@@` and Hungarian-stripped
   aliases like `Bulanci` for `CBulanci`) against the class name
   table; if a function references exactly one class's name, it gets
   reparented.
5. **`AssignByThisPointerType.java`** - reparents any file-scope
   function whose first parameter has a pointer-to-known-class type.
   Catches non-virtual methods Ghidra typed but never assigned.
6. **`PropagateCallerNamespaces.java`** - iterates: any function whose
   entire caller set lives in one class becomes a member of that
   class. Runs to a fixed point.

After the chain finishes, `GenerateMapping.java` re-exports
`mapping.csv` so the new ownership flows downstream.

Tuning knobs:

```bash
# Skip a phase
python scripts/promote_namespaces.py --skip-strings
python scripts/promote_namespaces.py --skip-callgraph

# Relax callgraph propagation (default 0 = every caller must be in the
# same class; higher values tolerate that many strangers per fn).
python scripts/promote_namespaces.py --callgraph-ambiguity 1
```

### Manual / iterative namespace work

After the automated chain, additional renames in Ghidra are picked up
by re-running the headless path:

1. In Ghidra, rename a function and/or move it into a class namespace.
2. Save the project (`Ctrl+S`).
3. Close the Ghidra GUI (the headless step needs the project lock).
4. Re-run:
   ```bash
   python scripts/update_mapping.py "%CD%" --project bulanci ^
       --program bulanci.exe --output config\bulanci\mapping.csv
   python scripts/internal/seed_units_listing.py
   python scripts/configure.py
   .\tools\ninja\ninja.exe
   .\progress_update.bat
   ```
5. The new namespaces appear as separate units in `objdiff.json` and
   `report.json` will reflect them on the next CI upload.

`seed_units_listing.py` only promotes to `_Globals` when no `::` is
present in the qualified name. Anything you've already namespaced in
Ghidra (manually or via the automated chain) is kept exactly as-is.

### Per-class annotation agents

Once the automated promotion chain has split the program into 130+
class units, agents can pick up classes individually and annotate them
inside Ghidra (rename functions, recover field names, tighten
signatures). See [`docs/ROADMAP.md`](ROADMAP.md) for the scaling plan.

The minimal loop for one agent:

1. Make sure the Ghidra GUI is open and the `ghidra-mcp` plugin is
   bound to `http://localhost:8089`.
2. Atomically claim a class:

   ```cmd
   python scripts\agent_coord.py pick --agent agent-073 ^
       --prefer "CBulanci,CGame,CGaming"
   ```

3. Dump the per-class context bundle the agent will read:

   ```cmd
   python scripts\run_annotation_agent.py --agent agent-073 ^
       --class CBulanci --prompt > bundle.txt
   ```

   `--prompt` prepends `docs/agent_prompts/annotate_class.md`, so
   `bundle.txt` is the complete agent input.

4. Hand `bundle.txt` to your agent runner (Cursor, Claude SDK,
   OpenAI API, ...). The agent must operate strictly via the MCP
   HTTP API on `:8089`.

5. When the agent finishes, release the claim:

   ```cmd
   python scripts\agent_coord.py release CBulanci --agent agent-073 ^
       --attempted FUN_004032c0 FUN_00405110
   ```

6. Close Ghidra, re-run `python scripts\promote_namespaces.py`
   (refreshes `mapping.csv`), `python scripts\internal\seed_units_listing.py`,
   `python scripts\configure.py --ghidra-mode existing`, and
   `tools\ninja\ninja.exe`. Any new namespaces / signatures the
   agent introduced flow into the next `report.json`.

Coordinator state lives under `state/claims.json` (gitignored). The
file is safe for concurrent access from multiple agent processes on
the same host - operations serialise through `state/claims.json.lock`.

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
  `.obj` per top-level namespace; currently 136 units after running
  `promote_namespaces.py` (each RTTI class plus `_Globals`, `ATL`,
  `_LocaleUpdate`, `exception`, `std`, `type_info`).
* `build.ninja` — defaults `cl` to `tools\msvc8\Bin\cl.exe`; override
  with `BULANCI_CL=path/to/cl.exe`. Build the stubs with
  `.\tools\ninja\ninja.exe`.
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
