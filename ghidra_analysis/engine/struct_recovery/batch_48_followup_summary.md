# Struct recovery batch 48 follow-up (round 2)

**Prior:** `batch_48_summary.md`  
**Date:** 2026-05-30  
**Status:** **FOLLOWUP_COMPLETE**

## Follow-ups from prior batch

| Item | Action | Result |
|------|--------|--------|
| Apply 12-byte `EHRegistrationNode` in demangler (replace 1-byte placeholder) | `create_struct` with `pNext`, `pFrameHandler`, `state` @ offsets 0/4/8 | `/Demangler/EHRegistrationNode` size **12** (was 1) |
| Document batch 49 `TranslatorGuardRN` embed | No change — header fields at `+0x00..+0x0B` already aligned | Verified `get_struct_layout TranslatorGuardRN` |

## Ghidra deltas

- **`create_struct EHRegistrationNode`** — 3 fields, total 12 bytes (`get_struct_layout` confirms).
- **Field naming:** Ghidra emitted `nState` at `+0x08` (requested name `state`); semantics unchanged vs batch doc.
- **`save_program bulanci.exe`** — saved once after struct mutation.

## Evidence check (unchanged)

- `FindHandler@0x0044b874` — still reads `param_2+8` as byte/dword per `maxState`; prototype now `EHRegistrationNode *`.
- CRT-only / **SKIP** for game allocation policy unchanged (`AGENT_PROTOCOL.md`).

## Artifacts updated

- `ghidra_analysis/engine/struct_recovery/EHRegistrationNode.md` — Ghidra apply section marked applied.

## Remaining UNK

- Same as `EHRegistrationNode.md` UNK section (`CSET_SPECIAL` at `*param_2`, `CallCatchBlock` EBP slot, no `ENABLE_EHTRACE` tail, no game xrefs).
- Optional later: rename `nState` → `state` in Ghidra if desired; re-run decompiler on CRT EH funcs for `->nState` field syntax (currently some paths still show `param_2[8]`).
