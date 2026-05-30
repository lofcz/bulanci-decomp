# Struct recovery batch 32/50

**Index:** 32 (`batches_50.json`)  
**Types:** `CDSImageMouse`, `CDSJpegImage`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CDSImageMouse` | VERIFIED size / PARTIAL layout | `0x88` | Factory @ `0x0042af00` (`classId 0x35`); one `CDSImage` save buffer at `+0x18`, dirty rect `+0x78..+0x84` |
| `CDSJpegImage` | PARTIAL | `0x64` wrapper / `0x60` image | **100-byte** factory @ `0x00432070` (`CDSJpegImage_CreateObject`); **0x60**-byte `CDSObject_CtorWithImage` targets are decode-only bitmaps; tail `+0x60` = default IJG quality `0x4b` (agent todo 36) |

## Key evidence

- **CDSImageMouse alloc:** `PUSH 0x88` @ `0x0042af02` before `OperatorNewWithBadAlloc` in unlabeled factory `0x0042af00` (`HandleClassRegister` class `0x35`).
- **CDSImageMouse use:** `CDSImageMouse_Draw@0x0042b9a0` / `Erase@0x0042ba90`; dtor destroys embedded image via `CDSObject_dtor@+0x18`.
- **CDSJpegImage alloc:** `OperatorNewWithBadAlloc(0x60)` in `CDSDsmFile::InitializeChildObject@0x00439af3` (and `CDSFlxFile::CreateBoundClone`).
- **CDSJpegImage identity:** `CDSJpegImage_GetTypeInfo` → `DAT_004b7d80`; engine class id **`0x15`** @ `CDSJpegImage_StaticClassRegister` (`0x0047cbf0`), factory **`0x00432070`**. **`0x0043c160`** = `CDSQueueStream_CreateObject` (class **`0x4b`**, unrelated).

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CDSImageMouse.md`
- `ghidra_analysis/engine/struct_recovery/CDSJpegImage.md`
- Ghidra: `create_struct` applied for both; `save_program bulanci.exe` at batch end.

## Follow-ups

- Name factory `0x0042af00` (`CDSImageMouse_CreateObject`) in Ghidra.
- Split embedded `CDSImage` at `CDSImageMouse+0x18` once `CDSImage` struct is verified.
- ~~Confirm `+0x60` on 100-byte wrapper~~ **done** (agent todo 36) — real dword, default quality `0x4b`; not registry class id.
