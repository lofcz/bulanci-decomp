# CDSStrmResInfo

## Status

**VERIFIED** — `OperatorNew(0x28)` in factory; serialize/compare paths; Ghidra struct size 40 bytes.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `OperatorNew(0x28)` | `CDSStrmResInfo_factory@0x00433d80` | `PUSH 0x28`; `OperatorNewWithBadAlloc`; vtables `0x4873f8` / `0x4873dc`; zero `+0x10`/`+0x14` |
| Serialized tail through `+0x1f` | `CDSStrmResInfo_Serialize@0x00433940` | `CDSResInfo_Load` then 8-byte + 4-byte `IDSStream::Read` |
| Same on write | `CDSStrmResInfo_Deserialize@0x00433980` | `CDSResInfo_Save` then 8-byte + 4-byte `Write` |
| Ghidra size `0x28` | `get_struct_layout` | Matches factory allocation |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CDSStrmResInfo_factory@0x00433d80`; stack key in `CloseStreamByKey@0x00433cb0` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | factory / `CloseStreamByKey` (`0x4873dc`) |
| `0x08` | 4 | `uint32` | `resourceId` (Ghidra: `dwResourceId`) | `CDSResInfo_Load@0x00434290` via `CDSResInfo*` at IDSChained subobject (`this+4`/`+8` on base); `CDSStrmResInfo_CompareKey@0x004342f0` sorts on `entry+8`; `CloseStreamByKey` stores search key at stack `+8` |
| `0x0C` | 4 | `uint32` | `classId` (Ghidra: `dwClassId`) | `CDSResInfo_Load` / `Save` second dword (`this+8` on `CDSResInfo*` → `+0xc` on object); pairs with `.eap` header `class_id` (`tools/bulanci_unpack.py` `ResourceHeader`) |
| `0x10` | 4 | `uint32` | `loaderAux` | **Init only:** `CDSStrmResInfo_factory@0x00433d80` (`MOV [obj+0x10],0`); stack key in `CloseStreamByKey@0x00433cb0` leaves `+0x10` zero. **Not** in `Serialize`/`Deserialize`. **No read** in `CompareKey`, `FUN_00433f70`, or other StrmResInfo xrefs (round 3 task 15). |
| `0x14` | 8 | `uint64` | `streamExtent` | `Serialize@0x00433940` / `Deserialize@0x00433980` 8-byte I/O; `CDSStreamStorage_AppendOrReuseStream@0x00433f70` branches on `(int)entry->streamExtent` |
| `0x1C` | 4 | `uint32` | `streamFlags` (Ghidra: `dwStreamFlags`) | `CDSStrmResInfo_Serialize` / `CDSStrmResInfo_Deserialize` 4-byte I/O; `CDSStreamStorage_AppendOrReuseStream@0x00433f70` reads `entry->dwStreamFlags` |
| `0x20` | 4 | `uint32` | `filterSliceAddend` (Ghidra: `dwFilterSliceAddend`) | `CDSStreamStorage_AppendOrReuseStream@0x00433f70` asm `MOV EDX,[EDI+0x20]` @ `0x0043405b` → `CreateFilterSafeStream@0x00434760` **param_4** (slice extent addend; `CDSFilterStream_Ctor` uses `param_2+param_4`); not in factory/serialize |
| `0x24` | 4 | `uint32` | `pad_reserved_hi` (Ghidra: `dwPad_reserved_hi`) | Alloc tail to `0x28`; no xref consumer |

**Note:** `CDSResInfo_ReleaseEmbeddedResource` treats `+0x14` as a pointer on plain `CDSResInfo`; on `CDSStrmResInfo` that offset is the persistence `streamExtent` — do not apply the release path to this derived layout.

## Ghidra apply

```
get_struct_layout CDSStrmResInfo → Size: 40 bytes
  +0x08 dwResourceId, +0x0c dwClassId, +0x10 loaderAux, +0x14 streamExtent, +0x1c dwStreamFlags, +0x20 filterSliceAddend, +0x24 pad_reserved_hi
```

Functions: `CDSStrmResInfo_factory@0x00433d80`, `CDSStrmResInfo_Serialize@0x00433940`, `CDSStrmResInfo_Deserialize@0x00433980`, `CDSStrmResInfo_CompareKey@0x004342f0`. Slice 40: prototypes unchanged; Ghidra field `dwStreamFlags` @ `+0x1c` (doc name `streamFlags`).

**Agent todo 47 (round 2, 2026-05-30):** Split tail `byte[8] pPad_tail` → `filterSliceAddend` @ `+0x20` + `pad_reserved_hi` @ `+0x24`. Only consumer: `AppendOrReuseStream@0x00433f70` `MOV EDX,[EDI+0x20]` @ `0x0043405b` → `CreateFilterSafeStream` param_4 (filter slice addend for `CDSFilterStream_Ctor` extent math). Not factory/serialize. Decompiler PRE @ `0x0043405b`. `save_program bulanci.exe`.

**Agent todo 47 (round 3, 2026-05-30):** Re-verified Ghidra layout: `streamExtent` `ulonglong` @ `+0x14`; `dwFilterSliceAddend` @ `+0x20` (sole xref `MOV EDX,[EDI+0x20]` @ `0x0043405b`). PRE comments @ `0x00433940`, `0x00433980`, `0x0043405b`, `0x00434270`, `0x00434290` distinguish `CDSResInfo::pEmbeddedResource` (base `+0x14`) from stream tail. `save_program bulanci.exe`.

**Agent todo 47 (round 4, 2026-05-30):** R3 closure verify — `get_struct_layout` + `force_decompile` on factory path functions; disasm confirms `8b5720` @ `0x0043405b`; decompiler uses `entry->dwFilterSliceAddend`. No struct mutations. Report: [round4_task_47_report.md](./round4_task_47_report.md). `save_program bulanci.exe`.

## Follow-up resolved (round 3, task 15)

- **`loaderAux` @ `+0x10`:** No dedicated consumer. Factory and `CloseStreamByKey` stack keys zero the slot; persistence skips it (keys at `+0x8`/`+0xc`, tail at `+0x14`..`+0x1f`). Treat as runtime-only dword between the two-vtable header and the serialized stream tail (cf. fourth vtable at `+0x10` on `CDSResourceSign`).

## UNK

- `pad_reserved_hi` @ `+0x24`: no consumer (allocator alignment / reserved).
