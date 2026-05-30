# Struct recovery — follow-up round (50 agents)

## Input (agent N)

Read **only** the prior batch **N** deliverables:

- `ghidra_analysis/engine/struct_recovery/batch_NN_summary.md` (NN = 00..49)
- Every `{StructName}.md` referenced in that summary

## Rules

- **user-ghidra-mcp**, agent mode — **NOT read-only**
- Evidence-only (`AGENT_PROTOCOL.md`); no guessing
- Work **every** explicit follow-up / UNK / cleanup item from the prior summary
- If the prior summary has **no follow-ups** (or only "none"): write `batch_NN_followup_summary.md` with status **HANDOFF_CLEAN** and stop (no Ghidra changes unless a trivial typo in docs)
- If prior batch files are missing: status **BATCH_INCOMPLETE**, list missing paths, stop
- One `save_program bulanci.exe` per batch when Ghidra was mutated
- Output: `batch_NN_followup_summary.md` — actions taken, Ghidra deltas, remaining UNK
