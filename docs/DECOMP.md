# Bulanci decomp pipeline

Reference for agents working `orig/bulanci.exe`. Two tracks:

| Track | Goal | Where | Status |
|---|---|---|---|
| **Logic** | Understand subsystems (script VM, net, formats) before matching them | `tools/bulanci_unpack/ghidra_analysis/` (see `STATUS.md` there) | per-subsystem |
| **Match** | Byte-exact COFF symbols, scored by `objdiff` | `src/bulanci/` + `include/bulanci/` | `report.json` → [decomp.dev/lofcz/bulanci](https://decomp.dev/lofcz/bulanci) |

## Toolchain (pinned by PE Rich Header)

| | |
|---|---|
| Compiler | MSVC 8 / VS2005 RTM (`cl 14.00.50727`) — `tools/msvc8/` |
| DirectX | DX SDK Feb 2007 — `tools/dxsdk_feb2007/` |
| Decompiler | Ghidra 12.0.x + delinker + ghidra-mcp (`http://localhost:8089`) |
| Diff | `tools/objdiff-cli.exe` driven by `tools/ninja/ninja.exe` |
| Harness | `decomp-goal-harness` via `uv` |

Compiler ID re-check: `python scripts\internal\dump_rich_header.py orig\bulanci.exe`.

## Layout

```
orig/bulanci.exe                          committed; SHA1 in orig/bulanci.exe.sha1
config/bulanci/                           source-of-truth CSVs (mapping, units_listing)
src/bulanci/<unit>.cpp                    HAND-OWNED stubs/matches
include/bulanci/<unit>.h                  HAND-OWNED headers
include/bulanci/_externs.h                generated; ExportDelinker's data symbols
include/globals.h                         shared typedefs + STUB_BODY()
build/                                    generated .obj (gitignored)
report.json                               objdiff measures; CI uploads
scripts/                                  build orchestration (Python)
scripts/ghidra/                           Ghidra headless Java
tools/bulanci_unpack/                     asset unpacker
tools/bulanci_unpack/ghidra_analysis/     logic-track workspace (STATUS.md)
```

## Source contract (splat-style)

Every function block in `src/` and `include/` is wrapped in
address-keyed markers. The address is the stable key; namespace
renames migrate the block, the address never moves.

```cpp
// src/bulanci/ODSImage.cpp
// !FUNC 0x00418c00 BEGIN
/* 418C00-418C5A 0005A */
void* ODSImage::FUN_00418c00(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00418c00 END
```

A block is **matched** iff its body has no `STUB_BODY(`. Matched
blocks are preserved verbatim by `sync_units.py`; stubs are
regenerated from `mapping.csv` on every sync. When Ghidra reassigns a
matched function to a different unit, the block moves whole and you
fix the `Namespace::` prefix by hand (intentional — guards against
silent corruption if a param type also shifted).

## Workflow: match one function

1. **Pick.** `python scripts/internal/near_miss.py --print LENGTH`
   buckets every paired symbol by byte diff: `EXACT` (done), `CLOSE`
   (same size, ≤10% diff), `LENGTH` (size differs by ≤2), `OFF`.
2. **Inspect.**
   ```bash
   tools\objdiff-cli.exe diff --project . --unit <unit> ^
                              --output - --format json-pretty "<name>"
   ```
3. **Fix Ghidra's prototype if wrong.** Ghidra's auto sigs under-count
   stack args and mistype returns on tiny fns; almost every manual
   match starts here. Via MCP `set_function_prototype` + `save_program`,
   then edit the matching row in `config/bulanci/mapping.csv`.
4. `python scripts/sync_units.py` — regenerates the stub with the new
   sig.
5. **Replace the `STUB_BODY()` body** in `src/bulanci/<unit>.cpp`.
   Starting point: Ghidra's `bulanci.exe.c` (File → Export Program →
   C/C++).
6. `tools\ninja\ninja.exe && progress_update.bat`.

Don't edit outside `// !FUNC` / `// !DECL` markers; preamble and
class skeletons are regenerated when `sync_units` rewrites the file.

## Workflow: refresh from Ghidra changes

After renaming/promoting/annotating in the GUI:

1. Save (`Ctrl+S`), close Ghidra (lock must be free).
2. `python scripts/configure.py` runs the whole chain: ExportDelinker
   → `build/orig/*.obj`, scaffold new units, sync, regen `build.ninja`
   and `objdiff.json`.
3. `tools\ninja\ninja.exe && progress_update.bat`.

`configure.py --ghidra-mode tempdir` imports into a throwaway project
(works with the GUI open; ~1.5 min). `--skip-ghidra` skips the COFF
refresh.

## Logic workspace

Pre-match reverse-engineering of subsystems
(`tools/bulanci_unpack/ghidra_analysis/`). Update `STATUS.md` there
whenever you produce new findings. Subsystems at **Verified**
confidence are the highest-leverage match candidates because their
struct layouts, dispatch tables and callsites are mapped.

## Helpers

| Script | What it does |
|---|---|
| `scripts/configure.py` | Full refresh chain (Ghidra → COFF → sources → ninja → objdiff) |
| `scripts/sync_units.py` | Migrate `// !FUNC` blocks between units; preserves matched bodies |
| `scripts/generate_sources.py` | Scaffold a new unit (`--force` to rewrite) |
| `scripts/promote_namespaces.py` | 10-pass headless namespace promotion; `--help` for tuning |
| `scripts/pick_unit.py` | Highest-leverage unit from `report.json` |
| `scripts/internal/near_miss.py` | Bucket paired symbols by byte distance |
| `scripts/internal/match_global_returns.py` | Auto-match `return &DAT_xxxxxxxx;` getters via MCP |
| `scripts/internal/extract_externs.py` | Regenerate `include/bulanci/_externs.h` |

`progress_update.bat` runs `objdiff-cli report generate` and writes
`report.json` (and `changes.json` if `baseline.json` exists).

## decomp-goal-harness

```bash
uv run decomp-goal run --repo "%CD%" --unit <unit>
```

Reads `decomp-goal.toml` → `scripts/oracle.py` (`--build`, `--score
--unit U`, `--diff --unit U`). Class-level locks via
`scripts/agent_coord.py`; scaling plan in `docs/ROADMAP.md`.

## One-time setup

* **SDK type archives** (`directx_feb2007.gdt`, `win32_msvc8.gdt`):
  `python scripts\apply_gdt.py --mode existing` (Ghidra closed).
* **FidDb** (`vsOlder_x86.fidb`, VS2005 CRT names):
  clone [ghidra-data](https://github.com/NationalSecurityAgency/ghidra-data)
  next to this repo, then `python scripts\setup_fiddb.py`.

## Why the build pipeline is what it is

(Background. Skip unless something breaks.)

`scripts/internal/compile_unit.py` wraps `cl /c` and then runs
`scripts/internal/demangle_obj_symbols.py`, which (a) rewrites every
MSVC-mangled symbol (`?...@...@...`) down to ExportDelinker's
qualified form via Win32 `UnDecorateSymbolName` and (b) strips MSVC's
leading `_` from C-linkage data symbols matching Ghidra's auto-name
patterns (`DAT_xxxx`, `LAB_xxxx`, `PTR_<api>_xxxx`, `s_xxx_xxxx`,
`switchD_*`, `switchdataD_*`, `IMAGE_DOS_HEADER_*`, `thunk_FUN_*`).
`objdiff-cli report generate` pairs symbols by exact raw-name equality
(encounter/objdiff#279), so without this normalisation nothing pairs
and the project stays at 0%. `include/bulanci/_externs.h` declares
every external symbol the target COFFs reference, so hand-matched
bodies can write `return &DAT_xxxx;` and produce a matching DIR32
relocation.
