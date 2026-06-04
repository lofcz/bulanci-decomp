# Round 9 `_Globals` — Task 038 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 38 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is MSVC CRT / `__get_sse2_info` helper, not game sim)* |
| **seed_address** | `0x0044fe57` |
| **ghidra_name** | `FUN_0044FE57` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN manifest entry (`_gen_r8_fun_manifest.py`, xref_count=1); sibling `FUN_0044FE54@0x0044fe54` (R8 task 24, SEH unwind thunk) |

## Status

**PARTIAL** — Role, xref closure, SEH/SSE2 probe behavior, and return semantics proven via live Ghidra MCP + PE bytes. **No rename:** MSVC CRT nested helper with no COFF/IJG/bulanci export name (Ghidra lib match applies only to caller `__get_sse2_info`, not this seed).

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044fe57` | `FUN_0044FE57` | *(none — keep `FUN_*`)* | **MSVC CRT SSE2 hardware probe** nested under `Runtime::MSVCRT::__get_sse2_info`: SEH-framed `MOVAPD XMM0,XMM1`; returns **1** if instruction runs, **0** if SEH filter sees `0xC0000005` (access violation) or `0xC000001D` (illegal instruction). Not game-reachable except via CRT feature detection. | See below |

### Caller closure (primary proof)

| Item | Value |
|------|-------|
| **Sole code xref** | `CALL 0x0044fe57` @ `0x0044fef4` inside `Runtime::MSVCRT::__get_sse2_info@0x0044fea7` |
| **Caller logic** (Ghidra decompile) | After CPUID feature bit test (`[EBP-0x4] & 0x4000000`): `if (FUN_0044fe57() == 0) return 0; else return 1;` |
| **Caller lib match** | Ghidra: *Library Function - Single Match `__get_sse2_info` — Visual Studio 2005 Release, Visual Studio 2008 Release* |
| **mapping.csv** (caller, not seed) | `Runtime::MSVCRT::__get_sse2_info;0x44fea7;0x60;__stdcall;;uint` |

### Callees / SEH frame

| VA | Ghidra symbol | Role in seed |
|----|---------------|--------------|
| `0x0044a724` | `__SEH_prolog4` | `PUSH 0xc` + `PUSH 0x004ac9c0` then `CALL` — CRT SEH prolog |
| `0x0044a769` | `__SEH_epilog4` | Epilog @ `0x0044fea1`; returns `[EBP-0x1c]` via EAX |
| `0x004ac9c0` | `DAT_004ac9c0` (.rdata scope table) | Sole data xref from `PUSH 0x004ac9c0` @ `0x0044fe59` |

### Size / signature

| Source | Value |
|--------|-------|
| Ghidra body | `0x0044fe57` – `0x0044fea6` → **0x50** (80 B) |
| `config/bulanci/mapping.csv` (seed) | `0x2d` (45 B) — **stale** (stub span `44FE57-44FE84` does not match Ghidra/PE) |
| Ghidra signature | `undefined4 __stdcall FUN_0044fe57(void)` — returns **0** or **1** (not a meaningful `uint` game value) |

### Disassembly (Ghidra listing + PE byte match)

`orig/bulanci_insturmented.exe` @ image base `0x00400000`, first 0x50 bytes @ `0x0044fe57`:

```
0044fe57  PUSH 0xc
0044fe59  PUSH 0x004ac9c0
0044fe5e  CALL 0x0044a724                 ; __SEH_prolog4
0044fe63  AND  dword ptr [EBP-0x4], 0
0044fe67  MOVAPD XMM0, XMM1               ; SSE2 probe
0044fe6b  MOV  dword ptr [EBP-0x1c], 1
0044fe72  JMP  0x0044fe97                 ; success -> epilog

; SEH filter / handler (embedded in same function)
0044fe74  MOV  EAX, dword ptr [EBP-0x14]
0044fe77  MOV  EAX, dword ptr [EAX]
0044fe79  MOV  EAX, dword ptr [EAX]       ; -> ExceptionCode
0044fe7b  CMP  EAX, 0xC0000005            ; STATUS_ACCESS_VIOLATION
0044fe80  JE   0x0044fe89
0044fe82  CMP  EAX, 0xC000001D            ; STATUS_ILLEGAL_INSTRUCTION
0044fe87  JE   0x0044fe89
0044fe89  XOR  EAX, EAX
0044fe8b  RET                             ; probe failed -> 0
0044fe8c  XOR  EAX, EAX
0044fe8e  INC  EAX
0044fe8f  RET                             ; alternate success -> 1

0044fe97  MOV  dword ptr [EBP-0x4], 0xFFFFFFFE
0044fe9e  MOV  EAX, dword ptr [EBP-0x1c]
0044fea1  CALL 0x0044a769                 ; __SEH_epilog4
0044fea6  RET
```

PE hex (80 B): `6a 0c 68 c0 c9 4a 00 e8 c1 a8 ff ff 83 65 fc 00 66 0f 28 c1 c7 45 e4 01 00 00 00 eb 23 ... e8 c3 a8 ff ff c3`

### `_Globals.cpp` stub (not trusted)

```cpp
uint _Globals::FUN_0044fe57() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044fe57` | Updated — SSE2 probe + exception codes + sole caller `__get_sse2_info` |
| `save_program` | `bulanci.exe` | Saved |
| `rename_function_by_address` | — | **Not applied** — no proven export/symbol name |

## Frida

**none** — CRT-only; static xref + disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Official MSVC symbol for this nested helper | **UNK** — not COFF-mapped; nested inside `__get_sse2_info` in VS2005/08 CRT |
| Whether seed should move to `Runtime::MSVCRT` namespace in repo stubs | Out of scope (FUN-only R9 task; address listed under `_Globals` export table) |
| `mapping.csv` size `0x2d` vs Ghidra `0x50` | **Stale mapping** — defer separate mapping pass |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_24_report.md](round8_fun_task_24_report.md) — sibling `FUN_0044FE54` SEH unwind @ `0x0044fe54`
- `Runtime::MSVCRT::__get_sse2_info@0x0044fea7` — immediate caller
