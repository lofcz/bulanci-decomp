# Round 3 — Task 15 report

## Task

| Field | Value |
|-------|--------|
| **id** | 15 |
| **title** | Decompile CMovieView InitTrackSequence audio bind path |
| **types** | `CMovieView`, `CDSAnimSequence`, `CDSBitmap`, `CMina` |
| **addresses** | `0x004237b0`, `0x004236a0`, `0x00422860`, `0x0041cb70` |
| **acceptance** | Update `CMovieView.md`; Ghidra mutation if evidence supports |

## Status

**DONE** — Audio bind path at `CMovieView_InitTrackSequence@0x004237b0` is disasm- and decompiler-proven. Ghidra `set_function_this_type` + `unaff_ESI` → `CMovieView *` yields typed `pAudioSequence` / `bitmapBase.trackImage` / `wViewFlags` access; `this` register remains `unaff_ESI` (ctor tail `JMP`, not `CALL`).

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Ctor tail enters bind | `CMovieView_Constructor@0x004236a0` | `JMP 0x004237b0` @ `0x00423785` (shared stack / SEH epilogue) |
| Primary audio load | `0x0042371d` | `LoadResource` vfn `+0x10` with `[EBP+0x14]` = `audioResId` → `CheckedVirtualBaseCast(DAT_004b826c)` → `[ESI+0x7c]` |
| Fallback when `pAudioSequence==0` | `CMovieView_InitTrackSequence@0x004237b0` | `[EBP+0x10]` = ctor `param_3` (movieId); same stream mgr (`EDI` = `[g_pApp+0x70]`, saved @ ctor `[EBP+0xc]`) |
| Release prior handle | `0x004237ce` / ctor `0x00423741` | `pAudioSequence->pVf_primary` vfn `+8` before replace |
| Bind drawable | `0x004237ed`–`0x004237f0` | `ODSImage__SetImage(ESI+0x68, [ESI+0x7c])` — `bitmapBase.trackImage` ← `(CDSImage*)pAudioSequence` |
| View flags | `0x004237f5` | `OR word [ESI+0x14], 0x278` — embedded `CDSBitmap.wViewFlags` |
| Shared epilogue | `0x0042380e` | `RET 0x14` (5 stack args + cleanup) |
| Runtime re-bind | `CMovieView::OnEvent@0x00422860` | event `1`: same `ODSImage__SetImage(&trackImage, pAudioSequence)` before player teardown |
| Script entry | `CHistoryScript` handler @ `0x00421af0` area | `HBuildHistoryMovieWidget` → `Constructor(self,x,y,movieId,extra1,extra2)` |
| Related sizing anchor | `CMina_Ctor@0x0041cb70` | batch-14 handoff type only (not on bind path) |

### Bind flow (ordered)

1. **Constructor** (`0x004236a0`): `CDSBitmap_ctor`; zero `pVideoResource` / `pAudioSequence` / `pPlayer`; load `audioResId` → `pAudioSequence@+0x7c`; load `videoResId` → `pVideoResource@+0x78`; `JMP InitTrackSequence`.
2. **InitTrackSequence** (`0x004237b0`): if `pAudioSequence==0`, retry `LoadResource(movieId)` + cast; always `ODSImage__SetImage(&bitmapBase.trackImage@+0x68, pAudioSequence)`; `bitmapBase.wViewFlags |= 0x278`.
3. **OnEvent** (`0x00422860`): event `1` repeats `SetImage` on stop path (player stop + parent event).

`DAT_004b826c` is the `CheckedVirtualBaseCast` class id shared with static drawable faces (`CDSFlxFile` / `CBulPicture`); here the stored pointer is typed **`CDSAnimSequence *`** at `+0x7c` and consumed as **`CDSImage *`** for `ODSImage`.

## Ghidra deltas

- `set_function_this_type` `CMovieView *` @ `CMovieView_InitTrackSequence@0x004237b0`, `CMovieView_Constructor@0x004236a0`.
- `set_decompiler_variable_type` `unaff_ESI` → `CMovieView *` @ `0x004237b0` (typed field refs in decompile).
- Existing: plate / EOL comments on bind; `CDSAnimSequence *` on `pAudioSequence`; `CDSAudioVideoPlayer *` on `pPlayer`.
- `save_program bulanci.exe`.

## Struct doc updates

- [CMovieView.md](./CMovieView.md) — R3 todo 15 bind path; decompiler status after `unaff_ESI` typing.

## Remaining UNK

- Decompiler still names **`unaff_ESI`** instead of merging with `this` (tail shares ctor frame; `this` lives in **ESI**, not ECX).
- `unaff_EDI` / `unaff_EBP` in `InitTrackSequence` (stream mgr scratch + ctor SEH frame) — not retyped.
- Whether fallback `movieId` load should be a named prototype parameter on a merged ctor+tail function.
