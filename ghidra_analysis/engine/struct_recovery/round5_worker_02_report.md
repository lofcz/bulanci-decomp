# Round 5 — Worker 02/50 Report

## Task

| Field | Value |
|-------|-------|
| **Worker** | 2 / 50 |
| **Round** | 5 (WRITE MODE) |
| **Slice** | `FUN_*` @ `0x00420000`–`0x00440000` |
| **Theme** | CGaming combat, explosions, weapons helpers |
| **Rules** | Evidence-only renames (disasm + xrefs + callers); `save_program bulanci.exe` |
| **Structs** | CGaming, CBulanek, CWeapon, CShot, CExplosion, CGameView |

## Status

**DONE** — **13** combat-adjacent `FUN_*` symbols renamed (12 new + `CWeapon_SetTrackHolder` already named). Sub-range `0x00420000`–`0x00424000` has **no** remaining default `FUN_*` names; `0x00424000`–`0x00440000` still holds ~90+ `FUN_*` (menu, scheduler, CRT glue, codecs) **out of scope** for this worker.

## Range inventory

| Sub-range | `FUN_*` left | Verdict |
|-----------|--------------|---------|
| `0x00420000`–`0x00423fff` | **0** (after this pass) | Combat / net handlers largely named in R3–R4 (`CGaming_OnNetMsg_*`, `CBulanek_OnEvent`, `CGame_ProcessNetMessage`, …) |
| `0x00424000`–`0x0042ffff` | ~40 | **Mixed** — renamed HUD ammo strip cluster (`0x427c30`–`0x427c90`, `0x427710`); deferred menu/scheduler (`FUN_0042a330`, `FUN_0042f620`, …) |
| `0x00430000`–`0x0043ffff` | ~50+ | **SKIP** — script VM, streams, IJG/zlib tails (no combat caller proof) |

Gameplay bodies for shots/explosions/weapons sit mostly **`0x00416xxx`–`0x00422xxx`** (slightly below `0x420000`); this worker included proven helpers **called from** the `0x42xxxx` event/net band.

## Evidence — renames applied

| Old | New | Address | Proof |
|-----|-----|---------|--------|
| `FUN_0041ed10` | `CBulanek_OnEvent_tD9_SyncWeaponFromSpawner` | `0x0041ed10` | **Caller:** `CBulanek_OnEvent` case **`0xD9`** @ `0x00420e1f`. **Disasm/decompile:** reads `CGaming+0x368` (`dwWeaponSpawnerMode`), scans weapon bar, **`CALL CBulanek_ApplyPickupEffect`** when kind differs. |
| `FUN_004180e0` | `CGaming_OnKillCredit_UpdateScoreHud` | `0x004180e0` | **Caller:** `CBulanek_OnEvent` case **`0xD8`** @ `0x00420e10` with `ECX = pGamingHostScratch`. **Callees:** `CBulanek_CopyScoreBlockToSlot`, `CGameCounter_UpdatePlayerScore`, `CGaming_SetRoundTimerLabelFromString`; may `CDSView__PostMessage(0x100,0x80cc)` when score cap hit. |
| `FUN_00427710` | `CGaming_SetRoundTimerLabelFromString` | `0x00427710` | **Callers:** `CGaming_OnTimerCountdown`, `CGaming_TickRoundStateAndScoring`, `CGaming_OnKillCredit_UpdateScoreHud`. **Disasm:** `CDsStringAssignFromHandle` then refresh widget @ `this+0xb8`. |
| `FUN_004165b0` | `CDSView_PostEntityDetachMessage_f9` | `0x004165b0` | **Callers:** `_Globals::ExplodeMine`, `CExplosion_OnEvent` (phase **-1**). **Disasm:** `LEA ECX,[param+0x10]`; `CDSView__PostMessage(...,0x200,0xf9,...)`. |
| `FUN_00417f40` | `CGameView_LookupBulletFrameStrip` | `0x00417f40` | **Sole caller:** `CShot_Ctor` @ `0x0041eecb`. Returns frame strip from map `+0x88` table or `HandleVirtualBaseCast(...,0xd)`. |
| `FUN_0041bf80` | `CWeapon_SetTrackHolder` | `0x0041bf80` | **Callers:** `CWeapon_ctor`, `Init`, `Destructor`. **Disasm:** `this+0x50` list remove/release + `CIntListInsertSortedOrAppend`. |
| `FUN_0041a270` | `CGaming_ReleaseAndClearSlotRef` | `0x0041a270` | **Caller:** `CGaming_dtor` (4× clears `+0x324..+0x330`). **Callee:** `CGaming_UnregisterAndRemoveObject` + vtable release. |
| `FUN_00419b40` | `CShot_OnTrackTick_AdvanceFacingRect` | `0x00419b40` | **Xref:** vtable `0x00481c5c` slot (CDSUpdatedItem facet). Moves rect by facing `+0x1c`; `Scheduler_ArmSlot` + `CDSView__Hide` when `+0x20` set. |
| `FUN_004172d0` | `CBulanek_ArmTournamentSchedulerDelays` | `0x004172d0` | **Callers:** `CBulanek_WeaponSchedulerCallback`, `CBulanek_OnShow`. **Disasm:** slot `+0x70` in `0x20..0x27`; arms scheduler slots **3/4** with `_rand`-scaled delays. |
| `FUN_00417dd0` | `CGaming_DispatchTraceAreaTransitionIfMasked` | `0x00417dd0` | **Caller:** `CGaming_CheckTraceAreasForEntity` (3 call sites). **Callee:** `CLevelScript_DispatchTraceAreaTransition`. |
| `FUN_00416700` | `CBulanek_RefreshHudWeaponStrip` | `0x00416700` | **Callers:** `CBulanek_ApplyPickupEffect`, `CBulanek_ResetAmmoAndPlayReload`, `CBulanek_DecrementWeaponAmmo`, `CBulanek_WeaponSchedulerCallback`. Loads `CGaming` panel @ `*(pGaming)+0x320` (`+800` disasm). |
| `FUN_00427c90` | `CGamingHud_UpdatePlayerAmmoStrip` | `0x00427c90` | **Callers:** `CBulanek_RefreshHudWeaponStrip`, `CGaming_ctor` @ `0x004203f3`. Uses `CWeapon+0x68/0x6c`, `CBulanek_HasAmmoForCurrentWeapon`, `CSpells_SetAmmoEmptyAndInvalidate`. |
| `FUN_00427c30` | `CGamingHud_SetAmmoStripState` | `0x00427c30` | **Sole caller:** `CGamingHud_UpdatePlayerAmmoStrip`. Swaps ODS drawable @ `this+0x70`, flags `+0x78..+0x7a`, vtable `+0x24` invalidate. |

## Already named (no change)

Examples verified in-band: `CGaming_OnNetMsg_t10_Hit@0x00420510`, `CGaming_OnNetMsg_t0d_PlayerState@0x004209f0`, `CGame_OnNetMsg_t0e_PrimaryAction@0x00420a70`, `CGaming_SpawnBulletAndPlaySound@0x0041f230`, `CExplosion_CollectEntitiesInBlastRect@0x004183d0`, `CExplosion_CollectLandminesInBlastRect@0x0041a2f0`, `_Globals::ExplodeMine@0x0041e070`, `CBulanek_OnEvent@0x00420d40`.

## Ghidra deltas

| Action | Target |
|--------|--------|
| `rename_function_by_address` | 12 symbols (table above) |
| `save_program` | `bulanci.exe` |

## Struct doc updates

- [CGaming.md](./CGaming.md) — R5 worker 02 HUD / dtor helper renames
- [CBulanek.md](./CBulanek.md) — event `0xD9` / HUD refresh / tournament scheduler
- [CWeapon.md](./CWeapon.md) — `CWeapon_SetTrackHolder` confirmed
- [CExplosion.md](./CExplosion.md) — detach message helper cross-ref

## Remaining UNK (this slice)

- `FUN_0041b190` / `FUN_0041ad80` — shared `CAnim` scheduler hook; needs paired rename (vtable `0x004820f4` cluster).
- Menu/scheduler `FUN_*` in `0x00424xxx`–`0x0042fxxx` (e.g. `FUN_0042a330` display mode, `FUN_0042f620` scheduler hooks) — not combat-themed.
- `CGaming_OnKillCredit_UpdateScoreHud` / net handlers: `set_function_this_type` to `CGaming *` still deferred (ECX cast from `pGamingHostScratch`).
