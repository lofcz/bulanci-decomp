# Struct recovery — batch 4/50

**Program:** `bulanci.exe`  
**Batch index:** 4 (`batches_50.json` → `CBulAnim`, `CBulPicture`)  
**Saved:** yes (`save_program bulanci.exe` — slice 04 agent 2026-05-30)

### Slice 04 agent deltas (2026-05-30)

- **CBulAnim** Ghidra size fixed **193 → 212** (`0xD4`): `trackManager` 72 B @ `+0x88`, `teamId` @ `+0xD0`; `pVbase_ptr` @ `+0x7C`.
- Renames: `CBulAnim_OnTeamPaletteEvt`, `CBulAnim_ApplyTeamPalette`, `CBulAnim_PrimaryTick_thunk`, `CBulPicture_DrawSurface`.
- **CBulPicture** struct unchanged (1136 B); `CBulPicture_ctor` plate comment + prototype string.

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CBulAnim` | PARTIAL | `0xD4` (212) | Applied — was 1-byte stub |
| `CBulPicture` | PARTIAL | `0x470` (1136) | Applied — was 1-byte stub |

## Evidence highlights

### CBulAnim

- Factory `CreateCBulAnim@0x0040eaa0` allocates `0xD4`, calls `CDSAnim::DefaultCtor`, patches seven vtable pointers (`0x480a14` … `0x480988`).
- Gameplay: `PrimaryTick_thunk` → `TM_TickBlit(this+0x68)`; `PrimaryRender_thunk` → `TM_RenderFrame(this+0x78)`; `teamId` at `+0xD0` for palette event `0xD1`.
- Teardown: `CleanupBody` destroys track manager at `(this+0x78)+0x10`, then `CDSAnim_CleanupBody`.

### CBulPicture

- Factory `CBulPicture_Create@0x0040eb30` allocates `0x470`, inits palette `+0x6C..+0x46B` to `0xFFFFFFFF`.
- Panel ctor `CBulPicture_ctor@0x004101d0` binds bitmap at `+0x68`, layout at `+0x20..+0x2C`, team/chroma bytes at `+0x46C/+0x46D`.
- Render: `DrawSurface@0x0040b050` blits via `this+0x30` / `*(this+0x68)` with optional chroma from `+0x46D`.

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CBulAnim.md`
- `ghidra_analysis/engine/struct_recovery/CBulPicture.md`

## Follow-ups (not in scope)

- Recover CDSView/CDSChained prefix shared with other views (batch dependency: `CDSView` excluded in manifest).
- Split `CBulPicture+0x30` blit consumer into its own struct (documented separately in `formats/sprite_container.md`).
- Name `CBulAnim` `trackManager` sub-fields (`+0x88..`) from `ConstructTrackManager` / `CDSVideoPlayer_TM_Destructor` xrefs.
