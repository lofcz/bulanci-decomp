# Struct recovery batch 32 — follow-up round 2

**Index:** 32 (`batches_50.json`)  
**Prior:** `batch_32_summary.md`  
**Date:** 2026-05-30  
**Status:** **COMPLETE**

## Follow-ups addressed

| Item | Result |
|------|--------|
| Name factory `0x0042af00` | **`CDSImageMouse_CreateObject`** — `create_function` @ `0x0042af00` (was undefined code); decompile shows `OperatorNewWithBadAlloc(0x88)` + `CDSImage_InitDefaults(this+0x18)` |
| Split embedded `CDSImage` @ `CDSImageMouse+0x18` | Ghidra field **`savedBackground`** typed **`CDSImage`** @ offset `0x18` (`get_struct_layout` size still **136**) |
| `CDSJpegImage` dword @ `+0x60` | **Real tail dword** on **100-byte** wrapper (`CDSJpegImage_CreateObject`); `InitVtables` stores **`0x4b`** @ `+0x60`. **Not padding** — distinct from standalone **`0x60`** image allocs (DSM/FLX decode targets). Documented as `classId_or_quality` in `CDSJpegImage.md` |

## Ghidra deltas

- `create_function` + name **`CDSImageMouse_CreateObject`** @ `0x0042af00`
- `modify_struct_field` **`CDSImageMouse.pEmbed_cdsObject`** → **`savedBackground` : `CDSImage`**
- `save_program bulanci.exe` (once)

## Remaining UNK

- `CDSImageMouse`: `pCursorSprite` resource type; inner `CDSImage` MI/slot details (`CDSImage.md`)
- ~~`CDSJpegImage` factory `0x0043c160` / `+0x60` dual role~~ **closed** (agent todo 36) — see `CDSJpegImage.md` factory table
- `CDSJpegImage`: whether pack deserialize overwrites `+0x60` after init

## Artifacts updated

- `CDSImageMouse.md`
- `CDSJpegImage.md`
