# Round 9 `_Globals` FUN — Task 016 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 16 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x00448a97` |
| **ghidra_name** | `FUN_00448A97` |
| **prior_hint** | R6 — high-xref sim/other |
| **prior art** | R8 FUN task 14 (`round8_fun_task_14_report.md`) |

## Status

**PARTIAL** — MSVC CRT default empty thread attach/detach hook (`void __cdecl`, single `RET`); role re-verified live in Ghidra. **No rename** (no unique COFF/export or bulanci game symbol). R8 plate/decompiler comments already present; **no Ghidra mutations** this session; **no `save_program`**.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00448a97` | `FUN_00448A97` | `FUN_00448A97` | `void __cdecl(void)`: single **`RET`** no-op; default target for thread hooks at `PTR_FUN_004895cc` (start) and `PTR_FUN_004895d0` (end) | Disasm `00448a97: RET`; body `return`; 5 xrefs; `read_memory` @ `0x004895cc` → `0x00448a97`; callers gate indirect call with `__IsNonwritableInCurrentImage` |

### Xrefs (5, Ghidra MCP)

| From | Kind | Context |
|------|------|---------|
| `0x004895cc` | DATA | `PTR_FUN_004895cc` — dword `0x00448a97` (`read_memory`: `97 8a 44 00`) |
| `0x004895d0` | DATA | `PTR_FUN_004895d0` — dword `0x00448a97` at slot start |
| `0x004496e6` | COMPUTED_CALL | `_threadstartex` — `if (PTR_FUN_004895cc != 0)` + `__IsNonwritableInCurrentImage` then `(*(code *)PTR_FUN_004895cc)()` |
| `0x00449610` | COMPUTED_CALL | `__endthreadex` — same pattern for `PTR_FUN_004895d0` |
| `0x0044c463` | UNCONDITIONAL_CALL | `Runtime::MSVCRT::__init_pointers` — direct call after `__encoded_null` helper batch |

### Disasm / decompile (Ghidra)

```
00448a97  RET
```

```c
void __cdecl _Globals::FUN_00448a97(void) { return; }
```

Plate: `CRT null thread attach/detach callback (PTR_FUN_004895cc/d0)` — matches R8.

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra | `void __cdecl FUN_00448a97(void)`; body `00448a97`–`00448a97` (1 B) |
| `config/bulanci/mapping.csv` | `;_Globals::FUN_00448a97;0x448a97;0x1;__cdecl;;void` |
| `src/bulanci/_Globals.cpp` | `STUB_BODY()` stub only — not trusted |

### Corrections vs prior hints

| Source | Claim | R9 finding |
|--------|-------|------------|
| R6 “high-xref sim/other” | Vague sim-band cluster | **5 xrefs**, all CRT thread/init wiring — not gameplay sim |
| R5 worker 16 boundary | `LeaveCriticalSection` probe | **Incorrect for this VA** — only `RET`; unlock is `__unlock@0x00449b1b` |
| `master_vtable_catalog.csv` | `std::bad_alloc` vftable [3][4] | **Layout artifact** — `PTR_FUN_004895cc`/`d0` are standalone globals in thread CRT paths |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(none)* | — | R8 comments intact; live state matches R8 proof; no rename/prototype change |

## Frida

**none** — static CRT pointer table + xref closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Official MSVC symbol | No PE export / library single-match name for this 1-byte stub |
| `PTR_FUN_004895cc` / `PTR_FUN_004895d0` rename | Data symbols out of scope for single-FUN task |
| `_Globals.cpp` / `mapping.csv` sync | Export-tree update deferred |

## Cross-links

- [round8_fun_task_14_report.md](round8_fun_task_14_report.md)
- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- `ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl` (PUSH `0x4895cc` / `0x4895d0`)
