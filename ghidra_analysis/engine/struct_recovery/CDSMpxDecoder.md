# CDSMpxDecoder

## Status

**PARTIAL** — not separately allocated; shares the `0x98c8`-byte `CDSMpx` object. One **active-decode** `IDSReferenced` vtable (`0x489524`, 3 slots) at `+0x00` while a bitstream is attached. Two additional **`CDSMpx` multiply-inherited** vtable pointers live at `+0x04` and `+0x18`; they are **restored only in `CDSMpx_dtor`**, not in `AttachBitstream` or `CDSMpxDecoder_dtor`. During decode, libmad `mad_stream` fields overlay those MI slots.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Same object as `CDSMpx`, size `0x98c8` | `0x00446b38` | Only alloc site: `CreateFromHandle@0x00446b00` → `OperatorNewWithBadAlloc(0x98c8)`; no `OperatorNew` xref for `CDSMpxDecoder` alone |
| Teardown reaches `+0x589c` | `0x004468b0` | `CDSMpxDecoder_dtor` → `CDSMemQueue_dtor(this+0x589c)`, release `*(this+0x5898)` |
| Active-decode vtable at `+0x00` | `0x00446a2c` | `AttachBitstream@0x00446a00` → `this->pVftable = CDSMpxDecoder::g_pCDSMpxDecoder_vftable_IDSReferenced` (`0x489524`) |
| MI faces restored on full `CDSMpx` teardown | `0x00432f74`, `0x00432f7a` | `CDSMpx_dtor@0x00432f40` → `param_1[1]=0x4872f4`, `param_1[6]=0x4872e0` |

## Vtable lifecycle (same `0x98c8` object)

| Phase | `+0x00` | `+0x04` | `+0x18` |
|-------|---------|---------|---------|
| After `CDSMpx_dtor` restore / pre-decode idle | `0x487318` (`CDSMpx` / `IDSReferenced`) | `0x4872f4` (8-slot audio/resource face) | `0x4872e0` (`IDSEventHandler`, 4 slots) |
| After `AttachBitstream` / during decode | `0x489524` (`CDSMpxDecoder` / `IDSReferenced`) | `mad_stream.buffer` (`stream+0x00`) | `mad_stream.this_frame` (`stream+0x14`, object `+0x18`) |
| `CDSMpxDecoder_dtor` | Rewrites `0x489524` at `+0x00` only | *(unchanged — still libmad stream bytes)* | *(unchanged)* |
| `CDSMpx_dtor` (full destroy) | → `0x487318`, then `g_pCDSObject_vftable` | → `0x4872f4` | → `0x4872e0` |

`CreateFromHandle` allocates and immediately calls `AttachBitstream`; there is no separate ctor that installs the `0x4872f4` / `0x4872e0` pointers before the first decode.

## Layout (decoder-relevant head)

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x0000 | 4 | `void *` | `pVftable_IDSReferenced` | `AttachBitstream@0x00446a00` → `0x489524`; `CDSMpxDecoder_dtor@0x004468b0`; `CDSMpx_dtor@0x00432f40` → `0x487318` |
| 0x0004 | 4 | `void *` | `pVftable_MI_audio` *(CDSMpx)* | `CDSMpx_dtor@0x00432f40` `param_1[1]=0x4872f4`; **overlaps** `mad_stream.buffer` while decoding |
| 0x0018 | 4 | `void *` | `pVftable_MI_event` *(CDSMpx)* | `CDSMpx_dtor@0x00432f40` `param_1[6]=0x4872e0`; **overlaps** `mad_stream.this_frame` while decoding |
| 0x0004 | 0x98c4 | — | *(body)* | Tail fields identical to `CDSMpx.md` (`+0x5898` stream, `+0x589c` queue, etc.) |

## Vtable: active decode (`0x00489524`, 3 slots)

| Slot | Address | Symbol (Ghidra) |
|------|---------|-----------------|
| 0 | `0x00446930` | `CDSMpxDecoder_GetTypeInfo` |
| 1 | `0x00446ae0` | `CDSMpxDecoder_ScalarDeletingDtor` → `CDSMpxDecoder_dtor` |
| 2 | `0x004245c0` | `CDSObject_ReleaseViaVtable` |

## Vtable: inherited MI @ `+0x04` (`0x004872f4`, 8 slots — `CDSMpx`)

| Slot | Address | Symbol (Ghidra) |
|------|---------|-----------------|
| 0 | `0x0041a510` | `CDSWav::FUN_0041a510` |
| 1 | `0x0042ac90` | `CDSChain::FUN_0042ac90` |
| 2 | `0x004330a0` | `CDSWav::FUN_004330a0` |
| 3 | `0x00432f20` | `CDSMpx::FUN_00432f20` |
| 4 | `0x00446b90` | `CDSMpx::ResolveResource` |
| 5 | `0x00446b00` | `CDSMpx::CreateFromHandle` |
| 6 | `0x004469a0` | `CDSMpx::ForwardReadPcmToInstance` |
| 7 | `0x004466a0` | `CDSMpx::FUN_004466a0` |

## Vtable: inherited MI @ `+0x18` (`0x004872e0`, 4 slots — `CDSMpx` / `IDSEventHandler`)

| Slot | Address | Symbol (Ghidra) |
|------|---------|-----------------|
| 0 | `0x00401600` | `CDSFileStream::FUN_00401600` |
| 1 | `0x004049d0` | `CBulanci::FUN_004049d0` |
| 2 | `0x00433070` | `CDSWav::FUN_00433070` |
| 3 | `0x00432f30` | `CDSMpx::FUN_00432f30` |

## Ghidra apply

**Slice 34 (2026-05-30):** Replaced `CDSMpxDecoder.pBody` blob with full layout mirroring `CDSMpx` (libmad + PCM + I/O tail). `get_struct_layout CDSMpxDecoder` → **39112 (0x98c8)**. `set_function_prototype` `CDSMpxDecoder_dtor(CDSMpxDecoder *)` — decompile uses `inputStream` / `pOutputQueue`.

```
remove_struct_field CDSMpxDecoder.pBody
add_struct_field … stream, frame, synth, channelL/R, inputBuffer, initFlag, …
get_struct_layout CDSMpxDecoder → size 0x98c8 (39112)
set_function_prototype CDSMpxDecoder_dtor@0x004468b0
save_program bulanci.exe
```

## UNK

- Whether MSVC emits `CDSMpxDecoder` as a separate inherited subobject with its own `this` adjustor, or only as a decode-mode vtable label on the `CDSMpx` allocation (RTTI: single `CDSMpxDecoder` table at `0x00489524`).
- `ForwardReadPcmToInstance@0x004469a0` caller envelope vs `this` adjustment for the `+0x04` audio face.

## Follow-up (round 3 task 22)

- **Proven:** `CDSMpx_dtor@0x00432f40` is the sole restorer of MI pointers at `+0x04` / `+0x18`; `AttachBitstream` and `CDSMpxDecoder_dtor` touch only `+0x00`.
- **Overlap:** `+0x04` ≡ `mad_stream.buffer`; `+0x18` ≡ `mad_stream.this_frame` during active decode (`CDSMpx_mad_stream_buffer` `param_1[5]`; `CDSMpx.md`).
