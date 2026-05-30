# CDSFlxFile

## Status

**VERIFIED** — instance size `0x50` (80 bytes). ClassID **52** (`BitmapSprite`) stream handle in `sprite_container.md`. Five MI vtable slots, 36-byte file header, stream/decode pointers, and frame-cursor fields proven via factory ctor, `BindStream`, `DecodeFrame`, `CreateBoundClone`, and `DestructInPlace`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSFlxFile) == 0x50` | `CDSFlxFile_CreateObject` @ `0x00432a50` | `OperatorNewWithBadAlloc(0x50)`; vtables stamped through `puVar1[10]` → last fixed slot at `+0x28`; factory zeroes `+0x38`/`+0x3c` (`[0xe]`/`[0xf]`) |
| `sizeof(CDSFlxFile) == 0x50` | `CDSStreamException::FUN_00430a90` @ `0x00430ab3` | Same `0x50` pattern for sibling stream resource class (cross-check) |
| Heap free on delete | `ScalarDeletingDtor` @ `0x00432aa0` | `_free(this)` after `DestructInPlace` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_IDSReferenced` | `CDSFlxFile_CreateObject@0x00432a50` `*puVar1 = 0x4872d0`; `DestructInPlace@0x004329c0` restores `g_pCDSObject_vftable_IDSReferenced` |
| `0x04` | 4 | `void *` | `vf_metaFace` | `CDSFlxFile_CreateObject@0x00432a50` `puVar1[1] = 0x4872a4` (`DecodeFrame` / `CloseStream` face) |
| `0x08` | 4 | `uint` | `nChannels` | `BindStream@0x00432ac0` file `+0x18` → outer `+0x08`; master pack always `3` (`sprite_container.md` §6) |
| `0x0c` | 4 | `uint` | `nBitmapHeight` | file `+0x14` → outer `+0x0c`; **bitmap height in pixels**; passed to `CDSImage__Allocate` via `&+0x0c` in `CreateBoundClone@0x00432be0` |
| `0x10` | 4 | `uint` | `nBitmapWidth` | file `+0x10` → outer `+0x10`; **bitmap width in pixels** (legacy header label was `height`) |
| `0x14` | 4 | `uint` | `nSeqTotalDurationMs` | file `+0x1c` `inMemSize` → outer `+0x14` @ `CDSFlxFile_BindStream@0x00432ac0` (`MOV [ESI+0x10]` on `+0x24` face @ `0x00432b18`). **Anim:** meta-face `seq[0x10]` for `TM_AdvanceFrame@0x004399b0` Bresenham total clip **ms**. Master pack: **1/130** sprites have `inMemSize == 0x470` (`sizeof(CBulPicture)`); observed **210..8591** (median ~65 ms/frame when divided by `flags+1`). Legacy header name “alloc hint” is misleading. |
| `0x18` | 4 | `uint` | `nSeqFrameCountMinusOne` | file `+0x20` `flags` → outer `+0x18`; `== animFrameCount - 1` (130/130). **Anim:** meta-face `seq[0x14]` — frame-index wrap compare in `TM_AdvanceFrame`. |
| `0x1c` | 4 | `void *` | `vf_event` | `CDSFlxFile_CreateObject@0x00432a50` `puVar1[7] = 0x487290` |
| `0x20` | 4 | `int` | `refcount` | `CDSFlxFile_CreateObject@0x00432a50` `puVar1[8] = 1`; `ReleaseRef@0x00432950` dec/tests `+0x20` |
| `0x24` | 4 | `void *` | `vf_IDSResource` | `CDSFlxFile_CreateObject@0x00432a50` `puVar1[9] = 0x487274` (`BindStream` adjustor `this-0x24`) |
| `0x28` | 4 | `void *` | `vf_chain` | `CDSFlxFile_CreateObject@0x00432a50` `puVar1[10] = 0x48725c`; `DestructInPlace@0x004329c0` `FUN_00434250(this+0x28)` |
| `0x2c` | 4 | `int` | `bodySeekBiasLo` | `CDSFlxFile_CreateObject@0x00432a6d` **sole writer** `MOV [EAX+0x2c],0` (R4 todo 33: no non-zero store in `.text`). **Not read** on outer base; `DecodeFrame`/`CloseStream` use **meta `+0x2c`** = outer `dwStreamTellLo` @ `+0x30` |
| `0x30` | 8 | `uint64` | `streamTell` | `CDSFlxFile_BindStream@0x00432ac0` `*(this+0xc)` when `this` is `+0x24` face → outer `+0x30` |
| `0x38` | 4 | `void *` | `pSourceStream` | `DestructInPlace@0x004329c0` `[0xe]`; `BindStream@0x00432ac0` `*(this+0x14)` |
| `0x3c` | 4 | `void *` | `pDecodeBuffer` | `DestructInPlace@0x004329c0` `[0xf]` free; `DecodeFrame@0x00432c60` alloc/read |
| `0x40` | 4 | `uint` | `bodyStartCursor` | `DecodeFrame@0x00432c60` wrap sets `*param_1 = *(this+0x3c)` on `+0x04` face; `BindStream` writes via `this+0x1c` |
| `0x44` | 4 | `uint` | `bodyEndCursor` | `DecodeFrame@0x00432c60` compares `*(this+0x40)`; `BindStream` `*(this+0x20)=len-0x24` |
| `0x48` | 4 | `uint` | `dwTotalSize` | file `+0x00` → outer `+0x48`; `== len(raw)`; stored on decode consumer `+0x14` in `CreateBoundClone@0x00432be0` |
| `0x4c` | 4 | `uint` | `dwEncodedSize2` | file `+0x08` → outer `+0x4c`; usually equals `encodedSize` (`bodyStartCursor` dword) |

## Downstream chain (FLX → portrait)

| Stage | Type / size | Role | Evidence |
|-------|-------------|------|----------|
| Resource pool | `CDSFlxFile` `0x50` | ClassID **52** lazy stream + header | `CBulPicture_ctor@0x004101d0` `g_pApp+0x70` factory; `CheckedVirtualBaseCast(..., DAT_004b826c)` |
| Decode consumer | `CDSObject` `0x60` | `CreateBoundClone@0x00432be0` → `CDSObject_CtorWithImage`; `DecodeFrame` `param_2` | `OperatorNew(0x60)`; planes via `GetColorPlane` / `GetPaletteBuffer` (`sprite_container.md` §2.3) |
| View wrapper | `CBulPicture` `0x470` | `pBitmap` @ `+0x68` = `CDSStaticDrawableFace *` from cast; extent `origin + cast->nBlitExtentW/H` | `CBulPicture_ctor@0x004101d0` — cast base **`CDSFlxFile+0x08`** so `+4/+8` = `nBitmapHeight` / `nBitmapWidth` (`+0x0c`/`+0x10`); `DrawSurface` → `BlitDispatch` |

`DecodeFrame` does **not** write into `CBulPicture` directly; it fills the `0x60` consumer (`CDSImage` embed), which the view binds as `pBitmap`.

## DecodeFrame callsites (worker 31 / 2026-05-30)

| Kind | Caller | Address | Evidence |
|------|--------|---------|----------|
| **Static** | `g_pCDSFlxFile_vftable_meta` slot 7 | `0x004872c0` → `0x00432c60` | Sole `get_xrefs_to(DecodeFrame)` — data pointer only; no direct `CALL 0x00432c60` in `.text` |
| **Dynamic** | `TM_AdvanceFrame` → `seq->vfn[7]` | `0x00439a0e`–`0x00439a15` | `MOV ECX,[track+4]`; `CALL [vtable+0x1c]` with `(trackEntry, renderTarget)` |
| **Dynamic** | `BeginCurrentTrackPlayback` → `seq->vfn[6]` (rewind) | `0x00439b57` | `CALL [vtable+0x18]` = `ResetCursor@0x00432840`; zeros `*trackEntry`; chains `TM_AdvanceFrame` |
| **Dynamic** | `TM_SeekToFrame` loop | `0x00439a53` | Repeated `TM_AdvanceFrame` until frame index catches up |
| **Dynamic** | `CDSVideoPlayer_AdvanceFrameAndPauseIfDone` | `0x00439b79` | Tail-call / call into `TM_AdvanceFrame` |
| **Dynamic** | `CDSAudioVideoPlayer_SetupTrack` | `0x0043bbe3` | Calls `TM_AdvanceFrame` after track wiring |

**Dispatch contract (FLX / ClassID 52):** track-manager entry is `{ u32 frameCursor, CDSAnimSequence* seq }` (`anim_runtime.md`). For BitmapSprite resources, `seq` is the **`CDSFlxFile` meta subobject** (`+0x04`; vtable `0x004872a4`). `DecodeFrame(this, pFrameCursor, consumer)` receives `pFrameCursor = trackEntry` (cursor in entry `+0x0`), `consumer = trackManager.pRenderTarget` (`+0x30`, typically `CreateBoundClone` `0x60` surface). **Not** written into `CBulPicture+0x68` directly — view binds decoded planes via `pBitmap` after pool load (`CBulPicture_ctor@0x004101d0`).

## Key functions (Ghidra)

| Symbol | Address | Notes |
|--------|---------|-------|
| `CDSFlxFile_CreateObject` | `0x00432a50` | `OperatorNew(0x50)` |
| `CDSFlxFile_BindStream` | `0x00432ac0` | `Read(0x24)` header |
| `CDSFlxFile_DecodeFrame` | `0x00432c60` | Chunk dispatcher; `void DecodeFrame(CDSFlxFile*, uint *pFrameCursor, CDSObject *consumer)` |
| `CDSFlxFile_CreateBoundClone` | `0x00432be0` | `OperatorNew(0x60)` playhead |
| `CDSFlxFile_CloseStream` | `0x00432b60` | Release stream + scratch buffer |

## Ghidra apply

```
get_struct_layout CDSFlxFile  → Size: 80 (0x50)
  Header band (+0x08..+0x18): nChannels, nBitmapHeight, nBitmapWidth, nSeqTotalDurationMs, nSeqFrameCountMinusOne
  Tail: dwTotalSize, dwEncodedSize2, bodySeekBiasLo, streamTellLo/Hi, pSourceStream, pDecodeBuffer, body cursors
modify_struct_field nSeqTotalDurationMs @ +0x14; nSeqFrameCountMinusOne @ +0x18
set_decompiler_comment @ 0x00432b18  (hdr inMemSize → seq[0x10] total clip ms)
rename_function FUN_004340c0 → CDSStrmStgLoadingInfo_CreateObject
set_decompiler_comment CDSFlxFile_BindStream @ 0x00432ac0  (file dword → outer field table)
set_function_prototype CDSFlxFile_BindStream @ 0x00432ac0  (void __thiscall, IDSResource +0x24 face)
save_program bulanci.exe   # agent todos 32–33 / 2026-05-30
```

## Follow-up (round 3 task 4 / R4 task 33)

- **`bodySeekBiasLo` @ outer `+0x2c`:** factory zero only (`CDSFlxFile_CreateObject@0x00432a6d`). Program-wide `search_byte_patterns` (`89 48 2c`, `89 46 2c`, `c7 40 2c`, …): **no** non-zero store targeting FLX outer `+0x2c`; `BindStream` does not touch it.
- **Seek addends in `DecodeFrame` / `CloseStream`:** `this` is meta face `+0x04`; `[ESI+0x2c]` / `[ESI+0x30]` are **`dwStreamTellLo` / `dwStreamTellHi`** (outer `+0x30` / `+0x34`) from `BindStream` `Tell()` — not `bodySeekBiasLo`. With factory zero, effective seek uses stream tell only.

## Agent todo 33 (2026-05-30) — BindStream header dwords

| file `+` | outer `+` | name | notes |
|---:|---:|---|---|
| `0x00` | `0x48` | `dwTotalSize` | `== len(raw)` |
| `0x04` | `0x40` | `bodyStartCursor` | first body dword |
| `0x08` | `0x4c` | `dwEncodedSize2` | usually `== bodyStartCursor` |
| `0x10` | `0x10` | `nBitmapWidth` | pixels |
| `0x14` | `0x0c` | `nBitmapHeight` | pixels |
| `0x18` | `0x08` | `nChannels` | master pack always `3` |
| `0x1c` | `0x14` | `nSeqTotalDurationMs` | file `inMemSize`; per-sprite total clip ms (rarely `0x470`) |
| `0x20` | `0x18` | `nSeqFrameCountMinusOne` | file `nAnimFrameCountMinusOne`; `frameCount - 1` |

Evidence: `CDSFlxFile_BindStream@0x00432ac0` disasm (`Read(0x24)` then stores at `[ESI-0x1c]`..`[ESI+0x28]` on the `+0x24` face). Ghidra: decompiler PRE_COMMENT @ `0x00432ac0`; `flx_file_format.md` table updated (replaces `hdr_dword*` placeholders).

## Opcode 0x0C subscribers (agent todo 31 / 2026-05-30)

`DecodeFrame` case `0x0C` @ `0x00432dc5` → `BroadcastFrameTimeHint(consumer, u16)`. Consumer is `CDSImage` decode surface (`CDSObject+4`, 0x60); fan-out walks `m_slotVector` @ `+0x38` / count `+0x40`.

| Mechanism | Address | Evidence |
|-----------|---------|----------|
| Subscribe | `ODSImage__SetImage@0x00439100` | `CIntListInsertSortedOrAppend(&pDrawable->m_slotVector, (int)odsImage+4, …)` — sole writer of decode-consumer slot list |
| Fan-out | `BroadcastFrameTimeHint@0x00436ef0` | `CALL [subscriber+0]+0x10` with `(consumer, u16)` |
| **Not** timing override | whole binary | No store to track-mgr `+0x44` on this callgraph; only `TM_SetFrameDelayOverrideMs@0x00439720` |

**Subscriber** = `ODSImage+4` (`pVf_odsimage` face). **vfn slot 4** (`vtable+0x10`) targets by patched vtable:

| `pVf_odsimage` | slot[4] | Role |
|----------------|---------|------|
| `0x00481ab4` (default) | `CDSView_OnMouseStub@0x00416770` | no-op for u16 hint |
| `0x00481f00` (`CBitmap`/`CGameView`) | `CBitmap_FireOnBitmapEvtFromView@0x00419280` | script `OnBitmapEvt` bridge |
| `0x004830e0` / `0x004831c8` (`CDSBitmap`/`CMovieView`) | `CDSView_OnMouseStub@0x00416770` | anim embed paths |
| `0x00483794` (`CGunMouse`) | `CGunMouse_OnAnimTick@0x00423bd0` | cursor weapon |
| `0x00481ed4` (`CWeapon`) | `Fire@0x004212b0` | weapon overlay |

### FrameTimeHint `u16` semantics (R4 task 31)

Chunk body = one **`u16`** (unpacker: `durationTicks`; **not** `trackMgr+0x44`). Per-subscriber use:

| Subscriber | Handler | `u16` role |
|------------|---------|------------|
| Default / `CDSBitmap` / `CMovieView` | `CDSView_OnMouseStub` | ignored |
| `CBitmap` | `CBitmap_FireOnBitmapEvtFromView` | **`eventCode`** → script export 3 (`OnBitmapEvt`) |
| `CGunMouse` | `CGunMouse_OnAnimTick` | only **`0xFFFF`** → random track + optional `TM_Play` |
| `CWeapon` | `CWeapon::Fire` | weapon-kind matrix (`0`, `0xFFFF`, shot kind `u16+3`) |

## UNK

- Legacy `sprite_container.md` tile-width/height labels at file `+0x0c`/`+0x10` — disasm maps those offsets to **`nBitmapHeight` / `nBitmapWidth`** (pack-verified); tile hints not used in `CreateBoundClone`.
- ~~**`seq[0x10]` / `inMemSize` semantics**~~ — **closed (R4 todo 32):** per-sprite total clip ms from `BindStream`; not `sizeof(CBulPicture)` ([round4_task_32_report.md](./round4_task_32_report.md)).
- **FLX `flags` at `seq[0x14]`** — stores `frameCount−1` while `CDSAnimSequence::dwFrameCount` name implies full count; DSM uses full count at same offset.
