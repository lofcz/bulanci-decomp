# Round 7 FUN — Task 18 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 18 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x0042b3ae` |
| **title** | FUN recovery: FUN_0042B3AE @ 0x0042b3ae (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**PARTIAL** — SEH epilogue fragment: restores `ExceptionList` from `[EBP-0xc]` only. Not a logical API; keep `FUN_0042b3ae`.

## Function

| Address | Name | Role | Evidence |
|---------|------|------|----------|
| `0x0042b3ae` | `FUN_0042b3ae` | `void __stdcall`: `ExceptionList = *(void **)(EBP-0xc)` | Decompile one store; disasm `MOV ECX, [EBP-0xc]` / `MOV ExceptionList, ECX`; xref `CDSApp_ctor@0x0042b399` tail after registry read |

### Related symbols

| Address | Name | Note |
|---------|------|------|
| `0x0042b39b` | `Catch_0042b39b` | Registry-key exception handler in ctor |
| `0x0042b170` | `CDSApp_ctor` | Full ctor; calls this epilogue before return |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x0042b399` | `CDSApp_ctor` — normal completion path |

## Ghidra deltas

**none** — renaming SEH glue would mislead (not `CDSApp_ctor` body).

## Frida

**none**

## Remaining UNK

| Item | Reason |
|------|--------|
| Merge with `Catch_0042b39b` | Ghidra splits SEH landing vs epilogue; manual label only if EH tables studied |
