# CBulanek

## Status

**PARTIAL** — heap size **`0x19c` (412 B)** and ctor/subobject graph proven (`CBulanekCtor` @ `0x0041e4b0`). Inherits **`CGameView`** prefix through `+0x87`; player-specific tail from `+0x88` (`CDSUpdatedItem`, track manager, weapon, combat state). Ghidra type **`CBulanek`** is undersized/incomplete (412 B declared but large `pad` gaps); use this doc + `CGameView.md` for the header.

**Spelling:** binary / Ghidra use **`CBulanek`**; reimplementation headers alias ctor as `CGameView::CBulanekCtor` (`include/bulanci/CGameView.h`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CBulanek) == 0x19c` | `0x00484224` region | `OperatorNewWithBadAlloc(0x19c)` before `CBulanekCtor` (`bulanci.ghidra.exe.c` spawn path; comment @ `0x00488755`) |
| ctor | `0x0041e4b0` | `CGameView::CBulanekCtor` — `CDSChained_ctor` → `CDSUpdatedItem_ctor(this+0x88)` → `ConstructTrackManager(this+0xa8)` |
| dtor scalar | `0x0041bc20` | `CBulanek_DtorScalar` |
| Class vtables | `master_vtable_catalog.csv` | Primary `0x00481e54`; facets @ `+0x04`, `+0x10`, `+0x18`, `+0x88`, `+0xa0` |

## Inheritance / vtables

```
CDSChained / CDSView header (+0x00..+0x87)  [see CGameView.md]
  +0x88  CDSUpdatedItem (scheduler facet)
  +0xA0  IDSEventHandler / ODSImage event vtable (CBulanek::g_pCBulanek_vftable_event)
  +0xA8  CDSVideoPlayer track manager (walk / pain / weapon anims)
  +0xF0  CBulAnim* (active drawable)
  +0xF8  CWeapon* (heap 0x70)
```

| Object offset | Vtable label | `.rdata` |
|---------------|--------------|----------|
| `+0x00` | primary (`CDSView` gameplay) | `0x00481e54` |
| `+0x04` | `IDSChained` | `0x00481e38` |
| `+0x10` | `IDSReferenced` (CDSView-sized facet) | `CBulanek::g_pCBulanek_vftable_IDSChained_a0` |
| `+0x18` | `IDSEventHandler` | `0x00481e20` |
| `+0x88` | `IDSUpdated` / scheduler | `0x00481df4` |
| `+0xA0` | event / ODSImage dispatch | `0x00481ddc` |

## Layout table

### CGameView prefix (`+0x00`–`+0x87`)

Shared with `CGameView.md` / `CBitmap.md` — only player-relevant highlights:

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x14` | 2 | `ushort` | `wViewFlags` | `CBulanekCtor@0x0041e4b0` `\|= 0x200` |
| `+0x20` | 4 | `int` | `origin_x` | ctor; `CBulanek_ApplyAction@0x00420910` net coords `pCdsViewPrefix+0x20` |
| `+0x24` | 4 | `int` | `origin_y` | same |
| `+0x44` | 4 | `uint` | `view_flags` | `TM_SetFrameDelayOverrideMs` on `this+0xa8` track mgr (`status.md` anim batch) |
| `+0x69` | 1 | `byte` | `bActiveInWorld` | ctor `this[0x69]=1`; `damage_pipeline.md` / `SpatialQuery` |
| `+0x6A` | 1 | `byte` | `bSkipSpatialWhenNonZero` | `FUN_00416590`; spatial query `param_5` |
| `+0x70` | 1 | `byte` | `bPlayerSlot` | `FUN_00416590`; `CBulanek_ApplyAction` → `CGame_NetSendPlayerState_t0d(..., bStateByte, …)` |
| `+0x74` | 4 | `int` | `nCollisionField74` | ctor from anim bounds |
| `+0x78` | 4 | `int` | `nCollisionField78` | ctor `= 0x19` |
| `+0x7C` | 4 | `int` | `nCollisionLeft` | ctor from `piVar8[1]-2` |
| `+0x80` | 4 | `int` | `nCollisionBottom` | ctor from `piVar8[2]` |
| `+0x84` | 4 | `void *` | `pGamingHostScratch` | ctor stores `param_4` then cleared after weapon create |

### Player extension (`+0x88`–`+0x19b`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x88` | 24 | `CDSUpdatedItem` | `scheduler` | `CDSUpdatedItem_ctor@0x0041e4b0`; `Scheduler_RegisterEventSlot(this+0x88,…)` |
| `+0xA0` | 4 | `pointer` | `vftable_event` | ctor `g_pCBulanek_vftable_event` |
| `+0xA4` | 4 | `uint` | `nFacingAxisExtent` | ctor `= 0`; `CBulanek_AdaptDisplaySize@0x00417615` stores walk-axis extent from track bounds; `CBulanek_OffsetCollisionRectByFacing@0x00417465` adds to collision rect; `CBulanek_SnapPositionToFacingAxis@0x00417910` writes origin axis when track idx `> 1` |
| `+0xA8` | 72 | `CDSVideoPlayer` | `videoTrackManager` | `ConstructTrackManager(this+0xa8)`; `CBulanek_ApplyAction` → `BeginCurrentTrackPlayback(&videoTrackManager)` |
| `+0xF0` | 4 | `CBulAnim *` | `pActiveAnim` | ctor assigns `piVar8`; death uses `+0xFC` corpse anim (`damage_pipeline.md`) |
| `+0xF4` | 4 | `CGame *` | `pGame` | ctor `*(this+0xf4)=param_2`; `CBulanek_ApplyAction` net send |
| `+0xF8` | 4 | `CWeapon *` | `pWeapon` | `OperatorNew(0x70)` + `CWeapon_ctor`; `CBulanek_GetActiveWeaponKind` reads `weapon+0x64` |
| `+0xFC` | 4 | `CBulAnim *` | `pCorpseAnim` | `damage_pipeline.md` |
| `+0x104` | 24 | `byte[24]` | `abReserved_preAmmo` | ctor write-only zero band before ammo bytes; no runtime reads (`CBulanekCtor@0x0041e4b0`) |
| `+0x11C` | 1 | `byte` | `bAmmoKind0` | ctor `GetMaxAmmoForKind(0)` |
| `+0x11D` | 1 | `byte` | `bAmmoKind1` | ctor if `(byte)param_3 > 4` |
| `+0x120` | 2 | `ushort` | `wField_120` | ctor `= 0`; OnDeath clears bytes `+0x121..+0x125` (weapon display band) |
| `+0x12C` | 16 | `int[4]` | `anOpponentScriptTally` | ctor zero; `CBulanek_OnEvent` case `0xD8` increments `[slot]` for human slots `<4` |
| `+0x123` | 1 | `byte` | `bTeamColor` | `CGameGetPlayerColorByte` / AI random; special variants `= 7` |
| `+0x124` | 1 | `byte` | `bSlotKind` | ctor `param_3` low byte; AI remap |
| `+0x128` | 4 | `uint` | `nScriptKillScore` | ctor `= 0`; `CBulanek_OnEvent@0x00420deb` case `0xD8` `++`; `CGaming_OnScriptKillScore@0x004180e0` copies to score block + scans max for mode 0 leader HUD |
| `+0x13C` | 4 | `uint` | `nDeathCount` | ctor `= 0`; `CBulanek_OnDeath@0x0041f9b8` `++`; `CGaming_TickRoundStateAndScoring@0x0041f386` reads (last-man mode 2 picks minimum) |
| `+0x140` | 4 | `int` | `nDrawAlphaPercent` | ctor `= 100`; `CBulanek_Draw@0x00417694` tints when `!= 100`; `CGaming_RespawnPlayer@0x0041f8bd` `= 0` (coop slots); `CBulanek_OnDeath@0x0041f94c` `= 100` |
| `+0x144` | 1 | `byte` | `bPendingKillerSlot` | ctor `= 0xff`; `CBulanek_OnDeath@0x0041f99f` `= killerSlot`; `CBulanek_OnEvent@0x00420ea2` read / restore `0xff` after delayed quip posts script `0xdb` |
| `+0x148` | 4 | `int` | `nInitialTrack` | ctor `param_5` → `SetCurrentTrack` |
| `+0x14C` | 16 | `pointer[4]` | `apWalkTrackSources` | ctor `AddTrackSource` loop; `param_2` → `gABulanekWalkAnimsNormal` / `Special` |
| `+0x168` | 1 | `byte` | `bField_168` | ctor `= 0` |
| `+0x169` | 1 | `byte` | `bField_169` | ctor `= 0` |
| `+0x16A` | 1 | `byte` | `bHitStun` | `CBulanek_OnTakeDamage`; `damage_pipeline.md` |
| `+0x16B` | 1 | `byte` | `bShotgunPickupGuard` | `damage_pipeline.md` |
| `+0x16C` | 4 | `void *` | `pDeathVoicePlayer` | `CBulanek_OnDeath` stores `TriggerBankSample` player; event `1` compares `lParam`; dtor releases @ `param_1+0x5b` |
| `+0x178` | 16 | `pointer[4]` | `apAiTrackHolders` | ctor AI loop `CBulanek_AllocAiTrackHolder@0x0041b420` when `(byte)param_3 > 3` |
| `+0x188` | 4 | `uint` | `dwPickupFacingLatch` | ctor `= 0`; `CBulanek_OnEvent@0x00421060` case `0xF2` stores facing latch; `0xF3` clears; non-zero blocks re-entry |
| `+0x18C` | 4 | `int` | `nLivesRemaining` | ctor `param_7`; script `0xD7` decrement (`damage_pipeline.md`) |
| `+0x190` | 4 | `int` | `nLivesSnapshot` | ctor `= param_7` (lives at spawn); `CBulanek_OnDeath@0x0041f926` copied into `nLivesRemaining` before corpse path |
| `+0x194` | 4 | `int` | `nSpeedParam` | ctor clamp `[0x28,200]`; `TM_SetFrameDelayOverrideMs` when `!= 100` |
| `+0x198` | 1 | `byte` | `bDrawAsMuted` | ctor `= 0`; `CBulanek_OnEvent@0x00420f8e`/`0xEF` sets/clears; `CBulanek_Draw@0x0041764a` gray `0x303030` when set; `CGaming_RetestTraceAreasForEntity` on toggle |
| `+0x199` | 1 | `byte` | `bIsAiVariant` | ctor `3 < (byte)param_3` |
| `+0x19A` | 1 | `byte` | `bScriptDeathGuard` | ctor `= 0`; `CBulanek_OnEvent@0x00420d80`/`0xEF` case `0xEE`/`0xEF` sub `1` arms guard; blocks script `0xD7` death while non-zero |

## Key methods (factory / gameplay)

| Symbol | Address | Role |
|--------|---------|------|
| `CBulanekCtor` | `0x0041e4b0` | Per-slot player construct (from `CGaming` spawn loop) |
| `CBulanek_ApplyAction` | `0x00420910` | Input actions 0..5 → facing / fire / weapon cycle |
| `CBulanek_OnTakeDamage` | `0x0041db00` | Teleport-gate respawn pain path |
| `CBulanek_OnDeath` | `0x0041f900` | Corpse / tournament branch |
| `CBulanek_OnEvent` | `0x00420d40` | Script dispatcher (`0xD7` life loss) |
| `CBulanek_SchedulerTick` | `0x0041aed0` | Per-player scheduler cases |
| `CBulanek_SpawnPlayerAndCampaignSlots` | `0x004205a0` | Match start entity spawn |
| `CBulanek_SetFacingTrack` | `0x004197b0` | Movement facing → walk tracks |
| `CBulanek_TriggerPrimaryActionAndBroadcast` | (via `_Globals`) | Fire / weapon dispatch |

## Ghidra apply

- **Agent todo 41 (2026-05-30):** Rebuilt **`CBulanek` 412 B** — replaced `pCdsViewPrefix` blob with flat **`CGameView` mirror @ +0x00..+0x87`** (`vftable_*`, `nOrigin_*`, `view_flags`, `bActiveInWorld` @ +0x69, `bPlayerSlot` @ +0x70, collision ints, `pGamingHostScratch`); tail renames **`vftable_event`**, **`apWalkTrackSources`**, **`apAiTrackHolders`**; split **`pDeath2Tombstone` @ +0x100** + **`pPad_preAmmoBand` @ +0x104..+0x11B**; **`pHitQuipPlayer` / `pDelayedQuipPlayer` @ +0x170/+0x174** (audio quip xrefs in `CBulanek_OnEvent`); `save_program`.
- `create_struct CBulanek` size **412** (`0x19c`); prefix fields mirror `CGameView` through `+0x84`.
- Do **not** embed full `CGame` inside `CBulanek`; `pGame` is a pointer at `+0xF4`.
- **`scheduler`** (`CDSUpdatedItem`, 24 B) @ **`+0x88`** — `CDSUpdatedItem_ctor` + host vtable `0x481df4` @ embed `+0`; **`Scheduler_RegisterEventSlot(&scheduler, …)`** in ctor (`0x0041ea18`..`0x0041ea6a`, eventKind **7**); `CBulanek_SchedulerTick` walks slots **0..5** via `&this->scheduler` (agent todo **41**, 2026-05-30).
- **`videoTrackManager`** (`CDSVideoPlayer`, 72 B) @ **`+0xA8`** — verified `get_struct_layout` offset 168; ctor `ConstructTrackManager(this+0xa8)`; `CBulanek_ApplyAction` action 0 → `BeginCurrentTrackPlayback(&videoTrackManager)` (agent todo **42**, 2026-05-30).
- `CBulanekCtor@0x0041e4b0` → `CBulanek * __thiscall CBulanekCtor(CBulanek *this, …)`; decompiler may still show `ods_image` @ +0x88 when typed via `CGameView` parent — use **`scheduler`** field on `CBulanek`.
- `get_struct_layout CBulanek` → **412** bytes with `scheduler` @ +0x88 and `videoTrackManager` @ +0xA8.
- Batch end: `save_program bulanci.exe`.
- **R4 todo 42 (2026-05-30):** Re-verified `set_function_this_type(CBulanek *)` @ `CBulanekCtor@0x0041e4b0`; decompile uses `&this->scheduler` / `&this->videoTrackManager`; asm `LEA [ESI+0x88]` / `LEA [ESI+0xa8]` in ctor; `CBulanek_ApplyAction@0x00420910` → `BeginCurrentTrackPlayback(&videoTrackManager)`; `save_program`.
- **R5 worker 02 (2026-05-30):** `CBulanek_OnEvent_tD9_SyncWeaponFromSpawner@0x0041ed10` (event `0xD9`); `CBulanek_RefreshHudWeaponStrip@0x00416700`; `CBulanek_ArmTournamentSchedulerDelays@0x004172d0`; [round5_worker_02_report.md](./round5_worker_02_report.md).

**R5 worker 03 (2026-05-30):** Slice `0x00440000`–`0x00460000` — proven CRT deps **`OperatorNewWithBadAlloc@0x00447c42`** (`0x19c` player @ `CGaming_SpawnAndInitializePlayer`, `0x70` weapon @ ctor `0x0041e8cd`) and **`_rand@0x004477ec`** (AI color, walk table, bot `TryBotRandomAction`, fire-delay scheduler, spawn facing); plate + call-site comments; renamed **`CBulanek_AllocAiTrackHolder@0x0041b420`**; blit/MPX sub-ranges deferred; [round5_worker_03_report.md](./round5_worker_03_report.md).
- **R4 todo 41 (2026-05-30):** Named **`pDeathVoicePlayer` @ +0x16C** (was `dwField_16C`); **`anOpponentScriptTally` @ +0x12C** (`int[4]`, script event `0xD8`); **`abReserved_preAmmo` @ +0x104..+0x11B** (ctor-only zeros); restored **`pHitQuipPlayer` / `pDelayedQuipPlayer`** @ +0x170/+0x174; plate/decompiler comments on ctor/OnEvent/OnDeath; `save_program`.
- **R5 worker 21 (2026-05-30):** Closed tail UNK band — renamed **`nFacingAxisExtent`**, **`nScriptKillScore`**, **`nDeathCount`**, **`nDrawAlphaPercent`**, **`bPendingKillerSlot`**, **`dwPickupFacingLatch`**, **`nLivesSnapshot`**, **`bDrawAsMuted`**, **`bScriptDeathGuard`**; `set_function_prototype` + `set_function_this_type` on **`CBulanek_Draw@0x00417640`**, **`CBulanek_SnapPositionToFacingAxis@0x00417910`**, **`CGaming_OnScriptKillScore@0x004180e0`**; `save_program`.

## UNK

- `+0x08..+0x13`, `+0x28..+0x2F`, `+0x40..+0x43`, `+0x48..+0x67`, `+0x6B..+0x6F`, `+0x71..+0x73`: CDSView mirror gaps (see `CGameView.md`).
- `+0x9C..+0xEF` inside track-manager / anim subgraph (defer to `CDSObject` track batch).
- Ghidra layout export may still prefix pointer arrays as `pWalkTrackSources` / `pAiTrackHolders` / `pOpponentScriptTally` despite doc `ap*` / `an*` names.
- `abPad_postWalkEmbed` @ +0x15C..+0x167 (ctor zero only; between walk sources and `bField_168`).
- Exact semantics of `view_flags@+0x44` vs track-manager `frameDelayOverrideMs` (related but distinct storage).
- **`nFacingAxisExtent@+0xA4`:** unit is pixels along the non-primary walk axis (paired with `nOrigin_x`/`nOrigin_y`); not the same as collision `nCollisionLeft` band.

## Upward links

| Parent | Relationship |
|--------|----------------|
| **`CGaming`** | Owns `m_apEntitySlots[128]` @ `+0xC8`; entries are `CBulanek*` (`CGaming.md`) |
| **`CGame`** | `pGame` @ `CBulanek+0xF4`; scheduler event 7 dispatches actions (`CGame.md`, `player_controls.md`) |
| **`CBulanci`** | App shell; embedded lobby `CGame` @ `+0x284` — not the live `CGaming` stack object |

## Follow-up

- Merge Ghidra `bStateByte@0x70` with `bPlayerSlot` naming.
- Recover `+0x11C..+0x120` ammo struct as typed array once `GetMaxAmmoForKind` table is documented.
- Leaf batch: **`CWeapon.md`**, **`CGameView.md`**, **`CGaming.md`**.
