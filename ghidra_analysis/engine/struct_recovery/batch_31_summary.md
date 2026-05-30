# Struct recovery batch 31/50

**Index:** 31 (`batches_50.json`)  
**Types:** `CDSGZipStreamData`, `CDSImage`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CDSGZipStreamData` | VERIFIED | `0x20` | Gzip chunk index; `OperatorNew(0x20)` in `CDSGZipStream__Open` |
| `CDSImage` | PARTIAL | `0x60` | Bitmap core + 5 MI vtables; `+0x38` slot vector unnamed |

## Key evidence

- **CDSGZipStreamData alloc:** `OperatorNewWithBadAlloc(0x20)` @ `CDSGZipStream__Open` (`0x004356e0`); dtor frees `m_chunks` at `+0x1c` (`0x00434fc0`).
- **CDSImage size:** four embedded images in `CGunMouse_ctor` (`0x00426060`) spaced by `0x60`; tail field `m_chain` at `+0x5c`.
- **CDSImage fields:** `CDSImage__Allocate` (`0x00436f40`), `GetColorPlane` (`0x004360f0`), `CDSImage_ReleaseRefcount` (`0x004322f0`).

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CDSGZipStreamData.md`
- `ghidra_analysis/engine/struct_recovery/CDSImage.md`
- Ghidra: deleted 1-byte placeholders → `create_struct` (`CDSGZipStreamData` 32 B, `CDSImage` 96 B); `save_program bulanci.exe` ✓

## Follow-ups

- ~~Name `m_slotVector` at `+0x38`~~ — done (`CDSPtrSlotVec`, follow-up round 2).
- Wire `CDSImage` into derived decoders (`CDSBmpImage`, `CDSJpegImage`) — partial (`batch_31_followup_summary.md`).
- **Slice 31 re-agent (2026-05-30):** verified Ghidra layouts; `CDSGZipStreamData_dtor` prototype; `CDSImage__Allocate` rename + prototype; plate on `CDSGZipStream__Open`.

See also: `batch_31_followup_summary.md`.
