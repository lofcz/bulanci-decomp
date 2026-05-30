# Round 5 — worker 24 report (CBulanci 1228 B tail)

## Task

| Field | Value |
|-------|-------|
| **worker** | 24 / 50 |
| **mode** | WRITE |
| **scope** | `CBulanci` **1228 B** object — embedded **`CGame`** tail (`CBulanci+0x4b0..+0x4c8` ≡ `game+0x22c..+0x244`) |
| **types** | `CBulanci`, `CGame`, `CMenu` |
| **acceptance** | Name/prove tail UNKs with func@addr; Ghidra field rename + decompile uses embed path; update `CBulanci.md` / `CGame.md`; `save_program` |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| No duplicate CBulanci tail fields | MCP | `get_struct_layout CBulanci` → 1228 B; members `app`, `pMainMenu`, `game` only |
| `CGame` tail sized through `+0x244` | MCP | `get_struct_layout CGame` → 584 B; `pAudioBankArray` @ 556 … `bMenuLastSplashFlag` @ 580 |
| Audio dynarray init | `0x0040275b` | `CBulanci_ctor`: `(game).pAudioBankArray=0`, `dwAudioBankCount=0`, `dwAudioBankCapacity=8` |
| `dwReserved_beforeBankCount` dead | `0x00402767` | Ctor zero only; no read xref in game code |
| UI SFX bank on embed | `0x00402b20` | `CBulanci_OnCreate`: release prior `(game).pReleaseOnDestroy`, assign from `(app).pMasterPackStorage` res `0x10004` |
| Cmdline on embed | `0x00401992`, `0x00402913` | `CBulanci_SetCmdLine` / `CBulanci_DestroyEmbedFields` → `(game).pCmdLine` |
| `bMenuLastSplashFlag` ctor zero | `0x004027a7` | `(game).bMenuLastSplashFlag = 0` |
| Post-match sets flag | `0x00401c59` | `CBulanci_ShowPostMatchScoreModal`: `CBulanci+0x4c8 = 1` when `game+0x62..0x64` byte condition (solo tie path) |
| Menu consumes flag | `0x00402490` | Case `0xcc`: `CMenu_ctor_with_ui(menu, cmd_arg, (game).bMenuLastSplashFlag)`; then `(game).bMenuLastSplashFlag = 0` |
| Maps to `CMenu.bLastSplashFlag` | `0x004265e0` | `CMenu_ctor_with_ui`: `this->bLastSplashFlag = lastSplashFlag` (param 3) |
| Release audio bank | `0x00407e10` | `CBulanci_ReleaseAudioBank`: `CIntList_BinarySearch` on `&(game).pAudioBankArray` (`CBulanci+0x4b0`) |
| Dtor bank loop | `0x00402c30` band | Dtor walks `(game).dwAudioBankCount` via `CBulanci_ReleaseAudioBank` |
| Menu modal scratch dword | `0x00402490` | `(game).pSchedTeamScoreScratch[2]` @ `CBulanci+0x4ac`: set `1` before `DoModal`, `0` after (separate from splash flag) |

## Ghidra deltas

- `modify_struct_field` `CGame.bByte_4c8` → **`bMenuLastSplashFlag`**
- `set_decompiler_comment` @ `0x00402490`, `0x004027a7`, `0x00402913`, `0x00401c59`
- `force_decompile` `CBulanci_OnEvent_MenuStateMachine@0x00402490` — shows `(game).bMenuLastSplashFlag`
- `save_program bulanci.exe`

## Struct doc updates

- [CBulanci.md](./CBulanci.md) — tail table `+0x4b0..+0x4c8`, splash-flag flow, UNK trimmed
- [CGame.md](./CGame.md) — `bMenuLastSplashFlag` layout row; dead-store note for `dwReserved_beforeBankCount`

## Remaining UNK (this slice)

- `CBulanci_DestroyEmbedFields` still restores raw `CDSApp` MI vtable slots on `app` prefix (not tail).
- `CBulanci_ShowPostMatchScoreModal` chain-byte predicate @ `game+0x62..0x64` — interior `CDSChain_full` field names deferred to chain task.
- `CGame` mid-body pads (`+0x28..`, demo journal embed, DirectPlay/recv multiplex) — see `CGame.md` follow-up.
