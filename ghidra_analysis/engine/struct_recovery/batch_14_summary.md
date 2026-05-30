# Struct recovery batch 14/50

**Structs:** `CMina`, `CMovieView`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNewWithBadAlloc` / ctor / dtor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CMina` | PARTIAL | `0x118` | `CAnim` @0, `CDSUpdatedItem` @0xf0 (24B span), tail `pOwnerBulanek`/`pAux`/`deployOrOwnerCtx`/`bArmed` |
| `CMovieView` | PARTIAL | `0x84` | `CDSBitmap` prefix, `trackImage` @0x68, `m_pVideoResource`/`m_pAudioSequence`/`m_pPlayer` |

## Key evidence anchors

- `CWeapon::Fire` @ `0x004212b0` — `OperatorNewWithBadAlloc(0x118)` → `CMina::CMina_Ctor`.
- `CMina_Ctor` @ `0x0041cb70` — `CAnim_SubobjectCtor`, `CDSUpdatedItem_ctor(this+0xf0)`, writes `+0x108..+0x114`.
- `_Globals__ExplodeMine` @ `0x0041e070` — `bArmed` at dword index `0x45` (`+0x114`), owner at `0x42` (`+0x108`).
- `CMovieView_CreateObject` @ `0x00422e30` — `OperatorNewWithBadAlloc(0x84)`.
- `CMovieView::Constructor` @ `0x004236a0` — `CDSBitmap_ctor`, resources at `+0x78..+0x80`.
- `CMovieView::StartPlayback` @ `0x00422d50` — allocates `CDSAudioVideoPlayer` (`0x50`), stores at `+0x80`.

## Ghidra actions

- [x] Deleted placeholder `CMina` / `CMovieView` (size 1)
- [x] `create_struct` `CMina` (280 bytes / `0x118`)
- [x] `create_struct` `CMovieView` (132 bytes / `0x84`)
- [x] `get_struct_layout` verified size > 1 for both
- [x] `save_program bulanci.exe`

## Follow-ups

- Recover full `CAnim` layout (batch 3: `CAnim`, `CBitmap`) to replace `animBase` blob in `CMina`.
- Recover `CDSBitmap` / `ODSImage` to replace `bitmapBase` / `trackImage` blobs in `CMovieView`.
- Rename remaining `CMovieView::*` factory-only symbols (`CreateObject` exists; other methods may still be `FUN_*`).
