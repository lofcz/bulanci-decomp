# Round 3 — agent todo 04 report (R3 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **title** | MCP ECX plate CAnim and CGameView __thiscall sites |
| **source** | `agent_todos_50_r3.json` (blocker; supersedes todo 3) |
| **types** | CAnim, CBitmap, CGameView |
| **addresses** | `0x00419940`, `0x004164f0`, `0x00416f50`, `0x0041acf0`, `0x00416590` |
| **acceptance** | `set_function_this_type` per class (`CAnim *`, `CGameView *`) + `force_decompile`; decompile shows `Class::method(Class *this)` field access |

## Status

**DONE**

## Evidence

| Address | Symbol | `this_type` | Decompile field access (post-plate) |
|---------|--------|-------------|-------------------------------------|
| `0x00419940` | `CAnim_ctor` | `CAnim *` | `this->nOrigin_x` / `nOrigin_y`; `&this->vftable_anim_sub`; vtable stores on `this->vftable_*` |
| `0x004164f0` | `CAnim_RenderAnimFrame` | `CAnim *` | `TM_RenderFrame(&this->vftable_anim_sub, …)` — anim inner @ `+0x98` |
| `0x00416f50` | `CGameView_GetSpatialBucketKey` | `CGameView *` | `return this->dwView_aux_6c + this->nSpatial_bucket_y` (`+0x6c` / `+0x2c`) |
| `0x0041acf0` | `CGameView_OnEvent` | `CGameView *` | `this->gaming_host`, `this->bGaming_slot_id`, `this->vftable_primary` |
| `0x00416590` | `CGameView_InitGamingFields` | `CGameView *` | `this->bView_state_68..6a`, `this->bGaming_slot_id = 0xff`, `this->gaming_host = 0` |

### ECX / calling convention

| Address | Convention | Notes |
|---------|------------|-------|
| `0x00419940` | `__thiscall` | Header init via `ODSImage::CBitmap_ViewHeader_Init(this)`; ctor chain documented in [CAnim.md](./CAnim.md) |
| `0x004164f0` | `__thiscall` | Plate: `ESI=ECX`; inner frame @ `this+0x98` (`vftable_anim_sub`) |
| `0x00416f50` | `__thiscall` | Shared CDSView header; bucket key uses aux + spatial Y |
| `0x0041acf0` | `__thiscall` | Vtable slot 27; slot/host reads match [CGameView.md](./CGameView.md) layout |
| `0x00416590` | `__fastcall` | `ECX` = `this`; moved into `CGameView` class namespace by MCP |

Prior blocker was ECX typed as `int` / `_Globals::FUN_*` in export C; plating removes `param_1`/`this[-1]` style access on these five sites.

## Ghidra deltas

- `set_function_this_type` @ `0x00419940`, `0x004164f0` → **`CAnim *`**
- `set_function_this_type` @ `0x00416f50`, `0x0041acf0`, `0x00416590` → **`CGameView *`** (`InitGamingFields` moved into class `CGameView`)
- `force_decompile` on all five addresses (decompiler comments `R3 todo4` retained)
- `save_program bulanci.exe`

## Struct doc updates

- [CAnim.md](./CAnim.md) — Ghidra apply note (this task)
- [CGameView.md](./CGameView.md) — Ghidra apply note (this task)

## Remaining UNK

- `CGameView_InitGamingFields`: `gaming_host` still decompiles as `undefined *` (not `CGaming *`) — follow CGaming layout chain (todos 12/14).
- `CGameView_OnEvent`: `gaming_host` passed to helpers as `(CGameView *)` cast in one path — callee prototypes, not ECX plating.
- `CAnim_RenderAnimFrame` still calls `CDSView_NoOpStub` before `TM_RenderFrame` — behavior unchanged by this task.
