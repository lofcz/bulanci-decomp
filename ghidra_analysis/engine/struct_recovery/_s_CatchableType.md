# _s_CatchableType

## Status

**SKIP** — MSVC CRT C++ EH metadata (`Visual Studio 2005 Release`). Describes a catchable type in `_s_ThrowInfo`; instances live in `.rdata` as compiler-generated throw tables. No game `OperatorNew`, ctor, or field write in bulanci code.

**Size/layout: VERIFIED** (`0x1C`). Ghidra struct at `/_s_CatchableType` (28 bytes, 7 fields). Demangler stub `/Demangler/_s_CatchableType` remains size **1** and blocks `set_function_prototype` on `___TypeMatch` / `CatchIt` (`Can't resolve datatype: _s_CatchableType *`); `FindHandler` `local_24` typed via `set_local_variable_type` (agent slice 46, 2026-05-30).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Minimum span through `copyFunction` at `+0x18` | `0x0044b5ef` | `___BuildCatchObjectHelper`: `CMP dword ptr [EDI + 0x18], 0` before validating copy helper |
| `sizeof == 0x1C` (28) — last dword ends at `0x1B`, struct aligned to 28 | (inferred) | No field access at `>= 0x1C`; VS2005 `CatchableType` / `_s_CatchableType` is 28 bytes on x86 (4+4+12+4+4) |
| Storage is pointer array, not inline array | `0x0044ba70` | `FindHandler`: `ThrowInfo+0xC` → count at `[0]`, iterate `ADD EBX, 4` over `CatchableType*` entries |
| Placeholder demangler type wrong | (Ghidra) | `/Demangler/_s_CatchableType` Size: **1** |
| No game allocation | — | All consumers are CRT EH (`FindHandler`, `CatchIt`, `___BuildCatchObject*`, `___TypeMatch`); xrefs only in `0x0044bxxx` MSVCRT |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `uint` | `properties` | `___BuildCatchObjectHelper@0x0044b535` `TEST dword ptr [EAX],0x80000000`; `@0x0044b555` `TEST AL,0x8` (bit 3); `___TypeMatch@0x0044b09d` `*param_2 & 2` (`param_2` = catchable, `FindHandler@0x0044ba88`) |
| `0x04` | 4 | `TypeDescriptor *` | `pType` | `___BuildCatchObjectHelper@0x0044b51a` `[EAX+0x4]`, null/name check `[ECX+0x8]`; `___TypeMatch@0x0044b09d` compare `*(param_1+4)` vs `*(param_2+4)` and `_strcmp` on `name` at `TypeDescriptor+8` |
| `0x08` | 4 | `int` | `thisDisplacement.mdisp` | `___BuildCatchObjectHelper@0x0044b52e` `[EAX+0x8]`; `@0x0044b54a` `LEA ESI,[ECX+ESI+0xc]` when `properties >= 0` (`ECX` = mdisp) |
| `0x0C` | 4 | `int` | `thisDisplacement.pdisp` | `___AdjustPointer@0x0044b283` on `(int*)(catchable+0x8)` — 12-byte PMD (`mdisp`,`pdisp`,`vdisp`) |
| `0x10` | 4 | `int` | `thisDisplacement.vdisp` | same PMD block |
| `0x14` | 4 | `int` | `sizeOrOffset` | `___BuildCatchObjectHelper@0x0044b5c7` `PUSH dword ptr [EDI+0x14]` for `_memmove`; `@0x0044b5d9` `CMP [EDI+0x14],4` (pointer-sized catch) |
| `0x18` | 4 | `void *` | `copyFunction` | `___BuildCatchObjectHelper@0x0044b5ef` / `@0x0044b644`; `___BuildCatchObject@0x0044b685` calls via `[param_4+0x18]` after `___AdjustPointer` |

### `properties` flags (this binary, partial)

| Mask | Seen in |
|------|---------|
| `0x80000000` | `___BuildCatchObjectHelper@0x0044b535` — early-out with `[+0x8]==0` |
| `0x8` | `@0x0044b555` — by-reference catch path (`mov` to catch buffer vs `_memmove`) |
| `0x2` | `___TypeMatch` on catchable `*param_2` vs handler `*param_1` |
| `0x1`, `0x2` on throw side | `___TypeMatch` tests `*param_3` (third arg is `_s_ThrowInfo*` from `[EHRecord+0x1c]`, `FindHandler@0x0044ba82`) |

## Ghidra apply

**Applied (follow-up round 2)** — 28-byte struct at `/_s_CatchableType` (CRT metadata; still **SKIP** for game allocation). Demangler placeholder `/Demangler/_s_CatchableType` (size 1) remains; decompiler uses `/_s_CatchableType` where typed (e.g. `CatchIt` → `_s_CatchableType *`).

```
get_struct_layout _s_CatchableType  → Size: 28
  +0x00 uint     dwProperties
  +0x04 pointer  pType
  +0x08 int      nMdisp / +0x0C nPdisp / +0x10 nVdisp  (PMD)
  +0x14 int      nSizeOrOffset
  +0x18 pointer  copyFunction
```

`create_struct` 2026-05-30 (batch 46 follow-up); `set_local_variable_type` `FindHandler@0x0044b874` `local_24` → `_s_CatchableType *` (slice 46); `save_program bulanci.exe`.

## UNK

- Demangler `/Demangler/_s_CatchableType` (size 1) vs `/_s_CatchableType` (28) — blocks automatic prototype apply on `___TypeMatch` / `CatchIt`.
- Exact semantic names for every `properties` bit (only `0x1`, `0x2`, `0x8`, `0x80000000` observed).
- Whether any tail padding exists after `copyFunction` (no access `>= 0x1C`).
- Full `_s_ThrowInfo` layout (`pCatchableTypeArray` at `ThrowInfo+0xC`, count at `[0]`) — separate struct batch.

## Xref summary (CRT only)

| Function | Address | Role |
|----------|---------|------|
| `FindHandler` | `0x0044b874` | Walks `CatchableType**` from throw info; `___TypeMatch`; calls `CatchIt` with matched entry |
| `CatchIt` | `0x0044b716` | Passes catchable to `___BuildCatchObject` (via `EBX` in prologue) |
| `___BuildCatchObject` | `0x0044b685` | Adjust catch buffer; delegate to helper / invoke `copyFunction` |
| `___BuildCatchObjectHelper` | `0x0044b506` | Primary field-level consumer |
| `___TypeMatch` | `0x0044b069` | `properties` + `pType` on catchable (`param_2`) |
