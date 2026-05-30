# Round 3 — task 01 report (agent todo 1 r3)

## Task

| Field | Value |
|-------|--------|
| **id** | 1 |
| **title** | CBulanci audio tail xrefs and CDSObject image +0x50 |
| **one_liner** | Prove legacy `CBulanci+0x4b0..+0x4c8` asm offsets alias embedded `CGame` audio/cmdline tail (`game+0x22c..`); name `CDSObject+0x50` dword tag in Ghidra and decompile. |
| **acceptance** | Xref evidence at listed addresses; Ghidra struct/decompile uses `CGame` embed fields (no duplicate CBulanci tail); `CDSObject.dwImageField_50` @ +0x50 with func@addr proof; `save_program` if mutated. |
| **types** | `CBulanci`, `CDSObject`, `CGame` |
| **priority** | critical (blocker; supersedes todo 7) |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Embedded `CGame` ctor + audio tail init | `0x004026f0` | `CBulanci_ctor`: `CGame_ctor(&param_1->game)` then `(game).pAudioBankArray` … `bByte_4c8` |
| `CBulanci+0x4b8` ≡ `game.dwAudioBankCount` | `0x00402761` | Asm `MOV [ESI+0x4b8], EBX`; decompile `(game).dwAudioBankCount = 0`; arith `0x284+0x234=0x4b8` |
| Audio-bank dtor loop uses embed tail | `0x00402c8a` / `0x00402d5b` | `CBulanci_dtor`: `(game).pReleaseOnDestroy` @ `+0x4c0`; loop on `(game).dwAudioBankCount` / `(game).pAudioBankArray` @ `+0x4b8`/`+0x4b0` |
| `CBulanci_ReleaseAudioBank` dynarray base | `0x00402bee` | Decompile `&(this->game).pAudioBankArray`; asm `LEA ESI,[EBX+0x4b0]` |
| `CDSObject+0x50` dword tag `= 1` | `0x0042563a` / `0x00425670` | `CDSObject_CtorWithImage`: `this->dwImageField_50 = 1` after `CDSImage_ctor(&embed_overlay)` |
| `CDSImage+0x44` default bpp tag | `0x00425460` / `0x004254af` | `CDSImage_ctor`: `this->nDefaultBppTag = 8`; on embed path abs `CDSObject+0x48` (`nEmbeddedImage_field_44`) |
| No duplicate CBulanci tail fields | MCP | `get_struct_layout CBulanci` → 1228 B; members `pMainMenu` @ 640, `game` `CGame` @ 644 only past `CDSApp` |
| `CGame` audio tail layout | MCP | `get_struct_layout CGame` → 584 B; `pAudioBankArray` @ 556 (`+0x22c`) … `bByte_4c8` @ 580 |

### Offset alias table (CBulanci abs → CGame rel)

| CBulanci abs | CGame rel | Field |
|--------------|-----------|-------|
| `+0x4b0` | `+0x22c` | `pAudioBankArray` |
| `+0x4b4` | `+0x230` | `dwReserved_beforeBankCount` |
| `+0x4b8` | `+0x234` | `dwAudioBankCount` |
| `+0x4bc` | `+0x238` | `dwAudioBankCapacity` |
| `+0x4c0` | `+0x23c` | `pReleaseOnDestroy` |
| `+0x4c4` | `+0x240` | `pCmdLine` |
| `+0x4c8` | `+0x244` | `bByte_4c8` |

Base: `CBulanci+0x284` = `&this->game`.

## Ghidra deltas

- `set_function_this_type` `CDSImage_ctor@0x00425460` → `CDSImage *` (decompile shows `nDefaultBppTag`, raster fields).
- `set_function_this_type` `CDSObject_CtorWithImage@0x00425620` → `CDSObject *` (confirmed).
- `force_decompile` on `CDSImage_ctor`, `CDSObject_CtorWithImage`.
- Decompiler / plate comments @ `0x00402761`, `0x00402d5b`, `0x004254af`, `0x00425670` (offset alias + `dwImageField_50` semantics).
- `save_program bulanci.exe`.

## Struct doc updates

- [`CBulanci.md`](./CBulanci.md) — agent todo 1 r3 xref alias table + Ghidra apply note.
- [`CGame.md`](./CGame.md) — round-3 task 1 audio-tail xref cross-ref.
- [`CDSObject.md`](./CDSObject.md) — agent todo 1 r3 Ghidra apply (`dwImageField_50`, `CDSImage_ctor` re-parent).

## Remaining UNK

- `FUN_004028d0` scalar-destruction helper still uses dword indices (`param_1[0x130]` …) — `__fastcall`, not in scope for `set_function_this_type`.
- `CDSApp` interior / `field_0xNN` on `CBulanci_ctor` (separate task).
- `dwReserved_beforeBankCount` purpose (dead store).
- `CDSObject` MI thunk wiring for `pImage_vf_streamHost` / `pImage_tail_5c` (defer to `CDSImage.md`).
