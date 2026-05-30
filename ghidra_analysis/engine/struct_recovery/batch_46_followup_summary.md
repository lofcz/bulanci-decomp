# Struct recovery batch 46 follow-up (round 2)

**Index:** 46  
**Prior:** `batch_46_summary.md`, `_s_CatchableType.md`  
**Date:** 2026-05-30  
**Status:** **FOLLOWUP_DONE**

## Actions taken

| Item | Source | Result |
|------|--------|--------|
| Optional 28-byte Ghidra `_s_CatchableType` | `batch_46_summary.md` follow-ups | `create_struct` → **28 bytes**, 7 fields at `/_s_CatchableType` |
| Verify layout | `AGENT_PROTOCOL.md` | `get_struct_layout` confirms size **28** (> 1) |
| Decompile spot-check | optional goal | `CatchIt` / `FindHandler` call chain now shows `_s_CatchableType *` |
| Persist | protocol | `save_program bulanci.exe` |
| Doc update | `_s_CatchableType.md` | Ghidra apply section updated |

## Ghidra deltas

- **Before:** `/Demangler/_s_CatchableType` size **1** (placeholder).
- **After:** `/_s_CatchableType` size **28** with `dwProperties`, `pType`, PMD (`nMdisp`/`nPdisp`/`nVdisp`), `nSizeOrOffset`, `copyFunction`.
- Demangler stub unchanged (size 1); new struct is sibling path used by decompiler where typed.

## Not in scope (deferred)

| Item | Reason |
|------|--------|
| `EHExceptionRecord` | Listed for **batch 47**, not batch 46 |
| `_s_ThrowInfo` full layout | UNK; separate struct batch |
| Re-type `___BuildCatchObjectHelper` / `___TypeMatch` locals | Still `byte*`/`uint*` offsets; struct exists for call-site typing only |

## Remaining UNK

- Exact semantic names for every `properties` bit (observed: `0x1`, `0x2`, `0x8`, `0x80000000` on catchable; throw-side flags via `_s_ThrowInfo*` third arg in `___TypeMatch`).
- Tail padding after `copyFunction` — no access `>= 0x1C` (28-byte size stands).

## Artifacts

- `ghidra_analysis/engine/struct_recovery/_s_CatchableType.md` (Ghidra apply section)
- `ghidra_analysis/engine/struct_recovery/batch_46_followup_summary.md` (this file)
