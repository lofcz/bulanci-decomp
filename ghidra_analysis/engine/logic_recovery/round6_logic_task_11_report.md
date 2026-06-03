# Round 6 — Task 11 report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **title** | Logic sim_429_436: 0x00429040–0x00429470 (22 funcs) |
| **range** | sim_429_436 |
| **range_note** | 0x00429–0x00436: game state, simulation, per-frame |
| **seed_address** | — (slice task) |

## Status

**DONE** — all 22 functions decompiled via Ghidra MCP (`batch_decompile`); control flow and `this` adjustors match prior struct docs (`CDSAudioBank.md`, `CDSAudioBankSample.md`, `formats/dsm_file_format.md`). No new Ghidra mutations required; `save_program` skipped (MCP disconnected after decompile pass; database already carries R3–R5 naming/typing).

## Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00429040` | `InitializeAndAllocate` | **CDSDsmFile** class-76 factory: `OperatorNew(0x68)` → `InitializeInstance` (`0x00428d40`). Registered @ `0x0047c300` (class id `0x4c`). | Decompile; xref DATA `0x0047c300`; [dsm_file_format.md](../../formats/dsm_file_format.md) |
| `0x00429060` | `CDSDsmFile::HandleScalarDelete` | Scalar-deleting dtor: `HandleDestructInstance` then `_free(this)` when delete flag set. | Decompile; dtor clears handle bank @ `obj+0x54` per CDSAudioBank.md |
| `0x00429080` | `CDSDsmFile::HandleReleaseReadThunk` | **MI thunk** on wav-shaped face: `this` adjusted to `obj+0x04` (`&this[-1].srcBaseOffset+4`), calls `HandleReleaseResource(..., flag=1)`. | Decompile field math; vtable3 slot 7 @ `obj+0x1c` |
| `0x004290a0` | `CDSDsmFile::HandleReleaseWriteThunk` | **MI thunk** on image-source face: `this` → `&this[-1].tailPadding` (≈ `obj+0x38`), `HandleReleaseResource(..., flag=2)`. | Decompile; vtable2 slot 8 |
| `0x004290c0` | `CDSDsmFile::HandleAcquireResource` | Alloc/reuse **0x30-byte handle** (flag byte, cursor=`0x24`, 1 MiB `CDSMemQueue`); append to handle bank `obj+0x54`. Reuses last handle if requested RW bit not yet set. | Decompile + inline plate comment; [dsm_file_format.md](../../formats/dsm_file_format.md) § Multi-stream |
| `0x00429190` | `CDSDsmFile::HandleAcquireReadThunk` | **Fastcall** adjust `ECX-0x1c` → full `CDSDsmFile*`, acquire read handle (`flag=1`), zero `chunkBytesLeft`. | Decompile `param_1 + -0x1c`; vtable3 slot 5 |
| `0x004291b0` | `CDSDsmFile::HandleAcquireWriteThunk` | **Fastcall** adjust `ECX-4` → `obj+0x38` chain face, acquire write handle (`flag=2`). | Decompile `param_1 + -4`; vtable2 slot 5 |
| `0x004291c0` | `CDSAudioBank_TypeinfoAdjust_14` | MSVC MI typeinfo adjustor: `return param_1 ? param_1 + 0x14 : 0`. Facet entry for bank-deserialize vtable @ full `+0x14`. | Decompile; xrefs DATA @ `0x0047c540`, `0x0047cad0`, …; CDSAudioBank.md MI section |
| `0x004291d0` | `IDSAudioSource::CDSAudioBankSample_HandleResourceRead` | **PCM Read** (vtable `0x486f10` slot 6): `this=alloc+4`; bounded `memcpy` from `pPcmBuffer` at face `+0x1c` (full `+0x20`); wraps `*pReadCursor` at `dwSampleByteSize`. | Decompile typed `IDSAudioSource*`; CDSAudioBankSample.md Read table |
| `0x00429240` | `CDSAudioBank::CDSAudioBank_ReleaseSampleSlots` | Walk `slotVector` (`+0x18`/`+0x1c`), `Release` each slot, `CDSPtrSlotVec_Resize(0)`. | Decompile; xrefs: `CDSAudioBank_dtor@0x00429430`, `CDSAudioBank_Deserialize@0x00429633`, `Catch@0x0042985a` |
| `0x00429280` | `CDSJpegImage_JpegSetupScope_dtor` | SEH scope dtor: vcall `vtable+0x1c` on held object, then `Release` via `vtable+8`. | Decompile; exception frame pattern (`ExceptionList`, `LAB_00478eb8`) |
| `0x004292f0` | `CDSAudioBank::CDSAudioBank_GetTypeInfo` | Returns `&DAT_004b3ae0` (class-67 `CDSAudioBank` RTTI). | Decompile |
| `0x00429300` | `CDSFileStream::CDSFileStream_AdjustThisOffset` | Shared **MI adjustor** thunk: `CDSChain_AdjustThisOffset(this-0x10)`. | Decompile `param_1 + -0x10`; exact vtable slot owner **UNK** (no xref pass — MCP drop) |
| `0x00429310` | `CDSAudioBank::CDSAudioBank_ScalarDeletingDtor_thunk_Sub0c` | Scalar-delete thunk: adjust to `&this[-1].pVftable_CDSWavStream_IDSChained5` → `CDSAudioBank_ScalarDeletingDtor`. | Decompile; MI `-0x0c` from sub0c face |
| `0x00429320` | `CDSAudioBank::CDSAudioBank_AdjustOffsetDtor` | Dtor thunk: `this-0x2c` (`&this[-1].dwStreamTellHi`) → scalar deleting dtor. | Decompile |
| `0x00429330` | `CDSAudioBank::CDSAudioBank_ScalarDeletingDtor_thunk` | Primary scalar-delete entry from `IDSReferenced` face: forwards via `this[-1].pPad_operatorNew0x40`. | Decompile |
| `0x00429340` | `CDSAudioBankSample::CDSAudioBankSample_GetTypeInfo` | Returns `&DAT_004b3ab4` (sample class meta). | Decompile |
| `0x00429350` | `CDSAudioBankSample::CDSAudioBankSample_ScalarDeletingDtorThunk` | Scalar-delete from **IDSAudioSource** face (`this-0x20` / `&this[-1].pPcmBuffer`). | Decompile; CDSAudioBankSample.md vtable `0x486f10` slot 3 |
| `0x00429360` | `CDSAudioBankSample::CDSAudioBankSample_ScalarDeletingDtor_thunk_Sub18` | Scalar-delete from **IDSStream** face (`this-0x18`). Plate comment documents vtable `0x486efc` slot 3. | Decompile + comment; CDSAudioBankSample.md IDSStream table |
| `0x00429370` | `CDSAudioBankSample::CDSAudioBankSample_dtor` | Body dtor: `Runtime_Free(pPcmBuffer@+0x20)`; `CDSIDSReferencedSub_ReleaseHeldRef(this+4)`; restore primary vtable. | Decompile `param_1[8]` free, `param_1+1` release |
| `0x004293e0` | `CDSAudioBank::CDSAudioBank_dtor` | Restore bank vtables (`0x486exx`), `ReleaseSampleSlots`, clear `slotVector`, `IDSChainedTail_ClearSubObjStash`, reset primary vtable. | Decompile call chain |
| `0x00429470` | `CDSAudioBank_Factory` | Class-67 factory: `OperatorNew(0x20)` compact header; sets vtables + `dwInitFlag=1`; clears `slotVector`. **Not** class-43 `CDSWavStream` `0x40` alloc. | Decompile + plate; xref DATA `CDSAudioBank_StaticClassRegister@0x0047c4b0`; CDSAudioBank.md |

### Slice grouping

1. **`0x00429040`–`0x004291b0`** — `CDSDsmFile` multi-stream handle acquire/release (MJPEG+PCM movie container, class 76). Distinct from standalone `CDSWavStream` / `CDSAudioBank` bank facet despite shared catalog names on thunks.
2. **`0x004291c0`–`0x00429470`** — `CDSAudioBank` / `CDSAudioBankSample` RTTI adjustors, vtable thunks, dtors, slot release, class-67 factory.
3. **`0x00429280`** — JPEG decompress SEH helper (shared runtime utility in slice VA range).

## Ghidra deltas

**none** — decompile pass (2026-06-03) shows symbols and `this` types already applied in R3–R5:

- `CDSDsmFile::*` on DSM handle path (`0x00429040`–`0x004291b0`)
- `IDSAudioSource *` on `CDSAudioBankSample_HandleResourceRead@0x004291d0`
- Named MI thunks and dtors on bank/sample (`0x00429310`–`0x00429370`, `0x004293e0`)
- `CDSAudioBank_Factory` plate @ `0x00429470`

Cosmetic decompiler gaps (no disasm contradiction): `CDSAudioBank_ReleaseSampleSlots` / bank & sample dtors still show `int`/`undefined4*` params under `__fastcall` — optional `set_function_this_type` when MCP available; not applied this session.

## Frida

**none** — PCM read bounds, slot release loop, DSM handle layout, and MI adjust constants are fully provable from static decompile + existing struct docs.

## Remaining UNK

- **`CDSFileStream_AdjustThisOffset@0x00429300`** — which concrete vtable installs this thunk (shared stream family); needs `get_xrefs_to` when Ghidra MCP restored.
- **`CDSJpegImage_JpegSetupScope_dtor@0x00429280`** — exact SEH parent function(s) in JPEG load path; behavior (release held decompress state) clear, callers not enumerated this session.
- **Class-67 `OperatorNew(0x20)` vs bank `0x40` facet** — whether compact factory instances ever grow to full `0x40` layout (open from CDSAudioBank.md UNK).
