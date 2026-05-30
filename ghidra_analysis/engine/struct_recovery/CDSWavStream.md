# CDSWavStream

## Status

**VERIFIED** — class-43 factory `OperatorNew(0x40)`; Ghidra struct **64** bytes (`0x40`, fields through `+0x38` + `pPad_operatorNew0x40@0x3c`); distinct RTTI from `CDSAudioBank` (see [CDSAudioBank.md](./CDSAudioBank.md)).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Heap `sizeof == 0x40` | `0x0043bb00` | `CDSWavStream_Factory`: `OperatorNewWithBadAlloc(0x40)` |
| Registry class **43** (`0x2b`) | `CDSWavStream_StaticClassRegister@0x0047d9e0` | `PUSH 0x2b`; factory `0x0043bb00`; meta `0x004b843c`; `_atexit` → `CDSWavStream_StaticClassRegister_atexit@0x0047ed20` |
| MI typeinfo registers | `0x0047d9b0`..`0x0047da40` | `HandleInterfaceRegister` adjustors `+0x18`/`+0x04`/`+0x34` (`0x004465d0`, `0x00428980`, `0x0043baa0`); R5 worker 9 |
| Factory vtable stores | `0x0043bb00` | `+0x00→0x4823c0`, `+0x04→0x48239c`, `+0x18→0x482388`, `+0x30→0x48236c`, `+0x34→0x482354`; `+0x14`/`+0x20`/`+0x38` zeroed; `+0x1c←1` |
| Dtor through `+0x34` stash | `0x0041bc00` | `CDSWavStream_dtor`: `FUN_00434250(this+0x34)`; `CDSObject__CDSObject_dtor(this)` |
| Ghidra struct size | — | `get_struct_layout CDSWavStream` → **64** bytes (`0x40`; matches `OperatorNew(0x40)`) |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `pointer` | `pVftable_IDSReferenced` | `CDSWavStream_Factory@0x0043bb00` → `0x4823c0` |
| 0x04 | 4 | `pointer` | `pVftable_face8slots` | Factory → `0x48239c` (`face_8slots`; slot4 `CDSWav_BindPcmMemStream`) |
| 0x08 | 4 | `uint` | `dwPcmEndBound` | Wav: end offset for `CDSWav_HandleResourceRead` (`face+4`); bank ctor writes **1** (`CDSAudioBank_Ctor`); class-43 factory leaves unset |
| 0x0C | 4 | `pointer` | `pVftable_sub0c` | Bank `CDSAudioBank_Ctor`; factory does not set |
| 0x10 | 4 | `pointer` | `pParentOrBackref` | Factory clears `+0x14` region; bank deserialize uses `+0x10` |
| 0x14 | 4 | `pointer` | `pVftable_sub14` | Factory → **0** (`puVar1[5]`) |
| 0x18 | 4 | `pointer` | `pVftable_IDSEventHandler` | Factory → `0x482388` |
| 0x1C | 4 | `uint` | `dwRefcountOrSlots` | Factory `= 1`; `CDSWav_ReleaseRefcount@0x00433040` dec/inc on primary |
| 0x20 | 4 | `pointer` | `pStreamStorage` | Factory **NULL**; `CDSWavStream_AttachStreamStorage@0x0043bb50` |
| 0x24 | 4 | `uint` | `dwPcmBindLo` | `CDSWav_BindPcmMemStream@0x0043ba30` (face `this+0x20`); cleared after bind |
| 0x28 | 4 | `uint` | `dwPcmBindHi` | Bind + attach tell QWORD low; `CDSWavStream_AttachStreamStorage` writes QWORD @ `+0x28` |
| 0x2C | 4 | `uint` | `dwStreamTellHi` | Upper half of attach tell QWORD |
| 0x30 | 4 | `pointer` | `pVftable_IDSChained6` | Factory → `0x48236c`; slot4 attach / slot5 `CDSWavStream_SaveToStream@0x0043bab0` |
| 0x34 | 4 | `pointer` | `pVftable_IDSChained5` | Factory → `0x482354`; dtor stash `FUN_00434250(this+0x34)` |
| 0x38 | 4 | `uint` | `dwReservedTail` | Factory zero; no consumer in wav/bank methods |

## Vtables (catalog)

| Address | Interface | Slots | Notes |
|---------|-----------|-------|-------|
| `0x00482354` | `IDSChained` (5) | 5 | `CDSWavStream_AttachStreamStorage` slot 4 |
| `0x0048236c` | `IDSChained` (6) | 6 | `CDSWavStream_SaveToStream` slot 5 |
| `0x00482388` | `IDSEventHandler` | 4 | Shared with `CDSWav` |
| `0x0048239c` | `face_8slots` | 8 | PCM bind slot 4 |
| `0x004823c0` | `IDSReferenced` | 3 | Primary base |

### MI adjustors / meta stubs (R5 worker 19)

| Thunk / stub | Vtable @ slot | Adjust | Target |
|--------------|---------------|--------|--------|
| `CDSWavStream_GetTypeInfo@0x0041a5c0` | `IDSReferenced` `0x4823c0` slot **0** | — | `MOV EAX,0x4b843c; RET` (class-43 RTTI) |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub4@0x0041a5d0` | `face_8slots` `0x48239c` slot **3** | `ECX - 4` | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` |
| `CDSWav_ReleaseChild_thunk_Sub30@0x0041a5e0` | `IDSChained6` `0x48236c` slot **2** | `ECX - 0x30` | `CDSWav_ReleaseRefcount@0x00433040` |
| `CDSWav_ReleaseChild_thunk_Sub34@0x0041a5f0` | `IDSChained5` `0x482354` slot **2** | `ECX - 0x34` | `CDSWav_ReleaseRefcount@0x00433040` |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub30@0x0041a600` | `IDSChained6` `0x48236c` slot **3** | `ECX - 0x30` | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` |
| `CDSChain_AdjustThisOffset_ThisMinus30@0x0041a610` | `IDSChained5` `0x482354` slot **1** | `ECX - 0x30` | `CDSChain_AdjustThisOffset@0x0042ac90` (`LEA EAX,[ECX-4]; RET`) |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub34@0x0041a620` | `IDSChained5` `0x482354` slot **3** | `ECX - 0x34` | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` |

Shared with `CDSWav` face: `CDSWav_GetClassMeta@0x0041a510` at `face_8slots` slot **0** on `0x48239c`.

## Ghidra apply

```
delete_data_type CDSWavStream
create_struct CDSWavStream (15 fields, 60 B) — slice 42
get_struct_layout CDSWavStream → size 60
rename_function_by_address 0x0043bb50 → CDSWavStream_AttachStreamStorage
save_program bulanci.exe
```

## Delete path (todo 50)

- **`CDSWavStream_ScalarDeletingDtor@0x0041bc00`** — `IDSReferenced` vtable `0x4823c0` slot1; only path that calls **`CDSWavStream_dtor@0x0041a640`** (`IDSChainedTail_ClearSubObjStash` at `this+0x34`).
- **`CDSWav_ScalarDeletingDtor@0x0041bbe0`** — lighter MI dtor on `0x482348`; does **not** invoke `CDSWavStream_dtor` (see [CDSWav.md](./CDSWav.md)).

## UNK

- Exact C++ MI declaration order vs `CDSAudioBank` (same footprint, different vtable sets).
- `pVftable_sub0c` on class-43 instances (never written by factory).

## Cross-ref

- [CDSAudioBank.md](./CDSAudioBank.md) — bank facet on same `0x40` layout; class-43 runtime type is **`CDSWavStream`**, not `CDSAudioBank_Ctor`.
- [CDSWav.md](./CDSWav.md) — `CDSWav` interface methods on shared object / DSM embed.
