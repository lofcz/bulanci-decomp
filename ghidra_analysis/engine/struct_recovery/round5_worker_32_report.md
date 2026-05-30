# Round 5 — worker 32 report (R5 worker 32/50)

## Task

| Field | Value |
|-------|-------|
| **id** | 32 |
| **round** | 5 |
| **title** | CDSView vftable + layout UNKs |
| **types** | `CDSView`, `CDSView_vftable_t`, `CDSChained`, `CWindow` |
| **addresses** | `0x0047f954`, `0x0047f90c`, `0x0047fd10`, `0x00402890`, `0x004049d0`, `0x00421910`, `0x0040bc10`, `0x004164f0`, `0x00417380`, `0x00416590`, `0x0040c3a6` |
| **acceptance** | Close remaining UNKs on primary/MI vtables and 128 B `CDSView` tail band; Ghidra renames + `save_program` |

## Status

**DONE** — `+0x18` MI face identified as **`IDSReferenced`** (4 slots, typed struct); primary slot 15 renamed **`pRenderSelf`** with override catalog; `+0x70..+0x7c` tail documented as per-class union; `dwField_08/0c/1c` confirmed ctor-padding with no view-shell consumers.

## Evidence

### IDSReferenced MI @ object `+0x18`

| Slot | Off | Function | Address | Evidence |
|-----:|----:|----------|---------|----------|
| 0 | `+0x00` | `CDSReferenced_GetTypeInfo_Base` | `0x00402890` | Decompile: `return &DAT_004b7c50`; stamped by `CDSChained_ctor@0x00403333` → `0x47f90c` |
| 1 | `+0x04` | `CDSChain_AdjustThisOffset_ThisMinus14` | `0x004049d0` | Decompile: `CDSChain_AdjustThisOffset(param_1 - 0x14)` |
| 2 | `+0x08` | `CDSObject_ReleaseViaVtable_ThisMinus18` | `0x00423db0` | Decompile: release on `param_1 - 0x18` |
| 3 | `+0x0c` | `CDSView_ScalarDeletingDtor_thunk_Sub18` | `0x00421910` | Decompile: `CDSView_DtorScalar(&this[-1].win.bModalFlag)` |
| 3′ | `+0x0c` | `CWindow_ScalarDeletingDtor_thunk_Sub18` | `0x0040bc10` | **CWindow** table @ `0x0047fd10`; calls `CWindow_vDtor(this-0x18)` |

Global rename: `g_pCDSView_vftable_field18` → **`g_pCDSView_vftable_IDSReferenced`**; dialog parallel **`g_pCWindow_vftable_IDSReferenced` @ `0x0047fd10`**.

### Primary vtable slot 15 (`pRenderSelf`, was `pReserved15`)

| Class | Address | Role |
|-------|---------|------|
| Base `CDSView` | `0x00438340` | `CDSView_NoOpStub` |
| `CAnim` / `CDeath` / `CDeath2` / `CExplosion` | `0x004164f0` | `CAnim_RenderAnimFrame` → `TM_RenderFrame(&anim_sub)` |
| `CBulAnim` / `CDSAnim` | `0x0040b850` | `PrimaryRender_thunk` → `TM_RenderFrame(&pVftable_AnimInner)` |
| `CBulanek` | `0x00417380` | `CBulanek_OnShow` — palette + fire-delay when shown |
| `CGaming` | `0x0041c140` | `CGaming_OnResumeOrStartGame` — resume/start round hook |
| `CMina` | `0x00416790` | `CMina_RenderAnimFrame` + scheduler arm |
| `CMovieView` | `0x00422840` | `OnMovieStop` |
| `CPanel` | `0x00427640` | panel-specific hook |

Slots **16–18** remain shared **`CDSView_NoOpStub`** except **`CButton`/`CIcon`**: slots 17–18 → `CIcon_Invalidate@0x004030a0`.

Slot **14** (`pRenderChildren`) vs **15**: containers draw children @ slot 14 (`CDSView_RenderChildrenClipped@0x0042ccf0` calls child `vfn+0x38`); leaf anim types self-render via slot 14 override and use slot 15 for track-manager blit.

### Slot 26 (`pCmdDispatch`) — naming

Base **`CDSApp_RouteSyntheticCloseEvent@0x0042c7d0`** handles synthetic close (`wMsg_id==0x100`, `code&0x8000`). Per-class overrides are intentional (not a misname on the base):

| Override | Address | Class |
|----------|---------|-------|
| `CBulanci_OnEvent_MenuStateMachine` | `0x00402490` | app root |
| `CMenu_CmdDispatch` | `0x00425970` | main menu |
| `CGaming_OnCmd` | `0x0041d5f0` | in-game host |
| `CHelpDlg` / `CHistoryDlg` / `CExitDlg` | various | dialog cmd routing |

Keep struct field **`pCmdDispatch`**; document overrides in vftable catalog only.

### Slot 27 (`pOnEvent`) — custom-message hook

`CDSView_DispatchEvent@0x0042c040` routes `wMsg_id==0x200` → primary slot 27. Notable overrides: `CGameView_OnEvent@0x0041acf0`, `CBulanek_OnEvent@0x00420d40`, `CMenu`-family widgets, `CEdit_OnFocusEvent`, etc. Base = `CDSView_EmptyHook27@0x00438f80`.

### Layout: `CDSView` 128 B tail `@ +0x70..+0x7c`

| Offset | Base `CDSView` name | Gameplay (`CAnim`/`CGameView`) | Dialog (`CSessionList`) |
|--------|---------------------|--------------------------------|-------------------------|
| `+0x70` | `bGaming_slot_id` | `bPlayerSlot` (`InitGamingFields@0x00416590` → `0xff`) | `pSessionListBox` (pointer) |
| `+0x74` | `nDest_x` | collision / teleport left | `pCaptionStatic` |
| `+0x78` | `nDest_y` | collision top | `pJoinButton` |
| `+0x7c` | `nSrc_x` | collision right (extends on `CGameView` through `+0x80`) | *(unused on 128 B shell)* |

**Conclusion:** no type uses four dwords through `+0x7f` on the **128 B** base shell — struct ends at `nSrc_x` @ `+0x7c` (size `0x80`). Larger subclasses (`CGameView` `0x98`, `CAnim` `0xf0`) extend past `+0x7c` with collision bottom @ `+0x80` and host pointer @ `+0x84`.

### Layout: `win.dwField_08` / `dwField_0c` / `dwField_1c`

| Claim | Address | Evidence |
|-------|---------|----------|
| Sole writer on view shell | `CDSChained_ctor@0x00403309/0x0040331d` | Zero-init `+0x08`, `+0x0c`, `+0x1c` |
| No non-ctor store to `+0x08` on `CDSChained` leaves | program scan | No `MOV [ESI+0x8]` outside stream/chain ctors |
| `dwField_1c` not stream storage on anim | `IDSAnim_SelectRandomTrack@0x004392a0` | Decompiler cast is misleading; effective read is `dwChainRoot` @ `+0x40` |

**Conclusion:** keep **`dwField_08` / `dwField_0c` / `dwField_1c`** as ctor-zero padding/reserved dwords on the shared drawable prefix; do not rename without a consumer.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `create_struct` | `CDSView_vftable_IDSReferenced_t` | 16 B, 4 pointer fields |
| `set_global` | `0x0047f90c` | `g_pCDSView_vftable_IDSReferenced` + plate |
| `set_global` | `0x0047fd10` | `g_pCWindow_vftable_IDSReferenced` + plate |
| `modify_struct_field` | `CDSView_vftable_t.pReserved15` | → `pRenderSelf` |
| `modify_struct_field` | `CDSChained.pVftable_field18`, `CWindow.pVftable_field18` | → `pVftable_IDSReferenced` |
| `set_plate_comment` | `0x00402890`, `0x00421910`, `0x004164f0`, `0x00417380` | R5 worker 32 notes |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSView_vftable.md](./CDSView_vftable.md) — IDSReferenced MI table; slot 15 catalog; UNK closure
- [CDSView.md](./CDSView.md) — tail union table; padding dword note
- [CDSChained.md](./CDSChained.md) — `pVftable_IDSReferenced` rename
- [CWindow.md](./CWindow.md) — `pVftable_IDSReferenced` + global name

## Remaining UNK

- Exact call site(s) that invoke primary **vfn[15]** (`pRenderSelf`) — no direct `CALL [reg+0x3c]` in `.text`; likely reached only through engine render/show dispatch not yet tied to a single callsite.
- **`pReserved16..18`** semantic names beyond `CButton`/`CIcon` invalidate pair — no other non-stub overrides found.
- Per-dialog **`pCmdDispatch`** / **`pOnEvent`** bodies beyond catalog listing (behavior is class-local).
