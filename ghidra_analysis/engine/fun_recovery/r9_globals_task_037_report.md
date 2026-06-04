# Round 9 `_Globals` FUN — Task 037 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 37 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044fe54` |
| **ghidra_name** | `FUN_0044FE54` |
| **prior_hint** | *(empty)* |
| **prior art** | R8 FUN task 24 (`round8_fun_task_24_report.md`) |

## Status

**PARTIAL** — Live Ghidra: 3-byte MSVC CRT NLG unwind stub (`CALL EAX; RET`) proven from disasm, decompile, and two-caller xref closure through `__NLG_Notify(0x101)` on `__local_unwind4` / `__local_unwind2`. **`FUN_0044FE54` kept** (no CRT Single Match export name; protocol forbids guessing `_NLG_Dispatch2` or generic thunk names). Ghidra comment + `void __stdcall` prototype refreshed; **`save_program bulanci.exe`**.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044fe54` | `FUN_0044FE54` | *(none — keep `FUN_*`)* | **MSVC CRT NLG unwind dispatch stub:** indirect call through **EAX** after `__NLG_Notify(0x101)` when scope-table entry type dword `[+4] == 0` (function-pointer destructor path) | See below |

### Disassembly (Ghidra live)

```
0044fe54  CALL EAX                 ; ffd0
0044fe56  RET                      ; c3
```

| Metric | Value |
|--------|-------|
| Size | **3 B** (`0x3`) — matches `mapping.csv` |
| Instructions | 2 |
| Inputs | **EAX** = dtor/function pointer at scope entry offset **+8** (reloaded immediately before call at both sites) |
| Outputs | Tail-calls through EAX; no meaningful return in AL |

### Caller closure (2 xrefs — CRT-only)

| Caller | Call site | Guard + setup |
|--------|-----------|---------------|
| `Runtime::MSVCRT::__local_unwind4` @ `0x0044fb8c` | `0x0044fc07` | `CMP dword ptr [EBX+0x4],0` / `JNZ` skip; `PUSH 0x101`; `MOV EAX,[EBX+0x8]`; `CALL __NLG_Notify@0x0044fe35`; `MOV EAX,[EBX+0x8]`; `CALL 0x0044fe54` |
| `Runtime::MSVCRT::__local_unwind2` @ `0x0044fd85` | `0x0044fdf0` | Decompile: `*(int *)(table + 4 + index*0xc) == 0`; same `__NLG_Notify(0x101)` then reload `MOV EAX,[EBX+ESI*4+0x8]` @ `0x0044fdec`; `CALL 0x0044fe54` |

`__NLG_Notify` (Ghidra Single Match @ `0x0044fe35`) records `param_1`, `in_EAX`, and `unaff_EBP` into `DAT_004b1958` / `DAT_004b1954` / `DAT_004b195c` — standard VS2017/2019 CRT NLG bookkeeping before the stub dispatches.

### Decompile (post-mutation)

```c
void _Globals::FUN_0044fe54(void)
{
  code *in_EAX;
  /* MSVC CRT NLG unwind stub (3 B): CALL EAX; RET. EAX = scope-table dtor fn ptr
     [entry+8] after __NLG_Notify(0x101) when entry[+4]==0. Callers
     __local_unwind4@0x0044fc07, __local_unwind2@0x0044fdf0. Keep FUN_* — no CRT
     Single Match export. */
  (*in_EAX)();
  return;
}
```

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044fe54;0x44fe54;0x3;__stdcall;;uchar
```

Size `0x3` matches; `uchar` return incorrect — body is void tail-dispatch with no meaningful AL.

```cpp
uchar _Globals::FUN_0044fe54() { STUB_BODY(); return 0; }
```

### Prior art (R8)

| Source | Note |
|--------|------|
| `round8_fun_task_24_report.md` | Same PARTIAL conclusion; R8 `set_decompiler_comment` “SEH unwind” (refined this session) |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044fe54` | Proof-only NLG stub + caller sites (no guessed export name) |
| `set_function_prototype` | `0x0044fe54` | `void FUN_0044fe54(void)` + `__stdcall` |
| `force_decompile` | `0x0044fe54` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

No rename (`FUN_0044FE54` unchanged).

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Concrete symbol for EAX target at each unwind site | Per-scope function pointer in EH table — not resolved statically |
| MSVC export name for this 3-byte stub | Ghidra has Single Match for `__NLG_Notify` but **not** for `0x0044fe54`; no COFF/asm proof for `_NLG_Dispatch2` rename |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_24_report.md](round8_fun_task_24_report.md)
- [r9_globals_task_036_report.md](r9_globals_task_036_report.md) — adjacent sim-band CRT @ `0x0044fcdf`
