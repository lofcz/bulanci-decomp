# Round 6 — Logic task 08 report

## Task

| Field | Value |
|-------|-------|
| **id** | 8 |
| **title** | Logic cluster: CGaming_Ctor + scheduler/gaming neighbors |
| **range** | cluster_41ab (`0x0041a`–`0x0041b`) |
| **seed_address** | `0x0041ab70` |

## Status

**PARTIAL** — seed and all nine neighbors decompiled via Ghidra MCP; disasm proof collected for `CGaming_Ctor`. Ghidra MCP disconnected before `set_function_this_type` / rename / `save_program` could be applied.

## Functions

| Address | Name | Role summary | Evidence |
|---------|------|--------------|----------|
| `0x0041ab70` | `CGaming_Ctor` | **Partial heap ctor** (CreateObject path): `CDSChained_ctor`; `CDSUpdatedItem_ctor` @ `+0x68`; installs four MI vtables + event-handler facet vtable @ `+0x68`; constructs `apHudBitmaps[16]` @ `+0x88`; inits four `CDSPtrSlotVec` @ `+0x2C8..+0x2F8` (capacities 8); zeroes `hLevelTitleCopy`, `pLevelScript`, audio/script tail through `+0x360`. Returns `this`. **Not** the full stack ctor (`CGaming_ctor` @ `0x00420380`). | Disasm: `MOV ESI,ECX` @ `0x0041ab94`; vtable stores @ `0x0041abaf..0x0041abca`; slot vec inits @ `0x0041abf5..0x0041ac54`. Sole xref: `CreateObject@0x0041beb0` (`OperatorNew(0x36C)` then `CALL 0x0041ab70`). |
| `0x0041b5a0` | `CGaming_RetestTraceAreasForEntity` | When `vecSlotVec_2e8` (`+0x2E0` count, `+0x2D8` data) non-empty, reads entity bbox via vtable `+0x70`, then walks trace-area nodes calling `CGaming_CheckTraceAreasForEntity` with entity `+0x70` team/slot and `+0x198` mute flag. | Decompile; `CBulanek.md` `bDrawAsMuted` consumer @ `0x0041b5d2`; script zones live in `vecSlotVec_2d8` per `script_dispatch_table.md` opcode 73. |
| `0x0041b620` | `FUN_0041b620` | **Match-start scheduler seed:** for `pOwnerGame->bTotalSlots==1`, enqueues scheduler slot **0** if unarmed; if not demo (`pOwnerGame+0x05==0`) and `dwWeaponSpawnerMode>2`, enqueues slot **1**; if not demo, enqueues slot **2** when present and unarmed. All via `Scheduler_GetEventSlot(&schedulerFacet,+0x68)` + `Scheduler_EnqueueEvent`. | Decompile uses `pOwnerGame`, `dwWeaponSpawnerMode`, `field_0x68`; R5 worker 12: **1 xref**, caller `CGaming_RunPreMatchModal@0x0041c290`. Rename deferred until live xref disasm. |
| `0x0041b6d0` | `CGaming_LoadBackgroundMusic` | Opens level MP3 stream via `CBulanci_CreateCDSFileStream`, builds inline `CDSWavStream` (vtables @ `0x4823c0`…), stores stream ref, constructs `CDSAudioPlayer`, binds to `pLevelBgmPlayer` path (`+0x354` per `CGaming.md`). | Named in Ghidra; `match_orchestration.md`; xrefs cache vtable stores @ `0x0041b745..0x0041b760`. **Wrong `this`:** decompiler shows `CBulanci*` / `(this->game).*`. |
| `0x0041b821` | `Catch@0041b821` | MSVC SEH **`__stdcall`** unwind helper inside `CGaming_LoadBackgroundMusic` body (`0x0041b6d0`–`0x0041b850`). | `mapping.csv`; `CBulanci.h` decl; not a standalone callable entry. |
| `0x0041b850` | `CGaming_dtor` | Full teardown: restore vtables; level script export **#2** (`OnDeinit`); release ambient anims @ `+0x324..+0x330`; `CGaming_ClearAllEntities`; free danger zones; stop audio globals; release `pLevelBgmPlayer` / `pIntroMusicPlayer` / default SFX bank / `pLevelScript`; resize four `CDSPtrSlotVec`; destroy `apHudBitmaps`; `CDSUpdatedItem_dtor` + `CDSView_dtor`. | Decompile; vtable restore disasm @ `0x0041b87f..0x0041b89a` (xrefs cache); `main_menu.md` stack dtor call. |
| `0x0041ba60` | `CGaming_SetMusicAndSoundBank` | Level-script `SetMusic(trackId, bankIndexId)`: stores `trackId` @ `this+0x350` (`dwMusicResourceId`); if `bankIndexId!=0`, resolves resource via `CMenuGetResourceById(g_pApp+0x284, bankIndexId)` into `this+0x80` (`pPad_80` / prior bank holder). | Decompile `MOV [this+0x350]` comment; R5 worker 25 disasm @ `0x0041ba67`; plate comment in decompiler; `wave2_batch_13.md`. |
| `0x0041bab0` | `FUN_0041bab0` | **Pre-match scheduler enqueue walk:** calls `CDSApp_PreCreateHook` on embedded app facet, then iterates scheduler slot count downward enqueueing every live unarmed slot via `Scheduler_EnqueueEvent`. | Decompile UNCERTAIN comment; R5 worker 12: **1 xref** from `CGaming_RunPreMatchModal`. Decompiler `this[1].pEntitySlots` indicates wrong `this` / struct layout — needs `set_function_this_type`. |
| `0x0041bb00` | `CLevelScriptOpExt_SetMusic` | Script VM export **#9** (`SetMusic`): reads two `i32` operands; calls `CGaming_SetMusicAndSoundBank(*(void**)(script+0x458), track, bank)`. | `script_dispatch_table.md` row 54; decompile. |
| `0x0041bb30` | `CLevelScriptOpExt_DefineDangerZone` | Script export **#28**: `CDSScript_ReadKindAndRect4` → `CGaming_AppendDangerZoneNode(pGaming, …)` appends CMine-shaped zone to `vecSlotVec_2d8` (`+0x2D8`). | `script_dispatch_table.md` row 73; decompile (pGaming via `CLevelScript+0x458`, not `param_1[1].nOpcodeCount`). |

### CGaming_Ctor vs CGaming_ctor (re-verified)

Prior docs center on **`CGaming_ctor@0x00420380`** (full stack construction in `CGame_StartGame`: HUD preload, level script `OnInit`, panels, audio). Task seed **`CGaming_Ctor@0x0041ab70`** is a **distinct, shorter** (`0x123` B) partial initializer used only from **`CreateObject@0x0041beb0`** after `OperatorNew(0x36C)`. It establishes MI/shell + scheduler embed + pointer vectors but does **not** run level load, script export #1, or HUD asset preload. Both write the same vtable set at `+0x00`, `+0x04`, `+0x10`, `+0x18`, and facet @ `+0x68`.

## Ghidra deltas

**None applied** (MCP disconnected after initial decompile/disasm pass).

Pending when MCP is back:

| Action | Target | Proof |
|--------|--------|-------|
| `set_function_this_type` | `0x0041b6d0` → `CGaming *` | Function stores BGM player on CGaming tail fields; mis-typed as `CBulanci*`. |
| `set_function_this_type` | `0x0041ba60` → `CGaming *` | Writes `[ECX+0x350]` / `[ECX+0x80]` — CGaming offsets per `CGaming.md`. |
| `set_function_this_type` + `force_decompile` | `0x0041bab0` → `CGaming *` | Decompile uses bogus `this[1].pEntitySlots`; should use `schedulerFacet` @ `+0x68`. |
| `set_function_prototype` | `0x0041ab70` | `CGaming * __fastcall CGaming_Ctor(CGaming *this)` (ECX=this @ `0x0041ab94`). |
| `rename_function_by_address` | `0x0041b620` → `CGaming_SeedMatchSchedulerEvents` | R5 w12: sole caller `CGaming_RunPreMatchModal`; body seeds slots 0/1/2. |
| `rename_function_by_address` | `0x0041bab0` → `CGaming_EnqueuePreMatchSchedulerSlots` | R5 w12: sole caller `CGaming_RunPreMatchModal`; scheduler walk. |
| `save_program` | `bulanci.exe` | Once after above. |

## Frida

**Existing:** [`scripts/frida/bulanci_audio_logger.js`](../../scripts/frida/bulanci_audio_logger.js) — hooks `CGaming_SetMusicAndSoundBank@0x0041ba60` (track + bank index written to `CGaming+0x350` / `+0x80`). Use during level load with script `SetMusic` opcodes to confirm runtime `pGaming` pointer matches `CLevelScript+0x458`.

**None new** — static decompile + prior R5 xref notes sufficient for all functions except `FUN_0041bab0` interior scheduler walk (needs `set_function_this_type` before trustworthy decompile).

## Remaining UNK

- Exact **`CALL` sites** inside `CGaming_RunPreMatchModal@0x0041c290` for `FUN_0041b620` / `FUN_0041bab0` (R5 xref count only; live disasm not captured this session).
- **`FUN_0041bab0`**: which subobject hosts `CDSApp_PreCreateHook` target (decompiler hypothesis `this+0x440` timer table vs scheduler facet) — blocked on `this` fix.
- **`Catch@0x0041b821`**: exact EH frame layout (handler only; no gameplay semantics).
- Whether heap **`CreateObject`** path is exercised at runtime vs stack **`CGaming_ctor`** only — needs Frida breakpoint on `0x0041beb0` vs `0x00420380` if product behavior unclear.

## Struct doc touch

Updated [CGaming.md](../struct_recovery/CGaming.md): documented dual ctor paths (`CGaming_Ctor` partial @ `0x0041ab70` vs full `CGaming_ctor` @ `0x00420380`).
