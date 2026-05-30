# CWeapon

## Status

**VERIFIED** (size/layout + ctor decompile) — **`sizeof == 0x70` (112 B)** in Ghidra; heap object owned by **`CBulanek+0xF8`**. Ctor `CWeapon::CWeapon_ctor` @ `0x0041dbc0` — `CWeapon *this`, `CBulanek *pOwner`, `uchar weaponKind` (class namespace + `set_function_this_type`). Embedded **`CDSVideoPlayer trackManager` @ +0x08** (72 B, weapon abs. through **`+0x4F`**) via `ConstructTrackManager`; interior names match [CDSVideoPlayer.md](./CDSVideoPlayer.md).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CWeapon) == 0x70` | `CBulanekCtor` | `OperatorNewWithBadAlloc(0x70)` → `CWeapon_ctor` → `*(this+0xf8)` |
| Replace on pickup | `CBulanek_ApplyPickupEffect` | Same alloc; release via vtable `+8` |
| Tail fields | `CWeapon::Destructor` | Last pointers @ `+0x68`, `+0x6c` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CWeapon_ctor` → `g_pCWeapon_vftable` |
| `+0x04` | 4 | `pointer` | `vftable_secondary` | `CWeapon::vftable` |
| `+0x08` | `0x48` | `CDSVideoPlayer` | `trackManager` | `ConstructTrackManager(&this->trackManager)`; `TM_Play(&trackManager,0)` @ `CWeapon_FirePistol` |
| `+0x50` | 4 | `pointer` | `pTrackHolder` | `FUN_0041bf80`; `CWeapon_HideAssociatedView` |
| `+0x54` | 4 | `CBulanek *` | `pOwner` | `Fire`, `SetAmmo`, ctor `this[0x15]` |
| `+0x58` | 4 | `CGaming *` | `pWorld` | ctor from `CBulanek+0x84`; `CWeapon_FirePistol` |
| `+0x5C` | 4 | `uint` | `dwParamA` | `CWeapon_UpdateStateFromParams` |
| `+0x60` | 4 | `uint` | `dwParamB` | `Update` |
| `+0x64` | 1 | `byte` | `weaponKind` | ctor; `CBulanek_GetActiveWeaponKind` (`weapon+100`) |
| `+0x68` | 4 | `CDSObject *` | `pHudIconA` | ctor/dtor index `0x1a` |
| `+0x6C` | 4 | `CDSObject *` | `pHudIconB` | ctor/dtor index `0x1b` |

## Vtables

| `.rdata` | Role | Slot 4 |
|----------|------|--------|
| `0x00481ed4` | `IDSChained` (5) | **`Fire` @ `0x004212b0`** |
| `0x00481eec` | `IDSEventHandler` (4) | dtor / release |

Weapon defs: **`gAWeaponDefsTable` @ `0x00482820`**, stride **`0x1C`**, 6 kinds.

## Key methods

| Symbol | Address |
|--------|---------|
| `CWeapon::CWeapon_ctor` | `0x0041dbc0` |
| `CWeapon::Fire` | `0x004212b0` |
| `CWeapon_FirePistol` | `0x0041fce0` |
| `CWeapon_FireGrenade` | `0x0041fdf0` |
| `CBulanek_DispatchCurrentWeaponAction` | `0x00420650` |

## Ghidra apply

- **Agent todo 48 (2026-05-30):** `CWeapon_ctor` @ `0x0041dbc0` — renamed from `ODSImage::CWeapon_ctor`; moved symbol parent **ODSImage → Global** (inline script). Plate comment documents `CWeapon *this`. Zero `ODSImage::`-prefixed functions remain.
- **Agent todo 49 (2026-05-30, R3):** `set_function_this_type` `CWeapon*` @ `0x0041dbc0` — decompile `CWeapon::CWeapon_ctor` with `&this->trackManager`, `(trackManager).pRenderTarget`, tail fields. `CDSVideoPlayer` embed @ `+0x08`..`+0x4F` (interior table in `round3_task_49_report.md`). `set_function_this_type` + prototype on `TM_Play`/`ConstructTrackManager`; `CWeapon_FirePistol` → `TM_Play(&trackManager,0)`; `CWeapon::Fire` → `CWeapon *this`. `save_program bulanci.exe`.

## UNK

- `FUN_0041bf80` holder swap semantics (`pTrackHolder` vs `pOwner` alias in `Fire`).
- Ctor epilogue `pOwner = local_4` (decompile shows 0) — confirm disasm vs `pOwner` param.

## Upward links

- **`CBulanek.md`** `pWeapon` @ `+0xF8`
- **`combat_projectiles.md`** — `CShot` / `CMina` spawn from `Fire`
