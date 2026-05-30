# R4 pass — CBulanci app shell map

## Status

**DONE** — `sizeof(CBulanci) == 0x4cc` (1228 B); three-region layout verified in Ghidra; `CBulanci` vs **`CBulanek`** distinction documented; config/audio paths tied to embed members.

## Memory map (shipping)

| Region | Abs offset | Size | Type | Role |
|--------|------------|------|------|------|
| App shell | `+0x00` | `0x280` (640) | `CDSApp` | `app` — Win32 shell, views, DirectSound, master pack |
| Main menu hook | `+0x280` | 4 | `CMenu *` | `pMainMenu` — heap menu during modal; `0` otherwise |
| Lobby / session | `+0x284` | `0x248` (584) | `CGame` | `game` — scheduler, `CDSChain_full`, net, audio/cmdline tail |

**Proof:** `CBulanci_CreateObject@0x00402a90` → `OperatorNew(0x4cc)`; `CBulanci_ctor@0x004026f0` → `CDSApp_ctor(&this->app)` then `CGame_ctor(&this->game)`.

## CBulanci vs CBulanek

| | **CBulanci** | **CBulanek** |
|---|--------------|--------------|
| Role | Global app singleton (`g_pApp`); factory class id **2000** | Per-player **heap** entity (`OperatorNew(0x19c)`) |
| Size | **1228 B** (`0x4cc`) | **412 B** (`0x19c`) |
| Base | **`CDSApp`** + menu ptr + embedded **`CGame`** | **`CGameView`** / `CDSView` prefix + player tail |
| `CGame` | **Embedded** @ `+0x284` (lobby, resources, session) | **Pointer** `pGame` @ `+0xF4` into app’s embed |
| Audio (UI bank) | `(game).pReleaseOnDestroy` @ `game+0x23c` (`CBulanci_OnCreate` loads res `0x10004`) | Weapon/track audio via `CDSVideoPlayer` / `CWeapon` |
| Config / levels | `(game).chain` (`CDSChain_full`); `CBulanci_BuildLevelResourceTable(&game.chain)` | N/A (uses app-level pack + level table) |
| Master assets | `(app).pMasterPackStorage` @ **`CDSApp+0x70`** | Resolved through `pGame` → app pack |

Spelling: binary uses **`CBulanek`** for the player class; do not confuse with **`CBulanci`** (application).

## Config

| Path | Object | Evidence |
|------|--------|----------|
| In-session | `(this->game).chain` — `CDSChain_full` @ `CGame+0x31` | `CGame_ctor` → `CDSChain_ctor`; `CGame_dtor` → `CBulanci_DestroyConfigStore(&chain)` |
| Level index | `game.chain.pLevelResourceTable` / `nLevelResourceCount` | `CBulanci_BuildLevelResourceTable@0x0040a0b0` ← `CBulanci_InitResourceBank@0x00402180` |
| Registry / profile | Heap **`CBulanciConfigStore`** (same tail layout as `CDSChain_full`; Ghidra struct still stale) | `SaveConfigToRegistry` / `CBulanci_ClearProfileKeyList` — see `round4_task_27_report.md` |

## Audio

| Field | `CGame` rel | `CBulanci` abs | Evidence |
|-------|-------------|----------------|----------|
| `pAudioBankArray` | `+0x22c` | `+0x4b0` | `CBulanci_ctor`, `CBulanci_dtor`, `CBulanci_ReleaseAudioBank` |
| `dwAudioBankCount` | `+0x234` | `+0x4b8` | same |
| `pReleaseOnDestroy` | `+0x23c` | `+0x4c0` | `CBulanci_OnCreate@0x00402b20` — UI/combat SFX bank from master pack |
| `pCmdLine` | `+0x240` | `+0x4c4` | `CBulanci_SetCmdLine` / `CBulanci_DestroyEmbedFields` |

No duplicate tail fields on `CBulanci` (R3 task 01 / R4 task 01).

## Key methods

| Symbol | Address | Notes |
|--------|---------|-------|
| `CBulanci_CreateObject` | `0x00402a90` | `new(0x4cc)` + ctor |
| `CBulanci_ctor` | `0x004026f0` | `app` + `game` embed init |
| `CBulanci_OnCreate` | `0x00402b20` | `CDSApp_OnCreate`; bitmap cache; audio bank `0x10004` |
| `CBulanci_InitResourceBank` | `0x00402180` | Master pack → `app.pMasterPackStorage`; level table |
| `CBulanci_OpenPackStream` | `0x00401d80` | `.bul` / gzip → `CDSStreamStorage` |
| `CBulanci_OnEvent_MenuStateMachine` | `0x00402490` | Splash / `CMenu` modal; `pMainMenu` |
| `CBulanci_DestroyEmbedFields` | `0x004028d0` | Cmdline, audio, `CGame_dtor`, `CDSApp` tail |
| `CBulanci_SetCmdLine` | `0x00401970` | `&(game).pCmdLine` |

## Ghidra deltas (this pass)

- **`CDSApp`**: shrunk to **640 B** (`0x280`); `pMiPad_before_backBuffer` → **`pMasterPackStorage`** `CDSStreamStorage *` @ **`+0x70`** (hex).
- **`CBulanci`**: recreated — `CDSApp app` @ 0, `pMainMenu` @ 640, `CGame game` @ 644; `get_struct_layout` → 1228 B.
- **`CBulanci_ctor`**: `CBulanci *` / `&this->app`, `CGame_ctor(&this->game)`, `(this->game).pAudioBankArray` …
- **`CBulanci_InitResourceBank` / `OnCreate`**: `(this->app).pMasterPackStorage`, `(this->game).*`.
- Comments @ `0x00402218`, `0x0040275b`.
- `save_program bulanci.exe`.

## Struct doc updates

- [`CBulanci.md`](./CBulanci.md)
- [`CDSApp.md`](./CDSApp.md) — `pMasterPackStorage` @ `+0x70`
- Cross-refs: [`round3_task_01_report.md`](./round3_task_01_report.md), [`round4_task_01_report.md`](./round4_task_01_report.md), [`CGame.md`](./CGame.md), [`CBulanek.md`](./CBulanek.md)

## Remaining UNK

- `CDSApp` interior pads (`+0x08..`, `+0x228..`, `pPad_tail` after resize).
- `CBulanci_DestroyEmbedFields` MI vtable restore on `app` (`field_0x4` / `field_0x10` / `field_0x18`) — separate `CDSApp` pass.
- `CBulanciConfigStore` Ghidra type vs `CDSChain_full` (heap config; todo 27).
- `CGaming` stack `game` vs `g_pApp->game` embed sizing (out of shell scope).
