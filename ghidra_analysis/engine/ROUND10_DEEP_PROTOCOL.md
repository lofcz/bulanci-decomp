# Round 10 — deep recovery (structures, algorithms, decomp fixes)

Ghidra MCP is live. Re-verify prior **PARTIAL** / **BLOCKED** work with xref tracing — no guessing.

## References (read first)

| Resource | Path |
|----------|------|
| Ghidra MCP workflow | `ghidra_analysis/engine/GHIDRA_MCP.md` |
| Struct rules | `ghidra_analysis/engine/struct_recovery/AGENT_PROTOCOL.md` |
| IDA export (ground truth for signatures) | `bulanci/bulanci.ida.exe.c` |
| Ghidra decomp export (may be stale/wrong) | `bulanci/bulanci.ghidra.exe.c` |
| Frida patterns | `scripts/frida/` |
| Task manifest | `ghidra_analysis/engine/agent_todos_20_r10_deep.json` |

## Input (agent N, N = 1..19)

1. Load task **N** from `agent_todos_20_r10_deep.json` (`id` == N).
2. **`connect_instance("bulanci")`**; pass **`program`: `bulanci.exe`** on mutating calls.
3. Read `mcps/user-ghidra-mcp/tools/*.json` before each MCP call.

## Rules

- **Never guess** names, field offsets, or algorithms. Every claim needs disasm/xref/caller proof.
- **Decomp can be wrong** — correlate with IDA (`bulanci.ida.exe.c`) at the same VA; if still ambiguous, write Frida under `scripts/frida/` and document observations.
- **Forbidden renames:** generic ordinals (`helper_2`, `process_data`), address suffixes kept as final names.
- **`set_function_prototype`** without explicit `this`, then **`set_function_this_type`** for `__thiscall` (see `GHIDRA_MCP.md`).
- Apply Ghidra mutations only when evidence is solid: `rename_function_by_address`, `create_struct` / `add_struct_field`, `set_decompiler_comment`, `force_decompile`.
- One `save_program bulanci.exe` at end **if** Ghidra was mutated.
- Stay within assigned `addresses` / `struct_name`; do not touch other workers' seeds.
- Update `struct_recovery/{Type}.md` only with **proven** layout/behavior.

## Output

- Report: `ghidra_analysis/engine/deep_recovery/r10_deep_task_{NN}_report.md` (`NN` = zero-padded id)
- Append one JSON line to `ghidra_analysis/engine/agent_todos_20_r10_deep_results.jsonl`

Report sections: **Task** | **Status** (`DONE` | `PARTIAL` | `BLOCKED` | `NO_ACTION`) | **Functions / Struct** (table with evidence) | **Ghidra deltas** | **Decomp corrections** (IDA vs Ghidra mismatches fixed) | **Frida** | **Remaining UNK**

## Worker 20 (coordinator)

- **No Ghidra mutations**
- Audit manifest coverage, list blocked items, write `deep_recovery/r10_deep_worker_20_report.md`, append coordinator jsonl line.
