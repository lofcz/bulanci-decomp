# CDSMpxStream

## Status

**PARTIAL** — RTTI name for the MPEG resource wrapper around a full `CDSMpx` decoder object (`0x98c8` bytes). Primary base is `CDSMpx` (`CDSMpxStream_dtor` → `CDSMpx_dtor`). Persistence and factory paths prove resource/I/O pointers at the head of the object; `SaveMpxFile` / `LoadMpxFile` run on the **`IDSChained` subobject at primary `+0x38`** (`vtable@0x00487340`). MI vptr placement at **`+0x04` / `+0x18` / `+0x38` / `+0x3c`** verified via `CDSMpxStream_CreateObject` and adjustor thunks (round 3 task 40).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x98c8` (same as `CDSMpx`) | `0x00446b38` | `CreateFromHandle@0x00446b00` → `OperatorNewWithBadAlloc(0x98c8)` then `AttachBitstream` |
| Primary base destroyed as `CDSMpx` | `0x004330b0` | `CDSMpxStream_dtor` → `CDSMpx_dtor(this)` |
| Tail cleanup at `+0x3c` | `0x004330b0` | `CDSMpxStream_dtor` → `FUN_00434250(this+0x3c)` when non-null |
| Registry factory alloc **`0x48`** | `0x00433112` | `CDSMpxStream_CreateObject@0x00433110` → `OperatorNewWithBadAlloc(0x48)`; registered @ `0x0047cec0` class **`0x30`** |

## Multiple-inheritance map (primary base `P`)

| Offset | Interface | Vtable | Evidence |
|--------|-----------|--------|----------|
| `+0x00` | `IDSReferenced` (primary) | `0x00487394` | `CDSMpxStream_CreateObject@0x00433130`; scalar dtor `0x00433160` |
| `+0x04` | `face_8slots` | `0x00487370` | ctor `MOV [EAX+0x4],0x487370` @ `0x00433136`; adjustor `SUB ECX,0x4` @ `0x00433030` |
| `+0x18` | `IDSEventHandler` | `0x0048735c` | ctor `MOV [EAX+0x18],0x48735c` @ `0x0043313d`; adjustor `SUB ECX,0x18` @ `0x00432ff0` |
| `+0x38` | `IDSChained` (persist) | `0x00487340` | ctor `MOV [EAX+0x38],0x487340` @ `0x00433144`; adjustor `SUB ECX,0x38` @ `0x00433000`; `SaveMpxFile` / `LoadMpxFile` |
| `+0x3c` | `IDSChained` (stash) | `0x00487328` | ctor `MOV [EAX+0x3c],0x487328` @ `0x0043314b`; adjustor `SUB ECX,0x3c` @ `0x00433020`; dtor `FUN_00434250(this+0x3c)` |

`CreateFromHandle@0x00446b00` (`CDSMpx` vtable slots @ `0x00487308` / `0x00487384`) allocates **`0x98c8`** and only rewrites **`+0x00`** inside `AttachBitstream` (decoder face). Stream MI vtables at `+0x04` / `+0x18` / `+0x38` / `+0x3c` match the registry ctor stamps above.

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x0000 | 0x98c8 | `CDSMpx` | `body` | `CDSMpxStream_dtor@0x004330b0`; decoder interior in `CDSMpx.md` only |

### Resource / persistence fields (absolute, primary base `P`)

Offsets below lie inside the `CDSMpx` allocation (same object as `body`); do not duplicate libmad interior in this struct.

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x0008 | 12 | `MpxFormatTail_persist` | `mpxFormatTail` | `SaveMpxFile@0x00432eb0` / `LoadMpxFile@0x00433180`: `LEA [this-0x30]` ⇒ `P+0x08`; on-disk `MpxFileHeader` bytes 4..15 (`wChannels`..`dwAvgBytesPerSec`). **Same bytes** as `mad_stream.bufend`+`skiplen`+`sync` (`stream+0x04..+0x0f` @ `P+0x04`) while decoding — see persist-band table below |
| 0x0020 | 4 | `IDSStream *` | `pPayloadStream` | `CreateFromHandle@0x00446b00` `*(param_1+0x1c)` with `param_1` at `P+4`; `LoadMpxFile` stores stream at `(this-0x18)` |
| 0x0028 | 4 | `uint` | `payloadStartLo` | `CreateFromHandle` `Seek64(*(+0x24),*(+0x28),0)`; `SaveMpxFile` seek arg `[this-0x10]` |
| 0x002c | 4 | `uint` | `payloadStartHi` | same; `SaveMpxFile` seek arg `[this-0xc]`; `LoadMpxFile` writes `Tell64` low/high here |
| 0x0030 | 4 | `uint` | `dwPayloadBytes` | `SaveMpxFile` / `LoadMpxFile`: `(this-0x08)`; `AttachBitstream` limit `*(param_1+0x2c)` on create |
| 0x003c | 4 | `void *` | `pSubObjStash` | `CDSMpxStream_dtor@0x004330b0` → `FUN_00434250(this+0x3c)` |

### `SaveMpxFile` / `LoadMpxFile` adjustor map (`this = P + 0x38`)

| Adjustor expr | Absolute | Field |
|---------------|----------|-------|
| `this - 0x08` | `P+0x30` | `dwPayloadBytes` |
| `this - 0x30` | `P+0x08` | `mpxFormatTail` (12 bytes) |
| `this - 0x18` | `P+0x20` | `pPayloadStream` (queue/stream copied in `FUN_0042fdf0`) |
| `this - 0x10` | `P+0x28` | `payloadStartLo` |
| `this - 0x0c` | `P+0x2c` | `payloadStartHi` |

On-disk order remains `[dwDataSize][12-byte format][bitstream]` per `mpx_audio_format.md`; in-memory `dwPayloadBytes` (`P+0x30`) is split from the 12-byte tail at `P+0x08`.

### Persist band `P+0x08..+0x30` vs live `mad_stream` (round 3 task 38)

`mad_stream` is embedded at **`P+0x04`** on the `0x98c8` decoder (`CDSMpx.md`). Persistence I/O uses the **same physical bytes** with different semantics:

| `P+off` | Persist field | Save/Load (`this=P+0x38`) | Live decode (`mad_stream` @ `P+0x04`) |
|---------|---------------|---------------------------|----------------------------------------|
| `+0x04` | — | not in 16-byte header | `buffer` (+ MI `pVftable_MI_audio` before decode — task 39) |
| `+0x08..+0x13` | `mpxFormatTail` (12 B) | `this-0x30`, R/W 12 B | `bufend`+`skiplen`+`sync` (`+0x04..+0x0f` rel stream) |
| `+0x14..+0x17` | — | not in 12-byte tail | `freerate` |
| `+0x18..+0x1b` | — | — | `this_frame` (+ MI slot — task 39) |
| `+0x20` | `pPayloadStream` | `this-0x18`; `CreateFromHandle` `*(handle+0x1c)` | `main_bit` first dword (not used as `mad_bit` in Save/Load) |
| `+0x28` | `payloadStartLo` | `this-0x10` (Save seek) / Load `Tell64` low | `anc_bit` bytes 0..3 |
| `+0x2c` | `payloadStartHi` | `this-0x0c` / Load `Tell64` high | `anc_bit` bytes 4..7 |
| `+0x30` | `dwPayloadBytes` | `this-0x08`, R/W 4 B (`dwDataSize` on disk) | `aux` |

**Lifecycle:** `LoadMpxFile` fills the band from a file; `CreateFromHandle@0x00446b00` reads `pPayloadStream` / seek / `dwPayloadBytes` from `handle+0x1c..+0x2c` (with `handle` at `P+4`) **before** `AttachBitstream`; `ResetDecoderState` then repurposes `P+0x04..` as libmad state. `SaveMpxFile` writes the band back as `.mpx` header + bitstream — not safe to interpret as `mad_stream` while actively decoding.

**Typing:** Ghidra names the 12-byte slot `MpxFormatTail_persist` (not `mad_stream` nested fields) so persistence and decode layouts stay distinct in the type manager.

## Registry shell (`CDSMpxStreamRegistry`, `0x44` / heap `0x48`)

`CDSMpxStream_CreateObject@0x00433110` (class id **`0x30`** @ `0x0047cec0`) allocates **`OperatorNew(0x48)`** and stamps MI vtables only — **not** the `0x98c8` decoder body. Logical object ends at **`pSubObjStash@+0x40`** (`puVar1[0x10]=0`); **`0x48`** is allocator padding.

| Offset | Field | Evidence |
|--------|-------|----------|
| `+0x00` | `pVftable_IDSReferenced` `0x487394` | ctor `*puVar1` @ `0x00433130` |
| `+0x04` | `pVftable_face8` `0x487370` | `puVar1[1]` @ `0x00433136` |
| `+0x14` | zero | `puVar1[5]` |
| `+0x18` | `pVftable_IDSEventHandler` `0x48735c` | `puVar1[6]` @ `0x0043313d` |
| `+0x1c` | `nRefcount = 1` | `puVar1[7]` |
| `+0x20` | zero | `puVar1[8]` |
| `+0x38` | `pVftable_IDSChainedPersist` `0x487340` | `puVar1[0xe]` @ `0x00433144` |
| `+0x3c` | `pVftable_IDSChainedStash` `0x487328` | `puVar1[0xf]` @ `0x0043314b` |
| `+0x40` | `pSubObjStash` | `puVar1[0x10]`; dtor `FUN_00434250` on facet `+0x3c` |

Full decode instances come from **`CDSMpx::CreateFromHandle@0x00446b00`** (`OperatorNew(0x98c8)` + `AttachBitstream`).

## Function leaf map

| Symbol | Address | Role |
|--------|---------|------|
| `CDSMpxStream_CreateObject` | `0x00433110` | Registry shell `CDSMpxStreamRegistry` |
| `CDSMpxStream_dtor` | `0x004330b0` | `FUN_00434250(+0x3c)`; `CDSMpx_dtor` |
| `CDSMpxStream::SaveMpxFile` | `0x00432eb0` | `IDSChained` adjustor `P+0x38` |
| `CDSMpxStream::LoadMpxFile` | `0x00433180` | `IDSChained` adjustor `P+0x38` |
| `CDSMpx::CreateFromHandle` | `0x00446b00` | `0x98c8` decoder instance |
| MI adjustor thunks | `0x00433000`–`0x00433030` | `SUB ECX,0x38/0x3c/0x18/0x4` |

## Ghidra apply

**Agent todo 38 (2026-05-30, worker r3-38):** `SaveMpxFile` / `LoadMpxFile` prototypes set to `CDSMpxPersistFacet *` (`__thiscall`, ECX @ primary `+0x38`). `mpxFormatTail@+0x08` typed as nested **`MpxFormatTail_persist`** (12 B: `alias_bufend_skiplen_sync` + `alias_freerate_thisframe`) documenting on-disk header tail vs live `mad_stream+0x04..+0x13` overlay. Plate comments @ `0x00432eb0` / `0x00433180` distinguish persistence band `P+0x08..+0x30` from decoder RAM. `save_program bulanci.exe`.

**Note:** Ghidra API cannot retype `__thiscall` ECX — decompiler may still show `CDSMpxStream *this`; use plate Parameters and `this−0x38` adjustor map. Ghidra layout still has `dwPayloadStartLo` @ `+0x1c` (doc `+0x28`) — follow-up if disasm field refs need exact offsets.

## UNK

- Whether registry shell and `CreateFromHandle` instance are ever the same allocation in live game code.
- `pSubObjStash` type at `+0x3c`.
- Exact sub-byte mapping of on-disk `wChannels`/`wBitsPerSample`/rates into `bufend`/`skiplen`/`sync` dwords (type-punned storage; dword alignment proven, WAVEFORMAT field order per `mpx_audio_format.md`).
