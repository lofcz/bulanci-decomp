# Round 6 logic — Task 18 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 18 |
| **title** | Logic sim_429_436: 0x0042ccd0–0x0042d3f0 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — All 22 addresses documented from Ghidra MCP `batch_decompile` + export cross-check (`bulanci.ghidra.exe.c`). Applied **11** `set_function_this_type` / `set_function_prototype` fixes and **1** rename; `save_program bulanci.exe` twice (after RenderChildrenClipped prototype fix). Remaining: three modal-focus `FUN_*` symbols, `CBulanci_CompareWideString` namespace/`this`, `CreateObject@0x0042ce30` target class id **0x17** not tied to a type name.

## 3. Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0042ccd0` | `CWindow_FocusSibling` | TAB-order helper: `CWindow_FindNextFocusable(this, direction)` → `CDSView_AcquireKeyboardFocus` on result; returns 1 if no sibling | Decompile; [widgets.md](../../widgets.md) §16; caller `CWindow_OnKeyDownTabEscEnter` |
| `0x0042ccf0` | `CDSView_RenderChildrenClipped` | Primary **vfn[14]**: walk child chain @ `win+0x54`; intersect global clip `DAT_004b3b9c` with each visible child (`+0x44 & 0x80`); set backbuffer clip; recurse `child vfn+0x38` | Decompile after `__thiscall CDSView*` fix; [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md) slot 14; [app_shell.md](../app_shell.md) §slot 14 |
| `0x0042ce30` | `CreateObject` | Static-init factory **classId 0x17** (`HandleClassRegister@0x0047c6a6`): `OperatorNew(0x68)` → `CDSChained_ctor` (104 B shell) | Decompile + export comment @ `FUN_0047e5f0`; **UNK:** which CDS class id 0x17 maps to (not `CDSView` 128 B) |
| `0x0042cea0` | `CDSView_dtor` | Body dtor: restore four vtables; clear `g_pInputChainHead` if match; `CMenu_ClearAllChildren`; `CDSChain_dtor@+0x54`; `CDSEventQueue_ClearForTarget`; `CDSChained_InitIDSReferencedVtable` | Decompile; `set_function_this_type CDSView*` |
| `0x0042cf50` | `CDSView_OnLButtonDownAcquireFocus` | Primary **vfn[19]**: one-line `CDSView_AcquireKeyboardFocus(this)` | Decompile; vtable slot 19; [app_shell.md](../app_shell.md) click-to-focus |
| `0x0042cf60` | `FUN_0042cf60` | On mouse-up path: if `g_pApp` and `g_pModalFocus`, clear `DAT_004b3b94`; vfn+0x1c hit-test vs `g_pApp+0xf0`; on miss call `FUN_0042c880(g_pModalFocus)` | Decompile; callers in mouse-up handlers (`bulanci.ghidra.exe.c` ~62095, 63394, 65556); plate **UNCERTAIN** — behavior partial, export name absent |
| `0x0042cfa0` | `FUN_0042cfa0` | Walk child intrusive list from `this` until `DAT_004b3b94` matches a view with `+0x44 & 8`; then `FUN_0042cf60` | Decompile; callee from `FUN_0042cff0`; **UNCERTAIN** name |
| `0x0042cff0` | `FUN_0042cff0` | Modal/volume focus teardown when view has `wViewStateFlags & 8`: optional `FUN_0042cfa0`; if `g_pModalFocus` and vfn+0x1c accepts coords, `FUN_0042c880(g_pModalFocus)` | Decompile after `CDSView*` typing shows `(this->win).wViewStateFlags`; callers `CDSView__Hide`, `CDSView_SetAsDefaultFocusChild` |
| `0x0042d040` | `CDSView__Hide` | If visible (`+0x44 & 1`): clear bit; if sign bit set → `CDSView_SetActive(0)`, `FUN_0042cff0`, primary vfn+0x24 invalidate | Decompile; [damage_pipeline.md](../../gameplay/damage_pipeline.md) xref |
| `0x0042d080` | `CDSView_SetAsDefaultFocusChild` | If `+0x44 & 4` unset: OR in bit 4 (default-focus-child); if sign bit → `FUN_0042cff0` + vfn+0x24 | Decompile; [widgets.md](../../widgets.md) `+0x6c` default-focus child |
| `0x0042d0b0` | `CDSView__AddChild` | Non-vtable helper: center child when anchor flags `0x40000/0x80000`; hide→`AddChildInternal`→show if child was visible; default keyboard focus when parent `+0x50==0` | Decompile after rename + `CDSView*` — uses `(this->win).nBbox_*`; [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md) |
| `0x0042d160` | `CMenu_DetachChildWithVisibility` | `(child+0x44)&2` → release keyboard focus; `&1` → hide; `CMenu_DetachChild`; restore show if was visible | Decompile; callers `CHelpDlg_LoadHelpPage`, `CHistoryDlg_LoadHistoryPage` ([round5_worker_13_report.md](../struct_recovery/round5_worker_13_report.md)) |
| `0x0042d1a0` | `CDSView_DoModal` | Modal pump: parent eligibility / optional `CDSView__AddChild`; set `g_pModalFocus`; acquire focus; `+0x44 \|= 0x10`; show; inner loop `CDSApp_PumpTick` until `+0x4a` exit; optional `vfn+0x34` filter when `+0x46 & 0x40`; restore focus; return exit code or `0x8003` | Decompile after `CDSView*` — field names via `win` sub-struct; [app_shell.md](../app_shell.md) pseudocode; **doc fix:** [tick_system.md](../tick_system.md) cites `@0x0042d240` — **stale**; live symbol @ **`0x0042d1a0`** (`ghidra_functions_dump.csv`) |
| `0x0042d2d0` | `CDsStringReleaseHeader` | Decrement refcount at `header+8`; `Runtime_Free` when zero | Decompile; xref `CEdit_OnKeyDown` temp strings ([CEdit_OnKeyDown.md](../CEdit_OnKeyDown.md)) |
| `0x0042d2f0` | `CNumEdit_ParseDecimalValue` | If `*this` handle non-null → `CNumEdit_ParseWideDigitsToUint`; else 0 | Decompile; `CNumEdit` member |
| `0x0042d310` | `CBulanci_NormalizePathForExistenceCheck` | `GetFileAttributesW`; on failure `0xffffff00`; else `(attrs>>4)&1` as existence/dir bit | Decompile; used by temp-path helper (task 19) |
| `0x0042d330` | `CBulanci_GetPathSuffixCompareIndex` | Same `GetFileAttributesW` gate; returns inverted `(attrs>>4)&1` for suffix-compare paths | Decompile; caller `CBulanci_GetPathBasename` band (task 19) |
| `0x0042d350` | `CDsString_AllocWide` | `__cdecl`: alloc `length*2+0xe` header; init capacity/refcount/length; return `wchar_t*` @ header+0xc | Decompile; layout doc @ `CDsStringAssignFromHandle` in export |
| `0x0042d390` | `thunk_FUN_0042d350` | 5-byte forward thunk to `CDsString_AllocWide` | `ghidra_functions_dump.csv` — manifest duplicate listing |
| `0x0042d3a0` | `CDsStringAllocAndCopyWide` | `CDsString_AllocWide(cap,cap)` + optional `memcpy` of `param_3` wchars | Decompile |
| `0x0042d3e0` | `CDsStringSetLengthAndTerminate` | `__thiscall` on **header** ptr: store length @ `+0`; NUL @ `buf+length*2` (`+0xc` base) | Export @ `104030`; family doc @ `0x0042d490` comment block |
| `0x0042d3f0` | `CBulanci_CompareWideString` | Lexicographic wide strcmp: `*this` handle vs `param_1` (empty-string fallback `PTR_DAT_004afce0`); returns `-1/0/1` | Export @ `104040`; used by level/score comparators ([round6_logic_task_19_report.md](./round6_logic_task_19_report.md)) — **still typed `CBulanci*`** in decompile (ECX is handle **slot**, not app object) |

### Slice themes

1. **View tree focus/modal (`0x0042ccd0`–`0x0042d1a0`)** — sibling focus, clipped child render, hide/show/default-focus, modal pump, menu page detach.
2. **Modal-focus globals (`FUN_0042cf60`–`0x0042cff0`)** — cooperate with `g_pModalFocus`, `DAT_004b3b94`, and `FUN_0042c880` (task 17 input-chain band).
3. **CDSString COW primitives (`0x0042d2d0`–`0x0042d3f0`)** — refcount release, wide alloc/copy, length update, generic handle compare (feeds task 19 string/stream slice).

### View flag bytes (this slice)

| Offset | Use in slice | Doc |
|--------|--------------|-----|
| `+0x44` | Visible `&1`, default-focus `&4`, modal-focus `&8`, modal-active `&0x10` | [CWindow.md](../struct_recovery/CWindow.md) `wViewStateFlags` |
| `+0x46` | Stay-modal eligibility `&0x40` in `DoModal` outer loop | [app_shell.md](../app_shell.md) `flags2` |
| `+0x4a` | Modal exit code (`DoModal` inner loop) | [app_shell.md](../app_shell.md) — Ghidra field name `wChainCounter_4a` at same offset |
| `+0x4c` | Parent view pointer | `CWindow.pParent` |
| `+0x50` | Focus-child link | Focus walk / `AddChild` default focus |
| `+0x54` | Child chain (`CDSChain`) | Render + detach paths |

## 4. Ghidra deltas

**Applied** (`save_program bulanci.exe` ×2):

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `CDSView *` @ `0x0042ccf0`, `0x0042cea0`, `0x0042cf50`, `0x0042cf60`, `0x0042cfa0`, `0x0042cff0`, `0x0042d040`, `0x0042d080`, `0x0042d0b0`, `0x0042d1a0` | RenderChildrenClipped / DoModal / AddChild now use `this->win.*`; modal helpers use `wViewStateFlags` |
| `set_function_prototype` | `__thiscall` @ `0x0042cf50`, `0x0042cf60`, `0x0042cfa0`, `0x0042cff0`, `0x0042d040`, `0x0042d080`, `0x0042ccf0` | Fixed `__fastcall`/`int*` artifacts on vtable + modal helpers |
| `set_function_prototype` | `int __thiscall CBulanci_CompareWideString(int *this, ushort *param_1)` @ `0x0042d3f0` | Prototype only — class namespace still `CBulanci` |
| `rename_function_by_address` | `CDSView::AddChild` → `CDSView__AddChild` @ `0x0042d0b0` | Matches non-vtable naming in [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md) |
| `force_decompile` | `0x0042ccf0`, `0x0042d0b0`, `0x0042d1a0`, `0x0042cff0`, `0x0042d3f0` | Verified typed field access |

**Queued (not executed):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `FUN_0042cf60` / `FUN_0042cfa0` / `FUN_0042cff0` | Plate comments still **UNCERTAIN**; need caller-based names (e.g. mouse-up vs hide teardown) without guessing |
| `set_function_this_type` + rename | `CDsString_CompareHandles` @ `0x0042d3f0` | Needs `CDsStringHandle` struct or global rename — `set_function_this_type int*` rejected |
| `rename_function_by_address` | `CreateObject` → class-specific factory @ `0x0042ce30` | Blocked until classId **0x17** → type name proven |
| `set_decompiler_comment` | `@0x0042d1a0` | Note `+0x4a` exit code vs Ghidra `wChainCounter_4a` label |

## 5. Frida

**none** — Control flow, vtable slots, string header layout, and Win32 path probes are fully visible in static decompile; modal pump already documented in [app_shell.md](../app_shell.md) / [main_menu.md](../../gameplay/main_menu.md).

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| `FUN_0042cf60` / `FUN_0042cfa0` / `FUN_0042cff0` export names | Decompiler plates mark **UNCERTAIN**; mouse-up vs hide/default-focus call graph not fully xref’d this pass |
| `CreateObject@0x0042ce30` classId 0x17 | Factory alloc **0x68** = `CDSChained` only; no struct doc ties id 0x17 to a named widget |
| `CBulanci_CompareWideString` decompile | Still shows `(this->app).vftable_primary` — needs handle-slot struct, not `CBulanci*` |
| `tick_system.md` DoModal address | Documents `0x0042d240`; binary has **`CDSView_DoModal@0x0042d1a0`** only |
| `DAT_004b3b94` vs `g_pModalFocus` | Same band as task 17 — global label reconcile pending |

## Cross-links

- [CDSView_vftable.md](../struct_recovery/CDSView_vftable.md) — slots 14, 19; non-vtable `CDSView__AddChild`
- [CDSView.md](../struct_recovery/CDSView.md) — 128 B shell, child chain @ `win+0x54`
- [round6_logic_task_17_report.md](./round6_logic_task_17_report.md) — `FUN_0042c880` input-chain neighbor
- [round6_logic_task_19_report.md](./round6_logic_task_19_report.md) — downstream CDSString/stream helpers
