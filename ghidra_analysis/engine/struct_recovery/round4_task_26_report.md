# Round 4 — Task 26 Report

## Task

| Field | Value |
|-------|--------|
| **id** | 26 |
| **round** | 4 |
| **title** | CDSBitmap chain band +0x30..64 semantics; InitTrackSequence set_function_this_type |
| **prior** | [round3_task_26_report.md](./round3_task_26_report.md) |
| **structs** | `CDSBitmap`, `CDSBmpImage`, `CMovieView`, `CDSChained` |

## Status

**DONE** — BMP helpers retyped to `CDSImage *` (stride/BITMAPINFOHEADER decompile fixed); `CDSBitmap` chain header names aligned with `CDSChained`; `InitTrackSequence` `this` merge verified via R4 todo 15.

## Evidence

### CDSBmpImage — ValidateStride / FillBitmapInfoHeader ECX

| Claim | Address | Evidence |
|-------|---------|----------|
| Save passes pixel plane, not stream host | `CDSBmpImage_SaveDibStream@0x00432440` | `LEA EBX,[ESI-0x50]` @ `0x00432475`; `MOV ECX,EBX`; `CALL 0x00436020` @ `0x0043247d` |
| Strict stride check fields | `CDSBmpImage_ValidateStride@0x00436020` | `[ECX+4]` width, `[ECX+0xc]` format → `DAT_004b0050`, `[ECX+0x10]` stride |
| Loose path DWORD-align | `0x00436046` | `[ECX+0x10] & 3 == 0` → `nM_stride` (not format) |
| Decompile after R4 | `0x00436020` | `DAT_004b0050[this->nM_format]*this->nM_width+7>>3 == this->nM_stride` |
| FillBitmapInfoHeader | `0x00436060` | `nM_paletteEntries`, `nM_format` bpp table, `biWidth`/`biHeight` from `nM_width`/`nM_height` |

### CDSBitmap — chain band +0x30..+0x64

| Claim | Address | Evidence |
|-------|---------|----------|
| Shell ctor zeros band | `CDSBitmap_ctor@0x004393d0` | `CDSChained_ctor` → zeros `+0x08..+0x3c`, tail `+0x5c..+0x64` |
| Reset band only `+0x40..+0x50` | `CDSChained_ResetChainCounters@0x0042beb0` | Writes `dwChainRoot`, `wChainInit44..4a`, `pParent`, `dwField_50` |
| `+0x30..+0x3c` | — | No bitmap-specific non-ctor consumers; shared `CDSChained` scratch (same UNK as `CDSAnim`) |
| Field rename | struct | `dwChainField_08/0c` → `dwField_08/0c` |

### CMovieView — InitTrackSequence

| Claim | Address | Evidence |
|-------|---------|----------|
| `this` in ESI, not ECX | `CMovieView_InitTrackSequence@0x004237b0` | `set_function_this_type` dry-run: `__stdcall` — no ECX `this` |
| R4 todo 15 merge | `0x004237b0` | Register `this`; `this->bitmapBase.trackImage`, `this->bitmapBase.wViewFlags`, `this->pAudioSequence` |
| Ctor tail | `CMovieView_Constructor@0x004236a0` | `JMP 0x004237b0` @ `0x00423785`; calls `CMovieView_InitTrackSequence()` with no arg |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x00436020`, `0x00436060` | `CDSImage *`; functions in class `CDSImage` |
| `set_function_prototype` | `0x00436020`, `0x00436060` | `CDSImage *this` on ValidateStride / FillBitmapInfoHeader |
| `set_decompiler_comment` | `0x00436020` | ECX = pixel plane; strict/loose stride rules |
| `modify_struct_field` | `CDSBitmap` | `dwField_08`, `dwField_0c` |
| `force_decompile` | `0x00436020`, `0x00436060`, `0x00432440` | Correct `nM_*` field names on BMP helpers |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSBmpImage.md](./CDSBmpImage.md) — R4 ValidateStride apply; removed stale UNK
- [CDSBitmap.md](./CDSBitmap.md) — R4 chain-band note; narrowed UNK

## Remaining UNK

- `CDSBitmap` / `CDSChained` `dwField_30..3c`, `dwField_50`, `dwField_5c/60/64` — no runtime semantics beyond ctor zero / `ResetChainCounters` band.
- `CMovieView_InitTrackSequence` — formal `this` remains ESI register-local (`__stdcall` ctor tail); MCP cannot bind `esi:4` storage.
- Dedicated `CDSBmpImage` factory path (heap still `CDSObject_CtorWithImage` @ `0x60`).
