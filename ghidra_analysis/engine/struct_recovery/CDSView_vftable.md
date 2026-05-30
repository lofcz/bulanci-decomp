# CDSView_vftable_t

## Status

**VERIFIED** — 28-slot **primary** vtable at `g_pCDSView_vftable_primary` (`0x0047f954`, 112 B). Ghidra struct `CDSView_vftable_t` matches `get_struct_layout` field names. Secondary MI faces documented below. **Not vtable:** `CDSView__AddChild@0x0042d0b0`, `CDSView__SetSize@0x0042cbf0` (global `__thiscall` helpers).

## Object relationship

| Layer | Doc | Notes |
|-------|-----|-------|
| `CDSView` 128 B shell | [CDSView.md](./CDSView.md) | `CWindow win` @ `+0` for dialogs; `CDSChained` drawable uses **same vtables** through `+0x67` without full `CDSView` struct |
| `CWindow` 112 B dialog | [CWindow.md](./CWindow.md) | Primary table **`g_pCWindow_vftable_primary` @ `0x0047fd5c`** — same 28 slots; overrides slots 14 and 22 |
| `ODSImage` mixin | [ODSImage.md](./ODSImage.md) | `SetOwner` → `CDSView__SetSize` on host shell (`+0x20` origin), not on `CDSView` Ghidra type |

## Vtable globals (`.rdata`)

| Global | Address | Object offset | Slots | Role |
|--------|---------|---------------|-------|------|
| `g_pCDSView_vftable_primary` | `0x0047f954` | `+0x00` | 28 | Drawable / widget shell (`CDSChained_ctor`, `CMenu`, `CGameView`, …) |
| `g_pCDSView_vftable_IDSChained` | `0x0047f938` | `+0x04` | 6 | `IDSChained` / `CDSChain` list face |
| `g_pCDSView_vftable_IDSEventHandler` | `0x0047f920` | `+0x10` | 5 | Posted-message router; **slot 4** = `CDSView_DispatchEvent` |
| `g_pCDSView_vftable_IDSReferenced` | `0x0047f90c` | `+0x18` | 4 | **`IDSReferenced`** MI; dialog variant `g_pCWindow_vftable_IDSReferenced` @ `0x0047fd10` |
| `g_pCWindow_vftable_primary` | `0x0047fd5c` | `+0x00` | 28 | `CWindow_BuildAt@0x00405560` dialog primary |

Catalog cross-ref: `master_vtable_catalog.csv` / [master_vtable_catalog.md](../master_vtable_catalog.md).

## Primary vtable — `CDSView_vftable_t` @ `0x0047f954`

| Slot | Off | Struct field | Function | Address | Evidence |
|-----:|----:|--------------|----------|---------|----------|
| 0 | `+0x00` | `pCreateObject` | `CDSView_GetClassTable` | `0x00403390` | `CDSChained_ctor@0x00403320` MOV `[ESI],0x47f954` |
| 1 | `+0x04` | `pVectorDtor` | `CDSView_DtorScalar` | `0x0040ada0` | Scalar-deleting dtor |
| 2 | `+0x08` | `pRelease` | `CDSObject_ReleaseViaVtable` | `0x004245c0` | Shared COM release |
| 3 | `+0x0c` | `pGetDataSize` | `CDSView_GetDataSize` | `0x0042c2e0` | Serialize size walk |
| 4 | `+0x10` | `pSaveData` | `CDSView_SaveData` | `0x0042c320` | Serialize write |
| 5 | `+0x14` | `pLoadData` | `CDSView_LoadData` | `0x0042c370` | Serialize read |
| 6 | `+0x18` | `pGetDataKind` | `CDSView_GetDataKindStubZero` | `0x004033a0` | `return 0` |
| 7 | `+0x1c` | `pHitTest` | `CDSView_HitTest` | `0x004028a0` | `inside(this+0x30..0x3c, pt)` — decompile |
| 8 | `+0x20` | `pAdaptDisplaySize` | `CDSView_AdaptDisplaySize` | `0x0042cae0` | **`CDSView__SetSize@0x0042cbf0`** calls `(*this+0x20)` |
| 9 | `+0x24` | `pInvalidateRect` | `CDSView_InvalidateRectClipped` | `0x0042ca30` | `CEdit_OnKeyDown` → `vtable+0x24` |
| 10 | `+0x28` | `pComputeAnchoredRect` | `CDSView_ComputeAnchoredRect` | `0x0042c580` | Anchor bits `wWidgetFlags & 0x0F` |
| 11 | `+0x2c` | `pSetRect` | `CDSView_SetRect` | `0x0042c480` | Bbox `+0x20..2c`; child chain `+0x54` |
| 12 | `+0x30` | `pGetParentBounds` | `CDSView_GetParentBounds` | `0x0042c430` | Parent size for layout |
| 13 | `+0x34` | `pIsModalDoneRecursive` | `CDSView_IsModalDoneRecursive` | `0x0042c3e0` | Modal-exit probe |
| 14 | `+0x38` | `pRenderChildren` | `CDSView_RenderChildrenClipped` | `0x0042ccf0` | Default: clip-draw **children** (`child vfn+0x38`); leaf widgets override slot 14 for self-draw |
| 15 | `+0x3c` | `pRenderSelf` | `CDSView_NoOpStub` | `0x00438340` | **Secondary render/show hook** — anim: `CAnim_RenderAnimFrame@0x004164f0` / `PrimaryRender_thunk@0x0040b850`; `CBulanek_OnShow@0x00417380`; `CGaming_OnResumeOrStartGame@0x0041c140` |
| 16 | `+0x40` | `pReserved16` | `CDSView_NoOpStub` | `0x00438340` | Base no-op |
| 17 | `+0x44` | `pReserved17` | `CDSView_NoOpStub` | `0x00438340` | **`CButton`/`CIcon` override → `CIcon_Invalidate@0x004030a0`** |
| 18 | `+0x48` | `pReserved18` | `CDSView_NoOpStub` | `0x00438340` | same as slot 17 on `CButton`/`CIcon` |
| 19 | `+0x4c` | `pOnLButtonDown` | `CDSView_OnLButtonDownAcquireFocus` | `0x0042cf50` | Click-to-focus |
| 20 | `+0x50` | `pOnMouseMove` | `CDSView_OnMouseStub` | `0x00416770` | Base no-op (mouse up path in engine) |
| 21 | `+0x54` | `pOnLButtonUp` | `CDSView_OnMouseStub` | `0x00416770` | Base no-op |
| 22 | `+0x58` | `pOnKeyDown` | `CDSView_OnKeyDown` | `0x0042c0c0` | `CEdit` overrides @ `0x0047ffc4` |
| 23 | `+0x5c` | `pOnKeyUp` | `CDSView_OnKeyUp` | `0x0042c0e0` | Bubble to parent |
| 24 | `+0x60` | `pOnChar` | `CDSView_OnChar` | `0x0042c100` | Bubble to parent |
| 25 | `+0x64` | `pBroadcastEvent` | `CDSApp_BroadcastSyntheticEventToChildren` | `0x0042c770` | **`CDSView_DispatchEvent`**: `evt->wMsg_id==0x400` |
| 26 | `+0x68` | `pCmdDispatch` | `CDSApp_RouteSyntheticCloseEvent` | `0x0042c7d0` | **`DispatchEvent`**: `wMsg_id==0x100`; `code&0x8000` → `CDSView__EndModal` |
| 27 | `+0x6c` | `pOnEvent` | `CDSView_EmptyHook27` | `0x00438f80` | **`DispatchEvent`**: `wMsg_id==0x200`; widgets override |

### CWindow primary diffs (`0x0047fd5c`)

| Slot | CDSView (`0x47f954`) | CWindow (`0x47fd5c`) |
|-----:|----------------------|----------------------|
| 0 | `CDSView_GetClassTable` | `CWindow_GetTypeDescriptor` |
| 1 | `CDSView_DtorScalar` | `CWindow_vDtor` |
| 14 | `CDSView_RenderChildrenClipped` | **`CWindow_Render@0x004055c0`** |
| 22 | `CDSView_OnKeyDown` | **`CWindow` dialog key handler @ `0x00403d60`** |

## Event dispatch (non-primary)

| Symbol | Address | Role |
|--------|---------|------|
| `CDSView_DispatchEvent` | `0x0042c040` | **IDSEventHandler vfn[4]** @ `this+0x10`; routes scheduler posts to primary **25/26/27** via `this-0x10` primary pointer |
| `CDSView__AddChild` | `0x0042d0b0` | Heap child attach + anchor centering + focus |
| `CDSView__SetSize` | `0x0042cbf0` | Delta size from origin `+0x20/+0x24` → **primary vfn[8]** |

`CDSView_DispatchEvent` plate comment (Ghidra) documents `CDSEventRecord.wMsg_id`: `0x100` cmd → slot 26, `0x200` custom → slot 27, `0x400` broadcast → slot 25.

## IDSChained face @ `0x0047f938` (`+0x04`)

| Slot | Function | Address |
|-----:|----------|---------|
| 0 | `CDSChain_GetClassTable` | `0x004011f0` |
| 1 | `CDSChain_AdjustThisOffset` | `0x0042ac90` |
| 2 | `CDSChain_ReleaseChild` | `0x0041aa40` |
| 3 | `CDSView_AdjustorThunk04_Dtor` | `0x00416f40` |
| 4–5 | `CDSView_NoOpStub` | `0x00438340` |

## IDSEventHandler face @ `0x0047f920` (`+0x10`)

| Slot | Function | Address |
|-----:|----------|---------|
| 0–3 | Shared `CBulanci` / registry thunks | `0x00402880` … |
| 4 | **`CDSView_DispatchEvent`** | `0x0042c040` |

## IDSReferenced face @ `0x0047f90c` (`+0x18`)

Ghidra struct **`CDSView_vftable_IDSReferenced_t`** (16 B). Object field: `pVftable_IDSReferenced` on `CDSChained` / `CWindow`.

| Slot | Struct field | Function | Address | Evidence |
|-----:|--------------|----------|---------|----------|
| 0 | `pGetTypeInfo` | `CDSReferenced_GetTypeInfo_Base` | `0x00402890` | `return &DAT_004b7c50` |
| 1 | `pAdjustThisOffset` | `CDSChain_AdjustThisOffset_ThisMinus14` | `0x004049d0` | adjustor **`this − 0x14`** |
| 2 | `pRelease` | `CDSObject_ReleaseViaVtable_ThisMinus18` | `0x00423db0` | adjustor **`this − 0x18`** |
| 3 | `pScalarDeletingDtor` | `CDSView_ScalarDeletingDtor_thunk_Sub18` | `0x00421910` | → `CDSView_DtorScalar(this−0x18)` |
| 3′ | *(CWindow table)* | `CWindow_ScalarDeletingDtor_thunk_Sub18` | `0x0040bc10` | → `CWindow_vDtor(this−0x18)` @ `g_pCWindow_vftable_IDSReferenced` |

## Ghidra apply

```
get_struct_layout CDSView_vftable_t   → 112 bytes, 28 fields
create_struct CDSView_vftable_IDSReferenced_t  (4 slots)
set_global 0x0047f954  g_pCDSView_vftable_primary  CDSView_vftable_t *
set_global 0x0047f938  g_pCDSView_vftable_IDSChained
set_global 0x0047f920  g_pCDSView_vftable_IDSEventHandler
set_global 0x0047f90c  g_pCDSView_vftable_IDSReferenced
set_global 0x0047fd10  g_pCWindow_vftable_IDSReferenced
set_global 0x0047fd5c  g_pCWindow_vftable_primary
modify_struct_field pReserved15 → pRenderSelf
modify_struct_field pVftable_field18 → pVftable_IDSReferenced (CDSChained, CWindow)
save_program bulanci.exe
```

**R5 worker 32 (2026-05-30):** closed field18 MI semantics; renamed slot 15; plate comments on IDSReferenced + render hooks. See [round5_worker_32_report.md](./round5_worker_32_report.md).

## Cross-links

- [CDSView.md](./CDSView.md) — 128 B object layout
- [CDSChained.md](./CDSChained.md) — `pVftable_primary` → `0x47f954` on minimal views
- [CWindow.md](./CWindow.md) — dialog vtable variant
- [app_shell.md](../app_shell.md) — `CDSApp` 34-slot superset (slots 0–27 shared semantics)

## UNK

- Call site(s) that dispatch primary **vfn[15] `pRenderSelf`** (no direct `CALL [reg+0x3c]` in `.text`).
- Semantics of **`pReserved16`** beyond base no-op (only slots 17–18 overridden on `CButton`/`CIcon`).
- Per-class **`pCmdDispatch`** / **`pOnEvent`** behavior beyond vftable catalog listing.
