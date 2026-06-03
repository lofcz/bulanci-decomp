# Round 8 FUN — Task 36 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 36 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00467150` |
| **title** | FUN recovery: FUN_00467150 @ 0x00467150 (xrefs=1) |
| **prior_hint** | — |
| **carry_over** | [round6_logic_task_48_report.md](../logic_recovery/round6_logic_task_48_report.md) |

## Status

**PARTIAL** — Fancy merged-upsampler pass callback proven via **DATA** xref; **no rename** (distinct from `h2v2_fancy_upsample@0x00464660`; no COFF-exact symbol).

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00467150` | `FUN_00467150` | **`FUN_00467150`** | **Fancy upsample (`cinfo+0x4c==2`):** `short` row workspace at `upsample+0x44[]`, colormap @ `cinfo+0x120`, toggles scan direction `upsample+0x54` | 273 B `__cdecl`; `IJG_jzero_far` per output row |

### Xref closure

| From | Type | Context |
|------|------|---------|
| `0x0046738d` | DATA | `FUN_00467340` when upsample mode field `== 2` |

### Related

May call `FUN_00467300` first when `upsample+0x44[0]==NULL` (row buffer alloc).

## Ghidra deltas

| Action | Result |
|--------|--------|
| **none** | |

## Frida

**none**

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG fancy-upsampler name | Not `h2v2_fancy_upsample` @ `0x464660` |
