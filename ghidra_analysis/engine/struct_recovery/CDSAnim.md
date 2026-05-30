# CDSAnim

## Status

**PARTIAL** — `sizeof` **0xd0** proven; vtable slots, **CDSView / CDSChained drawable shell** through `+0x67`, and core anim/track fields in Ghidra; tail `+0xb8..+0xcf` and `track_manager` innards still opaque.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSAnim) == 0xd0` | `0x004396c8` | `_Globals::CreateObject` — `OperatorNewWithBadAlloc(0xd0)` then `CDSAnim::DefaultCtor` @ `0x004396c0` |
| `sizeof(CDSAnim) == 0xd0` | `0x004110f3` | `CBulanci::CScoreCtor` — `OperatorNewWithBadAlloc(0xd0)` before `CDSAnim::ParameterizedCtor` @ `0x00411010` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `vft_primary` | `CDSAnim::DefaultCtor` @ `0x00439470` → `*(this+0)=0x4876fc`; `CDSAnim::ParameterizedCtor` @ `0x00439560` |
| 0x04 | 4 | `void *` | `vft_chain` | same ctors → `0x4876e0` |
| 0x08 | 4 | `uint` | `dwChainField_08` | `CDSChained_ctor` via `CDSBitmap_SubobjectCtor` @ `0x004228f0` |
| 0x0c | 4 | `uint` | `dwChainField_0c` | same |
| 0x10 | 4 | `void *` | `vft_event` | `DefaultCtor` / `ParameterizedCtor` → `0x4876c8` |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CDSChained` shell; `\|= 0x200` on roster anim (`anim_runtime.md`) |
| 0x16 | 2 | `ushort` | `wPad_16` | `CDSChained` mirror |
| 0x18 | 4 | `void *` | `vft_ref` | ctors → `0x4876b4` (`IDSReferenced` face) |
| 0x1c | 4 | `uint` | `dwField_1c` | `CDSChained_ctor` zero @ `0x004032d0` |
| 0x20 | 4 | `int` | `nPos_x` | `CDSAnim::ParameterizedCtor` @ `0x00439560` — `=param_1` |
| 0x24 | 4 | `int` | `nPos_y` | same — `=param_2` |
| 0x28 | 4 | `int` | `nBbox_right` | `CDSChained_ctor` zero band (`param_1[10]`) |
| 0x2c | 4 | `int` | `nBbox_bottom` | same (`param_1[0xb]`) |
| 0x30 | 4 | `uint` | `dwField_30` | `CDSChained_ctor` / `ResetChainCounters` band |
| 0x34 | 4 | `uint` | `dwField_34` | same |
| 0x38 | 4 | `uint` | `dwField_38` | same |
| 0x3c | 4 | `uint` | `dwField_3c` | same |
| 0x40 | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters` @ `0x0042beb0` |
| 0x44 | 2 | `ushort` | `wChainInit44` | `TM_SetTrack` @ `0x0043924c` — `TEST byte [outer+0x44],0x40` (visibility bit in low byte) |
| 0x46 | 2 | `ushort` | `wChainFlag46` | `ResetChainCounters` |
| 0x48 | 2 | `ushort` | `wChainFlag48` | same |
| 0x4a | 2 | `ushort` | `wChainFlag4a` | same |
| 0x4c | 4 | `void *` | `pParent` | `ResetChainCounters` zero; `CDSChained` layout |
| 0x50 | 4 | `uint` | `dwField_50` | same |
| 0x54 | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor` @ `0x004032d0` |
| 0x58 | 4 | `void *` | `pVftable_CDSChain_IDSChained` | same |
| 0x5c | 4 | `uint` | `dwField_5c` | ctor zero |
| 0x60 | 4 | `uint` | `dwField_60` | ctor zero |
| 0x64 | 4 | `uint` | `dwField_64` | ctor zero |
| 0x68 | 4 | `void *` | `vft_update` | `DefaultCtor` / `ParameterizedCtor` → `0x4876a0` |
| 0x6c | 4 | `void *` | `vft_anim` | same → `0x487688` |
| 0x70 | 4 | `uint` | `dwField_70` | *(no direct write in batch ctors; subscriber-key slot per `anim_runtime.md`)* |
| 0x74 | 4 | `void *` | `active_sequence` | `_Globals::AnimInner_Teardown` @ `0x004392e0` when called with `this+0x68` releases `*(base+0xc)` → object `+0x74` |
| 0xb8 | — | *(in `track_manager`)* | `pRenderTarget` | Same storage as former `cached_sequence` label: `AnimInner_Init@0x00439270` / `TM_SetTrack@0x004391e0` use `inner+0x40` = `track_manager+0x30` (`CDSVideoPlayer.pRenderTarget`) |
| 0x78 | 4 | `void *` | `vft_anim_sub` | `DefaultCtor` / `ParameterizedCtor` → `0x487674` |
| 0x7c | 4 | `void *` | `vbase_ptr` | `DefaultCtor` zeros; `_Globals::AnimInner_Init` @ `0x00439270` writes `*(inner+4)` with `CheckedVirtualBaseCast(outer,0x12)` |
| 0x80 | 4 | `void *` | `outer_this` | `DefaultCtor` zeros; `AnimInner_Init` @ `0x00439270` writes `*(inner+8)=param_1` |
| 0x84 | 4 | `uint` | `field_84` | `CDSAnim::DefaultCtor` @ `0x00439470` — zeroed |
| 0x88 | 72 | `CDSVideoPlayer` | `track_manager` | `CDSObject::ConstructTrackManager` @ `0x00439c70` on `(CDSVideoPlayer *)(this+0x88)`; `AnimInner_Init` reads `inner+0x40` → `track_manager.pRenderTarget` @ `+0xb8`; `ParameterizedCtor` / `AnimInner_InitParam` @ `inner+0x10` |

## Ghidra apply

```
get_struct_layout CDSAnim → Size: 208 (0xd0) bytes
```

Applied fields: full **CDSChained drawable shell** `+0x00..+0x67` (29 named fields, mirrors `CDSChained` / `CDSBitmap`); anim faces `+0x68..+0x78`; **`CDSVideoPlayer track_manager` @ `+0x88` (72 B)**. Factory: `_Globals::CreateObject` @ `0x004396c0`. **Agent todo 27 (2026-05-30):** named `+0x28..+0x67` CDSView shell; `save_program bulanci.exe`.

## UNK

- `dwChainField_08/0c`, `dwField_30..3c`, `dwField_50`, `dwField_5c/60/64` — semantic names beyond ctor zero / `ResetChainCounters`.
- `+0x70` (`dwField_70`) — no direct ctor write (subscriber-key slot per `anim_runtime.md`).
- `wChainInit44` — visibility uses **byte** at `+0x44` (`TM_SetTrack`); ushort typed for `CDSChained` alignment.
- `+0xbc..+0xcf` — tail past `track_manager` (CBulAnim `team_index` @ `+0xd0` is sibling class only).
- `track_manager` inner fields — see `CDSVideoPlayer.md` / `CDSObject.md` track-manager prefix (`+0x00..+0x44`); `CDSTrackEntry` (8 B) for heap vector elements.
