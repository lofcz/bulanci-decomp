# CDSStreamStorage

## Status

**VERIFIED** — allocation `0x60`; vtables, refcount, stream position, root safe-stream, embedded `CDSCollection` (`+0x1c`, see `CDSCollection.md`), embedded `CDSChain` list-head (`+0x34`, see `CDSChain.md`), and `CRITICAL_SECTION` at `+0x48`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSStreamStorage) == 0x60` | `0x0057214` | `CBulanci::OpenPackStream` path: `OperatorNewWithBadAlloc(0x60)` then `CDSStreamStorage_ctor` |
| Last initialized byte `+0x47`, lock at `+0x48` | `0x00401790` | `CDSStreamStorage_ctor`: `InitializeCriticalSection((LPCRITICAL_SECTION)(this + 0x48))` |
| Lock destroyed at `+0x48` | `0x00433e60` | `CDSStreamStorage_dtor`: `DeleteCriticalSection((LPCRITICAL_SECTION)(this + 0x48))` |
| `CRITICAL_SECTION` size `0x18` on Win32 | — | `0x48 + 0x18 = 0x60` matches allocation |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CDSStreamStorage_ctor@0x00401790` → `CDSStreamStorage::vftable`; dtor restore @ `0x00433e60` |
| `0x04` | 4 | `void *` | `pVftable_IDSStorage` | ctor → `g_pCDSStreamStorage_vftable_IDSStorage`; dtor restore |
| `0x08` | 4 | `void *` | `pVftable_IDSEventHandler` | ctor / dtor third vtable write |
| `0x0C` | 4 | `int` | `refCount` | ctor `= 1`; `CDSStreamStorage_ReleaseRefcount@0x004018c0` inc/dec |
| `0x10` | 8 | `uint64` | `streamPosition` | `FUN_00434160@0x00434160` stores `IStream::Seek` result; `FUN_00433f70@0x00433f70` reads for splice |
| `0x18` | 4 | `void *` | `pSafeStream` | ctor `= 0` then `FUN_00434160` assigns `CDSSafeStream*`; dtor releases via vtable `+8` |
| `0x1C` | `0x18` | `CDSCollection` | `collection` | Embedded collection (see [CDSCollection.md](./CDSCollection.md)); ctor writes vtables at `+0x1c`/`+0x20` |
| `0x1C` | 4 | `void *` | `collection.m_pVtable_IDSReferenced` | `CDSStreamStorage_ctor@0x00401790` → `CDSCollection::vftable` |
| `0x20` | 4 | `void *` | `collection.m_pVtable_IDSChained` | ctor → `g_pCDSCollection_vftable_IDSChained` |
| `0x24` | 4 | `void **` | `collection.m_items` | `FindKeyIndex@0x00431170` with `ECX=this+0x1c` reads `[EBP+0x8]`; `CloseStreamByKey@0x00433cb0` `MOV ECX,[ESI+0x24]` then `[ECX+EDI*4]` |
| `0x28` | 4 | `int` | `collection.m_count` | `FindKeyIndex` reads `[EBP+0xc]`; ctor zero |
| `0x2C` | 4 | `int` | `collection.m_capacity` | ctor zero; `CDSCollection_Resize` / `EnsureCapacity` |
| `0x30` | 4 | `int` | `collection.m_growthChunk` | ctor `= 0x20` |
| `0x34` | `0x14` | `CDSChain` | `chain` | ctor installs chain vtables + zeros; `CDSChain_dtor@0x00433e60` on `this+0x34`; `FUN_00433f00@0x00433f00` walks children |
| `0x48` | `0x18` | `CRITICAL_SECTION` | `lock` | ctor `InitializeCriticalSection`; `FUN_00433f70` `Enter/Leave`; dtor `DeleteCriticalSection` |

## Named methods (slice 40 / Ghidra)

| Address | Symbol | Role |
|---------|--------|------|
| `0x00401790` | `CBulanci::CDSStreamStorage_ctor` | Same body as standalone `OperatorNew(0x60)` ctor; xref `CBulanci_OpenPackStream@0x00401f36` |
| `0x00433e60` | `CDSStreamStorage_dtor` | Chain/collection/safe-stream teardown + `DeleteCriticalSection` @ `+0x48` |
| `0x00431170` | `CDSStreamStorage_FindKeyIndex` | Keyed index on embedded `CDSCollection` (`this+0x1c`) |
| `0x00433cb0` | `CDSStreamStorage_CloseStreamByKey` | Stack `CDSStrmResInfo` key → `FindKeyIndex` → `m_items[i]` |
| `0x00434160` | `CDSStreamStorage_InitRootSafeStream` | `CDSSafeStream` @ `pRootSafeStream` (`+0x18`); `IStream::Seek` → `streamBaseOffset` (`+0x10`) |
| `0x00433f00` | `CDSStreamStorage_GetThreadLoaderNode` | Per-thread loader node on embedded `CDSChain` @ `+0x34` |
| `0x00433f70` | `CDSStreamStorage_AppendOrReuseStream` | `EnterCriticalSection`; if `entry->streamExtent==0` append else `AddRefHeldObject`; append path reads `entry->filterSliceAddend` @ `+0x20` (`MOV EDX,[EDI+0x20]` @ `0x0043405b`) → `CreateFilterSafeStream` param_4 |
| `0x00434760` | `CDSStreamStorage_CreateFilterSafeStream` | Filter wrapper over root safe-stream slice |
| `0x004339c0` | `CDSStreamStorage_GetStreamCount` | IDSStorage +6: returns `m_items` ptr @ `+0x24` (misnamed; not `m_count`) |
| `0x004339d0` | `CDSStreamStorage_GetStreamEntry` | IDSStorage +7: vtable-only; indexes `m_pVtable_IDSChained` @ `+0x20` (dead) |
| `0x004392a0` | `CBulAnim::IDSAnim_SelectRandomTrack` | Fake `CDSStreamStorage*` @ `+0x1c` → reads `dwChainRoot` @ `+0x40` via stub |

## Ghidra apply

**Agent todo 45 (2026-05-30):** Rebuilt `CDSStreamStorage` (96 B / `0x60`) with nested embeds — flat `collection_*` / `chain_*` dword bands replaced by `CDSCollection collection` @ `+0x1c` (28) and `CDSChain chain` @ `+0x34` (52). `save_program bulanci.exe`.

**R3 worker #45 (2026-05-30):** Re-verified `get_struct_layout` — `CDSCollection collection` @ `+0x1c`, `CDSChain chain` @ `+0x34`, `CRITICAL_SECTION lock` @ `+0x48`. `CBulanci_OpenPackStream@0x00401f36` return `CDSStreamStorage *` (`&storage->pVftable_IDSStorage`). `CloseStreamByKey@0x00433cb0`: asm `LEA ECX,[ESI+0x1c]`; decompile `(this->collection).pM_items[i]`. `set_function_this_type` @ `0x00431170` (`CDSCollection*`) and `0x00401790` (`CDSStreamStorage*`) — ctor/`FindKeyIndex`/`GetThreadLoaderNode` use nested `collection`/`chain` members. `save_program bulanci.exe`. See [round3_task_45_report.md](./round3_task_45_report.md).

```
get_struct_layout CDSStreamStorage → 96 bytes
+0x00  pVftable_IDSReferenced
+0x04  pVftable_IDSStorage
+0x08  pVftable_IDSEventHandler
+0x0C  nRefcount
+0x10  dwStreamBaseOffsetLo / dwStreamBaseOffsetHi
+0x18  pRootSafeStream
+0x1C  collection (CDSCollection, 0x18)
+0x34  chain (CDSChain, 0x14)
+0x48  pCriticalSection[24]
```

**Prototypes:** `CDSStreamStorage_ctor@0x00401790` → `CDSStreamStorage * __thiscall CDSStreamStorage_ctor(CDSStreamStorage *this, int *param_1)`; `CDSStreamStorage_FindKeyIndex@0x00431170` → `CDSCollection *` ECX (plate comment); `CBulanci_OpenPackStream@0x00401d80` → `OperatorNew(0x60)` + `CDSStreamStorage_ctor` → return `&pCVar7->pVftable_IDSStorage`.

**Decompiler:** `CloseStreamByKey@0x00433cb0` uses `(this->collection).pM_items[i]` after `FindKeyIndex((CDSStreamStorage *)&this->collection, …)`. `CreateFilterSafeStream@0x00434760` already named (batch 29).

## IDSStorage interface stubs (`g_pCDSStreamStorage_vftable_IDSStorage` @ `0x0047f7a4`, 9 slots)

| Slot | Vtable addr | Function | `this` field read | Live? |
|------|-------------|----------|-------------------|-------|
| +6 | `0x0047f7bc` | `GetStreamCount@0x004339c0` | `[ECX+0x24]` → `m_items` ptr | Direct xref `IDSAnim_SelectRandomTrack@0x004392b0` (suspect cast) |
| +7 | `0x0047f7c0` | `GetStreamEntry@0x004339d0` | `[ECX+0x20]` → `m_pVtable_IDSChained` | **Vtable-only** — no direct callers |

## Agent todo 46 — IDSStorage stub audit (2026-05-30)

| Check | Result |
|-------|--------|
| `GetStreamCount@0x004339c0` vs `m_items`/`m_count` | **Closed** — disasm `MOV EAX,[ECX+0x24]` returns **`collection.m_items`** (`pStreamEntries`), **not** `m_count` at `+0x28`. Decompile already labels `pStreamEntries`. |
| `GetStreamEntry@0x004339d0` IDSStorage slot +7 | **Vtable-only** — disasm indexes `[ECX+0x20]` (`m_pVtable_IDSChained`), not `m_items`. Xrefs: data only @ `g_pCDSStreamStorage_vftable_IDSStorage+0x1c` (`0x0047f7c0`). Plate + decompiler comments applied. |
| `IDSAnim_SelectRandomTrack@0x004392a0` cast | **Closed (suspect)** — calls `GetStreamCount((CDSStreamStorage *)&this->dwField_1c)` with `ECX = CBulAnim+0x1c`; stub offset `+0x24` → reads **`CBulAnim::dwChainRoot` @ `+0x40`**, not pack storage. Random track bound uses chain-root dword, not `CDSStreamStorage` layout. |

Vtable `g_pCDSStreamStorage_vftable_IDSStorage@0x0047f7a4` (9 slots): slot +6 → `0x004339c0`, slot +7 → `0x004339d0` (verified `read_memory`).

## Agent todo 46 (2026-05-30) — loader chain helper prototypes

| Address | Symbol | Prototype (Ghidra signature) |
|---------|--------|------------------------------|
| `0x00433f00` | `CDSStreamStorage_GetThreadLoaderNode` | `void * __thiscall (CDSStreamStorage *this)` |
| `0x00433f70` | `CDSStreamStorage_AppendOrReuseStream` | `void * __thiscall (CDSStreamStorage *this, CDSStrmResInfo *entry, void **ppEventHandlerVtable)` |
| `0x00434160` | `CDSStreamStorage_InitRootSafeStream` | `void __thiscall (CDSStreamStorage *this, int *pUnderlyingStream)` — **R3 task 46:** `set_function_this_type` → decompile uses `pRootSafeStream` / `dwStreamBaseOffsetLo/Hi` |
| `0x00434760` | `CDSStreamStorage_CreateFilterSafeStream` | `void * __thiscall (CDSStreamStorage *this, int *pRootStream, uint sliceOffsetLo, int sliceOffsetHi, uint sliceExtentLo, int sliceExtentHi, uint pEventHandlerVtable)` |

Decompiler comments @ `0x00433f00`, `0x00433f70`, `0x00434160`, `0x00434760`, `0x0043405b` (`dwFilterSliceAddend`). Plate/PRE comments @ `0x004339c0`, `0x004339d0`, `0x00434160`, `0x004392a0`. `set_function_this_type CDSStreamStorage*` @ `InitRootSafeStream@0x00434160`. `save_program bulanci.exe` (**r3 task 46**, 2026-05-30). Report: [round3_task_46_report.md](./round3_task_46_report.md).

**Call quirk:** `AppendOrReuseStream` passes **`entry` in ECX** to `CreateFilterSafeStream` so `InitializeByClassId` reads `entry->dwClassId` @ `+0x0c` (not `storage->nRefcount`).

## UNK

- ~~Loader helpers prototypes~~ — **Closed** agent todo 46.
- `CBulanci::CDSStreamStorage_ctor` symbol: same body as `CDSStreamStorage_ctor@0x00401790` (standalone `0x60`-byte object).

## Follow-up resolved (round 2 / R3 task 45)

- **`CDSCollection` @ `this+0x1c` / `CDSChain` @ `+0x34`:** Ghidra nested embeds verified; decompiler shows `(this->collection).*` / `(this->chain).*` after `set_function_this_type` (R3 task 45). Xref table in **Round 3 (task 44)** below.

## Round 3 (task 44) — `CDSCollection` embed + `FindKeyIndex` path

| Step | Address | Evidence |
|------|---------|----------|
| Ctor init | `CDSStreamStorage_ctor@0x00401790` | `MOV [ESI+0x1c],0x47f700`; `MOV [ESI+0x20],0x47f6e4`; zeros `+0x24`..`+0x2c`; `MOV [ESI+0x30],0x20` |
| Key lookup call | `CDSStreamStorage_CloseStreamByKey@0x00433cb0` | `LEA ECX,[ESI+0x1c]` @ `0x00433d09` → `CALL FindKeyIndex` with `CDSStrmResInfo_CompareKey@0x004342f0` |
| `FindKeyIndex` body | `CDSStreamStorage_FindKeyIndex@0x00431170` | `MOV EBP,ECX`; `[EBP+0x8]` = `m_items`; `[EBP+0xc]` = `m_count`; linear if `compareFn==NULL`, else binary search |
| Entry resolve | `CloseStreamByKey@0x00433d24` | `MOV ECX,[ESI+0x24]`; `MOV EDI,[ECX+EDI*4]` — index into `m_items` on storage `this` |
| Other callers | `CDSCollection_InsertKeyed@0x004312dc` | `CALL FindKeyIndex` with `ECX` = `CDSCollection*` base (no `+0x1c` skip) |

Ghidra flat names: `collection_pVftable` @ `+0x1c`, `pStreamEntries` @ `+0x24`, `streamEntryCount` @ `+0x28`.

**Semantics:** `FindKeyIndex` is a collection helper (storage-scoped symbol). Operates on `CDSCollection*` (= `CDSStreamStorage+0x1c`). Returns index; `CloseStreamByKey` loads stream-entry pointer from `m_items[i]`.

**`GetStreamEntry` stub:** `MOV EAX,[ECX+0x20]` / `MOV EAX,[EAX+ECX*4]` indexes **`m_pVtable_IDSChained`** (storage `+0x20`), not **`m_items`** (`+0x24`). Vtable-only @ `0x0047f7c0` — dead interface stub, off-by-4 from live path.
