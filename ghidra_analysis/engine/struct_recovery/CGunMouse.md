# CGunMouse

## Status

**VERIFIED** — size `0x218` (536 bytes). Layout **PARTIAL** (proven scalar/cursor fields + four embedded `CDSImage` slots; track-manager and coord-ring internals mostly UNK).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CGunMouse) == 0x218` | `CGunMouse_CreateObject` @ `00426500` | `OperatorNewWithBadAlloc(0x218)` then `ODSImage::CGunMouse_ctor` |
| Last ctor dword index `0x85` | `CGunMouse_ctor` @ `00426060` | Writes through `param_1[0x85]` → offset `0x214` + 4 = `0x218` |
| Dtor walks through `this+0x208` | `CGunMouse_dtor` @ `00425730` | `CDSVec_ResizeIntPair(this+0x208,0)` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_IDSChained` | `CGunMouse_ctor@00426060` |
| `0x04` | 4 | `void *` | `vf_IDSEventHandler` | `CGunMouse_ctor@00426060` |
| `0x08` | 4 | `int` | `refCount` | `CGunMouse_ctor@00426060` (`param_1[2]=1`) |
| `0x0c` | 4 | `void *` | `vf_primary` | `CGunMouse_ctor@00426060`, `CGunMouse_dtor@00425730` |
| `0x10` | `0x60` | `CDSImage` | `armH` | `CGunMouse_ctor@00426060` (`+4`), `CGunMouse_dtor@00425730` (`+0x10`), `CGunMouse_Draw@00424610` (`+0x14`) |
| `0x70` | `0x60` | `CDSImage` | `armV` | `CGunMouse_ctor@00426060` (`+0x1c`), `CGunMouse_Draw@00424610` (`+0x74`) |
| `0xd0` | `0x60` | `CDSImage` | `dotImage` | `CGunMouse_ctor@00426060` (`+0x34`), `CGunMouse_Draw@00424610` (`+0xd4`) |
| `0x130` | `0x60` | `CDSImage` | `reticleRing` | `CGunMouse_ctor@00426060` (`+0x4c`), `CGunMouse_Draw@00424610` (`+0x134`) |
| `0x190` | 4 | `int` | `m_mouseX` | `CGunMouse_Draw@00424610` |
| `0x194` | 4 | `int` | `m_mouseY` | `CGunMouse_Draw@00424610` |
| `0x198` | 4 | `int` | `m_targetX` | `CGunMouse_Draw@00424610` (ring pop) |
| `0x19c` | 4 | `int` | `m_targetY` | `CGunMouse_Draw@00424610` |
| `0x1a0` | 4 | `int` | `m_dotOffsetX` | `CGunMouse_Draw@00424610`, `CGunMouse_OnMouseMove@00423900` |
| `0x1a4` | 4 | `int` | `m_dotOffsetY` | `CGunMouse_Draw@00424610`, `CGunMouse_OnMouseMove@00423900` |
| `0x1a8` | 4 | `int` | `m_dotBaseOffsetX` | `CGunMouse_OnMouseMove@00423900` |
| `0x1ac` | 4 | `int` | `m_dotBaseOffsetY` | `CGunMouse_OnMouseMove@00423900` |
| `0x1b0` | `0x48` | `CDSVideoPlayer` | `trackManager` | `CGunMouse_ctor@00426060` (`ConstructTrackManager+0x6c`), `CGunMouse_dtor@00425730` (`CDSVideoPlayer_TM_Destructor+0x1b0`), `CGunMouse_OnAnimTick@00423bd0`; ctor reads `pRenderTarget` @ `+0x1e0` |
| `0x1f8` | 4 | `uint` | `m_lastPushTick` | `CGunMouse_ctor@00426060` (`+0x7e`), `CGunMouse_Draw@00424610` |
| `0x1fc` | 4 | `void *` | `m_dotSpriteInfo` | `CGunMouse_ctor@00426060` (`+0x7f`), `CGunMouse_Draw@00424610` |
| `0x200` | 4 | `void *` | `m_armSpriteInfo` | `CGunMouse_ctor@00426060` (`+0x80`), `CGunMouse_Draw@00424610` |
| `0x204` | 1 | `byte` | `m_inFrameNotifyMoveFlag` | `CGunMouse_OnMouseMove@00423900` (`observer+0x1f8` → `+0x204`) |
| `0x205` | 1 | `byte` | `m_isActive` | `CGunMouse_ctor@00426060` (byte at `+0x205`) |
| `0x206` | 1 | `byte` | `m_dotReady` | `CGunMouse_Draw@00424610` |
| `0x207` | 1 | — | `pad_207` | — |
| `0x208` | `0x10` | `CGunMouseCoordQueue` | `m_coordQueue` | `CGunMouse_ctor@00426060` (zero + `CGunMouse_CoordRing_Reserve(0x1e)`), `CGunMouse_Draw@00424610`, `CGunMouse_CoordRing_Clear@00423ac0` |

### `m_coordQueue` (`CGunMouseCoordQueue`, `this+0x208`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `m_data` | `CDSVec_ResizeIntPair@004250c0` (`*(this+0)` realloc `capacity*8`); `CGunMouse_CoordRing_Push@00424370` (Ghidra label `pM_data`) |
| `0x04` | 4 | `int` | `m_capacity` | `CDSVec_ResizeIntPair@004250c0` (`*(this+4)`); `CGunMouse_CoordRing_Reserve@004253d0` (`param_1=0x1e`) (Ghidra `nM_capacity`) |
| `0x08` | 4 | `int` | `m_size` | `CGunMouse_CoordRing_Push@00424370`, `CGunMouse_CoordRing_Pop@004243b0`, `CGunMouse_CoordRing_Clear@00423ac0` (`+8=0`) (Ghidra `nM_size`) |
| `0x0c` | 4 | `int` | `m_head` | `CGunMouse_CoordRing_Pop@004243b0` (advance oldest index, wrap at `m_capacity`) (Ghidra `nM_head`) |

Ring stores `(i32,i32)` pairs in `m_data[(m_head+m_size)%m_capacity]`; push no-ops when `m_size==m_capacity`; draw pops only when `m_size==0x1e` (see `red_menu_cursor.md` §4).

## Ghidra apply

Batch 8: placeholder → `CGunMouse` (536 B). Follow-up round 2: `coordRing` → nested `CGunMouseCoordQueue` `m_coordQueue` (16 B).

**Slice 09 (2026-05-30):** `modify_struct_field` — `armH`/`armV`/`dotImage`/`reticleRing` typed as embedded `CDSImage` (96 B each); `CGunMouseCoordQueue` fields `m_data`/`m_capacity`/`m_size`/`m_head`; prototypes `CGunMouse_Draw`/`CGunMouse_ctor`/`CGunMouse_OnMouseMove`/`CGunMouse_CoordRing_{Push,Pop}` use `CGunMouse*` / `CGunMouseCoordQueue*`. `get_struct_layout`: **Size: 536**, 25 top-level fields (four `CDSImage` + nested queue).

**R4 task 31:** `pVf_odsimage` @ `0x00483794` — `vfn[4]` = `CGunMouse_OnAnimTick@0x00423bd0` (FLX opcode `0x0C`); **`u16` only `0xFFFF`** triggers random `SetCurrentTrack` + optional `TM_Play`. Slot `[1]` repatched to `CGunMouse_OnMouseMove@0x00423900` (opcode `0x0A` `NotifyMove`, not `IDSAnim_BindUserData`).

**Agent todo 10 (2026-05-30):** `trackManager` @ `+0x1b0` retyped `byte[72]` → embedded `CDSVideoPlayer` (72 B); decompiler shows `ConstructTrackManager(&this->trackManager)` and `(this->trackManager).pRenderTarget`. **Scalar gap closed:** `reticleRing` ends `+0x190`; eight `int` cursor fields `+0x190..+0x1af` are contiguous (no pad). `CGunMouseCoordQueue` Ghidra names aligned to `m_*`.

## Related types (round 3 task 12)

| Claim | Address | Evidence |
|-------|---------|----------|
| Registry factory alloc `0x218` | `CGunMouse_CreateObject` @ `0x00426500` | `OperatorNewWithBadAlloc(0x218)` → `CGunMouse_ctor`; registered class id **`0x7d1`** @ static init `0x0047c0b0` (DATA xref only) |
| Runtime menu cursor (no factory call) | `CBulanci_InitResourceBank` @ `0x00402286` | Same `0x218` + `CGunMouse_ctor`; stored via `CBulanci_AssignResourceIndexSlot` |
| **Not** `CDSImageMouse` subclass / consumer | compare @ `0x00426060` vs `0x0042af00` | `CGunMouse_ctor` stamps `0x4837c0` / `0x4837ac` / `0x483794` (not `0x486f7c` / `0x486f68`); four embedded `CDSImage`; never calls `CDSImageMouse_CreateObject` |
| Sibling `CDSImageMouse` (`0x88`) | `CDSImageMouse_CreateObject` @ `0x0042af00` | Class id **`0x35`**; one backing image + dirty rect — different size, layout, and vtables (`CDSImageMouse.md`) |

## UNK

- Exact `CDSImage` / `CDSObject` sub-layout sizes (assumed `0x60` per image from ctor spacing `+0x1c` dwords).
- Optional nested `CDSTrackVector` type on `trackManager.pTracks` (see `CDSVideoPlayer.md`).
