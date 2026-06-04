# Round 9 `_Globals` — Task 048 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 48 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00466920` |
| **ghidra_name** | `FUN_00466920` |
| **prior_hint** | R7/R8 — merged-upsampler dither table size |
| **prior art** | [round7_fun_task_50_report.md](round7_fun_task_50_report.md), [round8_fun_task_11_report.md](round8_fun_task_11_report.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verify (`connect_instance("bulanci")`, 2026-06-04) confirms R7/R8 closure. **No rename:** IJG merged-upsampler **LOCAL** ordered-dither sample-table size helper; no standalone COFF/export symbol (ROUND9 no-guess rule). Name stays `FUN_00466920`.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00466920` | `FUN_00466920` | **Merged-upsampler ordered-dither sample-table size** — finds smallest factorial base over `param_2[]` whose product exceeds `cinfo+0x54`; initializes `param_2[]` via `rep stosd`; walks permutation digits (optional index via `jpeg_natural_order` tail @ `0x0049df50` when `cinfo+0x2c==2`); returns byte count for `alloc_large` in `FUN_00466a50`; **JERR `0x38`** + `error_exit` when derived base `< 2` | Live decompile + disasm + **1×** CODE xref; **not** zlib `gen_codes` (R6 correction) |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (live) | `int __cdecl FUN_00466920(int * param_1, int * param_2)` |
| Body | `00466920`–`00466a06` → **231 B** (`0xE7`) |
| `config/bulanci/mapping.csv` | `int __cdecl`; size **`0xe2`** (stub row stale vs Ghidra body end) |
| `_Globals.cpp` | Stub only — not trusted |

### IJG `jpeg_compress_struct` field mapping (this binary)

| Offset | `param_1[]` index | Role in this helper |
|--------|-------------------|---------------------|
| `+0x00` | `*param_1` | Error manager pointer; JERR path calls `error_exit` via vtable |
| `+0x2c` | `[0xb]` | When `== 2`, permutation digit index uses `(&jpeg_natural_order)[i]` @ `0x0049df50` |
| `+0x54` | `[0x15]` | Cap / target for factorial product search |
| `+0x64` | `[0x19]` | Component count — outer loop bound |

### Algorithm (live decompile summary)

1. Increment base `iVar8` from 1 until `iVar8^num_components > cap` (nested multiply over `param_1[0x19]`).
2. If `iVar8 < 2`, set `err->msg_code = 0x38`, `err->msg_parm.i = product`, call `error_exit`.
3. `rep stosd` fill `param_2[]` with base `iVar8`; compute initial product.
4. Loop: increment permutation digits (via natural-order index when `[0xb]==2`); return first product `≥ cap`, or previous product on overflow branch.

Return value consumed by caller as `alloc_large` size argument.

### Disasm anchors (live Ghidra)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x00466950` | `IMUL EAX,EBP` | Factorial product loop |
| `0x00466966` | `MOV dword ptr [ECX+0x8],0x38` | JERR code **0x38** |
| `0x0046698f` | `STOSD.REP ES:EDI` | Initialize `param_2[]` array |
| `0x004669b4` | `CMP dword ptr [EDX+0x2c],0x2` | Natural-order remap gate |
| `0x004669ba` | `MOV EDI,dword ptr [EBP*4+0x49df50]` | `jpeg_natural_order` tail index |
| `0x004669cf` | `IDIV ESI` | Permutation increment / product update |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x00466a6a` | `FUN_00466a50` | `iVar3 = FUN_00466920(param_1, upsample+0x20)` → `alloc_large(cinfo, 1, iVar3, num_components)` |
| **Init chain** | `0x004674db` | `jinit_merged_upsampler` | Calls `FUN_00466a50` on merged-upsampler setup path |

Caller excerpt (live decompile):

```c
piVar11 = (int *)(param_1[0x6a] + 0x20);
iVar3 = FUN_00466920(param_1, piVar11);
piVar4 = (int *)(**(code **)(param_1[1] + 8))(param_1, 1, iVar3, param_1[0x19]);
```

### Callees

| Callee | Role |
|--------|------|
| — | Leaf on success path; JERR path indirect call through `*param_1` (`error_exit`) |

## Ghidra deltas

| Action | Result |
|--------|--------|
| **none** | R7 annotations verified intact: namespace `_Globals`, plate + decompiler pre-comments, prototype `int __cdecl FUN_00466920(int *, int *)`; no `save_program` |

## Frida

**none** — JPEG decompress merged-upsampler init; static xref + decompile + `jinit_merged_upsampler` chain sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG static symbol name | LOCAL helper from merged-upsampler init in libjpeg-6b; no COFF/export string in `bulanci.exe` |
| `FUN_00466a50` / `FUN_00466a10` naming | Separate R9 tasks on same init chain |
| JERR `0x38` message string | Error code proven in disasm/decompile; message table label not resolved |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_00466920` — export regen out of scope |

## Cross-links

- [round8_fun_task_13_report.md](round8_fun_task_13_report.md) — caller `FUN_00466a50` table fill after size calc
- [round6_logic_task_47_report.md](../logic_recovery/round6_logic_task_47_report.md) — R6 zlib mis-attribution corrected
