# Pass R4 — CDSMpxStream complete map

## Scope

Consolidate **CDSMpxStream** layout, persistence I/O, **`0x98c8`** decoder object, registry **`0x48`** shell, and factory **`0x4b` @ `0x0043c160`** distinction (round 3 tasks **36** / **37** / **38**, round 4 task **38**).

## Status

**DONE** — persist band `P+0x08..+0x30` vs live `mad_stream@P+0x04` documented; Save/Load/CreateFromHandle/dtor disasm-aligned; Ghidra head layout matches; `CDSMpxPersistFacet *` typed on Save/Load; `save_program bulanci.exe`.

## Two allocations, one logical type

| Path | Function | Heap | Class id | Role |
|------|----------|------|----------|------|
| Registry factory | `CDSMpxStream_CreateObject@0x00433110` | **`0x48`** (`CDSMpxStreamRegistry`) | **`0x30`** @ `CDSMpxStream_StaticClassRegister` (`PUSH 0x433110` @ `0x0047cec0`) | Stamps MI vtables `@+0/+4/+0x18/+0x38/+0x3c`; **`pSubObjStash@+0x40`=0**; **no** libmad body |
| Decode instance | `CDSMpx::CreateFromHandle@0x00446b00` | **`0x98c8`** (`CDSMpx` / `CDSMpxStream` body) | (resource open, not this factory) | Reads persist band from `ctx@P+4` (`face_8slots`); `OperatorNew(0x98c8)` + `AttachBitstream` |
| **Not MPx** | `CDSQueueStream_CreateObject@0x0043c160` | **`0x24`** | Registry **`0x4b`** @ `CDSQueueStream_StaticClassRegister@0x0047daa0` | In-memory queue `IDSStream`; **same numeric `0x4b` as JPEG default quality dword** — unrelated |

**Trap (task 36):** Do not confuse **`0x0043c160`** (queue stream, class **`0x4b`**) with **`0x00433110`** (MPx stream registry, class **`0x30`**) or JPEG **`CDSJpegImage_CreateObject@0x00432070`** (class **`0x15`**, `MOV [EAX+0x60],0x4b` quality).

## Primary base `P` — MI vtables

| `P+off` | Interface | Vtable | Adjustor thunk |
|---------|-----------|--------|----------------|
| `+0x00` | `IDSReferenced` | `0x00487394` | — |
| `+0x04` | `face_8slots` | `0x00487370` | `SUB ECX,4` @ `0x00433030` |
| `+0x18` | `IDSEventHandler` | `0x0048735c` | `SUB ECX,0x18` @ `0x00432ff0` |
| `+0x38` | `IDSChained` persist | `0x00487340` | `SUB ECX,0x38` @ `0x00433000` — **SaveMpxFile** / **LoadMpxFile** |
| `+0x3c` | `IDSChained` stash | `0x00487328` | `SUB ECX,0x3c` @ `0x00433020` — dtor clears `pSubObjStash@+0x40` |

Decoder interior (`mad_stream` / `mad_frame` / `mad_synth` / I/O tail): **[CDSMpx.md](./CDSMpx.md)** only.

## Persist band vs `mad_stream` (task 38 + 37)

`mad_stream` embeds at **`P+0x04`** (`CDSMpx.md`). Bytes **`P+0x08..+0x30`** are **shared storage** with different semantics by phase:

| `P+off` | Persist (file / pre-decode) | Live decode (`mad_stream` rel `+0x04`) |
|---------|----------------------------|----------------------------------------|
| `+0x08..+0x13` | `mpxFormatTail` — 12 B WAVEFORMAT tail on disk | `bufend` + `skiplen` + `sync` (`+0x04..+0x0f`) |
| `+0x14..+0x1f` | *(not in 12-byte tail)* | `freerate` + `this_frame` + `next_frame` head |
| `+0x20` | `pPayloadStream` | `main_bit` first dword |
| `+0x28` | `payloadStartLo` | `anc_bit` bytes 0..3 |
| `+0x2c` | `payloadStartHi` | `anc_bit` bytes 4..7 |
| `+0x30` | `dwPayloadBytes` (`dwDataSize` on disk) | `aux` |

**Lifecycle:** `LoadMpxFile` → persist band filled → `CreateFromHandle` **Seek64** + **`AttachBitstream`** → `ResetDecoderState` repurposes `P+0x04..` as libmad → `SaveMpxFile` writes band back (not safe mid-`DecodeFrame`).

## SaveMpxFile / LoadMpxFile (`ECX = P+0x38`)

Disasm @ `0x00432eb0` / `0x00433180` (`ESI`/`EDI` = facet):

| Adjustor | Absolute | Field |
|----------|----------|-------|
| `this - 0x30` | `P+0x08` | `mpxFormatTail` (12 B) |
| `this - 0x18` | `P+0x20` | `pPayloadStream` |
| `this - 0x10` | `P+0x28` | `payloadStartLo` |
| `this - 0x0c` | `P+0x2c` | `payloadStartHi` |
| `this - 0x08` | `P+0x30` | `dwPayloadBytes` (4 B) |

On-disk: `[dwDataSize][12-byte format][mpeg payload]` (`mpx_audio_format.md`).

## `CreateFromHandle` persist read (`ctx = P+4`)

| `ctx+off` | `P+off` | Use |
|-----------|---------|-----|
| `+0x1c` | `+0x20` | `pPayloadStream` → `AttachBitstream` stream arg |
| `+0x24` | `+0x28` | `payloadStartLo` → `Seek64` low |
| `+0x28` | `+0x2c` | `payloadStartHi` → `Seek64` high |
| `+0x2c` | `+0x30` | `dwPayloadBytes` → `AttachBitstream` limit |

## Dtor (`0x004330b0`)

1. `IDSChainedTail_ClearSubObjStash(&stashFacet@P+0x3c)` — reads `pSubObjStash@facet+4` (`P+0x40`).
2. `CDSMpx_dtor(P)` — restores MI vptrs at `+0x04` / `+0x18`; tears down libmad + I/O @ `+0x5898` etc.

## Ghidra deltas (this pass)

- `set_function_prototype` / `set_function_this_type`: `SaveMpxFile` / `LoadMpxFile` → `CDSMpxPersistFacet *` @ `P+0x38`
- Plates @ `CDSMpxStream_CreateObject@0x00433110`, `CDSMpx::CreateFromHandle@0x00446b00`
- `get_struct_layout CDSMpxStream` head: `mpxFormatTail@+0x08`, `pPayloadStream@+0x20`, `payloadStart*@+0x28/+0x2c`, `dwPayloadBytes@+0x30`, `stashFacet@+0x3c`, `frame@+0x44`, **size 39112 (0x98c8)**
- `save_program bulanci.exe`

## Struct doc

- [CDSMpxStream.md](./CDSMpxStream.md) — authoritative layout + function map

## Remaining UNK

- Registry `0x48` shell vs `0x98c8` decode object — same heap block in live game or always separate.
- Non-null writers / concrete type of `stashFacet.pSubObjStash` on MPx path.
- Sub-byte WAVEFORMAT ↔ dword alias inside `mpxFormatTail` during decode overlay.
