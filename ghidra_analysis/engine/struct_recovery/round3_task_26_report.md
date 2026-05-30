# Round 3 — task 26 report

## Task

| Field | Value |
|-------|-------|
| **id** | 26 (`agent_todos_50_r3.json`) |
| **title** | Apply CDSBmpImage Load/Save stream-host MI at +0x54; CDSBitmap CMovieView tail parity |
| **types** | `CDSBmpImage`, `CDSBitmap`, `ODSImage`, `CDSChained` |
| **acceptance** | Ghidra struct/prototype/plate apply; document in `CDSBmpImage.md` / `CDSBitmap.md` with func@addr evidence |

## Status

**DONE** — `CDSBmpImage` Load/Save decompile with stream-host `this` at object `+0x54` and `this[-1].m_image.*` adjustor (`-0x50` pixel plane); `CDSBitmap` shell field-named through `+0x67` with `CMovieView` embed parity (`wViewFlags \|= 0x278`, `trackImage` bind).

## Evidence

### CDSBmpImage — stream-host MI @ +0x54

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Load entry `ECX` = stream-host facet | `CDSBmpImage_LoadDibStream@0x004320c0` | Plate: stream-host MI @ object `+0x54`; pixel plane `(byte *)this - 0x50` → `m_image` @ object `+0x04` |
| Allocate / row walk use image base | `CDSBmpImage_LoadDibStream@0x004320c0` | `CDSImage__Allocate(&this[-1].m_image.nM_stride, …)`; palette/row reads via `&this[-1].m_image.*` |
| Save same adjustor | `CDSBmpImage_SaveDibStream@0x00432440` | PRE_COMMENT + decompile: `this` = stream-host `+0x54`; `this_00 = &this[-1].m_image.nM_stride`; stack temporaries call `CDSBmpImage_ctor((CDSBmpImage *)(local_* + 0x54), …)` for downconvert blit |
| Ghidra struct | `get_struct_layout CDSBmpImage` | **100 B**: `pVf_IDSReferenced` @ 0 + `CDSImage m_image` @ +4 (heap alloc still **0x60** via `OperatorNewWithBadAlloc`) |
| Primary vtable name | `CDSBmpImage_ctor@0x00432330` | `pVf_IDSReferenced` @ object `+0x00`; `m_image` facet init via `CDSImage` ctor path |

### CDSBitmap — CMovieView tail parity

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Shell size / embed | `CDSBitmap_ctor@0x004393d0` | `CDSChained_ctor` on `pVftable_primary`; `ODSImage_ctor(&trackImage)` @ `this+0x68`; `SetOwner(trackImage, this)` |
| `CMovieView` shares bitmap prefix | `CMovieView_Constructor@0x004236a0` | `CDSBitmap_ctor((CDSBitmap *)this, …)` — first **0x78** bytes = `CDSBitmap` |
| Audio sequence → `trackImage` | `CMovieView_InitTrackSequence@0x004237b0` | `ODSImage__SetImage(ESI+0x68, *(ESI+0x7c))` @ `0x004237ed`–`0x004237f0` |
| View flags on embedded bitmap | `CMovieView_InitTrackSequence@0x004237b0` | `OR word ptr [ESI+0x14], 0x278` @ `0x004237f5` — `bitmapBase.wViewFlags` @ `CDSBitmap+0x14` |
| Fallback audio load | `CMovieView_InitTrackSequence@0x004237b0` | If `pAudioSequence@+0x7c==0`: `g_pApp` stream mgr vfn `+0x10` → `CheckedVirtualBaseCast(DAT_004b826c)` → store `+0x7c` |
| Factory subobject (no `ODSImage_ctor`) | `CDSBitmap_SubobjectCtor@0x004228f0` | `SetOwner(param_1+0x1a*4, param_1)` — MI header init only |
| Ghidra shell layout | `get_struct_layout CDSBitmap` | **120 B (`0x78`)**: `CDSChained`-aligned through `+0x67`, `ODSImage trackImage` @ `+0x68` |

### CMovieView tail (above `CDSBitmap`)

| Offset | Field | Evidence |
|--------|-------|----------|
| `+0x78` | `pVideoResource` | `CMovieView_Constructor@0x004236a0` zero/init |
| `+0x7c` | `pAudioSequence` (`CDSAnimSequence *`) | ctor cast + `InitTrackSequence` bind |
| `+0x80` | `pPlayer` (`CDSAudioVideoPlayer *`) | `StartPlayback@0x00422d50` |

## Ghidra deltas

- `CDSBmpImage`: `pVf_primary` → `pVf_IDSReferenced`; nested `m_image` @ +4 (100 B struct)
- `set_function_prototype` on `CDSBmpImage_LoadDibStream@0x004320c0`, `CDSBmpImage_SaveDibStream@0x00432440`, `CDSBmpImage_ctor@0x00432330`
- `set_plate_comment` / decompiler comments on Load/Save stream-host `+0x54` / adjustor `-0x50`
- `CDSBitmap`: `CDSChained`-aligned field names (`nBbox_left/top`, chain vtables `@+0x54`/`+0x58`, `trackImage@+0x68`)
- `set_function_prototype` `CDSBitmap_ctor@0x004393d0`
- `rename_function` `CDSBitmap_SubobjectCtor@0x004228f0`, `CMovieView_Constructor@0x004236a0`, `CMovieView_InitTrackSequence@0x004237b0`
- `set_plate_comment` + EOL disasm comment @ `0x004237f5` (`wViewFlags \|= 0x278`)
- `save_program bulanci.exe`

## Struct doc updates

- [CDSBmpImage.md](./CDSBmpImage.md) — agent todo 26 r3 Ghidra apply block
- [CDSBitmap.md](./CDSBitmap.md) — shell rename + `CMovieView` / `InitTrackSequence` evidence table

## Remaining UNK

- `CMovieView_InitTrackSequence` decompile still uses `unaff_ESI` / `unaff_EDI` until tail is merged into `Constructor` epilogue (same blocker as R3 todo 15).
- `CDSBmpImage_ValidateStride` strict-branch field naming in decompiler (assembly uses `m_width` / `m_stride` / `m_format` per `bmp_decoder.md`).
- `CDSBitmap` chain band semantics (`dwChainField_08/0c`, `dwField_30..3c`, `dwField_50`, `dwField_5c/60/64`) beyond ctor / `ResetChainCounters`.
