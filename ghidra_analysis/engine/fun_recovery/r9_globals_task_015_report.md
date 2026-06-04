# Round 9 `_Globals` — Task 015 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 15 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is MSVC CRT `eh_vector_*` / SEH band, not game simulation)* |
| **seed_address** | `0x00448765` |
| **ghidra_name** | `FUN_00448765` |
| **prior_hint** | *(empty)* |
| **prior art** | R9 task 011 (`FUN_0044751C` — `__calloc_impl` SEH epilog, comment-only PARTIAL); R9 task 039 / R8 task 26 (`FUN_004500E2` — `_raise` SEH epilog); sibling task 014 `FUN_004486a4@0x004486a4` (`eh_vector_constructor_iterator` tail) |

## Status

**PARTIAL** — Role proven as MSVC **`eh_vector_destructor_iterator` SEH epilog** calling **`__ArrayUnwind`** when `[EBP-0x1c]==0`; **`FUN_00448765` kept** (no protocol-safe unique rename). Ghidra MCP live; prototype + decompiler comment refreshed; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00448765` | `FUN_00448765` | **`eh_vector_destructor_iterator` SEH epilog thunk:** on exceptional unwind (`[EBP-0x1c]==0`), call `__ArrayUnwind` with destructor-iterator frame args; pure MSVCRT glue split at parent function end. | See below |

### Disassembly (Ghidra live)

```
00448765  CMP  dword ptr [EBP-0x1c], 0
00448769  JNZ  0x0044877c
0044876b  PUSH dword ptr [EBP+0x14]    ; dtor functor
0044876e  PUSH dword ptr [EBP+0x10]    ; count
00448771  PUSH dword ptr [EBP+0xc]     ; element size
00448774  PUSH dword ptr [EBP+0x8]     ; base pointer
00448777  CALL 0x004486bc              ; __ArrayUnwind
0044877c  RET
```

**Size:** 24 B (`0x18`), matches `mapping.csv`. **Bytes:** `83 7d e4 00 75 0b ff 75 14 ff 75 10 ff 75 0c ff 75 08 e8 44 ff ff ff c3`.

### Parent layout (byte-accurate split)

| Symbol | Entry | Size | End (= next entry) |
|--------|-------|------|---------------------|
| `eh_vector_destructor_iterator` | `0x0044871a` | `0x4b` | **`0x00448765`** |
| **`FUN_00448765`** (this task) | **`0x00448765`** | **`0x18`** | `0x0044877d` |
| `CBulanci::_wcsrchr` | `0x0044877d` | `0x30` | … |

Ghidra library match on parent: **`eh vector destructor iterator`**, **Visual Studio 2005 Release** (`_eh_vector_destructor_iterator_`).

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x00448758` | `eh_vector_destructor_iterator@0x0044871a` | After normal loop sets `[EBP-0x1c]=1` @ `0x0044874a` and `[EBP-4]=0xfffffffe` @ `0x00448751`; `CALL` immediately before SEH epilog helper @ `0x0044a769` |
| **Callee** | `0x004486bc` | `__ArrayUnwind` | `mapping.csv` row `Runtime::MSVCRT::__ArrayUnwind`; size `0x32` |
| **SEH scope** | `0x004ac630` | `.rdata` | Pushed @ `0x0044871c` with `PUSH 0xc` — standard `__SEH_prolog4` scope for parent |

### Parent SEH flag semantics

| Instruction | Effect |
|-------------|--------|
| `0x00448726` | `AND dword ptr [EBP-0x1c], 0` — init “needs unwind” |
| `0x0044874a` | `MOV dword ptr [EBP-0x1c], 1` — normal completion, skip `__ArrayUnwind` |
| `0x00448765` | `CMP [EBP-0x1c], 0` — only unwinds partially constructed vector on exceptional path |

### Downstream consumers of `eh_vector_destructor_iterator` (sample)

Not direct xrefs to `FUN_00448765` — reachability is only via parent CRT helper. Examples: `CDSView_UnwindDtor_StringVec9@0x004048d2`, `CGame_dtor@0x00414d5a`, `CGaming_dtor@0x0041ba09`, many `Unwind@0047*` stubs (141+ EH sites per R5 worker 01).

### Decompile (Ghidra live)

```c
void _Globals::FUN_00448765(void)
{
  if (*(int *)(unaff_EBP + -0x1c) == 0) {
    Runtime::MSVCRT::__ArrayUnwind(
        *(void **)(unaff_EBP + 8), *(uint *)(unaff_EBP + 0xc),
        *(int *)(unaff_EBP + 0x10),
        *(_func_void_void_ptr **)(unaff_EBP + 0x14));
  }
  return;
}
```

*(Prior decompiler note citing `CBulanci::_qsort` was incorrect — xref/disasm tie this fragment to `eh_vector_destructor_iterator` only.)*

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_00448765;0x448765;0x18;__stdcall;;uchar
```

Body is void SEH thunk (`RET` with no meaningful AL); `uchar` return is incorrect.

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| `__ArrayUnwind` | **Rejected** — duplicate of real impl @ `0x004486bc` |
| `eh_vector_destructor_iterator` | **Rejected** — parent @ `0x0044871a` already named |
| Generic ordinal / address suffix | **Forbidden** by protocol |
| MSVC upstream export for this tail | **None** — compiler-generated SEH epilog split, not a separate documented CRT export |

Same rationale as R9 task 011 / R8 task 26.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x00448765` | `void FUN_00448765(void)` `__stdcall` |
| `set_decompiler_comment` | `0x00448765` | `eh_vector_destructor_iterator` SEH epilog + xref/callee proof |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — SEH epilog micro-stub; no unique IJG/libmad/bulanci game symbol |
| Merge tail into parent function boundary | Cosmetic Ghidra layout; out of scope |
| Sibling `FUN_004486a4` | Task 014 — parallel `eh_vector_constructor_iterator` tail |

## Cross-links

- [r9_globals_task_011_report.md](r9_globals_task_011_report.md) — SEH epilog comment-only PARTIAL precedent
- [r9_globals_task_039_report.md](r9_globals_task_039_report.md) — `_raise` / `__unlock` SEH epilog pattern
- [round5_worker_01_report.md](../struct_recovery/round5_worker_01_report.md) — EH vector consumer volume
- `ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl` — scope table `0x004ac630` for parent
