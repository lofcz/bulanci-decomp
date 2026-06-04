# Round 9 `_Globals` FUN — Task 028 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 28 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044c1d9` |
| **ghidra_name** | `FUN_0044C1D9` |
| **prior_hint** | (empty) |

## Status

**PARTIAL** — Role proven: MSVC CRT `__onexit` prolog that acquires lock index **8** via `Runtime::MSVCRT::__lock@0x00449bf3`. **No rename** (one-instruction CRT micro-stub; R5/R8 CRT policy — paired `__unlock(8)` stub `FUN_0044c1e2` and epilog `FUN_00447e6c` also kept as `FUN_*`).

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044c1d9` | `FUN_0044c1d9` | *(none — keep `FUN_*`)* | **`void __cdecl(void)`** CRT lock thunk: `__lock(8)` before `__onexit_nolock`; sole caller `Runtime::MSVCRT::__onexit` | See below |

### Disassembly (Ghidra)

```
0044c1d9  PUSH 0x8
0044c1db  CALL 0x00449bf3          ; Runtime::MSVCRT::__lock
0044c1e0  POP  ECX
0044c1e1  RET
```

Size **`0x9`** (9 B) — matches `config/bulanci/mapping.csv`.

### Callee

| Address | Ghidra name | Role |
|---------|-------------|------|
| `0x00449bf3` | `Runtime::MSVCRT::__lock` | Ghidra library match; `EnterCriticalSection` on `(&DAT_004b1038)[index*2]` (R5 worker 16 / worker 48 proof) |

Lock index **8** is from the immediate **`PUSH 0x8`** — not inferred from CRT headers.

### Caller / pairing (xref closure)

| From | Kind | Context |
|------|------|---------|
| `0x00447e42` | CALL | `Runtime::MSVCRT::__onexit@0x00447e36` — Ghidra **Library Function - Single Match** (`__onexit`, VS2005 Release) |

Decompile of `__onexit`:

```c
_Globals::FUN_0044c1d9();              // lock(8)
p_Var1 = (_onexit_t)__onexit_nolock(_Func);
_Globals::FUN_00447e6c();              // epilog → FUN_0044c1e2 → __unlock(8)
return p_Var1;
```

Paired unlock stub @ `0x0044c1e2` (task 029, out of scope):

```
0044c1e2  PUSH 0x8
0044c1e4  CALL 0x00449b1b          ; __unlock (R5 worker 48 rename)
0044c1e9  POP  ECX
0044c1ea  RET
```

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044c1d9;0x44c1d9;0x9;__stdcall;;uchar
```

Ghidra-corrected: **`void __cdecl(void)`** — body ends in bare `RET` with no meaningful `AL`; `__stdcall`/`uchar` are export-tree artifacts.

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_0044c1d9() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044c1d9` | `void FUN_0044c1d9(void)` + `__cdecl` |
| `set_decompiler_comment` | `0x0044c1d9` | Proven __onexit / __lock(8) role |
| `set_plate_comment` | `0x0044c1d9` | `CRT __onexit lock thunk: __lock(8)` |
| `force_decompile` | `0x0044c1d9` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

**No rename** — not a game, IJG, libmad, or zlib symbol; no unique upstream export name for this 9-byte wrapper.

## Frida

**none** — CRT-only xref closure; static disasm + caller decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Official MSVC symbol for lock index 8 | Not in PE exports; cannot assign `_lockonexit`-class name without upstream CRT label |
| `FUN_0044c1e2` / `FUN_00447e6c` rename | Task 029 / separate FUN — same CRT micro-stub policy |
| `mapping.csv` / `_Globals.cpp` stub sync | Out of scope for single-FUN task |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — `__lock` / `__unlock` pair @ `0x00449bf3` / `0x00449b1b`
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` rename proof
- [r9_globals_task_039_report.md](r9_globals_task_039_report.md) — sibling CRT SEH epilog thunk pattern
