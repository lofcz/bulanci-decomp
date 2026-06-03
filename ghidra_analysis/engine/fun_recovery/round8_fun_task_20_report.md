# Round 8 FUN — Task 20 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 20 |
| **round** | 8 |
| **band** | sim |
| **seed_address** | `0x00439710` |
| **title** | FUN recovery: FUN_00439710 @ 0x00439710 (xrefs=2) |
| **prior_hint** | R6 — sim band UNK |

## Status

**DONE** — Live Ghidra MCP proves **`CDSVideoPlayer::pNotifyCookie` setter** @ `+0x3c` (10-byte `__thiscall` thunk). Renamed to **`CDSVideoPlayer_SetNotifyCookie`** per [CDSVideoPlayer.md](../struct_recovery/CDSVideoPlayer.md) layout proof (`ConstructTrackManager@0x00439c70` zero-init same field). **Not** `dwCurrentFrameIdx` @ `+0x38` (`TM_AdvanceFrame` consumer).

## Function

| Address | Ghidra (before) | Ghidra (after) | Role | Evidence |
|---------|-----------------|----------------|------|----------|
| `0x00439710` | `FUN_00439710` | **`CDSVideoPlayer_SetNotifyCookie`** | `this->pNotifyCookie = value; return;` | Disasm `MOV [ECX+0x3c],EAX`; `__thiscall`; 0xa B |

### Xrefs (2)

| From | Site | Context |
|------|------|---------|
| `TM_SetAnimFrameFromAnimSub` | `0x00438fd3` | `CALL` with `this` = anim subobject @ `CAnim+0x98` → inner `+0x10` (embedded `CDSVideoPlayer` track manager) |
| `CGunMouse_ctor` | `0x004261f2` | `CALL` stores literal `1` into gun-mouse `trackManager.pNotifyCookie` |

### Layout cross-check

| Offset | Field | Writer |
|--------|-------|--------|
| `+0x38` | `dwCurrentFrameIdx` | `TM_AdvanceFrame@0x004399b0` — **not** this function |
| `+0x3c` | `pNotifyCookie` | **`ConstructTrackManager`** (`=0`); **`CDSVideoPlayer_SetNotifyCookie`** (runtime) |

Script dispatch table “SetAnimFrame → frame index” is **misleading** for this thunk: opcode passes a value into **`pNotifyCookie`**, not the frame counter ([round5_worker_10_report.md](../struct_recovery/round5_worker_10_report.md) defer resolved).

### Decompile (post-R8)

```c
void __thiscall CDSVideoPlayer_SetNotifyCookie(CDSVideoPlayer *this, uint value)
{
  this->pNotifyCookie = (void *)value;
}
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00439710` → `CDSVideoPlayer_SetNotifyCookie` | OK |
| `set_function_prototype` | `void CDSVideoPlayer_SetNotifyCookie(void)` + `__thiscall` | OK |
| `set_function_this_type` | `CDSVideoPlayer *` | OK |
| `set_decompiler_comment` | Entry | OK |
| `save_program` | `bulanci.exe` | OK |

## Frida

**none** — Single-field store; static proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Semantic meaning of `pNotifyCookie` values (`1` in `CGunMouse_ctor`, script arg in `SetAnimFrame`) | **UNK** — pointer/handle purpose not closed |
| `mapping.csv` / `_Globals.cpp` export sync | **Deferred** — still lists `FUN_00439710` stub |

## Cross-links

- [CDSVideoPlayer.md](../struct_recovery/CDSVideoPlayer.md) — `pNotifyCookie` @ `+0x3c`
- [round5_worker_10_report.md](../struct_recovery/round5_worker_10_report.md) — prior defer
