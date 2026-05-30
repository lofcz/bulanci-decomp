# Struct recovery — batch 04 follow-up (round 2)

**Program:** `bulanci.exe`  
**Prior:** `batch_04_summary.md`  
**Status:** `FOLLOWUP_PARTIAL`  
**Saved:** yes (`save_all_programs` — one save for Ghidra mutations)

## Actions taken

| Follow-up (batch 04) | Result |
|----------------------|--------|
| Name `CBulAnim` `trackManager` sub-fields | **Done** — Ghidra embed **`CDSVideoPlayer` @ `+0x88`** (72 B) on `CBulAnim` and **`CAnim.track_manager` @ `+0xa8`** (agent todo 08, 2026-05-30); nested `CDSUpdatedItem scheduler` @ embed `+0x4`. Docs: `CDSVideoPlayer.md`, `ConstructTrackManager@0x00439c70`. |
| Split `CBulPicture+0x30` blit consumer | **Resolved (reclassified)** — not a separate FLX consumer. `+0x30..+0x3C` are CDSView **render rect** (`BlitDispatch` `param_1[0..3]` @ `DrawSurface@0x0040b050` / `BlitDispatch@0x004368d0`). FLX consumer fields remain on `pBitmap` (`param_2`). |
| Recover `CDSView`/`CDSChained` prefix | **Deferred** — `CDSView` excluded from `batches_50.json`; only corroborated `+0x30..+0x3C` render rect via widget catalog + blit path (no new Ghidra base type). |

## Ghidra deltas

### `CBulAnim` (212 bytes — unchanged total)

- `trackManager`: `byte[68]` → `byte[72]` at `+0x88`, renamed from `pTrackManager`.
- Removed erroneous `pPad_cc`; `bTeamId` remains at `+0xD0` (`208`).
- **Todo 08:** `trackManager` → **`CDSVideoPlayer` (72 B)** @ `+0x88`; `CAnim.pTrack_manager` → **`track_manager` `CDSVideoPlayer` @ `+0xa8`**.

### `CBulPicture` (1136 bytes — unchanged total)

- Replaced `pBlitConsumer` `byte[56]` with `nRenderLeft` / `nRenderTop` / `nRenderRight` / `nRenderBottom` at `+0x30..+0x3C`, `pPad_40` `byte[40]` at `+0x40..+0x67`.
- `pBitmap` / `palette` / tail offsets unchanged (`+0x68`, `+0x6C`, `+0x46C`).

## Deliverables updated

- `CBulAnim.md`
- `CBulPicture.md`

## Remaining UNK

- `CBulAnim` / `CBulPicture` `+0x08..+0x67` (and picture `+0x40..+0x67` tail): full `CDSView` layout — blocked on a dedicated `CDSView` recovery batch.
- `CBulAnim` / `CAnim` `trackManager`: optional `CDSTrackVector` nested type at `+0x1c` inside embed (todo 44).
- `CBulPicture` `pBitmap` exact type (`CDSBitmap*` vs `void *`) until `CDSBitmap` / cast id `DAT_004b826c` is sized in struct recovery.
