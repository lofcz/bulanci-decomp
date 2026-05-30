# CDSBitmap

## Status

**PARTIAL** — heap size `0x78` verified; full **CDSChained drawable shell** through `+0x67` field-named in Ghidra; embedded `ODSImage` @ `+0x68` instruction-proven.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSBitmap) == 0x78` | `0x00423530` region | `CHistoryScript::HhBuildHistoryBitmapWidget` → `OperatorNewWithBadAlloc(0x78)` → `CDSBitmap_ctor` |
| Same allocation | `0x004267fc` region | `CMenu` icon paths: `OperatorNewWithBadAlloc(0x78)` → `CDSBitmap_ctor` (`main_menu.md`) |
| Embedded span ends at `+0x77` | `0x004393d0` | `ODSImage_ctor(this+0x68)`; `0x68+0x10=0x78` |
| CMovieView embed | `0x004236a0` | `CMovieView::Constructor` calls `CDSBitmap_ctor((CDSBitmap *)this)`; tail fields at `this+0x78` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `pointer` | `vf_primary` | `CDSBitmap_ctor@0x004393d0` |
| 0x04 | 4 | `pointer` | `vf_IDSChained` | same |
| 0x08 | 4 | `uint` | `dwField_08` | `CDSChained_ctor` @ `CDSBitmap_ctor` / `CDSBitmap_SubobjectCtor` |
| 0x0c | 4 | `uint` | `dwField_0c` | same |
| 0x10 | 4 | `pointer` | `vf_slot_10` | `CDSBitmap_ctor` |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CMovieView::InitTrackSequence` `\|= 0x278` @ `*(ushort *)(bitmap+0x14)` |
| 0x16 | 2 | `ushort` | `wPad_16` | `CDSChained` mirror |
| 0x18 | 4 | `pointer` | `vf_IDSEventHandler` | `CDSBitmap_ctor` |
| 0x1c | 4 | `uint` | `dwField_1c` | `CDSChained_ctor` |
| 0x20 | 4 | `int` | `origin_x` | `*(this+0x20)=param_1` @ `CDSBitmap_ctor` |
| 0x24 | 4 | `int` | `origin_y` | `*(this+0x24)=param_2` @ `CDSBitmap_ctor` |
| 0x28 | 4 | `int` | `nBbox_right` | `CDSChained_ctor` zero band |
| 0x2c | 4 | `int` | `nBbox_bottom` | same |
| 0x30 | 4 | `uint` | `dwField_30` | `ResetChainCounters` band |
| 0x34 | 4 | `uint` | `dwField_34` | same |
| 0x38 | 4 | `uint` | `dwField_38` | same |
| 0x3c | 4 | `uint` | `dwField_3c` | same |
| 0x40 | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters` |
| 0x44 | 2 | `ushort` | `wChainInit44` | `CDSChained` mirror |
| 0x46 | 2 | `ushort` | `wChainFlag46` | same |
| 0x48 | 2 | `ushort` | `wChainFlag48` | same |
| 0x4a | 2 | `ushort` | `wChainFlag4a` | same |
| 0x4c | 4 | `pointer` | `pParent` | `CMovieView::OnEvent@0x00422860` |
| 0x50 | 4 | `uint` | `dwField_50` | `CDSChained` mirror |
| 0x54 | 4 | `pointer` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor` |
| 0x58 | 4 | `pointer` | `pVftable_CDSChain_IDSChained` | same |
| 0x5c | 4 | `uint` | `dwField_5c` | ctor zero |
| 0x60 | 4 | `uint` | `dwField_60` | same |
| 0x64 | 4 | `uint` | `dwField_64` | same |
| 0x68 | 0x10 | `ODSImage` | `trackImage` | `ODSImage_ctor(this+0x68)` @ `CDSBitmap_ctor`; `ODSImage__SetImage` @ `InitTrackSequence` |

## Ghidra apply (slice 25 verified)

```
get_struct_layout CDSBitmap → Size: 120 bytes (0x78)
  CDSChained shell +0x00..+0x67 (wViewFlags, bbox/chain band, CDSChain vtables @ +0x54)
  trackImage (ODSImage) @ +0x68
```

**Agent todo 27 (2026-05-30):** Replaced `pViewHeader_28` / `pPad_50` blobs with `CDSChained`-aligned field names; `save_program bulanci.exe`.

**Agent todo 26 round-2 (2026-05-30):** Renamed drawable shell `+0x00..+0x67` to match `CDSChained` (`pVftable_*`, `nBbox_left/top`); prototyped `CDSBitmap_ctor@0x004393d0`; `save_program bulanci.exe`.

**Agent todo 26 r3 (2026-05-30):** `CDSBitmap_SubobjectCtor@0x004228f0` (was `CDSBitmap_ShellCtor`); `CMovieView_Constructor` / `CMovieView_InitTrackSequence` renamed; plate + disasm EOL on `InitTrackSequence` for `wViewFlags \|= 0x278` @ `bitmapBase+0x14` (`0x004237f5`); `CDSBitmap_ctor` prototype; `save_program bulanci.exe`.

**R4 todo 5 (2026-05-30):** `CheckedVirtualBaseCast(..., DAT_004b826c)` on heap `CDSBitmap` returns **`bitmap+0x1c`** for `CBulPicture` / `BlitDispatch` extent band — cast `+4/+8` = `nBbox_left` / `nBbox_top` @ `+0x20`/`+0x24`. Nominal `CBulPicture.pBitmap` is `CDSStaticDrawableFace *` (see `round4_task_05_report.md`).

**Agent todo 26 r4 (2026-05-30):** `dwChainField_08/0c` → `dwField_08/0c` (parity with [CDSChained.md](./CDSChained.md)); chain band `+0x30..+0x3c` documented as ctor-zero scratch shared with `CDSAnim`/`CDSChained` (no bitmap-specific readers); `InitTrackSequence` `this` merge completed in [round4_task_15_report.md](./round4_task_15_report.md); `save_program bulanci.exe`.

Factory subobject init (no `ODSImage_ctor`): `CDSBitmap::CDSBitmap_SubobjectCtor` @ `0x004228f0`.

| Extra evidence | Address | Note |
|----------------|---------|------|
| `wViewFlags \|= 0x278` | `CMovieView::InitTrackSequence` @ `0x00423719` | `*(ushort *)(bitmap + 0x14)` — Ghidra field `wViewFlags` @ `+0x14` |
| History opcode 47 alloc | `CHistoryScript::HhBuildHistoryBitmapWidget` @ `0x00423530` | `OperatorNew(0x78)` → `CDSBitmap_ctor` |

## UNK

- `dwField_30..3c`, `dwField_50`, `dwField_5c/60/64` — runtime semantics beyond `CDSChained_ctor` zero and `CDSChained_ResetChainCounters@0x0042beb0` (`+0x40..+0x50` band only).
- Whether standalone heap `CDSBitmap` and `CMovieView` embed share identical tail layout above `+0x68` (CMovieView adds resource pointers at `+0x78`).
