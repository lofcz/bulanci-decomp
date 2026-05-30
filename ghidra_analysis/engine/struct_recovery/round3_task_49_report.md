# Round 3 — Task 49 report (CWeapon)

## Task

| Field | Value |
|-------|-------|
| **id** | 49 |
| **title** | Retype CWeapon_ctor this; name trackManager interior +0x08..+0x4F |
| **priority** | blocker |
| **types** | `CWeapon`, `CBulanek`, `CDSVideoPlayer`, `CGaming` |
| **acceptance** | `CWeapon *this` on ctor; `trackManager` `CDSVideoPlayer` embed with interior names; `CWeapon.md` + jsonl |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CWeapon) == 0x70` | `CBulanekCtor` | `OperatorNew(0x70)` → `CWeapon_ctor` → store @ `CBulanek+0xF8` |
| `trackManager` @ `+0x08`, 72 B | Ghidra `get_struct_layout CWeapon` | `CDSVideoPlayer trackManager` @ 8..79; tail `pTrackHolder` @ 80 |
| Ctor `this` typed | `0x0041dbc0` | `void __thiscall CWeapon::CWeapon_ctor(CWeapon *this, CBulanek *pOwner, uchar weaponKind)` |
| Plate comment | `0x0041dbc0` | ECX=`CWeapon*`; embedded TM @ `+0x08`; 4× `AddTrackSource` from `gAWeaponDefsTable` |
| TM init | `0x0041dbc0` | `ConstructTrackManager(&this->trackManager, …)` |
| Interior through `+0x4F` | ctor + `ConstructTrackManager` | Weapon abs. = `+0x08` + CDSVideoPlayer rel. (see table below) |
| Fire pistol uses TM | `0x0041fce0` | `TM_Play(&param_1->trackManager, 0)` |
| Fire vfn typed | `0x004212b0` | `void __thiscall CWeapon::CWeapon_Fire(CWeapon *this, …)` |
| `TM_Play` fields | `0x00439940` | `this->bPaused`, `this->bPlayFlags`, `&this->scheduler` after `set_function_this_type` |

### `trackManager` interior map (CWeapon absolute offsets)

| CWeapon abs. | TM rel. | Field | Ctor / helper |
|--------------|---------|-------|----------------|
| `+0x08` | `+0x00` | `pVftable_IDSChained` | `ConstructTrackManager` → `0x487788` |
| `+0x0C` | `+0x04` | `scheduler` (`CDSUpdatedItem`) | `CDSUpdatedItem_ctor(&this->scheduler)` |
| `+0x24` | `+0x1c` | `trackVector` | zero + `dwTrackCapacity=8` |
| `+0x34` | `+0x2c` | `nCurrentTrackIdx` | `=-1` |
| `+0x38` | `+0x30` | `pRenderTarget` | ctor passes to `FUN_0041bf80` |
| `+0x39` | `+0x34` | `bPlayFlags` | `TM_Play` |
| `+0x3A` | `+0x35` | `bPaused` | `ConstructTrackManager`; `TM_Play` |
| `+0x40` | `+0x38` | `dwCurrentFrameIdx` | — |
| `+0x44` | `+0x3c` | `pNotifyCookie` | `=0` |
| `+0x48` | `+0x40` | `dwSchedulerCookie` | `=0xffffffff` |
| `+0x4C` | `+0x44` | `nFrameDelayOverrideMs` | `=-1` |

Tail (outside TM band): `pTrackHolder` `+0x50`, `pOwner` `+0x54`, `pWorld` `+0x58`, `dwParamA/B` `+0x5C/60`, `bWeaponKind` `+0x64`, HUD icons `+0x68/6C`.

## Ghidra deltas

- `set_function_this_type` `CWeapon *` @ `CWeapon_ctor` (`0x0041dbc0`) — confirmed in class `CWeapon`.
- `set_function_this_type` `CDSVideoPlayer *` @ `TM_Play` (`0x00439940`), `ConstructTrackManager` (`0x00439c70`).
- `set_function_prototype` `TM_Play(CDSVideoPlayer *, uchar)`, `ConstructTrackManager(CDSVideoPlayer *, int)`.
- Struct already applied: `CWeapon.trackManager` (`CDSVideoPlayer`, 72 B @ `+0x08`).
- `save_program bulanci.exe`.

## Struct doc updates

- `ghidra_analysis/engine/struct_recovery/CWeapon.md` — status/decompile notes; key-methods table; Ghidra apply section.

## Remaining UNK

- Ctor epilogue stores `pOwner` from `local_4` (always 0 in decompile) — likely `pOwner` param not wired in SEH block; verify disasm if gameplay depends on immediate `pOwner`.
- `FUN_0041bf80` holder swap vs `pTrackHolder` / `pOwner` alias in `CWeapon_Fire`.
- `ConstructTrackManager` still listed under `CDSObject::` in some xrefs; behavior matches `CDSVideoPlayer` layout.
