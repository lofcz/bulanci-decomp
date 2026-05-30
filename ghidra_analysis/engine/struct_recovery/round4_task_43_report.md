# Round 4 — Task 43 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 43 |
| **round** | 4 |
| **title** | Retype FindKeyIndex; fix InsertKeyed callee cast |
| **source** | `agent_todos_50_r4.json` (handoff) |
| **prior** | [round3_task_43_report.md](./round3_task_43_report.md) |
| **types** | CDSCollection, CDSStreamStorage, CDSUpdatedItem |
| **addresses** | `0x00431170`, `0x004312c0`, `0x00433cb0`, `0x0042f060` |

## Status

**DONE** — `CDSCollection_FindKeyIndex` lives in class `CDSCollection` with `CDSCollection *this`; `InsertKeyed` decompile calls `CDSCollection_FindKeyIndex(this,…)` (no `CDSStreamStorage*` cast). `CloseStreamByKey` uses `&this->collection` (see also [round4_task_45_report.md](./round4_task_45_report.md)).

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Callee symbol / class | `0x00431170` | `get_function_variables`: `CDSCollection_FindKeyIndex`; `this` = `CDSCollection *`, `ECX:4 (auto)` |
| Callee body fields | `0x00431170` | Decompile: `this->pM_items`, `this->nM_count` (+0x08 / +0x0c) |
| InsertKeyed callee | `0x004312c0` | `CDSCollection_FindKeyIndex(this,key,compareFn)` — was `CDSStreamStorage_FindKeyIndex((CDSStreamStorage *)this,…)` after R3 |
| InsertKeyed `this` | `0x004312c0` | `CDSCollection::CDSCollection_InsertKeyed(CDSCollection *this,…)`; asm `MOV ESI,ECX` @ `0x004312d2` |
| CALL site asm | `0x004312dc` | `CALL 0x00431170` with ECX = standalone collection (`ESI` from ctor path) |
| Caller not InsertKeyed `this` | `0x0042f107` | `CDSCollection_InsertKeyed(g_pTaskList,…)` from `CDSUpdatedItem_ctor` |
| Embedded caller | `0x00433cb0` | `CDSCollection_FindKeyIndex(&this->collection,…)`; asm `LEA ECX,[ESI+0x1c]` @ `0x00433d09` |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x00431170` | Confirmed `CDSCollection *` in class `CDSCollection` |
| `set_function_prototype` | `0x00431170` | `int __thiscall CDSCollection_FindKeyIndex(CDSCollection *this, int key, void *compareFn)` |
| `set_decompiler_comment` | `0x00431178`, `0x004312dc` | R4 todo 43: ECX bases / callee name |
| `force_decompile` | `0x00431170`, `0x004312c0`, `0x00433cb0`, `0x0042f060` | InsertKeyed + CloseStreamByKey + ctor refreshed |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSCollection.md](./CDSCollection.md) — R4 todo 43: InsertKeyed callee + `FindKeyIndex` symbol table rows.
- [CDSStreamStorage.md](./CDSStreamStorage.md) — cross-ref `CDSCollection_FindKeyIndex` @ `0x00431170` (R4 task 45).

## Remaining UNK

- `CDSUpdatedItem_ctor` body still names `pSelf` / `pEventSlots` where `g_pTaskList` dword walk touches `+0x8`/`+0xc` (collision with collection fields on wrong `this` view only).
- `compareFn` prototype remains `void *` (binary-search callback); key type `int` vs pointer keys unchanged.
