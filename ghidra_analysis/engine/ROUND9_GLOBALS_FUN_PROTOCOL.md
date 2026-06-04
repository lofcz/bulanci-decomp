# Round 9 — `_Globals::FUN_*` rename / logic recovery (65 workers)

One agent, one function. Goal: prove role, rename when evidence supports a **meaningful** name, or document **PARTIAL/UNK** with disasm/xref proof.

## Input (agent N, N = 1..65)

1. Read this file and `ghidra_analysis/engine/GHIDRA_MCP.md`.
2. Load task **N** from `ghidra_analysis/engine/agent_todos_65_globals_r9.json` (`id` == N).
3. **`connect_instance("bulanci")`** then work only on **`seed_address`**.
4. Read `mcps/user-ghidra-mcp/tools/*.json` before MCP calls.

## Rules

- **Never guess** a library or game symbol name.
- Allowed Ghidra writes when proven: `rename_function_by_address`, `set_function_prototype`, `set_function_this_type`, `set_decompiler_comment`, `set_plate_comment`, `force_decompile` (GET + `address` param).
- **Rename only** when disasm + xref closure supports a unique name:
  - IJG/libmad/zlib **upstream export** with byte/behavior match, or
  - bulanci symbol from `ghidra_analysis/` struct/gameplay docs with caller proof.
- **Forbidden names:** generic ordinals (`resize_1`, `helper_2`, `process_data`), address suffixes (`FUN_004xxxxx` kept as rename target only).
- If proof is insufficient: status **PARTIAL**, keep `FUN_*`, document evidence in report.
- One `save_program bulanci.exe` if you mutated Ghidra.
- Do **not** touch other `FUN_*` symbols in this session.
- Namespace in Ghidra is `_Globals`; `rename_function_by_address` uses the **bare** symbol name (Ghidra adds namespace).

## Prior art

1. Check `prior_hint` in manifest (R7/R8 carry-over).
2. Grep `ghidra_analysis/engine/fun_recovery/` for this VA.
3. `config/bulanci/mapping.csv` row for signature/size.
4. `src/bulanci/_Globals.cpp` stub — do not trust stub bodies; Ghidra is source of truth.
5. Re-verify in Ghidra — hints may be outdated.

## Frida

Use `scripts/frida/` only when static proof is insufficient (runtime args, vtable slot, call frequency). Document script path and observed values in report.

## Output

- Report: `ghidra_analysis/engine/fun_recovery/r9_globals_task_{NNN}_report.md` (NNN = zero-padded id)
- Append one JSON line to `ghidra_analysis/engine/agent_todos_65_globals_r9_results.jsonl`

Report sections: Task | Status | Function (single row table) | Ghidra deltas | Frida | Remaining UNK

## Status values

| Status | Meaning |
|--------|---------|
| **DONE** | Renamed in Ghidra with proof |
| **PARTIAL** | Role documented; `FUN_*` kept |
| **BLOCKED** | Ghidra MCP unavailable or function missing |
