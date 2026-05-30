# Round 3 — Task 45 report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 45 |
| **title** | Verify CDSStreamStorage nested CDSCollection/CDSChain decompile |
| **one_liner** | Replace flat `collection_*` / `chain_*` with nested `CDSCollection` @ `+0x1c` and `CDSChain` @ `+0x34`; confirm `CloseStreamByKey` passes embed base to `FindKeyIndex` and reads `m_items[i]`; retype `CBulanci_OpenPackStream` return as `CDSStreamStorage*`. |
| **acceptance** | `get_struct_layout` nested embeds; decompile `CloseStreamByKey` / ctor / `FindKeyIndex` / `OpenPackStream`; `save_program` |

## 2. Status

**DONE**

## 3. Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Nested `CDSCollection` @ storage `+0x1c` (28) | `get_struct_layout CDSStreamStorage` | 96 B; `collection` `CDSCollection` @ offset 28 (`0x1c`) |
| Nested `CDSChain` @ storage `+0x34` (52) | same | `chain` `CDSChain` @ offset 52 (`0x34`); `lock` `CRITICAL_SECTION` @ 72 (`0x48`) |
| Ctor initializes collection vtables + data | `CDSStreamStorage_ctor` @ `0x00401790` | Decompile: `(this->collection).pM_pVtable_*`, `pM_items`, `nM_count`, `nM_capacity`, `nM_growthChunk = 0x20` |
| Ctor initializes chain head | same | Decompile: `(this->chain).pVftable_*`, `pFirstChild`, `pAuxHeap`, `dwChildCount`; asm `MOV [ESI+0x34],0x47f6d4` @ `0x004017fa` |
| `CloseStreamByKey` passes collection base | `CDSStreamStorage_CloseStreamByKey` @ `0x00433cb0` | Asm `LEA ECX,[ESI+0x1c]` @ `0x00433d09`; `CALL FindKeyIndex` |
| Entry from `m_items[index]` | same | Asm `MOV ECX,[ESI+0x24]` @ `0x00433d24`; decompile `(this->collection).pM_items[iVar2]` |
| `FindKeyIndex` uses collection fields | `CDSStreamStorage_FindKeyIndex` @ `0x00431170` | After `set_function_this_type CDSCollection*`: `this->pM_items`, `this->nM_count`; linear / binary search |
| Loader walks embedded chain | `CDSStreamStorage_GetThreadLoaderNode` @ `0x00433f00` | Decompile: `CDSChained_GetFirstChildView(&this->chain)`; append via `CDSChained_AppendChild(&this->chain,…)` |
| `OpenPackStream` returns storage MI pointer | `CBulanci_OpenPackStream` @ `0x00401f36` | `OperatorNew(0x60)` + `CDSStreamStorage_ctor`; return `(CDSStreamStorage *)&pCVar7->pVftable_IDSStorage` |
| `CDSCollection` / `CDSChain` sizes | layouts | `CDSCollection` 24 B (`pM_items` @ `+0x08`); `CDSChain` 20 B (`pFirstChild` @ `+0x08`) |

## 4. Ghidra deltas

- Verified nested struct fields already present (`CDSCollection collection`, `CDSChain chain`).
- `set_function_this_type CDSCollection *` @ `0x00431170` — `FindKeyIndex` decompile now uses `pM_items` / `nM_count`.
- `set_function_this_type CDSStreamStorage *` @ `0x00401790` — ctor decompile uses `(this->collection).*` and `(this->chain).*`.
- `save_program bulanci.exe` — once at end.

## 5. Struct doc updates

- [CDSStreamStorage.md](./CDSStreamStorage.md) — **Round 3 (task 45)** verification: nested embed decompile + `set_function_this_type` refresh (supersedes task 44 “flat fields retained” note).

## 6. Remaining UNK

- `CloseStreamByKey` still casts `(CDSStreamStorage *)&this->collection` into `FindKeyIndex` (harmless; prefer `&this->collection` / `CDSCollection*` prototype).
- `CDSStreamStorage_InitRootSafeStream` call from ctor decompiles with `(CBulanci *)this` (shared-body / prototype quirk; task 46 loader protos).
- `GetStreamCount` / `GetStreamEntry` IDSStorage stubs unchanged (task 46).
