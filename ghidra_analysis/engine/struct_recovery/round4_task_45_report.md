# Round 4 — Task 45 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 45 |
| **round** | 4 |
| **title** | CloseStreamByKey pass CDSCollection* to FindKeyIndex |
| **source** | handoff |
| **supersedes_todo_id** | 45 |
| **one_liner** | Drop `(CDSStreamStorage *)&this->collection` cast; callee `FindKeyIndex` is `CDSCollection*` @ storage `+0x1c`. |
| **prior** | [round3_task_45_report.md](./round3_task_45_report.md) |
| **structs** | CDSStreamStorage, CDSCollection, CBulanci |

## Status

**DONE** — R4 renamed callee to `CDSCollection_FindKeyIndex`, confirmed `CDSCollection *this`, decompile call uses `&this->collection`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Caller passes embed base | `0x00433d09` | Asm `LEA ECX,[ESI+0x1c]` before `CALL 0x00431170` |
| Decompile call site | `0x00433cb0` | `CDSCollection::CDSCollection_FindKeyIndex(&this->collection, (int)&local_34, CDSStrmResInfo_CompareKey)` — no `CDSStreamStorage*` cast |
| Entry load uses collection | `0x00433d24` | `MOV ECX,[ESI+0x24]` → `(this->collection).pM_items[iVar2]` |
| Callee `this` type | `0x00431170` | `get_function_variables`: `this` = `CDSCollection *`, ECX auto |
| Callee body fields | `0x00431170` | Decompile: `this->pM_items`, `this->nM_count` (+0x08 / +0x0c) |
| Nested embed layout | struct | `CDSStreamStorage.collection` @ `+0x1c` (28); `pM_items` @ collection `+0x08` = storage `+0x24` |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x00431170` | Confirmed in class `CDSCollection`; `this` = `CDSCollection *` |
| `set_function_prototype` | `0x00431170` | `int __thiscall CDSCollection_FindKeyIndex(CDSCollection *this, void *pKey, void *compareFn)` |
| `set_decompiler_comment` | `0x00433d09` | R4: embedded collection ECX, not storage cast |
| `set_decompiler_comment` | `0x00431170` | ECX = collection; `m_items`/`m_count` offsets |
| `force_decompile` | `0x00431170`, `0x00433cb0` | Call/callee names and `&this->collection` verified |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSStreamStorage.md](./CDSStreamStorage.md) — R4 todo 45 apply log; decompiler note for `CloseStreamByKey` call fixed.
- [CDSCollection.md](./CDSCollection.md) — R4 todo 45: `FindKeyIndex` renamed / class membership closed.

## Remaining UNK

- `pKey` stack object in `CloseStreamByKey` still typed `int` in prototype (live path uses `CDSStrmResInfo` key facet + `CDSStrmResInfo_CompareKey`); out of scope for this task.
- `CDSStreamStorage_InitRootSafeStream` ctor call-site `(CBulanci *)this` quirk (task 46).
