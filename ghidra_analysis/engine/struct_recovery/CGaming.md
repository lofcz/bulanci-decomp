# CGaming

## Status

**PARTIAL** — match-modal coordinator **`0x36C` (876 B)** on stack during `CGame_StartGame` (`local_414[16]` + `local_404[860]`). **Not** `0x20c` — that size is **`CDirectKeyb`** (`player_controls.md`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CGaming) == 0x36C` | `0x00413ce0` region | `CGame_StartGame` stack locals 16+860; `CGaming_dtor(local_414)` |
| ctor | `0x00420380` | `CGaming_ctor` — vtables, slot array zero, `CGame*` owner, level script OnInit |
| Entity slots | `0x00481561` | `CGaming_RegisterObjectAtSlot`: `*(this + 200 + slot*4)` |

## Layout table (selected proven offsets)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CGaming_ctor@0x00420380` (`0x00482764` cluster) |
| `+0x04` | — | `CGame` / embed | `game` | MI embed; net offsets +4 vs standalone `CGame` (`net_protocol.md`) |
| `+0xC8` | `0x200` | `void *[128]` | `m_apEntitySlots` | ctor `_memset(..., 0x200)` @ `pPad_preGame+0x3c`; `RegisterObjectAtSlot` `this+200+slot*4` |
| `+0x32` / `+0x33` | 1+1 | `byte` | local player slot / seat count | **`CGame`** fields (`CGame__SchedulerDispatch@0x00416030`); linear `+0x36/+0x37` with embed |
| `+0x208` | 4 | `CDirectKeyb *` | `pDirectKeyb` | `CGame_StartGame` `OperatorNew(0x20c)` stored in game facet |
| `+0x334` | 4 | `CPauseDlg *` | `pPauseDlg` | `CPauseDlg_Build` after `OperatorNew(0x7c)` |
| `+0x338` | 4 | `int` | `nRoundEndWait` | `CGaming_IsRoundEndTransitionComplete` |
| `+0x360` | 2 | `ushort` | `wModalExitCode` | `CGaming_OnCmd` |
| `+0x368` | 4 | `uint` | `dwWeaponSpawnerMode` | ctor default 2; host-admin → 6 |

## Relationship

| Type | Link |
|------|------|
| **`CGame`** | Owner pointer in ctor (`pPad_44+0x40`); scheduler pump `0x00416030` on embedded game |
| **`CBulanek`** | Elements of `m_apEntitySlots`; spawned by `CBulanek_SpawnPlayerAndCampaignSlots` |
| **`CBulanci`** | Persistent app; **does not** host stack `CGaming` — only embedded lobby `CGame` @ `+0x284` |
| **`CLevelScript`** | `pGaming` @ **`CLevelScript+0x458`** (script VM), not a field inside `CGaming` |

## Key methods

| Symbol | Address |
|--------|---------|
| `CGaming_ctor` | `0x00420380` |
| `CGaming_GetObjectAtSlotSafe` | `~0x0041f8f0` |
| `CGaming_GetObjectAtSlotUnchecked` | `~0x0041f9c0` |
| `CGaming_RegisterObjectAtSlot` | `~0x0041f9ed` |
| `CGaming_OnSchedulerTimer` | `0x0041f050` |
| `CGame__SchedulerDispatch` | `0x00416030` |

## Ghidra apply

- `create_struct CGaming` size **876** (`0x36C`).
- Model embedded `CGame` at `+0x04` carefully — do not overlay 872 B type on `CBulanci.pGameEmbed` (556 B slice).

**Agent todo 12 (2026-05-30):** `modify_struct_field CGaming.game` → **`CGame`** (584 B / `0x248`, was 872 B `-BAD-`); tail `pPauseDlg` @ `+0x334`, `nRoundEndWait` @ `+0x338`, `wModalExitCode` @ `+0x360`, `dwWeaponSpawnerMode` @ `+0x368`; `CGaming_ctor` prototype `(CGaming *this, CGame *ownerGame)` @ `0x00420380`; `save_program bulanci.exe`.

## UNK

- CDSView prefix `+0x08..+0x87` naming.
- `+0x31C` entity list head (separate from slot array).
- Interior `pGameEmbed[n]` byte indexing in ctor — retype to named fields.

## Follow-up

- Cross-link `match_orchestration.md`, `map_slots_spawner.md`.
- Slice **campaign-04** in `gameplay_struct_backlog.md`.
