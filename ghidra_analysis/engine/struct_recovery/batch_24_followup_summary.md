# Struct recovery batch 24 — follow-up round 2

**Status:** `HANDOFF_CLEAN`  
**Prior:** [batch_24_summary.md](./batch_24_summary.md)  
**Deliverables reviewed:** [CDSAudioVideoPlayer.md](./CDSAudioVideoPlayer.md), [CDSBackBuffer.md](./CDSBackBuffer.md)

## Prior batch follow-ups

| Item | Source |
|------|--------|
| Map embedded track manager `+0x08` as nested type | `batch_24_summary.md` — after `CDSVideoPlayer` / `CDSUpdatedItem` batches |
| Expand `CDSBackBuffer+0x04` using verified `CDSImage` layout | `batch_24_summary.md` — after `CDSImage` batch (31) |

Dependency batches landed: `CDSImage` (batch 31), `CDSVideoPlayer` / `CDSUpdatedItem` (batches 41–42).

## Actions taken

| Action | Result |
|--------|--------|
| Re-read deliverables + `CDSImage.md`, `CDSVideoPlayer.md`, `CDSUpdatedItem.md` | Confirmed embed spans: AV player `+0x08..+0x4f` = 72 B `CDSVideoPlayer`; back-buffer image `+0x04..+0x4f` = 76 B `CDSImage` prefix |
| Decompile consumers | `Constructor@0x0043bd00` → `ConstructTrackManager(param_1+2)`; `CDSBackBuffer_Flip@0x00429930` → `ClearPreFlipFields(param_1+4)`, surface at `+0x4c`; `CDSImage_dtor(param_1+1)` @ `0x0042ad40` |
| Recreate `CDSAudioVideoPlayer` in Ghidra | Deleted corrupted 10 B layout; `create_struct` → 80 B with `videoTrackManager` (`CDSVideoPlayer`) @ `+0x08` |
| Expand `CDSBackBuffer` image body | New `CDSImage_BackBufferEmbed` (76 B, `CDSImage` fields through `pDirectDrawSurface` @ embed `+0x48`); `CDSBackBuffer` = `vftable_IDSReferenced` + `embeddedImage` |
| Update struct deliverables | `CDSAudioVideoPlayer.md`, `CDSBackBuffer.md` layout/Ghidra/UNK sections |
| `save_program bulanci.exe` | ✓ (single save) |

## Ghidra deltas

| Type | Before | After |
|------|--------|-------|
| `CDSAudioVideoPlayer` | 80 B flat tail fields (`trackSlots`, `nTrackCount`, …) | 80 B: `vftable_IDSEventHandler`, `pAudioPlayer`, `videoTrackManager` (`CDSVideoPlayer` 72 B @ `+0x08`) |
| `CDSImage_BackBufferEmbed` | — | **New** 76 B — `CDSImage` core through `pDirectDrawSurface` (repurposed `nField_48` slot) |
| `CDSBackBuffer` | 80 B sparse (`nFlipPending`, `pPad_mid`, lone `pDirectDrawSurface`) | 80 B: `vftable_IDSReferenced` @ `0`, nested `embeddedImage` |

## Remaining UNK (documented only)

- `CDSImage_BackBufferEmbed` omits tail MI vtables (`vf_IDSChained` … `m_chain`) — parent is only 80 B; not heap `CDSImage` (96 B).
- `SetupTrack` byte at parent `+0x3d` aligns with `CDSVideoPlayer::bPaused` (`+0x35` in embed); “trackReady” naming vs play-state flags.
- `CDSBackBuffer::embeddedImage::nField_44` (`CDSApp_ctor` writes `8` @ `CDSApp+0xc4`) — same offset as batch-24 `defaultBpp`; consumer beyond ctor not re-proven this round.
- `CDSImage` slot vector / `field_40` semantics unchanged from batch 31.

## Handoff

Batch 24 follow-up queue is cleared. Optional later: rename `pVf_primary` ↔ `vftable_IDSEventHandler` in decompiler comments for MI clarity; apply `CDSImage_BackBufferEmbed` pattern to other truncated embeds if discovered.
