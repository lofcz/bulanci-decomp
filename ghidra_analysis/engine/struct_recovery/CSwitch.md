# CSwitch

## Status

**PARTIAL** — heap size `0xc8` verified; six vtable faces, embedded `CDSVideoPlayer` track manager @ `+0x7c`, and press/cmd tail @ `+0xc4`/`+0xc6` are instruction-proven. CDSChained drawable shell `+0x28..+0x67` field-named in Ghidra (agent todo **21**, 2026-05-30); `Button_Click` flat `+0xa8`/`+0xb4` reads resolve to `trackManager.nCurrentTrackIdx` / `dwCurrentFrameIdx`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CSwitch) == 0xc8` | `CMenu` build @ `0x00425900` region | Three× `OperatorNewWithBadAlloc(200)` → `CBulanci::CSwitch_ctor` (`bulanci.ghidra.exe.c` ~95908) |
| Same size (decor path) | `0x004452bc` region | `OperatorNewWithBadAlloc(200)` @ `0x004452bc` (`94576`) |
| Tail ends at `+0xc7` | `CSwitch_ctor@0x00424bc0` | `*(this+0xc4)=0`; `*(ushort*)(this+0xc6)=cmd` |
| Track manager span | `CSwitch_ctor@0x00424bc0` | `ConstructTrackManager(this+0x7c)`; `CDSVideoPlayer` is `0x48` → `0x7c+0x48=0xc4` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CSwitch_ctor@0x00424bc0` → `g_pCSwitch_vftable_primary` |
| 0x04 | 4 | `void *` | `pVftable_IDSChained` | same → `g_pCSwitch_vftable_chain` |
| 0x10 | 4 | `void *` | `pVftable_IDSEventHandler` | same → `g_pCSwitch_vftable_event` |
| 0x18 | 4 | `void *` | `pVftable_IDSReferenced` | same → `g_pCSwitch_vftable_ref` |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CSwitch_ctor` `\|= 0x238` on `*(ushort*)(this+0x14)` |
| 0x20 | 4 | `int` | `nBbox_left` | `CSwitch_ctor` `= param_1` (x); mirrors `CDSChained` |
| 0x24 | 4 | `int` | `nBbox_top` | `CSwitch_ctor` `= param_2` (y) |
| 0x28 | 4 | `int` | `nBbox_right` | `CDSChained` shell parity |
| 0x2C | 4 | `int` | `nBbox_bottom` | same |
| 0x30 | 16 | `uint`×4 | `dwField_30`…`dwField_3c` | ctor zero band; `CDSChained` names |
| 0x40 | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters@0x0042beb0` |
| 0x44 | 1 | `byte` | `wChainInit44` | `Button_Click` `(this+0x44)&5==1` — low byte of chain init ushort |
| 0x46 | 6 | `ushort`×3 | `wChainFlag46`…`wChainFlag4a` | `ResetChainCounters` |
| 0x4C | 4 | `void *` | `pParent` | `Button_Click` posts to `*(this+0x4c)+0x10` |
| 0x50 | 4 | `uint` | `dwField_50` | chain band |
| 0x54 | 8 | `void *`×2 | `pVftable_CDSChain_*` | embedded `CDSChain` MI @ `+0x54`/`+0x58` |
| 0x5C | 12 | `uint`×3 | `dwField_5c`…`dwField_64` | ctor zero tail |
| 0x68 | 4 | `void *` | `pVftable_IDSUpdated` | `CSwitch_ctor`; `CSwitch_dtor@0x00423950` restore |
| 0x6C | 4 | `void *` | `pVftable_IDSAnim` | `CSwitch_ctor`; `OnAnimEnd` uses `this-0x6c` base |
| 0x78 | 1 | `byte` | `bType` | `CSwitch_ctor` `this+0x78 = param_3` (`1` = momentary menu button) |
| 0x7C | 0x48 | `CDSVideoPlayer` | `trackManager` | `ConstructTrackManager@0x00439c70`; `CSwitch_dtor` `CDSVideoPlayer_TM_Destructor(this+0x7c)` |
| 0xC4 | 1 | `byte` | `bPressedState` | `CSwitch_ctor` zero; `Button_Click` `=1`; `OnSiblingPressed@0x00424dc0` clear |
| 0xC6 | 2 | `ushort` | `wCmd` | `CSwitch_ctor`; `Button_Click` `Scheduler_PostMessage(..., cmd@+0xc6)` |

## Vtable cluster (primary @ `0x004834d4`)

| Slot | Address | Role |
|------|---------|------|
| 14 | `0x0040b840` | `Tick` → `TM_TickBlit` on anim face |
| 17–20 | `0x00424cf0`…`0x00425260` | hover / focus / click |
| 27 | `0x00424dc0` | `OnSiblingPressed` — `msg==200`, same `bType` (`+0x78`) |

See `main_menu.md` §9.1 and `master_vtable_catalog.csv` (`CSwitch` rows).

## Lifecycle

| Path | Evidence |
|------|----------|
| Main menu buttons | `CMenu` ctor region — `OperatorNew(0xc8)` ×3 @ `(0x23,0x25|0x79|0xcd)`, cmds `0xc9`/`0xca`/`0xcb` |
| Shell base | `ODSImage::CDSBitmap__CDSBitmap_SubobjectCtor@0x004228f0` then CSwitch vtables (not full `CDSBitmap_ctor`) |
| Click routing | `_Globals::Button_Click@0x00424d30` → parent `OnEvent` → `CMenu_CmdDispatch@0x00425970` |

## Ghidra apply

```
delete_data_type CSwitch
create_struct CSwitch (vtable prefix + CDSVideoPlayer@0x7c + bState/wCmd tail)
set_function_prototype 0x00424bc0 → CSwitch * CSwitch_ctor(CSwitch *this, int x, int y, byte type, ushort cmd)
get_struct_layout CSwitch → size 200 (0xc8); trackManager → CDSVideoPlayer @ +124 (0x7c)
set_function_prototype CSwitch_ctor@0x00424bc0 → CSwitch * CSwitch_ctor(CSwitch *, int x, int y, uchar type, ushort cmd)
save_program bulanci.exe
```

Applied 2026-05-30 slice 20.

**Agent todo 21 (2026-05-30):** `get_struct_layout CSwitch` — shell `+0x30..+0x64` mirrors `CDSChained` (`dwField_30..3c`, `dwChainRoot`, `wChainInit44` ushort @ `+0x44`, `wChainFlag*`, `pParent`, `dwField_50`, CDSChain MI, `dwField_5c..64`). `bChainInit44`→`wChainInit44` (ushort). `set_function_prototype` `CSwitch_ctor@0x00424bc0`, `Button_Click@0x00424d30` (`CSwitch *`); decompiler shows `trackManager.nCurrentTrackIdx` / `dwCurrentFrameIdx`. Comments @ `0x0042beb0`, `0x00424d30`. `save_program`.

## UNK

- Semantic names for `dwField_30`…`dwField_3c` / `dwField_50` / `dwField_5c`…`64` beyond ctor zero (shared with `CDSChained` — see `CDSChained.md`).
- `wChainInit44` stored as **byte** in Ghidra layout (1 B @ `+0x44`); live `ResetChainCounters` writes **ushort** `=1` — decompiler uses `bChainInit44`/`wChainInit44` interchangeably for `(flags & 5)==1`.
- `OnAnimEnd` hover test uses anim-subobject-relative `+0x58` (maps to host `+0xc4` `bState` when rebased).
- Decor / non-menu `CSwitch` instances (if any) not exhaustively xrefs’d beyond menu alloc sites.
