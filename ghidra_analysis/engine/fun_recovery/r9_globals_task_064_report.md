# Round 9 `_Globals` — Task 064 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 64 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | zlib_tail |
| **seed_address** | `0x004713d0` |
| **ghidra_name (before)** | `FUN_004713D0` |
| **prior_hint** | R5 w09 — zlib FUN not audio |
| **prior art** | [round5_worker_09_report.md](../struct_recovery/round5_worker_09_report.md) (zlib `build_tree` callee); [round5_worker_04_report.md](../struct_recovery/round5_worker_04_report.md) (IJG attribution — **refuted**); [gzip_stream.md](../../formats/gzip_stream.md) |

## Status

**PARTIAL** — Role proven via live Ghidra MCP (`connect_instance bulanci`, 2026-06-04): **zlib 1.1.3 `trees.c` `gen_bitlen` compiler-split tail**, sole caller **`build_tree@0x004725a0`**. **WRITE:** `src/bulanci/_Globals.cpp` stub replaced with decompiler-matched body (`EAX` = `deflate_state*` at entry). **No rename:** `gen_bitlen` is a `local` symbol in upstream zlib (no COFF export); fragment is `__fastcall` with implicit register `s` (ROUND9 no-guess rule). Stale IJG decompiler comment corrected.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x004713d0` | `FUN_004713D0` | **zlib `gen_bitlen` tail:** after Huffman heap merge in `build_tree`, clear `s->bl_count[]`, assign `tree[n].Len` from parent depths (cap `max_length`), accumulate `opt_len` / `static_len`, run overflow rebalance on `bl_count`, rescan heap to fix leaf lengths | Live xrefs, disasm, decompile; zlib 1.1.3 `trees.c` `gen_bitlen` + `build_tree` call sequence |

### Caller / callee closure (live Ghidra)

| From | To | Proof |
|------|-----|-------|
| `build_tree@0x00472776` | `FUN_004713d0` | **1×** `UNCONDITIONAL_CALL`; disasm `MOV EAX,ESI` / `CALL 0x004713d0` (`ESI` = `deflate_state*` throughout `build_tree`) |
| `build_tree@0x00472785` | `gen_codes@0x00472520` | Immediate next call — matches zlib `build_tree`: `gen_bitlen(s,desc); gen_codes(tree,max_code,s->bl_count);` |

`build_tree` xrefs (deflate cluster, not audio/gameplay):

| From | Function |
|------|----------|
| `0x00472b2e`, `0x00472b3a` | `_tr_flush_block` |
| `0x004727ce` | `build_bl_tree` |

### `deflate_state` / `tree_desc` field mapping (this binary)

| Offset | zlib field | Use in this function |
|--------|------------|----------------------|
| `s+0xb34` | `bl_count[]` | Zero 8 dwords; histogram `++` per code length; overflow fix `bl_count[bits]--`, `bl_count[bits+1]+=2` |
| `s+0xb54` | `heap[]` | Walk `h = heap_max+1 .. HEAP_SIZE-1` (`0x23d`) |
| `s+0x144c` | `heap_max` | Root index for `tree[heap[heap_max]].Len = 0` |
| `s+0x16a0` | `opt_len` | `+= (bits + xbits) * Freq` per leaf |
| `s+0x16a4` | `static_len` | Same when `stree != 0` |
| `desc->dyn_tree` | `*param_1` | `ct_data` array (`Len` at `+2`, `Freq` at `+0` per 4-byte node) |
| `desc->max_code` | `param_1[1]` | Skip leaves with `n > max_code` |
| `desc->stat_desc` | `param_1[2]` | `extra_bits`, `extra_base`, `max_length` at `[0],[2],[4]` |

### zlib 1.1.3 source correspondence

From [zlib v1.1.3 `trees.c` `gen_bitlen`](https://github.com/madler/zlib/blob/v1.1.3/trees.c) (lines 473–550):

1. `for (bits = 0; bits <= MAX_BITS; bits++) s->bl_count[bits] = 0;` → eight `MOV dword [EAX+0xb34..0xb50],0` @ `004713f3`–`0047141d`.
2. `tree[s->heap[s->heap_max]].Len = 0;` → `MOV word [EBX+EDX*4+2],0` @ `00471435`.
3. Main loop: `bits = tree[tree[n].Dad].Len + 1;` cap `max_length`; `s->bl_count[bits]++;` `s->opt_len += f*(bits+xbits);` → inner loop @ `00471479`–`004714f4`.
4. Overflow rebalance while `overflow > 0` → @ `00471506`–`0047155f`.
5. Rescan decreasing `bits`, fix `tree[m].Len`, adjust `opt_len` → @ `00471561`–`004715dd`.

### Prior-hint correction

| Source | Claim | Verdict |
|--------|-------|---------|
| R5 w09 | zlib, not audio | **Confirmed** — closure is `_tr_flush_block` / `build_bl_tree` / `build_tree` only |
| R5 w04 | IJG Huffman length adjust | **Refuted** — caller is named `build_tree`, not JPEG; offsets match `deflate_state`, not `jpeg_compress_struct` |
| Ghidra pre-comment (pre-task) | IJG `jchuff.c` | **Removed** — replaced with zlib `gen_bitlen` proof comment |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (live) | `undefined __fastcall FUN_004713d0(int * param_1)`; body `004713d0`–`004715e6` (**0x217** B) |
| `config/bulanci/mapping.csv` | `uchar __fastcall`; size **`0x211`** (stale — live body +7 B) |
| Call convention | **`ECX` = `tree_desc*`; `EAX` = `deflate_state*`** set by caller (`build_tree@0x00472774`) — not expressible as portable cdecl |

### Disasm (call site + prolog)

```
00472774  MOV EAX,ESI          ; s = deflate_state*
00472776  CALL FUN_004713d0    ; gen_bitlen tail
0047277f  LEA EDX,[ESI+0xb34]
00472785  CALL gen_codes       ; gen_codes(tree, max_code, bl_count)
```

Prolog clears `bl_count` and enters heap walk @ `004713f3` (`CMP EDI,0x23d` = `HEAP_SIZE`).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x004713d0` | zlib `gen_bitlen` tail; refutes IJG comment |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static xref closure + zlib source alignment sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Rename to `gen_bitlen` | **Deferred** — upstream `local` symbol; compiler-split `__fastcall` + register `EAX` coupling |
| `mapping.csv` size `0x211` / `uchar` return | **Stale** — live `0x217` B, `void` return (RET @ `004715e6`) |
| Byte-exact match vs zlib 1.1.3 object | **Not run** — register allocation differs from published source (see [gzip_stream.md](../../formats/gzip_stream.md) “Why the engine helpers don't byte-match”) |
| `_Globals.h` export name | Still `FUN_004713d0` (separate mapping pass) |

## Cross-links

- [round8_fun_task_25_report.md](round8_fun_task_25_report.md) — sibling zlib tail `FUN_00472420` (`send_bits` cluster)
- [round5_worker_09_report.md](../struct_recovery/round5_worker_09_report.md) — prior zlib band survey
