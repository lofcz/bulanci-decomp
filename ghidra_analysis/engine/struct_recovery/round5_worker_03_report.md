# Round 5 — Worker 03/50 Report

## Task

| Field | Value |
|-------|-------|
| **worker** | 3 / 50 |
| **round** | 5 (WRITE MODE) |
| **slice** | `FUN_*` @ `0x00440000`–`0x00460000` |
| **theme** | CBulanek movement / combat / AI cross-slice dependencies |
| **acceptance** | Disasm + xrefs + callers proof for every claimed link; Ghidra comments/plate; `save_program bulanci` |
| **structs** | CBulanek, CWeapon, CGaming |

## Status

**DONE** — slice catalogued; **two** CRT helpers in-range proven on CBulanek paths (`OperatorNewWithBadAlloc`, `_rand`); gameplay bodies stay `0x0041xxxx`; AI track factory renamed out-of-slice; blit/MPX/EH clusters marked out-of-scope for this worker.

## Slice map (`0x00440000`–`0x00460000`)

| Sub-range | ~count | Role | CBulanek link |
|-----------|--------|------|----------------|
| `0x00440160`–`0x004456d0` | ~70 | `Blit*` / `CPoemScroller_*` pixel pipelines | **None direct** — `CBulanek_Draw` → `BlitDispatch@0x004368d0` (outside slice); no `CALL` xrefs from `0x00416xxx`–`0x00421xxx` into blit entry points |
| `0x00446550`–`0x00446b90` | ~15 | `CDSMpx_*`, `InitAlphaBlendLut` | **None** — audio decode / app LUT; callers are `CDSApp_ctor`, stream factories |
| `0x00446c00`–`0x004472e0` | ~20 | `CDSSafeStream_*`, `CDSWorkingThread_*` | **None** on player entity |
| `0x00447303`–`0x00448e40` | ~120+ | MSVC EH, heap (`_malloc`, `_memcpy`), `_rand`, CRT startup | **Partial** — see below |
| `0x00448ed6`+ | tail | CRT string/stdio helpers | Generic runtime |

## CBulanek-proven functions in slice

### `OperatorNewWithBadAlloc` @ `0x00447c42`

| Claim | Evidence |
|-------|----------|
| Wraps `_malloc(size)`; failure throws `std::bad_alloc` via `__CxxThrowException@8` | Disasm @ `0x00447c57`–`0x00447ca6`: `CALL 0x0044938f`; on fail `CALL 0x00447be4` + `CALL 0x00447303` |
| **CBulanek object** `sizeof == 0x19c` | `CGaming_SpawnAndInitializePlayer@0x0041f500`: `PUSH 0x19c` @ `0x0041f526`; `CALL 0x00447c42`; `CALL CBulanekCtor` @ `0x0041f568` |
| **CWeapon** `sizeof == 0x70` on player | `CBulanekCtor@0x0041e4b0`: `PUSH 0x70` @ `0x0041e8c5`; `CALL 0x00447c42`; `CALL CWeapon_ctor` @ `0x0041e8ef`; `MOV [ESI+0xf8],EAX` |
| Callers (CBulanek band) | `get_function_callers`: `CBulanekCtor`, `CBulanek_ApplyPickupEffect`, `CBulanek_CreateRespawnTeleportPair`, `CBulanek_OnDeath`, `CGaming_SpawnAndInitializePlayer`, `CGaming_ctor`, … |

### `_rand` @ `0x004477ec`

| Claim | Evidence |
|-------|----------|
| MSVC LCG: `seed = seed*0x343fd+0x269ec3`; return `(seed>>16)&0x7fff` | Disasm @ `0x004477ec`–`0x0044780d` |
| Game scales to small integers via `IMUL` + `SAR 0xf` (÷32768) | See ctor @ `0x0041e603`, bot tick @ `0x00420ab6` |
| **AI team color** when `slotKind > 3` | `CBulanekCtor`: `CALL _rand` @ `0x0041e7f0`; `MOV [ESI+0x123],AL` (`bTeamColor`) |
| **Walk anim table** when color param negative | `CBulanekCtor`: `CALL _rand` @ `0x0041e603`; index `gABulanekWalkAnims*` @ `0x00482xxx` |
| **Bot random actions** | `CBulanek_TryBotRandomAction@0x00420a90`: `CALL _rand` @ `0x00420ab6` (idle branch), `0x00420af2` (action 0..3 → `CBulanek_ApplyAction`) |
| **AI weapon fire delay** | `CBulanek_ArmFireDelayScheduler@0x00417260`: `CALL _rand` @ `0x0041727e`; `IMUL …,0xb` + `+5` + `*0x3e8` → scheduler slot 2 ms |
| **Spawn facing** | `CGaming_SpawnAndInitializePlayer`: `CALL _rand` @ `0x0041f599` → `CBulanek_SetFacingTrack` |
| Callers (CBulanek-named) | `get_function_callers`: `CBulanekCtor`, `CBulanek_ArmFireDelayScheduler`, `CBulanek_TryBotRandomAction` (+ CGaming spawn/ambient helpers) |

## CBulanek gameplay core (outside slice, documented for scope)

| Symbol | Address | Role |
|--------|---------|------|
| `CBulanek_ApplyAction` | `0x00420910` | Input actions 0..5 → facing / fire / weapon |
| `CBulanek_ClampMoveRectByCollision` | `0x004195f0` | Movement — `SpatialQuery@0x00418300` (not in slice) |
| `CBulanek_TryBotRandomAction` | `0x00420a90` | AI scheduler tick |
| `CBulanek_SchedulerTick` | `0x0041aed0` | Per-player scheduler cases |
| `CBulanek_SetFacingTrack` | `0x004197b0` | Walk track from facing |
| `CBulanek_AllocAiTrackHolder` | `0x0041b420` | **Renamed** from `FUN_0041b420`; ctor fills `apAiTrackHolders@+0x178` |

`get_function_callees` on movement/combat entry points shows **no callees** in `0x0044xxxx` except `_rand` / `OperatorNewWithBadAlloc` as above.

## Ghidra deltas (R5 worker 03)

| Action | Target |
|--------|--------|
| `set_plate_comment` | `OperatorNewWithBadAlloc@0x00447c42`, `_rand@0x004477ec` |
| `set_decompiler_comment` | `0x0041e8cd`, `0x0041f52b`, `0x0041e603`, `0x0041e7f0`, `0x0041727e`, `0x00420ab6`, `0x00420af2`, `0x0041f599` |
| `rename_function_by_address` | `FUN_0041b420` → `CBulanek_AllocAiTrackHolder` |
| `save_program` | `bulanci` |

## Struct doc updates

- [CBulanek.md](./CBulanek.md) — R5 worker 03 CRT cross-ref + AI factory rename

## Remaining UNK / deferred

- **Blit cluster** (`0x00440160`–`0x004456d0`): likely reached via format dispatch table from `BlitDispatch`; recover in dedicated render batch (not CBulanek-specific).
- **`FUN_004438a0` / `FUN_004466a0` / …** in slice: no CBulanek-band callers found; leave for render/MPX workers.
- Movement collision math (`SpatialQuery`, `CBulanek_ClampMoveRectByCollision`) — addresses `0x00418300` / `0x004195f0` (worker slice 2 band).
