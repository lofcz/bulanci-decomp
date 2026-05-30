# CDSWav

## Status

**VERIFIED** (class-43 / face MI through `+0x38`) — **60**-byte Ghidra struct matches class-43 **`CDSWavStream`** heap layout; `CDSWav` is also a **DSM sub-object facet** at `CDSDsmFile+0x1c` (see `dsm_file_format.md`, object `0x68`, **not** a standalone `OperatorNew(0x40)` instance). Destruction on standalone path: `CDSWav_ScalarDeletingDtor@0x0041bbe0` → `CDSObject__CDSObject_dtor` only (not `CDSWavStream_dtor`). Agent todo **50** (2026-05-30): `CDSWav_HandleResourceRead@0x0043b960` uses **`face_8slots` `this` (`primary+4`)**; PCM end bound at **`primary+8`** (`dwPcmEndBound`); `CDSDsmFile::HandleResourceRead@0x00428ad0` is a separate DSM reader on `obj+0x1c`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Shared `0x40` alloc with `CDSWavStream` | `0x0043bb00` | Class-43 factory builds stream object; wav methods use same tail offsets |
| Refcount / slots @ `+0x18`/`+0x1c` | `0x00433040` | `CDSWav_ReleaseRefcount`: `*(this+0x18)` vtable dispatch; `*(this+0x1c)` counter |
| PCM bind @ `+0x20..+0x2c` (face `this=@+4`) | `0x0043ba30` | `CDSWav_BindPcmMemStream`: `this+0x1c` → primary `pStreamStorage`; `+0x24`/`+0x28` aux |
| Resource read (class-43) | `0x0043b960` | `CDSWav_HandleResourceRead`: **`this` = face (`primary+4`)**; stream `[this+0x1c]` → `pStreamStorage@+0x20`; seek QWORD `@+0x24`; bound `[this+4]` → `dwPcmEndBound@+0x08` |
| Resource read (DSM) | `0x00428ad0` | `CDSDsmFile::HandleResourceRead` on **`CDSDsmFile+0x1c`** vtable3 — looping chunk reader, not this struct |
| Ghidra struct | — | `get_struct_layout CDSWav` → **60** bytes (slice 42) |

## Layout (class-43 primary / shared with `CDSWavStream`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `pointer` | `pVftable_IDSReferenced` | `CDSWavStream_Factory`; catalog `0x00482344` (`IDSReferenced`) |
| 0x04 | 4 | `pointer` | `pVftable_face8slots` | Catalog `0x00482320`; slot4 `CDSWav_BindPcmMemStream@0x0043ba30` |
| 0x08 | 4 | `uint` | `dwPcmEndBound` | `CDSWav_HandleResourceRead@0x0043b960` / `CDSWav_BindPcmMemStream` limit via `face+4`; `CDSAudioBank_Ctor` writes **1** here (bank init flag, same offset) |
| 0x0C | 4 | `pointer` | `pVftable_sub0c` | Bank-only in `CDSAudioBank_Ctor` |
| 0x10 | 4 | `pointer` | `pParentOrBackref` | Bank deserialize |
| 0x14 | 4 | `pointer` | `pVftable_sub14` | Factory zero on class-43 |
| 0x18 | 4 | `pointer` | `pVftable_IDSEventHandler` | `CDSWav_ReleaseRefcount` slot dispatch; catalog `0x0048230c` |
| 0x1C | 4 | `uint` | `dwRefcountOrSlots` | `CDSWav_ReleaseRefcount@0x00433040` |
| 0x20 | 4 | `pointer` | `pStreamStorage` | `CDSWav_BindPcmMemStream` when `this` is face at `+4` |
| 0x24 | 4 | `uint` | `dwPcmBindLo` | `CDSWav_BindPcmMemStream` / `CDSWav_HandleResourceRead` |
| 0x28 | 4 | `uint` | `dwPcmBindHi` | Same |
| 0x2C | 4 | `uint` | `dwStreamTellHi` | QWORD high half from stream attach |
| 0x30 | 4 | `pointer` | `pVftable_IDSChained6` | Shared with `CDSWavStream` |
| 0x34 | 4 | `pointer` | `pVftable_IDSChained5` | Shared with `CDSWavStream` |
| 0x38 | 4 | `uint` | `dwReservedTail` | Factory zero |

### MI adjustors (thunks → primary)

| Thunk | Adjust | Target |
|-------|--------|--------|
| `CDSWav_ScalarDeletingDtor_thunk@0x0041a530` | `this - 0x18` | `CDSWav_ScalarDeletingDtor` |
| `CDSWav_ScalarDeletingDtor_thunk_Sub4@0x0041a540` | `this - 4` | same |
| `CDSWav_ReleaseChild_thunk_Sub30@0x0041a5e0` | `this - 0x30` | `CDSWav_ReleaseRefcount` |
| `CDSWav_ReleaseChild_thunk_Sub34@0x0041a5f0` | `this - 0x34` | same |

## Ghidra apply

```
create_struct CDSWav_face8slots (40 B face view @ primary+4)
set_function_prototype @ 0x0043b960 CDSWav_HandleResourceRead(CDSWav_face8slots *this, …)
set_function_prototype @ 0x0043ba30 CDSWav_BindPcmMemStream(CDSWav_face8slots *face_this)
set_function_prototype @ 0x00433040 CDSWav_ReleaseRefcount(CDSWav *primary_this)
rename_function_by_address 0x00428ad0 → CDSDsmFile_HandleResourceRead
set_decompiler_comment @ 0x0043b960, 0x0043ba30, 0x00433040, 0x00428ad0
save_program bulanci.exe
```

## DSM embed vs class-43 heap (`CDSDsmFile+0x1c`)

| | **Class-43 `CDSWavStream`** | **`CDSDsmFile` vtable3 @ `+0x1c`** |
|---|---------------------------|-------------------------------------|
| Allocation | `OperatorNew(0x40)` → 60 B Ghidra struct | `OperatorNew(0x68)` movie container |
| `HandleResourceRead` | `CDSWav_HandleResourceRead@0x0043b960` | `CDSDsmFile::HandleResourceRead@0x00428ad0` |
| `this` register | **`primary+4`** (face_8slots) | **`obj+0x1c`** (vtable3 base) |
| Tail fields | `pStreamStorage`, PCM bind QWORD, IDSChained @ `+0x20..+0x38` | Header dwords straddle `+0x1c`; `src` @ `+0x44`, payload loop @ `dwPayloadEndOffset` |
| Vtables | `0x4823xx` (factory) | DSM-specific; slot 6 still named `HandleResourceRead` |

Same **interface shape** (IDSReadStream / CDSWav catalog), different **object sizes and field overlay** — do not apply `CDSWav` struct at `CDSDsmFile+0x1c`.

## Delete paths (todo 50, VERIFIED)

| Vtable / path | Deleting dtor | Body | `free` when |
|---------------|---------------|------|-------------|
| `IDSReferenced` `0x4823c0` slot1 | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | `CDSWavStream_dtor` → `IDSChainedTail_ClearSubObjStash(+0x34)` → `CDSObject__CDSObject_dtor` | Class-43 factory `OperatorNew(0x40)` |
| `0x482348` slot0 (+ IDSEventHandler thunks) | `CDSWav_ScalarDeletingDtor@0x0041bbe0` | `CDSObject__CDSObject_dtor` only — **no** stash clear | Secondary MI; not the standalone heap owner |
| `face_8slots` `0x48239c` slot3 | `CDSWavStream_ScalarDeletingDtor_thunk_Sub4` | Adjusts to primary → `CDSWavStream_ScalarDeletingDtor` | Same object, chained-5 facet |

**Ownership:** class-43 instances are **`CDSWavStream`**; authoritative teardown is **`CDSWavStream_ScalarDeletingDtor`**, not `CDSWav_ScalarDeletingDtor`. `CDSWav` names the shared layout / DSM facet docs, not a separate heap type.

## Cross-ref

- [CDSWavStream.md](./CDSWavStream.md) — factory, stream attach, class-43 registry.
- [CDSAudioBank.md](./CDSAudioBank.md) — alternate vtable set on same footprint.
