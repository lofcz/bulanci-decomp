# Struct recovery batch 25/50 — follow-up round 2

**Status:** `FOLLOWUP_DONE`  
**Prior:** `batch_25_summary.md`  
**Structs:** `CDSImageMouse`, `CDSJpegImage`

## Actions taken

| Follow-up (batch 25) | Result |
|----------------------|--------|
| Name factory `0x0042af00` | Already **`CDSImageMouse_CreateObject`** (`__stdcall`, `PUSH 0x88` → `OperatorNewWithBadAlloc`). Verified decompilation. |
| Name factory for `CDSJpegImage` | Created **`CDSJpegImage_CreateObject`** @ **`0x00432070`** (`PUSH 0x64` = **100** bytes → `CDSJpegImage_InitVtables`). Prior summary address **`0x00432060`** was inside **`CDSJpegImage_Save`** tail, not the factory. |
| Replace `embed_cdsObject` byte blob | Rebuilt **`CDSImageMouse`**: `pEmbed_cdsObject[96]` → **`CDSImage savedBackground`** @ `+0x18` (struct size still **136**). |
| Split `m_jpegQuality` from `m_format` | **No separate field.** `CDSJpegImage_Save` (`0x00432030`) with `ECX` on IDSImage face loads **`[object+0x60]`** (class id **`0x4b` = 75**) as `jpeg_set_quality` argument; `CompressFromImage` still reads pixel format from embedded image **`+0xc`** (`m_format`). Documented in `CDSJpegImage.md`. |

## Ghidra deltas

- `create_function` → `CDSJpegImage_CreateObject` @ `0x00432070`
- `delete_data_type` / `create_struct` → `CDSImageMouse` with nested `CDSImage`
- `save_program bulanci.exe`

## Remaining UNK

- `CDSImageMouse`: cursor sprite type; full inner `CDSImage` field use beyond Draw/Erase blit at `+0x1c`.
- `CDSJpegImage`: registry slot @ `0x0043c160` is **`0x24`**-byte factory (not this type); `+0x60` dual role (class id constant vs default JPEG quality) — do not rename to `m_jpegQuality` without new xrefs.
- `CDSObject` / `CDSImage` inner layouts unchanged this batch.

## Doc updates

- `CDSImageMouse.md` — Ghidra apply section
- `CDSJpegImage.md` — factory address, quality/class-id `+0x60` note
