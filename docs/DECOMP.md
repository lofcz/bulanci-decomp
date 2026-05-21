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
bodies, and **everything outside the marker pairs** (includes,
hand-written ctors, member variables, free-standing helpers,
inheritance specs, your blank lines), are preserved verbatim by
`sync_units.py` -- the script rewrites *only* what's inside marker
blocks. Stubs are regenerated from `mapping.csv` on every sync, but
C++ modifiers you add inside a `// !DECL` block (`virtual`,
`explicit`, `inline`, `const`, `override`, `final`, `noexcept`) are
detected and replayed onto the freshly rendered declaration line --
keep `virtual uchar* GetClassMeta();` and the next sync still has
`virtual`. When Ghidra reassigns a
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

Anything outside `// !FUNC` / `// !DECL` markers is yours to edit
freely -- `sync_units` rewrites *only* marker-block contents and
leaves the surrounding text (includes, class members, hand-written
ctors / dtors, inheritance specs, helper functions, blank lines)
untouched. Inside a `// !DECL` block the signature is regenerated
from `mapping.csv`, but C++ modifiers (`virtual`, `const`,
`override`, ...) are preserved across rewrites.

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

## Concurrent agent workflow

Multiple agents on the same dev box would previously trample each
other: parallel `ninja` runs saw each other's WIP source, `git
checkout` to recover destroyed siblings' work, and two
`export_mapping_via_mcp.py` runs raced to be the last writer of
`mapping.csv` — the loser silently rolled back the winner's Ghidra
edits. The pipeline now isolates by per-agent **worktree** and
serialises mapping refreshes through a single **refresher** role
running on `main`.

### Topology

```
bulanci/                                main checkout (clean; only refresher writes here)
├── config/bulanci/mapping.csv          committed; refreshed by scripts/refresh_mapping.py
├── state/                              shared coordinator state (see below)
│   ├── claims.json                     agent_coord.py — per-class claims
│   └── mapping.lock                    serialises Ghidra exports
└── worktrees/
    ├── agent-A/                        git worktree on branch agent/agent-A
    ├── agent-B/                        git worktree on branch agent/agent-B
    └── …
```

`scripts/_state_root.py` resolves `state/` to the **main** checkout
regardless of which worktree the caller is running in, so all worktrees
see the same `claims.json` and `mapping.lock`.

### Agent loop (inside a worktree)

```bash
# Once, on the main checkout:
python scripts/worktree.py create --agent agent-A --unit CGame
cd worktrees/agent-A
source .envrc                            # exports BULANCI_AGENT=agent-A

# Tight loop:
# … edit src/bulanci/CGame.cpp …
tools\ninja\ninja.exe                    # build only this worktree's tree
python scripts/oracle.py --score --unit CGame

# Pick up the latest committed mapping.csv from main:
python scripts/worktree.py reset --agent agent-A

# When the unit is matched (or just at a clean stopping point):
git commit -am "CGame: match Update() (matched 3 fns)"
# From main: fast-forward merge if linear, otherwise open a PR:
python scripts/worktree.py merge --agent agent-A

# Tear down when done:
python scripts/worktree.py destroy --agent agent-A
```

`BULANCI_AGENT` gates the write-side scripts (`sync_units.py`,
`generate_sources.py`, `rename_matched_bodies.py`): they refuse to
modify a unit unless the env-var agent currently holds the claim in
`state/claims.json`. Inside a worktree this means an over-broad
rerun of `sync_units.py` can never overwrite a sibling agent's
in-flight unit, even after `git rebase` brings their committed state
into your tree.

### Refresher role

`config/bulanci/mapping.csv` is canonical state on `main`. The
**only** sanctioned writer is `scripts/refresh_mapping.py`, run from
the main checkout (it refuses to run from a worktree). It holds
`state/mapping.lock` for the entire export and commits the result:

```bash
# Must be run from the main checkout.
python scripts/refresh_mapping.py
# - acquires state/mapping.lock
# - exports Ghidra → config/bulanci/mapping.csv
# - configure.py --skip-ghidra (regenerate headers/stubs)
# - rename_matched_bodies.py (re-align signatures/call sites)
# - git commit -m "ghidra: refresh mapping (NN rows)"
```

Worktrees consume the refresh by rebasing
(`scripts/worktree.py reset --agent <id>`), never by running their own
export. Ghidra GUI edits remain concurrent — multiple agents can
rename / promote / re-prototype simultaneously — but the
`mapping.csv` capture is single-writer.

`rename_matched_bodies.py` only touches `// !FUNC` blocks whose body is
*not* a `STUB_BODY()` stub: it rewrites the function name on the
definition line and every `FUN_HHHHHHHH` call site within the body to
the new leaf name (applying the same sanitization
`generate_sources.py` uses for the header). Stubs are regenerated by
`sync_units` so they don't need to be touched. Safe to run repeatedly;
it's a no-op when names already match.

If you're a solo committer with no need for isolation, leave
`BULANCI_AGENT` unset and run the scripts directly — the guards are
no-ops in that mode and the legacy `export_mapping_via_mcp.py` +
`configure.py --skip-ghidra` flow still works (just without
`mapping.lock`'s protection from a second concurrent exporter).

## Logic workspace

Pre-match reverse-engineering of subsystems
(`tools/bulanci_unpack/ghidra_analysis/`). Update `STATUS.md` there
whenever you produce new findings. Subsystems at **Verified**
confidence are the highest-leverage match candidates because their
struct layouts, dispatch tables and callsites are mapped.

## Helpers

| Script | What it does |
|---|---|
| `scripts/worktree.py` | Per-agent git worktree manager (`create`/`destroy`/`reset`/`list`/`merge`); writes `.envrc` with `BULANCI_AGENT` and symlinks `build/orig/` to avoid re-running ExportDelinker per worktree |
| `scripts/refresh_mapping.py` | **Refresher role.** Holds `state/mapping.lock`, runs export + sync + rename, commits `mapping.csv` on `main`. Refuses to run inside a worktree. |
| `scripts/agent_coord.py` | Per-class claims (`claim`/`release`/`heartbeat`/`list`/`cleanup`); `ClaimStore.is_held(class, agent)` is the policy primitive used by every write script |
| `scripts/configure.py` | Full refresh chain (Ghidra → COFF → sources → ninja → objdiff) |
| `scripts/sync_units.py` | In-place rewrite of `// !FUNC` / `// !DECL` block contents to match `mapping.csv`. Everything outside markers is preserved verbatim; user-added C++ modifiers (`virtual`, `const`, `override`, ...) inside DECL blocks are detected and replayed. Migrates matched bodies across unit boundaries when Ghidra reassigns a function's namespace. Accepts `--agent <id>` / `BULANCI_AGENT` to enforce claims |
| `scripts/internal/rename_matched_bodies.py` | Repoint definition-line names and `FUN_HHHHHHHH` call sites inside matched bodies after a Ghidra rename. Honours `--agent` / `BULANCI_AGENT` |
| `scripts/internal/export_mapping_via_mcp.py` | Pull a fresh `mapping.csv` from a running Ghidra MCP. Now serialises on `state/mapping.lock`; usually invoked indirectly via `refresh_mapping.py` |
| `scripts/generate_sources.py` | Scaffold a new unit (`--force` to rewrite); honours `--agent` / `BULANCI_AGENT` |
| `scripts/_state_root.py` | Resolve the canonical `state/` directory regardless of which worktree the caller runs in |
| `scripts/_file_lock.py` | O_EXCL + TTL + heartbeat file-lock primitive shared by `agent_coord` and `mapping.lock` |
| `scripts/_claim_guard.py` | `resolve_agent` / `guard_units` helpers; the policy layer wired into the write scripts |
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
