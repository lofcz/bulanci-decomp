# Struct recovery follow-up — batch 14/50

**Prior:** [batch_14_summary.md](./batch_14_summary.md) (`CMina`, `CMovieView`)  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_COMPLETE** (explicit batch-14 follow-ups addressed; residual UNK documented)

## Actions taken

| Follow-up (batch 14) | Action | Result |
|----------------------|--------|--------|
| Replace `animBase` blob in `CMina` | Recreated `CMina` with `CAnim_recovered@0`, `CDSUpdatedItem@0xf0` | `get_struct_layout CMina` → 280 B, typed subobjects |
| Replace `bitmapBase` / `trackImage` blobs in `CMovieView` | Created `CDSBitmap` (120 B / `0x78`) with `ODSImage@0x68`; recreated `CMovieView` with `CDSBitmap@0` + tail pointers | `get_struct_layout CMovieView` → 132 B |
| Rename remaining `CMovieView` factory symbols | `FUN_004228f0` → `CDSBitmap::CDSBitmap_SubobjectCtor` (used by `CMovieView_CreateObject` before full ctor) | Other `CMovieView::*` methods already named per `movie_cinema_views.md` |

## Ghidra deltas

- [x] `delete_data_type` / `create_struct` **`CDSBitmap`** — 11 fields, 120 bytes; `trackImage` typed as `ODSImage`
- [x] Recreated **`CMina`** — nested `CAnim_recovered` + `CDSUpdatedItem`
- [x] Recreated **`CMovieView`** — nested `CDSBitmap` + `m_pVideoResource` / `m_pAudioSequence` / `m_pPlayer`
- [x] `rename_function_by_address` **`0x004228f0`** → `CDSBitmap::CDSBitmap_SubobjectCtor`
- [x] `save_program bulanci.exe` (once)

## Deliverables updated

- [CMina.md](./CMina.md) — Ghidra apply + layout types
- [CMovieView.md](./CMovieView.md) — layout reflects embedded `CDSBitmap` / `ODSImage`
- [CDSBitmap.md](./CDSBitmap.md) — **new** PARTIAL struct note

## Remaining UNK

- `CAnim_recovered` gaps `+0x28..+0x67`, `+0xac..+0xef` (batch 3 scope; not expanded here).
- `CDSBitmap` opaque regions `viewHeader_28`, `pad_50`; `CDSChained` type still absent in Ghidra.
- `CMina`: `pAux` / `deployOrOwnerCtx` semantics.
- ~~`CAnim_recovered` → `CAnim` on `CMina.animBase`~~ — **done** (agent todo 20 / round-3 task 47); `get_struct_layout CMina` → `CAnim animBase` @ 0.
- ~~`gaming_host` @ `CAnim+0x84`~~ — **done** (agent todo 15): `CGaming *`; `CMina_UpdateTraceAreas@0x00419fd0`; `CBitmap.gaming_host` for `CGameView_Update`.
- `m_pAudioSequence` concrete type (`CDSAnimSequence *` vs `void *`) pending RTTI target struct.

## Evidence anchors (unchanged)

- `CMina` size `0x118` @ `CWeapon::Fire` / `CMina_Ctor@0x0041cb70`
- `CMovieView` size `0x84` @ `CMovieView_CreateObject@0x00422e30`
- `CDSBitmap` size `0x78` @ `OperatorNewWithBadAlloc(0x78)` + `CDSBitmap_ctor@0x004393d0`
