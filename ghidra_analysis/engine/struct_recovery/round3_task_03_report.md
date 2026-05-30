# Round 3 — Task 03 Report (R3 worker todo id=3)

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **title** | Embed CWindow win @ 0 on CAdvertising in Ghidra |
| **one_liner** | `CAdvertising`: embed **`CWindow win`** @ `+0x00` (112 B dialog prefix); `CDSUpdatedItem updatedItem` @ `+0x70`; dismiss flag @ `+0x8c`. |
| **acceptance** | Ghidra `CAdvertising` shows embedded `CWindow` at offset 0 (144 B total); ctor/dtor/event handlers decompile with `&this->win` / `this->updatedItem`; stack splash `CAdvertising splash` @ `CBulanci_OnEvent_MenuStateMachine`. |
| **source** | `agent_todos_50_r3.json` (supersedes round3_tasks.json id 3 / CDSJpegImage) |
| **structs** | CAdvertising, CWindow, CDSScript (context) |
| **addresses** | `0x0040e5f0`, `0x00401a10`, `0x00405560`, `0x00402490` |

## Status

**DONE**

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Embedded **`CWindow win`** @ object `+0` | `CAdvertising_ctor@0x0040e5f0` | `CWindow::CWindow_BuildAt(&this->win,0,0,800,600,'\0')`; subclass vtables patched on `(this->win).pVftable_*` |
| **`CDSUpdatedItem`** @ `+0x70` | `CAdvertising_ctor@0x0040e5f0` | `CDSUpdatedItem_ctor(&this->updatedItem)`; host writes `(this->updatedItem).pVftable_IDSUpdated` |
| **`m_bBlockDismiss`** @ `+0x8c` | `CAdvertising_ctor@0x0040e5f0` | `this->bM_bBlockDismiss = 0` (Ghidra byte prefix); consumers `OnKeyDown@0x0040b500`, `OnLButtonDown@0x0040b4d0` |
| Dtor order | `CAdvertising_dtor@0x00401a10` | `CDSUpdatedItem_dtor(&this->updatedItem)` then `CWindow_dtor(this)` |
| Base dialog builder | `CWindow_BuildAt@0x00405560` | `CDSChained_ctor` + four vtables + bbox `+0x20..+0x2c`; size **112** (`0x70`) |
| Stack **144 B** splash | `CBulanci_OnEvent_MenuStateMachine@0x00402490` | Local `CAdvertising splash`; event `0xf7` → ctor → `DoModal` → dtor |
| Total size **144** / **0x90** | `get_struct_layout CAdvertising` | `win` 112 + `updatedItem` 24 + tail pad + dismiss byte |

## Ghidra deltas

- Verified / retained: `CAdvertising` **144 B** — `CWindow win` @ `0`, `CDSUpdatedItem updatedItem` @ `0x70` (`112` decimal = `0x70` hex in layout listing)
- `set_function_prototype` + `set_function_this_type` (`CAdvertising *`): `CAdvertising_ctor@0x0040e5f0`, `CAdvertising_dtor@0x00401a10`, `OnKeyDown@0x0040b500`, `OnLButtonDown@0x0040b4d0` (__fastcall → __thiscall), `ArmDismissTimer@0x0040aca0`
- `modify_struct_field`: dismiss flag logical name `m_bBlockDismiss` (Ghidra may still display `bM_bBlockDismiss` auto-prefix on bytes)
- `save_program bulanci.exe`

## Struct doc updates

- [CAdvertising.md](./CAdvertising.md) — layout and Ghidra apply already matched; no layout change required

## Remaining UNK

- `win` interior / chain band fields: inherited from `CWindow` / `CDSChained` (no `CAdvertising`-specific xrefs beyond ctor vtable patches).
- Heap splash/backdrop children (`operator new(0x6c)`, `CDSBitmap` `0x78`) remain off-object via `CDSView__AddChild`.
- `CAdvertising_dtor` still calls `_Globals::CWindow_dtor((int)this)` — global thunk naming; functionally destroys embedded prefix at `this+0`.
