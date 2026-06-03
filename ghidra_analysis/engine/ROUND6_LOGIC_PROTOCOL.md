# Logic / internals recovery — round 6 (per-function agents)

Three address bands in `bulanci.exe` (prior analysis may be outdated — re-verify):

| Band | Range | Focus |
|------|-------|--------|
| **cluster** | `0x0041a000`–`0x0041bfff` | Gaming entities, per-entity scheduler ticks (`CBulanek_SchedulerTick`, `CShot_SchedulerTick`, …) |
| **sim** | `0x00429000`–`0x00436fff` | Game state, simulation, rendering/audio helpers, per-frame paths |
| **dispatch** | `0x0045a000`–`0x00468fff` | Input, network, UI dispatch, codec/utility hot paths |

Global scheduler (`Scheduler_*` @ `0x0042e`–`0x0042f`) is **out of scope** for this round unless xrefs from your slice require it.

## Input (agent N)

1. Read this file and `ghidra_analysis/engine/struct_recovery/AGENT_PROTOCOL.md`.
2. Load task **N** from `ghidra_analysis/engine/agent_todos_50_r6_logic.json` (`id` must equal N).
3. Grep/read `ghidra_analysis/` paths in `evidence_paths` and any struct doc tied to your seed (`struct_recovery/{Type}.md`).
4. **user-ghidra-mcp** — **WRITE mode** (not read-only). Read tool schemas under `mcps/user-ghidra-mcp/tools/` before each call. See **`GHIDRA_MCP.md`** (connect_instance, prototype vs this_type, HTTP client).

## Rules

- **Never guess.** Decompiler `UNCERTAIN` comments are hypotheses — prove or mark UNK.
- **Runtime proof:** when static analysis is insufficient, add a Frida script under `scripts/frida/` (and reference it in your report). Do not invent field names or opcodes.
- **`set_function_prototype`** then **`set_function_this_type`** when ECX/`this` is wrong (`ghidra-mcp/docs/THIS_POINTER_TYPING.md`): prototype **without** explicit `this`; pass `calling_convention: __thiscall` separately. Then `force_decompile`.
- Other Ghidra edits (`rename_function_by_address`, `set_decompiler_comment`, …) only with disasm/xref evidence.
- One `save_program bulanci.exe` at end **if** Ghidra was mutated (once per agent).
- Stay within assigned `addresses`; if another worker already fixed a symbol, document and skip re-applying.

## Output

- Report: `ghidra_analysis/engine/logic_recovery/round6_logic_task_{NN}_report.md` (`{NN}` = zero-padded `id`, e.g. `01`).
- Append one JSON line to `ghidra_analysis/engine/agent_todos_50_r6_logic_results.jsonl`.

Report sections:

1. **Task** — `id`, `title`, `range`, `seed_address` (if any)
2. **Status** — `DONE` | `PARTIAL` | `BLOCKED` | `NO_ACTION`
3. **Functions** — table: addr | name | role summary | evidence (xref/disasm)
4. **Ghidra deltas** — or “none”; list every `set_function_this_type` / rename
5. **Frida** — path + what it verifies, or “none”
6. **Remaining UNK** — only unproven items

Update touched `struct_recovery/{Type}.md` only when you add **proven** layout or behavior (per AGENT_PROTOCOL).

## Worker 50 (coordinator-lite)

- **No Ghidra mutations**
- Delivers: this protocol, manifest, checklist, `logic_recovery/round6_logic_worker_50_report.md`, coordinator jsonl line

## Blocked

If Ghidra MCP is down or task needs live game data you cannot obtain, status `BLOCKED` with reason; still write the report.
