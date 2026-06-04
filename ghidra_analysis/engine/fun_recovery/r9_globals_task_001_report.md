# Round 9 `_Globals` FUN — Task 001 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | early |
| **seed_address** | `0x00405280` |
| **ghidra_name (before)** | `FUN_00405280` |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Renamed to `CRadio_HitTestRowAtPoint` with disasm + sole-caller xref closure; `__thiscall` + `CRadio *` this typed; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Proposed / actual name | Role | Evidence |
|---------|-------------|----------------------|------|----------|
| `0x00405280` | `CRadio_HitTestRowAtPoint` | `CRadio_HitTestRowAtPoint` | **Option-row hit test:** given client `(x,y)` in `param_1`, return row index `0..n-1` or `0xFF` (miss / disabled / out of bbox) | See below |

### Caller closure (primary proof)

| Caller | Site | Behavior |
|--------|------|----------|
| `CRadio_OnMouseMove` | `0x00406a28` | `iVar1 = CRadio_HitTestRowAtPoint(this, param_1)`; if `(char)iVar1 != *(char *)(this+0x6a)` then store `+0x6a` and vtable `+0x24` invalidate |

`get_xrefs_to(0x00405280)` → **1** xref (unconditional call from `CRadio_OnMouseMove` only).

`CRadio_OnMouseDown@0x00402fb0` reads `this->bHoverIndex` (`+0x6a`) set by the mouse-move path. `CRadio_Invalidate@0x00403020` clears `+0x6a` to `0xFF`.

### Disasm logic (`0x00405280`–`0x004052d2`, size `0x53`)

| Step | Instruction pattern | Meaning |
|------|---------------------|---------|
| 1 | `CMP [ECX+0x30..0x3c]` vs `param_1[0/1]` | Point inside widget bbox (`CDSView` rect fields) |
| 2 | `SUB EAX,[ECX+0x34]`; `IDIV` by `([ECX+0x94]+8)+2` | Row index = `(y - top) / (bitmap_height + 2)` |
| 3 | `CMP EAX,[ECX+0x74]` | Row `< bOptionCount` |
| 4 | `[ECX+0x6c+EAX*4]` → `TEST [EDX+4],1` | Disabled option if bit 0 set → return `0xFF` |
| 5 | Miss path | `OR AL,0xFF` / `MOV EAX,0xFF` |

Post-rename decompile uses typed fields: `pBitmap0`, `pLabelPtrs`, `bOptionCount` (Ghidra `CRadio` struct).

### Signature / mapping

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `int __thiscall(void* this, int* param_1)`; size **`0x53`** |
| `_Globals.cpp` stub | `FUN_00405280` — **not trusted** (empty stub) |

### Prior art

| Source | Note |
|--------|------|
| `round5_worker_06_report.md` | Listed `0x00405280` as BLOCKED “Unverified widget helpers” — **superseded** by live Ghidra proof |
| `fun_recovery/` grep | No prior R9 report for this VA |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x00405280` | `int CRadio_HitTestRowAtPoint(int * param_1)`, `__thiscall` |
| `set_function_this_type` | `0x00405280` | `CRadio *` (moved into class `CRadio`) |
| `rename_function_by_address` | `0x00405280` | `CRadio_HitTestRowAtPoint` |
| `set_decompiler_comment` | `0x00405280` | Confirmed row/hover semantics |
| `force_decompile` | `0x00405280` | Refreshed typed decompile |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static disasm + single caller + `bHoverIndex` field chain sufficient.

## Remaining UNK

- `CRadio_OnMouseMove@0x00406a20` still uses `void * this` in `_Globals` namespace (separate task; vtable thunks at `0x00407700`).
- `mapping.csv` / `_Globals.h` / `_Globals.cpp` not updated in this FUN-only pass.
