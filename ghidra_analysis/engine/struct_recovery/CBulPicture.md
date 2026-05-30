# CBulPicture

## Status

**PARTIAL** — `0x470` size and portrait/blit/palette tail are proven; `+0x30..+0x67` blit-consumer subobject is sized but not field-resolved.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CBulPicture) == 0x470` (1136) | `0x0040eb30` | `CBulPicture_Create` → `OperatorNewWithBadAlloc(0x470)` |
| Heap object is `CBulPicture` | `0x0040ebd0` | `CBulPicture_ScalarDeletingDtor` → `_free(this)` |
| Embedded in parent panel | `0x004101d0` | `CBulPicture_ctor` runs on caller-allocated storage (same layout as factory) |

## Layout table (proven offsets only)

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------:|-----:|------|------|----------------------|
| `+0x00` | 4 | `void *` | `vftable_CDSView` | `CBulPicture_Create@0x0040eb30`, `CBulPicture_ctor@0x004101d0` |
| `+0x04` | 4 | `void *` | `vftable_IDSChained` | `CBulPicture_ctor@0x004101d0` |
| `+0x08` | 4 | `uint` | `dwField_08` | `CDSChained_ctor@0x004032d0` via `CBulPicture_ctor@0x004101d0` (`param_1[2]=0`); no CBulPicture consumer |
| `+0x0C` | 4 | `uint` | `dwField_0c` | same (`param_1[3]=0`) |
| `+0x10` | 4 | `void *` | `vftable_IDSReferenced` | `CBulPicture_ctor@0x004101d0` → `[this+0x10]=0x480a9c` |
| `+0x14` | 2 | `ushort` | `wViewFlags` | `CBulPicture_ctor@0x004101d0` ORs `0x200` into `*(ushort*)(this+0x14)` |
| `+0x16` | 2 | `ushort` | `wPad_16` | shared `CDSChained` padding; no CBulPicture-specific xref |
| `+0x18` | 4 | `void *` | `vftable_IDSEventHandler` | `CBulPicture_ctor@0x004101d0` → `[this+0x18]=0x480a88` |
| `+0x1C` | 4 | `uint` | `dwField_1c` | `CDSChained_ctor@0x004032d0` (`param_1[7]=0`); no CBulPicture consumer |
| `+0x20` | 4 | `int` | `originX` | `CBulPicture_ctor@0x004101d0` stores `originX` |
| `+0x24` | 4 | `int` | `originY` | ctor stores `originY` |
| `+0x28` | 4 | `int` | `extentW` | ctor: `originX + *(bitmap+4)` |
| `+0x2C` | 4 | `int` | `extentH` | ctor: `originY + *(bitmap+8)` |
| `+0x30` | 4 | `int` | `renderLeft` | `CBulPicture_DrawSurface@0x0040b050` → `BlitDispatch(this+0x30, *(this+0x68), …)` |
| `+0x34` | 4 | `int` | `renderTop` | same |
| `+0x38` | 4 | `int` | `renderRight` | same |
| `+0x3C` | 4 | `int` | `renderBottom` | same |
| `+0x40` | 40 | `byte[40]` | `pad_40` | no CBulPicture-specific xrefs; shared CDSView tail before `pBitmap` |
| `+0x68` | 4 | `CDSBitmap *` | `pBitmap` | `Create` sets `[0x1a]=0`; ctor `CheckedVirtualBaseCast(..., DAT_004b826c)` → `this+0x68`; dtor releases prior handle; `DrawSurface` → `BlitDispatch(..., pBitmap, ...)` |
| `+0x6C` | 1024 | `uint[256]` | `palette` | `Create`/`ctor` init `0x100` dwords to `0xFFFFFFFF`; `ApplyTeamPalette@0x0040aa70` writes LUT here |
| `+0x46C` | 1 | `byte` | `teamId` | `CBulPicture_ctor` stores low byte of `param_1`; `OnRecolorEvent@0x0040aaa0` filters on it |
| `+0x46D` | 1 | `byte` | `chromaKeyIndex` | ctor sets `0xFF`; `DrawSurface@0x0040b050` — if not `0xFF`, sets chroma `flags=0xC` |

## Ghidra apply

```
get_struct_layout CBulPicture → Size: 1136 bytes (0x470)
```

Applied via `create_struct` (1136 bytes). Header `+0x08..+0x1F` matches `CDSChained` (`CDSChained.md`); round-3 task 33 renamed `dwField_08` / `dwField_0c` / `dwField_1c` / `wPad_16` in Ghidra. `+0x30..+0x3C` = CDSView on-screen render rect (`nRenderLeft`..`nRenderBottom`); **`pBitmap` typed `CDSBitmap *` @ `+0x68`** (R3 todo 5, 2026-05-30): `modify_struct_field` + `set_function_this_type` on `CBulPicture_ctor@0x004101d0` / `CBulPicture_DrawSurface@0x0040b050` → decompiler `this->pBitmap`; pool load + `CheckedVirtualBaseCast(..., DAT_004b826c)` static drawable face (often `CDSFlxFile` ClassID **52** subobject; extent via disasm `pBitmap+4/+8`). `palette` @ `+0x6C`, `bTeamId` / `bChromaKeyIndex` @ `+0x46C`/`+0x46D`. See `CDSFlxFile.md` downstream chain.

**FLX decode path (worker 31):** per-frame pixels are **not** decoded into `CBulPicture` directly. Track manager `TM_AdvanceFrame@0x00439a15` calls `CDSFlxFile::DecodeFrame` on the resource meta face (`CheckedVirtualBaseCast(..., DAT_004b8370)` at bind) with `consumer = trackManager.pRenderTarget` (`CreateBoundClone` `0x60` surface). `CBulPicture` consumes the static bitmap handle at `pBitmap` for `BlitDispatch`.

**Sequence timing (worker 32):** `CBulPicture_Create@0x0040eb30` does **not** write `seq[0x10]`. Track entries hold `CDSFlxFile*` meta face `resource+4` (`AddTrackSource` stack `{key, seq*}`). `TM_AdvanceFrame` reads `seq[0x10]` / `seq[0x14]` from the bound `CDSFlxFile` object (`nSeqTotalDurationMs` @ `+0x14`, `nSeqFrameCountMinusOne` @ `+0x18`), populated when the FLX stream binds: `CDSFlxFile_BindStream@0x00432ac0` stores file header `+0x1c` (`inMemSize`, usually `0x470` = 1136 ms total) and `+0x20` (`flags` = frame count − 1). Do **not** alias `CBulPicture.wViewFlags` @ `+0x14` — that ushort is unrelated to anim timing.

Slice **04** (2026-05-30): renamed `DrawSurface` → `CBulPicture_DrawSurface@0x0040b050`; plate comment on `CBulPicture_ctor@0x004101d0` (embedded ctor, `CBulPicture*` intended — decompiler `this` still `CBulanci*`; Ghidra API limitation).

## RTTI / vtables

Primary view vtable used by `DrawSurface` path (`0x00480ad4` in `vftable_methods.csv`). `DrawSurface@0x0040b050` is CDSView slot 14.

## UNK

- Semantic meaning of `dwField_08`, `dwField_0c`, `dwField_1c` (zeroed by `CDSChained_ctor`; no CBulPicture-specific use).
- `+0x40..+0x67` — CDSView/widget tail between render rect and `pBitmap`; no CBulPicture-only field xrefs.
- `+0x46E..+0x46F` — tail padding to `0x470` (two bytes after chroma index).
- Runtime `pBitmap` may point at a **`CDSFlxFile` static-face subobject** (not a `OperatorNew(0x78)` heap `CDSBitmap`); Ghidra nominal type is `CDSBitmap *` for `BlitDispatch` / drawable MI. `CBulPicture_ctor` decompiler `this` still `CBulanci*` (__thiscall API limitation).
- Whether `nSeqTotalDurationMs` (`0x470`) is always the intended total clip ms vs a coincidental reuse of the `sizeof(CBulPicture)` alloc hint (pack shows constant `0x470` on all master-pack sprites; Bresenham then yields ~`1136/N` ms per frame).
