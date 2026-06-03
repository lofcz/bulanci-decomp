# Round 8 — FUN task 22 report

## Task

| Field | Value |
|-------|-------|
| **id** | 22 |
| **round** | 8 |
| **band** | sim |
| **seed_address** | `0x00449c90` |
| **title** | FUN recovery: FUN_00449C90 @ 0x00449c90 (xrefs=2) |
| **prior_hint** | (none) |

## Status

**DONE** — Live Ghidra disasm + xref closure prove MSVC CRT small-block heap **region lookup** (`___sbh_find_block`). Renamed; `uint *` / `__thiscall` prototype; program saved. Adjacent `___sbh_free_block@0x00449c97` (Ghidra library Single Match) consumes the returned header.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00449c90` | `FUN_00449c90` → **`___sbh_find_block`** | Walk `0x14`-byte SBH region descriptors from `DAT_004ba1e0` until `ECX` end; return region header in **EAX** when `(block_ptr - *(node+0xc)) < 0x100000`, else **0** | **Disasm:** loop `0x00449c7e`–`0x00449c96` (`SUB EDX,[EAX+0xc]` / `CMP EDX,0x100000` / `ADD EAX,0x14` / `CMP EAX,ECX`). **Xrefs:** tail `JMP` from `thunk_FUN_00449c90@0x00449c7c`; thunk called from `_free`, `_realloc` (×2), `__msize`. **Pair:** `___sbh_free_block` @ `0x00449c97` |

### Disassembly (`0x00449c7e`–`0x00449c96`, entry `0x00449c90`)

```
00449c7e  MOV  EDX, [ESP+4]       ; block_ptr (stack)
00449c82  SUB  EDX, [EAX+0xc]     ; block - region committed size
00449c85  CMP  EDX, 0x100000      ; __sbh_threshold (1 MiB)
00449c8b  JC   0x00449c96         ; found → RET EAX
00449c8d  ADD  EAX, 0x14          ; next region node
00449c90  CMP  EAX, ECX           ; vs region_list_end (this)
00449c92  JC   0x00449c7e         ; loop
00449c94  XOR  EAX, EAX           ; not in SBH → 0
00449c96  RET
```

### Thunk prologue (`thunk_FUN_00449c90@0x00449c6c`, out of scope)

```
00449c6c  MOV  ECX, [DAT_004ba1dc]
00449c72  MOV  EAX, [DAT_004ba1e0]    ; region array base → in_EAX at entry
00449c77  IMUL ECX, ECX, 0x14
00449c7a  ADD  ECX, EAX               ; region_list_end → ECX (this)
00449c7c  JMP  0x00449c90
```

### Xrefs (2 to `0x00449c90`)

| From | Type | Context |
|------|------|---------|
| `thunk_FUN_00449c90` | `0x00449c7c` `JMP` | Sets `EAX`/`ECX`, passes `block_ptr` on stack |
| `thunk_FUN_00449c90` | decompile `CALL` edge | Ghidra also records call graph into body |

Thunk callers (CRT heap, `__lock(4)` guarded): `_free@0x004473bb`, `_realloc@0x004475d8` / `0x00447628`, `__msize@0x004494a0`.

### Upstream match

VS6/VS2005 MSVCRT: `__sbh_find_block(pBlock)` documented in Microsoft KB realloc/SBH articles; paired with `___sbh_free_block` in `_free` / `_realloc` / `__msize` (see `bulanci.ghidra.exe.c` @ `_free` / `_realloc`). Naming follows adjacent Ghidra symbol `___sbh_free_block` (Library Single Match).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00449c90` | `___sbh_find_block` |
| `set_function_prototype` | `0x00449c90` | `uint * ___sbh_find_block(void * block_ptr)` + `__thiscall` (`this` = region list end in ECX) |
| `set_decompiler_comment` | `0x00449c90` | SBH walk role + callers + `___sbh_free_block` pair |
| `force_decompile` | `0x00449c90` | Clean `while` with `in_EAX[3]` / `in_EAX + 5` stride |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — CRT heap internals; static disasm + CRT-only xref closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| `thunk_FUN_00449c90@0x00449c6c` | Separate symbol; R8 scope was seed `0x00449c90` only |
| Function entry vs body | Ghidra entry `0x00449c90` (loop tail); body starts `0x00449c7e` — pre-entry path only via thunk `JMP` |
| `mapping.csv` | Still `_Globals::FUN_00449c90` / return `uchar` until mapping export sync |
| Ghidra FLIRT | No “Library Function - Single Match” on this VA (unlike `___sbh_free_block`); name from algorithm + CRT pairing |

## Cross-links

- `___sbh_free_block@0x00449c97` — immediate successor in SBH cluster
- MSVCRT `_free` / `_realloc` / `__msize` — sole consumers via thunk
