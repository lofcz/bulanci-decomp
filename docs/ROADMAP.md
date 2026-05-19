# Bulanci decomp roadmap

This document lays out the path from the current state (auto-analysis +
namespace promotion) to a production decomp harness running 100-1000
agents in parallel. It is opinionated: phases are sequenced so each one
removes a concrete blocker for the next, and each phase has an exit
criterion you can measure against `report.json`.

The numbers throughout are current as of the latest run (`report.json`
committed alongside this doc): **138 translation units, 3 996 functions,
422 487 bytes of code, 0% matched, 1 954 functions (49%) attributed to
a class or runtime namespace, 2 042 functions still in `_Globals`
(44.5% of code, down from 88% at session start).**

---

## Phase 0 - Foundation (DONE)

Everything an agent needs to make a single-function match attempt is in
place and reproducible from a clean checkout:

- Original binary committed under `orig/bulanci.exe` with a hash file.
- Compiler identified (VS2005 RTM, link 8.00.50727, cl 14.00.50727) via
  `scripts/internal/dump_rich_header.py`.
- Compiler **vendored** under `tools/msvc8/` (cl, link, mspdb80, CRT
  headers/libs) plus the Win Server 2003 R2 Platform SDK at
  `tools/msvc8/PlatformSDK/` and the DirectX SDK Feb 2007 at
  `tools/dxsdk_feb2007/`. All `.gitignore`d, with `README.md` files
  describing how to re-stage them.
- Ghidra 12 + `ghidra-delinker-extension` produce real relocatable
  COFFs (`build/orig/bulanci/<unit>.obj`) - 136 files, one per
  Ghidra-discovered class.
- `tools/ninja/ninja.exe` + `tools/objdiff-cli.exe` drive the
  recompile / compare loop; `progress_update.bat` refreshes
  `report.json`; `.github/workflows/progress.yml` uploads it.
- Stub generator (`scripts/generate_sources.py`) emits a compiling
  `.cpp`/`.h` pair per unit. The whole project links and produces
  `build/Src/bulanci/<unit>.obj` for objdiff to consume.

**Exit criterion (met):** `python scripts/configure.py && ninja &&
progress_update.bat` produces a 136-unit report.

---

## Phase 1 - Saturate automatic namespace promotion (IN PROGRESS)

Goal: minimise how many functions remain in `_Globals` before agents
start touching code, because every function in `_Globals` is a function
that *every* agent will trip over.

The pipeline runs ten Ghidra-side promoters in order
(`scripts/promote_namespaces.py`):

1. `ApplyFidDb.java` - applies attached FunctionID databases to name
   CRT / STL / ATL / MFC entry points. The NSA-published
   `vsOlder_x86.fidb` (VS6 - VS2010) covers VS2005 RTM. Registered
   once via `scripts/setup_fiddb.py`. Expected to lift several hundred
   CRT helpers out of `FUN_xxxx`.
2. `AssignEntryPoints.java` - names the PE entry as `_mainCRTStartup`
   and walks one hop to anchor `WinMain` / `WndProc` / `DialogProc`
   from `RegisterClass*` / `DialogBox*` argument slots.
3. `PromoteMsvcrt.java` - carves every underscore-prefixed CRT helper
   (~232 functions: `_malloc`, `_free`, `__SEH_prolog4`,
   `___tmainCRTStartup`, `_CxxThrowException@8`, `__alloca_probe_*`,
   `__cinit`, `___sbh_*`, `_memcpy`, ...) into a dedicated
   `Runtime::MSVCRT` namespace. They land in the new `Runtime.cpp`
   translation unit (132k code bytes) and stop polluting `_Globals`.
4. `PromoteVftableMembers.java` - reparents every RTTI vftable
   member into its class namespace (922 today).
5. `PromoteByVtableWrite.java` - decompiles every `FUN_xxxx` in the
   global namespace and looks for a STORE op whose value is a known
   vftable address. Whoever writes a vftable into `*this` is a
   constructor (and most destructors write their parent's vftable
   too). Captures the constructors that PromoteVftableMembers
   couldn't see because they're never themselves listed in a
   vftable. Adds a single-class fallback for static factories that
   initialise an object on the stack. Today: 164 functions
   reparented.
6. `AssignByStringRefs.java` - matches debug strings (incl. RTTI
   mangled forms and `C`/`CDS`-stripped aliases) to class names and
   reparents single-class referrers (~5 direct hits today).
7. `AssignByThisPointerType.java` - reparents file-scope functions
   whose *stored* first parameter has a recovered class-pointer type.
   Catches non-virtual methods Ghidra typed but never assigned.
8. `PromoteByConstantThis.java` - decompiles every function once and
   aggregates, per callee, the data type of arg0 *at the call site*.
   If every typed call site agrees on the same `CClass*`, reparent
   the callee. Closes the gap left by `AssignByThisPointerType`:
   functions whose stored signature is still `undefined4 *` even
   though every caller's HighFunction proves arg0 is a class
   pointer. Today: 96 functions reparented at min-sites=1.
9. `PropagateCallerNamespaces.java` - iterates to a fixed point:
   functions whose entire caller set lives in one class get
   attributed to that class (402 today on the first run, ~80 more
   after the constructor / constant-this passes seed more classed
   callers).
10. `PromoteEhFunclets.java` - for each MSVC `Catch@xxxx` /
    `Unwind@xxxx` funclet, find the nearest preceding non-funclet
    function within 0x40 bytes; if that anchor has a class namespace,
    the funclet inherits it. Runs last so it benefits from every
    prior pass. Today this rescues ~30 funclets directly: most of the
    1074 EH funclets are adjacent to a still-unclassified `FUN_xxxx`,
    so each class-promotion of a `FUN_xxxx` typically drags ~5-15 EH
    funclets along with it next run.

**Open work in this phase:**

- **Verify FidDb yield.** Once `setup_fiddb.py` has been run and the
  Ghidra GUI is closed, re-run `promote_namespaces.py` and measure how
  many CRT functions are named. Expected: 300-600 functions in
  `_Globals` move from `FUN_xxxx` to canonical CRT names like
  `_RTC_Initialize`, `_set_se_translator`, `_invoke_watson`.
- **Constant-propagated vftable arguments.** Many "free" helpers are
  actually called via `mov ecx,this; call CClass::Member`. Walking the
  decompiler's High Function and seeing what `this` points to at each
  call site would attribute another big chunk of `_Globals`.
- **Sibling-by-allocation.** Functions that take a `this` pointer
  produced by `operator new` whose return type Ghidra has already
  inferred as `CClass*` are very likely `CClass::Init` /
  `CClass::Setup` style. (Partially handled by AssignByThisPointerType
  but needs decompiler-level allocation tracking.)
- **`FUN_xxxx` -> `CClass::FUN_xxxx`.** Today the leaf is still a
  Ghidra placeholder name; agents will rename these. Encode a soft
  contract in the toolchain that the leaf can be replaced freely but
  the address suffix must remain (so renames stay diffable).
- **SDK `.gdt` archives (DONE).** Two vendored archives now sit under
  `tools/*/gdt/`:
  - `tools/dxsdk_feb2007/gdt/directx_feb2007.gdt` (~23k types)
    resolves every DirectDraw / DirectSound / DirectInput COM vtable.
  - `tools/msvc8/gdt/win32_msvc8.gdt` (~7k types) covers the full
    Win32 surface (kernel32 / user32 / advapi32 / ole32 / gdi32 /
    winmm import structs - STARTUPINFOA, OVERLAPPED, WNDCLASSEXA,
    BITMAPINFOHEADER, LOGFONTA, ...).
  Both are built by `scripts/build_gdt.py --target {dx,win32,all}` and
  applied via `scripts/apply_gdt.py`. Currently bulanci.gpr has 45k
  types in its DTM (up from ~500 originally).

**Exit criterion:** `_Globals` accounts for <= 30% of `total_code`
(currently 44.5%) and contains <= 1 000 functions (currently 2 042).

**Recent progress (May 2026):** `_Globals` shrank from 2 642 -> 2 042
(-600 functions, **-22.7%**) across two rounds of new passes:

* Round 1 (~-257): `PromoteMsvcrt.java` carved 232 MSVCRT helpers into
  a `Runtime::MSVCRT` namespace; `PromoteEhFunclets.java` attributed
  26 EH funclets adjacent to classed neighbours.
* Round 2 (~-343): `PromoteByVtableWrite.java` reparented 164
  constructors / destructors by their vftable-store pattern;
  `PromoteByConstantThis.java` aggregated arg0 types at call sites to
  promote another 96 functions; the existing callgraph pass cascaded
  78 more after those seeds, and EH funclets picked up 5 more.

The remaining 2 042 `_Globals` functions are dominated by 1 023
`Unwind@xxxx` / 51 `Catch@xxxx` MSVC EH funclets whose immediate
neighbour is itself an unclassified `FUN_xxxx`. Every class promotion
of one of those parents will drag 5-15 funclets into the class with it
next run; the cheapest path to <1 000 in `_Globals` is to keep
sharpening the call-site type-flow heuristics (lower
`--constant-this-dominance` to 0.66, allow `--callgraph-ambiguity 1`)
and re-run the chain.

---

## Phase 2 - Per-class Ghidra annotation (NEW - precedes matching)

Source-side matching agents will burn tokens guessing at `FUN_x(int,
int, int)` signatures until Ghidra knows real types. Run a wave of
**annotation agents** through Ghidra MCP first.

Wired today:

- `scripts/agent_coord.py` - file-based claim/release with TTLs and
  attempted-function tracking. State lives at `state/claims.json`
  (gitignored). Concurrent-safe via O_EXCL sidecar lockfile.
- `scripts/run_annotation_agent.py` - bundles a class's functions,
  callers, callees, xrefs and current types into a single JSON blob
  for one agent to consume. Talks to `ghidra-mcp` on
  `http://localhost:8089`.
- `docs/agent_prompts/annotate_class.md` - the prompt one agent
  follows: rename vtable slots, ctors / dtors, non-virtual members,
  fields, globals. Stays inside the assigned class; out-of-scope
  observations get filed as "hints" for a follow-up pass.

The minimum-viable swarm:

1. Operator runs `setup_fiddb.py` once and `promote_namespaces.py` once
   so the class skeleton is in place.
2. N agents (start with N = 1, then 5, then ~30) call:
   ```
   python scripts/run_annotation_agent.py --agent <id> --pick --prompt
   ```
   Each takes one class, annotates via MCP, releases. The MCP plugin
   serialises mutations so we don't need extra locking at the Ghidra
   level.
3. Operator periodically closes Ghidra, re-runs
   `promote_namespaces.py` (which reads the new mapping), regenerates
   stubs, refreshes `report.json`.

**Exit criterion:** every class with >= 5 functions has at least one
documented annotation pass (visible in `state/claims.json` history);
remaining default-named `FUN_xxxx` count below 1 500 (down from 2 571).

## Phase 3 - Single-agent matching MVP

Once class signatures are recovered, the source-side matching loop
becomes tractable. What's still needed:

1. **Function picker.** A CLI that, given the current `report.json`,
   yields the next-best function to attempt. Sort key:
   `(size_bytes, complexity_score, calls_count_only_known)` so agents
   start with small leaf functions whose callees are already
   matched-or-don't-exist.
2. **Per-function decompiler dump.** Ghidra headless that, for a given
   function entry, produces:
   - the disassembly,
   - the High C output (`DecompInterface`),
   - a JSON sidecar with parameter types, calling convention, callees,
     callers, and any data references.
3. **Per-function diff renderer.** `objdiff-cli diff --unit X
   --symbol Y` already exists; wrap it so the agent gets a
   token-aligned text diff plus a `{score, first_mismatch_off}` summary.
4. **Single-shot oracle.** `decomp-goal-harness` already provides
   `--build --score --diff`. Verify the harness round-trips end-to-end
   on a real function (e.g. `CDSException::CDSException`, a small
   leaf).
5. **Matching agent prompt template.** Distinct from the annotation
   prompt; takes (decompilation, asm, diff, prior attempt) and
   outputs a C++ body. Iteration cap (e.g. 8 attempts) per function,
   then give up and mark the unit `attempted`.

**Exit criterion:** one human (or one agent) can run a single command
and have a function matched to 100%. `report.json` shows
`matched_functions > 0`.

---

## Phase 4 - Sharded parallelism (10 - 50 agents)

The bottleneck shifts from "can the loop close?" to "can N loops close
without stepping on each other?".

Key design choices:

- **Unit-level ownership.** An agent claims a `unit` (e.g. `CBulanci`),
  works on it serially, releases it. Two agents never touch the same
  unit. This maps perfectly onto our existing 136-unit slicing.
- **Source layout.** Each unit is **one** `.cpp` plus **one** header.
  This is already how `scripts/generate_sources.py` works, which means
  agents can freely edit `src/bulanci/CBulanci.cpp` without merging
  pain. Cross-unit shared structs go into `include/bulanci/<unit>.h`
  with strict include-once semantics.
- **Claim store.** A tiny JSON or SQLite file under `state/claims.json`
  in a coordinator branch:
  ```json
  {
    "CBulanci": {
      "agent_id": "agent-073",
      "claimed_at": "2026-05-19T01:00:00Z",
      "expires_at": "2026-05-19T01:30:00Z",
      "attempted_functions": ["FUN_0040a000"]
    }
  }
  ```
  Stale claims auto-expire. Coordinator runs on the CI worker; agents
  call a `claim`/`release`/`heartbeat` endpoint.
- **CI-as-merge-gate.** Every agent pushes its branch as a PR. The PR
  CI runs the full pipeline and refuses to merge if:
  - any unit's `matched_code_percent` regressed,
  - any function compiles but reduces its match score vs `main`,
  - the build/link breaks.
- **Baseline tracking.** Commit `baseline.json` (snapshot of `main`'s
  `report.json`) and let `objdiff-cli report changes` produce the
  delta. PR reviewers see `+12 functions, +1 240 B` per PR.
- **Per-agent sandbox.** Each agent runs in a fresh worktree
  (`git worktree add ../bulanci-agent-073 main`) so 50 of them
  rebuild incrementally without cache contention.

**Exit criterion:** 10 agents can run for an hour and produce 10
non-conflicting PRs that all merge cleanly with monotonic
`matched_code` growth.

---

## Phase 5 - Continuous swarm (100 - 1 000 agents)

At this scale you need every step to be metered, retryable, and
boring:

- **Queue not poll.** Agents pull tasks from a queue (SQS, Redis,
  whatever) instead of negotiating per-claim. The coordinator
  populates the queue from `report.json` every minute.
- **Cold-task / hot-task split.** Cold tasks = an unmatched function.
  Hot tasks = a regression alert. Two separate queues so a wave of
  cold work can't starve regression handling.
- **Per-class throughput budgets.** Big classes (`CBulanci`,
  `CGaming`, ...) get N parallel workers; small classes get 1. Avoids
  10 agents fighting over `CExitDlg`.
- **Cross-function context cache.** Decompiler output for a function
  changes rarely; cache `(function_addr, ghidra_version, code_sha) ->
  decomp_output` in `state/cache/`. A 1 000-agent swarm hitting Ghidra
  fresh would melt it.
- **Heuristic match acceleration.** Before invoking the agent, try a
  cheap structural template:
  - one-line `return *(T*)this;` getters
  - copy ctor / dtor templates
  - assignment-operator templates
  These often hit 100% directly and never need an agent.
- **Failure budget.** A function that's failed N agent attempts gets
  routed to a "manual" queue and an alert. Don't burn 100 agent runs on
  the same intractable function.
- **Observability.** A live Grafana / web dashboard reading
  `report.json` history + claim logs. KPIs: matched bytes / hour, agent
  attempts / accept ratio, p50 attempts to match, time-since-progress.
- **Cost / quality dial.** Agents are graded on score-per-token; the
  scheduler can prefer cheaper agents on low-complexity functions and
  reserve expensive agents for hard cases (long, big call graphs,
  recovered-string heavy).

**Exit criterion:** 100-agent swarm sustains > 5 KB / hour of newly
matched code with < 5% PR-rejection rate and zero coordinator
intervention required for routine operation.

---

## Phase 6 - QoL & finishing touches

Below the headline progress %, these affect whether the project
finishes in months or years:

- **Type recovery feedback loop.** When an agent successfully matches
  `CClass::Foo`, capture the parameter types it converged on and push
  them back into Ghidra (via the MCP plugin) as suggestions. Future
  decomps of related functions get cleaner output.
- **Per-unit objdiff dashboards.** A page per class showing matched %,
  unmatched leaves, hottest unmatched function (by size). Lets manual
  reviewers prioritise.
- **String / asset round-trip.** A separate harness verifies that the
  Czech UI strings, Smacker video assets, DSM audio etc. are
  bit-identical between rebuilt and original after each merge.
- **Modernisation branch.** Once `matched_code_percent` >= 95%, fork
  a `modernise` branch that lifts the matched code to current MSVC and
  drops DirectDraw for SDL/DX9. Keep `main` permanently at the matched
  baseline for verification.

---

## Suggested immediate next steps (next ~1 day of work)

The shortest path from "promotion done" to "first matched function":

1. **Run the FidDb pass.** Close Ghidra, then
   `python scripts/promote_namespaces.py`. Measure how many CRT
   helpers come out of `_Globals` (expected: 300-600).
2. **Run one annotation agent end-to-end** on the smallest non-empty
   class - `CDSChain` (8 functions, 334 B) or `CDSChained` (similar).
   That exercises every layer:
   `agent_coord.py pick` -> `run_annotation_agent.py` -> agent
   renames via MCP -> `agent_coord.py release` -> re-run
   `promote_namespaces.py` -> verify the renames appear in
   `mapping.csv`.
3. **Add `scripts/internal/select_next_function.py`** for the
   source-side matcher (Phase 3): smallest, fewest-callees,
   most-callers function in the smallest non-trivial unit. Print as
   JSON for harness consumption.
4. **Add `scripts/internal/dump_function_context.py`.** Given an
   address, produce `{decomp, asm, callees, callers, types}` via
   Ghidra MCP. Cache on disk.
5. **Implement `scripts/oracle.py --next-task`.** Glue the picker +
   context dumper + objdiff scorer into a single
   `decomp-goal-harness`-compatible task spec.

After steps 1-2 you have proof the annotation loop works end-to-end;
3-5 unlock the source-side matching MVP (Phase 3) and gate parallel
matching (Phase 4).
