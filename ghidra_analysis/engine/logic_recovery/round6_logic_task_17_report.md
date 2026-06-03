# Round 6 logic — Task 17 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 17 |
| **title** | Logic sim_429_436: 0x0042c430–0x0042cc80 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — All 22 addresses decompiled via Ghidra MCP (`batch_decompile`). Ghidra MCP disconnected before `disassemble_function`, `set_function_this_type`, renames, and `save_program`. No Frida required: layout, vtable slots, and control flow are proven from decompiler output cross-checked against [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md), [CDSView.md](../struct_recovery/CDSView.md), and [app_shell.md](../app_shell.md).

## 3. Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0042c430` | `CDSView_GetParentBounds` | Primary vfn[12]: if `this+0x4c` parent non-null, writes parent bbox width/height from parent `+0x20..+0x2c`; else `(0x7fff, 0x7fff)` | Decompile; vtable slot 12 @ `0x0047f954`; **this mis-typed `CBulanci *`** |
| `0x0042c480` | `CDSView_SetRect` | Primary vfn[11]: stores LTRB @ `+0x20..+0x2c`; if `+0x44 & 0x40` calls `CDSView__UpdateScreenCoordinates`; walks child chain @ `+0x54` calling child vfn+0x28 then vfn+0x20 on size delta | Decompile; [CDSView.md](../struct_recovery/CDSView.md); **this mis-typed `CBulanci *`** |
| `0x0042c540` | `CBulanci_AdjustAnchoredEdge` | Anchor helper: if anchor bit `0x10` center, scale edge coord; else add delta — called from `ComputeAnchoredRect` per edge | Decompile callee graph; anchor masks `1/2/4/8` in sibling; **should be `CDSView *` / rename `CDSView_AdjustAnchoredEdge`** |
| `0x0042c580` | `CDSView_ComputeAnchoredRect` | Primary vfn[10]: read parent frame @ `this+0x4c`; apply anchor bits to copy of self bbox; self vfn+0x30 + `CDSRect_ClampToBounds` | Decompile; vtable slot 10; **this mis-typed `CBulanci *`** |
| `0x0042c660` | `CWindow_FindNextFocusable` | TAB-order walk on child intrusive list (`+0x54` chain): resume cursor @ `+0x50`; forward `+8` / backward `+0xc` link; requires `+0x46 & 1` and `+0x44 & 5` | Decompile; correctly `CWindow *this` |
| `0x0042c6c0` | `CMenu_ClearAllChildren` | Detach loop: `GetFirstChildView` → `CMenu_DetachChild` → child vfn+8 (release); repeat until empty | Decompile; `CMenu.md` chain band |
| `0x0042c700` | `FUN_0042c700` | Input-chain hit-test refresh: walk `g_pInputChainHead` (or build via `FUN_0042c1c0(g_pApp, g_pApp+0xf0)`); views with `+0x46 & 0x10` run vfn+0x18 hit test; hit → `CDSApp_SetPendingChildView` | Decompile; caller `CDSApp_SetInputChainHead`; xref note in task-14 report |
| `0x0042c770` | `CDSApp_BroadcastSyntheticEventToChildren` | Primary vfn[25]: wrap `(0x200, evt…)`; for each child with `+0x4c` parent link set, `FUN_0042ec90(child+0x10, …)` | Decompile; [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md) slot 25; **`CDSView *this`, not `CBulanci`/`CDSApp`** |
| `0x0042c7d0` | `CDSApp_RouteSyntheticCloseEvent` | Primary vfn[26]: if `param_1 & 0x8000`, `CDSView__EndModal(this, …)` | Decompile; vtable slot 26; **`CDSView *this`** |
| `0x0042c7f0` | `CDSApp_SetInputChainHead` | `__fastcall`: sync old/new input chains via `FUN_0042c120` / `FUN_0042c140`; store `g_pInputChainHead`; call `FUN_0042c700` | Decompile; [app_shell.md](../app_shell.md) globals |
| `0x0042c860` | `CControl_ClaimModalFocusOnPress` | On press: if `+0x46` has `0x80` without `0x04`, set input-chain head + `g_pModalFocus` (`DAT_004b3b94`) | Decompile; modal mouse path |
| `0x0042c880` | `FUN_0042c880` | Mouse-move focus chain: vfn+0x1c hit on `g_pApp+0xf0` or active modal focus → `SetInputChainHead(FUN_0042c1c0(…))`; else walk chain with `FUN_0042c140` and clear head | Decompile; **caller `CDSApp_DispatchInputEvent` MOVE branch** ([app_shell.md](../app_shell.md) L376); also `CDSView_AdaptDisplaySize` |
| `0x0042c8e0` | `CDSView_AcquireKeyboardFocus` | Recursive keyboard focus: parent chain; sets `+0x44` bit1; stores focus child @ parent `+0x50`; posts `0x400/2` on parent event face; vfn+0x24 invalidate | Decompile; [status.md](../status.md) named symbol |
| `0x0042c960` | `FUN_0042c960` | `CDSView__Show` helper: when visible and no modal focus, vfn+0x1c hit → `FUN_0042c880` | Decompile; callee from `CDSView__Show` / `CDSView_EnableWidget` |
| `0x0042c990` | `CDSView__Show` | Set visible `+0x44` bit0; if parent shown or `this==g_pApp`: `CDSView_SetActive(1)`; optional `FUN_0042c960`; vfn+0x24 | Decompile |
| `0x0042c9f0` | `CDSView_EnableWidget` | Clear disabled bit `+0x44` bit2; on re-enable run `FUN_0042c960` + invalidate | Decompile |
| `0x0042ca30` | `CDSView_InvalidateRectClipped` | Primary vfn[9]: if `+0x44 & 0x40` and forced/sign-bit path, clip optional sub-rect to self bbox @ `+0x20..+0x2c`, forward to parent vfn+0x24 | Decompile; correctly `CDSView *this` |
| `0x0042cae0` | `CDSView_AdaptDisplaySize` | Primary vfn[8]: compare proposed rect (stack) vs current via vfn+0x30; if changed and was hidden, deactivate → vfn+0x2c SetRect → reactivate; may call `FUN_0042c880(g_pModalFocus)` | Decompile; vtable slot 8; **this mis-typed `CBulanci *`** |
| `0x0042cbb0` | `FUN_0042cbb0` | `CScroller` helper: offset embedded child view rect (`pPad_base+0x20..+0x2c`) by `(dx,dy)`; child vfn+0x20 | Decompile; UNCERTAIN plate comment (scrollbar child build) |
| `0x0042cbf0` | `CDSView__SetSize` | Non-vtable helper: origin @ `+0x20/+0x24` + size → rect; dispatch primary vfn+0x20 (`AdaptDisplaySize`) | Decompile; [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md); **this mis-typed `CBulanci *`** |
| `0x0042cc30` | `FUN_0042cc30` | Offset stored LTRB @ `+0x20..+0x2c` by `(dx,dy)`; call vfn+0x20 — used from `IDSAnim_BindUserData` | Decompile; [CBitmap.md](../struct_recovery/CBitmap.md) chained-face xref |
| `0x0042cc80` | `CDSView__SetPosition` | Absolute position: build rect from `(x,y)` + extent from origin/bucket fields; vfn+0x20 | Decompile; teleport xref [round3_task_22_report.md](../struct_recovery/round3_task_22_report.md); **this mis-typed `CBulanek *`** |

### View flag bytes (shared evidence)

| Offset | Decompile use in slice | Doc |
|--------|------------------------|-----|
| `+0x44` | Visible (`&1`), disabled (`&4`), modal stack (`&0x40`), hidden/sign in `AdaptDisplaySize` | [CWindow.md](../struct_recovery/CWindow.md) `wViewStateFlags`; [app_shell.md](../app_shell.md) `flags1` |
| `+0x46` | Focusable (`&1`), mouse-move eligible (`&0x10`), modal press (`&0x80`) | [app_shell.md](../app_shell.md) `flags2` / widget eligibility |
| `+0x4c` | Parent view pointer | `CWindow.pParent` |
| `+0x50` | Focus-child link / chain resume cursor | `CWindow` / focus walk |
| `+0x54` | Child chain head (CDSChain face) | `CDSView_SetRect` child walk |

### Input / focus globals

| Global | Address | Role in slice |
|--------|---------|---------------|
| `g_pApp` | `0x004b3b88` | Mouse coords @ `+0xf0` passed to hit tests |
| `g_pModalFocus` | `0x004b3b8c` | Keyboard focus; `FUN_0042c880` / `AdaptDisplaySize` |
| `g_pInputChainHead` | `0x004b3b90` | Mouse recipient chain; `SetInputChainHead` / `FUN_0042c700` |
| `DAT_004b3b94` | *(alias in decompile)* | Same slot as modal-focus store in `CControl_ClaimModalFocusOnPress` — reconcile with `g_pModalFocus` label |

## 4. Ghidra deltas

**None applied** — MCP session lost (`Not connected`) after `batch_decompile`.

**Queued (evidence-backed, not executed):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `set_function_this_type` | `CDSView *` @ `0x0042c430`, `0x0042c480`, `0x0042c540`, `0x0042c580`, `0x0042c770`, `0x0042c7d0`, `0x0042cae0`, `0x0042cbf0`, `0x0042cc80` | Decompiler shows `CBulanci`/`CBulanek`/`CDSApp` but bodies use view offsets `+0x20..+0x54` and primary vtable slots 8–12, 25–26 ([CDSView_vftable.md](../struct_recovery/CDSView_vftable.md)) |
| `rename_function_by_address` | `CBulanci_AdjustAnchoredEdge` → `CDSView_AdjustAnchoredEdge` @ `0x0042c540` | Only caller `CDSView_ComputeAnchoredRect`; operates on view anchor bits |
| `rename_function_by_address` | `FUN_0042c700` → `CDSApp_RefreshInputChainHitTest` | Body + sole post-chain caller `CDSApp_SetInputChainHead`; drop UNCERTAIN when disasm confirms |
| `rename_function_by_address` | `FUN_0042c880` → `CDSView_UpdateInputChainOnMouseMove` | Documented caller `CDSApp_DispatchInputEvent` MOVE path |
| `rename_function_by_address` | `FUN_0042c960` → `CDSView__Show_UpdateFocusChain` | Sole callers `CDSView__Show`, `CDSView_EnableWidget` |
| `rename_function_by_address` | `FUN_0042cc30` → `CDSView_OffsetRectAndAdapt` | Same pattern as `CDSView__SetPosition`/`__SetSize`; xref `IDSAnim_BindUserData` |
| `rename_function_by_address` | `FUN_0042cbb0` → `CScroller_OffsetChildViewRect` | `CScroller *this`; offsets `pPad_base` child bbox |
| `set_function_prototype` | `CDSView_GetParentBounds(CDSView *this, int *outZero, int *outSize)` | Match vtable slot 12 signature after this-type fix |
| `force_decompile` | All nine `set_function_this_type` targets | Prove field names (`win.nBbox_*`, `win.pParent`, …) |

`CWindow_FindNextFocusable`, `CDSView_InvalidateRectClipped`, `CMenu_ClearAllChildren` already decompile with plausible `this` types.

## 5. Frida

**none** — Static decompile + vtable catalog + `app_shell.md` input dispatch sufficient; no opcode or field offset ambiguity requiring runtime.

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| `FUN_0042c120` / `FUN_0042c140` / `FUN_0042c1c0` | Input-chain splice / hit-target resolve; defined in task-16 slice — not disassembled this pass (MCP down) |
| `DAT_004b3b94` vs `g_pModalFocus` | Decompile uses both labels at `0x004b3b8c` band; needs Ghidra global rename reconcile |
| `CDSApp_BroadcastSyntheticEventToChildren` naming | Symbol prefix says `CDSApp_` but installed as **CDSView** primary vfn[25] — keep name for xref stability or alias comment only |
| `CDSView__SetPosition` origin fields | Decompile shows `CBulanek` field names at `+0x20` — same offsets as `CWindow.nBbox_*` / gaming `nOrigin_*`; subclass-specific semantics not re-proven without disasm |
| `FUN_0042cbb0` scrollbar caller | Plate comment cites `CScroller_BuildAt`; caller xrefs not fetched (MCP down) |

## Cross-links

- [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md) — primary slots 8–12, 25–26 mapped to this slice
- [CDSView.md](../struct_recovery/CDSView.md) — `CDSView_SetRect`, 128 B shell
- [CWindow.md](../struct_recovery/CWindow.md) — focus walk, flag bytes
- [app_shell.md](../app_shell.md) — `CDSApp_DispatchInputEvent`, `g_pModalFocus`, `g_pInputChainHead`
- [round6_logic_task_14_report.md](./round6_logic_task_14_report.md) — prior worker pattern; `CDSApp_SetPendingChildView` xref to `FUN_0042c700`
