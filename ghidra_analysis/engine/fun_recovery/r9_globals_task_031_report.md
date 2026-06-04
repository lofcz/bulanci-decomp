# Round 9 `_Globals` — Task 031 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 31 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is MSVC CRT `__VEC_memcpy` tail, not game sim logic)* |
| **seed_address** | `0x0044c482` |
| **ghidra_name** | `FUN_0044C482` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R5 worker 16 skip table (`__VEC_memcpy` helper); `Runtime::MSVCRT::__VEC_memcpy@0x0044c509` in `mapping.csv` |

## Status

**PARTIAL** — Role proven: aligned 128-byte SSE2 (`MOVDQA`) copy loop; sole callee from Ghidra-matched `Runtime::MSVCRT::__VEC_memcpy`. **Sim body written** in `_Globals.cpp`. **No rename** (MSVC internal tail symbol not verified; protocol forbids invented `helper_*` names). Ghidra prototype corrected to `void __cdecl`; comment + `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044c482` | `FUN_0044c482` | *(keep `FUN_*`)* | **`__VEC_memcpy` aligned fast-path chunk copier:** for `param_3 >> 7` iterations, copy 128 bytes (eight 16-byte `MOVDQA` pairs) from `param_2` to `param_1`, advancing both by `0x80` per iter; parent copies `param_3 & 0x7f` tail bytes | See below |

### Disassembly (Ghidra)

```
0044c494  MOV  ECX, dword ptr [EBP+0x10]   ; byte count
0044c497  SHR  ECX, 0x7                    ; iterations = count / 128
0044c4a2  MOVDQA XMM0, xmmword ptr [ESI]   ; 8× MOVDQA load/store → 128 B
...
0044c4f0  LEA  ESI, [ESI+0x80]
0044c4f6  LEA  EDI, [EDI+0x80]
0044c4fc  DEC  ECX
0044c4fd  JNZ  0x0044c4a2
0044c508  RET                              ; void — no AL
```

Size **`0x81`** (129 B), `__cdecl`, ends `0x0044c508`; next symbol `__VEC_memcpy@0x0044c509`.

### Caller closure (single xref)

| From | In function | Context |
|------|-------------|---------|
| `0x0044c556` `CALL 0x0044c482` | `Runtime::MSVCRT::__VEC_memcpy` @ `0x0044c509` | When both pointers are 16-byte aligned (`iVar1 == 0 && uVar6 == uVar3`), `uVar3 = param_3 & 0x7f`; if `param_3 != uVar3`, call this helper with **`param_3 - uVar3`** (multiple of 128); then byte-copy tail `uVar3` bytes |

Ghidra library match on parent: **Visual Studio 2005/2008 Debug+Release `__VEC_memcpy`**.

Parent also reached from `_memcpy@0x00447ed2` and `_memmove@0x00448f32` (not direct xrefs to this VA).

### Callees

None — leaf loop.

### mapping.csv / stub (not trusted for semantics)

```
;_Globals::FUN_0044c482;0x44c482;0x81;__cdecl;;uchar;uint*;uint*;uint
```

Size/convention match PE; **`uchar` return incorrect** — body is void `RET` (corrected in Ghidra).

### Sim write (`_Globals.cpp`)

```cpp
uchar _Globals::FUN_0044c482(uint* param_1, uint* param_2, uint param_3) {
	for (uint blocks = param_3 >> 7; blocks != 0; --blocks) {
		for (uint i = 0; i < 32; ++i)
			param_1[i] = param_2[i];
		param_1 += 32;
		param_2 += 32;
	}
	return 0;
}
```

Semantic match to decompiler (32 dwords × 128 B); host sim uses scalar stores instead of `MOVDQA`.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044c482` | `void FUN_0044c482(undefined4 *param_1, undefined4 *param_2, uint param_3)`, `__cdecl` |
| `set_decompiler_comment` | `0x0044c482` | R9w31 `__VEC_memcpy` tail proof |
| `force_decompile` | `0x0044c482` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |
| `rename_function_by_address` | — | **Not applied** (PARTIAL) |

## Frida

**none** — CRT-only xref closure; Ghidra disasm + parent decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Official MSVC export name for this 128 B MOVDQA tail | **UNK** — not in Ghidra library match list; keep `FUN_*` |
| `mapping.csv` / `_Globals.h` `uchar` return | Out of scope — separate mapping pass |
| Move to `Runtime::MSVCRT` namespace | Cosmetic; behavior proven under `_Globals` |

## Cross-links

- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — slice skip note for `0x0044c482`
- `config/bulanci/mapping.csv` — `__VEC_memcpy@0x44c509` sibling row
