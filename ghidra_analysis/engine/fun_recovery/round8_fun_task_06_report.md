# Round 8 FUN — Task 06 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 6 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00460f30` |
| **prior_hint** | R7 task 42 — jcprepct edge expand; caller prep vtable |

## Status

**PARTIAL** — R8 re-verify: IJG `jcprepct.c` edge-replication tail; **no rename** (LOCAL split from `create_context_buffer`).

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00460f30` | `FUN_00460f30` | Three per-component loops mirror/replicate fake row pointers at `prep+0x38/+0x3c` (top/bottom/left edge expand) | **Xref (1):** `FUN_004613e0@0x0046142e` when `start_pass` and `need_context_rows`. Prototype `void __cdecl(int cinfo)`. R7 comments intact |

## Ghidra deltas

None this session.

## Frida

**none**

## Remaining UNK

Exact IJG static name; pairs with `create_context_buffer@0x00460e80`.

## Cross-links

- [round7_fun_task_42_report.md](round7_fun_task_42_report.md)
