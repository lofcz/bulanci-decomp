# Round 3 — Task 13 report

## Task

| Field | Value |
|-------|--------|
| **id** | 13 |
| **title** | Trace CGame+0x66 writer for CLevelList resource table |
| **one_liner** | `CGame+0x66` / `+0x6e` are `chain.pLevelResourceTable` / `dwLevelResourceCount`; populated at boot, read by lobby/network and name lookup — not written with a bare `MOV [reg+0x66]`. |
| **acceptance** | Writer path proven; Ghidra comments/renames; update `CLevelList.md` / `CGame.md` |

## Status

**DONE**

## Offset map

| Absolute (`CGame`) | `CDSChain_full` | Ghidra field | Role |
|--------------------|-----------------|--------------|------|
| `+0x66` | `+0x35` (53) | `pLevelResourceTable` | `CIntList` head: `int*` vector of `CLevelScriptResource*` (MSVC `m_data` at `table-4` indexing) |
| `+0x6e` | `+0x3d` (61) | `dwLevelResourceCount` | `CIntList.m_count` — element count |

`CGame.chain` embed starts at `CGame+0x31`; `0x31+0x35=0x66`, `0x31+0x3d=0x6e`.

## Writer trace (sole population path)

```
CBulanci_InitResourceBank@0x00402180
  └─ CBulanci_BuildLevelResourceTable@0x0040a0b0  (&(param_1->game).chain)
       ├─ CBulanci_ClearLevelScriptList@0x00409f30   // free prior rows via CLevelScriptResource_dtor
       ├─ CBulanci_EnumerateLevelScripts@0x00409f60  (master pack)
       ├─ foreach *.pak under exe dir → OpenPackStream → EnumerateLevelScripts
       └─ if count>1: _qsort(pLevelResourceTable, count, 4, CGame_LevelResourceQsortCmp@0x00408ff0)
```

**Insert site** (`CBulanci_EnumerateLevelScripts@0x00409f60`):

| Step | Evidence |
|------|----------|
| Filter pack index rows | `*(int*)(row+0xc) == 0x7ea` (class **2026** level script) |
| Run script export | `CDSScript::CallExport(this,0,1,&stack)` |
| Allocate row | `OperatorNew(0x14)` → **20 B** `CLevelScriptResource` |
| Fill row | `[0]=resId`, `[1]=globalVar(1)` sort key, `[2]=name@gv0`, `[3]=pack AddRef`, `[4]=name@gv2` |
| **Write table** | `ADD ECX,0x35` @ `0x0040a062` → `CIntListInsertSortedOrAppend(&pLevelResourceTable, row, NULL, 1)` @ `0x00407e20` |

No instruction writes `dword ptr [reg+0x66]` directly (`search_instructions` **0** store hits on `0x66]`). Table pointer/count mutate only inside `CIntListInsertSortedOrAppend` / clear loop.

## Heap row layout (`CLevelScriptResource`, 0x14)

| Offset | Field | Writer | Reader / dtor |
|--------|-------|--------|----------------|
| `+0x00` | `dwResourceId` | pack entry `+8` | — |
| `+0x04` | `dwSortKey` | `CDSScript_GetGlobalVar(1)` | `CGame_LevelResourceQsortCmp` (`elem[1]`) |
| `+0x08` | `hNameAlt` | `GetGlobalVar(0)` → `CDsStringAssignFromLiteral` | `CLevelScriptResource_dtor@0x004097f0` |
| `+0x0c` | `pPackStream` | `AddRef` on enumerator stream | dtor `Release` |
| `+0x10` | `hLevelName` | `GetGlobalVar(2)` | `CGame_FindResourceByName`, `CMenu_OpenNetworkSession`, dtor |

## Readers (todo seed addresses)

| Address | Function | Use of `+0x66` / `+0x6e` |
|---------|----------|----------------------------|
| `0x00413560` | `CGame_FindResourceByName` | Walk `table[-4+idx*4]`; compare `row+0x10` via `CStartGame2_CompareLevelName` |
| `0x00414ee4` | `CMenu_OpenNetworkSession` | **Host:** loop `count` → copy `row+0x10` → `CGame_LevelList_AddByName` (`parent+0xbc` name list). **Join:** reverse walk table → pack 0x24-byte name slots into join msg |
| `0x0040d490` | `CLevelList_RenderItem` | UI only — `CListBoxItem` row tint/label (no `CGame+0x66`) |
| `0x00404c50` | `CItemInfo__CreateObject` | **Unrelated** to table — batch-12 metadata factory; xref DATA @ `0x0047b1d0` only |

Lobby UI rows (`CMenu::CLevelList_AddItem@0x0040d360`) use `CListBoxItem` (0x14) built from **name-list** handles (`levelEntry+8`), not the resource table directly.

## Ghidra deltas

- Renamed `FUN_004097f0` → **`CLevelScriptResource_dtor`**
- Decompiler comments @ `0x0040a062` (writer `ADD 0x35`), `0x004097f0`, `0x00408ff0` (qsort sort-key)
- `CDSChain_full.pLevelResourceTable` / `dwLevelResourceCount` already named @ `+0x35` / `+0x3d`
- `save_program bulanci.exe`

## Struct doc updates

- `CLevelList.md` — `CLevelScriptResource` heap row table; writer call graph
- `CGame.md` — todo-13 UNK cleared (writer resolved)

## Remaining UNK

- Formal Ghidra struct `CLevelScriptResource` (20 B) not created — layout proven from enum/dtor only
- `CItemInfo` (`0x00404c50`) still no gameplay consumers (batch 12)
- `CListViewer` / `CDSChained` interior inside `CLevelList.base` (other tasks)
