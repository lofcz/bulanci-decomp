# Round 6 logic — task 37 report

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 37 |
| **title** | Logic sim_429_436: 0x00435d00–0x00436270 (22 funcs) |
| **range** | `sim_429_436` |
| **seed_address** | — (slice) |

Addresses: `0x00435d00` … `0x00436270` (22 functions). Manifest names from `agent_todos_50_r6_logic.json`.

## 2. Status

**DONE** — Ghidra decompile + xref proof for the pixel sample/write helper cluster and CPoemScroller palette blit path; two `FUN_*` renames applied; one callee left UNK.

## 3. Functions

| Addr | Name | Role summary | Evidence |
|------|------|--------------|----------|
| `0x00435d00` | `SampleSourcePixel_Indexed1` | Read 1 bpp source bit: `base + y*stride + (x>>3)`, shift by `7-(x&7)` | Decompile; DATA `PTR_SampleSourcePixel_Indexed1_004b0070` |
| `0x00435d30` | `SampleSourcePixel_Indexed2` | 2 bpp nibble extract from packed row | Decompile |
| `0x00435d70` | `SampleSourcePixel_Indexed4` | 4 bpp nibble extract from packed row | Decompile |
| `0x00435db0` | `SampleSourcePixel_Indexed8` | 8 bpp direct byte `*(base + y*stride + x)` (renamed from `FUN_00435db0`) | Decompile; DATA table slot `0x004b0088` family |
| `0x00435dd0` | `Blit_ReadDstPixel_16bpp` | Load `uint16` at `base + y*stride + x*2` | Decompile; `PTR_LAB_004b0074` index 7 |
| `0x00435df0` | `CDSImage_GetPixel24` | Load 24 bpp BGR as `uint24` at `x*3` | Decompile |
| `0x00435e20` | `Blit_ReadDstPixel_32bpp` | Load `uint32` at `x*4` (renamed from `FUN_00435e20`; pairs with write @ `0x00435fb0`) | Decompile |
| `0x00435f30` | `Blit_WriteDstPixel_8bpp` | Store index byte at `(x,y)` on dest plane | Decompile; `PTR_LAB_004b0074` |
| `0x00435f50` | `Blit_WriteDstPixel_16bpp` | Store `uint16` RGB565 at dest | Decompile |
| `0x00435f80` | `CDSImage_PutPixel24` | Store 24 bpp BGR (low 16 + high byte) | Decompile |
| `0x00435fb0` | `Blit_WriteDstPixel_32bpp` | Store `uint32` at dest | Decompile |
| `0x00435fe0` | `ComputeBufferSize` | `stride*height` (+ `paletteEntries*4` when indexed & non-strict) | Decompile; xrefs `CDSImage_Load/Save`, `ComputeAllocationSize` |
| `0x00436020` | `CDSBmpImage_ValidateStride` | Strict: `DAT_004b0050[format]*width+7>>3 == stride`; loose: `stride & 3 == 0` | Decompile; xref `CDSBmpImage_SaveDibStream@0x0043247d` |
| `0x00436060` | `CDSBmpImage_FillBitmapInfoHeader` | Fill `BITMAPINFOHEADER` from `CDSImage` fields + bpp table | Decompile; `bmp_decoder.md` |
| `0x004360d0` | `GetPaletteBuffer` | Return `m_pixels` if `1 <= m_paletteEntries <= 0x100`, else NULL | Decompile; FLX/BMP consumers (`sprite_container.md`) |
| `0x004360f0` | `GetColorPlane` | Return `m_pixels + paletteEntries*4` when palette present | Decompile |
| `0x00436110` | `CPoemScroller_GetPaletteEntry` | `*(GetPaletteBuffer() + index*4)` | Decompile |
| `0x00436130` | `CPoemScroller_SampleSourcePixel` | Dispatch source sample via `PTR_SampleSourcePixel_Indexed1_004b0070[nField_0c*2]` on `GetColorPlane(this)` | Decompile |
| `0x00436160` | `FUN_00436160` | Dest write via `PTR_LAB_004b0074[format*2]` (5-arg); xref `BlitOpaqueFallback@0x00436847` only | Decompile + xref — **semantic name UNK** |
| `0x004361a0` | `CPoemScroller_OpaqueBlitPixel_Indexed1` | Sample 1 bpp → palette dword or raw index; xref `BlitOpaqueFallback@0x00436883` | Decompile + xref |
| `0x00436210` | `CPoemScroller_FindNearestPaletteIndex` | L1 RGB distance vs palette (`ColorDistanceL1`); pick min entry | Decompile; called from `BlitPixelViaFormatTable` |
| `0x00436270` | `CPoemScroller_BlitPixelViaFormatTable` | If palette size in `(0,0x100)`, nearest-index path else direct `PTR_LAB_004b0074` write; xref `BlitOpaqueFallback@0x0043689e` | Decompile + xref |

### Dispatch tables (data)

| Addr | Symbol | Use |
|------|--------|-----|
| `0x004b0070` | `PTR_SampleSourcePixel_Indexed1_004b0070` | Source-format sample thunks (indexed 1/2/4/8, 16/24/32 readers) |
| `0x004b0074` | `PTR_LAB_004b0074` | Dest-format read/write thunks used by poem scroller blit + opaque fallback |

Indexed by `nField_0c` (source/dest **format index** on embedded `CDSImage` / scroller plane), stride `+0x10` on consumer — consistent with `CDSImage.m_format` / `sprite_container.md` blit tables `(src<<3)|dst`.

### Upstream consumer (out of slice, xref proof)

`CPoemScroller::BlitDispatch@0x004368d0` — master compositor (`BlitTable_Opaque` @ `0x004b08c8`, etc.). Representative callers: `CBulPicture_DrawSurface`, `CGunMouse_Draw`, `CDSFont::DrawChar`, `CScrollBar_Render`, `CColorSet_Render`, `CompressFromImage`.

## 4. Ghidra deltas

| Action | Target |
|--------|--------|
| `rename_function_by_address` | `0x00435db0` → `SampleSourcePixel_Indexed8` |
| `rename_function_by_address` | `0x00435e20` → `Blit_ReadDstPixel_32bpp` |
| `save_program` | `bulanci.exe` (once) |

No `set_function_this_type` changes — decompiler already types `CDSImage` helpers correctly; `FindNearestPaletteIndex` intentionally operates on the embedded image facet (`CDSImage *` parameter) when called from scroller blit code.

## 5. Frida

none — static decompile + DATA table xrefs sufficient for this helper slice.

## 6. Remaining UNK

| Item | Notes |
|------|--------|
| `FUN_00436160@0x00436160` | Dest-side `PTR_LAB_004b0074` dispatch; only caller `BlitOpaqueFallback`. Needs table slot index ↔ format matrix proof before rename. |
| Exact mapping of every `PTR_LAB_004b0074` slot index to bpp/format | Partially documented via decompiler comments; full 8×8 matrix not enumerated in this task. |
| `CPoemScroller_ColorDistanceL1@0x00435c20` | Uses bytes at `pVftable_primary` as reference RGB — likely wrong struct field names in decompiler (target color storage), not wrong math. |

## Evidence paths consulted

- `ghidra_analysis/engine/ROUND6_LOGIC_PROTOCOL.md`
- `ghidra_analysis/engine/struct_recovery/CDSImage.md`
- `ghidra_analysis/formats/bmp_decoder.md`
- `ghidra_analysis/formats/sprite_container.md`
- `ghidra_analysis/engine/struct_recovery/CPoemScroller.md`
- `config/bulanci/ghidra_functions_dump.csv` (function sizes)
