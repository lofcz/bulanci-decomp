# Struct recovery batch 43/50

**Structs:** `ODSImage`, `type_info`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile, `get_struct_layout`, `create_struct`, `master_vtable_catalog.csv`, `bulanci.ghidra.exe.c`

## Results

| Struct | Status | Recovered size | Ghidra size | Applied |
|--------|--------|----------------|-------------|---------|
| `ODSImage` | **VERIFIED** (mixin `0x10`) | `0x10` | 16 | Yes (`create_struct`, saved) |
| `type_info` | **SKIP** (CRT RTTI) | — (opaque CRT) | 1 (placeholder) | No |

## ODSImage highlights

- **Not** the `0x218` menu cursor object — that is `CGunMouse` (`CGunMouse_CreateObject` @ `0x00426500`); recovered separately in batch 8.
- Canonical **embedded** mixin: `ODSImage_ctor` @ `0x00418c00` + `SetOwner` @ `0x00439050` + `ODSImage__SetImage` @ `0x00439100`.
- Two independent embed offsets: `CDSBitmap` @ `this+0x68`, `CGameView` / `CBitmap` @ `this+0x88` (both call the same ctor on a `0x10`-byte subobject).

## type_info highlights

- Library: Visual Studio 2005 Release (`~type_info` @ `0x0044734d`, `_Type_info_dtor` @ `0x004498d0`).
- CRT SKIP per protocol — no program mutation.

## Ghidra actions

- [x] `delete_data_type ODSImage` (clear size-1 placeholder)
- [x] `create_struct ODSImage` (4 fields, 16 bytes)
- [x] `get_struct_layout ODSImage` → Size: 16
- [x] `get_struct_layout type_info` → Size: 1 (SKIP)
- [x] `save_program bulanci.exe`
- [x] **Slice 43** — `modify_struct_field` `pOwner` → `CBulanci *` (fixed `-BAD-`); restore field name `pOwner`; `set_function_prototype` `SetOwner@0x00439050`; `save_program`

## Deliverables

- `ghidra_analysis/engine/struct_recovery/ODSImage.md`
- `ghidra_analysis/engine/struct_recovery/type_info.md`
- `ghidra_analysis/engine/struct_recovery/batch_43_summary.md`

## Agent todo 48 — namespace cleanup (2026-05-30)

- [x] `ODSImage::SetImage` → `ODSImage__SetImage` @ `0x00439100`
- [x] `CDSBitmap::CDSBitmap_SubobjectCtor` → `CDSBitmap_ShellCtor` @ `0x004228f0`
- [x] `CWeapon_ctor` moved from `ODSImage` namespace → **Global** (inline script); no `ODSImage::` functions remain
- [x] `CBitmap_ViewHeader_Init` @ `0x00419070` — already named (slice 03)
- [x] `save_program bulanci.exe`

## Follow-ups

- Type `pDrawable` once `CDSImage` / resource drawable struct is verified (batch 28 manifest).
- ~~Manual decompiler retype of `CWeapon_ctor` `this`~~ — **done** (R3 todo 49: `set_function_this_type` `CWeapon*`).
- **Agent todo 49:** `CWeapon` **112 B**, `trackManager` `CDSVideoPlayer` @ +8..+0x4F — see `CWeapon.md`, `round3_task_49_report.md`.
