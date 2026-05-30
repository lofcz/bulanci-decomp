# Round 4 — Task 13 report

## Task

| Field | Value |
|-------|--------|
| **id** | 13 |
| **title** | Create Ghidra CLevelScriptResource 0x14 row struct |
| **priority** | medium |
| **source** | handoff (supersedes R3 todo 13) |
| **types** | CLevelList, CGame, CDSChain, CItemInfo |
| **addresses** | `0x004097f0`, `0x00409f60`, `0x0040a062`, `0x00408ff0` |

## Status

**DONE**

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Row size `0x14` | `CBulanci_EnumerateLevelScripts@0x00409f60` | `OperatorNewWithBadAlloc(0x14)` before fill (R3) |
| Ghidra struct 20 B | `get_struct_layout CLevelScriptResource` | `dwResourceId`, `dwSortKey`, `dwNameAlt`, `pPackStream`, `dwLevelName` |
| Dtor field access | `CLevelScriptResource_dtor@0x004097f0` | Decompile: `this->dwLevelName`, `this->pPackStream->pVftable+8` Release, `this->dwNameAlt` |
| Qsort on sort key | `CGame_LevelResourceQsortCmp@0x00408ff0` | Prototype `CLevelScriptResource **elemA/B`; body `(*elemA)->dwSortKey - (*elemB)->dwSortKey` |
| Writer unchanged | `0x0040a062` | R3 plate: `ADD ECX,0x35` → `CIntListInsertSortedOrAppend` on `chain.pLevelResourceTable` |

**Semantic note:** `dwNameAlt` / `dwLevelName` are CDsString handles (same pattern as `CListBoxItem.dwLabelStringHandle`); docs may refer to them as `hNameAlt` / `hLevelName`.

## Ghidra deltas

- `create_struct CLevelScriptResource` — 20 B, 5 fields (see layout below).
- `modify_struct_field`: `pPackStream` → `IDSStream *` @ `+0xc`.
- `set_function_prototype` / `set_function_this_type`: `CLevelScriptResource_dtor@0x004097f0` → `CLevelScriptResource::CLevelScriptResource_dtor(CLevelScriptResource *this)`.
- `set_function_prototype`: `CGame_LevelResourceQsortCmp@0x00408ff0` → `int __cdecl (CLevelScriptResource **, CLevelScriptResource **)`.
- `set_decompiler_comment@0x00409f60` — row alloc + table append context.
- `force_decompile` @ `0x004097f0`, `0x00408ff0`.
- `save_program bulanci.exe`.

```
Structure: CLevelScriptResource  Size: 20
  dwResourceId @ 0
  dwSortKey @ 4
  dwNameAlt @ 8
  pPackStream (IDSStream *) @ 12
  dwLevelName @ 16
```

## Struct doc updates

- `CLevelList.md` — Ghidra apply: `/CLevelScriptResource` created; dtor/qsort typed.
- `CGame.md` — R4 cross-link for heap row type.

## Remaining UNK

- `CBulanci_EnumerateLevelScripts@0x00409f60` still uses `undefined4 *puVar6` for the new row — no ctor symbol; field writes via pointer arithmetic (acceptable).
- `CGame_FindResourceByName` / `CMenu_OpenNetworkSession` still index `CIntList` as `int*` table — row pointers not auto-cast to `CLevelScriptResource*` in those loops (separate typing pass).
- `CItemInfo` (`0x00404c50`) remains orthogonal (R3).
