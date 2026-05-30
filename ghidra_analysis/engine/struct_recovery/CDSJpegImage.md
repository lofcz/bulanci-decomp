# CDSJpegImage

## Status

**PARTIAL** — wrapper **`0x64`** (100 bytes) via `CDSJpegImage_CreateObject`; embedded bitmap objects remain **`0x60`** (`CDSObject_CtorWithImage` / DSM-FLX paths). JPEG-specific vtable patch via `CDSJpegImage_InitVtables`. Libjpeg API bodies are separate (not part of this struct).

## Heap factories vs engine registration

Two unrelated factories were conflated under “JPEG registry @ `0x0043c160`”:

| Role | func@addr | `OperatorNew` size | Type constructed | `HandleClassRegister` site | Class id |
|------|-----------|-------------------|------------------|---------------------------|----------|
| **JPEG wrapper factory** | `CDSJpegImage_CreateObject` @ `0x00432070` | `0x64` (100) | `CDSJpegImage` via `CDSJpegImage_InitVtables` | `CDSJpegImage_StaticClassRegister` @ `0x0047cbf0` | **`0x15`** (21), meta `0x004b7d80` |
| **Queue-stream factory** (not this struct) | `CDSQueueStream_CreateObject` @ `0x0043c160` | `0x24` (36) | `CDSQueueStream` vtables `0x486dec` / `0x486dac` / `0x486d94` | `CDSQueueStream_StaticClassRegister` @ `0x0047daa0` | **`0x4b`** (75), meta `0x004b847c` |

The numeric **`0x4b`** (75) appears in three distinct places: engine class id for **`CDSQueueStream`** registration, a dword written at **`CDSJpegImage+0x60`** by `InitVtables`, and (by value) default IJG JPEG quality — do not treat these as one field without per-consumer xrefs.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Wrapper `sizeof == 0x64` (100) | `CDSJpegImage_CreateObject` @ `0x00432070` | `OperatorNewWithBadAlloc(100)` → `CDSJpegImage_InitVtables`; store `0x4b` @ `+0x60` |
| Embedded image `sizeof == 0x60` | `CDSDsmFile::InitializeChildObject` @ `0x00439af3` | `OperatorNewWithBadAlloc(0x60)` → `CDSObject_CtorWithImage` (decode target `IDSImage*`, not full wrapper) |
| Same `0x60` image alloc | `CDSFlxFile::CreateBoundClone` @ `0x00432c0e` | `OperatorNewWithBadAlloc(0x60)` |
| Dtor through `+0x58` | `CDSJpegImage_dtor` @ `0x00431de0` | `IDSChainedTail_ClearSubObjStash(this+0x58)` then `CDSImage_dtor(this+4)` |
| Engine registers JPEG wrapper | `CDSJpegImage_StaticClassRegister` @ `0x0047cbf0` | `PUSH 0x432070`, `PUSH 0x4b7bfc`, `PUSH 0x15`, `MOV ECX,0x4b7d80`, `CALL HandleClassRegister` |
| `0x4b` on wrapper tail only | `CDSJpegImage_InitVtables` @ `0x00431d48` | `MOV [EAX+0x60], 0x4b` after 100-byte alloc |
| Init writes through `+0x5c` | `CDSJpegImage_InitVtables` @ `0x00431cf0` | `param_1[0x17]=0` @ `+0x5c`; vtables at `+0/+4/+4c/+54/+58` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_IDSReferenced` | `CDSJpegImage_InitVtables@0x00431cf0` `= 0x004871e0` |
| `0x04` | 4 | `void *` | `vf_IDSImage` | `InitVtables` `= 0x004871cc` |
| `0x08` | 4 | `uint` | `field_08` | `InitVtables` `= 0` |
| `0x0c` | 4 | `uint` | `field_0c` | `InitVtables` `= 0` |
| `0x28` | 4 | `uint` | `field_28` | `InitVtables` `= 8` |
| `0x4c` | 4 | `void *` | `vf_loadSave` | `InitVtables` `= 0x004871b8` |
| `0x50` | 4 | `uint` | `refCount` | `InitVtables` `= 1` |
| `0x54` | 4 | `void *` | `vf_IDSChained` | `InitVtables` `= 0x0048719c` |
| `0x58` | 4 | `void *` | `vf_chainTail` | `InitVtables` `= 0x00487184`; `CDSJpegImage_dtor` → `IDSChainedTail_ClearSubObjStash(this+0x58)` |
| `0x60` | 4 | `uint` | `classId_or_quality` | **Write:** `InitVtables@0x00431d48` `MOV [EAX+0x60],0x4b`. **Read:** `CDSJpegImage_Save@0x00432030` with `ECX` on IDSChained face (`+0x54`): `MOV ECX,[ECX+0xc]` → object `+0x60` → `CompressFromImage` → `jpeg_set_quality(..., param_3, 1)`. **Not** registry class id (`0x15` @ `CDSJpegImage_StaticClassRegister`). Only on **100-byte** wrapper |

## MI / interface offsets (not duplicate fields)

| Adjustment | Evidence |
|------------|----------|
| `IDSImage::Load` uses `this - 0x50` when `this != 0x54` | `CDSJpegImage_Load@0x00431cc0` → `CDSJpegImage__DecompressToImage` into pixel `CDSImage` at object `+0x04` |
| `IDSImage::Save` passes `[this+0xc]` as quality when `this` on IDSChained (`+0x54`) | `CDSJpegImage_Save@0x00432030` — see layout `+0x60` row |
| Scalar deleting dtors: `this-0x4`, `-0x4c`, `-0x54`, `-0x58` | thunks @ `0x00431d70`..`0x00431db0` |

## Ghidra apply

**Slice 32 (2026-05-30):** `get_struct_layout` → **Size: 100** — `pVf_IDSReferenced` @ `0`, nested **`CDSImage m_image`** @ `+4` (96 B). Object `+0x60` is the last dword of the wrapper: **`InitVtables`** writes `param_1[0x18]` → aliases **`m_image.pM_chain`** (`CDSImage+0x5c`); do not add a separate struct field (would double-count). Prototypes: `CDSJpegImage_CreateObject` → `CDSJpegImage *`; `CDSJpegImage_InitVtables` → `CDSJpegImage *`. Dtor uses **`IDSChainedTail_ClearSubObjStash`** on `this+0x58` (was `FUN_00434250`).

**Agent todo 35 r2 (2026-05-30):** Ghidra **`CDSJpegImage` = 100 B** (`OperatorNew(0x64)` @ `CreateObject`): `pVf_IDSReferenced` @ `0` + nested **`CDSImage m_image` @ `+4` (96 B)**. MI vtable dwords alias the leading `m_image` bytes; tail dword @ object `+0x60` is **`m_image.pM_chain`** in the type system (quality `0x4b` write) — not a separate field. **Distinct** from **`OperatorNew(0x60)`** decode targets (`CDSObject_CtorWithImage` / FLX/DSM) which are standalone `CDSImage` objects without the 4-byte wrapper header. Plate comment on `CreateObject` documents 100 vs 60 split.

**Agent todo 36 (2026-05-30):** `rename_function` `CDSJpegImage__DecompressToImage` @ `0x00431b70`; disasm/decompiler comments @ `0x00431d48` (`MOV [EAX+0x60],0x4b`); plate on `CDSJpegImage_StaticClassRegister` @ `0x0047cbf0` (`void __stdcall`); pre-comments on `CreateObject` / `InitVtables` / `Save`. **`CDSImage.pM_chain` left unchanged** (shared embed in `CDSImageMouse.savedBackground`); doc name `classId_or_quality` = object `+0x60` only.

**Agent todo 36 r3 (2026-05-30):** Disasm verified `0x0043c160` = `CDSQueueStream_CreateObject` (`PUSH 0x24`); JPEG factory `CDSJpegImage_CreateObject@0x00432070` (`PUSH 0x64`). Ghidra EOL/plate comments @ `0x0043c160`, `0x00432070`, `0x00431d48`, `CDSJpegImage_Save`, static registers `0x0047cbf0`/`0x0047daa0`. `save_program`. Report: [round3_task_36_report.md](./round3_task_36_report.md).

**Agent todo 36 r4 (2026-05-30):** R4 plates on `CDSQueueStream_StaticClassRegister@0x0047daa0` (registry class id **0x4b**), `CDSJpegImage_StaticClassRegister@0x0047cbf0` (**0x15**), both factories, and `CDSJpegImage_Save` decompiler comment (MI quality dword @ `+0x60` vs registry ids). `get_xrefs_to` confirms factories only referenced from their static registers (+ init table `0x0047f488`). `save_program`. Report: [round4_task_36_report.md](./round4_task_36_report.md).

**R5 worker 07 (2026-05-30):** `FUN_00431d60` → **`CDSJpegImage_AlwaysReturnsOne`** — IDSChained vftable `0x00487184` slot **[4]**; decompile `return 1`; peer resources use `AlwaysReturnsZero@0x00409480` at the same slot. Report: [round5_worker_07_report.md](./round5_worker_07_report.md).

**Agent todo 36 r2 (2026-05-30):** Program-wide audit of object-offset `+0x60` on **100-byte** `CDSJpegImage` wrappers:

| Kind | Site | Notes |
|------|------|-------|
| **Store** | `CDSJpegImage_InitVtables@0x00431d48` | Sole `MOV [EAX+0x60],0x4b` in binary (`search_byte_patterns` `c7 40 60 4b 00 00 00` → one hit) |
| **Read** | `CDSJpegImage_Save@0x00432030` | `MOV ECX,[ECX+0xc]` with `ECX` on IDSChained (`+0x54`) → object `+0x60` → `CompressFromImage` / `jpeg_set_quality` |
| **Not object +0x60** | `CDSJpegImage::DecompressToImage@0x00431b70` | `[EBP+0x60]` is stack frame only |
| **Not wrapper tail** | `CDSDsmFile::InitializeChildObject@0x00439af3` | `OperatorNew(0x60)` → `CDSObject_CtorWithImage` (embedded **bitmap** object; no byte at offset `+0x60` on a 96 B `CDSImage`) |

`CDSJpegImage_Load@0x00431cc0` does not read or write wrapper `+0x60` (only decompresses into embedded `m_image` at `+0x04`). **No pack-deserialize path** overwrites the quality dword after factory init.

Class factory **`CDSJpegImage_CreateObject`** @ `0x00432070` (`PUSH 100`). Shared **`0x60`**-byte image allocs (`CDSObject_CtorWithImage`) are standalone bitmap objects, not this wrapper.

**`CDSQueueStream_CreateObject`** @ `0x0043c160` is **not** JPEG (see factory table). Static: **`CDSJpegImage_StaticClassRegister`** @ `0x0047cbf0` (class id **`0x15`**), **`CDSQueueStream_StaticClassRegister`** @ `0x0047daa0`.

## Follow-up (round 3 task 3)

- **Done:** `0x0043c160` = `CDSQueueStream_CreateObject` (`0x24`); `CDSJpegImage_CreateObject` @ `0x00432070` (`0x64`) registered @ `0x0047cbf0` with class id **`0x15`**.
- **Done:** `+0x60` stores **`0x4b`** once; `Save` reads it via MI as **jpeg quality** — same numeric value as `CDSQueueStream` registry id **not** evidence of a shared semantic field.
- **Done (r2):** post-factory store audit — write-once `0x4b` only; no pack overwrite.
- **Naming:** keep `classId_or_quality`; do **not** rename to `m_jpegQuality` (write-once constant, no separate serialize path).

## UNK

- ~~Whether pack deserialize overwrites dword @ `+0x60` after factory init~~ **closed (r2):** no writer besides `InitVtables`; `Load`/`DecompressToImage` do not touch wrapper tail.
- Pixel buffer / stride / palette live in embedded `CDSImage` at `+0x04` (see `CDSJpegImage__DecompressToImage@0x00431b70` consumer `param_3+0x10` stride).
