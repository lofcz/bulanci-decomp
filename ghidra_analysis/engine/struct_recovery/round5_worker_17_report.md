# Round 5 worker 17 — FUN_* ctor/init paths (OperatorNew, vtable setup)

## Task

| Field | Value |
|-------|-------|
| **Worker** | 17 / 50 |
| **Scope** | Cross-cutting `FUN_*` in allocation + constructor/init paths |
| **Mode** | WRITE — evidence from `OperatorNewWithBadAlloc` xrefs, field writes, vtable data xrefs |
| **Status** | **DONE** (7 renames + 2 `set_function_this_type` + 1 prototype) |

## Summary

Harvested unnamed helpers reachable from `OperatorNewWithBadAlloc@0x00447c42` and from EH vector ctor/dtor tables in `CGame_ctor` / `CDSChain_ctor`. Renamed only where disassembly or vtable data xrefs fix role (factory alloc, vtable patch, POD vector elem ctor, scoreboard static-text factory).

## Evidence — renames applied

| Address | Old name | New name | Proof |
|---------|----------|----------|-------|
| `0x0042d8c0` | `FUN_0042d8c0` | `CBulanci_CreateCDSFileStream` | `PUSH 0x20` → `CALL 0x00447c42`; `MOV [EAX],0x0047f780` (+ MI @ `+4/+c/+14`); `CALL CDSFileStream_Open`; xrefs `CGaming_LoadLevelAssetAndMusic`, `CGaming_LoadBackgroundMusic` |
| `0x004096d0` | `CStaticText_CreateAndAddChild` | `CScore_AllocStaticTextAt` | `PUSH 0x98` → `OperatorNew`; `CALL CStaticText_BuildAt`; `CALL CDSView__AddChild`; sole caller `CScore_RenderHighScoreRow` (5×) |
| `0x0041b420` | `CBulanek_AllocAiTrackHolder` | `CBulanci_AppendZoneRect` | `OperatorNew(0x20)`; copies rect → node; `CIntListInsertSortedOrAppend(this+0x2d8)`; callers `DefineDangerZone`, `DefineTraceArea`, `CMina_RegisterDangerZone`, `CBulanekCtor` |
| `0x004097d0` | `FUN_004097d0` | `CGame_PlayerRec_ctor` | DATA xref from `CGame_ctor` + `CDSChain_ctor` as `_eh_vector_constructor_iterator_` elem ctor; pairs `CGame_PlayerRec_dtor@0x004097e0` |
| `0x00409010` | `FUN_00409010` | `Int32_ZeroInit_fastcall` | `*param_1 = 0`; 21 sites — MSVC vector default-init for 4-byte elements |
| `0x00401340` | `CDSString_handle_EH_dtor` | `CDsString_EhVectorElemDtor` | If `*ptr != 0` → `CDsStringReleaseHeader(ptr-0xc)`; EH unwind helper |
| `0x004075f0` | (already `CRadio_AddOption`) | `CRadio_AddOption` | `OperatorNew(8)` + `CDsStringAssignFromHandle` + `CIntListInsertSortedOrAppend(this+0x6c)`; callers `CStartGame1_BuildUi` / `CStartGame2` radio build |

## Evidence — already named (documented, no change)

| Address | Symbol | Role |
|---------|--------|------|
| `0x00419070` | `CBitmap_ViewHeader_Init` | `CDSChained_ctor` + 6× CBitmap vtable writes + `ODSImage::SetOwner`; called from `CAnim_ctor` chain |
| `0x00416590` | `CGameView_InitGamingFields` | Zeros `bView_state_68..6a`, `bPlayerSlot=0xff`, `pGaming_host=NULL` |
| `0x00434160` | `CDSStreamStorage_InitRootSafeStream` | `OperatorNew(0x48)` → `CDSSafeStream_ctor`; stores `pRootSafeStream` facet |
| `0x00434760` | `CDSStreamStorage_CreateFilterSafeStream` | `OperatorNew(0x38)` filter + `OperatorNew(0x48)` safe stream |
| `0x0042e910` | `HandleClassRegister` | Static class registry (`class_registry.py`) |

## Ghidra deltas

- `rename_function_by_address` × 7 (see table)
- `set_function_this_type`: `CBulanci_CreateCDSFileStream`, `CBulanci_AppendZoneRect` → `CBulanci *`
- `set_function_prototype`: `CScore_AllocStaticTextAt` (stdcall, `CScoreItem *` parent)
- `set_decompiler_comment@0x0042d903` (vtable install cluster)
- `save_program bulanci.exe`

## Remaining UNK (out of scope / blocked)

| Address | Notes |
|---------|-------|
| `FUN_00407e10` | `CDSPtrSlotVec_Resize` wrapper only — not alloc/vtable |
| `FUN_00405370` | `CStaticText` label refresh — runtime, not ctor |
| `FUN_0040b540` | Generic release-through-vtable+8 — needs owning-class proof per callsite |
| `FUN_00404890` | Shared `CDSView` teardown — **destructor** band (worker 18) |
| `FUN_0040f2c0` | Stack `CMsgDialog` + `DoModal` — exception UI, not persistent object ctor |
| `FUN_004014b0` | Path builder for dir enum — not object construction |
| `FUN_00403240` | `memmove` helper for vector erase — not ctor |

## Follow-up

- Tie `CBulanci_AppendZoneRect` return type to recovered 0x20-byte zone-node struct when layout is proven.
- Worker 19: vtable thunks/MI adjustors overlapping `CBitmap_ViewHeader_Init` facet writes.
