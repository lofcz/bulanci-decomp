# CAdvertising

## Status

**VERIFIED** (size **144** / **0x90** bytes). Embedded **`CWindow win`** @ `+0x00` (112 B dialog prefix via `CWindow_BuildAt`); `CDSUpdatedItem updatedItem` @ `+0x70`; `m_bBlockDismiss` @ `+0x8c`. Heap splash/backdrop children remain off-object (`CDSView__AddChild`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Stack object **144 bytes** | `0x00402490` | `CBulanci_OnEvent_MenuStateMachine`: `CAdvertising splash [144]` for event `0xf7` |
| `CDSUpdatedItem` at `+0x70` | `0x0040e5f0` | `CDSUpdatedItem_ctor(this+0x70)`; dtor `CAdvertising_dtor@0x00401a10` |
| `m_bBlockDismiss` at `+0x8c` | `0x0040e5f0`, `0x0040b500`, `0x0040b4d0` | ctor `this[0x8c]=0`; `OnKeyDown` / `OnLButtonDown` (`param_1[0x23]`) gate dismiss |
| Total **0x90** | — | last field at `0x8c` + 1 byte + 3-byte tail pad to 144 |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 112 | `CWindow` | win | `CWindow_BuildAt(&this->win,0,0,800,600,0)` @ `CAdvertising_ctor@0x0040e5f0`; MI vtables on `(this->win).pVftable_*` |
| 0x70 | 24 | `CDSUpdatedItem` | updatedItem | `CDSUpdatedItem_ctor@0x0040e649` (`LEA EDI,[ESI+0x70]`); `ArmDismissTimer@0x0040aca0`; `OnTimerTick@0x0040b4b0` (`this-0x70`) |
| 0x70 | 4 | `void*` | `updatedItem.pVftable_IDSUpdated` | `CAdvertising_ctor@0x0040e670`: `MOV [EDI],0x4817b8` (`EDI=this+0x70`); vtable `@0x004817b8` slot 4 = `OnTimerTick` |
| 0x88 | 4 | — | pPad_preDismiss | No ctor/dtor access; separates `updatedItem` from dismiss flag |
| 0x8c | 1 | byte | m_bBlockDismiss | `CAdvertising_ctor@0x0040e5f0`; `OnKeyDown@0x0040b500`; `OnLButtonDown@0x0040b4d0` |

## Ghidra apply

**Agent todo 3 (R3, 2026-05-30):** Embedded **`CWindow win`** @ `0`, `CDSUpdatedItem updatedItem` @ `0x70`, tail unchanged. `get_struct_layout` → **144 bytes**. Decompile `CAdvertising_ctor@0x0040e5f0`: `CWindow::CWindow_BuildAt(&this->win,…)`; vtables on `(this->win).pVftable_*`; `CDSUpdatedItem_ctor(&this->updatedItem)`. `set_function_prototype` / `set_function_this_type` on ctor, dtor, `OnKeyDown`, `OnLButtonDown`, `ArmDismissTimer` (`CAdvertising *`). `save_program bulanci.exe`. Report: [round3_task_03_report.md](./round3_task_03_report.md).

```
Structure: CAdvertising — 144 bytes
  win @ 0 (CWindow, 112 bytes)
  updatedItem @ 0x70 (CDSUpdatedItem, 24 bytes)
  pPad_preDismiss @ 0x88 (4 bytes)
  m_bBlockDismiss @ 0x8c
```

| Function | Address | Evidence in decompile |
|----------|---------|----------------------|
| `CAdvertising_ctor` | `0x0040e5f0` | `CWindow_BuildAt(&this->win,0,0,800,600,0)`; `(this->win).wViewFlags \|= 0x111`; `CDSUpdatedItem_ctor(&this->updatedItem)`; host `updatedItem.pVftable_IDSUpdated = 0x4817b8` |
| `CAdvertising_dtor` | `0x00401a10` | `CDSUpdatedItem_dtor(&this->updatedItem)` then `CWindow_dtor` |
| `ArmDismissTimer` | `0x0040aca0` | `Scheduler_RegisterEventSlot(&this->updatedItem, …)` |
| `OnKeyDown` | `0x0040b500` | Gates on `m_bBlockDismiss` before `CAdvertising_Dismiss` |

Ghidra may display `(this->win).wViewFlags` / `bM_bBlockDismiss` (auto prefix); logical names `win.wViewFlags` / `m_bBlockDismiss`.

## Heap child views (R4 todo 3, 2026-05-30)

Not inline fields — owned by the dialog’s child chain via `CDSView__AddChild`.

| Phase | Alloc | Type (decompile) | Function @ addr | Role |
|-------|-------|------------------|-----------------|------|
| ctor | `OperatorNew(0x6c)` | `CDSChained*` | `CAdvertising_ctor@0x0040e5f0` | Full-screen white fill; `CDSChained_InitWithRect(0,0,800,600)`; vtable `0x47fb04`; `AddChild` @ `0x0040e6e0` |
| `LoadSplashImage` | `OperatorNew(0x78)` | `CDSBitmap*` | `LoadSplashImage@0x0040fe30` | Centered splash from FLX track size; `CDSBitmap_ctor`; second `AddChild` @ `0x0040fee0` |

Menu splash path: `CBulanci_OnEvent_MenuStateMachine` event `0xf7` → `CAdvertising_ctor` → `LoadSplashImage(..., 0x1013a)` → `DoModal` → dtor.

## UNK

- **`win` interior dwords** (`dwField_*`, chain band): inherited from `CWindow` / `CDSChained`; no `CAdvertising`-specific xrefs beyond ctor.
- **108 B backdrop** exact struct name (108 B < `CWindow` 112 B): only alloc + `CDSChained_InitWithRect` proven; not a separate Ghidra struct yet.

## Follow-up

- Batch 42: `updatedItem+0x00` (`IDSUpdated` facet `0x4817b8`) proven host-written in `CAdvertising_ctor@0x0040e670` — not an UNK padding dword. See [round3_batch42_cdsupdateditem_facets_report.md](./round3_batch42_cdsupdateditem_facets_report.md), [CDSUpdatedItem.md](./CDSUpdatedItem.md).
- R4: [round4_task_03_report.md](./round4_task_03_report.md) — heap children + `CWindow_dtor` path documented in Ghidra.
