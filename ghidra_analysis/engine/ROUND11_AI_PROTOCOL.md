# Round 11 — AI behavior recovery (1:1 reimplementation)

Goal: document every AI archetype and its **exact** algorithms — vampires (co-op slots), moving campaign bots, stationary practice dummies, shoot-only paths, teleports, facing/rotation, script-driven hazards.

## References

| Resource | Path |
|----------|------|
| Ghidra MCP | `ghidra_analysis/engine/GHIDRA_MCP.md` |
| Slot taxonomy | `ghidra_analysis/gameplay/map_slots_spawner.md` |
| Player input vs AI | `ghidra_analysis/engine/player_controls.md` |
| Combat / weapons | `ghidra_analysis/gameplay/combat_projectiles.md`, `damage_pipeline.md` |
| Scheduler | `ghidra_analysis/engine/tick_system.md` |
| Script opcodes | `ghidra_analysis/engine/script_dispatch_table.md` |
| CBulanek struct | `ghidra_analysis/engine/struct_recovery/CBulanek.md` |
| IDA | `bulanci/bulanci.ida.exe.c` |
| Ghidra export (may be wrong) | `bulanci/bulanci.ghidra.exe.c` |
| Frida | `scripts/frida/` |
| Manifest | `ghidra_analysis/engine/agent_todos_30_r11_ai.json` |

## AI archetype map (verify — do not trust blindly)

| Slots (hex) | Role | Spawn entry |
|-------------|------|-------------|
| `0x00..0x03` | Human players | `CBulanek_SpawnPlayerAndCampaignSlots` |
| `0x20..0x23` | Co-op / **InsertVampires** allies | `CGaming_SpawnCoopPartnerSlots` |
| `0x24..0x27` | Solo **practice dummies** (stationary targets) | `CGaming_SpawnPracticeDummy` |
| `0x7d..0x7f` | **Campaign AI** enemies (solo, 1 human) | `CBulanek_SpawnPlayerAndCampaignSlots` when `bTotalSlots==1` |
| `0x04..0x1f`, `0x28..0x63`, `0x6c..0x7c` | Script-bound views (movers, rotators, hazards) | `BindToSlot`, `SpawnAtView`, `SetAnimDirection`, `TranslateTo` |

All player-like AI uses **`CBulanek`** (class `0x7ec`) with `bIsAiVariant`, scheduler slots, `TryBotRandomAction`, `ApplyAction`, movement step `FUN_0041af70`.

## Input (agent N, N = 1..29)

1. Load task **N** from `agent_todos_30_r11_ai.json`.
2. `connect_instance("bulanci")`; `program`: `bulanci.exe`.
3. Read MCP tool schemas before calls.

## Rules

- **Never guess.** Every branch, formula, and field offset needs disasm/xref/caller proof.
- Correlate **IDA vs Ghidra decomp**; fix decompiler mistakes in Ghidra when proven.
- **Frida** under `scripts/frida/` when static analysis cannot close a branch (document observed registers/args).
- Forbidden rename targets: generic ordinals (`helper_2`, `process_data`).
- Ghidra writes only with proof: `rename_function_by_address`, `set_function_prototype`, `set_function_this_type`, `set_decompiler_comment`, `create_struct` / `add_struct_field`.
- One `save_program bulanci.exe` per agent if mutated.
- Update `struct_recovery/CBulanek.md` or `gameplay/ai_*.md` only with proven deltas.

## Output

- Report: `ghidra_analysis/engine/ai_recovery/r11_ai_task_{NN}_report.md`
- Append JSON line to `ghidra_analysis/engine/agent_todos_30_r11_ai_results.jsonl`

Sections: **Task** | **Status** | **AI archetype** | **Algorithm** (pseudocode with proven constants) | **Functions table** | **Struct fields** | **Ghidra deltas** | **Decomp fixes** | **Frida** | **Remaining UNK**

Worker **30**: coordinator — no Ghidra mutations; `ai_recovery/r11_ai_worker_30_report.md`.
