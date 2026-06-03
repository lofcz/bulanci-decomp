# Round 7 FUN — Task 11 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x00429880` |
| **title** | FUN recovery: FUN_00429880 @ 0x00429880 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — `__fastcall` RECT pixel-area helper; sole caller `CDSApp_AddDirtyRectCoalesced`. Renamed (was `CDSRect_GetPixelArea`); program saved.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00429880` | `CDSRect_GetPixelArea` | `CDSApp_DirtyRect_ComputeAreaPixels` | Returns `(bottom-top)*(right-left)` when `left<right` and `top<bottom`, else `0` | Disasm: `CMP` bounds, `SUB`/`IMUL`; decompile matches `RECT*` as `int*`; xref `CDSApp_AddDirtyRectCoalesced@0x0042b70c`; [round6_logic_task_15_report.md](../logic_recovery/round6_logic_task_15_report.md) merge heuristic |

### Disasm highlights

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x00429885` | `CMP EDI, EDX` | `left` vs `right` |
| `0x00429891` | `CMP ESI, EAX` | `top` vs `bottom` |
| `0x0042989a`–`0x0042989f` | `SUB` + `IMUL` | Area product |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x0042b70c` | `CDSApp_AddDirtyRectCoalesced` — pixel-area merge threshold |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00429880` → `CDSApp_DirtyRect_ComputeAreaPixels` | Success |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Pure arithmetic on `RECT` fields; static proof sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Global vs member naming | Lives in `_Globals` namespace in export; dirty-rect list is `CDSApp+0x254` band — no `this` parameter |
