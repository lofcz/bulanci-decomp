# Struct recovery batch 3/50 — slice 03 agent

**Batch index:** 3 (`batches_50.json`)  
**Structs:** `CAnim`, `CBitmap`  
**Protocol:** `AGENT_PROTOCOL.md` (evidence-only)  
**Agent:** slice **03** (2026-05-30)

## Results

| Struct | Status | Size | Ghidra type |
|--------|--------|------|-------------|
| `CAnim` | PARTIAL | `0xf0` (240) | **`CAnim`** — rebuilt (was 188 B truncated) |
| `CBitmap` | PARTIAL | `0x98` (152) | **`CBitmap`** — verified; `ODSImage` @ `+0x88` |

## Key findings

### CAnim (`0xf0`)

- **Allocation:** `OperatorNewWithBadAlloc(0xf0)` in `CBulanci_AllocAnimFromSprite@0x0041cfb0`, `CLevelScriptOpExt_CreateAnim@0x0041d6e0`, `CScoreCtor@0x00411010`.
- **Not** a subclass of `CDSAnim` (`0xd0`); widget-framed sibling with `IDSAnim` at `+0x8c`, inner at `+0x98`, track manager at `+0xa8` (`CAnim_SubobjectCtor` → `ConstructTrackManager(this+0x2a)`; `CAnim_dtor` → `TM_Destructor(this+0xa8)`).
- **Ctor:** `CAnim_ctor@0x00419940` — `CBitmap_ViewHeader_Init` → `AnimInner_InitParam(&vftable_anim_sub)` → CAnim vtables; coords `+0x20/+0x24`.
- **Runtime:** `CAnim_RenderAnimFrame@0x004164f0` → `TM_RenderFrame(&vftable_anim_sub,…)`.

### CBitmap (`0x98`)

- **Heap:** `CreateImage@0x0041d820` → `OperatorNew(0x98)` → `CGameView_ctor` patches **`CBitmap`** vtables.
- **Shared header** with `CAnim` prefix via `CBitmap_ViewHeader_Init@0x00419070`.
- **Draw:** `CBitmap_OnDraw@0x00419130` → `TM_TickBlit(this+0x88)`.
- **Script:** `CBitmap_FireOnBitmapEvtFromView@0x00419280` (IDSAnim thunk); slot byte `+0x70`.

## Ghidra MCP actions (slice 03)

1. `get_struct_layout` — found `CAnim` truncated at 188 B (missing `pTrack_manager`).
2. `delete_data_type CAnim` → `create_struct CAnim` (240 B, 42 fields).
3. `modify_struct_field ODSImage` — `pOwner` → `CBulanci *` @ offset 8.
4. `rename_function_by_address` — `0x00419070` → `CBitmap_ViewHeader_Init`.
5. `set_function_prototype` — `CAnim_ctor`, `CGameView_ctor`, `CBitmap_ViewHeader_Init`, `CAnim_RenderAnimFrame`.
6. `save_program bulanci.exe`.

## Deliverables

- [`CAnim.md`](CAnim.md)
- [`CBitmap.md`](CBitmap.md)

## Blockers / follow-up

- `pTrack_manager` (+0xa8..+0xef) — opaque 72 B blob; inner `CDSUpdatedItem` / scheduler fields not split.
- `CBitmap` / shared header — `wViewFlags@+0x14` not in Ghidra `CBitmap` (same gap as pre-rebuild `CAnim`).
- Mid-header pads (`+0x28..+0x67`, `dwField_40`, `pChain_pad_48`) — no named consumers in this slice.
- Ghidra API: `__thiscall` `this` stays `void*` in decompiler despite `CAnim *` prototypes.
