# Struct recovery batch 31/50 — follow-up round 2

**Index:** 31  
**Prior:** `batch_31_summary.md`  
**Date:** 2026-05-30  
**Status:** **FOLLOWUP_PARTIAL**

## Actions taken

| Follow-up (from batch 31) | Result |
|---------------------------|--------|
| Name `m_slotVector` / re-tag resize helper `0x00406340` | **Done** — new type `CDSPtrSlotVec` (8 B); function renamed `CDSPtrSlotVec_Resize`; `CDSImage.m_slotVector` field typed `CDSPtrSlotVec` |
| Wire `CDSImage` into `CDSBmpImage` / `CDSJpegImage` Load/Save | **Done** (agent todo 35) — `CDSBmpImage` `{pVf_primary; m_image @+4}`; Load/Save decompile uses `this[-1].m_image.*`; `CompressFromImage` takes `CDSImage *`; `save_program` ✓ |
| `CDSGZipStreamData` | No prior follow-ups — unchanged |

## Ghidra deltas

- **Created:** `CDSPtrSlotVec` (8 bytes)
- **Renamed:** `CDSAudioBank_ResizeSlots` → `CDSPtrSlotVec_Resize` @ `0x00406340` (prototype `void __thiscall CDSPtrSlotVec_Resize(CDSPtrSlotVec *, int)`)
- **Modified:** `CDSImage.m_slotVector` `byte[8]` → `CDSPtrSlotVec`
- **Recreated:** `CDSBmpImage` (was 1-byte placeholder) → `pVf_IDSReferenced` + `m_image` (`CDSImage`)
- **Prototype:** `CDSImage_dtor@0x004254f0` → `void __fastcall CDSImage_dtor(CDSImage *this)` — decompiler now uses `this->m_slotVector`, `this->pM_pixels`, etc.
- **Plate:** `CDSBmpImage_Load@0x004320c0` — stream-host `this` vs pixel `CDSImage` adjustment
- **Saved:** `save_program bulanci.exe` ✓

## Decompiler spot-check

- `CDSImage_dtor`: field-aware teardown via `this->m_slotVector` / `CDSPtrSlotVec_Resize` (call site still prefixed `CDSAudioBank::` — stale Ghidra namespace on callee, not wrong offset)
- `CDSBmpImage_Load` / `CDSJpegImage_Load`: assembly unchanged (`EDI/ECX-0x50`); pseudocode may show `this[-1].m_image.*` when `this` is the `+0x54` vtable slot — documented in `CDSImage.md` UNK

## Artifacts

- `CDSPtrSlotVec.md` (new)
- Updated: `CDSImage.md`, `CDSAudioBank.md`

## Remaining UNK

- `CDSPtrSlotVec_Resize` callee namespace (`CDSAudioBank::` prefix in decompiler display only)
- `CDSBmpImage` / `CDSJpegImage` Ghidra struct size 100 vs heap `OperatorNew(0x60)` — intentional view; BMP/JPEG Load/Save decompile via `m_image` + `this-0x50` asm
- `CDSImage` `field_44` / `field_48` constant `8`; full MI thunk graph
- `CDSGZipStreamData` `+0x0c..+0x0f` padding (carried from batch 31)
