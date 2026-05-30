# Round 3 — Task 05 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **title** | Retype CBulPicture pBitmap to CDSBitmap* |
| **one_liner** | `CBulPicture::pBitmap` @ `+0x68` is the static drawable handle for `BlitDispatch`; pool bind uses `CheckedVirtualBaseCast(..., DAT_004b826c)`. |
| **acceptance** | Ghidra `CBulPicture.pBitmap` typed `CDSBitmap *`; member methods use `this->pBitmap` where applicable; update struct doc |
| **structs** | `CBulPicture`, `CDSBitmap`, `CBulAnim` (lobby cast corroboration) |
| **addresses** | `0x0040eb30`, `0x004101d0`, `0x0040b050`, `0x004104f0` |
| **supersedes** | R2 todo 8 |

## Status

**DONE**

## Evidence

| Claim | Function | Address | Evidence |
|-------|----------|---------|----------|
| `pBitmap` @ `+0x68`, 4 B | `CBulPicture_ctor` | `0x00410221` | `MOV dword ptr [ESI+0x68], 0` before pool load |
| Prior handle release | `CBulPicture_ctor` | `0x0041025f`–`0x0041026d` | `ECX = [ESI+0x68]`; vtable `+0x8` release if non-NULL |
| Cast to drawable face | `CBulPicture_ctor` | `0x00410273`–`0x00410281` | `PUSH DAT_004b826c`; `CheckedVirtualBaseCast`; `MOV [ESI+0x68], EAX` |
| Extent from bitmap `+4`/`+8` | `CBulPicture_ctor` | `0x00410284`–`0x0041029d` | `[EAX+4]` added to `nOriginX` → `nExtentW`; `[EAX+8]` + `nOriginY` → `nExtentH` |
| Factory clears handle | `CBulPicture_Create` | `0x0040eb30` | Decompile: `pCVar1->pBitmap = (CDSBitmap *)0x0` after `OperatorNew(0x470)` |
| Blit consumer | `CBulPicture_DrawSurface` | `0x0040b050` | `BlitDispatch(..., &nRenderLeft, this->pBitmap, ...)`; chroma via `bChromaKeyIndex` |
| Same cast id in lobby | `CStartGame2_ctor` | `0x004104f0` | `local_420 = CheckedVirtualBaseCast(pvVar7, DAT_004b826c)` for `CGameView_ctor` avatar bitmap (CBulAnim path; not stored on `CBulPicture`) |

**Semantics:** Nominal type is `CDSBitmap *` for the drawable MI used by `BlitDispatch`. Runtime object is often a **`CDSFlxFile` static-face subobject** (ClassID **52** via `DAT_004b826c`), not necessarily a heap `OperatorNew(0x78)` `CDSBitmap`. Height/width band reads use **`pBitmap+4` / `pBitmap+8`** on that face (may decompile as wrong `CDSBitmap` field names until `CDSBitmap` / FLX face layout is aligned).

## Ghidra deltas

- `get_struct_layout CBulPicture` — `pBitmap` **`CDSBitmap *`** @ offset **104** (`0x68`); size **1136** (`0x470`).
- `modify_struct_field` `CBulPicture.pBitmap` → `CDSBitmap *` (idempotent confirm).
- `set_function_this_type` `CBulPicture_ctor@0x004101d0` → `CBulPicture *` (was `CBulanci *`; decompiler now `CBulPicture::CBulPicture_ctor` with `this->pBitmap`, `this->pPalette`, named layout fields).
- `set_function_this_type` `CBulPicture_DrawSurface@0x0040b050` — confirmed in class `CBulPicture`.
- `CBulPicture_Create@0x0040eb30` — **not** retargeted (`__stdcall` factory, no implicit `this`).
- Existing plate/decompiler comments on ctor/draw retained.
- `save_program bulanci.exe`.

## Struct doc updates

- [CBulPicture.md](./CBulPicture.md) — Ghidra apply line already documents R3 todo 5; no layout change.

## Remaining UNK

- `CBulPicture_ctor` decompile may still map `pBitmap+4/+8` to wrong `CDSBitmap` member names (`pVftable_IDSChained` / `dwChainField_08`) until `CDSBitmap` / FLX drawable face offsets match disasm.
- Whether every `pBitmap` instance is ClassID **52** FLX static face vs a true heap `CDSBitmap` (nominal pointer type is intentional).
- `+0x40..+0x67` CDSView tail between render rect and `pBitmap` (unchanged).
