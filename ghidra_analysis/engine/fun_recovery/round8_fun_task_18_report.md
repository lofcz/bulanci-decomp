# Round 8 FUN — Task 18 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 18 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046c3d0` |
| **title** | FUN recovery: FUN_0046C3D0 @ 0x0046c3d0 (xrefs=3) |
| **prior_hint** | R6 — dispatch UNK |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03) confirms IJG **`jcmaster.c` `select_scan_parameters`**: progressive `scan_info` branch vs default `Ss=0` / `Se=0x3f` / `Ah=Al=0`; JERR `0x1a` if `num_components>4`. Symbol already renamed in program to **`select_scan_parameters`** (upstream IJG export name).

## Function

| Address | Ghidra (before) | Ghidra (after) | Role | Evidence |
|---------|-----------------|----------------|------|----------|
| `0x0046c3d0` | `FUN_0046c3d0` | **`select_scan_parameters`** | **JPEG compress master scan setup:** if `cinfo->scan_info` (`+0xac`), bind `comps_in_scan`, `cur_comp_info[]`, `Ss/Se/Ah/Al` from `scan_info[scan_number]`; else default spectral selection and row pointer bases (`+0xe8` stride `0x54`) | Decompile matches IJG `select_scan_parameters`; 0xe7 B; `__cdecl` with **ESI=cinfo** at entry |

### Xrefs (3)

| From | Site | Type |
|------|------|------|
| `FUN_0046c690` | `0x0046c6d7`, `0x0046c714`, `0x0046c7a8` | UNCONDITIONAL_CALL |

Caller `FUN_0046c690` is compress **prepare_for_pass** cluster (always followed by per-scan setup per decompiler plate).

### Disasm highlights

| VA | Proof |
|----|-------|
| `0x0046c3de` | `CMP [ESI+0xac],0` — `scan_info` null? |
| `0x0046c3f9` | `CMP [EAX],EDI` / `JLE` — component row loop |
| `0x0046c442` | `CMP [ESI+0xf],4` / JERR `0x1a` |
| `0x0046c4a8` | `MOV [ESI+0x4c],0x3f` — default `Se` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(pre-existing)* | `rename_function_by_address` → `select_scan_parameters` | Verified live @ 2026-06-03 |
| `set_decompiler_comment` | Entry | IJG role + caller + field offsets (plate present) |
| `save_program` | — | No R8 mutation required (rename already in DB) |

## Frida

**none** — Static libjpeg compress path.

## Remaining UNK

| Item | Status |
|------|--------|
| `FUN_0046c690` upstream name | **Out of scope** — separate FUN task |
| Decompiler `unaff_ESI` vs `cinfo` param | **Cosmetic** — ESI register-arg at call sites |

## Cross-links

- [round6_logic_task_45_report.md](../logic_recovery/round6_logic_task_45_report.md) — compress band context
- IJG `jcmaster.c` `select_scan_parameters` (27-Mar-1998 libjpeg-6b)
