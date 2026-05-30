# CBulanci

## Status

**PARTIAL** — total size **`0x4cc` (1228 B)**; three-region layout proven in Ghidra. **`CDSApp app`** @ `+0x00` (**640 B** / `0x280`); **`CMenu * pMainMenu`** @ `+0x280`; **`CGame game`** @ `+0x284` (**584 B** / `0x248`). Not to be confused with player class **`CBulanek`** (412 B heap, `CGameView` base) — see distinction table below.

## CBulanci vs CBulanek

| | **CBulanci** (this type) | **CBulanek** |
|---|--------------------------|--------------|
| Role | App singleton (`g_pApp`); class id **2000** | Per-player entity in match |
| Size | **1228 B** | **412 B** (`0x19c`) |
| Layout | `CDSApp` + `pMainMenu` + embedded **`CGame`** | `CGameView` prefix + player tail |
| Session state | **`game`** embed (lobby, chain, net) | **`pGame`** @ `+0xF4` → app’s embed |
| Pack / levels | `(app).pMasterPackStorage`, `(game).chain` | Via `pGame` only |

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CBulanci) == 0x4cc` | `0x00402ab1` | `CBulanci_CreateObject`: `PUSH 0x4cc` → `OperatorNewWithBadAlloc` |
| `CDSApp app` span `0x280` | `get_struct_layout` | `CDSApp` **640 B**; `pMainMenu` @ 640 |
| Embedded `CGame` at `+0x284` | `0x004026f0` | `CBulanci_ctor`: `CGame_ctor(&this->game)` |
| Embedded span `0x248` | (derived) | `0x4cc - 0x284 = 0x248`; `CGame` **584 B** |
| `pMainMenu` @ `+0x280` | `0x00402490` | `CBulanci_OnEvent_MenuStateMachine`: assign / clear `this->pMainMenu` |
| Audio-bank tail inside `game` | `0x004026f0` | `(this->game).pAudioBankArray` … `bMenuLastSplashFlag` |
| Master pack on app | `0x00402180` | `CBulanci_InitResourceBank`: `(this->app).pMasterPackStorage` @ **`CDSApp+0x70`** |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | `0x280` | `CDSApp` | `app` | `CBulanci_ctor@0x004026f0` → `CDSApp_ctor(&this->app)` — `CDSApp.md` |
| `+0x280` | 4 | `CMenu *` | `pMainMenu` | ctor `= 0`; `CBulanci_OnEvent_MenuStateMachine@0x00402490` |
| `+0x284` | `0x248` | `CGame` | `game` | `CGame_ctor(&this->game)`; config @ `game.chain`; audio/cmdline tail |

### `CDSApp app` highlights (@ `+0x00`)

| `CDSApp` rel | Name | Evidence |
|--------------|------|----------|
| `+0x70` | `pMasterPackStorage` | `CBulanci_InitResourceBank@0x00402180`, `CBulanci_OnCreate@0x00402b20`, `CBulanci_BuildBitmapCache` |
| `+0x7c` | `backBuffer` | `CDSApp_ctor` |
| `+0x200` | `directSound` | `CDSApp_ctor` |
| `+0x100` | `keyLatchByVk` | `CDSApp.md` |

### `CGame game` highlights (@ `+0x284`)

| `CGame` rel | `CBulanci` abs | Name | Evidence |
|-------------|----------------|------|----------|
| `+0x31` | `+0x2b5` | `chain` (`CDSChain_full`) | `CBulanci_InitResourceBank` → `BuildLevelResourceTable(&game.chain)` |
| `+0x22c` | `+0x4b0` | `pAudioBankArray` | `CBulanci_ctor` / dtor / `ReleaseAudioBank` |
| `+0x23c` | `+0x4c0` | `pReleaseOnDestroy` | `CBulanci_OnCreate` — UI SFX bank from pack |
| `+0x240` | `+0x4c4` | `pCmdLine` | `CBulanci_SetCmdLine` / `DestroyEmbedFields` |

## Key methods

| Symbol | Address |
|--------|---------|
| `CBulanci_CreateObject` | `0x00402a90` |
| `CBulanci_ctor` | `0x004026f0` |
| `CBulanci_OnCreate` | `0x00402b20` |
| `CBulanci_InitResourceBank` | `0x00402180` |
| `CBulanci_OpenPackStream` | `0x00401d80` |
| `CBulanci_OnEvent_MenuStateMachine` | `0x00402490` |
| `CBulanci_DestroyEmbedFields` | `0x004028d0` |
| `CBulanci_SetCmdLine` | `0x00401970` |
| `CBulanci_BuildLevelResourceTable` | `0x0040a0b0` |
| `CBulanci_DirEnumCtx_FormatCurrentPath` | `0x004014b0` — joins `ctx+0` path + `ctx+0x30` `cFileName` via `CBulanci_FormatPathPair`; `CBulanci_dtor` / `BuildLevelResourceTable` |
| `CBulanci_DestroyConfigStore` | `0x0040a380` |

## Config vs audio (where state lives)

- **Config / level catalog:** `(this->game).chain` — embedded `CDSChain_full`; populated from master pack in `CBulanci_InitResourceBank`. Heap **`CBulanciConfigStore`** shares tail layout for registry save/load (`round4_task_27_report.md`).
- **UI audio bank:** `(this->game).pReleaseOnDestroy` — not on `CDSApp`; loaded in `CBulanci_OnCreate` via `(app).pMasterPackStorage`.
- **Dynarray banks:** `(this->game).pAudioBankArray` / `dwAudioBankCount` — see alias table below.

## Ghidra apply

**R4 pass CBulanci (2026-05-30):**

- `CDSApp` resized **640 B**; **`pMasterPackStorage`** `CDSStreamStorage *` @ **`+0x70`**.
- **`CBulanci`** recreated: `app` @ 0, `pMainMenu` @ 640, `game` @ 644 (1228 B).
- `CBulanci_ctor` → `CBulanci *`; decompile `&this->app`, `&this->game`, `(this->game).pAudioBankArray` …
- `CBulanci_InitResourceBank` / `OnCreate` / `OnEvent_MenuStateMachine` use `app` / `game` / `pMainMenu`.
- Report: [pass_r4_CBulanci_report.md](./pass_r4_CBulanci_report.md). `save_program bulanci.exe`.

**Agent todo 12 (2026-05-30):** `CGame` **584 B**; duplicate tail @ `+0x4b0` removed.

**Agent todo 1 r3 (2026-05-30):** `CBulanci+0x4b0..+0x4c8` ≡ `game+0x22c..+0x247`. [round3_task_01_report.md](./round3_task_01_report.md).

**Agent todo 1 r4 (2026-05-30):** `CBulanci_DestroyEmbedFields@0x004028d0` `__thiscall`; `(this->game).pCmdLine`. [round4_task_01_report.md](./round4_task_01_report.md).

**R5 worker 24 (2026-05-30):** `CGame.bByte_4c8` → **`bMenuLastSplashFlag`**; tail `+0x4b0..+0x4c8` table + splash-flag flow. [round5_worker_24_report.md](./round5_worker_24_report.md).

### Offset alias table (`CBulanci` abs → `game` rel)

| CBulanci abs | `game` rel | Field |
|--------------|------------|-------|
| `+0x4b0` | `+0x22c` | `pAudioBankArray` |
| `+0x4b8` | `+0x234` | `dwAudioBankCount` |
| `+0x4c0` | `+0x23c` | `pReleaseOnDestroy` |
| `+0x4c4` | `+0x240` | `pCmdLine` |
| `+0x4c8` | `+0x244` | `bMenuLastSplashFlag` |

Base: `CBulanci+0x284` = `&this->game`.

### `CGame` tail @ `CBulanci+0x4b0..+0x4c8` (embed `game+0x22c..+0x244`)

| CBulanci abs | `game` rel | Name | Evidence |
|--------------|------------|------|----------|
| `+0x4b0` | `+0x22c` | `pAudioBankArray` | `CBulanci_ctor@0x0040275b`; `CBulanci_ReleaseAudioBank@0x00407e10`; dtor loop |
| `+0x4b4` | `+0x230` | `dwReserved_beforeBankCount` | Ctor zero only — no consumer (dead store) |
| `+0x4b8` | `+0x234` | `dwAudioBankCount` | `CBulanci_DestroyEmbedFields@0x00402943`; dtor `ReleaseAudioBank` loop |
| `+0x4bc` | `+0x238` | `dwAudioBankCapacity` | Ctor `= 8` @ `0x0040276f` |
| `+0x4c0` | `+0x23c` | `pReleaseOnDestroy` | `CBulanci_OnCreate@0x00402b20` UI SFX bank from master pack |
| `+0x4c4` | `+0x240` | `pCmdLine` | `CBulanci_SetCmdLine@0x00401992`; `DestroyEmbedFields` `CDsStringReleaseHeader` |
| `+0x4c8` | `+0x244` | `bMenuLastSplashFlag` | Staging byte → `CMenu_ctor_with_ui` `lastSplashFlag`; see below |

**`bMenuLastSplashFlag` flow (R5-24):**

| Step | Address | Effect |
|------|---------|--------|
| Ctor zero | `0x004027a7` | `(game).bMenuLastSplashFlag = 0` |
| Post-match solo tie | `0x00401c59` | `CBulanci_ShowPostMatchScoreModal`: if `game+0x62..0x64` bytes match, set `CBulanci+0x4c8 = 1` |
| Menu event `0xcc` | `0x00402490` | `CMenu_ctor_with_ui(menu, cmd_arg, (game).bMenuLastSplashFlag)` → `CMenu.bLastSplashFlag`; then clear flag |
| Start auto-back | `CMenu.md` | When `bLastSplashFlag != 0`, `CMenu_CmdDispatch` case `0xc9` triggers synthetic back |

**Related (not tail byte):** `(game).pSchedTeamScoreScratch[2]` @ `CBulanci+0x4ac` — menu modal arm (`=1` before `DoModal`, `=0` after) @ `0x00402490`; `CBulanci_GetDataKindStubZero` reads same dword @ `+0x4ac`.

## UNK

- `CDSApp` interior gaps (`CDSApp.md`).
- `CBulanci_DestroyEmbedFields` MI vtable restore on `app` prefix (`vftable_sub04/sub10/sub18` literals).
- `CBulanciConfigStore` Ghidra struct vs `CDSChain_full`.
- `CGame` mid-body pads / chain interior (`CGame.md`).
- `CGaming` stack `game` vs embedded `g_pApp->game`.

## Follow-up

- Name remaining `CDSApp` pads in decompile (`field_0xNN` on legacy helpers).
- Typedef or replace stale `CBulanciConfigStore` with `CDSChain_full`.
