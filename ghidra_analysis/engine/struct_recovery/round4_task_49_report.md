# Round 4 — Task 49 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 49 |
| **round** | 4 (R3 verify + MCP fixes) |
| **title** | Verify R3 closure: Retype CWeapon_ctor this; name trackManager interior +0x08..+0x4F |
| **prior** | [round3_task_49_report.md](./round3_task_49_report.md) |
| **structs** | CWeapon, CDSVideoPlayer, CBulanek, CGaming |

## Status

**DONE** — R4 re-verified struct layout and decompiles; fixed `pOwner` type, ctor prototype, and `CWeapon_FirePistol` class/`this` typing.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CWeapon) == 0x70` | struct | `get_struct_layout CWeapon` → 112 B; `CDSVideoPlayer trackManager` @ +0x08 (72 B) |
| TM interior through +0x4F | struct | CDSVideoPlayer: scheduler +0x0C, trackVector +0x24, nCurrentTrackIdx +0x34, pRenderTarget +0x38, bPlayFlags/bPaused +0x39/+0x3A, dwCurrentFrameIdx +0x40, cookies +0x44/+0x48, nFrameDelayOverrideMs +0x4C |
| Ctor `CWeapon *this` | `0x0041dbc0` | `CWeapon::CWeapon_ctor(CWeapon*, CBulanek *pOwner, uchar weaponKind)`; `ConstructTrackManager(&trackManager)`; `(trackManager).pRenderTarget` |
| Fire pistol TM_Play | `0x0041fce0` | `CDSVideoPlayer::TM_Play(&this->trackManager,'\0')`; `this->pOwner` typed |
| Fire vfn | `0x004212b0` | `CWeapon::CWeapon_Fire(CWeapon *this,…)`; `this->pOwner` on spawn paths |
| TM_Play / ConstructTrackManager | `0x00439940`, `0x00439c70` | `CDSVideoPlayer::TM_Play` uses `bPaused`, `bPlayFlags`, `&scheduler` |
| Plate / ctor comment | `0x0041dbc0` | ECX=CWeapon* (not ODSImage*); embedded TM @ +0x08 |

### trackManager interior (CWeapon absolute)

| CWeapon abs. | TM rel. | Field |
|--------------|---------|-------|
| +0x08 | +0x00 | pVftable_IDSChained |
| +0x0C | +0x04 | scheduler |
| +0x24 | +0x1c | trackVector |
| +0x34 | +0x2c | nCurrentTrackIdx |
| +0x38 | +0x30 | pRenderTarget |
| +0x39 | +0x34 | bPlayFlags |
| +0x3A | +0x35 | bPaused |
| +0x40 | +0x38 | dwCurrentFrameIdx |
| +0x44 | +0x3c | pNotifyCookie |
| +0x48 | +0x40 | dwSchedulerCookie |
| +0x4C | +0x44 | nFrameDelayOverrideMs |

Tail: `pTrackHolder` +0x50, `pOwner` +0x54 (`CBulanek *`), `pWorld` +0x58, params +0x5C/60, `bWeaponKind` +0x64, HUD icons +0x68/6C.

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | CWeapon.pOwner | `CBulanek *` (was `-BAD-`) |
| `set_function_prototype` | `0x0041dbc0` | `CBulanek *pOwner`, `uchar weaponKind` |
| `set_function_prototype` | `0x0041fce0` | `__thiscall CWeapon_FirePistol(CWeapon *)` (was `__fastcall`) |
| `set_function_this_type` | `0x0041dbc0`, `0x0041fce0` | Confirmed in class `CWeapon` |
| `force_decompile` | ctor / Fire / FirePistol / TM helpers | Typed `trackManager`, `pOwner`, `pWorld` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CWeapon.md](./CWeapon.md) — R4 todo 49 verify log.

## Remaining UNK

- Ctor epilogue `this->pOwner = local_4` (SEH/stack artifact; param `pOwner` used for `pWorld` / `SetCurrentTrack`).
- `pTrackHolder` aliases `CBulanek *` in `CWeapon_Fire` mine/grenade paths — holder swap via `FUN_0041bf80`.
- Several track helpers still under `_Globals::` namespace (`AddTrackSource`, `SetCurrentTrack`, ammo helpers).
