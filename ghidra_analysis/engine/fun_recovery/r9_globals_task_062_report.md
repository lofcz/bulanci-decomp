# Round 9 `_Globals` — Task 062 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 62 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x0046c850` |
| **ghidra_name (before)** | `FUN_0046C850` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R9 task 061 (`r9_globals_task_061_report.md`) — `jpeg_comp_master` slot `[1]`; R6 logic task 44 marker vtable `+4`/`+8` |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-04) confirms IJG **`jcmaster.c` `pass_startup`**: clears `master->call_pass_startup` (`cinfo+0x13c+0xc`); calls `marker->write_frame_header` / `write_scan_header` via `cinfo+0x14c` vtable slots `+4` / `+8`. Renamed to **`pass_startup`** (upstream IJG `jpeg_comp_master` method); `save_program bulanci.exe` applied.

## Function

| Address | Ghidra (after) | Role | Evidence |
|---------|----------------|------|----------|
| `0x0046c850` | **`pass_startup`** | **IJG compress master pass-start hook:** `master->call_pass_startup = FALSE`; emit frame + scan markers through installed marker writer | See below |

### Vtable install (primary xref proof)

`CDSJpegImage::FUN_0046c8f0` (`jinit_c_master_control` variant) allocates **0x20**-byte master block at `cinfo+0x13c` and stores `jpeg_comp_master` method pointers:

| Slot | VA | IJG `jpeg_comp_master` field |
|------|-----|------------------------------|
| `[0]` | `0x0046c690` | `prepare_for_pass` (task 061) |
| `[1]` | **`0x0046c850`** | **`pass_startup`** |
| `[2]` | `LAB_0046c880` | `finish_pass` *(task 063)* |

**Ghidra xref:** `FUN_0046c8f0@0x0046c916` → **DATA** → `pass_startup`.

**Disasm @ `0x0046c916`:** `MOV dword ptr [EDI+0x4],0x46c850` immediately after `MOV [EDI],0x46c690` @ `0x0046c910`.

### Disasm ↔ IJG `pass_startup` (libjpeg-6b `jcmaster.c`)

| VA | Instruction | IJG source line |
|----|-------------|-----------------|
| `0x0046c855` | `MOV EAX,[ESI+0x13c]` | `master = cinfo->master` |
| `0x0046c85b` | `MOV byte ptr [EAX+0xc],0` | `master->call_pass_startup = FALSE` |
| `0x0046c85f`–`0x0046c869` | `CALL [ECX+4]` with `ECX=[ESI+0x14c]` | `(*cinfo->marker->write_frame_header)(cinfo)` |
| `0x0046c86b`–`0x0046c875` | `CALL [EAX+8]` | `(*cinfo->marker->write_scan_header)(cinfo)` |

Upstream reference (IJG 6b, verified text match):

```c
pass_startup (j_compress_ptr cinfo)
{
  cinfo->master->call_pass_startup = FALSE;
  (*cinfo->marker->write_frame_header) (cinfo);
  (*cinfo->marker->write_scan_header) (cinfo);
}
```

Marker writer slots confirmed in Ghidra: `write_frame_header` @ `0x00460b20` (vtable `+4`), `write_scan_header` @ `0x00460bf0` (vtable `+8`) — [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md).

### Decompile (Ghidra, post-rename)

```c
void pass_startup(int *cinfo)
{
  *(undefined1 *)(cinfo[0x4f] + 0xc) = 0;      /* master @ +0x13c */
  (**(code **)(cinfo[0x53] + 4))(cinfo);       /* marker @ +0x14c */
  (**(code **)(cinfo[0x53] + 8))(cinfo);
  return;
}
```

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (post-rename) | `void __cdecl pass_startup(int * cinfo)`; body `0046c850`–`0046c87b` (**0x2c** B) |
| `config/bulanci/mapping.csv` | `FUN_0046c850`; size **`0x2c`**; `__cdecl`; `uchar` return *(incorrect — void)* |

### mapping.csv stub (not trusted)

```
;_Globals::FUN_0046c850;0x46c850;0x2c;__cdecl;;uchar;int
```

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_0046c850(int param_1) { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046c850` → `pass_startup` | Success |
| `set_function_prototype` | `void __cdecl pass_startup(int * cinfo)` | Success |
| `set_decompiler_comment` | Entry | IJG role + vtable install + field offsets |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static `jpeg_comp_master` vtable install + byte-for-byte IJG `pass_startup` behavior proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| `mapping.csv` / `_Globals.cpp` / `_Globals.h` still list `FUN_0046c850` | Separate mapping pass |
| Indirect runtime caller (`(*master->pass_startup)(cinfo)` when `call_pass_startup`) | **Not traced** — no direct `CALL 0x0046c850` (vtable dispatch only); flag set in `prepare_for_pass` |
| Sibling `LAB_0046c880` (`finish_pass` / `finish_pass_master`) | Task 063 |
| `CDSJpegImage::FUN_0046c8f0` upstream rename (`jinit_c_master_control`) | Out of scope |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [r9_globals_task_061_report.md](r9_globals_task_061_report.md) — `prepare_for_pass` + master vtable layout
- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) — marker writer vtable `+4`/`+8`
- IJG `jcmaster.c` `pass_startup` / `jpegint.h` `struct jpeg_comp_master` (libjpeg-6b)
