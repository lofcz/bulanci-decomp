# CMsgDialog

## Status

**PARTIAL** — heap size `0x70` verified; full `CWindow` / `CDSChained` dialog base including chain band `0x40–0x50` from `CDSChained_ResetChainCounters`. No game-specific tail past `CWindow`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CMsgDialog) == 0x70` | `0x0040e780` | `CMsgDialog_Allocate` → `OperatorNewWithBadAlloc(0x70)` |
| Stack instance same size | `0x004137b0`, `0x00413a40` | `CMsgDialog local_7c [112]` / ctor on `0x70`-byte object |
| `CWindow` base ~`0x70` | `0x00405560` | `CWindow_BuildAt` comment: layout size ~`0x70` before subclass extension |
| Default-focus slot cleared | `0x0040e780` | `CMsgDialog_Allocate`: `puVar1[0x1b] = 0` → offset `0x6c` |

## Class registry (CDS engine — not MSVC RTTI)

Vtable slot 0 returns a **`ClassRegEntry`** blob (`static_texts.md`, 20 B). There is no `.?AVCMsgDialog@@` / COL record in the binary; batch “RTTI” wording means this registry.

| Claim | Address | Evidence |
|-------|---------|----------|
| `GetClassTable` → registry blob | `CMsgDialog_GetClassTable@0x0040b760` | `return &g_CMsgDialog_ClassRegEntry` (`0x004b3524`) |
| **classId = 2021 (`0x7e5`)** | `CMsgDialog_StaticClassRegister@0x0047b5c0` | `PUSH 0x7e5` before `HandleClassRegister@0x0042e910` |
| Factory | `CMsgDialog_Allocate@0x0040e780` | `PUSH 0x40e780` @ `0x0047b5c0`; `OperatorNew(0x70)` + vtable install |
| Parent meta | `CWindow_GetTypeDescriptor@0x004049b0` | `PUSH 0x4b335c` @ `0x0047b5c5` → `g_CWindow_ClassRegEntry` |
| Registry storage | `g_CMsgDialog_ClassRegEntry@0x004b3524` | Ghidra `ClassRegEntry` (20 B); image zeros until CRT static init |
| `ClassRegEntry.class_id` | `g_CMsgDialog_ClassRegEntry+0x08` | Set by `HandleClassRegister` from `PUSH 0x7e5` |
| `ClassRegEntry.factory` | `g_CMsgDialog_ClassRegEntry+0x0c` | `CMsgDialog_Allocate` |
| `ClassRegEntry.parent_meta_ptr` | `g_CMsgDialog_ClassRegEntry+0x04` | `&g_CWindow_ClassRegEntry` |

Runtime lookup: `g_apClassByIdTable[2021]` via `HandleClassRegister` / `InitializeByClassId@0x0042ef00`. Live UI still uses stack `CMsgDialog_ctor` (`CMenu_ShowConnectingDialog@0x004137b0`), not the factory.

**Sibling pause dialog:** `CPauseDlg` uses the same parent meta but classId **`0x808` (2056)** in the `0x805`..`0x80c` dialog-widget band (`CPauseDlg_StaticClassRegister@0x0047b920`) — see [CPauseDlg.md](./CPauseDlg.md).

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CMsgDialog_ctor@0x0040be80` → `0x480844`; `CMsgDialog_Allocate@0x0040e780` |
| 0x04 | 4 | `void *` | `pVftable_IDSChained` | ctor → `0x480824` |
| 0x08 | 4 | `dword` | `dwField_08` | `CDSChained_ctor@0x004032d0` |
| 0x0C | 4 | `dword` | `dwField_0c` | `CDSChained_ctor@0x004032d0` |
| 0x10 | 4 | `void *` | `pVftable_IDSEventHandler` | ctor → `0x48080c` |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CWindow_BuildAt@0x00405560` `\|= 0x77f` |
| 0x18 | 4 | `void *` | `pVftable_field18` | ctor → `0x4807f8` |
| 0x1C | 4 | `dword` | `dwField_1c` | `CDSChained_ctor@0x004032d0` |
| 0x20 | 4 | `int` | `nBbox_left` | `CWindow_BuildAt` + ctor resize `@0x0040be80` |
| 0x24 | 4 | `int` | `nBbox_top` | same |
| 0x28 | 4 | `int` | `nBbox_right` | ctor writes `this+0x28` `@0x0040be80` |
| 0x2C | 4 | `int` | `nBbox_bottom` | ctor writes `this+0x2c` `@0x0040be80` |
| 0x30 | 4 | `dword` | `dwField_30` | `CDSChained_ctor@0x004032d0` |
| 0x34 | 4 | `dword` | `dwField_34` | `CDSChained_ctor@0x004032d0` |
| 0x38 | 4 | `dword` | `dwField_38` | `CDSChained_ctor@0x004032d0` |
| 0x3C | 4 | `dword` | `dwField_3c` | `CDSChained_ctor@0x004032d0` |
| 0x40 | 4 | `dword` | `dwChainHead_40` | `CDSChained_ResetChainCounters@0x0042beb0` `*(this+0x40)=0` (via `CDSChained_ctor@0x004032d0` in `CWindow_BuildAt`) |
| 0x44 | 2 | `ushort` | `wViewStateFlags` | `CDSChained_ResetChainCounters@0x0042beb0` `=1` |
| 0x46 | 2 | `ushort` | `wWidgetFlags` | `CDSChained_ResetChainCounters@0x0042beb0` `=0`; `CWindow_BuildAt@0x00405560` `\|= 1`; `CMsgDialog_ctor@0x0040be80` `\|= 0xc` |
| 0x48 | 2 | `ushort` | `wChainCounter_48` | `CDSChained_ResetChainCounters@0x0042beb0` `=0` |
| 0x4A | 2 | `ushort` | `wChainCounter_4a` | `CDSChained_ResetChainCounters@0x0042beb0` `=0` |
| 0x4C | 4 | `void *` | `pParent` | `CDSChained_ResetChainCounters@0x0042beb0` `=0` (no `CMsgDialog` method reads) |
| 0x50 | 4 | `dword` | `dwChainField_50` | `CDSChained_ResetChainCounters@0x0042beb0` `=0` |
| 0x54 | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor@0x004032d0` |
| 0x58 | 4 | `void *` | `pVftable_CDSChain_IDSChained` | `CDSChained_ctor@0x004032d0` |
| 0x5C | 4 | `dword` | `dwField_5c` | `CDSChained_ctor@0x004032d0` |
| 0x60 | 4 | `dword` | `dwField_60` | `CDSChained_ctor@0x004032d0` |
| 0x64 | 4 | `dword` | `dwField_64` | `CDSChained_ctor@0x004032d0` |
| 0x68 | 1 | `byte` | `bModalFlag` | `CWindow_BuildAt@0x00405560` (`modalFlag` arg) |
| 0x6C | 4 | `void *` | `pDefaultFocusChild` | `CMsgDialog_Allocate@0x0040e780` clears index `0x1b` |

## Leaf functions (vtable / named)

| Symbol | Address | Role |
|--------|---------|------|
| `CMsgDialog_ctor` | `0x0040be80` | Stack/heap init: `CWindow_BuildAt`, title `CStaticText`, mode button; xrefs `CMenu_ShowConnectingDialog@0x004137fa`, `FUN_0040f2c0@0x0040f30f` |
| `CMsgDialog_Allocate` | `0x0040e780` | Factory: `OperatorNew(0x70)` + vtables; clears `pDefaultFocusChild` |
| `CMsgDialog_GetClassTable` | `0x0040b760` | Returns `&g_CMsgDialog_ClassRegEntry` |
| `CMsgDialog_StaticClassRegister` | `0x0047b5c0` | CRT: classId `0x7e5`, parent `CWindow` meta |
| `CMenu_ShowConnectingDialog` | `0x004137b0` | Stack `CMsgDialog local_7c` → modal via `CMenu_DoModalChild` |

Inherited `CWindow` handlers (paint, key, command) use primary vtable `0x480844` — no additional `CMsgDialog_*` instance methods beyond ctor.

## Ghidra apply

```
get_struct_layout CMsgDialog → size 0x70 (112)
modify_struct_field chain band: dwChainHead_40, wViewStateFlags, wWidgetFlags, wChainCounter_48/4a, pParent, dwChainField_50
set_function_prototype CMsgDialog_ctor@0x0040be80 → (CMsgDialog *this, int cdsStringHandle, undefined4 *pButtonMode)
```

Applied in batch 13 via inline script (clone of dialog base + `CWindow` tail). Round 3 task 16: `ClassRegEntry` on `g_CMsgDialog_ClassRegEntry@0x004b3524`.

**Slice 15 (2026-05-30):** Verified layout matches doc; renamed CDS chain-band fields; `CMsgDialog_ctor` prototype; `save_program bulanci.exe`.

## UNK

- Semantics of `wChainCounter_48` / `wChainCounter_4a` (ctor-zeroed only).
- No in-tree overlay resource confirmed to deserialize `classId 2021` (registration present; menu uses direct ctor).

## Follow-up (round 3)

- **Task 5 / 16** — `CMsgDialog_GetClassTable` → `g_CMsgDialog_ClassRegEntry`; classId **2021 / `0x7e5`** decoded from static init; Ghidra `ClassRegEntry` @ `0x004b3524`.
- **Task 6** — distinguished CDS registry vs MSVC RTTI; cross-linked `CPauseDlg` **`0x808`** tier; Ghidra `CMsgDialog_StaticClassRegister@0x0047b5c0`.
