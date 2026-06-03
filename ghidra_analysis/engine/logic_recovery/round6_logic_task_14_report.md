# Round 6 logic — Task 14 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 14 |
| **title** | Logic sim_429_436: 0x0042abb9–0x0042b170 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — All 22 addresses decompiled via Ghidra MCP (`batch_decompile`, `get_xrefs_to`). Ghidra MCP disconnected before `disassemble_function`, rename/`set_function_this_type`, and `save_program`. No Frida required: behavior is proven from decompiler + xref graph + existing struct docs.

## 3. Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0042abb9` | `Catch@0042abb9` | SEH unwind: `Release` on `*(EBP-0x24)` object vtable+8; resume `0x0042abc9` | Decompile; paired with `Catch@0042ab28` path |
| `0x0042abcf` | `Catch_0042ab28_ShowMessageAndRelease` | If `g_pApp`: `MessageBoxW(g_pHwnd, text, caption\|default, 0x2010)` then `g_pApp->Release`; always `Release` on saved `EDI`; resume `0x0042aaf8` | Decompile; `g_pApp` / `g_pHwnd` globals |
| `0x0042ac20` | `CDSApp_DirtyRectList_InsertAt` | 16-byte `RECT` vector insert at index: `EnsureCapacity(count+1)`; `FUN_0042a130` memmove tail; copy 4×`int`; `*(this+8)++` | Decompile; xrefs from `FUN_0042ae50` @ `0x0042aedc`, `0x0042aeee` |
| `0x0042ac80` | `CDSImageMouse_GetTypeDescriptor` | RTTI/class id blob: `return &DAT_004b3b74` | Decompile; `CDSImageMouse.md` class id `0x35` |
| `0x0042aca0` | `CDSImageMouse_ScalarDeletingDtor_thunk` | **Not** `CDSChain_AdjustThisOffset` (manifest stale). IDSEventHandler vtable slot 3: MI adjustor → `CDSImageMouse_vDtor(&this[-1].nM_dirtyBottom, …)` | Decompile; `master_vtable_catalog.csv` `CDSImageMouse,0x00486f68,,3,0x0042aca0` |
| `0x0042acb0` | `CDSImageMouse_dtor` | `Release(pCursorSprite)`; `CDSObject_dtor` on `this+0x18` (`savedBackground`); restore `g_pCDSObject_vftable_IDSReferenced` | Decompile; `CDSImageMouse.md` layout |
| `0x0042ad20` | `CDSBackBuffer_GetClassTable` | `return &DAT_004b3b34` | Decompile |
| `0x0042ad30` | `CDSBackBuffer_OnEvent` | MI thunk → `CDSBackBuffer_DtorScalar` on `embeddedImage` adjustor base | Decompile; `CDSBackBuffer.md` |
| `0x0042ad40` | `CDSBackBuffer_dtor` | `CDSBackBuffer_FreeImageMember`; `CDSImage_dtor(&embeddedImage)`; patch back-buffer vtables | Decompile; callers embed @ `CDSApp+0x7c` |
| `0x0042adb0` | `CDSBackBuffer_DtorScalar` | `CDSBackBuffer_dtor`; `_free(this)` if `param_1&1` | Decompile |
| `0x0042add0` | `CDSApp_SetPendingChildView` | If `classId != *(this+0x27c)`: release `*(this+0xec)`; resolve `DAT_004b7c10[classId]`; AddRef `+8`; store `+0xec` / `+0x27c` | Decompile; xref `CDSApp_dtor@0x0042b577`, `FUN_0042c700` |
| `0x0042ae40` | `FUN_0042ae40` | EH helper: `*(this+8)=0`; `CDSApp_DirtyRectList_SetSize(this,0)` — clears dirty-rect subobject | Decompile; jmp targets in EH tables (per R5 worker 06) |
| `0x0042ae50` | `FUN_0042ae50` | Dirty-rect **upsert**: `FUN_0042a070` search; on miss `InsertAt`; on hit optional re-insert; else append 16-byte record | Decompile; **sole caller** `CDSApp_AddDirtyRectCoalesced@0x0042b886` |
| `0x0042af00` | `CDSImageMouse_CreateObject` | `OperatorNew(0x88)`; dual vtables; `pCursorSprite=NULL`; `CDSImage_InitDefaults(savedBackground)`; dirty rect zeroed | Decompile; `CDSImageMouse.md` size proof |
| `0x0042af60` | `CDSImageMouse_vDtor` | `CDSImageMouse_dtor`; conditional `_free` | Decompile |
| `0x0042afd0` | `FUN_0042afd0` | Partial `CDSApp` shell init (`CDSChained_ctor`, four vtables, back-buffer vtable words @ `param_1[0x1f/0x20]`, `CDSDirectSound_ctor` @ `+0x200`); **not** full ctor | Decompile; overlaps `CDSApp_ctor@0x0042b170` (R5: rename blocked) |
| `0x0042b130` | `CDSApp_GetClassTable` | `return &g_AppClassTable` | Decompile; `app_shell.md` vtable slot 0 |
| `0x0042b140` | `CDSApp_Referenced_GetTypeInfo` | MI scalar-deleting dtor thunk → `CDSApp_DtorScalar(this-0x10)` | Decompile |
| `0x0042b150` | `CDSApp_EventHandler_GetTypeInfo` | MI thunk → `CDSApp_DtorScalar(this-0x8)` | Decompile |
| `0x0042b160` | `CDSApp_Chain_GetTypeInfo` | MI thunk → `CDSApp_DtorScalar` on chain subobject (`&this[-1].field_0x27c`) | Decompile |
| `0x0042b170` | `CDSApp_ctor` | Full engine ctor: four vtables; class/registry strings; zero/init `backBuffer` @ `+0x7c`; physical rect; `keyDownBitmap`; `CDSDirectSound_ctor` @ `+0x200`; dirty-rect list capacity 8; `g_pApp`/`g_pInputChainHead`; `keyLatchByVk` memset; display mode 7; alpha LUT; registry `Windowed` → `bWindowed` | Decompile + comments; xref `CBulanci_ctor@0x00402724`; `CDSApp.md` |

### Dirty-rect subobject (shared by `0x0042ac20`, `0x0042ae40`, `0x0042ae50`)

`this` is a **12+ byte vector header** (not full `CDSApp`):

| Offset | Field | Use |
|--------|-------|-----|
| `+0x00` | `pRects` | Pointer to array of 16-byte records |
| `+0x08` | `count` | Live element count |

Parent storage: `CDSApp_ctor` zeros `pDirtyRectArray` @ `+0x254`, `nDirtyRectCount`, and sets capacity dword @ `pPad_260` to **8** (`CDSApp.md` `+0x254`/`+0x25c`).

### Manifest correction

Task JSON `function_names[4]` lists `CDSChain_AdjustThisOffset` @ `0x0042aca0`. Ghidra and `master_vtable_catalog.csv` prove **`CDSImageMouse_ScalarDeletingDtor_thunk`** (vtable `0x00486f68` slot 3). No `CDSChain_AdjustThisOffset` at this address.

## 4. Ghidra deltas

**None applied** — MCP session lost (`Not connected`) after decompile/xref pass.

**Queued (evidence-backed, not executed):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `FUN_0042ae50` → `CDSApp_DirtyRectList_UpsertRect` | Sole xref `CDSApp_AddDirtyRectCoalesced` |
| `rename_function_by_address` | `FUN_0042ae40` → `CDSApp_DirtyRectList_Clear` | Body = zero count + `SetSize(0)` |
| `set_decompiler_comment` | `FUN_0042ae50` | Drop stale `UNCERTAIN` — caller/sibling calls proven |
| *(optional)* | `set_function_this_type` on dirty-rect helpers | Needs dedicated `CDSDirtyRectList` struct in Ghidra (not invented here) |

`CDSApp_ctor`, `CDSImageMouse_*`, `CDSBackBuffer_*` already typed and commented from prior passes.

## 5. Frida

**none** — Static decompile + xref + struct docs sufficient.

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| `FUN_0042afd0` vs `CDSApp_ctor` | Duplicate partial init; sole consumer of `CreateObject@0x42b910` path not traced this pass |
| `FUN_0042ae40` EH jmp sites | Disasm/xref to SEH tables not fetched (MCP down) |
| `CDSApp_SetPendingChildView` `DAT_004b7c10` | Class-id → instance table; entries not enumerated |
| Dirty-rect list Ghidra type | Helpers still `void *this` in `_Globals::` namespace |
| `Catch@0042abb9` parent function | Exception frame layout @ `EBP-0x24` not mapped to throwing ctor |

## Cross-links

- [`CDSApp.md`](../struct_recovery/CDSApp.md) — `backBuffer`, dirty rects, `keyLatchByVk`, ctor @ `0x0042b170`
- [`CDSImageMouse.md`](../struct_recovery/CDSImageMouse.md) — factory/dtor/draw band
- [`CDSBackBuffer.md`](../struct_recovery/CDSBackBuffer.md) — embed @ `CDSApp+0x7c`
- [`app_shell.md`](../app_shell.md) — primary vtable; `FUN_0042add0` in `CDSApp_dtor` slot 2 body
- [`main_menu.md`](../../main_menu.md) — `CDSApp_AppMain` / `CBulanci_ctor` chain into `CDSApp_ctor`
