# Struct recovery — round 3 (per-task agents)

## Input (agent N)

1. Read this file and `AGENT_PROTOCOL.md`.
2. Load task **N** from `round3_tasks.json` (field `id` must equal N).
3. Read every struct deliverable listed in the task’s `structs` array:
   - `ghidra_analysis/engine/struct_recovery/{StructName}.md`
4. If `source_batches` is non-empty, skim the matching follow-up summaries for context only:
   - `batch_{NN}_followup_summary.md` (preferred)
   - else `batch_{NN}_summary.md`

## Rules

- **user-ghidra-mcp**, agent mode — **NOT read-only**
- Evidence-only: no field without ≥1 xref or ctor write + consumer
- Work **only** the assigned task `one_liner` / `acceptance`; do not expand scope
- Ghidra mutations: `create_function`, `rename_function_by_address`, `set_function_prototype`, `create_struct`, field edits — only with proof
- Merge or delete `*_recovered` placeholder types only when `get_struct_layout` proves equivalence
- One `save_program bulanci.exe` at end **if** Ghidra was mutated (once per agent)

## Output

Write `ghidra_analysis/engine/struct_recovery/round3_task_{NN}_report.md` where `{NN}` is zero-padded `id` (e.g. `round3_task_00_report.md`).

Report sections:

1. **Task** — copy `id`, `title`, `one_liner`, `acceptance`
2. **Status** — `DONE` | `PARTIAL` | `BLOCKED` | `NO_ACTION`
3. **Evidence** — func@addr table for every claim
4. **Ghidra deltas** — bullet list (or “none”)
5. **Struct doc updates** — which `.md` files changed
6. **Remaining UNK** — only what is still unproven for this task

Update each touched `{StructName}.md`: layout / UNK / Follow-up sections per `AGENT_PROTOCOL.md`.

## Blocked

If Ghidra MCP is unavailable or task is impossible without new binaries, set status `BLOCKED` with reason; still write the report.
