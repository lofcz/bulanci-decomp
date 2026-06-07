# Round 12 — Pointer Task 08 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 8 |
| **title** | Fix pointer arithmetic in `CBulanek_SyncWeaponWalkFromTrack` |
| **address** | `0x0041bf00` |
| **callee** | `CBulanek_StepMovementAndCollision` @ `0x0041bf61` (`JMP 0x0041af70`) |
| **prior name** | `CWeapon::Update` (misleading namespace) |

## Status

**DONE** — Live Ghidra MCP. `this` typed **`CBulanek *`** (ECX). Index-based `this[0x55]`/`pWalkTrackSources[2/3]`/`pPad_postWalkEmbed+8` accesses mapped to proven struct fields at **`+0x154`/`+0x158`/`+0x164`**. Decompile clean; `save_program bulanci.exe`.

## Disasm proof (offset map)

| Instruction | Asm | Proven field |
|-------------|-----|--------------|
| `0x0041bf00` | `MOV EAX,[ECX+0xF8]` | `CBulanek::pWeapon` |
| `0x0041bf0a` | `MOV EDX,[ECX+0x154]` | `dwCachedWeaponParamA` |
| `0x0041bf10` | `CMP EDX,[EAX+0x5C]` | `CWeapon::dwParamA` |
| `0x0041bf15` | `MOV EDX,[ECX+0x158]` | `dwCachedWeaponParamB` |
| `0x0041bf1b` | `CMP EDX,[EAX+0x60]` | `CWeapon::dwParamB` |
| `0x0041bf20` | `MOV EDX,[ECX+0x164]` | `pCachedTrackHolder` |
| `0x0041bf26` | `CMP EDX,[EAX+0x50]` | `CWeapon::pTrackHolder` (`CBulanek*`) |
| `0x0041bf2b` | `CALL [vftable+0x24]` | params unchanged → `CDSView_InvalidateRectClipped` path |
| `0x0041bf3a` | `MOV [ECX+0x154],EDX` | cache `dwParamA` |
| `0x0041bf43` | `MOV [ECX+0x158],EAX` | cache `dwParamB` |
| `0x0041bf49` | `CMP byte [ECX+0x168],0` | `bField_168` re-entrancy guard |
| `0x0041bf59` | `MOV [ECX+0x164],EAX` | cache `pTrackHolder` |
| `0x0041bf61` | `JMP 0x0041af70` | `CBulanek_StepMovementAndCollision(this)` |

### Index → offset correction

| Stale decompile (pre-fix) | Index math | Correct offset | Named field |
|-----------------------------|------------|----------------|-------------|
| `this[0x3e]` / `pWeapon` via wrong base | — | `+0xF8` | `pWeapon` |
| `this[0x55]` / `pWalkTrackSources[2]` | `0x14C + 8` | `+0x154` | `dwCachedWeaponParamA` |
| `pWalkTrackSources[3]` | `0x14C + 12` | `+0x158` | `dwCachedWeaponParamB` |
| `*(type**)(pPad_postWalkEmbed+8)` | `0x15C + 8` | `+0x164` | `pCachedTrackHolder` |
| `bField_168` | — | `+0x168` | `bField_168` |

**Note:** `apWalkTrackSources[0/1]` remain at `+0x14C`/`+0x150` (ctor walk-track refs). Slots at `+0x154`/`+0x158` are **weapon-param caches**, not walk-source pointers — proven by comparison against `CWeapon` tail fields, not walk anim tables.

## Xrefs

| Kind | Address | Symbol | Role |
|------|---------|--------|------|
| CODE | `0x0041bf77` | `CWeapon_HideAssociatedView` | Sole caller; `CALL` with `this->pTrackHolder` as ECX (`CBulanek*`) |
| CODE | `0x0041bf61` | `CBulanek_StepMovementAndCollision` | Tail `JMP` when cache dirty and `bField_168==0` |

## Algorithm (post-fix decompile)

```c
void __fastcall CBulanek_SyncWeaponWalkFromTrack(CBulanek *this)
{
    CWeapon *pWeapon = this->pWeapon;          // +0xF8
    if (pWeapon == NULL) return;

    if (this->dwCachedWeaponParamA == pWeapon->dwParamA &&       // +0x154 vs weapon+0x5C
        this->dwCachedWeaponParamB == pWeapon->dwParamB &&       // +0x158 vs weapon+0x60
        this->pCachedTrackHolder == pWeapon->pTrackHolder) {     // +0x164 vs weapon+0x50
        this->vftable_primary[9](0, 0);   // +0x24 — no movement needed
        return;
    }
    this->dwCachedWeaponParamA = pWeapon->dwParamA;
    this->dwCachedWeaponParamB = pWeapon->dwParamB;
    this->pCachedTrackHolder = pWeapon->pTrackHolder;
    if (this->bField_168 == 0)              // +0x168
        CBulanek_StepMovementAndCollision(this);
}
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `delete_data_type` + `create_struct` | `CBulanek` | Rebuilt **412 B** layout; weapon-sync band split at `+0x14C..+0x167` |
| `modify_struct_field` | `offset:356` | Named `pCachedTrackHolder` (`CBulanek *`) @ `+0x164` |
| `set_function_prototype` | `0x0041bf00` | `void __fastcall CBulanek_SyncWeaponWalkFromTrack(CBulanek *this)` |
| `rename_variable` | `0x0041bf00` | `pCVar1` → `pWeapon` |
| `set_decompiler_comment` | `0x0041bf00` | Offset map + caller note |
| `force_decompile` | `0x0041bf00` | Clean field names |
| `save_program` | `bulanci.exe` | Saved |

### `CBulanek` weapon-sync band (new fields)

| Offset | Name | Type | Evidence |
|--------|------|------|----------|
| `+0x14C` | `pWalkTrackSources_0` | `void *` | Ctor `AddTrackSource` slot 0 |
| `+0x150` | `pWalkTrackSources_1` | `void *` | Ctor slot 1 |
| `+0x154` | `dwCachedWeaponParamA` | `uint` | Disasm `CMP [ECX+0x154],[EAX+0x5C]` |
| `+0x158` | `dwCachedWeaponParamB` | `uint` | Disasm `CMP [ECX+0x158],[EAX+0x60]` |
| `+0x15C` | `pPad_weaponSync` | `byte[4]` | Pad before holder; no runtime reads in this fn |
| `+0x164` | `pCachedTrackHolder` | `CBulanek *` | Disasm `MOV/CMP [ECX+0x164]` vs `[EAX+0x50]` |
| `+0x168` | `bField_168` | `byte` | Disasm `CMP byte [ECX+0x168],0` |

## Decomp before / after

| Issue | Before | After |
|-------|--------|-------|
| `this` type | `int *` / index `this[0x55]` | `CBulanek *this` |
| Cached param A | `pWalkTrackSources[2]` | `dwCachedWeaponParamA` |
| Cached param B | `pWalkTrackSources[3]` | `dwCachedWeaponParamB` |
| Cached holder | `*(CBulanek**)(pPad_postWalkEmbed+8)` | `pCachedTrackHolder` |
| Weapon local | `pCVar1` / wrong embed base | `pWeapon` from `this->pWeapon` |
| Seed guard | wrong field (`bDrawAsMuted` etc. after struct drift) | `bField_168` |

## Frida

**none** — static disasm + single caller closure sufficient.

## Remaining UNK

- Ghidra **`CWeapon::`** namespace prefix on `CBulanek_SyncWeaponWalkFromTrack` — cosmetic only.
- `_Globals::` prefix on `CBulanek_StepMovementAndCollision` callee — decompiler artifact.
- `pPad_weaponSync` @ `+0x15C..+0x163` — 8 B pad band; only `+0x164` read/written here; semantic name for `+0x160..+0x163` deferred.
