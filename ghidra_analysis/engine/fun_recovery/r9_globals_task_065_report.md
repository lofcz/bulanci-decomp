# Round 9 `_Globals` — Task 065 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 65 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | zlib_tail |
| **seed_address** | `0x00472420` |
| **ghidra_name** | `FUN_00472420` |
| **prior_hint** | R5 w09 — zlib FUN not audio |
| **prior art** | [round8_fun_task_25_report.md](round8_fun_task_25_report.md); [round5_worker_09_report.md](../struct_recovery/round5_worker_09_report.md); [gzip_stream.md](../../formats/gzip_stream.md) |

## Status

**PARTIAL** — Role re-verified via live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). Matches **zlib 1.1.3 `trees.c` `copy_block` (LOCAL)** byte-for-byte control flow, not `_tr_stored_block` entry (that only calls `send_bits` then `copy_block`). **WRITE:** `src/bulanci/_Globals.cpp` stub replaced with decompiler-matched body. **No rename:** `copy_block` is a `local` static in upstream zlib — no COFF/export symbol (ROUND9 no-guess rule). Ghidra comments refreshed; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00472420` | `FUN_00472420` | **zlib 1.1.3 `copy_block` homolog** (compiler split, invoked only from `send_bits` after STORED_BLOCK type bits): `bi_windup`; `last_eob_len = 8` at `deflate_state+0x16ac`; optional LEN/NLEN (`put_short` ×2); byte-copy `stored_len` bytes from `buf` into `pending_buf[pending++]` | Live disasm/decompile; zlib 1.1.3 source `copy_block` @ [trees.c L1147–1169](https://github.com/madler/zlib/blob/v1.1.3/trees.c); **not audio** |

### zlib 1.1.3 source correspondence (`copy_block`)

| Step | Upstream (`trees.c`) | This binary (`0x00472420`) |
|------|----------------------|----------------------------|
| Align bits | `bi_windup(s)` | `CALL bi_windup@0x004723a0` (EAX = `s*` from `send_bits` caller) |
| Lookahead | `s->last_eob_len = 8` | `MOV dword [EAX+0x16ac], 8` |
| Header | `if (header) put_short(len); put_short(~len);` | `emit_len_nlen != 0` → 4-byte LEN/NLEN store via `pending`/`pending_buf` |
| Payload | `while (len--) put_byte(s, *buf++)` | `TEST EBX,EBX` / copy loop @ `0x00472480`–`0x0047249c` |

`_tr_stored_block` upstream is only `send_bits(...,3)` + `copy_block(...,1)` — this VA is the **`copy_block` body**, not the `send_bits` prologue.

### Deflate_state field offsets (this binary)

| Offset | zlib field (1.1.3) | Use in this function |
|--------|-------------------|----------------------|
| `+0x08` | `pending_buf` | Base for pending output bytes |
| `+0x14` | `pending` | Running index into `pending_buf` |
| `+0x16ac` | `last_eob_len` | Set to `8` after `bi_windup` |
| `+0x16b0` / `+0x16b4` | `bi_buf` / `bi_valid` | Used by `bi_windup` callee |

*(R8 plate text said “status=8”; live proof + upstream source: value is **`last_eob_len`, not `status`**.)*

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra | `void __fastcall FUN_00472420(uint stored_len, uchar * buf, int emit_len_nlen)`; body `00472420`–`004724a1` (**0x82** B) |
| `config/bulanci/mapping.csv` | `0x472420`; size **`0x82`**; `__fastcall`; `void;uint;uchar*;int` |
| Callee | `bi_windup@0x004723a0` (1×) |

### Disassembly highlights

| VA | Proof |
|----|-------|
| `0x00472423`–`0x00472425` | `__fastcall`: `stored_len`→`EBX`, `buf`→`ESI` |
| `0x00472427` | `CALL bi_windup` (EAX = `s*` preserved from `send_bits`) |
| `0x00472431` | `last_eob_len = 8` @ `[EAX+0x16ac]` |
| `0x0047242c` | `CMP [ESP+0x10],0` — `emit_len_nlen` stack arg |
| `0x00472442`–`0x00472478` | LEN low/high + `NOT` complements (NLEN) |
| `0x00472480`–`0x0047249c` | Decrement `stored_len`, `put_byte` homolog |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Callers (2× CODE)** | `0x004728e1` | `send_bits` | Branch `bi_valid > 13`; `PUSH 1` then `CALL` |
| | `0x00472908` | `send_bits` | Branch `bi_valid ≤ 13`; `PUSH 1` then `CALL` |
| **Callee** | `0x004723a0` | `bi_windup` | Bit-buffer align to byte |
| **send_bits callers** | `0x0046f5a4` | `deflate` | Engine zlib deflate |
| | `0x00472b87` | `_tr_flush_block` | Block flush / stored path |
| **Gameplay chain** | `0x00434f40` | `CDSGZipStream_Compress_static` | `deflate` xref → [gzip_stream.md](../../formats/gzip_stream.md) |

`send_bits` decompile always passes `emit_len_nlen = 1` (stored block **with** LEN/NLEN header).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00472420` | `copy_block` LOCAL homolog; `last_eob_len@+0x16ac` |
| `set_plate_comment` | `0x00472420` | R9 task 65 summary |
| `force_decompile` | `0x00472420` | Refreshed (pre-write verification) |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Static disasm/decompile + zlib 1.1.3 `trees.c` line match; xref chain ends at `CDSGZipStream::Compress` (documented), not runtime audio decode.

## Remaining UNK

| Item | Reason |
|------|--------|
| Rename to `copy_block` | Upstream symbol is **`local`** in `trees.c` — not a COFF/export; protocol requires export or bulanci doc name |
| Merge with `send_bits` | MSVC split: `send_bits` macro tail + out-of-line `copy_block` body |
| `bi_windup` / `send_bits` stub bodies | Separate zlib_tail tasks; this task only seeds `0x00472420` |
| objdiff byte match | Depends on full zlib TU compile + register allocator matching VS2005 `EAX` convention |

## WRITE deliverable

| File | Change |
|------|--------|
| `src/bulanci/_Globals.cpp` | `FUN_00472420` — `STUB_BODY()` replaced with body matching Ghidra `force_decompile` @ `0x00472420` (`EAX` capture for `send_bits` caller convention) |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_25_report.md](round8_fun_task_25_report.md)
- [gzip_stream.md](../../formats/gzip_stream.md) — zlib 1.1.3 static link, `CDSGZipStream` deflate path
