# Struct recovery — round 5 (per-task agents)

## Input (agent N, N = 1..49)

1. Read this file and `ghidra_analysis/engine/struct_recovery/AGENT_PROTOCOL.md`.
2. Load task **N** from `ghidra_analysis/engine/agent_todos_50_r5.json` (`id` must equal N). Fallback slice: `ghidra_analysis/engine/todos_gather_r5_{G}.json` where `G = (N-1)//10` (when present).
3. Read the prior-round report: `ghidra_analysis/engine/struct_recovery/round4_task_{NN}_report.md`.
4. Read every struct deliverable referenced in the task’s `types` / `evidence_paths`:
   - `ghidra_analysis/engine/struct_recovery/{StructName}.md`

## Rules

- **user-ghidra-mcp**, agent mode — **NOT read-only**
- **Evidence-only:** no field without ≥1 xref or ctor write + consumer (`AGENT_PROTOCOL.md`)
- Work **only** the assigned task `title` / `acceptance` / `ghidra_actions`; do not expand scope
- **Prefer `set_function_this_type` first** when ECX/`this` is wrong or a callee is called on a facet/adjustor base; pair with `set_function_prototype` and `force_decompile` to prove the fix
- Other Ghidra mutations (`create_function`, `rename_function_by_address`, `create_struct`, `modify_struct_field`, …) — only with proof from disasm or xref
- Merge or delete `*_recovered` placeholder types only when `get_struct_layout` proves equivalence
- One `save_program bulanci.exe` at end **if** Ghidra was mutated (once per agent)
- **Do not duplicate** another worker’s Ghidra edits: stay within your task `addresses` / `types`; if a symbol was already fixed by another agent, document in your report and skip re-applying

## Output

Write `ghidra_analysis/engine/struct_recovery/round5_task_{NN}_report.md` where `{NN}` is zero-padded `id` (e.g. `round5_task_01_report.md`).

Append one JSON line to `ghidra_analysis/engine/agent_todos_50_r5_results.jsonl`.

Report sections:

1. **Task** — copy `id`, `title`, `source`, `supersedes_todo_id`, `r4_unk_summary`
2. **Status** — `DONE` | `PARTIAL` | `BLOCKED` | `NO_ACTION`
3. **Evidence** — func@addr table for every claim
4. **Ghidra deltas** — bullet list (or “none”); call out every `set_function_this_type` / prototype change
5. **Struct doc updates** — which `.md` files changed
6. **Remaining UNK** — only what is still unproven for this task

Update each touched `{StructName}.md`: layout / UNK / Follow-up sections per `AGENT_PROTOCOL.md`.

## Worker 50 (coordinator-lite)

- **No Ghidra mutations** — manifest, protocol, checklist, and coordinator report only
- Delivers: this protocol, `agent_todos_50_r5.json` (tasks 1–49), `agent_todos_50_r5.md`, `struct_recovery/round5_worker_50_report.md`, `struct_recovery/round5_worker_50_results.jsonl`

## Blocked

If Ghidra MCP is unavailable or the task is impossible without new binaries, set status `BLOCKED` with reason; still write the report.
