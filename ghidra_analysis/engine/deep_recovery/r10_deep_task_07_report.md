# Round 10 — Deep Task 07 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **title** | R6 rerun: sim slice `0x004327e0`–`0x00432be0` |
| **kind** | logic_rerun |
| **prior_round** | 6 / task 29 |
| **seed_address** | `0x004327e0` |
| **addresses** | `0x004327e0`, `0x00432840`, `0x004328a0`, `0x00432900`, `0x00432960`, `0x004329c0`, `0x00432a20`, `0x00432a80`, `0x00432ae0`, `0x00432b40`, `0x00432ba0`, `0x00432be0` |
| **evidence_paths** | [round6_logic_task_29_report.md](../logic_recovery/round6_logic_task_29_report.md), [CDSFlxFile.md](../struct_recovery/CDSFlxFile.md), [CDSMpx.md](../struct_recovery/CDSMpx.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verifies the full **CDSFlxFile** (ClassID 52 / FLX) slice from R6 task 29. Chunk decoders, lifecycle, stream bind, and MI thunks match prior static docs. **`pSubObjStash@+0x40`** on the MPx registry shell remains **BLOCKED** (sole writer is null @ `0x0043312d`, one function past slice end). **MPx decode overlay** (`mad_stream` vs persist band) lives in the adjacent band starting `DecodeFrame@0x00432c60` / `CDSMpx_dtor@0x00432f40` — documented by cross-reference, not re-derived in this slice.

## Functions / Struct

Slice contains **22 functions** for **`CDSFlxFile`** (`sizeof == 0x50`). Manifest `CDSMpx.md` applies to the **next** `.text` band (MPx persist/decode); do not conflate with FLX outer `+0x40` (`bodyStartCursor`).

### Chunk decoders (called from `DecodeFrame@0x00432c60`, outside slice)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004327e0` | `DecodeCopyColor` | `memcpy(colorPlane, chunkBody, n)` — inner tag **8** | **Decomp:** `_memcpy(dst,src,n)`. **Xrefs (1):** `DecodeFrame@0x00432d80` `CALL` |
| `0x00432800` | `DecodePaletteRgb` | FLI/FLC **COLOR_256**-shape palette: skip + `(count+1)` BGR triplets into consumer palette — tag **9** | **Decomp:** loop with `param_3 += startSkip*2`, 3-byte RGB writes. Plate comment documents pack survey |
| `0x00432850` | `DecodeRegionList` | Up to **32** records `{u8 kind, s16 x, s16 y}` → `NotifyRegionList` — tag **11** | **Decomp:** stack buffer `local_17c[95]`, max 32 entries, `NotifyRegionList(param_3, count, table)` |

### Meta-face vtable (`vf_metaFace` @ outer `+0x04`, vtable `0x004872a4`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00432840` | `ResetCursor` | Vtable slot 6 (`+0x18`): `*pTrackEntry = 0` | **Decomp:** `*param_1 = 0; return` |
| `0x00432b60` | `CDSFlxFile_CloseStream` | Slot 4: release stream, realloc decode scratch, `Seek64` via meta `+0x2c/+0x30` | **Decomp:** free `pDecodeBuffer`, `Seek64(bodySeekBiasLo, dwStreamTellLo, 0)`, `Read` into new buffer |
| `0x00432be0` | `CreateBoundClone` | Slot 9: `OperatorNew(0x60)` → `CDSObject_CtorWithImage`; copy `dwTotalSize` bytes into consumer embed | **Decomp:** `OperatorNew(0x60)`, ctor with `&vf_metaFace`, `nBitmapHeight`, copies 4 bytes from `dwTotalSize`. **Xrefs (1):** data `0x004872c8` (vtable slot 9) |

### Primary / refcount face (`vf_IDSReferenced` @ `+0x00`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004328e0` | `GetClassRegistry` | Slot 0: `return &DAT_004b7e08` (ClassID 52) | **Decomp:** `return &DAT_004b7e08` |
| `0x00432950` | `ReleaseRef` | Slot 2: dec `refcount@+0x20`; scalar delete when zero | **Decomp:** tests `*(param_1+0x20)==1`, dispatches via event vtable |
| `0x00432aa0` | `ScalarDeletingDtor` | Slot 1: `DestructInPlace`; `_free(this)` if `param_1&1` | **Decomp:** calls `DestructInPlace(&vf_IDSReferenced)` |
| `0x004329c0` | `DestructInPlace` | Free `pDecodeBuffer@+0x3c`, release `pSourceStream@+0x38`, `IDSChainedTail_ClearSubObjStash(this+0x28)`, restore primary vtable | **Disasm:** `MOV EAX,[ESI+0x3c]` free; `[ESI+0x38]` release; `LEA ECX,[ESI+0x28]` → `CALL 0x00434250`; `MOV [ESI],0x47f6a8` |
| `0x00432a50` | `CDSFlxFile_CreateObject` | `OperatorNew(0x50)`; stamp five vtables; `refcount=1`; zero `bodySeekBiasLo@+0x2c` | **Decomp:** `puVar1[0xb]=0` sole writer of `+0x2c`; vtables `0x4872d0/2a4/290/274/25c` |

### MI adjustor thunks

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004328f0` | `CDSFlxFile_Sub1cDtor` | Event face (`+0x1c`) scalar dtor thunk | R3 vtable catalog |
| `0x00432980` | `CDSFlxFile_Sub1cRelease` | Event face release thunk | R3 vtable catalog |
| `0x00432910` | `CDSFlxFile_Sub04Dtor` | Meta face (`+0x04`) dtor thunk | R3 vtable catalog |
| `0x00432990` | `CDSFlxFile_Sub04Release` | Meta face release thunk | R3 vtable catalog |
| `0x00432920` | `CDSFlxFile_Sub24Dtor` | IDSResource face (`+0x24`) dtor thunk | R3 vtable catalog |
| `0x004329a0` | `CDSFlxFile_Sub24Release` | IDSResource face release thunk | R3 vtable catalog |
| `0x00432900` | `CDSFlxFile_Sub20ChainOp` | IDSResource slot 1: `ECX-=0x20` → `CDSChain` helper | **Disasm:** `SUB ECX,0x20; JMP 0x0042ac90` |
| `0x00432930` | `CDSFlxFile_Sub24ChainOp` | Chain face (`+0x28`) slot 1: `ECX-=0x24` → same helper | R3 vtable catalog |
| `0x00432940` | `CDSFlxFile_Sub28Dtor` | Chain face dtor thunk | R3 vtable catalog |
| `0x004329b0` | `CDSFlxFile_Sub28Release` | Chain face release thunk | R3 vtable catalog |

### Stream bind (`vf_IDSResource` @ outer `+0x24`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00432ac0` | `CDSFlxFile_BindStream` | `Read(0x24)` header; map nine dwords; `Tell()` → `streamTell`; store stream; set body cursors | **Disasm:** `ESI=ECX` (+0x24 face); `Read 0x24` stack; stores via `[ESI-0x1c]`..`[ESI+0x28]` → outer `+0x48`..`+0x08`; `file+0x04` → `[ESI-0x1c]` = outer `bodyStartCursor@+0x40`; plate @ `0x00432b18` |

### FLX lifecycle (this slice)

```text
ClassID 52 registry → CreateObject (0x50 B)
  → BindStream (+0x24 face): 36 B header → geometry + anim meta + body cursors
  → CreateBoundClone (meta slot 9): 0x60 B CDSObject decode consumer
Track playback (outside slice):
  BeginCurrentTrackPlayback → ResetCursor (vtable+0x18)
  TM_AdvanceFrame → DecodeFrame (vtable+0x1c) → chunk helpers in this slice
DestructInPlace / CloseStream: release stream, free scratch, ClearSubObjStash(chain@+0x28)
```

### `pSubObjStash@+0x40` investigation (acceptance criterion)

| Object | `+0x40` field | Writers | Status |
|--------|---------------|---------|--------|
| **`CDSFlxFile`** | `bodyStartCursor` (uint) | `BindStream` from file `+0x04`; `DecodeFrame` cursor wrap | **PROVEN** — not a stash pointer |
| **`CDSMpxStreamRegistry`** (`0x48` shell) | `pSubObjStash` (`IDSChainedTailStash *`) | **Only** `CDSMpxStream_CreateObject@0x0043312d` `MOV [EAX+0x40], ECX` with `ECX=0` | **BLOCKED** — no non-null writer in `.text` |
| Other `MOV [reg+0x40]` hits | unrelated fields | e.g. `CDSImage_InitDefaults@0x0042559f`, `InitializeInstance@0x00428d54` — zero init on different types | Not `pSubObjStash` |

**Clear helper protocol** (`IDSChainedTail_ClearSubObjStash@0x00434250`): given stash **facet** pointer, load `*(facet+4)`; if non-null, zero `stash+0x10` / `stash+0x14`, null facet+4. **FLX** passes `this+0x28` (chain facet). **MPx** dtor `@0x004330b0` passes `param_1+0x3c` (stash facet) — live decomp confirms `puVar1 = param_1 + 0xf` (dword index) = `P+0x3c`.

### MPx decode overlay (adjacent band — not in assigned addresses)

Persist band `P+0x08..+0x30` on the `0x98c8` **`CDSMpx`** object aliases **`mad_stream`** tail after `AttachBitstream` / `ResetDecoderState`. On-disk order from `SaveMpxFile@0x00432eb0` (next slice): `[dwPayloadBytes@P+0x30][mpxFormatTail@P+0x08 (12 B)][mpeg payload]`. Full offset map: [CDSMpx.md](../struct_recovery/CDSMpx.md) §Persist band overlay and [CDSMpxStream.md](../struct_recovery/CDSMpxStream.md).

## Ghidra deltas

**none** — Live verification matches prior R3/R4/R5 apply. No prototype/struct fixes required in this slice.

## Decomp corrections (IDA vs Ghidra)

| Issue | Ghidra decomp (live) | Disasm / struct truth |
|-------|---------------------|------------------------|
| `CDSFlxFile_BindStream` field names | Uses `this->nSeqTotalDurationMs` as stream pointer, negative-index `this[-1].*` for outer fields | **Expected artifact:** `this` is IDSResource face (`CDSFlxFile+0x24`). Disasm @ `0x00432ac0` shows correct `[ESI±offset]` → outer layout per plate comment. Use disasm/plate, not raw decomp field names. |
| `CreateBoundClone` consumer typing | Casts embed to `CDSUpdatedItem` for 4-byte copy | Semantic target is `CDSObject` embed `+0x14` (`dwTotalSize` copy) — cosmetic decomp only |

## Frida

**none** — FLX chunk algorithms, header map, vtable dispatch, and `IDSChainedTail_ClearSubObjStash` protocol are statically closed. MPx overlay is persist-vs-decode memory alias (documented); runtime would not resolve `pSubObjStash` type without a non-null store path.

## Remaining UNK

| Item | Status |
|------|--------|
| `pSubObjStash@+0x40` concrete type on MPx path | **BLOCKED** — confirmed live: sole writer null @ `0x0043312d`; `search_byte_patterns` `89 48 40` has no MPx non-null assign |
| `IDSChainedTailStash.pClearField_10/14` semantics | **UNK** — cleared by `IDSChainedTail_ClearSubObjStash`; no non-null stash observed on MPx/FLX paths |
| `CDSChain` helper @ `0x0042ac90` callee name | **UNK** — target of `Sub20ChainOp` / `Sub24ChainOp` (outside slice) |
| WAVEFORMAT bit-field alias inside `mpxFormatTail` during decode | **BLOCKED** — adjacent MPx band; dual-purpose bytes documented, bit layout not isolated |
| Manifest `CDSMpx.md` vs slice content | **Clarified** — this task address band is **FLX** (`CDSFlxFile`); MPx acceptance items apply to **`0x00432c60+`** band (R6 task 30) |

## Cross-links

- [round6_logic_task_29_report.md](../logic_recovery/round6_logic_task_29_report.md) — prior PARTIAL (MCP down)
- [round6_logic_task_30_report.md](../logic_recovery/round6_logic_task_30_report.md) — adjacent MPx/DecodeFrame slice
- [CDSFlxFile.md](../struct_recovery/CDSFlxFile.md) — VERIFIED `0x50` layout
- [CDSMpxStream.md](../struct_recovery/CDSMpxStream.md) — `pSubObjStash` @ registry `+0x40`
