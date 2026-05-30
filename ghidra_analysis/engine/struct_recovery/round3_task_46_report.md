# Round 3 — Task 46 report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 46 |
| **title** | Sweep CDSStreamStorage IDSStorage stub comments + loader protos |
| **one_liner** | Document IDSStorage slots +6/+7 (`GetStreamCount` / `GetStreamEntry`); set loader helper prototypes; audit `IDSAnim_SelectRandomTrack` fake `CDSStreamStorage*` cast. |
| **acceptance** | Update CDSStreamStorage.md; Ghidra mutation if evidence supports |
| **types** | CDSStreamStorage, CDSChain, CDSStrmResInfo |
| **addresses** | `0x004339c0`, `0x004339d0`, `0x00433f00`, `0x00433f70`, `0x00434160`, `0x00434760`, `0x004392a0` |

## 2. Status

**DONE** — IDSStorage stubs annotated; loader chain helpers carry `CDSStreamStorage*` / `CDSStrmResInfo*` prototypes; `InitRootSafeStream` ECX retyped from `CBulanci*`; `IDSAnim_SelectRandomTrack` suspect cast documented.

## 3. Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| `GetStreamCount` returns `m_items`, not `m_count` | `CDSStreamStorage_GetStreamCount@0x004339c0` | Disasm `MOV EAX,[ECX+0x24]`; decompile `(this->collection).pM_items` |
| `GetStreamEntry` indexes `m_pVtable_IDSChained` | `CDSStreamStorage_GetStreamEntry@0x004339d0` | Disasm `MOV EAX,[ECX+0x20]`; `MOV EAX,[EAX+ECX*4]` — off-by-4 from live `m_items` @ `+0x24` |
| `GetStreamEntry` vtable-only | `0x004339d0` | `get_function_xrefs` → data @ `g_pCDSStreamStorage_vftable_IDSStorage+0x1c` (`0x0047f7c0`) only |
| `GetStreamCount` live caller (suspect) | `IDSAnim_SelectRandomTrack@0x004392a0` | Call @ `0x004392b0` with `ECX = CBulAnim+0x1c` → effective read **`dwChainRoot` @ `+0x40`**, not pack storage |
| Per-thread loader node | `CDSStreamStorage_GetThreadLoaderNode@0x00433f00` | Decompile walks `this->chain`; `GetCurrentThreadId` vs child `+0x10` |
| Append / reuse + filter addend | `CDSStreamStorage_AppendOrReuseStream@0x00433f70` | `streamExtent==0` append path; `entry->dwFilterSliceAddend` @ `+0x20` → `CreateFilterSafeStream`; `ECX=entry` quirk for class id |
| Root safe-stream + seek base | `CDSStreamStorage_InitRootSafeStream@0x00434160` | `OperatorNew(0x48)` → `pRootSafeStream` @ `+0x18`; `IStream::Seek` → `dwStreamBaseOffsetLo/Hi` @ `+0x10` |
| Filter wrapper | `CDSStreamStorage_CreateFilterSafeStream@0x00434760` | 7-param `__thiscall`; batch 29 naming retained |

### IDSStorage vtable (`g_pCDSStreamStorage_vftable_IDSStorage` @ `0x0047f7a4`)

| Slot | Ptr | Function | Field used |
|------|-----|----------|------------|
| +6 | `0x0047f7bc` | `GetStreamCount@0x004339c0` | `[ECX+0x24]` → `collection.m_items` |
| +7 | `0x0047f7c0` | `GetStreamEntry@0x004339d0` | `[ECX+0x20]` → `m_pVtable_IDSChained` (dead stub) |

## 4. Ghidra deltas

- PRE/decompiler comments @ `0x004339c0`, `0x004339d0`, `0x004392a0` (fake cast / `dwChainRoot` audit).
- PRE comments @ `0x00433f00`, `0x00433f70`, `0x00434160`, `0x0043405b` (`dwFilterSliceAddend`).
- Prototypes (verified `get_function_signature`): `GetThreadLoaderNode`, `AppendOrReuseStream`, `CreateFilterSafeStream` — `CDSStreamStorage*` + typed params; `GetStreamCount` / `GetStreamEntry` — minimal stubs.
- **`set_function_this_type` `CDSStreamStorage*` @ `0x00434160`** — decompile now `CDSStreamStorage::InitRootSafeStream` with `pRootSafeStream` / `dwStreamBaseOffsetLo/Hi` (was `CBulanci*` / `field_0x18`).
- `force_decompile` @ `0x00434160`.
- `save_program bulanci.exe`.

## 5. Struct doc updates

- [CDSStreamStorage.md](./CDSStreamStorage.md) — Agent todo 46 audit table; loader prototype table; `InitRootSafeStream` this-type note.
- [CBulAnim.md](./CBulAnim.md) — cross-ref on `dwField_1c` fake storage cast (pre-existing round-3 note).

## 6. Remaining UNK

- `InitRootSafeStream` still issues one virtual call through `(this->collection).pM_pVtable_IDSChained + 0x10` in decompile (likely Ghidra vfunc resolution artifact on `pRootSafeStream` setup).
- Whether any non-stub caller ever uses real `CDSStreamStorage*` with `GetStreamCount` for a true entry count (only suspect `CBulAnim` xref found).
- `GetStreamEntry` remains dead; no rename of vtable slot symbols.
