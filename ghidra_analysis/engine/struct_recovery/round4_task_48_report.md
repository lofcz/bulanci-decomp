# Round 4 — Task 48 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 48 |
| **round** | 4 |
| **title** | Evaluate ODSImage.pOwner as CDSView* embedder host |
| **one_liner** | Ghidra `CDSView` (128 B, `CWindow@0`) does not match runtime `SetOwner` / blit consumers; keep `CBulanci *` on `pOwner`. |
| **prior** | [round3_task_48_report.md](./round3_task_48_report.md) |
| **structs** | ODSImage, CDSView, CWeapon, CBulanek, CBitmap, CDSBitmap, CAnim |

## Status

**DONE** — **`CDSView *` rejected**; `ODSImage.pOwner` remains **`CBulanci *`**. Ghidra comments + `save_program` applied.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `CDSView` layout ≠ embedder hosts | struct | `CDSView` 128 B: `CWindow` @ `+0`, `nDest_x` @ `+0x74` |
| Embedder hosts share `+0x20` origin pair | `CBitmap`, `CAnim`, `CDSBitmap` | `nOrigin_x` / `nBbox_left` @ `+0x20`, `+0x24` |
| `CDSView__SetSize` uses `+0x20`/`+0x24` | `0x0042cbf0` | `MOV EDX,[ECX+0x24]`; `MOV EAX,[ECX+0x20]` before vfn `+0x20` |
| Blit uses `pOwner+0x30` bounds | `0x00439080` | `TM_TickBlit`: `BlitDispatch(..., (int *)(iVar2+0x30), …)` with `iVar2 = *(param_1+8)` |
| Four `SetOwner` callers unchanged | `0x00439050` xrefs | `CGameView_ctor`, `CBitmap_ViewHeader_Init`, `CDSBitmap_ctor`, `CDSBitmap_SubobjectCtor` |
| Caller passes outer shell `this` | `0x00419258` | `SetOwner(&ods_image, (CBulanci *)this)` — `CGameView*` / `CBitmap` layout, not `CDSView` |
| `CWeapon` still separate | `0x0041dbc0` | No `SetOwner` xref; `CWeapon.pOwner` @ `+0x54` (`CBulanek*`) |
| Decompile after R3 `this` types | `0x00439050`, `0x00439100` | `ODSImage::SetOwner` / `ODSImage__SetImage` with `this->pOwner` (`CBulanci *`) |

### Verdict table

| Candidate type | Result | Reason |
|----------------|--------|--------|
| `CBulanek *` | **Rejected** (R3) | Zero `SetOwner` xrefs from `CBulanek` / `CWeapon_ctor` |
| `CDSView *` | **Rejected** (R4) | Ghidra struct is menu/window `CDSView`; consumers use **CDSChained view shell** (`CBitmap`/`CAnim`/`CDSBitmap`) offsets |
| `CBulanci *` | **Keep** | Namespaces `CBulanci::CDSView__SetSize`; opaque host pointer matching proven `+0x20`/`+0x30` usage |
| `CBitmap *` | **Deferred** | Accurate for `CGameView`/`CAnim` paths; wrong label for `CDSBitmap` embed (`trackImage@+0x68`) |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `get_struct_layout` | `ODSImage`, `CDSView`, `CBitmap`, `CDSBitmap`, `CGameView` | `pOwner` still `CBulanci *` @ `+8`; layout mismatch documented |
| `modify_struct_field` (dry-run) | `ODSImage.pOwner` → `CDSView *` | Succeeds in dry-run; **not applied** |
| `get_xrefs_to` | `SetOwner@0x00439050` | 4 callers (same as R3) |
| `decompile_function` | `SetOwner`, `ODSImage__SetImage`, `CDSView__SetSize`, `CGameView_ctor` | Confirmed offset consumers |
| `disassemble_function` | `CDSView__SetSize@0x0042cbf0` | `[ECX+0x20]` / `[ECX+0x24]` |
| `set_decompiler_comment` | `0x00439050`, `0x00439100` | R4 rejection rationale |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [ODSImage.md](./ODSImage.md) — R4 todo 48 evaluation table; UNK updated (`CDSView*` ruled out).

## Remaining UNK

- Introduce shared **CDSChained view base** struct (or per-class `CBitmap *` / `CDSBitmap *` union) when MI embed offsets are catalogued.
- `CDSBitmap_SubobjectCtor` Ghidra symbol name vs doc `CDSBitmap_ShellCtor` (cosmetic).
- `CWeapon_ctor` ECX `this` typing (`CWeapon *`) — agent todo 49 R4 verify track.
