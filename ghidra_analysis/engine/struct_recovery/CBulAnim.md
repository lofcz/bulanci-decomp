# CBulAnim

## Status

**PARTIAL** — `sizeof` **0xD4**; prefix through `+0x77` field-named (`ODSImage` embed at `+0x68..+0x77`, round 3 task 32); `trackManager` interior still opaque.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CBulAnim) == 0xD4` (212) | `0x0040eaa0` | `CBulAnim::CreateCBulAnim` → `_Globals::OperatorNewWithBadAlloc(0xd4)` |
| Free uses whole object | `0x004396f0` | `CBulAnim::ScalarDeletingDtor` → `_free(this)` when `param_1 & 1` |
| Lobby heap path | `0x004104f0` | `CMenu::CStartGame2_ctor` → `OperatorNew(0xd4)` → `CDSAnim::ParameterizedCtor` then CBulAnim vtable patch |

## Layout table (proven offsets)

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------:|-----:|------|------|----------------------|
| `+0x00` | 4 | `void *` | `vftable_CDSView` | ctor write `0x00480a14` @ `CreateCBulAnim@0x0040eaa0` |
| `+0x04` | 4 | `void *` | `vftable_IDSChained` | ctor write `0x004809f4` @ `CreateCBulAnim@0x0040eaa0` |
| `+0x08` | 4 | `uint` | `dwField_08` | `CDSChained_ctor@0x004032d0` via `CDSAnim::DefaultCtor@0x00439470` |
| `+0x0C` | 4 | `uint` | `dwField_0c` | same |
| `+0x10` | 4 | `void *` | `vftable_IDSReferenced` | ctor write `0x004809dc` @ `CreateCBulAnim@0x0040eaa0` |
| `+0x14` | 2 | `ushort` | `wViewFlags` | `CStartGame2_ctor@0x004104f0` `\|= 0x200` on lobby avatar |
| `+0x16` | 2 | `ushort` | `wPad_16` | padding before `+0x18` vtable |
| `+0x18` | 4 | `void *` | `vftable_IDSEventHandler` | ctor write `0x004809c8` @ `CreateCBulAnim@0x0040eaa0` |
| `+0x1C` | 4 | `uint` | `dwField_1c` | `CDSChained_ctor@0x004032d0` |
| `+0x20` | 4 | `int` | `nPos_x` | `CDSAnim::ParameterizedCtor@0x00439560`; lobby `(rowX+10)` @ `CStartGame2_ctor` |
| `+0x24` | 4 | `int` | `nPos_y` | same; lobby `y=0x44` |
| `+0x28` | 4 | `int` | `nBbox_right` | `CDSChained_ctor` zero-init |
| `+0x2C` | 4 | `int` | `nBbox_bottom` | `CDSChained_ctor` zero-init |
| `+0x30` | 4 | `uint` | `dwField_30` | `CDSChained_ctor` zero-init |
| `+0x34` | 4 | `uint` | `dwField_34` | same |
| `+0x38` | 4 | `uint` | `dwField_38` | same |
| `+0x3C` | 4 | `uint` | `dwField_3c` | same |
| `+0x40` | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters@0x0042beb0` |
| `+0x44` | 1 | `byte` | `bVisibility_mask` | `TM_SetTrack@0x004391e0` `TEST byte [outer+0x44], 0x40` |
| `+0x46` | 2 | `ushort` | `wChainFlag46` | `ResetChainCounters@0x0042beb0` |
| `+0x48` | 2 | `ushort` | `wChainFlag48` | same |
| `+0x4A` | 2 | `ushort` | `wChainFlag4a` | same |
| `+0x4C` | 4 | `void *` | `pParent` | `ResetChainCounters` |
| `+0x50` | 4 | `uint` | `dwField_50` | `ResetChainCounters` |
| `+0x54` | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor` |
| `+0x58` | 4 | `void *` | `pVftable_CDSChain_IDSChained` | `CDSChained_ctor` |
| `+0x5C` | 4 | `uint` | `dwField_5c` | `CDSChained_ctor` zero |
| `+0x60` | 4 | `uint` | `dwField_60` | same |
| `+0x64` | 4 | `uint` | `dwField_64` | same |
| `+0x68` | 4 | `void *` | `vftable_IDSAnim` | `CreateCBulAnim@0x0040eaa0`; `PrimaryTick_thunk@0x0040b840` → `TM_TickBlit(this+0x68)` |
| `+0x6C` | 4 | `void *` | `vftable_CDSUpdatedItem` | ctor write `0x0048099c` @ `CreateCBulAnim@0x0040eaa0` (second `ODSImage` vptr — see embed note) |
| `+0x70` | 4 | `CBulanci *` | `pOwner` | `ODSImage` mixin @ `+0x68`: `SetPalette@0x00439010` `[facet+0x8]`; `IDSAnim_NotifyEvents@0x00438f20` / `IDSAnim_BindUserData@0x00438f60` via facet `+0x4` → `+0x70`; `AnimInner_Teardown@0x004392e0` clears when `ESI=outer+0x68` |
| `+0x74` | 4 | `CDSImage *` | `pDrawable` | `SetPalette@0x00439010` → `GetPaletteBuffer([facet+0xc])`; `ODSImage__SetImage@0x00439100`; `IDSAnim_SetSequence_thunk@0x004391d0`; released in `AnimInner_Teardown@0x004392e0` / `CDSAnim_CleanupBody@0x0040a7d0` |
| `+0x78` | 4 | `void *` | `vftable_AnimInner` | ctor write `0x00480988` @ `CreateCBulAnim@0x0040eaa0`; `PrimaryRender_thunk@0x0040b850` → `TM_RenderFrame(this+0x78)` |
| `+0x7C` | 4 | `void *` | `pVbase_ptr` | zeroed @ `CDSAnim::DefaultCtor@0x00439470`; `AnimInner_Init@0x00439270` |
| `+0x80` | 4 | `void *` | `pOuter_this` | zeroed @ `DefaultCtor`; `AnimInner_Init` writes back-pointer |
| `+0x84` | 4 | `uint` | `dwField_84` | zeroed @ `CDSAnim::DefaultCtor@0x00439470` |
| `+0x88` | 72 | `CDSVideoPlayer` | `trackManager` | `CDSAnim::DefaultCtor@0x00439470` → `ConstructTrackManager(this+0x88)`; `CleanupBody@0x0040ad20` → `CDSVideoPlayer_TM_Destructor(this+0x88)`; `scheduler` = `CDSUpdatedItem` @ `+0x8c` |
| `+0xD0` | 1 | `byte` | `teamId` | `CBulAnim_OnTeamPaletteEvt@0x0040b020` — `CMP byte [ECX+0xD0],AL`; `CStartGame2_ctor` writes team byte |

### `ODSImage` embed at `+0x68..+0x77` (16 bytes)

Dual vptrs at `+0x68` / `+0x6C` (Ghidra names `pVftable_IDSAnim` / `pVftable_CDSUpdatedItem`; RTTI maps to `ODSImage` + second face — same pattern as `CDSBitmap_ctor@0x004393d0` calling `ODSImage_ctor(this+0x68)`). Instance data:

| Offset | Size | Name | Evidence |
|-------:|-----:|------|----------|
| `+0x70` | 4 | `pOwner` | `ODSImage::SetOwner@0x00439050`; palette invalidate + script notify target (`SetPalette`, `IDSAnim_NotifyEvents`, `IDSAnim_BindUserData`) |
| `+0x74` | 4 | `pDrawable` | `ODSImage__SetImage`, sequence bind thunk, `GetPaletteBuffer` input |

`ApplyTeamPalette@0x0040afd0` calls `SetPalette((CBulAnim *)&this->pVftable_IDSAnim, lut)` → `ECX = outer+0x68` (`SetPalette@0x00439010` disasm).

### Prefix init chain (round 3 task 31)

`CreateCBulAnim` / `CDSAnim::DefaultCtor` / `ParameterizedCtor` all run `ODSImage::CDSBitmap_SubobjectCtor` (`0x004228f0`), which calls `CDSChained_ctor` (`0x004032d0`) and `CDSChained_ResetChainCounters` (`0x0042beb0`). Bytes `+0x08..+0x67` are therefore the shared **CDSView / CDSChained drawable header** documented in [CDSChained.md](./CDSChained.md) and [anim_runtime.md](../anim_runtime.md); CBulAnim only overrides vtables at `+0x00/+0x04/+0x10/+0x18` and anim facets from `+0x68`.

## Ghidra apply

```
get_struct_layout CBulAnim → Size: 212 bytes (0xD4)
```

Slice **04** (2026-05-30): repaired tail after round-3 task 32 — `trackManager_lo` `undefined1[56]` @ `+0x88`, `trackManager_cont` `undefined1[16]` @ `+0xC0` (72 bytes total, matches `ConstructTrackManager(this+0x88)`); `teamId` @ `+0xD0`; `pad_0xD1` `byte[3]`; renamed `pVbase_ptr` @ `+0x7C`, `pOwner` @ `+0x70`. Decompile `CBulAnim_OnTeamPaletteEvt@0x0040b020` now uses `this->teamId` (`CMP [ECX+0xD0]`). Functions renamed: `CBulAnim_OnTeamPaletteEvt`, `CBulAnim_ApplyTeamPalette`, `CBulAnim_PrimaryTick_thunk`.

**Agent todo 08 (2026-05-30):** merged opaque tail into **`CDSVideoPlayer trackManager` @ `+0x88` (72 B)** with nested `CDSUpdatedItem scheduler` @ `+0x8c`; `bTeamId` @ `+0xD0`; `pOwner` `CBulanci *` @ `+0x70`. `get_struct_layout CBulAnim` → 212 bytes. `save_program bulanci.exe`.

**Round-2 todo 8 (2026-05-30):** named CDSView drawable prefix `+0x08..+0x67` to mirror `CDSAnim`/`CDSBitmap`: `dwChainField_08/0c`, `pVft_event`/`pVft_ref`, `wChainInit44` (ushort, was `bVisibility_mask` byte); chain band `+0x40..+0x64` unchanged. Decompile `CStartGame2_ctor@0x004104f0` uses `wViewFlags`, `nPos_x/y`. `save_program bulanci.exe`.

## RTTI / vtables

Primary CDSView vtable `0x00480a14` (`CBulAnim` in `vftable_methods.csv`). Factory: `CreateCBulAnim@0x0040eaa0`. Inherits `CDSAnim::DefaultCtor` before CBulAnim vtable patch (`0x00439470`).

## UNK

- Semantic names for `dwField_08` / `dwField_0c` / `dwField_30..3c` (inherit `CDSChained` UNK).
- `dwField_1c` @ `+0x1C`: `IDSAnim_SelectRandomTrack@0x004392a0` casts to `CDSStreamStorage*` and calls `GetStreamCount` — asm effective read is **`dwChainRoot` @ `+0x40`**, not pack `m_items` (see [CDSStreamStorage.md](./CDSStreamStorage.md) todo 46).
- `nBbox_right` / `nBbox_bottom` — zero-init only on anim path; no CBulAnim-specific bbox writer found.
- `+0x45` — high byte overlapping `wChainInit44` vs `bVisibility_mask` at `+0x44`.
- `trackManager` — optional nested `CDSTrackVector` at `trackManager+0x1c` (see [CDSVideoPlayer.md](./CDSVideoPlayer.md) todo 44).
