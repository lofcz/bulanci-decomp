# Round 10 deep — Task 14 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 14 |
| **title** | Struct deep: CMenu UI dispatch |
| **kind** | struct |
| **struct_name** | `CMenu` |
| **seed_address** | `0x0042d160` |
| **addresses** | `0x0042d160`, `0x0042d200`, `0x0042d280` |
| **acceptance** | Menu event routing; prove vtable slots and owner pointer offsets |

## Status

**DONE** — CMenu primary-vtable dispatch slots (22/26/27), `IDSEventHandler` routing, and shell owner-pointer offsets proven via live Ghidra decompile + disasm, correlated with `bulanci.ida.exe.c`. Seed `0x0042d160` is `CMenu_DetachChildWithVisibility`; `0x0042d200` / `0x0042d280` are **intra-`CDSView_DoModal`** landmarks on the menu modal pump (not separate symbols).

## Functions / Struct

### A. Event routing graph (UI `CMenu`)

```
Button_Click / PostMessage(0x100, cmd)
  → Scheduler → CDSView_DispatchEvent(IDSEventHandler+0x10)
      wMsg_id==0x100 → primary vfn[26]  → CMenu_CmdDispatch@0x00425970
      wMsg_id==0x200 → primary vfn[27]  → CMenu_OnEvent@0x00424f50
      wMsg_id==0x400 → primary vfn[25]  → CDSApp_BroadcastSyntheticEventToChildren (base)

CBulanci_OnEvent_MenuStateMachine case 0xcc
  → CMenu_ctor_with_ui → CDSView_DoModal(menu, app)@0x0042d1a0
      inner pump until +0x4a exit code set (e.g. CMenu_OnEvent(1) via DispatchHotkey)
```

Sub-screens (`CStartGame1` / `CHistoryDlg` / `CExitDlg`) are **child views** via `CDSView__AddChild` — the outer `CMenu` `DoModal` keeps running ([`pass_r4_CMenu_report.md`](../struct_recovery/pass_r4_CMenu_report.md)).

### B. `g_pCMenu_vftable_primary` @ `0x004836b4` (28 slots, 112 B)

Read via Ghidra `read_memory@0x004836b4`. CMenu overrides relative to base `g_pCDSView_vftable_primary` (`0x0047f954`):

| Slot | Off | Function | Address | Evidence |
|-----:|----:|----------|---------|----------|
| 0 | `+0x00` | `CMenu_GetClassTable` | `0x004245b0` | `CMenu_ctor@0x00424562` MOV `[ESI],0x4836b4` |
| 1 | `+0x04` | `CMenu_DtorScalar` | `0x00425240` | vtable dump slot 1 |
| 22 | `+0x58` | `CMenu_OnKeyDown` | `0x00424fa0` | vtable dump; `S/H/K/X` hotkeys |
| 26 | `+0x68` | **`CMenu_CmdDispatch`** | `0x00425970` | byte pattern `70 59 42 00` @ `0x0048371c`; IDA `sub_425970` |
| 27 | `+0x6c` | **`CMenu_OnEvent`** | `0x00424f50` | byte pattern `50 4f 42 00` @ `0x00483720`; audio-complete → `EndModal` |

Slots 9/13 used by menu modal pump (shared CDSView implementations):

| Slot | Off | Function | Address | Use in menu path |
|-----:|----:|----------|---------|------------------|
| 9 | `+0x24` | `CDSView_InvalidateRectClipped` | `0x0042ca30` | `DoModal` before/after pump (`0x0042d22d`, `0x0042d27a`) |
| 13 | `+0x34` | `CDSView_IsModalDoneRecursive` | `0x0042c3e0` | `DoModal` stay-modal filter when `+0x46 & 0x40` (`0x0042d268..0x0042d278`) |

`CDSView_DispatchEvent@0x0042c040` resolves primary via `this-0x10` from `IDSEventHandler` face @ `+0x10` → slot 26/27 on `CMenu`.

### C. Seed band — three task addresses

| Address | Symbol / context | Role | Evidence |
|---------|------------------|------|----------|
| **`0x0042d160`** | `CMenu_DetachChildWithVisibility` | Detach child with focus/hide preserve: `(child+0x44)&2` → `ReleaseKeyboardFocus`; `&1` → `Hide`; `CMenu_DetachChild`; restore `Show` | Ghidra decompile; IDA `sub_42D160`: `a2[34]` = `+0x44` (`34×2` ushort index); xrefs: `CHelpDlg_LoadHelpPage`, `CHistoryDlg_LoadHistoryPage`, `CMenu_CloseCurrentSubScreen`, `CMenu_LoadBackgroundResource`, `CDSView_DoModal` epilogue |
| **`0x0042d200`** | inside `CDSView_DoModal` | `MOV EBP,[parent+0x50]` — stash **prior keyboard-focus child** from parent before modal takes focus | Disasm `0x0042d1f8..0x0042d203`; IDA `sub_42D1A0`: `*(_BYTE **)(v7+80)` where `v7=*(_DWORD*)(a1+76)` = `this+0x4c` parent, `+0x50` = `0x50` |
| **`0x0042d280`** | inside `CDSView_DoModal` | Second **`vfn+0x24`** invalidate after modal loop (pairs with `0x0042d22d`) | Disasm `0x0042d27a..0x0042d285`; IDA `(*(_DWORD*)a1+36)` = slot 9 |

### D. Owner / shell pointer offsets (`CMenu` / `CWindow` prefix)

| Offset | Name | Menu dispatch use | Proof |
|--------|------|-------------------|-------|
| `+0x10` | `pVftable_IDSEventHandler` | Posted-message target; `DispatchHotkey` arms audio player with `&this+0x10` | `CMenu_DispatchHotkey` decompile; `TriggerBankSample` event target |
| `+0x44` | `wViewStateFlags` | bit0 visible, bit2 keyboard-focus, bit0x10 modal-active; `DetachChildWithVisibility` tests child flags | `0x0042d160` / IDA `a2[34]`; `DoModal` OR/CLEAR `0x10` |
| `+0x46` | `wWidgetFlags` | `&0x40` → stay-modal outer loop calls `vfn+0x34` | `DoModal@0x0042d262` |
| `+0x4a` | modal exit code | `DoModal` inner loop until non-zero; `CMenu_OnEvent(1)` writes via `CDSView__EndModal` | IDA `*(_WORD*)(a1+74)`; Ghidra `wChainCounter_4a` (same offset, modal semantics) |
| `+0x4c` | `pParent` | `DoModal` eligibility: parent `+0x44 & 0x80` anchor flag | `0x0042d1c7`, IDA `a1+76` |
| `+0x50` | `dwChainField_50` | Parent's keyboard-focus child saved/restored across modal | **`0x0042d200`**, restored `0x0042d2a6` |
| `+0x54` | child chain head | `CMenu_DetachChild` unlinks via `CDSChain_RemoveListNode` | `CMenu_DetachChild@0x0042c000` |
| `+0x80` | `nPushedSubScreenId` | Sub-screen index `0/1/2/-1` | `CMenu_CmdDispatch` / IDA `this+128` |
| `+0x84` | `pCurSubScreen` | Active child dialog pointer | IDA `this+132`; `CloseCurrentSubScreen` |
| `+0xcc` | `wModalExitCodeStash` | `DispatchHotkey` stash → `OnEvent(1)` → `EndModal` | `CMenu_DispatchHotkey` / `CMenu_OnEvent` |
| `+0xc4` | `pDeferredExitVoicePlayer` | Audio-cued exit; released in `OnEvent(1)` | same |

### E. `CMenu_CmdDispatch@0x00425970` command table (IDA-correlated)

| Cmd | Action | Key `this` fields |
|-----|--------|-------------------|
| `0xc9` | Start → `CStartGame1`, bg `0x1013c`, `nPushedSubScreenId=0` | `+0x90/+0x94` icons, `+0x84`, `+0xe0` splash auto-back |
| `0xca` | History → `CHistoryDlg`, bg `0x1013e`, id=1 | `g_pApp+0x306` scroll index |
| `0xcb` | Quit → `CExitDlg`, bg `0x1013d`, id=2 | icon swap `+0xa0/+0xa4` |
| `0xcf` | Back → `CloseCurrentSubScreen` + `CMenu_OnSubScreenBack` | may call `CGame` network path |
| `0x8004` / `0x80ce` | `CMenu_DispatchHotkey` | `+0xcc`, `+0xc4`, hide `+0xb0..+0xb8` |
| else | `CDSApp_RouteSyntheticCloseEvent` (base slot 26) | synthetic close `code&0x8000` |

IDA field map (`sub_425970`): `+128`=`nPushedSubScreenId`, `+132`=`pCurSubScreen`, `+144/+148` start icons, `+152/+156` history, `+160/+164` quit, `+208`=`pRuch0` anchor for `AddChild`, `+224`=`bLastSplashFlag`.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0042d200` | Parent `+0x50` focus stash landmark |
| `set_decompiler_comment` | `0x0042d280` | Post-loop `vfn+0x24` invalidate landmark |
| `set_decompiler_comment` | `0x004836b4` | `g_pCMenu_vftable_primary` slot 22/26/27 catalog |
| `save_program` | `bulanci.exe` | saved (retry after initial transaction lock) |

No renames or prototype changes required — symbols and `CMenu *` typing already correct from R4/R5/R6.

## Decomp corrections (IDA vs Ghidra)

| Item | IDA | Ghidra (live) | Resolution |
|------|-----|---------------|------------|
| `0x0042d160` child param | `unsigned __int16 *a2`, flag test `a2[34]` | `CDSView *param_1`, `(param_1->win).wViewStateFlags` | **Same** — `+0x44` as ushort index 34 |
| `0x0042d1a0` parent focus | `*(_BYTE**)(v7+80)` @ `a1+76` parent | `(this->win).pParent` + `+0x50` deref | **Same** — `0x4c` / `0x50` |
| `0x0042d1a0` modal exit | `*(_WORD*)(a1+74)` return | `wChainCounter_4a` @ `+0x4a` | **Same offset** — modal exit code, not chain counter |
| `0x0042d1a0` vtable calls | `+36` / `+52` | `+0x24` / `+0x34` | **Same** slots 9 and 13 |
| `0x00425970` | `sub_425970(int this, …)` | `CMenu::CMenu_CmdDispatch(CMenu *this, …)` | Offsets align with [CMenu.md](../struct_recovery/CMenu.md) |

## Frida

**none** — static decompile, disasm, vtable dump, and IDA export sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| `g_pCMenu_vftable_primary` Ghidra global label | Vtable bytes at `0x004836b4` proven; optional `rename_label` on data block — cosmetic |
| `CMenu_CloseCurrentSubScreen` icon table via `pPad_a8_af` | Ghidra names gap `+0xa8` as struct anchor for computed icon pointer pairs — layout doc already notes 8 B pad |

## Cross-links

- [pass_r4_CMenu_report.md](../struct_recovery/pass_r4_CMenu_report.md) — R4 layout `0xe4`, dual `CMenu`/`CGame` symbol split
- [CMenu.md](../struct_recovery/CMenu.md) — full field table
- [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md) — slot catalog 9/13/25/26/27
- [round6_logic_task_18_report.md](../logic_recovery/round6_logic_task_18_report.md) — `DoModal` / detach band context
- [main_menu.md](../../gameplay/main_menu.md) — narrative menu flow
