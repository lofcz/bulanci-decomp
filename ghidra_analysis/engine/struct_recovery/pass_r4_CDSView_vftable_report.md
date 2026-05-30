# Round 4 — CDSView_vftable_t pass

## Task

Document **`CDSView_vftable_t`** / CDSView primary and MI vtable slots (AddChild, SetSize, hit-test, draw, event dispatch) with named function pointers where provable. Ghidra: label globals, plate comments, `save_program`.

## Status

**DONE** — struct already present in Ghidra; globals labeled; key thunks commented; markdown + save.

## Evidence summary

| Claim | Address | Evidence |
|-------|---------|----------|
| 28-slot primary | `0x0047f954` | `CDSView_vftable_t` size 112 B; `CDSChained_ctor` stamps `[ESI]=0x47f954` |
| Hit-test slot 7 | `0x004028a0` | `CDSView_HitTest` — screen bbox `+0x30..0x3c` |
| Draw slot 14 | `0x0042ccf0` | `CDSView_RenderChildrenClipped` — child walk `+0x54`, child `vfn+0x38` |
| SetSize → vfn 8 | `0x0042cbf0` | `CDSView__SetSize` decompile: `(*this+0x20)(rect)` → `AdaptDisplaySize@0x0042cae0` |
| AddChild not vfn | `0x0042d0b0` | `CDSView__AddChild` — global helper, `AddChildInternal` |
| Dispatch slot 4 @ +0x10 | `0x0042c040` | `CDSView_DispatchEvent` — routes `0x100/0x200/0x400` → primary 26/27/25 |
| CWindow overrides | `0x0047fd5c` | Slot 14 `CWindow_Render`; slot 0 `CWindow_GetTypeDescriptor` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `get_struct_layout` | `CDSView_vftable_t` | 28 named fields (`pHitTest` … `pOnEvent`) |
| `set_global` | `0x0047f954` | `g_pCDSView_vftable_primary` + `CDSView_vftable_t *` + plate |
| `set_global` | `0x0047f938` | `g_pCDSView_vftable_IDSChained` |
| `set_global` | `0x0047f920` | `g_pCDSView_vftable_IDSEventHandler` |
| `set_global` | `0x0047f90c` | `g_pCDSView_vftable_field18` |
| `set_global` | `0x0047fd5c` | `g_pCWindow_vftable_primary` |
| `set_plate_comment` | `CDSView__AddChild`, `CDSView__SetSize`, HitTest, SetRect, AdaptDisplaySize, RenderChildrenClipped | applied |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSView_vftable.md](./CDSView_vftable.md) — **new** canonical slot table
- [CDSView.md](./CDSView.md) — vtable section + cross-link

## Remaining UNK

- `g_pCDSView_vftable_field18` per-slot semantics.
- Widget-specific overrides for reserved slots 15–18 and slot 27 (`pOnEvent`).
