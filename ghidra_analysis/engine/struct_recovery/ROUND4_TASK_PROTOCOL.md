# Struct recovery — round 4 (per-task agents)

## Input (agent N)

1. Read this file and `AGENT_PROTOCOL.md`.
2. Load task **N** from `ghidra_analysis/engine/agent_todos_50_r4.json` (field `id` must equal N). Fallback slice: `ghidra_analysis/engine/todos_gather_r4_{G}.json` where `G = (N-1)//10`.
3. Read the prior-round report: `ghidra_analysis/engine/struct_recovery/round3_task_{NN}_report.md`.
4. Read every struct deliverable referenced in the task’s `types` / `evidence_paths`:
   - `ghidra_analysis/engine/struct_recovery/{StructName}.md`

## Rules

- **user-ghidra-mcp**, agent mode — **NOT read-only**
- Evidence-only: no field without ≥1 xref or ctor write + consumer
- Work **only** the assigned task `title` / `ghidra_actions`; do not expand scope
- Ghidra mutations: `create_function`, `rename_function_by_address`, `set_function_prototype`, `set_function_this_type`, `create_struct`, field edits — only with proof
- Merge or delete `*_recovered` placeholder types only when `get_struct_layout` proves equivalence
- One `save_program bulanci.exe` at end **if** Ghidra was mutated (once per agent)

## Output

Write `ghidra_analysis/engine/struct_recovery/round4_task_{NN}_report.md` where `{NN}` is zero-padded `id` (e.g. `round4_task_01_report.md`).

Append one JSON line to `ghidra_analysis/engine/agent_todos_50_r4_results.jsonl`.

Report sections:

1. **Task** — copy `id`, `title`, `source`, `supersedes_todo_id`
2. **Status** — `DONE` | `PARTIAL` | `BLOCKED` | `NO_ACTION`
3. **Evidence** — func@addr table for every claim
4. **Ghidra deltas** — bullet list (or “none”)
5. **Struct doc updates** — which `.md` files changed
6. **Remaining UNK** — only what is still unproven for this task

Update each touched `{StructName}.md`: layout / UNK / Follow-up sections per `AGENT_PROTOCOL.md`.

## Blocked

If Ghidra MCP is unavailable or task is impossible without new binaries, set status `BLOCKED` with reason; still write the report.
