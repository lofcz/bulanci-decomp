# CPauseDlg

## Status

**PARTIAL** — heap size `0x7c` verified; `CWindow` base (chain band `0x40–0x50` proven) plus `pGame` / two button pointers at `+0x70..+0x78`. Heap instance owned by `CGaming+0x334`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CPauseDlg) == 0x7c` | `0x0040f090` | `CPauseDlg_Allocate` → `OperatorNewWithBadAlloc(0x7c)` |
| Tail `0x70`..`0x78` written in build | `0x00411df0` | `CPauseDlg_Build` stores `param_1` @ `+0x70`, buttons @ `+0x74`, `+0x78` |
| Extends `CWindow` (`0x70`) + `0xc` | — | `0x70 + 0xc == 0x7c` matches alloc |

## Class registry

| Claim | Address | Evidence |
|-------|---------|----------|
| `GetClassTable` → registry blob | `CPauseDlg_GetClassTable@0x0040be10` | `return &g_CPauseDlg_classMeta` (`0x004b367c`); primary vtable slot 0 |
| **classId = 2056 (`0x808`)** | static init `@0x0047b92a` | `PUSH 0x808` before `HandleClassRegister@0x0042e910` in `CPauseDlg_StaticClassRegister@0x0047b920` |
| Factory | `CPauseDlg_Allocate@0x0040f090` | `PUSH 0x40f090` @ `0x0047b920`; `OperatorNew(0x7c)` + `CDSChained_ctor` + vtables (no `Build`) |
| Parent meta | `CWindow_GetTypeDescriptor@0x004049b0` | `PUSH 0x4b335c` @ `0x0047b925` (shared `CWindow` `ClassRegEntry`, not `CListBox` @ `0x4b33e0`) |
| Registry storage | `g_CPauseDlg_classMeta@0x004b367c` | 20-byte `ClassRegEntry`; zero in `.data` until CRT static init fills via `HandleClassRegister` |
| Neighbor ids (dialog tier) | `0x0047b8fa`, `0x0047b94a`… | `CChatList` **0x807** @ `0x0047b8fa`; `CSetupDlg` **0x80a** @ next stub — sequential UI class ids |

Runtime lookup: `g_apClassByIdTable[2056]` via `HandleClassRegister` / `InitializeByClassId@0x0042ef00` (`static_texts.md`). In-match pause UI uses `CGaming_ctor` → `CPauseDlg_Build` (not `InitializeByClassId(0x808)`).

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CPauseDlg_Build@0x00411df0` → `g_pCPauseDlg_vftable_primary`; `CPauseDlg_Allocate@0x0040f090` |
| 0x04 | 4 | `void *` | `pVftable_IDSChained` | `CPauseDlg_Build` → `g_pCPauseDlg_vftable_IDSChained` |
| 0x08 | 4 | `dword` | `dwField_08` | `CDSChained_ctor@0x004032d0` (via `CWindow_BuildAt`) |
| 0x0C | 4 | `dword` | `dwField_0c` | `CDSChained_ctor@0x004032d0` |
| 0x10 | 4 | `void *` | `pVftable_IDSEventHandler` | `CPauseDlg_Build` → `CPauseDlg::vftable` |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CWindow_BuildAt@0x00405560` `\|= 0x77f` |
| 0x18 | 4 | `void *` | `pVftable_field18` | `CPauseDlg_Build` second view vtable |
| 0x1C | 4 | `dword` | `dwField_1c` | `CDSChained_ctor@0x004032d0` |
| 0x20 | 4 | `int` | `nBbox_left` | `CWindow_BuildAt` rect `(0,0,0x132,0x5a)` `@0x00411df0` |
| 0x24 | 4 | `int` | `nBbox_top` | same |
| 0x28 | 4 | `int` | `nBbox_right` | same (`0x132` width) |
| 0x2C | 4 | `int` | `nBbox_bottom` | `CPauseDlg_Build` may add `+200` to bottom when lobby chat `@0x00411df0` |
| 0x30 | 4 | `dword` | `dwField_30` | `CDSChained_ctor@0x004032d0` |
| 0x34 | 4 | `dword` | `dwField_34` | `CDSChained_ctor@0x004032d0` |
| 0x38 | 4 | `dword` | `dwField_38` | `CDSChained_ctor@0x004032d0` |
| 0x3C | 4 | `dword` | `dwField_3c` | `CDSChained_ctor@0x004032d0` |
| 0x40 | 4 | `dword` | `dwChainHead_40` | `CDSChained_ResetChainCounters@0x0042beb0` `*(this+0x40)=0` (via `CDSChained_ctor@0x004032d0`) |
| 0x44 | 2 | `ushort` | `wViewStateFlags` | `CDSChained_ResetChainCounters@0x0042beb0` `=1`; `CPauseDlg_OnNotify@0x0040b2f0` tests low byte bit 0 (`CDSView__Show`/`Hide` self) |
| 0x46 | 2 | `ushort` | `wWidgetFlags` | `CDSChained_ResetChainCounters@0x0042beb0` `=0`; `CWindow_BuildAt@0x00405560` `\|= 1`; `CPauseDlg_Build@0x00411df0` `\|= 0x2c` |
| 0x48 | 2 | `ushort` | `wChainCounter_48` | `CDSChained_ResetChainCounters@0x0042beb0` `=0` |
| 0x4A | 2 | `ushort` | `wChainCounter_4a` | `CDSChained_ResetChainCounters@0x0042beb0` `=0` |
| 0x4C | 4 | `void *` | `pParent` | `CDSChained_ResetChainCounters@0x0042beb0` `=0`; `CPauseDlg_OnCommand@0x0040b410` `*(this+0x4c)+0x10`; `CPauseDlg_OnNotify@0x0040b2f0` |
| 0x50 | 4 | `dword` | `dwChainField_50` | `CDSChained_ResetChainCounters@0x0042beb0` `*(this+0x50)=0` |
| 0x54 | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor@0x004032d0` |
| 0x58 | 4 | `void *` | `pVftable_CDSChain_IDSChained` | `CDSChained_ctor@0x004032d0` |
| 0x5C | 4 | `dword` | `dwField_5c` | `CDSChained_ctor@0x004032d0` |
| 0x60 | 4 | `dword` | `dwField_60` | `CDSChained_ctor@0x004032d0` |
| 0x64 | 4 | `dword` | `dwField_64` | `CDSChained_ctor@0x004032d0` |
| 0x68 | 1 | `byte` | `bModalFlag` | `CWindow_BuildAt@0x00405560` (`'\x01'` in `CPauseDlg_Build`) |
| 0x6C | 4 | `void *` | `pDefaultFocusChild` | `CPauseDlg_Allocate@0x0040f090` clears `puVar1[0x1b]` |
| 0x70 | 4 | `CGame *` | `pGame` | `CPauseDlg_Build@0x00411df0` stores `CGame*` from `CGaming_ctor`; `CPauseDlg_OnKeyDown@0x0040ac20` `CGame_NetSendKick_t0a`; `OnNotify@0x0040b2f0` `pGame->bHostSlotCursor` / `pGame->bLocalSenderSlot`; `Build` widens when `pGame->pRecvBuf` (`+0x1dc`) non-null |
| 0x74 | 4 | `void *` | `pBtnPrimary` | `CPauseDlg_Build@0x00411df0`; `CPauseDlg_OnNotify@0x0040b2f0` show/hide/focus |
| 0x78 | 4 | `void *` | `pBtnSecondary` | `CPauseDlg_Build@0x00411df0`; `CPauseDlg_OnNotify@0x0040b2f0` |

## `pGame` target (`CGame`, `0x248` bytes)

| `CGame` offset | Name | Evidence |
|----------------|------|----------|
| `+0xda` | `bHostSlotCursor` | `CPauseDlg_OnNotify@0x0040b375` `this->pGame->bHostSlotCursor` loop bound |
| `+0xdb` | `bLocalSenderSlot` | `CPauseDlg_OnNotify@0x0040b2f0` compares notify `param_2` to `this->pGame->bLocalSenderSlot` |
| `+0x1dc` | `pRecvBuf` | `CPauseDlg_Build@0x00411df0` non-null → lobby chat panel; `CGame_ctor@0x00414a80` `param_1[0x77]=0` |

Pointer is the **embedded** game at `CBulanci+0x284` (`0x248` span — [CGame.md](./CGame.md), [CBulanci.md](./CBulanci.md)), passed from `CGaming_ctor@0x00420423` via the gaming owner slot (`CGame*`, not `CBulanci*` base).

## Leaf functions

| Symbol | Address | Role |
|--------|---------|------|
| `CPauseDlg_Build` | `0x00411df0` | Gameplay ctor: `CWindow_BuildAt`, `pGame` @ `+0x70`, buttons @ `+0x74`/`+0x78`; sole code xref `CGaming_ctor@0x00420423` |
| `CPauseDlg_OnNotify` | `0x0040b2f0` | Net notify `0xe9`: slot vs `pGame->bLocalSenderSlot`, host cursor loop, button show/hide |
| `CPauseDlg_OnCommand` | `0x0040b410` | `0xea`/`0xeb` → `CGame_NetSendKick_t0a(pGame,…)`; routes via `pParent+0x10` |
| `CPauseDlg_OnKeyDown` | `0x0040ac20` | Esc → `CGame_NetSendKick_t0a(pGame,0)` |
| `CPauseDlg_Allocate` | `0x0040f090` | Factory-only (`StaticClassRegister`); no `Build` |
| `CPauseDlg_GetClassTable` | `0x0040be10` | Returns `&g_CPauseDlg_classMeta` |
| `CPauseDlg_StaticClassRegister` | `0x0047b920` | CRT: classId `0x808`, parent `CWindow` meta |

## Ghidra apply

```
get_struct_layout CPauseDlg → size 0x7c (124); pGame @ +0x70 → CGame *; pBtnPrimary/Secondary → CButton *
get_struct_layout CGame → size 0x248 (584)
set_function_this_type CPauseDlg_Build@0x00411df0 → `CPauseDlg *` (R3 task 16 — fixes ECX `this`)
set_function_prototype CPauseDlg_Build@0x00411df0 → `CPauseDlg * __thiscall CPauseDlg_Build(CPauseDlg *this, CGame *pGame)`
force_decompile @0x00411df0
modify_struct_field CPauseDlg.pGame → `CGame *` (was `-BAD-`)
set_plate_comment + set_decompiler_comment @0x00411df0, call-site @0x00420423
save_program bulanci.exe
modify_struct_field chain band + tail button types (slice 15)
delete_data_type CGame_embedded   # superseded by CGame @ 0x248 (agent todo 12)
```

Round 3 task 23: `CPauseDlg.pGame` → **`CGame *`**; `CPauseDlg_OnNotify` uses `pGame->bHostSlotCursor` / `bLocalSenderSlot`. Interim `CGame_embedded` removed after canonical `CGame` layout (task 1 + 23).

**Slice 15 (2026-05-30):** Chain-band field names aligned with `CDSChained_ResetChainCounters`; `pBtnPrimary`/`pBtnSecondary` typed `CButton *`; `CPauseDlg_OnNotify` decompiles with `pGame`/`pBtn*`/`wViewStateFlags`; `save_program bulanci.exe`.

**R3 task 16 (2026-05-30):** `set_function_this_type` `CPauseDlg_Build@0x00411df0` → **`CPauseDlg *`** (moves fn into class `CPauseDlg`); `set_function_prototype` + `force_decompile`. Decompiler signature: `CPauseDlg::CPauseDlg_Build(CPauseDlg *this, CGame *pGame)`; body uses `this->pVftable_*`, `pBtnPrimary`/`pBtnSecondary`, `nBbox_bottom`, `wWidgetFlags` (not `field_0x*` / `CBulanci *this`). Plate/asm EOL retained @ call `0x00420423`.

## Lifecycle (heap vs factory)

| Path | Caller | Init | Xrefs to allocator/build |
|------|--------|------|--------------------------|
| **Gameplay (in-match UI)** | `CGaming_ctor@0x00420380` | `OperatorNew(0x7c)` → `CPauseDlg_Build@0x00411df0` (`CWindow_BuildAt`, buttons, `pGame` @ `+0x70`) | `CPauseDlg_Build`: **1 code xref** — `CGaming_ctor@0x00420423` |
| **CDS class factory** | `HandleClassRegister@0x0042e910` via static init | `CPauseDlg_Allocate@0x0040f090` — `OperatorNew(0x7c)` + `CDSChained_ctor` + four vtable writes + `pDefaultFocusChild=0`; **no** `CWindow_BuildAt`, **no** `pGame`/buttons | `CPauseDlg_Allocate`: **1 xref total** — DATA push in `CPauseDlg_StaticClassRegister@0x0047b920` |
| Not embedded | — | Pause dialog is a **heap sibling** of `CGaming` (`CGaming+0x334`), not a subobject inside `CBulanci` | — (task 16: `CPauseDlg::CPauseDlg_Build`) |

`CMsgDialog` is registered separately at classId **`0x7e5` (2021)** — app UI pack tier, not the `0x805`..`0x80c` band in **Class registry** above. Both use CDS `ClassRegEntry` blobs (vtable slot 0), not MSVC `type_info` ([round3_task_05_report.md](./round3_task_05_report.md)).

## Follow-up (round 3)

- **Task 7:** Factory vs gameplay — `CPauseDlg_Allocate` registration-only; live UI via `CPauseDlg_Build` from `CGaming_ctor`.
- **Task 17:** classId **`0x808` (2056)** and `ClassRegEntry` @ `g_CPauseDlg_classMeta` proven; Ghidra typed blob as `ClassRegEntry`.
- **Task 6:** CDS registry vs MSVC RTTI; tier note vs `CMsgDialog` `0x7e5`.
- **Task 23:** `pGame` → **`CGame *`** @ `+0x70` ([round3_task_23_report.md](./round3_task_23_report.md)); canonical **`CGame`** layout **`0x248`** in [CGame.md](./CGame.md).

## UNK

- Semantics of `wChainCounter_48` / `wChainCounter_4a` (zeroed at ctor; no `CPauseDlg`-local readers).
- Extra pause buttons in `CPauseDlg_Build` are heap children via `CDSView__AddChild` (not tail fields past `+0x78`).
- **`CPauseDlg_Build` `pGame` field name** — decompile may show `field32_0x70` until `CPauseDlg.pGame` component rename sticks; offset `+0x70` and `CGame *` type are correct (task 21/23).
- Whether any shipped overlay / stream calls `InitializeByClassId(2056)` (factory registered; gameplay uses `CPauseDlg_Build`).
