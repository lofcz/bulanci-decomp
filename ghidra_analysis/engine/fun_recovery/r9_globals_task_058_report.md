# Round 9 `_Globals` — Task 058 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 58 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00467340` |
| **ghidra_name (before)** | `FUN_00467340` |
| **prior_hint** | R7 — caller of row alloc / upsampler vtable |
| **prior art** | R8 FUN task 12 (`round8_fun_task_12_report.md`); R9 task 053 (`FUN_00466d40` caller); R9 task 056 (`FUN_00467150` install) |

## Status

**PARTIAL** — Merged-upsampler **`start_pass`** role, `cinfo`/`upsample` field gates, method-pointer installs, callee closure, and vtable xref re-verified via live Ghidra MCP (disasm, decompile, xrefs). Matches IJG **libjpeg-6b `jdmerge.c` `start_pass` control flow**; no COFF/upstream export name in repo — **`FUN_00467340` kept**. **`src/bulanci/_Globals.cpp` body written** from post-R8 decompile (R9 WRITE).

## Function

| Address | Ghidra (this session) | Role | Evidence |
|---------|----------------------|------|----------|
| `0x00467340` | `FUN_00467340` | **IJG libjpeg-6b merged-upsampler `start_pass`:** copy `upsample+0x10/+0x14` → `cinfo+0x74/+0x70`; install scanline method at `upsample+4` from `cinfo+0x4c` and `cinfo+0x64`; merged path may call `zlib::gen_codes`, `FUN_00466d40`, `FUN_00467300`, `IJG_jzero_far` | Sole **DATA** xref `jinit_merged_upsampler@0x00467479` → `mov [upsample],0x467340`; **0xe3** B; `void __cdecl(int *cinfo)` |

### Vtable install (proven)

| Site | Instruction | Meaning |
|------|-------------|---------|
| `0x00467479` | `MOV dword ptr [EAX],0x467340` | `upsample->start_pass = FUN_00467340` after `alloc_large(cinfo,1,0x58)` |

Caller chain: `master_selection` / decompress init → `jinit_merged_upsampler@0x00467460` → indirect call via vtable slot 0 at pass start.

### `cinfo+0x4c` (`cinfo[0x13]`) dispatch

| `cinfo+0x4c` | `cinfo+0x64` | `*(upsample+4)` | Side effects |
|-------------|--------------|-----------------|--------------|
| `0` (separate upsample) | `!= 3` | `LAB_00466d90` | — |
| `0` | `== 3` | `color_quantize` | — |
| `1` (merged) | `== 3` | `FUN_00467020` | `upsample+0x30 = 0`; if `upsample+0x1c==0` → `zlib::gen_codes@0x00466b70`; if `upsample+0x34==0` → `FUN_00466d40` (`EBX=cinfo`) |
| `1` | `!= 3` | `FUN_00466f00` | same |
| `2` (fancy merged) | — | `FUN_00467150` | `upsample+0x54 = 0`; if `upsample+0x44[0]==0` → `FUN_00467300`; loop `IJG_jzero_far` each `color_buf[i]` size `2*cinfo+0x5c+4` |
| other | — | — | `err->msg_code = 0x30`; `err->emit_message(cinfo)` |

Always at entry: `cinfo+0x74 = *(upsample+0x10)`, `cinfo+0x70 = *(upsample+0x14)`.

### Disassembly (dispatch core, live Ghidra)

```
00467346  MOV EDI,[ESI+0x1a8]       ; upsample
0046734f  MOV [ESI+0x74],EAX        ; cinfo+0x74 <- upsample+0x10
00467352  MOV EAX,[ESI+0x4c]        ; upsample mode
0046735b  MOV [ESI+0x70],ECX        ; cinfo+0x70 <- upsample+0x14
0046735e  JZ  0x00467409            ; mode == 0
00467368  JZ  0x004673cc            ; mode == 1
0046736d  JZ  0x00467386            ; mode == 2
00467386  MOV [EDI+4],0x467150      ; FUN_00467150
0046739a  CALL 0x00467300           ; row alloc if color_buf[0]==0
004673b4  CALL 0x0045f880           ; IJG_jzero_far per component row
004673cc  ... 0x467020 / 0x466f00 ...
004673f0  CALL 0x00466b70           ; zlib::gen_codes
00467400  CALL 0x00466d40           ; scaled quant tables (EBX=ESI=cinfo)
```

### Callees (from disasm)

| Callee | Site | When |
|--------|------|------|
| `zlib::gen_codes` | `0x004673f0` | `cinfo+0x4c==1` and `upsample+0x1c==0` |
| `FUN_00466d40` | `0x00467400` | `cinfo+0x4c==1` and `upsample+0x34==0` (`MOV EBX,ESI` @ `0x004673fe`) |
| `FUN_00467300` | `0x0046739a` | `cinfo+0x4c==2` and `upsample+0x44[0]==0` |
| `IJG_jzero_far` | `0x004673b4` | fancy path; per `num_components` |

### Xrefs to (`get_xrefs_to`)

| From | Type | Context |
|------|------|---------|
| `0x00467479` | DATA | `jinit_merged_upsampler` — `start_pass` vtable slot 0 |

No direct `CALL` xrefs (vtable-only entry).

### `_Globals.cpp` WRITE (this session)

Recovered body at `src/bulanci/_Globals.cpp` (`FUN_00467340`) mirrors live decompile. Callee `zlib::gen_codes@0x00466b70` invoked via `reinterpret_cast` until that VA has a named stub in this TU (mapping row `;_Globals::zlib::gen_codes;0x466b70`).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `get_function_by_address` | `0x00467340` | `void __cdecl FUN_00467340(int *cinfo)` — unchanged from R8 |
| `set_decompiler_comment` | `0x00467340` | R9 task 058 summary |
| `force_decompile` | `0x00467340` | OK |
| `save_program` | `bulanci.exe` | OK |

**Not applied:** `rename_function_by_address` — no unique COFF / `ref/libjpeg6b` export for this 0xe3 B vtable method.

## Frida

**none** — Static IJG decompressor vtable plumbing; disasm + single DATA xref sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact IJG `jdmerge.c` symbol (e.g. `start_pass_merged_upsample`) | **UNK** — control-flow + vtable slot match only |
| Rename to descriptive alias | **Deferred** — protocol forbids guess without upstream export |
| `FUN_00467300` / `FUN_00466d40` register args (`ESI`/`EBX`=cinfo) in C++ port | **Cosmetic** — decompile artifacts; binary passes registers |
| `zlib::gen_codes@0x00466b70` symbolic call in `_Globals.cpp` | **Pending** — separate manifest VA; cpp uses VA cast |

## Cross-links

- [round8_fun_task_12_report.md](round8_fun_task_12_report.md)
- [r9_globals_task_053_report.md](r9_globals_task_053_report.md) — callee `FUN_00466d40`
- [r9_globals_task_056_report.md](r9_globals_task_056_report.md) — callee `FUN_00467150`
- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
