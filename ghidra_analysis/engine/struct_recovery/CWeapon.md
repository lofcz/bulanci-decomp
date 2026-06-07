# CWeapon

## Status

**VERIFIED** (R10 task 9 re-proof) — **`sizeof == 0x70` (112 B)** via `get_struct_layout` + `OperatorNewWithBadAlloc(0x70)` xrefs @ `CBulanekCtor` / `CBulanek_ApplyPickupEffect`; tail `+0x50`..`+0x6C` closed by ctor disasm + `CWeapon_dtor@0x0041c550` / IDA `sub_41C550`. Heap object owned by **`CBulanek+0xF8`**. Ctor `CWeapon::CWeapon_ctor` @ `0x0041dbc0` — `CBulanek *pOwner`, `uchar weaponKind`, `uchar initialTrackIdx` (`__thiscall`, 4 stack params). Embedded **`CDSVideoPlayer trackManager` @ +0x08** (72 B) via `ConstructTrackManager`; interior names match [CDSVideoPlayer.md](./CDSVideoPlayer.md).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CWeapon) == 0x70` | `CBulanekCtor@0x0041e8ef` | `OperatorNewWithBadAlloc(0x70)` → `CWeapon_ctor`; stored `CBulanek+0xF8` — **Ghidra xrefs_to `0x0041dbc0` (2)** |
| Replace on pickup | `CBulanek_ApplyPickupEffect@0x0041eb0f` | Same `0x70` alloc; prior weapon released via `pVftable_primary+8` |
| Tail ends @ `+0x6C` | `CWeapon_ctor` disasm | Zero-init `+0x50/+0x5c/+0x60/+0x68/+0x6c`; last store before `SetCurrentTrack` @ `+0x6c` |
| Tail release order | `CWeapon_dtor@0x0041c550` | IDA: `this+20` (+0x50), `+26` (+0x68), `+27` (+0x6c); then `trackManager` dtor |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CWeapon_ctor` → `g_pCWeapon_vftable` |
| `+0x04` | 4 | `pointer` | `vftable_secondary` | `CWeapon::vftable` |
| `+0x08` | `0x48` | `CDSVideoPlayer` | `trackManager` | `ConstructTrackManager(&this->trackManager)`; `TM_Play(&trackManager,0)` @ `CWeapon_FirePistol` |
| `+0x50` | 4 | `CBulanek *` | `pTrackHolder` | `CWeapon_SetTrackHolder@0x0041bf80` — copy of `trackManager.pRenderTarget` (FLX consumer = owner host); `CWeapon_Fire` ammo/spawn reads |
| `+0x54` | 4 | `CBulanek *` | `pOwner` | `Fire`, `SetAmmo`, ctor `this[0x15]` |
| `+0x58` | 4 | `CGaming *` | `pWorld` | ctor from `CBulanek+0x84`; `CWeapon_FirePistol` |
| `+0x5C` | 4 | `uint` | `dwParamA` | `CWeapon_UpdateStateFromParams` |
| `+0x60` | 4 | `uint` | `dwParamB` | `Update` |
| `+0x64` | 1 | `byte` | `bWeaponKind` | ctor `MOV [ESI+0x64], AL` from `[ESP+0x34]` @ `0x0041dc2e` |
| `+0x65` | 3 | padding | `pPad_0x65` | No ctor/dtor/xref accesses; Ghidra padding to align `+0x68` |
| `+0x68` | 4 | `CDSObject *` | `pHudIconA` | ctor/dtor dword index `26` (+104); HUD still bitmap |
| `+0x6C` | 4 | `CDSObject *` | `pHudIconB` | ctor/dtor dword index `27` (+108); secondary bitmap optional |

## Vtables

| `.rdata` | Role | Slot 4 |
|----------|------|--------|
| `0x00481ed4` | `IDSChained` (5) | **`Fire` @ `0x004212b0`** |
| `0x00481eec` | `IDSEventHandler` (4) | dtor / release |

**R4 task 31 — `Fire` as FLX `0x0C` subscriber (`vfn[4]`):** `param_2` (`u16`) is weapon-kind-specific: kind `2` + `0` → `CMina`; kind `4` + not `0xFFFF` → `CShot` with kind `(param_2+3)`; kind `5` + `0` → pistol shot; **`0xFFFF`** → empty-ammo / bot branches. Not frame-delay ms.

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
- **R5 worker 26 (2026-05-30):** Asm-closed ctor `SetCurrentTrack` arg + `pOwner` store; `pTrackHolder` → `CBulanek*`; `FUN_0041bf80` → `CWeapon_SetTrackHolder`; `CDSVideoPlayer.trackVector` → `CDSTrackVector`. Report: [round5_worker_26_report.md](./round5_worker_26_report.md). `save_program bulanci.exe`.
- **R10 task 9 (2026-06-07):** Live MCP re-proof sizeof/tail; ctor prototype `+ initialTrackIdx` 4th param; asm distinguishes `[ESP+0x34]` weaponKind vs `[ESP+0x38]` SetCurrentTrack index. Report: [r10_deep_task_09_report.md](../deep_recovery/r10_deep_task_09_report.md). `save_program bulanci.exe`.

### `trackManager` interior — weapon runtime (R5 worker 26)

| CWeapon abs. | TM rel. | Field | Weapon evidence |
|--------------|---------|-------|-----------------|
| `+0x08` | `+0x00` | `pVftable_IDSChained` | `ConstructTrackManager@0x00439c70` |
| `+0x0C` | `+0x04` | `scheduler` | `CBulanek` event poll on `pWeapon->trackManager.scheduler` |
| `+0x24` | `+0x1c` | `trackVector` | Ctor: 4× `AddTrackSource` from `gAWeaponDefsTable[bWeaponKind]` |
| `+0x34` | `+0x2c` | `nCurrentTrackIdx` | `SetCurrentTrack(&trackManager, initialTrackIdx, 0)` — **asm** `MOVZX` from `[ESP+0x38]` @ `0x0041dd2a` (4th ctor param) |
| `+0x38` | `+0x30` | `pRenderTarget` | Active-track FLX consumer; passed to `CWeapon_SetTrackHolder` @ `0x0041dd3c` |
| `+0x39` / `+0x3A` | `+0x34` / `+0x35` | `bPlayFlags` / `bPaused` | `TM_Play(&trackManager, 0)` @ `CWeapon_FirePistol` |
| `+0x40` | `+0x38` | `dwCurrentFrameIdx` | `BeginCurrentTrackPlayback` after ctor |
| `+0x44` / `+0x48` / `+0x4C` | `+0x3c` / `+0x40` / `+0x44` | cookies / delay | `ConstructTrackManager` defaults only |

**`pTrackHolder` vs `pOwner`:** `pTrackHolder` (`+0x50`) mirrors `trackManager.pRenderTarget` and is the **`CBulanek*`** used in `CWeapon_Fire` for aim/ammo (`[ESI+0x50]` + player offsets). `pOwner` (`+0x54`) is the ctor `pOwner` param (`MOV [ESI+0x54], EAX` @ `0x0041dd52`) used for `AddEntity` / world dispatch.

## UNK

- `SetCurrentTrack` decompile may still show `(uint)pOwner & 0xff`; **asm @ `0x0041dd2a` uses `[ESP+0x38]` = `initialTrackIdx` (4th ctor param)**, distinct from `weaponKind` @ `[ESP+0x34]` → `+0x64` (IDA `sub_439EB0(this+8,a4,0)`).
- `CBulanekCtor` Ghidra call may still label 3rd ctor arg `skinPaletteId`; IDA passes separate `a9` weapon-kind param (`sub_41DBC0(v20,this,a9,a5)`).
- MFC factory `CreateObject@0x0041a710` alloc `0x70` → `sub_418EF0` is a **partial** stub (not gameplay weapon); full init only via `CWeapon_ctor@0x0041dbc0`.
- Only **4** tracks loaded per weapon; `SetCurrentTrack` initial index from caller (`a5` / pickup `this+0xD4`) — bounds not checked in `SetCurrentTrack@0x00439eb0`.

## Upward links

- **`CBulanek.md`** `pWeapon` @ `+0xF8`
- **`combat_projectiles.md`** — `CShot` / `CMina` spawn from `Fire`
