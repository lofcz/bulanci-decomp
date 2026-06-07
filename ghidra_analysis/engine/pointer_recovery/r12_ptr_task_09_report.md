# Round 12 — Pointer Task 09 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 9 |
| **title** | Fix pointer arithmetic / `this` typing in `CBulanek_ApplyPickupEffect` |
| **seed_address** | `0x0041ea90` |
| **callee** | `CBulanek_StepMovementAndCollision` @ `0x0041af70` |
| **call_site** | `0x0041eb28` |

## Status

**DONE** — Disasm proves unadjusted `CBulanek*` passed to seed. Weapon/pickup field accesses use typed `CBulanek` members at proven offsets. `CBulanek_IsHumanPlayer` callee retyped. Program saved.

## Disassembly proof (`CBulanek_ApplyPickupEffect`)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x0041eab4` | `MOV ESI, ECX` | Save `this` (`CBulanek*`) in **ESI** for field access |
| `0x0041eab6` | `MOV AL, byte ptr [ESI+0x70]` | `bPlayerSlot` |
| `0x0041ead1` | `MOV ECX, dword ptr [ESI+0xf8]` | `pWeapon` |
| `0x0041eb04` | `MOV ECX, dword ptr [ESI+0xd4]` | `videoTrackManager.nCurrentTrackIdx` (`+0xA8` embed `+0x2C`) |
| `0x0041eb0c` | `PUSH ESI` | `CWeapon_ctor` owner = `CBulanek*` |
| `0x0041eb18` | `MOV ECX, ESI` | Restore `this` before seed |
| `0x0041eb22` | `MOV [ESI+0xf8], EAX` | Store new `pWeapon` |
| `0x0041eb28` | `CALL 0x0041af70` | **`CBulanek_StepMovementAndCollision`** — **no** MI adjustor |
| `0x0041eb2d` | `MOV ECX, ESI` | `CBulanek_IsHumanPlayer(this)` |
| `0x0041eb38` | `MOVZX EAX, byte ptr [ESI+0x70]` | Slot for net msg |
| `0x0041eb3c` | `MOV ECX, dword ptr [ESI+0xf4]` | **`pGame`** (`CGame*`) for `CGame_NetSendDamage_t0c` |
| `0x0041eb53` | `LEA EDI, [ESI+0x11c]` | `&bAmmoKind0` ammo refill loop |
| `0x0041eb49` | `CMP byte ptr [ESI+0x70], 0x3` | AI slot refill gate (`bPlayerSlot > 3`) |

### Seed call (`0x0041eb28`) — no pointer adjustment

Contrast with `CWeapon_OnSchedulerEvent@0x0041b186` (`ADD ECX,-0xA0` before same callee, R12 task 07):

```
ApplyPickupEffect:  MOV ECX, ESI   ; ECX = CBulanek* as-is
                    CALL 0x0041af70
```

## Struct offsets verified (`get_struct_layout CBulanek` → 412 B)

| Offset | Field | Disasm use in this function |
|--------|-------|------------------------------|
| `+0x70` | `bPlayerSlot` | `[ESI+0x70]` |
| `+0xA8` / `+0xD4` | `videoTrackManager` / `nCurrentTrackIdx` | `[ESI+0xd4]` pushed to `CWeapon_ctor` |
| `+0xF4` | `pGame` | `[ESI+0xf4]` → `CGame_NetSendDamage_t0c` ECX |
| `+0xF8` | `pWeapon` | `[ESI+0xf8]` release + store |
| `+0x104..+0x11B` | `pWalkEmbed` (24 B) | Walk-state band; separates weapon ptr from ammo bytes |
| `+0x11C` | `bAmmoKind0` | `LEA EDI,[ESI+0x11c]` |

## Before / after decompilation

### Before (stale issues)

```c
_Globals::CBulanek_IsHumanPlayer((int)this);
CGame_NetSendDamage_t0c((CBulanek *)this->pGame, this->bPlayerSlot, param_1);
```

Problems: `(int)` cast on `CBulanek*`; net send `this` typed as `CBulanek*` though ECX is `pGame`; `CBulanek_IsHumanPlayer` body used raw `param_1 + 0x70` / `+ 0xf4`.

### After (live Ghidra post-fix)

```c
this->pWeapon = pCVar2;
_Globals::CBulanek_StepMovementAndCollision(this);          // unadjusted CBulanek*
uVar3 = _Globals::CBulanek_IsHumanPlayer(this);           // CBulanek* (no int cast)
CGame_NetSendDamage_t0c((CBulanek *)this->pGame, ...);    // asm: ECX = pGame (CGame*)
pbVar4 = &this->bAmmoKind0;                               // +0x11C
CWeapon_ctor(pCVar2, this, _param_1,
             (this->videoTrackManager).nCurrentTrackIdx);  // +0xD4
```

Improvements:

- All weapon/pickup paths use **`this->pWeapon`**, **`this->pGame`**, **`this->bPlayerSlot`**, **`this->bAmmoKind0`**, **`videoTrackManager.nCurrentTrackIdx`** — no raw `this+offset` in this function.
- Seed receives **`this`** directly (matches `MOV ECX,ESI` @ `0x0041eb18`/`0x0041eb2d`).
- **`CBulanek_IsHumanPlayer`** callee now decompiles with **`param_1->bPlayerSlot`** / **`param_1->pGame`**.

### Callee `CBulanek_IsHumanPlayer` (`0x00416720`) after `set_parameter_type`

```c
uint __fastcall _Globals::CBulanek_IsHumanPlayer(CBulanek *param_1)
{
  if (param_1->bPlayerSlot < 4) {
    return CGame_IsLocalPlayerSlot(param_1->pGame, param_1->bPlayerSlot);
  }
  return (uint)((param_1->pGame->chain).field_0x5 == '\0');
}
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0041ea90` | `void __thiscall CBulanek_ApplyPickupEffect(CBulanek *this, uchar param_1)` |
| `set_function_prototype` | `0x0041af70` | `uchar __thiscall CBulanek_StepMovementAndCollision(CBulanek *this)` |
| `set_function_prototype` | `0x00416720` | `uint __fastcall CBulanek_IsHumanPlayer(CBulanek *this)` |
| `set_parameter_type` | `0x00416720` / `param_1` | `int` → **`CBulanek *`** |
| `set_function_prototype` | `0x00412a40` | `void __thiscall CGame_NetSendDamage_t0c(CGame *this, uchar, uchar)` |
| `set_decompiler_comment` | `0x0041eb28`, `0x0041eb44` | Seed + net-send disasm notes |
| `force_decompile` | `0x0041ea90`, `0x00416720`, `0x0041af70`, `0x00412a40` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Remaining limitations

- **`_Globals::` prefix** on `CBulanek_StepMovementAndCollision` / `CBulanek_IsHumanPlayer` — decompiler artifact; call semantics correct.
- **Call-site cast** `(CBulanek *)this->pGame` persists because `CGame_NetSendDamage_t0c` ECX auto-parameter cannot be retyped via MCP (`ECX:4 (auto)`). Disasm @ `0x0041eb3c` proves **`CGame*`** is passed.
- **`CGame_NetSendDamage_t0c` body** still indexes as `CBulanek*` until manual UI retype; function reads `[ECX+0x1dc]` = `CGame.pDirectPlayActive` (`search_instructions` @ `0x00412a41`).
- **`CBulanek_StepMovementAndCollision` body** still shows `void *this` (same Ghidra ECX limitation); raw `this+0xf8` etc. remain inside seed only.
- Weapon release vtable dispatch still shows `(int)this->pWeapon->pVftable_primary + 8` — normal virtual dtor call pattern.

## Frida

**none** — static disasm + struct layout sufficient.
