# Round 5 — Worker 12 Report

## Task

| Field | Value |
|-------|-------|
| **worker** | 12 / 50 |
| **title** | FUN_* CMenu/CStartGame/dialogs ~0x410000–0x428000 |
| **mode** | WRITE (evidence-only renames) |
| **program** | `bulanci.exe` via user-ghidra-mcp |
| **address_range** | `0x00410000`–`0x00428000` |

## Status

**DONE** — **35** unrename `FUN_*` entry points in slice; **10** menu/lobby/dialog helpers renamed with disasm + xref + caller proof; **25** gameplay/combat helpers documented **SKIP** (out of menu/dialog scope, deferred to other R5 workers).

## Evidence — renamed (menu / lobby / dialog)

| New name | Address | Callers (proof) | Disasm / behavior |
|----------|---------|-----------------|-------------------|
| `CGame_CopyPlayerBindingsFromProfile` | `0x00413620` | **`CMenu_OpenNetworkSession@0x00415017`** `CALL 0x00413620` (ECX=`CGame*`) | Zeros `+0xdc..` player rows; `CALL CMenu_GetProfileBindIndex@0x00412640`; copies 6-byte bind tuples from `+0x3e`; sets `bGameModeIndex`/`anScoreLimitByMode` when `bTotalSlots==1`; default `hLevelTitle` from `levelNameList[0]` @ `+0xbc` when empty (`0x0041376f`) |
| `CLevelNameList_FindIndex` | `0x00413440` | `CLevelNameList_FindIndexWithKey@0x00413c70` | Binary search when `param_2` comparator set; else linear scan via **`CStartGame2_CompareLevelName`** |
| `CLevelNameList_FindIndexWithKey` | `0x00413c70` | **`CGame_LevelList_AddByName@0x00414977`** | SEH wrapper; releases search-key `CDsString` in `finally` |
| `CLevelNameList_ZeroNewElements` | `0x00413410` | **`CLevelNameList_Resize@0x004146e2`** | Loop: `*param_1=0` for `param_2` new `CDsString*` slots |
| `CLevelNameList_Resize` | `0x004146b0` | **`CGame_LevelList_EnsureCapacity@0x00414761`**, **`CGame_dtor@0x00414d85`**, `CLevelNameList_Clear@0x00414919` | Realloc via `FUN_0042f730`; grow zeroes tail; shrink calls `CDsStringPtrArray_ReleaseRange` |
| `CLevelNameList_Clear` | `0x00414910` | **`CGame_dtor`** path | `*(this+8)=0`; `CLevelNameList_Resize(this,0)` |
| `Runtime_FreePointerFieldZero` | `0x004134f0` | MSVC **`Catch@0x004359f3`**, **`Catch@0x00435b6d`** (×2 each) | `Runtime_Free(&DAT_004b7c94,*field); *field=0` — EH cleanup only |
| `CDSVec_ZeroNewIntPairs` | `0x00424400` | **`CDSVec_ResizeIntPair@0x004250f2`** | Zero `{dword,dword}` pairs on vector grow (menu `CGunMouse` coord ring, `CPoemScroller` lists) |
| `CPoemScroller_ReleaseOwnedResources` | `0x00425060` | **`Destructor@0x00425cf7`**, **`CatchExceptionHelper@0x004264c8`** | `Scheduler_ArmSlot(+0x68,0)`; Release `+0xa8`; walk `+0x80` ptr array |
| `CDSPtrSlotVec_ResetOnUnwind` | `0x00414770` | MSVC **`Unwind@0x00476803`**, **`Unwind@0x004768cf`** | `*(this+0xc)=0`; `CDSPtrSlotVec_Resize(this+4,0)` — ctor-failure cleanup on lobby slot vec |

### Caller chain — network open → lobby (primary menu flow)

```
CMenu_OpenNetworkSession@0x00414dd0
  … level script enum → CGame_LevelList_AddByName@0x00414f09 …
  … CMenu_InitPlayerSlotFromDefaults@0x00414f17 …
  … CMenu_ShowConnectingDialog@0x00414ff1 …
  CALL CGame_CopyPlayerBindingsFromProfile@0x00415017   ; ECX = CGame*
  CALL CMenu_ShowLobby@0x00415022                       ; stack CStartGame2 modal
```

## Evidence — SKIP (in range, not menu/dialog)

| FUN_* (unchanged) | Address | Xrefs | Reason deferred |
|-------------------|---------|-------|-----------------|
| `FUN_004165b0` | `0x004165b0` | 2 | `CDSView__PostMessage` mine/explosion detach — gameplay entity |
| `FUN_004166d0` | `0x004166d0` | 2 | `CBulanek` anim anchor sum — entity movement |
| `FUN_00416700` | `0x00416700` | 4 | HUD ammo strip refresh via `FUN_00427c90` — in-match CGaming |
| `FUN_004172d0` | `0x004172d0` | 2 | `CBulanek` scheduler re-arm — combat AI |
| `FUN_00417910` | `0x00417910` | 2 | `CBulanek` TM pause + reposition — entity |
| `FUN_00417c80` | `0x00417c80` | 7 | `CBulanci` ambient-anim scheduler — level shell |
| `FUN_00417dd0` | `0x00417dd0` | 3 | Trace-area enter/leave — level script opcode 83 |
| `FUN_00417f40` | `0x00417f40` | 1 | `CGameView` bullet frame lookup — combat |
| `FUN_004180e0` | `0x004180e0` | 1 | Score HUD + `FUN_00427710` — match scoring |
| `FUN_00418d70` | `0x00418d70` | 1 | `CGameView`/shell `CBulanek` ctor — entity factory |
| `FUN_00419b40` | `0x00419b40` | 1 | `CShot` scheduler track advance — projectile |
| `FUN_0041a270` | `0x0041a270` | 4 | `CGaming` owned-object release — match teardown |
| `FUN_0041ad80` | `0x0041ad80` | 14 | Scheduler tick-dispatch node alloc — shared engine |
| `FUN_0041af70` | `0x0041af70` | 3 | `CBulanek` movement/collision step — entity |
| `FUN_0041b190` | `0x0041b190` | 6 | `CAnim` scheduler hook — entity widget |
| `FUN_0041b3b0` | `0x0041b3b0` | 1 | `CGaming` custom-event entity purge — match |
| `FUN_0041b420` | `0x0041b420` | 4 | Danger/trace zone node alloc — level script |
| `FUN_0041b4a0` | `0x0041b4a0` | 1 | `CMina` zone node dtor helper — level script |
| `FUN_0041b620` | `0x0041b620` | 1 | `CGaming` scheduler seed — match start |
| `FUN_0041bab0` | `0x0041bab0` | 1 | `CGaming_RunPreMatchModal` scheduler walk — match |
| `FUN_0041bf80` | `0x0041bf80` | 3 | `CWeapon` track-holder swap — combat |
| `FUN_0041ed10` | `0x0041ed10` | 1 | `CBulanek` weapon-bar sync — entity |
| `FUN_00427710` | `0x00427710` | 3 | CGaming+0x320 round-timer label — in-match HUD |
| `FUN_00427c30` | `0x00427c30` | 1 | HUD weapon-strip ODSImage swap — in-match |
| `FUN_00427c90` | `0x00427c90` | 2 | CGaming HUD per-player ammo refresh — in-match |

## Ghidra deltas

- `rename_function_by_address` ×10 (table above)
- `set_decompiler_comment` @ `0x00413620`, `0x00415017`
- `save_program bulanci.exe`

## Struct doc updates

- [CGame.md](./CGame.md) — `levelNameList` evidence strings updated to R5 symbol names

## Remaining UNK

- **`CGame_CopyPlayerBindingsFromProfile`** still `__fastcall` in Ghidra (ECX=`CGame*`); `set_function_this_type` blocked until prototype switched to `__thiscall`.
- **`CLevelNameList_*`** vector at `CGame+0xbc` not yet a standalone Ghidra struct (inline `{pData,count,cGrow}` layout @ `+0x0/+0x4/+0x8/+0xc`).
- **25 gameplay `FUN_*`** in slice — assign to entity/combat R5 workers or global high-xref pass (worker 16).
- **`Runtime_FreePointerFieldZero`** — shared EH helper; confirm all 4 catch sites are menu-adjacent vs generic engine.
