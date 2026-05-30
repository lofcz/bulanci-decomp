# CDSView

## Status

**VERIFIED** — engine view shell **128 B** (`0x80`): **`CWindow win`** @ `+0x00` (112 B dialog / drawable prefix) + **16 B** extension @ `+0x70..+0x7F`. Relates to [CWindow.md](./CWindow.md) (`0x70` dialog base) and dialog subclasses ([CSessionList.md](./CSessionList.md), [CMsgDialog.md](./CMsgDialog.md)).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Dialog heap `0x70` | `0x0040e780` | `CMsgDialog_Allocate` → `OperatorNew(0x70)` — **CWindow** size only |
| Session dialog `0x7c` | `0x0040e980` | `CSessionList` factory → `OperatorNew(0x7c)` = `CWindow` + 3 child pointers @ `+0x70..+0x78` |
| Ghidra `CDSView` | — | `get_struct_layout` → **128 bytes** (agent todo 11, 2026-05-30) |
| Ghidra `CWindow` | — | **112 bytes** — first component of `CDSView` |

## Layout table

| Offset | Size | Type | Name | Notes |
|--------|------|------|------|-------|
| `0x00` | 112 | `CWindow` | `win` | Dialog prefix: vtables, bbox `+0x20..+0x2c`, chain band `+0x40..+0x50`, `bModalFlag` @ `+0x68`, `pDefaultFocusChild` @ `+0x6c` |
| `0x70` | 1 | `byte` | `bGaming_slot_id` | `CAnim` / `CGameView` gaming slot; unused on pure dialogs |
| `0x71` | 3 | `byte[3]` | `pPad_71` | padding |
| `0x74` | 4 | `int` | `nDest_x` | gameplay coord band; **`CSessionList.pSessionListBox`** aliases this offset |
| `0x78` | 4 | `int` | `nDest_y` | gameplay; **`CSessionList.pCaptionStatic`** aliases |
| `0x7c` | 4 | `int` | `nSrc_x` | gameplay; **`CSessionList.pJoinButton`** aliases |

**Union @ `+0x68..+0x6f` (inside `win`):** dialog uses `bModalFlag` + `pDefaultFocusChild`; gameplay views (`CAnim`, `CGameView`) use `bView_state_*` / `dwView_aux_6c` at the same offsets — do not apply both interpretations on one instance.

## Key functions

| Symbol | Address | Role |
|--------|---------|------|
| `CWindow_BuildAt` | `0x00405560` | Initializes **`win`** prefix only (dialogs / containers) |
| `CDSView_SetRect` | `0x0042c480` | Updates `win.nBbox_*`, tests `win` view flags @ `+0x44`, walks `child_chain` @ `win+0x54` |
| `CMsgDialog_Allocate` | `0x0040e780` | Heap object **112 B** — no `+0x70..+0x7f` extension |
| `CSessionList_BuildDialog` | `0x0040c2d0` | `CWindow_BuildAt(&this->win,…)` then stores children @ `+0x70..+0x78` |

## Ghidra apply

```
delete_data_type CDSView
create_struct CDSView  (CWindow win @0 + extension @0x70..0x7c)
create_pointer_type CDSView *
set_function_prototype CDSView_SetRect@0x0042c480 → void __thiscall CDSView_SetRect(CDSView *this, int *rect)
set_decompiler_comment / set_plate_comment @0x00405560, @0x0042c480
save_program bulanci.exe
```

## UNK

- Whether any type uses all four extension dwords through `+0x7f` on the base 128 B shell (vs larger subclasses like `CGameView` `0x98+`).
- Full semantic names for `win.dwField_08` / chain dwords (shared with `CDSChained`).

## Cross-links

- [CWindow.md](./CWindow.md) — `0x70` dialog base embedded in `CDSView.win`
- [CSessionList.md](./CSessionList.md) — `CWindow win` + pointer tail @ `+0x70..+0x78`
- [CGameView.md](./CGameView.md) / [CAnim.md](./CAnim.md) — flat header mirrors `win` offsets through `+0x67`, gaming bytes @ `+0x68+`
