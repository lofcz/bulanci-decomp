# Round 4 — agent todo 04 report (R4 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **title** | Retype CGameView gaming_host to CGaming* in InitGamingFields |
| **source** | `todos_gather_r4_0.json` / `agent_todos_50_r4.json` (handoff; supersedes R3 todo 4) |
| **types** | CGameView, CGaming, CAnim |
| **addresses** | `0x00416590`, `0x0041acf0`, `0x00416f50` |
| **acceptance** | `modify_struct_field` `gaming_host` → `CGaming *`; `set_function_this_type` `CGameView *` on proof sites; decompile shows `this->pGaming_host` as `CGaming *`; `save_program` |

## Status

**DONE**

## Evidence

| Address | Symbol | `this_type` | Decompile (post-R4) |
|---------|--------|-------------|---------------------|
| `0x00416590` | `CGameView_InitGamingFields` | `CGameView *` | `this->pGaming_host = (CGaming *)0x0`; `bGaming_slot_id = 0xff` |
| `0x0041acf0` | `CGameView_OnEvent` | `CGameView *` | `this->pGaming_host`; `CGaming_UnregisterAndRemoveObject(this->pGaming_host, this)` @ event `0xf9` |
| `0x00416f50` | `CGameView_GetSpatialBucketKey` | `CGameView *` | `return this->dwView_aux_6c + this->nSpatial_bucket_y` (unchanged; re-verified) |

### Struct layout

| Offset | Field | Type (Ghidra) |
|--------|-------|---------------|
| `+0x84` | `pGaming_host` | **`CGaming *`** (was generic `pointer` / `undefined *` in decompile) |

Aligns with `CAnim.pGaming_host` / `CBitmap.gaming_host` @ same offset (R3 todo 15 on anim/bitmap paths).

## Ghidra deltas

- `modify_struct_field` `CGameView` @ `offset:0x84` → **`CGaming *`**, name **`pGaming_host`**
- `set_function_this_type` **`CGameView *`** @ `0x00416590`, `0x0041acf0`, `0x00416f50` (confirmed in class `CGameView`)
- `set_decompiler_comment` @ `0x00416590`, `0x0041acf0` (`R4 todo4`)
- `force_decompile` on all three addresses
- `save_program bulanci.exe`

## Struct doc updates

- [CGameView.md](./CGameView.md) — layout `+0x84`, R4 Ghidra apply note; follow-up UNK cleared for host pointer type

## Remaining UNK

- `CLevelScript_FireOnSlotPlaced_FromView` / `CGameView_FireOnSlotDisplaced` still declared with `(CGameView *)` first parameter — decompile casts `this->pGaming_host` at call sites; fix callee `set_function_prototype` when those symbols are owned (not ECX/`this` on `CGameView_OnEvent`).
- R3 `CAnim_ctor` / `CAnim_RenderAnimFrame` (`0x00419940`, `0x004164f0`) out of R4 scope — already **`CAnim *`** in R3 todo 4.
