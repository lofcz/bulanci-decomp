# Round 9 `_LocaleUpdate` FUN — Other Task 002 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **round** | 9 |
| **unit** | other |
| **namespace** | `_LocaleUpdate` |
| **seed_address** | `0x0044cf75` |
| **ghidra_name (before)** | `FUN_0044cf75` |
| **prior_hint** | R8 FUN manifest entry; sibling `FUN_0044ae46@0x0044ae46` (`__unlock(0xc)` in `__freefls@4`, R9 globals task 025) |

## Status

**PARTIAL** — Role proven: MSVC CRT `___updatetlocinfo` SEH epilog stub — `__unlock(12)` plus reload of `__updatetlocinfoEx_nolock` result into **ESI** from `[EBP-0x1c]`. **No rename** (protocol: CRT micro-stub; no IJG/libmad/bulanci game symbol; same deferred `FUN_*` policy as R9 `__unlock(0xc)` epilogs). Ghidra prototype + decompiler comment refreshed; **`save_program bulanci.exe`**.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044cf75` | `FUN_0044cf75` | *(keep `FUN_*`)* | **`___updatetlocinfo` locale-lock unlock epilog:** 12-byte stub `push 0xc` → `__unlock` @ `0x00449b1b` → `pop ecx` → `mov esi,[ebp-0x1c]` → `ret`; pairs `__lock(0xc)` in same parent | See below |

### Disassembly (Ghidra live)

```
0044cf75  PUSH 0xc
0044cf77  CALL 0x00449b1b          ; __unlock
0044cf7c  POP  ECX
0044cf7d  MOV  ESI,dword ptr [EBP + -0x1c]
0044cf80  RET
```

| Metric | Value |
|--------|-------|
| PE bytes | **`6a 0c e8 9f cb ff ff 59 8b 75 e4 c3`** (12 B) — matches `mapping.csv` size `0xc` |
| Callee | `__unlock@0x00449b1b` (R5 worker 48; `LeaveCriticalSection` on lock table) |
| Lock index | **12** (`0xc`) — locale/tlocinfo critical section |
| Unique body | Byte pattern `6A 0C E8 ?? ?? ?? ?? 59 8B 75 E4 C3` — **sole hit** @ this VA (unlike generic 9 B `__unlock(0xc)` stubs) |

The extra `MOV ESI,[EBP-0x1c]` reloads the pointer stored by `__updatetlocinfoEx_nolock@0x0044cecd` (`MOV [EBP-0x1c],EAX` @ `0x0044cf64`) before the parent `JMP 0x0044cf36` path uses **ESI** (`TEST ESI,ESI` @ `0x0044cf36`).

### Caller closure (single xref)

| Caller | Call site | Context |
|--------|-----------|---------|
| `_LocaleUpdate::___updatetlocinfo` @ `0x0044cf0b` (body `0x6a` B, Ghidra library match) | `0x0044cf6e` | After `__lock(0xc)` @ `0x0044cf4a` → `__updatetlocinfoEx_nolock()` @ `0x0044cf5f`; SEH cookie `[EBP-0x4]` → `-2`; then `CALL` this stub → `JMP 0x0044cf36` |

Paired acquire in same function:

```
0044cf4a  PUSH 0xc
0044cf4c  CALL 0x00449bf3          ; __lock
0044cf51  POP  ECX
...
0044cf5f  CALL 0x0044cecd          ; __updatetlocinfoEx_nolock
0044cf64  MOV  dword ptr [EBP + -0x1c],EAX
0044cf67  MOV  dword ptr [EBP + -0x4],0xfffffffe
0044cf6e  CALL 0x0044cf75          ; this stub
0044cf73  JMP  0x0044cf36
```

### Sibling stub (same lock, different parent — not this task)

| VA | Size | Caller | Body |
|----|------|--------|------|
| `0x0044ae46` | 9 B | `Runtime::MSVCRT::__freefls@4` | `__unlock(0xc)` only — R9 globals task 025 |

### mapping.csv / stub (not trusted for semantics)

```
;_LocaleUpdate::FUN_0044cf75;0x44cf75;0xc;__stdcall;;uchar
```

Size `0xc` matches PE; **`uchar` return incorrect** — body ends in plain `RET` with no meaningful AL (corrected in Ghidra to `void __stdcall`).

```cpp
uchar _LocaleUpdate::FUN_0044cf75() { STUB_BODY(); return 0; }
```

### Prior art

| Source | Note |
|--------|------|
| [_LocaleUpdate.md](../struct_recovery/_LocaleUpdate.md) | CRT stack guard; related `__updatetlocinfoEx_nolock` / `___updatetlocinfo` band |
| [r9_globals_task_025_report.md](r9_globals_task_025_report.md) | Generic 9 B `__unlock(0xc)` epilog kept as `FUN_*` |
| [r9_globals_task_019_report.md](r9_globals_task_019_report.md) | Renamed only when `__unlock_10` symbol collision required disambiguation |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044cf75` | `void FUN_0044cf75(void)` + `__stdcall` |
| `set_decompiler_comment` | `0x0044cf75` | `___updatetlocinfo` SEH epilog + lock pair + ESI reload note |
| `force_decompile` | `0x0044cf75` | Refreshed; shows `__unlock(0xc)` (MOV ESI elided by decompiler — disasm proof) |
| `save_program` | `bulanci.exe` | Saved |

No rename (`FUN_0044cf75` unchanged).

## Frida

**none** — CRT-only single-caller xref closure; static Ghidra disasm sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful symbol rename | **Deferred** — MSVC CRT SEH epilog micro-stub; no upstream export or bulanci gameplay symbol |
| Decompiler elision of `MOV ESI,[EBP-0x1c]` | Cosmetic — live disasm @ `0x0044cf7d` is authoritative |
| `mapping.csv` / `_LocaleUpdate.cpp` stub signature | Out of scope — separate mapping pass |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [_LocaleUpdate.md](../struct_recovery/_LocaleUpdate.md)
- [r9_globals_task_025_report.md](r9_globals_task_025_report.md) — `__unlock(0xc)` sibling in `__freefls@4`
