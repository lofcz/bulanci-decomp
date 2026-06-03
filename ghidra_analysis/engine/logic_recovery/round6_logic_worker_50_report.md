# Round 6 — Worker 50 report (logic coordinator-lite)

## Task

| Field | Value |
|-------|-------|
| **id** | 50 |
| **round** | 6 |
| **role** | coordinator-lite (documentation only) |
| **title** | R6 logic coordinator: protocol, manifest, checklist, aggregate status |
| **range** | coordinator (no address slice) |

## Status

**DONE** — launch artifacts published; **no** Ghidra MCP calls and **no** `save_program bulanci.exe` (per worker-50 scope).

## Deliverables

| Artifact | Path |
|----------|------|
| Task protocol | [ROUND6_LOGIC_PROTOCOL.md](../ROUND6_LOGIC_PROTOCOL.md) |
| Manifest (workers 1–49) | [agent_todos_50_r6_logic.json](../agent_todos_50_r6_logic.json) |
| Checklist | [agent_todos_50_r6_logic.md](../agent_todos_50_r6_logic.md) |
| Spawn template | [scripts/spawn_logic_agents.md](../../scripts/spawn_logic_agents.md) |
| Coordinator report | this file |
| Aggregate results (append-only) | [agent_todos_50_r6_logic_results.jsonl](../agent_todos_50_r6_logic_results.jsonl) |
| Manifest generator (optional) | [_gen_r6_logic_manifest.py](../_gen_r6_logic_manifest.py) |
| Checklist generator (optional) | [_gen_r6_logic_checklist.py](../_gen_r6_logic_checklist.py) |

## Address bands and function counts

Counts are from the manifest `summary.bands` (Ghidra `list_functions_enhanced` at manifest generation time; re-verify if the database diverges).

| Band key | VA range | Focus | Functions in band |
|----------|----------|-------|------------------:|
| **cluster_41ab** | `0x0041a000`–`0x0041bfff` | Gaming entities, per-entity scheduler ticks | 96 |
| **sim_429_436** | `0x00429000`–`0x00436fff` | Game state, simulation, UI/frame loop, streams/images | 615 |
| **dispatch_45a_468** | `0x0045a000`–`0x00468fff` | Codec hot paths (MP3/JPEG/mad/zlib), utility dispatch | 196 |

**Out of scope (unless xrefs from a slice require it):** global `Scheduler_*` @ `0x0042e`–`0x0042f` per protocol.

## Worker map (tasks 1–49)

| Workers | Band | Tasks | Slice style | Priority mix |
|---------|------|-------|-------------|--------------|
| **1–10** | cluster_41ab | 1–10 | Seed + neighbor addresses (~10 each) | high: 1–5; medium: 6–10 |
| **11–38** | sim_429_436 | 11–38 | Contiguous 22-function slices (task 38: 21 funcs) | medium |
| **39–49** | dispatch_45a_468 | 39–49 | Contiguous 16–18-function slices | medium |
| **50** | coordinator | — | No Ghidra; aggregate + docs | high |

### Manifest summary

| Metric | Value |
|--------|------:|
| Worker tasks (logic) | 49 |
| Coordinator task | 1 |
| **high** priority | 6 (tasks 1–5, 50) |
| **medium** priority | 43 (tasks 6–49) |
| **seed** source | 10 (cluster) |
| **slice** source | 39 (sim + dispatch) |

Each worker task lists `addresses`, optional `seed_address` / `function_names`, `ghidra_actions`, `evidence_paths`, and `acceptance` criteria in the JSON manifest.

## How workers run

1. Read [ROUND6_LOGIC_PROTOCOL.md](../ROUND6_LOGIC_PROTOCOL.md) and [struct_recovery/AGENT_PROTOCOL.md](../struct_recovery/AGENT_PROTOCOL.md).
2. Load task **N** from [agent_todos_50_r6_logic.json](../agent_todos_50_r6_logic.json) (`id` must equal **N**).
3. Grep/read paths in `evidence_paths`; stay within assigned `addresses`.
4. **user-ghidra-mcp** WRITE mode: read `mcps/user-ghidra-mcp/tools/*.json` before each call.
5. Write `logic_recovery/round6_logic_task_{NN}_report.md` (`NN` zero-padded, e.g. `01`).
6. Append **one** JSON object line to `agent_todos_50_r6_logic_results.jsonl`.
7. One `save_program bulanci.exe` at end **only if** Ghidra was mutated (once per agent).

Spawn one background agent per worker using [scripts/spawn_logic_agents.md](../../scripts/spawn_logic_agents.md) (`NN` = `01`–`50`).

## Monitoring `agent_todos_50_r6_logic_results.jsonl`

Append-only ledger: one JSON object per line per finished worker (coordinator included). Workers **must not** rewrite prior lines.

### Quick status (PowerShell, repo root)

```powershell
$j = "ghidra_analysis/engine/agent_todos_50_r6_logic_results.jsonl"
(Get-Content $j -ErrorAction SilentlyContinue | Measure-Object -Line).Lines
Get-Content $j -ErrorAction SilentlyContinue | ForEach-Object { $_ | ConvertFrom-Json } |
  Group-Object status | Select-Object Name, Count
Get-Content $j -ErrorAction SilentlyContinue | ForEach-Object { $_ | ConvertFrom-Json } |
  Sort-Object { [int]$_.id } | Select-Object id, status, title
```

### Expected completion

| Milestone | Lines in jsonl | Reports in `logic_recovery/` |
|-----------|----------------|------------------------------|
| Coordinator only | 1 (id 50) | `round6_logic_worker_50_report.md` |
| Partial | 1–49 | `round6_logic_task_{01..NN}_report.md` |
| Full round | 50 | 49 task reports + coordinator report |

### Report files on disk

```powershell
Get-ChildItem ghidra_analysis/engine/logic_recovery/round6_logic_task_*_report.md |
  Sort-Object Name | ForEach-Object { $_.Name }
```

Missing report for task **N** with no jsonl line → worker not finished. Jsonl line without report → incomplete handoff (re-run worker **N**).

### Suggested jsonl fields (workers 1–49)

Align with [ROUND6_LOGIC_PROTOCOL.md](../ROUND6_LOGIC_PROTOCOL.md) report **Status** values:

- `id`, `round` (6), `status` (`DONE` | `PARTIAL` | `BLOCKED` | `NO_ACTION`)
- `title`, `range`, `seed_address` (if any)
- `report`: `ghidra_analysis/engine/logic_recovery/round6_logic_task_{NN}_report.md`
- `ghidra_actions`: list of mutations (empty if none)
- `frida`: path or `none`
- `blockers`: strings if `BLOCKED`

Coordinator line uses `role: coordinator-lite` and empty `ghidra_actions`.

## Aggregate status (at coordinator publish)

| id | report | jsonl |
|----|--------|-------|
| 1–49 | not started | not started |
| 50 | this file | coordinator line appended |

Re-scan this table after workers finish by comparing jsonl ids to `round6_logic_task_*_report.md` on disk.

## Evidence

| Claim | Source |
|-------|--------|
| Band sizes and 49 worker slices | [agent_todos_50_r6_logic.json](../agent_todos_50_r6_logic.json) `summary` + `tasks` |
| Coordinator pattern | R5 [round5_worker_50_report.md](../struct_recovery/round5_worker_50_report.md) |
| Evidence-only + struct doc rules | [struct_recovery/AGENT_PROTOCOL.md](../struct_recovery/AGENT_PROTOCOL.md) |
| `set_function_this_type` guidance | `ghidra-mcp/docs/THIS_POINTER_TYPING.md` (via protocol) |

## Ghidra deltas

**none** — coordinator-lite.

## Frida

**none** — coordinator-lite.

## Remaining UNK

- Per-function logic recovery for **907** band functions → workers **1–49**.
- Optional: regenerate manifest if Ghidra function list drifts (`_gen_r6_logic_manifest.py` requires live MCP).
- No `agent_todos_50_r6_logic_workers.json` tracking file (use jsonl + report glob above).
