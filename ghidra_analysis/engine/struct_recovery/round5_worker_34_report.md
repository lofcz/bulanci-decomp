# Round 5 — Worker 34 report (`CScrollBar` remaining UNKs)

## Task

| Field | Value |
|-------|-------|
| **worker** | 34 / 50 |
| **mode** | WRITE |
| **target** | [CScrollBar.md](./CScrollBar.md) — close R4 **Remaining UNK** (shell dwords, Render `this`, global helpers) |
| **manifest note** | Parent dispatch scoped **CScrollBar** (not manifest todo id 34 = CDSFont) |
| **prior** | [pass_r4_CScrollBar_report.md](./pass_r4_CScrollBar_report.md), R4 task 20 |

## Status

**DONE** — Shell `dwField_*` band has **no CScrollBar-specific consumers** (instruction sweep). `CScrollBar_Render` / `SetValue` / `SetScrollRange` / `OnMouseMove` / `OnTimerTick` retyped with `CScrollBar *` this. `wChainInit44` @ `+0x44` proven tint input in `Render`.

## Evidence

### Shell fields `+0x08`, `+0x0c`, `+0x1c`, `+0x50`, `+0x5c`, `+0x60`

| Claim | Method | Evidence |
|-------|--------|----------|
| No `+0x50`/`+0x5c`/`+0x60` in scrollbar code | `search_instructions` | Zero matches in `CScrollBar_HitTest`, `SetValue`, `SetScrollRange`, `BuildAt`, `dtor` |
| `+0x08` in `BuildAt` is bitmap dim | `CScrollBar_BuildAt@0x00408473` | `[EAX+0x8]` on loaded `CDSImage*`, not view `this` |
| `+0x08` in `Render` is bitmap dim | `CScrollBar_Render@0x004035c9` | `[EDI+0x8]` on top-cap `CDSImage` (`nM_height`) |
| Writers remain shared shell | `CDSChained_ctor@0x004032d0` | Called from `CScrollBar_ctor@0x00404790`; `ResetChainCounters@0x0042beb0` for `+0x40..+0x50` |

### `wChainInit44` @ `+0x44`

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Tint from chain-init ushort | `CScrollBar_Render@0x00403537` | `MOVZX EAX, byte ptr [ESI+0x44]` → `WidgetStateFlags_ToTintColor(al & 6)` |
| Post-decompile field name | `force_decompile@0x00403530` | `(byte)this->wChainInit44 & 6` |

### Decompiler / helpers (closed)

| Symbol | Address | R5 fix |
|--------|---------|--------|
| `CScrollBar_Render` | `0x00403530` | `void __thiscall CScrollBar_Render(CScrollBar *this)` + `set_function_this_type` |
| `CScrollBar_SetValue` | `0x00404f70` | Moved into class `CScrollBar`; uses `this->nMinValue`/`nMaxValue`/`nCurrentValue`/`pParent` |
| `CScrollBar_SetScrollRange` | `0x004051d0` | Same; `CDSView__Show`/`Hide` when range empty |
| `CScrollBar_OnMouseMove` | `0x00402f60` | `CScrollBar *` this |
| `CScrollBar_OnTimerTick` | `0x00403a80` | `CScrollBar *` this |

### Size unchanged

| Claim | Evidence |
|-------|----------|
| `sizeof(CScrollBar)==0xcc` | `get_struct_layout` → **204 bytes** (unchanged) |

## Ghidra deltas

1. `set_function_this_type` `CScrollBar *` @ `0x00403530`, `0x00404f70`, `0x004051d0`, `0x00402f60`, `0x00403a80`
2. `set_function_prototype` __thiscall on same five symbols
3. `force_decompile` @ `0x00403530`, `0x00404f70`, `0x004051d0`
4. `set_decompiler_comment` @ `0x00403537`, `0x00404f70`
5. `save_program bulanci.exe`

## Struct doc updates

- [CScrollBar.md](./CScrollBar.md) — R5 Ghidra apply; `wChainInit44` consumer; UNK trimmed

## Remaining UNK

- Shared `CDSChained` dword semantics for `dwField_08` / `dwField_0c` / `dwField_1c` / `dwField_50` / `dwField_5c` / `dwField_60` — **not scrollbar-specific**; track under [CDSChained.md](./CDSChained.md) / R5 todo 21.
- `CScrollBar_Render` decompiler may still route some `pBitmap_thumb*` / `pBitmap_bottom*` through `pPad_a5` pointer math; struct layout `+0x80..+0xa0` is correct in Ghidra struct editor.
- `FUN_0042ecc0` notify helper namespace not reparented.
