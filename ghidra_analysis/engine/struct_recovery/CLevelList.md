# CLevelList

## Status

**VERIFIED** — size `0xe4` (228 bytes); **no fields beyond `CListBox`**. Subclass overrides five vtable slots and `CLevelList_RenderItem`; layout matches sized `CListBox` (`widgets.md` §14.1).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CLevelList) == 0xe4` | `0x0040ba51` | `CreateObject@0x0040ba30`: `PUSH 0xe4` → `OperatorNewWithBadAlloc` → `CLevelList_ctor` |
| Class registration | `0x0047b9b0` | `PUSH 0x40ba30`; `PUSH 0x4b33e0` (`CListBox` parent); `PUSH 0x80c` (2060); `MOV ECX,0x4b36b8` |
| Same size in menu embed | `0x00474693` area | `CMenu` ctor path: `OperatorNewWithBadAlloc(0xe4)` → `CLevelList_ctor`; stored at `CMenu+0xb4` (`bulanci.ghidra.exe.c`) |
| Matches `CListBox` alloc | `0x00408df0` | `CListBox_Allocate`: `OperatorNewWithBadAlloc(0xe4)` — base list widget size |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00..+0xe3` | 228 | `CListBox` | `base` | `CLevelList_ctor@0x0040b0c0` → `CListBox_BuildAt`; `CLevelList_RenderItem@0x0040d490` uses `base.dwTextShaperFlags` (`+0xd0`), `base.dwTintColor` (`+0xe0`) |

No derived fields (contrast `CChatList` `+0xe4` callback — not present on `CLevelList`).

### `base` (`CListBox`) — `p_base` resolved (round 3 task 49)

Former Ghidra field `CListBox.p_base` (`undefined1[204]`) is the embedded **`CListViewer`** subobject (`get_struct_layout`: `listViewer` @ `+0`, 204 B). `CLevelList` reaches list/scroller state only through this embed plus the six `CListBox` tail fields.

| `CLevelList` abs. | `CListBox` rel. | Component | Field (Ghidra) | Evidence |
|-------------------|-----------------|-----------|----------------|----------|
| `+0x00..+0x67` | `listViewer+0x00` | `CDSChained` (in `pPad_base`) | vftables, bbox, chain | `CLevelList_ctor@0x0040b0c0` writes vtables at `+0`, `+4`, `+0x10`, `+0x18`, `+0x68` via `p_base` → now `listViewer` |
| `+0x68..+0x7f` | `+0x68` | `CDSUpdatedItem` | tick facet | `CScroller_ctor@0x004033d0` → `CDSUpdatedItem_ctor(this+0x68)` |
| `+0x80..+0x9b` | `+0x80..+0x9b` | `CScroller` | scroll metrics, `CScrollBar*` children | `widgets.md` §13; `CScroller_BuildAt@0x004089c0` |
| `+0x9c..+0xcb` | `+0x9c..+0xcb` | `CListViewer` | `dwLayoutFlags`, `nColumnWidth_px`, `pItems`, counts, `nScrollStep` | `CListViewer_ctor@0x00407f50` (`param_1[0x28..0x32]`); `CListViewer_AddItem@0x00405e10` → `nItemCount`, `pItems` |
| `+0xcc` | `+0xcc` (`labelFont`) | `CListBox` | row label font | `CListBox_BuildAt@0x00408cc0` |
| `+0xd0` | `dwTextShaperFlags` | `CListBox` | TextShaper flags | `CLevelList_RenderItem@0x0040d490` |
| `+0xd4..+0xdc` | style state | `CListBox` | `dwStyleState_*`, `styleBlock` | `CListBox_BuildAt@0x00408cc0` |
| `+0xe0` | `dwTintColor` | `CListBox` | row tint | `CLevelList_RenderItem@0x0040d490` |

**Construction chain:** `CLevelList_ctor` → `CListBox_BuildAt` → `CListViewer_BuildAt((CListViewer *)this, …)` (`CListBox_BuildAt@0x00408cc0` decompile). Level rows use `CListViewer_AddItem` on the embedded viewer (`main_menu.md` §8.6).

**Item row payload** (heap, not in `CLevelList`): `CListBoxItem` (20 B) — see `CListBoxItem.md`; `CLevelList_RenderItem` reads `item+0xc` (`dwRowFlags`), `item+0x10` (label handle).

### `CGame` level resource table (slice 12)

Lobby rows resolve packed level resources through `CGame`, not by scanning inside `CLevelList`:

| `CGame` offset | Role | Evidence |
|----------------|------|----------|
| `+0x66` | `pLevelResourceTable` — base of dword resource pointer array | `CGame_FindResourceByName@0x00413560` → `*(int*)(this+0x66 + (n-1)*4)` |
| `+0x6e` | `nLevelResourceCount` — walk index (decrements to 0) | same function; paired with `+0x66` |
| `+0xbc..+0xc4` | Level **name** list (`CDsString` handles) on parent lobby state | `CStartGame2_ctor@0x004104f0` loop: `parent+0xc4` count, `parent+0xbc` name array → `CGame_FindResourceByName` → `CMenu::CLevelList_AddItem` |

`CMenu_OpenNetworkSession@0x00414ee4` also reads `CGame+0x66` when joining (see `main_menu.md` §8.6). **Writer (task 13):** `CBulanci_InitResourceBank` → `CBulanci_BuildLevelResourceTable@0x0040a0b0` → `CBulanci_EnumerateLevelScripts@0x00409f60` → `CIntListInsertSortedOrAppend` on `chain+0x35` (`ADD ECX,0x35` @ `0x0040a062`); no bare `MOV [reg+0x66]`. See `round3_task_13_report.md`.

### `CLevelScriptResource` heap row (0x14, not in `CLevelList`)

Ghidra type **`CLevelScriptResource`** (20 B) — R4 todo 13; writer trace R3 todo 13.

| Offset | Ghidra field | Role | Evidence |
|--------|--------------|------|----------|
| `+0x00` | `dwResourceId` | pack entry id | `CBulanci_EnumerateLevelScripts@0x00409f60` |
| `+0x04` | `dwSortKey` | `GetGlobalVar(1)` | `CGame_LevelResourceQsortCmp@0x00408ff0` → `(*elem)->dwSortKey` |
| `+0x08` | `dwNameAlt` | CDsString handle (`GetGlobalVar(0)`) | `CLevelScriptResource_dtor@0x004097f0` |
| `+0x0c` | `pPackStream` | `IDSStream *` pack ref | dtor `pPackStream->pVftable` Release |
| `+0x10` | `dwLevelName` | CDsString handle (`GetGlobalVar(2)`) | `CGame_FindResourceByName` compares `row+0x10` |

## Leaf functions (Ghidra names)

| Address | Symbol | Role |
|---------|--------|------|
| `0x0040ba30` | `CLevelList::CreateObject` | MFC factory: `OperatorNew(0xe4)` → `CLevelList_ctor` |
| `0x0040b0c0` | `CLevelList::CLevelList_ctor` | `CListBox_BuildAt(30,270,230,440, font=0xbe)` + five vtables |
| `0x0040b120` | `CLevelList::CLevelList_GetTypeDescriptor` | returns `&LAB_004b36b8` |
| `0x0040d490` | `CLevelList::CLevelList_RenderItem` | row tint via `dwRowFlags` bit 0; 5 px left pad |
| `0x0040d360` | `CMenu::CLevelList_AddItem` | `OperatorNew(0x14)` → `CListBoxItem_ctorWithAssoc` → `CListViewer_InsertItemAt` |

## ctor / vtable highlights

| Address | Notes |
|---------|-------|
| `0x0040b0c0` | `CListBox_BuildAt(this, 30, 270, 230, 440, _, font=0xbe)` then vtables `0x48077c`, `0x48075c`, `0x480744`, `0x480730`, `0x480718` at `listViewer+0`, `+4`, `+0x10`, `+0x18`, `+0x68` |
| `0x0040d490` | `CLevelList_RenderItem`: 5 px left pad vs `CListBox_RenderItem` 2 px; fallback string `DAT_004ae7a8` |

## Ghidra apply

- `CLevelList`: `base` (`CListBox`) @ 0 — 228 B (unchanged).
- **Round 3 / task 49:** `CListBox.p_base` → **`listViewer` (`CListViewer`, 204 B)**.
- **Slice 12 (2026-05-30):** `CLevelList::CreateObject@0x40ba30` (was `_Globals::CreateObject` collision); `CMenu::CLevelList_AddItem@0x40d360` prototype `void (CMenu*, int levelEntry)`.

**Agent todo 13 (2026-05-30):** `chain.pLevelResourceTable` / `nLevelResourceCount` (`CGame+0x66`/`+0x6e`); writer `CBulanci_EnumerateLevelScripts@0x0040a062`; `CBulanci_BuildLevelResourceTable@0x0040a0b0`; `CGame_LevelResourceQsortCmp@0x00408ff0`; `CGame_FindResourceByName` prototype; comments @ readers/host path.

**R4 todo 13 (2026-05-30):** `create_struct CLevelScriptResource` (20 B); `CLevelScriptResource_dtor` / `CGame_LevelResourceQsortCmp` prototypes use row type. See [round4_task_13_report.md](./round4_task_13_report.md).

```
Structure: CLevelList  Size: 228
  base (CListBox) @ 0
    listViewer (CListViewer) @ 0
    labelFont @ 204 (0xcc)
    dwTextShaperFlags @ 208
    dwTintColor @ 224
```

## UNK

- `CListViewer.pPad_base` / `CScroller.pPad_base` still opaque `byte[104]` in Ghidra (should be `CDSChained` — separate `CDSChained` recovery).
- `CListViewer.updatedItem` marked `-BAD-` at `+0x68` in Ghidra layout vs proven `CDSUpdatedItem` ctor site — fix in `CListViewer` / `CScroller` pass.
- `CListViewer` gaps `pPad_0xa8`, `pPad_scrollCalc` (`+0xa8..+0xbf`, `+0xbc..+0xc7`).

## Follow-up

- `p_base` byte blob decomposed → `listViewer` (`CListViewer`) embed — see **Layout** / Ghidra apply in this file (manifest id 49 is `CWeapon_ctor`; do not use `round3_task_49_report.md` for `CListBox`).
- Full `CDSChained` naming inside `CListViewer.pPad_base` — `CListViewer` / `CScroller` struct tasks (not `CLevelList`).
