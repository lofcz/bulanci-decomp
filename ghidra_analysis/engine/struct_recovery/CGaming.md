# CGaming

## Status

**PARTIAL** — match-modal coordinator **`0x36C` (876 B)** on stack during `CGame_StartGame` (`local_414[16]` + `local_404[860]`). **Not** `0x20c` — that size is **`CDirectKeyb`** (`player_controls.md`). R5 worker 25 closed the Ghidra gap **`+0x588..+0x33F`** (was a single `CGame` blob) and corrected **`wModalExitCode`** to **`+0x364`**. **R10 task 12** closed tail UNKs **`nAmbientAnimMode`**, **`pPrefaceDrawable`**, **`nNetPurgeEntitySlotCount`**, renamed danger/purge slot vecs, verified **`wModalExitCode`** consumers (7 disasm sites).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CGaming) == 0x36C` | `0x00413ce0` region | `CGame_StartGame` stack locals 16+860; `CGaming_dtor(local_414)` |
| partial ctor (CreateObject) | `0x0041ab70` | `CGaming_Ctor` — `OperatorNew(0x36C)` path via `CreateObject@0x0041beb0`; MI vtables, `CDSUpdatedItem` @ `+0x68`, `apHudBitmaps[16]` @ `+0x88`, four `CDSPtrSlotVec` @ `+0x2C8..+0x2F8`; **no** level script / HUD preload / owner wiring (R6 task 08 disasm) |
| full ctor (stack) | `0x00420380` | `CGaming_ctor` — complete shell: `CGame*` owner @ `+0x84`, level script `OnInit`, panels, audio, entity root |
| Entity slots | `0x004168d0` / `0x00482062` | `CGaming_GetObjectAtSlotUnchecked`: `[this + slot*4 + 0xC8]`; ctor `memset` **0x200** @ `ESI+0xC8` |

## Layout table (proven offsets)

### MI / view shell (`+0x00..+0x87`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `pVftable_primary` | `CGaming_ctor@0x0041ffd4` → `0x00482764` |
| `+0x04` | 4 | `pointer` | `pVftable_IDSChained` | `CGaming_ctor@0x0041ffda` → `0x00482748` |
| `+0x10` | 4 | `pointer` | `pVftable_IDSUpdated` | `CGaming_ctor@0x0041ffe1` → `0x00482730` |
| `+0x18` | 4 | `pointer` | `pVftable_IDSReferenced` | `CGaming_ctor@0x0041ffe8` → `0x0048271c` |
| `+0x68` | 24 | `CDSUpdatedItem` | `schedulerFacet` | `CGaming_ctor@0x0041ffc5` `CDSUpdatedItem_ctor(ESI+0x68)`; facet vtable @ `EDI` → `0x00482704` |
| `+0x14` | 2 | `ushort` | `wViewFlags` | ctor `OR [ESI+0x14],0x77f` @ `0x004200f7` |
| `+0x20` | 4 | `int` | `nBbox_left` | ctor zero @ `0x004200ad` |
| `+0x24` | 4 | `int` | `nBbox_top` | ctor zero @ `0x004200b0` |
| `+0x28` | 4 | `int` | `nBbox_right` | ctor `=0x320` (800) @ `0x004200b9` |
| `+0x2c` | 4 | `int` | `nBbox_bottom` | ctor `=0x258` (600) @ `0x004200c0` |
| `+0x46` | 2 | `ushort` | `wFlags2` | ctor `OR [ESI+0x46],1` @ `0x00420103` |
| `+0x80` | 4 | `pointer` | `pPad_80` | ctor `=0` @ `0x0041fff5` |
| `+0x88` | 64 | `ODSImage *[16]` | `apHudBitmaps` | ctor vector ctor **16×4** @ `ESI+0x88` @ `0x00420007`; preload loop `0x004201ac..0x00420205` |

### Owner, slots, gameplay vectors

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x84` | 4 | `CGame *` | `pOwnerGame` | `CGaming_ctor@0x004200cf` `MOV [ESI+0x84],EDI`; consumers `MOV ECX,[ESI+0x84]` (`OnResumeOrStartGame`, level load) |
| `+0xC8` | 512 | `void *[128]` | `apEntitySlots` | ctor `memset(ESI+0xC8,0,0x200)` @ `0x004200a7`; `GetObjectAtSlotUnchecked@0x00482062` |
| `+0x148` | 16 | `CDSView *[4]` | `apPlayerHudViews` | `CGaming_CountActiveEntities@0x00416943` `LEA EDX,[ECX+0x148]` loop 4; `PostMouseMoveToOccupiedHudSlot@0x004168f3` |
| `+0x2C8` | 8 | `CDSPtrSlotVec` | `bulletSlotVec` | ctor init `ESI+0x2C8..+0x2D4` @ `0x0042001e`; `CGaming_CleanupInactiveBullets@0x0041a2a0` |
| `+0x2D0` | 4 | `int` | `nBulletSlotCount` | companion live-count dword after `bulletSlotVec` (ctor zero @ `0x0042002a`; pattern matches other vecs) |
| `+0x2D8` | 8 | `CDSPtrSlotVec` | `dangerZoneSlotVec` | `CGaming_AppendDangerZoneNode` → `CIntListInsertSortedOrAppend(&+0x2d8)`; `DefineTraceArea` / `DefineDangerZone` / `CMina_RegisterDangerZone`; `CGaming_RetestTraceAreasForEntity` walks count `@+0x2e0` |
| `+0x2E0` | 4 | `int` | `nDangerZoneSlotCount` | `CGaming_RetestTraceAreasForEntity@0x0041b5a0` reverse-walk count; `AppendDangerZoneNode` insert helper |
| `+0x2E8` | 8 | `CDSPtrSlotVec` | `vecSlotVec_2e8` | ctor @ `0x0042004e`; dtor resize @ `0x0041b8..` — **no read xrefs** (reserved) |
| `+0x2F0` | 4 | `int` | `nVecSlot2e8Count` | ctor zero only @ `0x0042005a` — no consumer |
| `+0x2F8` | 8 | `CDSPtrSlotVec` | `netPurgeEntitySlotVec` | `CGaming_OnCustomEvent_0xF5_RemoveEntitiesBySlotId@0x0041b3b0` reverse-walk; match `entity+0x3C` (slot id); max 2 removals |
| `+0x300` | 4 | `int` | `nNetPurgeEntitySlotCount` | `0xF5` handler `MOV EDI,[ECX+0x300]` @ `0x0041b3b3` |
| `+0x304` | 4 | `int` | `cNetPurgeEntitySlotCapacity` | ctor `=8` @ `0x0042007e` (parallel to other vec capacity dwords) |
| `+0x30C` | 4 | `int` | `pad_0x30c` | ctor zero @ `0x004200d5` — **no read** |
| `+0x310` | 4 | `CDSString` | `hLevelTitleCopy` | ctor `MOV [ESI+0x310],0` @ `0x0042007e`; assign `CDsStringAssignFromHandle` @ `0x0042028e`; dtor `CDsStringReleaseHeader` @ `0x0041b98d` |

### HUD / script / audio tail (`+0x31C..+0x368`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x314` | 1 | `byte` | `bHudPanelFlag` | ctor `MOV byte [ESI+0x314],1` @ `0x004200db` |
| `+0x31C` | 4 | `CDSChained *` | `pEntityViewRoot` | ctor `MOV [ESI+0x31c],EAX` @ `0x00420263` (`OperatorNew(0x68)` panel); `CollectEntitiesInBlastRect@0x004183d3` `[ECX+0x31c]` + child walk `+0x54`; `CGaming_ClearAllEntities@0x00419d20` |
| `+0x320` | 4 | `CDSView *` | `pScorePanel` | ctor `MOV [ESI+0x320],EAX` @ `0x004203c0` (`OperatorNew(0xFC)` + `CPanelCtor`); dtor teardown via view tree |
| `+0x324` | 4 | `CBulAnim *` | `pAmbientAnim0` | solo ctor `LEA EAX,[ESI+0x324]` + `AllocAnimFromSprite(0x10125)` @ `0x00420321` |
| `+0x328` | 4 | `CBulAnim *` | `pAmbientAnim1` | `+0x328` + res `0x10126` @ `0x00420335` |
| `+0x32C` | 4 | `CBulAnim *` | `pAmbientInsects` | `+0x32c` + res `0x100dd` @ `0x00420349` |
| `+0x330` | 4 | `CBulAnim *` | `pAmbientSky` | `+0x330` + res `0x100dc` @ `0x0042035d` |
| `+0x334` | 4 | `CPauseDlg *` | `pPauseDlg` | `OperatorNew(0x7c)` → `CPauseDlg_Build` → `MOV [ESI+0x334],EAX` @ `0x00420434` |
| `+0x338` | 4 | `int` | `nRoundEndWait` | `CGaming_IsRoundEndTransitionComplete@0x004168e0` `[param_1+0x338]==0`; `OnCmd` `ADD [ESI+0x338],1` @ `0x0041d63b` |
| `+0x341` | 1 | `byte` | `bEntityRegisterMode` | **R6 logic todo 6:** `MOVZX EAX,byte [ESI+0x341]` @ `CGaming_AddEntity@0x0041a3ac` branches `0/1/2`; writer `CGaming_SetEntityRegisterMode@0x004168c0` (`MOV [ECX+0x341],AL`); also `CGaming_SpawnPracticeDummy@0x0041f583` sets `1` before spawn |
| `+0x33C` | 4 | `int` | `nAmbientAnimMode` | `CGaming_SetAmbientAnimMode@0x00417c87` `MOV [ESI+0x33c],EAX`; modes **0** → 25 s / **1** → 10 s scheduler delay on `schedulerFacet` slot 0; callers `CGaming_ctor` (solo), `CGaming_TickAmbientAnimations`, `CGaming_OnPlayerCollectItem`, `CGaming_UnregisterRoundHudObjects` |
| `+0x344` | 4 | `CLevelScript *` | `pLevelScript` | ctor store menu script @ `0x004202c9`; `OnResumeOrStartGame` `CallExport(...,10,...)` @ `0x0041c157`; dtor `CallExport(...,2,...)` @ `0x0041b8a0` |
| `+0x348` | 4 | `CDSView *` | `pDepthInsertHead` | `CGaming_InsertEntityByDepth` / `AddEntity` compare-update @ `0x004184fe`, `0x0041a3e8` |
| `+0x34C` | 4 | `CDSView *` | `pDepthInsertTail` | `InsertEntityByDepth` / `Unregister` @ `0x00418529`, `0x00419cca` |
| `+0x350` | 4 | `uint` | `dwMusicResourceId` | `CGaming_SetMusicAndSoundBank@0x0041ba67` `MOV [ESI+0x350],EAX`; `LoadLevelAssetAndMusic` @ `0x0041d341` |
| `+0x354` | 4 | `CDSAudioPlayer *` | `pLevelBgmPlayer` | dtor release @ `0x0041b9..`; `OnResumeOrStartGame` play/stop |
| `+0x358` | 4 | `CDSAudioPlayer *` | `pIntroMusicPlayer` | ctor `CreateFromResource(0x1014b)` → `[ESI+0x358]` @ `0x00420174`; dtor release |
| `+0x35C` | 4 | `CDSAudioPlayer *` | `pDefaultSfxBankPlayer` | ctor stores resource `0x10003` bank via `[ESI+0x35c]` path; dtor release @ `0x0041b977` |
| `+0x360` | 4 | `void *` | `pPrefaceDrawable` | `CLevelScriptOpExt_LoadPreface@0x0041d96f` store cast `DAT_004b826c`; `CGaming_LoadLevelAssetAndMusic` / `CGaming_dtor` Release via vtable `+8` |
| `+0x364` | 2 | `ushort` | `wModalExitCode` | ctor `MOV word [ESI+0x364],0xFFFF` @ `0x004200ee`; **`CGaming_OnCmd`** `CMP/MOV/MOVZX` @ `0x0041d602`/`0x0041d615`/`0x0041d686`/`0x0041d69f`; `cmd==0xED && sentinel` → `CGaming_RunPreMatchModal`; high-bit → round-end + `EndModal`; **`CGaming_OnCustomEvent`** `CMP` @ `0x004206f7` (case `0xE9` gate) |
| `+0x368` | 4 | `uint` | `dwWeaponSpawnerMode` | ctor `=2` / host-admin `=6` @ `0x00420117`, `0x00420132`; `RandomPickupSpawner_Tick` reads `dwWeaponSpawnerMode` |

**Linear `CGame` fields** (e.g. `+0x32` local player slot, `+0x208` `pDirectKeyb`, demo stream `+0x1BC`) are on **`pOwnerGame`**, not duplicated on the `CGaming` shell — use `pOwnerGame + offset` (see `net_protocol.md` embed table when `pOwnerGame` points at `CBulanci.game`).

## Relationship

| Type | Link |
|------|------|
| **`CGame`** | **`pOwnerGame` @ `+0x84`** — lobby / match `CGame` instance (often `CBulanci+0x284`); **not** a 584 B subobject at `+0x04` (prior Ghidra `game` embed was incorrect) |
| **`CBulanek`** | Elements of `apEntitySlots`; spawned by `CBulanek_SpawnPlayerAndCampaignSlots` |
| **`CBulanci`** | Persistent app; **does not** host stack `CGaming` — only embedded lobby `CGame` @ `+0x284` |
| **`CLevelScript`** | `pLevelScript` @ **`+0x344`** on `CGaming`; script VM also has `pGaming` @ **`CLevelScript+0x458`** |

## Key methods

| Symbol | Address |
|--------|---------|
| `CGaming_Ctor` (partial / CreateObject) | `0x0041ab70` |
| `CGaming_ctor` (full / stack) | `0x00420380` |
| `CGaming_GetObjectAtSlotSafe` | `0x00416810` |
| `CGaming_GetObjectAtSlotUnchecked` | `0x004168d0` |
| `CGaming_RegisterObjectAtSlot` | `0x00482074` |
| `CGaming_OnSchedulerTimer` | `0x0041f050` |
| `CGaming_RunPreMatchModal` | `0x0041c290` |
| `CGaming_ArmTrackMgrSchedulerIfUnpaused` | `0x00439880` |
| `CGaming_PreMatchModal_RestartDeferredAudio` | `0x0043aa70` |
| `CGaming_SetAmbientAnimMode` | `0x00417c80` |
| `CGaming_OnCmd` | `0x0041d5f0` |
| `CGaming_OnPlayerCollectItem` | `0x0041a020` — unregister pickup, `CBulanek_NetSendTeamScoreOnCollect`, quip slot from `weaponKind` when collector `bPlayerSlot==0` (R6 logic task 10) |
| `CGaming_TickPlayerCollisions` | `0x0041f0c0` — calls `OnPlayerCollectItem` (4 sites @ `0x0041f154`..`0x0041f1bd`) |
| `CGaming_RespawnPlayerAtSafeLocation` | `0x0041a140` — random 800×516 placement; `SpatialQuery(p4=1,p5=0)` loop; callers include `SpawnAndInitializePlayer`, `RespawnPlayer`, `CreateRespawnTeleportPair` |
| `CGame__SchedulerDispatch` | `0x00416030` (on **`pOwnerGame`**) |
| `CExplosion_CollectEntitiesInBlastRect` | `0x004183d0` |
| `CGaming_ClearAllEntities` | `0x00419d20` |

## Ghidra apply

- `create_struct CGaming` size **876** (`0x36C`).
- **R5 worker 25:** removed erroneous **`CGame game` @ +4**; added **`pOwnerGame`**, **`apEntitySlots[128]`**, **`bulletSlotVec`**, **`pEntityViewRoot`**, depth-insert pointers, script/audio tail, **`wModalExitCode` @ +0x364** (was wrongly @ +0x360).
- Plate comment @ `0x00420263` (`pEntityViewRoot`).
- `save_program bulanci.exe`.

**Agent todo 12 (2026-05-30):** prior pass typed `CGaming.game` as **`CGame`** 584 B — **superseded** by `pOwnerGame` pointer (R5 w25 disasm). Tail fields `pPauseDlg` / `nRoundEndWait` / `dwWeaponSpawnerMode` unchanged.

## UNK

- **`vecSlotVec_2e8` / `nVecSlot2e8Count`**: ctor/dtor only — no read xrefs in `.text`.
- **`pad_0x30c`**: ctor zero only.
- Exact nominal type of **`pPrefaceDrawable`** (`CDSStaticDrawableFace *` vs generic COM) — cast id `DAT_004b826c` shared with `CBulPicture.pBitmap`.
- Interior **`CDSView` / `CDSChained`** fields inside `pEntityViewRoot` panel — use `CDSView.md` (+0x54 child chain for entity iteration).

## Follow-up

- Retype **`CGaming_GetObjectAtSlot*`** / **`RegisterObjectAtSlot`** to use `apEntitySlots` member (still shows `chain.pPad_a4` in some decompiles).
- ~~Fold **`vecSlotVec_*`** names~~ — **R10 t12:** `dangerZoneSlotVec`, `netPurgeEntitySlotVec` + companion counts; `vecSlotVec_2e8` still reserved.
- Cross-link `match_orchestration.md`, `map_slots_spawner.md`, `round4_task_08_report.md` (collect helpers).
