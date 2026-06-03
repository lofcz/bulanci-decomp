# Round 7 — Single `FUN_*` rename / logic recovery

One agent, one function. Goal: prove role, rename if evidence supports it, or document **UNK** with disasm/xref proof.

## Input (agent N, N = 1..50)

1. Read this file and `ghidra_analysis/engine/GHIDRA_MCP.md`.
2. Load task **N** from `ghidra_analysis/engine/agent_todos_50_fun_r7.json` (`id` == N).
3. **`connect_instance("bulanci")`** then work only on **`seed_address`** / the single `addresses[0]` entry.
4. Read `mcps/user-ghidra-mcp/tools/*.json` before MCP calls.

## Rules

- **Never guess** a library or game symbol name.
- Allowed Ghidra writes when proven: `rename_function_by_address`, `set_function_prototype`, `set_function_this_type`, `set_decompiler_comment`, `set_plate_comment`, `force_decompile` (GET + `address` param).
- **Rename only** when disasm + xref closure supports a unique name (IJG/libmad upstream name, or bulanci game symbol from struct docs).
- If proof is insufficient: status **PARTIAL**, keep `FUN_*`, document evidence in report.
- One `save_program bulanci.exe` if you mutated Ghidra.
- Do **not** touch other `FUN_*` symbols in this session.

## Output

- Report: `ghidra_analysis/engine/fun_recovery/round7_fun_task_{NN}_report.md`
- Append one line to `ghidra_analysis/engine/agent_todos_50_fun_r7_results.jsonl`

Report sections: Task | Status | Function (single row table) | Ghidra deltas | Frida | Remaining UNK

## Prior hints

Check `ghidra_analysis/engine/logic_recovery/round6_logic_task_*_report.md` and `config/bulanci/mapping.csv` for the same VA — may be outdated; re-verify in Ghidra.
