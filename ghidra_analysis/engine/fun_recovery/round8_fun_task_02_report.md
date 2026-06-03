# Round 8 FUN — Task 02 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00467300` |
| **ghidra_name (before)** | `FUN_00467300` |
| **xref_count** | 2 |
| **prior_hint** | R7 task 34 — jdmerge row alloc helper |

## Status

**PARTIAL** — Role, offsets, and caller contract re-verified via live Ghidra MCP (2026-06-03). No unique upstream IJG export/static name in repo or COFF; **`FUN_00467300` kept**. R7 prototype and comment refreshed for R8; decompiler still shows `unaff_ESI` (`set_variable_storage` → ESI blocked by plugin).

## Function

| Address | Ghidra (before) | Ghidra (after) | Role | Evidence |
|---------|-----------------|----------------|------|----------|
| `0x00467300` | `FUN_00467300` | `FUN_00467300` | **IJG libjpeg-6b merged/fancy upsampler helper:** for each `num_components`, calls `cinfo->mem->alloc_small(cinfo, 1, 2*cinfo_field_5c+4)` and stores pointers in `upsample+0x44[]` | Entry uses **ESI=cinfo**; loop `cmp [esi+0x64]`; `lea ebx,[upsample+0x44]`; indirect `call [mem+4]` with `push esi` |

### Behavior (proven)

| Step | Action |
|------|--------|
| 1 | `upsample = *(cinfo + 0x1a8)` |
| 2 | `row_bytes = 2 * *(cinfo + 0x5c) + 4` (`lea ebp,[ebp+ebp+4]`) |
| 3 | For `ci = 0 .. num_components-1` (`cinfo+0x64`): `upsample->color_buf[ci] = alloc_small(cinfo, 1, row_bytes)` via vtable at `*(cinfo+4)+4` |
| 4 | Return (`RET`, 0x40 bytes) |

Callers **must** leave `jpeg_decompress_struct *` in **ESI** (register-arg IJG convention); both call sites satisfy this.

### `cinfo+0x4c == 2` gate (fancy / 2v merged path)

| Caller | Site | Condition |
|--------|------|-----------|
| `jinit_merged_upsampler` | `0x004674ef` | After `zlib__gen_codes`; `cmp [esi+0x4c], 2` |
| `FUN_00467340` (merged `start_pass`) | `0x0046739a` | Branch when `[esi+0x4c]==2`; only if `*(upsample+0x44)==0` |

`FUN_00467340` then zero-fills each allocated row via `IJG_jzero_far` (`0x0045f880`).

### Call graph

```mermaid
flowchart TD
  Master[FUN_00460200 decompress master]
  Init[jinit_merged_upsampler 0x467460]
  Pass[FUN_00467340 merged start_pass]
  Alloc[FUN_00467300 row buffer alloc]
  Fancy[FUN_00467150 fancy upsample method]

  Master --> Init
  Init -->|cinfo+0x4c==2| Alloc
  Init --> Pass
  Pass -->|+0x44[0]==0| Alloc
  Pass --> Fancy
```

### Disassembly (core loop, live MCP)

```
00467300: MOV EAX,[ESI+0x1a8]     ; upsample
00467307: MOV EBP,[ESI+0x5c]      ; width-related field
00467310: LEA EBP,[EBP+EBP+4]     ; alloc size
0046730d: CMP [ESI+0x64],0        ; num_components
00467317: LEA EBX,[EAX+0x44]      ; color_buf[]
00467320: MOV EAX,[ESI+4]         ; mem
00467323: MOV ECX,[EAX+4]         ; alloc_small
00467326: PUSH EBP                ; size
00467327: PUSH 1                  ; JPOOL_IMAGE
00467329: PUSH ESI                ; cinfo
0046732a: CALL ECX
0046732c: MOV [EBX],EAX
```

### Xrefs to (`get_xrefs_to`)

| From | Context |
|------|---------|
| `0x004674ef` | `jinit_merged_upsampler` — post-`zlib__gen_codes`, when `[cinfo+0x4c]==2` |
| `0x0046739a` | `FUN_00467340` — fancy path setup when first `upsample+0x44` slot is null |

### Decompile (post-R8 refresh; ESI artifact)

```c
void FUN_00467300(int *cinfo)
{
  /* body still references unaff_ESI — Ghidra maps param to Stack[0x4], not ESI */
  for (ci = 0; ci < cinfo->num_components; ci++)
    upsample->color_buf[ci] = alloc_small(cinfo, 1, 2 * cinfo->output_width_field + 4);
}
```

(Logically equivalent; field names are descriptive, not Ghidra struct members.)

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00467300` | OK — R8 verification note |
| `set_variable_storage` | `cinfo` → `ESI:4` | **Blocked** — manual decompiler edit required |
| `force_decompile` | `0x00467300` | OK — `unaff_ESI` persists |
| `save_program` | `bulanci.exe` | OK |

**Not applied:** `rename_function_by_address` — no COFF/upstream name (R7/R8 protocol). **Unchanged:** `void __stdcall FUN_00467300(int *cinfo)` from R7.

## Frida

**none** — Static IJG decompressor plumbing; no gameplay-visible state. JPEG paths documented in [formats/status.md](../../formats/status.md).

## Remaining UNK

| Item | Status |
|------|--------|
| Exact IJG `jdmerge.c` / `jdsample.c` symbol for `0x00467300` | **UNK** — control-flow match only; no `ref/libjpeg6b` byte diff in repo |
| Rename to descriptive alias (e.g. `merged_upsample_alloc_rows`) | **Deferred** — not an upstream export; protocol forbids guess |
| Decompiler `cinfo` in ESI vs `unaff_ESI` | **Cosmetic** — `set_variable_storage` limitation |
| `FUN_00467340` upstream name (`start_pass_merged_upsample` family) | **Out of scope** — separate FUN task |
| `mapping.csv` `__stdcall` / `uchar` for `0x467300` | **Stale** — Ghidra prototype is authoritative |
