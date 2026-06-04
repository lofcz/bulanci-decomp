# Round 9 `_Globals` — Task 045 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 45 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00461080` |
| **ghidra_name** | `FUN_00461080` |
| **prior_hint** | R7 — jcprepct row replicate |
| **prior art** | R7 FUN task 43 (`round7_fun_task_43_report.md`); R8 FUN task 07 (`round8_fun_task_07_report.md`); R6 logic task 44 (`round6_logic_task_44_report.md`) |

## Status

**PARTIAL** — Live Ghidra MCP re-verification (`connect_instance bulanci`, Jun 2026) confirms R7/R8 role and 1× xref closure. **No rename:** compiler-split **LOCAL** from IJG `jcprepct.c` `pre_process_context` cluster; no standalone COFF/export symbol (ROUND9 no-guess rule). Prototype remains `void __cdecl FUN_00461080(int cinfo)`; plate + decompiler comments stamped **R9 verified**; `bulanci.exe` saved.

## Function

| Address | Ghidra name | Proposed rename | Role | Evidence |
|---------|-------------|-----------------|------|----------|
| `0x00461080` | `FUN_00461080` | *(none — keep)* | **Vertical context-row sample replication** in fake `color_buf`: per component, copy `samples_per_row` dwords from row `max_v+1` → row `-1` and from row `0` → row `max_v+2`, syncing paired real/fake pointer tables at `prep+0x38` / `prep+0x3c` | Live decompile; disasm body `0x00461080`–`0x00461156` (215 B / `0xD7`); 1× CODE xref @ `0x0046137d`; caller bytes `E8 FE FC FF FF` |

### IJG field mapping (this binary)

| Offset | IJG role |
|--------|----------|
| `cinfo+0x24` | `num_components` |
| `cinfo+0xc4` | `comp_info` (loop stride `0x54`; width at `+0xc`, blocks at `+0x18`) |
| `cinfo+0x118` | `max_v_samp_factor` |
| `cinfo+0x184` | prep workspace (`cinfo->prep` in decompress wiring) |
| `prep+0x38` / `prep+0x3c` | real vs fake `JSAMPROW` pointer tables |
| `prep+0x4c` | pass-phase flag; must be `1` before this helper runs |

Where `samples_per_row = (width_in_blocks × DCTSIZE) / max_v_samp_factor` from `comp_info` (`IMUL` + `IDIV` @ `0x004610c4`–`0x004610cc`).

### Algorithm (live decompile, per component)

Given `max_v = cinfo+0x118`, row base `puVar9` from `prep+0x3c[ci]`:

- `puVar5 = puVar9 - samples_per_row` — fake row index **−1**
- `puVar8 = puVar9 + (max_v+1)*samples_per_row` — source row above center
- `puVar6 = puVar9 + (max_v+2)*samples_per_row` — fake row index **max_v+2**
- Inner loop copies dword pairs into both `+0x3c` and mirrored offsets through `prep+0x38[ci]` base delta `iVar4`.

Matches IJG `jcprepct.c` context-mode padding inside `pre_process_context` (not `expand_bottom_edge` whole-row copy).

### Disassembly highlights (`0x00461080`–`0x00461156`)

| VA | Proof |
|----|-------|
| `0x00461088` / `0x0046109c` | Load `max_v_samp_factor`, prep ptr from `[cinfo+0x118]`, `[cinfo+0x184]` |
| `0x00461098` | `num_components` at `[cinfo+0x24]` |
| `0x004610c4`–`0x004610cc` | `samples_per_row = IMUL comp_width / max_v` |
| `0x004610d2`–`0x004610db` | Index `prep+0x38[ci]`, `prep+0x3c[ci]` |
| `0x004610e6`–`0x004610f2` | Row pointer math: `-samples`, `+(max_v+1)*samples`, `+(max_v+2)*samples` |
| `0x00461107`–`0x00461127` | Inner loop: 4-byte copy top mirror + bottom mirror |
| `0x0046113b` | Component loop `comp_info += 0x54` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0046137d` | `LAB_00461270` (`pre_process_context` homolog) | `CALL FUN_00461080` when `[prep+0x4c]==1` after `FUN_00461160` @ `0x00461331` and downsampler indirect @ `0x00461365` |
| **Sibling calls (same block)** | `0x00461331` | `FUN_00461160` | Row-pointer shift (R9 task 46) |
| **Vtable install** | `0x00461483` | `FUN_00461460` | Stores prep vtable; does **not** call this function directly |

Caller disasm (live Ghidra @ `0x00461330`):

```
00461331  CALL 0x00461160          ; row shift
00461365  CALL ECX                ; downsampler pre_process hook
00461377  CMP dword ptr [ESI+0x4c], EBX
0046137a  JNZ 0x00461385
0046137d  CALL 0x00461080          ; this function
```

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00461080;0x461080;0xd4;__cdecl;;void;int
```

*(Ghidra body size **`0xD7`** (215 B); mapping lists **`0xd4`** — stale by 3 bytes. Return/signature `void __cdecl(int)` matches live Ghidra.)*

### `_Globals.cpp` stub (not trusted)

```cpp
void _Globals::FUN_00461080(int param_1) { STUB_BODY(); }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x00461080` | Applied — **R9 verified** stamp (was R8) |
| `set_decompiler_comment` | `0x00461080` | Applied — R9 re-verify note |
| `set_function_prototype` | — | **Skipped** — already `void __cdecl FUN_00461080(int cinfo)` |
| `force_decompile` | `0x00461080` | Refreshed — plate shows R9 verified |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — no unique IJG export; LOCAL split from `pre_process_context`, not a documented `expand_*` COFF label.

## Frida

**none** — Static disasm/decompile + IJG `jcprepct.c` cluster match close the role; runtime hooking would not yield a symbol name.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG static name | Replication is **inline** in `pre_process_context` in IJG 6b; MSVC split into `FUN_00461160` + this helper — no separate COFF label |
| `LAB_00461270` / prep vtable method naming | Out of scope for this FUN-only task |
| `mapping.csv` / `_Globals.cpp` export regen | Still `FUN_00461080`; size `0xd4` mismatch — separate mapping pass |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round7_fun_task_43_report.md](round7_fun_task_43_report.md)
- [round8_fun_task_07_report.md](round8_fun_task_07_report.md)
- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md)
- `config/bulanci/mapping.csv` (`0x461080`)
