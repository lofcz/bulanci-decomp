# Round 6 logic — Task 29 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 29 |
| **title** | Logic sim_429_436: 0x004327e0–0x00432be0 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — Ghidra MCP (`user-ghidra-mcp`) returned `Not connected` for the entire session (no live `decompile` / `disassemble` / `get_xrefs_to`). Per-function logic below is sourced from **prior Ghidra-verified** rounds (R3/R4 task 33, batch 30 slice 30, R4 task 32/33) plus `master_vtable_catalog.csv` / `vftable_methods.csv`. No Frida: decode/lifecycle paths are statically closed in existing docs.

## 3. Functions

Slice covers **ClassID 52 (`CDSFlxFile`)** FLX chunk decoders, five MI vtable faces, factory/lifecycle, stream bind, and decode-consumer allocation. `DecodeFrame` @ `0x00432c60` is **task 30** (next slice) but is referenced here as the sole dispatcher for chunk helpers.

### 3.1 Chunk decoders (called from `DecodeFrame` switch)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004327e0` | `DecodeCopyColor` | `memcpy(colorPlane, chunkBody, n)` — FLX inner tag **8** | `sprite_container.md` §4.3; `flx_file_format.md` §DecodeCopyColor; `DecodeFrame` case 8 @ `0x00432c60` (R3) |
| `0x00432800` | `DecodePaletteRgb` | FLI/FLC **COLOR_256**-shape palette update: relative skip + `(count+1)` BGR triplets into consumer palette | `sprite_container.md` §4.4; `flx_file_format.md` §DecodePaletteRgb; tag **9** in `DecodeFrame` |
| `0x00432850` | `DecodeRegionList` | Parse up to **32** `(u8 kind, s16 x, s16 y)` records from chunk body (no count prefix); fan-out via `NotifyRegionList@0x00436eb0` | `sprite_container.md` §4.5; `flx_file_format.md` §DecodeRegionList; tag **11** |

### 3.2 Meta-face vtable methods (`vf_metaFace` @ outer `+0x04`, vtable `0x004872a4`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00432840` | `ResetCursor` | Vtable **slot 6** (`+0x18`): `*pTrackEntry = 0` — rewinds frame cursor before playback | `vftable_methods.csv` `004872a4;;6;00432840`; `CDSFlxFile.md` §DecodeFrame callsites (`BeginCurrentTrackPlayback@0x00439b57` → `CALL [vtable+0x18]`) |
| `0x00432b60` | `CDSFlxFile_CloseStream` | Vtable **slot 4**: release `pSourceStream`, free `pDecodeBuffer`, Seek64 using meta-face `[+0x2c/+0x30]` (= outer `streamTell`) | `vftable_methods.csv` slot 4; R4 task 33 disasm @ `0x00432ba2` |
| `0x00432be0` | `CreateBoundClone` | Vtable **slot 9**: `OperatorNew(0x60)` → `CDSObject_CtorWithImage`; copies `nBitmapHeight`/`nBitmapWidth`/`dwTotalSize` into decode consumer; returns per-instance **0x60** image surface for `DecodeFrame` `param_2` | `CDSFlxFile.md` downstream chain; `CDSObject.md` alloc @ `0x00432c0e`; `vftable_methods.csv` slot 9 |

### 3.3 Primary / refcount face (`vf_IDSReferenced` @ outer `+0x00`, vtable `0x004872d0`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004328e0` | `GetClassRegistry` | Vtable slot 0: `return &DAT_004b7e08` (ClassID **52** `CDSResInfo` registry) | `vftable_methods.csv`; init stub pushes factory `0x00432a50` @ `0x0047cdd0` (`sprite_container.md` §1.1) |
| `0x00432950` | `ReleaseRef` | Vtable slot 2: dec `refcount` @ outer `+0x20`; chained delete via `ScalarDeletingDtor` when zero | `CDSFlxFile.md` layout `+0x20`; `vftable_methods.csv` |
| `0x00432aa0` | `ScalarDeletingDtor` | Vtable slot 1: `DestructInPlace`; `_free(this)` if `param_1&1` | `CDSFlxFile.md` size proof; `vftable_methods.csv` |
| `0x004329c0` | `DestructInPlace` | Non-scalar dtor: free `pDecodeBuffer` @ `[0xf]`, release `pSourceStream` @ `[0xe]`, `IDSChainedTail_ClearSubObjStash(this+0x28)`, restore primary vtable | `CDSFlxFile.md` layout rows `+0x38`/`+0x3c`/`+0x28` |
| `0x00432a50` | `CDSFlxFile_CreateObject` | Class factory: `OperatorNew(0x50)`; stamp five vtables; `refcount=1`; zero `bodySeekBiasLo` @ `+0x2c` (sole writer) | R4 task 33 @ `0x00432a6d`; batch 30; `CDSFlxFile.md` size proof |

### 3.4 MSVC MI adjustor thunks (not game semantics — ECX adjust + forward)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004328f0` | `CDSFlxFile_Sub1cDtor` | Event face (`+0x1c`) scalar-deleting dtor thunk → `DestructInPlace` on adjusted base | `vftable_methods.csv` `00487290;;3` |
| `0x00432980` | `CDSFlxFile_Sub1cRelease` | Event face release thunk | `vftable_methods.csv` slot 2 |
| `0x00432910` | `CDSFlxFile_Sub04Dtor` | Meta face (`+0x04`) dtor thunk | `vftable_methods.csv` `004872a4;;3` |
| `0x00432990` | `CDSFlxFile_Sub04Release` | Meta face release thunk | `vftable_methods.csv` slot 2 |
| `0x00432920` | `CDSFlxFile_Sub24Dtor` | IDSResource face (`+0x24`) dtor thunk | `vftable_methods.csv` `00487274;;3` |
| `0x004329a0` | `CDSFlxFile_Sub24Release` | IDSResource face release thunk | `vftable_methods.csv` slot 2 |
| `0x00432900` | `CDSFlxFile_Sub20ChainOp` | IDSResource face slot 1: adjust `ECX-=0x20`, forward to `CDSChain::FUN_0042ac90` | `master_vtable_catalog.csv`; `flx_file_format.md` §Sub20ChainOp |
| `0x00432930` | `CDSFlxFile_Sub24ChainOp` | Chain face (`+0x28`) slot 1: adjust `ECX-=0x24`, forward to same `CDSChain` helper | `vftable_methods.csv` `0048725c;;1` |
| `0x00432940` | `CDSFlxFile_Sub28Dtor` | Chain face dtor thunk | `vftable_methods.csv` slot 3 |
| `0x004329b0` | `CDSFlxFile_Sub28Release` | Chain face release thunk | `vftable_methods.csv` slot 2 |

### 3.5 Stream bind (`vf_IDSResource` @ outer `+0x24`, vtable `0x00487274`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00432ac0` | `CDSFlxFile_BindStream` | Vtable slot 4 on **+0x24 face**: `Read(0x24)` file header; map nine dwords to outer fields; `Tell()` → `streamTell` @ `+0x30`; store stream @ `+0x38`; set body cursors @ `+0x40`/`+0x44` | R3/R4 task 33 disasm; `CDSFlxFile.md` §Agent todo 33 table; plate comment @ `0x00432ac0` (prior Ghidra apply) |

### Control-flow summary (FLX resource lifecycle)

```text
ClassID 52 registry (0x0047cdd0) → CreateObject (0x50 B handle)
  → BindStream (+0x24 face): read 36 B header, wire stream + geometry + anim meta
  → CreateBoundClone (meta vtable slot 9): 0x60 B CDSObject image consumer
Track manager:
  BeginCurrentTrackPlayback → ResetCursor (vtable+0x18) zeros track entry cursor
  TM_AdvanceFrame → DecodeFrame (vtable+0x1c, task 30) with consumer = pRenderTarget
CloseStream / DestructInPlace: release stream, free decode scratch, tear down embedded CDSChain @ +0x28
```

## 4. Ghidra deltas

**None applied** — MCP unavailable (`Not connected`).

**Already applied in prior rounds (not re-run):**

- `CDSFlxFile` struct 80 B with canonical field names (batch 30 / R3 task 33)
- `set_function_prototype` on `DecodeFrame@0x00432c60`, `CreateBoundClone@0x00432be0` (batch 30)
- Decompiler/plate comments on `BindStream`, `CreateObject` `bodySeekBiasLo`, `DecodeFrame` seek operands (R4 task 33)
- All 22 symbols renamed per `_cds_vtable_renames.tsv` / `flx_file_format.md` rename table

**Queued if MCP returns (optional polish only — evidence already in struct doc):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `set_function_this_type` | `CDSFlxFile_BindStream@0x00432ac0` | `this` is IDSResource face (`CDSFlxFile` adjustor **−0x24**) |
| `set_function_this_type` | `ResetCursor`, `CloseStream`, `CreateBoundClone` | Meta face (`CDSFlxFile` adjustor **−0x04**) |
| `force_decompile` | Above after this-type fix | Prove decompiler field access on `nBitmapWidth` / `streamTell` |

## 5. Frida

**none** — Chunk algorithms, header map, vtable dispatch, and track-manager callsites are statically proven (R3/R4 + pack unpacker cross-check). Runtime would only duplicate `TM_AdvanceFrame` → `DecodeFrame` already documented in `CDSFlxFile.md`.

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| Live Ghidra re-verify of this slice | MCP down; could not refresh decompile/disasm/xrefs in R6 session |
| `CDSChain::FUN_0042ac90` callee name | Target of `Sub20ChainOp` / `Sub24ChainOp`; still `FUN_`-named outside this slice (`flx_file_format.md` open Q5) |
| `bodySeekBiasLo` @ outer `+0x2c` | Factory zero only; no non-zero writer in `.text` (R4 task 33 closed — field likely dead) |
| `sprite_container.md` §2.1 handle layout | Partially stale (+0x08 labeled `void*` stream); superseded by `CDSFlxFile.md` canonical layout (`nChannels` @ `+0x08`) |

## Cross-links

- [`CDSFlxFile.md`](../struct_recovery/CDSFlxFile.md) — VERIFIED 0x50 layout, BindStream map, DecodeFrame dispatch contract
- [`sprite_container.md`](../../formats/sprite_container.md) — chunk decoder algorithms §4, DecodeFrame state machine §3
- [`flx_file_format.md`](../../formats/flx_file_format.md) — on-disk header ↔ outer field table
- [`CDSObject.md`](../struct_recovery/CDSObject.md) — `CreateBoundClone` 0x60 consumer
- [`round3_task_33_report.md`](../struct_recovery/round3_task_33_report.md) — BindStream disasm proof
- [`round4_task_33_report.md`](../struct_recovery/round4_task_33_report.md) — `bodySeekBiasLo` / streamTell face aliases
