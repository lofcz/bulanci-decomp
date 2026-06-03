# Round 6 — Task 30 report (logic sim slice: FLX DecodeFrame + CDSMpxStream)

## Task

| Field | Value |
|-------|-------|
| **id** | 30 |
| **title** | Logic sim_429_436: 0x00432c60–0x00433180 (22 funcs) |
| **range** | sim_429_436 |
| **range_note** | 0x00429–0x00436: game state, simulation, per-frame |
| **seed_address** | *(none — contiguous slice)* |

## Status

**PARTIAL** — Per-function logic documented and re-verified against saved export `bulanci.ghidra.exe.c` plus struct-recovery docs (`CDSFlxFile.md`, `CDSMpxStream.md`, `CDSMpx.md`, `mpx_audio_format.md`). **Ghidra MCP was not connected** during this worker run (`switch_program` / `decompile_function` → `Not connected`); no live disasm/xref pass and no `save_program bulanci.exe`.

## Slice note (manifest vs. ownership)

The slice **starts** with `CDSFlxFile::DecodeFrame@0x00432c60` (ClassID 52 / BitmapSprite animation) and **continues** into the **CDSMpxStream / CDSMpx** registry shell, MI thunks, persistence I/O, and shared refcount helpers. These are adjacent in `.text` but serve different resource types (FLX sprite frames vs. `.mpx` MPEG audio container).

## Functions

| Address | Ghidra symbol | Role summary | Evidence |
|---------|---------------|--------------|----------|
| `0x00432c60` | `CDSFlxFile::DecodeFrame` | FLX frame parser: reads 5-byte frame header (`u32 size`, `u8 chunkCount`), dispatches inner chunks by opcode `{0,4,8,9,10,11,12,13,14,15}` to RLE/delta/palette/move/region/timing/mask decoders on `CDSImage` consumer; advances circular cursor over bound stream body | Decomp @ export L110439; static xref only `g_pCDSFlxFile_vftable_meta` slot 7 @ `0x004872c0`; live dispatch `TM_AdvanceFrame@0x00439a15` → `seq->vfn[7]` ([CDSFlxFile.md](../struct_recovery/CDSFlxFile.md)) |
| `0x00432eb0` | `CDSMpxPersistFacet::SaveMpxFile` | Persist facet (`ECX = P+0x38`): writes on-disk `[dwPayloadBytes@P+0x30][mpxFormatTail@P+0x08 (12 B)][mpeg payload]` via `Seek64`+copy on `pPayloadStream@P+0x20` | Decomp @ export L110567; disasm plates per [CDSMpxStream.md](../struct_recovery/CDSMpxStream.md); R4/R5 typed `CDSMpxPersistFacet *` |
| `0x00432f10` | `CDSMpx::CDSMpx_GetTypeInfo` | Returns static class descriptor `&DAT_004b8510` for RTTI/factory lookup | Decomp @ export L110591; xref `HandleClassRegister` path in `mpx_audio_format.md` |
| `0x00432f20` | `CDSMpx::CDSMpx_AdjustOffsetDtor` | MI scalar-deleting dtor adjustor: `this -= 0x18` (approx. via `field_0x98c4` indexing) → `CDSMpx_vDtor` on primary base | Decomp @ export L110599 |
| `0x00432f30` | `CDSMpx::CDSMpx_ScalarDeletingDtor_thunk_Sub18` | MI adjustor thunk: subtract `0x18` from facet pointer, forward to `CDSMpx_vDtor` | Decomp @ export L110608 |
| `0x00432f40` | `CDSMpx::CDSMpx_dtor` | Primary teardown: restores MI vtables `@+0x04` (`0x4872f4`) and `@+0x18` (`0x4872e0`) clobbered during libmad decode; releases `pHeldRef@+0x20`; chains `CDSIDSReferencedSub_ReleaseHeldRef` | Decomp @ export L110617; overlay proof [CDSMpx.md](../struct_recovery/CDSMpx.md) |
| `0x00432fc0` | `CDSMpx::CDSMpx_vDtor` | Scalar-deleting wrapper: calls `CDSMpx_dtor`, optional `_free(this)` when `param_1 & 1` | Decomp @ export L110652 |
| `0x00432fe0` | `CDSMpxStream::CDSMpxStream_GetTypeInfo` | Returns registry class table `&DAT_004b7e4c` (class id **0x30**) | Decomp @ export L110664; factory xref @ export L179528 |
| `0x00432ff0` | `CDSMpxStream_ScalarDeletingDtor_thunk_Sub18` | MI adjustor: `ECX -= 0x18` (`IDSEventHandler` @ `P+0x18`) → scalar dtor on primary | Decomp @ export L110674; `SUB ECX,0x18` per [CDSMpxStream.md](../struct_recovery/CDSMpxStream.md) |
| `0x00433000` | `CDSMpxStream_ScalarDeletingDtor_thunk_Sub38` | MI adjustor: `ECX -= 0x38` (`IDSChained` persist @ `P+0x38`) → scalar dtor | Decomp @ export L110686 |
| `0x00433010` | `CDSChain_AdjustThisOffset_ThisMinus38` | MI vtable slot 1 on persist facet: `param_1 - 0x38` → `CDSChain_AdjustThisOffset` (AddRef path) | Decomp @ export L110696; export labels `CDSDsmFile::…` — **naming mismatch** (see UNK) |
| `0x00433020` | `CDSMpxStream_ScalarDeletingDtor_thunk_Sub3c` | MI adjustor: `ECX -= 0x3c` (stash facet @ `P+0x3c`) → scalar dtor | Decomp @ export L110707 |
| `0x00433030` | `CDSMpxStream_ScalarDeletingDtor_thunk_Sub4` | MI adjustor: `ECX -= 4` (`face_8slots` @ `P+0x04`) → scalar dtor | Decomp @ export L110719 |
| `0x00433040` | `CDSWav::CDSWav_ReleaseRefcount` | Shared primary-base refcount: if `dwRefcountOrSlots@+0x1c == 1` dispatch delete via `IDSEventHandler` vtable; else decrement | Decomp @ export L110729; [CDSWav.md](../struct_recovery/CDSWav.md) @ `0x00433040` |
| `0x00433070` | `CDSMpxStream_ReleaseRefcount_thunk_Sub18` | Adjustor `-0x18` → `CDSWav_ReleaseRefcount` | Decomp @ export L110755 |
| `0x00433080` | `CDSMpxStream_ReleaseRefcount_thunk_Sub38` | Adjustor `-0x38` → `CDSWav_ReleaseRefcount` | Decomp @ export L110764 |
| `0x00433090` | `CDSMpxStream_ReleaseRefcount_thunk_Sub3c` | Adjustor `-0x3c` → `CDSWav_ReleaseRefcount` | Decomp @ export L110775 |
| `0x004330a0` | `CDSMpxStream_ReleaseRefcount_thunk_Sub4` | Adjustor `-4` → `CDSWav_ReleaseRefcount` (face_8slots slot 2) | Decomp @ export L110784; decompiler comment marks interface name UNCERTAIN |
| `0x004330b0` | `CDSMpxStream::CDSMpxStream_dtor` | Stream dtor: `IDSChainedTail_ClearSubObjStash(&stashFacet@P+0x3c)` then `CDSMpx_dtor(P)` on full `0x98c8` layout | Decomp @ export L110797; [pass_r4_CDSMpxStream_report.md](../struct_recovery/pass_r4_CDSMpxStream_report.md) |
| `0x00433110` | `CDSMpxStream_CreateObject` | Registry factory: `OperatorNew(0x48)` MI shell only (class **0x30**); stamps five vtables; `nRefcount=1`; `pSubObjStash=0` | Decomp @ export L110828; **not** `CDSQueueStream_CreateObject@0x0043c160` (class **0x4b**) |
| `0x00433160` | `CDSMpxStream_ScalarDeletingDtor` | Scalar-deleting wrapper on primary: `CDSMpxStream_dtor` + conditional `_free` | Decomp @ export L110857 |
| `0x00433180` | `CDSMpxPersistFacet::LoadMpxFile` | Persist facet load: `Read` 4 B size + 12 B `mpxFormatTail`; `Tell64` → `payloadStartLo/Hi`; stores `inStream` @ `pPayloadStream@P+0x20` with AddRef | Decomp @ export L110873; bytes `P+0x14..+0x1f` not read from file (live `mad_stream` only) |

### Control-flow highlights

**FLX path (`DecodeFrame`):** Bound stream → optional seek/read frame blob → per-chunk `switch(tag)` → pixel writers set dirty flag → `NotifyDirtyAll(consumer)` → advance cursor with wrap at `bodyStartCursor`/`bodyEndCursor`.

**MPX persist path:** `LoadMpxFile` fills persist band `P+0x08..+0x30` → downstream `CDSMpx::CreateFromHandle@0x00446b00` (outside slice) allocates `0x98c8` and `AttachBitstream` → decode overlays `mad_stream@P+0x04` → `SaveMpxFile` writes band back (unsafe mid-decode).

**MPX lifecycle / dtors:** Registry `0x48` shell from `CreateObject` is separate from decode `0x98c8` body ([CDSMpxStream.md](../struct_recovery/CDSMpxStream.md) R5 w41 PROVEN). Full-object dtor `@0x004330b0` clears stash then runs `CDSMpx_dtor` which restores MI vptrs overwritten by libmad.

## Ghidra deltas

**none** — Ghidra MCP unavailable; prior rounds already applied:

- R4/R5: `set_function_prototype` + `set_function_this_type` on `SaveMpxFile` / `LoadMpxFile` → `CDSMpxPersistFacet *`
- R5 w29/w41: struct field renames, decompiler/plate comments, `save_program bulanci.exe`
- R4 task 33 / batch 30: `CDSFlxFile_DecodeFrame` prototype with `CDSObject *consumer`

Pending when MCP returns (not applied here):

- Rename `0x00433010` from export label `CDSDsmFile::CDSChain_AdjustThisOffset_ThisMinus38` to `CDSMpxStream`-scoped name if vtable xref confirms MPX persist facet only
- Live `get_xrefs_to` / `force_decompile` pass to confirm export still matches database

## Frida

**none** — All slice behavior is proven statically (export decompilation + prior disasm from R4/R5). Runtime `.mpx` round-trip remains blocked on missing retail sample file ([formats/status.md](../../formats/status.md)), not on understanding these functions.

## Remaining UNK

| Item | Status |
|------|--------|
| `pSubObjStash@+0x40` concrete type on MPx path | **BLOCKED** — sole writer is null in `CreateObject@0x00433110` (R5 w41) |
| WAVEFORMAT sub-byte alias inside `mpxFormatTail` during decode overlay | **BLOCKED** — dual-purpose bytes documented; bit-level alias not isolated |
| Ghidra symbol @ `0x00433010` namespace (`CDSDsmFile` vs `CDSMpxStream`) | **UNK** — needs live vtable xref when MCP up |
| Exact IDS interface name for `ReleaseRefcount_thunk_Sub4@0x004330a0` | **UNCERTAIN** — decompiler comment @ export L110787 |
