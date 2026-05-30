# Round 4 — Task 05 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **round** | 4 |
| **title** | Align CDSBitmap FLX face +4/+8 for CBulPicture extent reads |
| **one_liner** | `CBulPicture_ctor` disasm adds `originX/Y` to `[pBitmap+4]` / `[pBitmap+8]`; R3 typed `pBitmap` as `CDSBitmap *` so decompiler used `pVftable_IDSChained` / `dwChainField_08`. |
| **prior** | [round3_task_05_report.md](./round3_task_05_report.md) |
| **structs** | `CBulPicture`, `CDSBitmap`, `CDSFlxFile` |

## Status

**DONE** — `CDSStaticDrawableFace` models cast-pointer band; `CBulPicture.pBitmap` retyped; `CBulPicture_ctor` decompile uses `nBlitExtentW` / `nBlitExtentH`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Extent W from cast+4 | `0x0041028d` | `MOV EDX,[ESI+0x20]`; `ADD EDX,[EAX+0x4]` → `nExtentW` @ `+0x28` |
| Extent H from cast+8 | `0x00410287`–`0x00410290` | `MOV ECX,[EAX+0x8]`; `ADD ECX,[ESI+0x24]` → `nExtentH` @ `+0x2c` |
| Cast stores handle | `0x00410281` | `MOV [ESI+0x68],EAX` after `CheckedVirtualBaseCast(..., DAT_004b826c)` |
| BlitDispatch same +4/+8 band | `0x0043691d` | When `param_3==0`: `MOV ECX,[ESI+0x4]`; `MOV EAX,[ESI+0x8]` (source sub-rect size) |
| FLX absolute fields | `CDSFlxFile` layout | `nBitmapHeight` @ `+0x0c`, `nBitmapWidth` @ `+0x10` — seen at cast **`+8`** as `+4`/`+8` (`nChannels` @ cast+0) |
| CDSBitmap absolute fields | `CDSBitmap` layout | `nBbox_left` @ `+0x20`, `nBbox_top` @ `+0x24` — seen at cast **`+0x1c`** as `+4`/`+8` |
| Decompile after R4 | `0x004101d0` | `nExtentW = nOriginX + pBitmap->nBlitExtentW`; `nExtentH = nOriginY + pBitmap->nBlitExtentH` |

### Cast adjustor summary (static face `DAT_004b826c`)

| Resource | `CheckedVirtualBaseCast` result | `[ptr+4]` / `[ptr+8]` |
|----------|--------------------------------|------------------------|
| `CDSFlxFile` (ClassID 52) | `flx + 0x08` | `nBitmapHeight` / `nBitmapWidth` |
| Heap `CDSBitmap` | `bitmap + 0x1c` | `nBbox_left` / `nBbox_top` |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `create_struct` | `CDSStaticDrawableFace` (12 B) | `nFaceBand0` @0, `nBlitExtentW` @+4, `nBlitExtentH` @+8 |
| `modify_struct_field` | `CBulPicture` @ `+0x68` | `pBitmap` → `CDSStaticDrawableFace *` |
| `set_decompiler_comment` | `0x00410284` | Cast adjustor note |
| `set_disassembly_comment` | `0x0041028d`, `0x00410290` | Extent addends |
| `set_plate_comment` | `0x004101d0` | Drawable-face cast summary |
| `set_function_this_type` | `0x004101d0`, `0x0040b050` | Confirmed `CBulPicture *` |
| `force_decompile` | ctor / `DrawSurface` | Extent uses `nBlitExtentW/H` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CBulPicture.md](./CBulPicture.md) — R4 `pBitmap` type + cast adjustor table.
- [CDSBitmap.md](./CDSBitmap.md) — static-face cast `+0x1c` note for extent band.
- [CDSFlxFile.md](./CDSFlxFile.md) — `+0x08` cast base for extent band.

## Remaining UNK

- Prior-handle release @ `0x00410268` uses `[pBitmap]` as vtable (`MOV EDX,[ECX]`); cast pointers at `FLX+8` / `bitmap+0x1c` have non-vtable dwords at +0 — release path may use uncast pool object or union not modeled.
- `BlitDispatch` still types bitmap arg as `CPoemScroller *` (shared helper naming).
- Whether portrait resources are always one resource class in practice (FLX vs heap `CDSBitmap`) for a single adjustor rule.
