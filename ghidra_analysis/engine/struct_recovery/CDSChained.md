# CDSChained

## Status

**VERIFIED** — size `0x68` (104 bytes) for minimal view objects (`CHistoryView`, `CBlackView`, …). Chain-header band `0x40`–`0x50` proven via `CDSChained_ResetChainCounters`; ctor band `0x00`–`0x3c` and tail `0x54`–`0x64` via `CDSChained_ctor` / `CDSChained_InitWithRect@0x0040b560`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Init through `+0x64` | `0x004032d0` | `CDSChained_ctor` zeroes `param_1[0x17..0x19]` → offsets `0x5c..0x64` |
| Same span in rect-init helper | `0x0040b560` | `CDSChained_InitWithRect` zeroes `this+0x5c..+0x64` then calls `ResetChainCounters` |
| `CHistoryView` allocation | `0x00422f00` | `OperatorNew(0x68)` — no tail past `0x64` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CDSChained_ctor@0x004032d0` → `0x47f954` |
| 0x04 | 4 | `void *` | `pVftable_IDSChained` | ctor → `0x47f938` |
| 0x08 | 4 | `uint` | `dwField_08` | ctor zero |
| 0x0C | 4 | `uint` | `dwField_0c` | ctor zero |
| 0x10 | 4 | `void *` | `pVftable_IDSEventHandler` | `CDSEventHandler_ctor(this+0x10)` then ctor overwrites `0x47f920` |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CHistoryView_ctor` `\|= 0x67f@0x00422a70`; `CHelpView_ctor` `\|= 0x77f@0x004218a0` |
| 0x16 | 2 | `ushort` | `wPad_16` | padding between `wViewFlags` and `+0x18` vtable |
| 0x18 | 4 | `void *` | `pVftable_IDSReferenced` | ctor → `g_pCDSView_vftable_IDSReferenced` (`0x47f90c`); `CHistoryView_ctor` overwrites |
| 0x1C | 4 | `uint` | `dwField_1c` | ctor zero |
| 0x20 | 4 | `int` | `bbox_left` | `CDSChained_InitWithRect@0x0040b560` copies `pRect[0]`; `CHistoryView_ctor` `{0,0,0x212,0x1fe}` |
| 0x24 | 4 | `int` | `bbox_top` | same |
| 0x28 | 4 | `int` | `bbox_right` | same |
| 0x2C | 4 | `int` | `bbox_bottom` | same |
| 0x30 | 4 | `int` | `nScreenBbox_left` | `CDSView__UpdateScreenCoordinates@0x0042bf40` copies from `bbox_left`; `CScrollBar_Render@0x004035ad` reads for blit |
| 0x34 | 4 | `int` | `nScreenBbox_top` | same (`bbox_top` / `+0x34`) |
| 0x38 | 4 | `int` | `nScreenBbox_right` | same (`bbox_right` / `+0x38`) |
| 0x3C | 4 | `int` | `nScreenBbox_bottom` | same (`bbox_bottom` / `+0x3c`) |
| 0x40 | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters@0x0042beb0` `= 0` |
| 0x44 | 2 | `ushort` | `wChainInit44` | `ResetChainCounters` `= 1` |
| 0x46 | 2 | `ushort` | `wChainFlag46` | `ResetChainCounters` `= 0`; **bit 0** keyboard-focusable (`CDSView_AcquireKeyboardFocus@0x0042c8f9` tests `&1`; `CWindow_BuildAt@0x004055b3` `\|= 1`; `CHelpView_ctor@0x004218eb` `\|= 1`); `CPauseDlg_Build` `\|= 0x2c` on dialog builds |
| 0x48 | 2 | `ushort` | `wChainFlag48` | `ResetChainCounters` `= 0` |
| 0x4A | 2 | `ushort` | `wChainFlag4a` | `ResetChainCounters` `= 0` |
| 0x4C | 4 | `void *` | `pParent` | `ResetChainCounters` `= 0`; `CHistoryView_OnEvent@0x00422670` |
| 0x50 | 4 | `uint` | `dwField_50` | `ResetChainCounters` `= 0` |
| 0x54 | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | ctor → `g_pCDSChain_vftable_IDSReferenced` |
| 0x58 | 4 | `void *` | `pVftable_CDSChain_IDSChained` | ctor → `g_pCDSChain_vftable_IDSChained` |
| 0x5C | 4 | `uint` | `dwField_5c` | ctor zero |
| 0x60 | 4 | `uint` | `dwField_60` | ctor zero |
| 0x64 | 4 | `void *` | `pOverlapEntity` | `CDSChained_ctor@0x004032d0` zero; `CBulanek_UpdateStateFromParams@0x004178f7` / `CTeleportPoint_OnEvent@0x0041feff` on gameplay views — **not** written on dialog/`CSwitch` paths |

## Leaf functions (view tree)

| Symbol | Address | Role | Evidence |
|--------|---------|------|----------|
| `CDSChained_ctor` | `0x004032d0` | Default header + embedded `CDSChain` vtables @ `+0x54` | Decompile; calls `ResetChainCounters` |
| `CDSChained_InitWithRect` | `0x0040b560` | Same as ctor but copies caller `int[4]` → `bbox_*` | `CHistoryView_ctor@0x00422a70`; `CProgressBar` menu children |
| `CDSChained_ResetChainCounters` | `0x0042beb0` | Zeros `+0x40..+0x50` chain band | Sole writer of `wChainInit44=1` |
| `CDSChained_InitIDSReferencedVtable` | `0x0042f8a0` | Stamps IDSReferenced face on `CDSChain` slice | 2-instruction helper |
| `CDSChained_AppendChild` | `0x0042f9b0` | Intrusive list append (`+0x8` head, `+0x10` count) | `CDSChain_Append@0x0042fb70` callee |
| `CDSChained_ClearChildren` | `0x0042fab0` | Drain list via `CDSChain_RemoveListNode` | `CDSChain_Append` pre-clear |
| `CDSChain_RemoveListNode` | `0x0042f940` | Unlink head / dec count; optional `Release` | `ClearChildren` loop |
| `CDSChained_GetFirstChildView` | `0x0042f7c0` | `*(pChain->pFirstChild + 0xc)` or NULL | `CDSView_SetRect`, `SpatialQuery`, `CDSChain_Remove` |
| `CDSChained_GetNextSiblingView` | `0x0042f920` | Next view via `*(pCurrentView+0xc)`; skips sentinel equal to first child | ECX=`pChain`, stack=`pCurrentView` |

## Ghidra apply

```
get_struct_layout CDSChained → size 0x68 (104), 29 fields
```

Slice **26** (2026-05-30): renamed `FUN_0040b560` → `CDSChained_InitWithRect`; prototyped `CDSChained_InitWithRect`, `CDSChained_ClearChildren`, `CDSChain_RemoveListNode`.

Agent todo **22** (2026-05-30): `CDSChained_GetFirstChildView` / `GetNextSiblingView` @ `0x0042f7c0` / `0x0042f920` — `void * __fastcall GetFirstChildView(CDSChain *pChain)`; `void * __stdcall GetNextSiblingView(void *pCurrentView)` with **ECX** = `pChain` at entry. `CDSChained_ClearChildren` → `void __fastcall ClearChildren(CDSChain *pChain, char bRelease)`. `CDSChain_Remove@0x0042fc30` decompiles as `for` over both walkers.

**Agent todo 21 R4 (2026-05-30):** Renamed `dwField_30..3c` → `nScreenBbox_*` (screen-space blit rect); `dwField_64` → `pOverlapEntity`. `set_decompiler_comment` @ `0x0042bf40`, `0x00403346`. `save_program`.

## UNK

- ~~Semantic names for `dwField_08` / `dwField_0c` / `dwField_1c`~~ — **closed (R5 worker 32):** ctor-zero padding; no leaf consumer ([round5_worker_32_report.md](./round5_worker_32_report.md)).
- **Shared shell:** `CDSAnim` / `CDSBitmap` reuse this `+0x00..+0x67` layout (agent todo **27**, 2026-05-30); anim classes repurpose `+0x20/+0x24` as `nPos_x/y` and stamp anim vtables from `+0x68`.
- `dwChainRoot` / `wChainInit44` linkage semantics (child/sibling chain counters).
- Decompiler still prefixes `_Globals::` on the walker symbols despite global names; `CDSChain_RemoveListNode` may still show `CBulanci::` in call sites until namespace cleanup.
