# Round 5 — Worker 49 Report (top-xref `FUN_*` band `0x460000–0x490000`)

## Task

| Field | Value |
|-------|-------|
| **worker** | 49 / 50 |
| **round** | 5 |
| **mode** | WRITE |
| **title** | Top xref `FUN_*` in `0x00460000–0x00490000` |
| **scope** | libjpeg IJG-6b cluster (`jchuff.c` / `jdhuff.c`); rename top five generic `FUN_*` by xref rank |

## Status

**DONE** — Ghidra script ranked **332** `FUN_*` in band; top five renamed to IJG `jchuff.c` / `jdhuff.c` symbols with structural + call-graph proof. `save_program bulanci.exe`. `jpeg_decoder.md` Huffman table extended.

## Top `FUN_*` by xref count (band scan)

| Rank | Address | Was | Xrefs | Renamed to |
|------|---------|-----|-------|------------|
| 1 | `0x00468e10` | `FUN_00468e10` | 8 | `emit_bits_e` |
| 2 | `0x00468f50` | `FUN_00468f50` | 8 | `emit_eobrun` |
| 3 | `0x00468120` | `FUN_00468120` | 7 | `emit_bits_s` |
| 4 | `0x00468ef0` | `FUN_00468ef0` | 6 | `emit_ac_symbol` |
| 5 | `0x00462f80` | `FUN_00462f80` | 4 | `process_restart` |

(Tied at xref=4: `FUN_004680f0`, `FUN_00468dd0`, `FUN_00462f80` — picked `process_restart` for decoder restart hook used by three already-named `jpeg_decode_mcu_*` entry points.)

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `emit_bits_e` bit buffer | `emit_bits_e@0x00468e10` | `put_buffer` @ `this+0x18`, `put_bits` @ `+0x1c`; byte loop with `0xFF` → stuff `0`; `size==0` → `JERR_HUFF_MISSING_CODE` (`err+8 = 0x28`) — matches IJG `emit_bits_e` |
| `emit_bits_e` callers | xrefs | `encode_one_block@0x004691b0`, `emit_ac_symbol`, `emit_eobrun`, `encode_one_block` tail sites `0x004692ec` / `0x004693de` / `0x004695ac` |
| `emit_bits_s` working-state layout | `emit_bits_s@0x00468120` | `put_buffer` @ `+8`, `put_bits` @ `+0xc`; returns boolean; `FUN_004680f0` refill when `free_in_buffer==0` — matches IJG `emit_bits_s` + `emit_byte_s` |
| `emit_bits_s` callers | xrefs | Seven sites inside `encode_one_block@0x00468200` (DC/AC magnitude path, `jpeg_natural_order@0x0049db54`) |
| `emit_ac_symbol` gather path | `emit_ac_symbol@0x00468ef0` | If `gather_statistics` (`this+0xc`): increment `ac_count_ptrs[tbl][sym]` @ `+0x5c`; else `emit_bits_e` with `ehufco/si` @ `+0x4c` — IJG `emit_ac_symbol` |
| `emit_eobrun` EOBRUN | `emit_eobrun@0x00468f50` | When `EOBRUN@+0x38 != 0`: bit-length loop, `emit_ac_symbol`, `emit_bits_e(EOBRUN)`, clear `+0x38`, `FUN_00468f20` on `bit_buffer@+0x40` — IJG `emit_eobrun` |
| `emit_eobrun` callers | xrefs | `encode_one_block@0x004691b0` (EOB run flush), `FUN_004696a0`, gather/finish paths `0x00469528` / `0x00469573` / `0x00469604` / `0x0046967f` |
| `process_restart` gate | `process_restart@0x00462f80` | Prologue: `cinfo+0xfc` (`restart_interval`) && `entropy+0x28` (`restarts_to_go==0`); calls `marker->read_restart_marker`; reloads `restarts_to_go` from `restart_interval`; zeros MCU coef blocks — IJG `process_restart` |
| `process_restart` callers | xrefs | `jpeg_decode_mcu_DC_first@0x00463010`, `jpeg_decode_mcu_AC_first@0x00463250`, `jpeg_decode_mcu_AC_refine@0x004634a0` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00468e10` | `emit_bits_e` |
| `rename_function_by_address` | `0x00468f50` | `emit_eobrun` |
| `rename_function_by_address` | `0x00468120` | `emit_bits_s` |
| `rename_function_by_address` | `0x00468ef0` | `emit_ac_symbol` |
| `rename_function_by_address` | `0x00462f80` | `process_restart` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [jpeg_decoder.md](../../formats/jpeg_decoder.md) — added Huffman encoder/decoder helper table (five RVAs)

## Remaining UNK (band follow-up)

| Address | Was | Xrefs | Notes |
|---------|-----|-------|-------|
| `0x004680f0` | `FUN_004680f0` | 4 | `emit_byte_s` refill for `emit_bits_s` cluster — rename next pass |
| `0x00468dd0` | `FUN_00468dd0` | 4 | `emit_byte_e` / dest-mgr flush for `emit_bits_e` cluster |
| `0x00468ed0` | `FUN_00468ed0` | 2 | `flush_bits_e` (`emit_bits_e(0x7f)` + clear buffer) — plate comment already |
| `0x00468f20` | `FUN_00468f20` | 3 | `emit_buffered_bits` tail in `emit_eobrun` |
| ~327 | other `FUN_*` | 1–3 | Progressive Huffman (`jcphuff.c`), DCT, marker writer — out of worker-49 top-5 scope |
