# CGameView

## Status

**PARTIAL** — **`0x98` (152 B)** shell proven (`CGameView_ctor` @ `0x004191a0`, vtables patched to **`CBitmap`**). **`0x88` (136 B)** partial shell via `CGameView_CreateObject` @ `0x00418c60` (no `ODSImage_ctor`). **`CBulanek`** shares the header through **`+0x87`** (`0x88`..`0x8b`); tail from **`+0x88`** is **`CDSUpdatedItem`** / player fields (`CBulanek.md`, `0x19c`).

Ghidra **`CGameView` 152 B** + **`CGameView *`**: header **`+0x00..+0x87`** aligned with **`CDSChained`** / **`CBulanek`** prefix. **`pGaming_host`** @ **`+0x84`** is **`CGaming *`** on the view shell; **`CBulanek`** reuses the offset as **`pGamingHostScratch`** during ctor only.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x98` | `0x0041d820` | `CLevelScriptOpExt_CreateImage`: `OperatorNew(0x98)` → `CGameView_ctor` |
| `sizeof == 0x88` | `0x00418c60` | `CGameView_CreateObject`: `OperatorNew(0x88)` — no `ODSImage_ctor` |
| `sizeof == 0xB0` | `0x00417030` | `CShot_SubobjectCtor` — shot shell extends header |
| `sizeof == 0x19c` | spawn path | **`CBulanek`** child (`CBulanek.md`) |

## Layout table (`+0x00`–`+0x97`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CGameView_ctor@0x004191a0` → `CBitmap` primary `0x00481f74` |
| `+0x04` | 4 | `pointer` | `vftable_IDSChained` | ctor |
| `+0x08` | 4 | `uint` | `dwField_08` | `CDSChained_ctor@0x00403309` `MOV [ESI+0x8],0` |
| `+0x0C` | 4 | `uint` | `dwField_0c` | `CDSChained_ctor@0x0040330c` `MOV [ESI+0xc],0` |
| `+0x10` | 4 | `pointer` | `vftable_IDSReferenced` | ctor |
| `+0x14` | 2 | `ushort` | `wViewFlags` | `CBulanekCtor@0x0041e4b0` `\|= 0x200`; `CDSView_SetRect` band |
| `+0x16` | 2 | `ushort` | `wPad_16` | padding before `+0x18` vtable |
| `+0x18` | 4 | `pointer` | `vftable_IDSEventHandler` | ctor |
| `+0x1C` | 4 | `uint` | `dwField_1c` | `CDSChained_ctor@0x0040331d` zero |
| `+0x20` | 4 | `int` | `nOrigin_x` | ctor; `GetWorldCollisionRect@0x00417210` adds origin |
| `+0x24` | 4 | `int` | `nOrigin_y` | same |
| `+0x28` | 4 | `int` | `nSpatial_bucket_x` | spatial grid X |
| `+0x2C` | 4 | `int` | `nSpatial_bucket_y` | `GetSpatialBucketKey@0x00416f50` summand |
| `+0x30` | 16 | `int[4]` | `nBounds_*` | `CDSView_SetRect@0x0042c480` (`left/top/right/bottom`) |
| `+0x40` | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters@0x0042bec4` `MOV [ECX+0x40],0` |
| `+0x44` | 2 | `ushort` | `wChainInit44` | `ResetChainCounters@0x0042beb2` `MOV word [ECX+0x44],1` |
| `+0x46` | 2 | `ushort` | `wChainFlag46` | `ResetChainCounters@0x0042beb8` zero |
| `+0x48` | 2 | `ushort` | `wChainFlag48` | `ResetChainCounters@0x0042bebc` zero |
| `+0x4A` | 2 | `ushort` | `wChainFlag4a` | `ResetChainCounters@0x0042bec0` zero |
| `+0x4C` | 4 | `pointer` | `pChainParent` | `ResetChainCounters@0x0042beca` `MOV [ECX+0x4c],0` |
| `+0x50` | 4 | `uint` | `dwChainField_50` | `ResetChainCounters@0x0042bec7` `MOV [ECX+0x50],0` |
| `+0x54` | 4 | `pointer` | `pVftable_embeddedChain_IDSReferenced` | `CDSChained_ctor@0x00403352` → `0x47f6d4`; `GetFirstChildView` ECX=`this+0x54` |
| `+0x58` | 4 | `pointer` | `pVftable_embeddedChain_IDSChained` | `CDSChained_ctor@0x00403359` → `0x47f6b8` |
| `+0x5C` | 4 | `uint` | `dwEmbeddedChainField_5c` | `CDSChained_ctor@0x00403360` zero |
| `+0x60` | 4 | `uint` | `dwEmbeddedChainField_60` | `CDSChained_ctor@0x00403363` zero |
| `+0x64` | 4 | `pointer` | `pOverlapEntity` | ctor zero; `CBulanek_UpdateStateFromParams@0x004178f7` `MOV [ECX+0x64],…` |
| `+0x68` | 1 | `byte` | `bActiveInWorld` | `InitGamingFields@0x00416592`; `Update@0x00419010`; `UpdateStateFromParams` dword @ `+0x68` |
| `+0x69` | 1 | `byte` | `bSkipSpatialWhenNonZero` | `InitGamingFields`; spatial query (`CBulanek.md`) |
| `+0x6A` | 1 | `byte` | `bView_state_6a` | `InitGamingFields` |
| `+0x6B` | 1 | `byte` | `bNetStateByte3` | high byte of dword written @ `+0x68` by `UpdateStateFromParams@0x004178fd` |
| `+0x6C` | 4 | `uint` | `dwView_aux_6c` | `GetSpatialBucketKey` — not cleared in `InitGamingFields` |
| `+0x70` | 1 | `byte` | `bPlayerSlot` | `InitGamingFields` `=0xff`; `OnEvent@0x0041acf0` slot events |
| `+0x71` | 3 | `byte[3]` | `pPad_71` | no `[reg+0x71]` store/load in gameplay `.text` (R5 w23 sweep) |
| `+0x74` | 4 | `int` | `nCollisionLeft` | ctor `=0`; `GetWorldCollisionRect`; `CBulanekCtor` anim bounds |
| `+0x78` | 4 | `int` | `nCollisionTop` | same |
| `+0x7C` | 4 | `int` | `nCollisionRight` | same |
| `+0x80` | 4 | `int` | `nCollisionBottom` | same |
| `+0x84` | 4 | `CGaming *` | `pGaming_host` | `InitGamingFields` `=0`; `Update` / `OnEvent` |
| `+0x88` | 16 | `ODSImage` | `ods_image` | `ODSImage_ctor` in full ctor; **`CBulanek`** → `scheduler` |

### Collision gate (`+0x44`)

`GetWorldCollisionRect@0x00417210` uses **`TEST byte ptr [ECX+0x44], 1`** — low byte of **`wChainInit44`**, not a separate `dwView_flags` dword. **`TM_SetFrameDelayOverrideMs@0x00439724`** writes **`[ECX+0x44]`** as a full dword but **`CBulanekCtor`** passes **`ECX = &videoTrackManager` (+0xa8)** → **`CDSObject.nFrameDelayOverrideMs`**, not the view chain band ([CDSObject.md](./CDSObject.md) `+0x44`).

## CBulanek prefix overlap (`+0x00`–`+0x87`)

| Offset | `CGameView` | `CBulanek` | Notes |
|--------|-------------|------------|-------|
| `+0x14` | `wViewFlags` | `wViewFlags` | Player ctor `\|= 0x200` |
| `+0x28..+0x3F` | `nSpatial_bucket_*`, `nBounds_*` | same names | Bucket key: `dwView_aux_6c + nSpatial_bucket_y` |
| `+0x40..+0x50` | CDS chain band | same | See [CDSChained.md](./CDSChained.md) |
| `+0x54..+0x64` | embedded `CDSChain` facet | same | `GetFirstChildView` / `pOverlapEntity` |
| `+0x68..+0x6B` | gaming bytes | same | `UpdateStateFromParams` packs net dword @ `+0x68` |
| `+0x70` | `bPlayerSlot` | `bPlayerSlot` | Was `bGaming_slot_id` on view shell |
| `+0x74..+0x80` | `nCollisionLeft/Top/Right/Bottom` | same | World AABB before origin add |
| `+0x84` | `pGaming_host` (`CGaming *`) | `pGamingHostScratch` (`void *`) | Live host vs ctor scratch |

## Key methods

| Symbol | Address | `this` |
|--------|---------|--------|
| `CGameView_InitGamingFields` | `0x00416590` | `CGameView *` (`__fastcall` ECX) |
| `CGameView_GetSpatialBucketKey` | `0x00416f50` | `CGameView *` |
| `CGameView_GetWorldCollisionRect` | `0x00417210` | `CGameView *` |
| `CGameView_Update` | `0x00419010` | `CGameView *` |
| `CGameView_ctor` | `0x004191a0` | `CGameView *` |
| `CGameView_CreateObject` | `0x00418c60` | factory (no ECX) |
| `CGameView_OnEvent` | `0x0041acf0` | `CGameView *` |
| `CBitmap_OnDraw` | `0x00419130` | `CGameView *` → `TM_TickBlit(&ods_image)` |
| `TM_SetFrameDelayOverrideMs` | `0x00439720` | **`CDSObject` / track-mgr facet** when called from `CBulanekCtor` (`ECX = this+0xa8`) |
| `CBitmap_FireOnBitmapEvtFromView` | `0x00419280` | `CGameView *` (IDSAnim facet @ `+0x8c`) |
| `CBitmap_ViewHeader_Init` | `0x00419070` | shared init; decompiler may show `CAnim *` when called from `CAnim_ctor` |
| `CBulanek_UpdateStateFromParams` | `0x004178f0` | `CBulanek *` — `+0x64` overlap, `+0x68` packed state |

## Ghidra apply

**R3 todo 4** — `set_function_this_type` **`CGameView *`** @ `InitGamingFields`, `GetSpatialBucketKey`, `OnEvent` ([round3_task_04_report.md](./round3_task_04_report.md)).

**R4 todo 4** — `pGaming_host` → **`CGaming *`** ([round4_task_04_report.md](./round4_task_04_report.md)).

**Pass R4 CGameView (2026-05-30):** Renamed header/collision/gaming fields to match **`CBulanek`** prefix; report [pass_r4_CGameView_report.md](./pass_r4_CGameView_report.md).

**R5 worker 23 (2026-05-30):** Resolved UNK band `+0x08..+0x64` from `CDSChained_ctor` / `ResetChainCounters` / `GetWorldCollisionRect` / `UpdateStateFromParams` disasm; split chain band; renamed `bNetStateByte3`; `save_program bulanci.exe`. Report: [round5_worker_23_report.md](./round5_worker_23_report.md).

## UNK

- `dwField_08`, `dwField_0c`, `dwField_1c` — ctor zero only (shared with [CDSChained.md](./CDSChained.md)).
- `dwEmbeddedChainField_5c`, `dwEmbeddedChainField_60` — ctor zero; no gameplay consumer on `CGameView` shell.
- `pPad_71` — alignment before collision ints; no direct `.text` xrefs on `CGameView`-sized objects.
- `wChainFlag46` / `wChainFlag48` / `wChainFlag4a` bit semantics beyond `ResetChainCounters` zero init.
- `CLevelScript_FireOnSlotPlaced_FromView` / `CGameView_FireOnSlotDisplaced` still take `(CGameView *)` where first arg should be `CGaming *`.
- `CBitmap_ViewHeader_Init` — dual `CAnim` / `CGameView` caller; left as `CAnim *` when entered from `CAnim_ctor`.

## Follow-up

- Retype `TM_SetFrameDelayOverrideMs` → `CDSObject *` / `CDSVideoPlayer *` when ECX is track manager.
- Plate `CDSView_SetRect` with `CDSView *` / `CGameView *` instead of `CBulanci *` cast at `Update` call sites.
- `CGameView_CreateObject` factory `this` typing when shell-only path is owned.
