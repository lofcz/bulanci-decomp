# ODSImage

## Status

**VERIFIED** — mixin subobject size `0x10` (16 bytes). Four pointer fields at `+0`…`+0xC` proven by ctor, `SetOwner`, `ODSImage__SetImage`, and `TM_TickBlit`. Larger stand-alone types (`CGunMouse` `0x218`, `CWeapon` track object) share the Ghidra `ODSImage::` namespace but are **not** this layout; see `CGunMouse.md`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(ODSImage) == 0x10` | `ODSImage_ctor` @ `0x00418c00` | Last ctor dword write `*(this+0xc)=0` → high water `+0xC` + 4 |
| Same span in `CDSBitmap_ctor` | `0x004393d0` | `ODSImage_ctor(this+0x68)`; zeros at `+0x70`/`+0x74` (`param_1[0x1c]`/`[0x1d]`) → `0x68+0x10=0x78` |
| Same span in `CGameView_ctor` | `0x00419200` | `ODSImage_ctor(this+0x88)`; parent writes `this+0x8c` (`+0x4` within subobject) |
| Embedded offset in `CBitmap` | `CBitmap_OnDraw` @ `0x00439150` area | `TM_TickBlit(param_1+0x88)`; dtor thunk `CBitmap_ScalarDeletingDtor_thunk_Sub88` (`this-0x88`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_primary` | `CGameView_ctor@0x00419200` `*(ODSImage*)=CBitmap::vftable`; `CDSBitmap_ctor@0x004393d0` `*(this+0x68)=vftable` |
| `0x04` | 4 | `void *` | `vf_odsimage` | `ODSImage_ctor@0x00418c00` `*(this+4)=0x481ab4`; `CGameView_ctor@0x00419240` repatches embedder `CBitmap+0x8c` → `0x481f00` (IDSAnim); `CBulanek` shell @ `0x00418d70` `param_1[0x28]` |
| `0x08` | 4 | `CBulanci *` | `pOwner` | `ODSImage_ctor@0x00418c00` zero; `SetOwner@0x00439050` `this->pOwner=param_2`; `ODSImage__SetImage@0x00439100` + `TM_TickBlit@0x00439080` read `+8` |
| `0x0C` | 4 | `CDSImage *` | `pDrawable` | `ODSImage_ctor@0x00418c00` zero; `ODSImage__SetImage@0x00439100` stores `CDSImage*`, uses `+0x38` slot vector; `SetOwner@0x00439050` reads `nM_width`/`nM_height` (`CDSImage` `+4`/`+8`) |

## RTTI / vtable

| Item | Address | Evidence |
|------|---------|----------|
| `ODSImage` vtable (`IDSChained` cluster) | `0x00481ab4` | `master_vtable_catalog.csv` — size `0xA0`, slots include `CDSView_OnMouseStub` |

## FLX opcode 0x0C — `vf_odsimage` vfn[4] (agent todo 31)

`CDSFlxFile::DecodeFrame` opcode `0x0C` → `BroadcastFrameTimeHint@0x00436ef0` walks `CDSImage.m_slotVector` and calls each subscriber’s **vfn[4]** (`vtable+0x10`) as `(consumer, u16)`. Subscribers are registered by `ODSImage__SetImage@0x00439100` (`ODSImage+4` face). **Not** `frameDelayOverrideMs` — see [CDSFlxFile.md](./CDSFlxFile.md).

| `pVf_odsimage` | slot[4] target | Role |
|----------------|----------------|------|
| `0x00481ab4` (default) | `CDSView_OnMouseStub@0x00416770` | no-op for u16 hint |
| `0x00481f00` (`CBitmap` / `CGameView`) | `CBitmap_FireOnBitmapEvtFromView@0x00419280` | `OnBitmapEvt` script bridge |
| `0x004830e0` / `0x004831c8` (`CDSBitmap` / `CMovieView`) | `CDSView_OnMouseStub@0x00416770` | anim embed paths |
| `0x00483794` (`CGunMouse`) | `CGunMouse_OnAnimTick@0x00423bd0` | cursor weapon tick |
| `0x00481ed4` (`CWeapon`) | `CWeapon_Fire@0x004212b0` | weapon overlay |

## Ghidra apply

```
delete_data_type ODSImage   # removed size-1 placeholder (batch 43)
create_struct ODSImage: vf_primary@0, vf_odsimage@4, pOwner@8, pDrawable@12
modify_struct_field ODSImage pDrawable → CDSImage * (batch 43 follow-up; `CDSImage` verified batch 31)
modify_struct_field ODSImage pOwner → CBulanci * (round 3 task 26)
get_struct_layout ODSImage → Size: 16 bytes; pOwner → CBulanci *
save_program bulanci.exe
```

Post-apply field names in Ghidra: `pVf_primary`, `pVf_odsimage`, `pOwner` (`CBulanci *`), `pDrawable` (`CDSImage *`).

## Slice 43 re-verify (2026-05-30)

| Action | Evidence |
|--------|----------|
| `modify_struct_field` `pOwner` → `CBulanci *` | `get_struct_layout` had `-BAD-` at `+8` (stale/broken pointer) |
| `modify_struct_field` `offset:8` → name `pOwner` | Decompile had `field2_0x8` after type repair |
| `set_function_prototype` `SetOwner@0x00439050` | `void SetOwner(ODSImage *, CBulanci *)` — store + `CDSView__SetSize` |
| Decompile check | `ODSImage_ctor` zeros `pOwner`/`pDrawable`; `SetOwner` assigns `this->pOwner` |

## Follow-up (round 3 task 26)

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `pOwner` is `CBulanci *` | `SetOwner@0x00439050` | Prototype `void SetOwner(ODSImage *, CBulanci *)`; store `this->pOwner = param_2`; calls `CDSView::SetSize@0x0042cbf0` |
| Same type in `SetImage` | `ODSImage__SetImage@0x00439100` | `CDSView::SetSize@0x0042cbf0` on `*(CBulanci **)(this+8)` when owner non-null |
| Blit consumer | `TM_TickBlit@0x00439080` | `iVar2 = *(int *)(param_1+8)`; `BlitDispatch(..., (int *)(iVar2+0x30), …)` — `+0x30` matches `CBitmap` `bounds` (`CBitmap.md`) |
| Callers pass embedder `this` | `CGameView_ctor@0x00419200`, `FUN_00419070@0x00419070`, `CDSBitmap_ctor@0x004393d0`, `CDSBitmap_ShellCtor@0x004228f0` | Each `SetOwner(embedded ODSImage*, outer this)` — outer object is a `CDSView`/`CBitmap` host, not `g_pApp`; Ghidra uses `CBulanci *` because `CDSView__SetSize` is namespaced on `CBulanci` (shared CDSView prefix through `+0x20`/`+0x24`) |

Runtime values are **host views** (`CBitmap` / `CGameView` / `CDSBitmap` / `CAnim` base), not the heap `CBulanci` app singleton — see `CBulanci.md` § ODSImage owner.

## Naming (not mixin size)

| Symbol | Address | Role |
|--------|---------|------|
| `ODSImage_ctor` | `0x00418c00` | `0x10` mixin ctor + `ODSImage__SetImage` |
| `ODSImage__SetImage` | `0x00439100` | Was `ODSImage::SetImage` — mixin only |
| `SetOwner` | `0x00439050` | `void SetOwner(ODSImage *, CBulanci *)` |
| `CDSBitmap_ctor` | `0x004393d0` | Full `CDSBitmap`; calls `ODSImage_ctor(this+0x68, param_3)` |
| `CDSBitmap_ShellCtor` | `0x004228f0` | Was `CDSBitmap::CDSBitmap_SubobjectCtor`; shared shell: `CDSChained_ctor` + embedded `ODSImage@+0x68` vtables **without** `ODSImage_ctor` (manual zero `pOwner`/`pDrawable`); used by `CSwitch`, `CMovieView`, `CDSAnim`, etc. |
| `CBitmap_ViewHeader_Init` | `0x00419070` | Shared `CBitmap`/`CAnim` MI header (not mixin-sized) |
| `CWeapon_ctor` | `0x0041dbc0` | **112 B** `CWeapon` heap ctor — **not** in `ODSImage` namespace (moved Global, todo 48) |

## Agent todo 48 (2026-05-30) — pOwner type verdict

| Check | Result |
|-------|--------|
| `CBulanek*` on `ODSImage.pOwner` | **Rejected** — no `SetOwner` xref from `CBulanek` / `CWeapon_ctor` |
| `SetOwner` callers (4) | `CGameView_ctor`, `CBitmap_ViewHeader_Init`, `CDSBitmap_ctor`, `CDSBitmap_ShellCtor` — each passes **embedder outer `this`** (`CGameView*` / `CAnim*` / `CDSBitmap*`) |
| `CWeapon` ownership | Separate field `CWeapon.pOwner` @ `+0x54` (`CBulanek*`); **112 B** object does not call `SetOwner` on the `0x10` mixin |
| Ghidra repair | `pOwner` was `-BAD-`; reapplied `CBulanci *`; `void SetOwner(ODSImage *, CBulanci *)`; comments @ `0x00439050` / `0x00439100` |
| `0x0042ea80` | Already `CDSObject_AddRef` (slot-vector AddRef in `ODSImage__SetImage`) |

Runtime `pOwner` values are **CDSView hosts** (bitmap/view shells), not the heap `CBulanek` actor — `CBulanci *` documents the shared `CDSView__SetSize` / `+0x30` blit prefix.

## Namespace audit (agent todo 48, 2026-05-30)

| Category | Symbols | Notes |
|----------|---------|-------|
| **Mixin (`0x10`)** | `ODSImage_ctor`, `ODSImage_FactoryCtor`, `ODSImage__SetImage`, `SetOwner` | Legitimate `ODSImage` API; no `ODSImage::` prefix remains on any function |
| **Full objects (mis-namespaced)** | `CWeapon_ctor` was parented under Ghidra `ODSImage` because `this` typed `ODSImage*` | `sym.setNamespace(Global)` @ `0x0041dbc0`; decompile now `CWeapon_ctor(void *this,…)` with offset field access |
| **Other classes** | `CGunMouse_ctor`, `CDSBitmap_ShellCtor` | Never used `ODSImage::` prefix; `CDSBitmap::…SubobjectCtor` renamed to `CDSBitmap_ShellCtor` |
| **Decompiler `this`** | `CWeapon_ctor`, `SetOwner`, `ODSImage__SetImage` | Ghidra MCP cannot retype ECX `this`; plate comment @ `0x0041dbc0` documents `CWeapon *` |

## UNK

- Whether `pOwner` should eventually be a dedicated `CDSView *` (or `CBitmap *`) instead of `CBulanci *` once `CDSView` is a formal Ghidra struct (today all embedders share the `CDSView__SetSize`/`+0x30` blit layout proven on `CBitmap`).
- Whether `vf_primary` and `vf_odsimage` are both required for all embedders (some paths only repatch `vf_primary` after `ODSImage_ctor`).
- `FUN_00419070` / `CWeapon_ctor` / `CGunMouse_ctor` — distinct full-object ctors; not the `0x10` mixin (see `CGunMouse.md`, weapon overlay notes in `damage_pipeline.md`).
- `FUN_0042ea80` AddRef helper in `ODSImage__SetImage` — exact COM base type not named.
