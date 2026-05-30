# CMovieView

## Status

**PARTIAL** — allocation size `0x84` verified; Ghidra nests full `CDSBitmap` (`0x78`, includes `ODSImage` @ `bitmapBase.trackImage`) plus resource/player pointers at `0x78–0x80`. `CDSBitmap` header interior `+0x28..+0x67` still opaque.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CMovieView) == 0x84` | `0x00422e30` | `CMovieView_CreateObject` → `OperatorNewWithBadAlloc(0x84)` |
| Frees same object | `0x00422ee0` | `CMovieView::ScalarDeletingDtor` → `_free(this)` when `param_1 & 1` |
| Last field at `+0x80` | `0x004236a0` | ctor `*(this+0x80)=0`; `StartPlayback` stores `CDSAudioVideoPlayer*` at `+0x80` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 0x78 | `CDSBitmap` | `bitmapBase` | `CDSBitmap::CDSBitmap_ctor((CDSBitmap *)this,...)` @ `Constructor@0x004236a0`; see [CDSBitmap.md](./CDSBitmap.md) |
| 0x68 | 0x10 | `ODSImage` | `bitmapBase.trackImage` | embedded in `CDSBitmap`; `ODSImage__SetImage(this+0x68,...)` @ `InitTrackSequence` / `StartPlayback` |
| 0x78 | 4 | `void *` | `pVideoResource` | zero + assign @ `Constructor@0x004236a0`; released `param_1[0x1e]` @ `Cleanup@0x00422cb0` |
| 0x7C | 4 | `CDSAnimSequence *` | `pAudioSequence` | RTTI cast `DAT_004b826c` @ `Constructor@0x004236a0`; `OnEvent` @ `0x00422860`; `InitTrackSequence@0x004237b0` |
| 0x80 | 4 | `CDSAudioVideoPlayer *` | `pPlayer` | zero ctor; `OperatorNewWithBadAlloc(0x50)` + `CDSAudioVideoPlayer::Constructor` @ `StartPlayback@0x00422d50`; cleared @ `OnEvent@0x00422860` |

## Ghidra apply

Slice **14** (2026-05-30): `get_struct_layout CMovieView` → **132 B (`0x84`)** — `CDSBitmap bitmapBase` @ 0, tail `pVideoResource` / `pAudioSequence` / `pPlayer` (`CDSAudioVideoPlayer *` @ +0x80). Recreated struct after `pM_*` auto-prefix collision on `m_*` field names. `save_program bulanci.exe`.

Round-2 todo **15** (2026-05-30): created **`CDSAnimSequence`** (24 B — `vf_primary`, `width`, `height`, `nDurationMs`, `nFrameCount` per `anim_runtime.md`); `CMovieView.pAudioSequence` → `CDSAnimSequence *` @ +0x7c; prototypes on `Constructor@0x004236a0`, `InitTrackSequence@0x004237b0`, `OnEvent@0x00422860`; decompiler shows typed `this->pAudioSequence` in ctor/OnEvent. `save_program bulanci.exe`.

Round-3 todo **15** (2026-05-30): **InitTrackSequence audio bind path** disasm-proven @ `0x004237b0` (ctor tail via `JMP` @ `0x00423785`, not `CALL`). Flow: (1) if `pAudioSequence@+0x7c==0`, `g_pApp+0x70` stream mgr `LoadResource(param_3@EBP+0x10,0)` → `CheckedVirtualBaseCast(DAT_004b826c)` → `CDSAnimSequence*`; (2) always `ODSImage__SetImage(&bitmapBase.trackImage@+0x68, (CDSImage*)pAudioSequence)`; (3) `bitmapBase.wViewFlags@+0x14 \|= 0x278`. Primary audio id load remains in `Constructor@0x0042371d` (`audioResId`). `pPlayer` typed `CDSAudioVideoPlayer *` @ +0x80; plate + decompiler comments @ bind sites. Separate `InitTrackSequence` function still shows `unaff_ESI` (this in ESI on tail entry).

Round-3 todo **26** (2026-05-30): `CDSBitmap` shell parity with [CDSBitmap.md](./CDSBitmap.md) — `CMovieView_Constructor` / `CMovieView_InitTrackSequence` renamed; EOL disasm @ `0x004237f5` documents `wViewFlags \|= 0x278` on embedded bitmap `+0x14`. See [round3_task_26_report.md](./round3_task_26_report.md).

Round-4 todo **15** (2026-05-30): merged **`unaff_ESI` → `this`** in `CMovieView_InitTrackSequence@0x004237b0` — prototype `void __stdcall …(void)` (ESI register local, not ECX); body uses `this->pAudioSequence`, `this->bitmapBase.trackImage`, `this->bitmapBase.wViewFlags`. Renamed ctor-frame scratch: `pStreamMgr` (EDI), `ctorFrame` (EBP), `pvLoad` (LoadResource temp). Constructor tail: `CMovieView_InitTrackSequence()` after JMP @ `0x00423785` (no bogus `this_00` arg). See [round4_task_15_report.md](./round4_task_15_report.md).

## UNK

- Exact `CDSBitmap` / `CDSChained` field names in `viewHeader_28` / `pad_50` (see [CDSBitmap.md](./CDSBitmap.md)).
- ~~Whether `pAudioSequence` is `CDSAnimSequence *` vs generic COM pointer~~ — **done** (R2 todo 15): typed `CDSAnimSequence *`; cast `CheckedVirtualBaseCast(..., DAT_004b826c)`.
- ~~`InitTrackSequence@0x004237b0` decompilation incomplete~~ — **done** (R3 bind path + R4 todo 15): `CMovieView *this` in body; ctor tail `JMP` still a separate function with `pStreamMgr`/`ctorFrame` scratch locals.
