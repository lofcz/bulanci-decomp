# Round 8 FUN — Task 38 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 38 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x004683a0` |
| **title** | FUN recovery: FUN_004683A0 @ 0x004683a0 (xrefs=1) |
| **prior_hint** | — |
| **carry_over** | [round6_logic_task_49_report.md](../logic_recovery/round6_logic_task_49_report.md) |

## Status

**DONE** — IJG `jchuff.c` **LOCAL `emit_restart(working_state *, int restart_num)`** proven on the `flush_bits_s` / `encode_one_block` / `encode_mcu` path. Renamed in Ghidra; `bulanci.exe` saved. PE also contains a second homolog at `0x00468fd0` (COFF export `emit_restart`, `emit_bits_e` + `emit_eobrun` prologue) — Ghidra permits duplicate symbol names at distinct RVAs.

## Function

| Address | Before | After | IJG name | Role | Evidence |
|---------|--------|-------|----------|------|----------|
| `0x004683a0` | `FUN_004683a0` | **`emit_restart`** | `emit_restart` (LOCAL) | Flush bit buffer, emit `0xFF` + `JPEG_RST0 + restart_num`, zero `last_dc_val[]` for `comps_in_scan` | 125 B (`0x7d`); sole CALL from `encode_mcu@0x00468484`; disasm + IJG `jchuff.c` lines 451–465 |

### IJG correspondence (`jchuff.c`)

| Step | IJG source | This binary (`0x004683a0`) |
|------|------------|----------------------------|
| Flush | `flush_bits(state)` | `CALL flush_bits_s@0x004681d0` (`0x004683ab`) |
| Marker prefix | `emit_byte(state, 0xFF)` | `MOV byte [ptr],0xFF` + dest bump (`0x004683ba`–`0x004683d1`) |
| RSTn | `emit_byte(state, JPEG_RST0 + restart_num)` | `SUB BL,0x30` / `MOV [ptr],BL` (`0x004683d5`–`0x004683d8`) — `restart_num - 0x30` ≡ `RST0 + n` for `n∈[0,7]` |
| DC reset | `state->cur.last_dc_val[ci]=0` for `ci < comps_in_scan` | `LEA ECX,[EDI+0x10]` loop `0..[cinfo+0xe4]` (`0x004683f8`–`0x00468415`) |

### Disassembly (`emit_restart`)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x004683a1` | `MOV BL,[ESP+0x8]` | `restart_num` (after `PUSH EBX`) |
| `0x004683a7` | `MOV EDI,EAX` | `working_state *` in **EAX** at entry |
| `0x004683ab` | `CALL 0x004681d0` | `flush_bits_s` |
| `0x004683ba`–`0x004683d1` | emit `0xFF`, optional `emit_byte_s@0x004680f0` | dest-manager refill when `free_in_buffer==0` |
| `0x004683d5`–`0x004683d8` | `SUB BL,0x30` / store | RST marker byte |
| `0x004683f8`–`0x00468415` | zero `[EDI+0x10]` stride 4 | `last_dc_val[]` |

### Caller disassembly (`encode_mcu@0x00468484`)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x0046847c` | `MOV EAX,[ESI+0x28]` | `entropy->next_restart_num` |
| `0x0046847f` | `PUSH EAX` | stack arg: restart index |
| `0x00468480` | `LEA EAX,[ESP+0xc]` | `working_state` on stack (saved dest fields + `last_dc_val` snapshot) |
| `0x00468484` | `CALL emit_restart` | when `cinfo->restart_interval` (`[EDI+0xbc]`) and `restarts_to_go==0` (`[ESI+0x24]`) |

### Xref closure (1)

| From | Type | Context |
|------|------|---------|
| `0x00468484` | UNCONDITIONAL_CALL | `encode_mcu@0x00468420` — pre-MCU restart marker |

### Duplicate homolog (not this task)

| Address | Ghidra | Path | Notes |
|---------|--------|------|-------|
| `0x00468fd0` | `emit_restart` | `emit_eobrun` → `flush_bits_e` → `emit_byte_e` | COFF export in `mapping.csv`; gather/alt entropy layout (`+0x38` EOBRUN) |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x004683a0` → `emit_restart` | Success (duplicate name allowed vs `0x00468fd0`) |
| `set_function_prototype` | `uint __cdecl emit_restart(void *state, char restart_num)` | Success — **note:** live ABI passes `state` in **EAX**, `restart_num` on stack |
| `set_decompiler_comment` | `0x004683a0` | IJG role + caller + duplicate RVA note |
| `force_decompile` | `0x004683a0` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Stock libjpeg-6b Huffman encoder; static IJG + disasm/xref proof suffices.

## Remaining UNK

| Item | Notes |
|------|-------|
| `encode_mcu` call-site typing | After prototype set, decompiler may show wrong `emit_restart` args until custom calling convention (EAX=`state`) is modeled |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_004683a0` export stub — regen out of R8 scope |
| Which `emit_restart` is “the” export | COFF label `0x468fd0`; this RVA is the `encode_mcu` / `working_state` implementation |

## Evidence paths

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- IJG 6b [`jchuff.c` `emit_restart`](https://github.com/LuaDist/libjpeg/blob/master/jchuff.c) (LOCAL, `working_state` path)
- [round6_logic_task_49_report.md](../logic_recovery/round6_logic_task_49_report.md)
- `config/bulanci/mapping.csv` (`0x4683a0` / `0x468fd0`)
