# Round 3 — Task 48 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 48 |
| **title** | Prove ODSImage.pOwner CBulanek* on gameplay SetOwner paths |
| **types** | `ODSImage`, `CWeapon`, `CBulanek`, `CDSImage` |
| **addresses** | `0x00439050`, `0x00439100`, `0x0041dbc0`, `0x0042ea80` |
| **supersedes** | R2/R3 todo 48 (handoff) |

## Status

**DONE** — `CBulanek *` on `ODSImage.pOwner` **rejected**; field stays **`CBulanci *`**. `SetOwner` / `ODSImage__SetImage` use `ODSImage *` `this` via `set_function_this_type`. `CWeapon.pOwner` @ `+0x54` is a separate `CBulanek *` field; `CWeapon_ctor` does not xref `SetOwner`.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `ODSImage.pOwner` is `CBulanci *` | `get_struct_layout ODSImage` | `+0x08` → `CBulanci *` (16 B struct) |
| `SetOwner` 2nd param is `CBulanci *` | `ODSImage::SetOwner@0x00439050` | `void __thiscall SetOwner(ODSImage *, CBulanci *)`; `this->pOwner = pOwner` |
| `SetImage` uses `this->pOwner` | `ODSImage::ODSImage__SetImage@0x00439100` | `CBulanci::CDSView__SetSize(this->pOwner, …)` when owner/drawable set |
| Four `SetOwner` xrefs only | xref scan @ `0x00439050` | `CGameView_ctor`, `CBitmap_ViewHeader_Init`, `CDSBitmap_ctor`, `CDSBitmap_ShellCtor` |
| Callers pass embedder `this` as owner | `CGameView_ctor@0x00419200` | `ODSImage::SetOwner(this_00, (CBulanci *)this)` |
| | `CBitmap_ViewHeader_Init@0x00419070` | `SetOwner((ODSImage *)&this->vftable_IDSUpdated, (CBulanci *)this)` (`CAnim *` header) |
| | `CDSBitmap_ctor@0x004393d0` | `ODSImage::SetOwner(this_00, (CBulanci *)this)` |
| | `CDSBitmap_ShellCtor@0x004228f0` | `SetOwner((ODSImage *)(param_1+0x1a), (CBulanci *)param_1)` |
| No `CBulanek` on mixin path | xref scan | **Zero** `SetOwner` xrefs from `CBulanek_ctor` / `CBulanek_ApplyPickupEffect` / `CWeapon_ctor` |
| `CWeapon` ownership separate | `CWeapon_ctor@0x0041dbc0` | `CBulanek *pOwner` param; stores `this->pOwner` @ weapon `+0x54`; uses `ConstructTrackManager`, not `ODSImage::SetOwner` |
| AddRef helper named | `CDSObject_AddRef@0x0042ea80` | `_Globals::CDSObject_AddRef`; called from `ODSImage__SetImage` on drawable |

**Verdict on todo title:** Gameplay **weapon** ownership is `CWeapon.pOwner` (`CBulanek *`). The **0x10 `ODSImage` mixin** `pOwner` always receives the **outer CDSView host** (`CBitmap` / `CGameView` / `CDSBitmap` / `CAnim` base), typed **`CBulanci *`** because `CDSView__SetSize` and blit `+0x30` layout match the app/view prefix — not the heap `CBulanek` actor.

## Ghidra deltas

- Confirmed `ODSImage.pOwner` → **`CBulanci *`** (no change to `CBulanek *`).
- `set_function_prototype` `SetOwner@0x00439050` — already `void SetOwner(ODSImage *, CBulanci *)`.
- **`set_function_this_type`** `ODSImage *` @ `SetOwner@0x00439050`, `ODSImage__SetImage@0x00439100` — decompiler now `ODSImage::SetOwner` / `ODSImage::ODSImage__SetImage` with `this->pOwner`, `this->pDrawable`, `pDrawable->m_slotVector`.
- Decompiler comments retained @ `SetOwner` / `ODSImage__SetImage` (embedder host vs `CBulanek`).
- `CDSObject_AddRef@0x0042ea80` — already named (no rename).
- `save_program bulanci.exe`

## Struct doc updates

- [ODSImage.md](./ODSImage.md) — **Agent todo 48** table (verdict + xref list); Ghidra apply note for `set_function_this_type`.

## Remaining UNK

- Whether `pOwner` should become `CDSView *` once `CDSView` is a verified Ghidra struct (runtime values remain embedder shells).
- `CDSBitmap_ShellCtor` Ghidra symbol still `CDSBitmap_SubobjectCtor` in some listings (rename from batch 43; cosmetic).
