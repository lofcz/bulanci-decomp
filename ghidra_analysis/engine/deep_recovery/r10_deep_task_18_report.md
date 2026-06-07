# Round 10 Deep — Task 18 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 18 |
| **round** | 10 |
| **kind** | codec |
| **seed_address** | `0x00448a97` |
| **title** | Codec follow-up: FUN_00448A97 high-xref sim |
| **prior art** | R8 FUN task 14 (`round8_fun_task_14_report.md`), R9 globals task 016 (`r9_globals_task_016_report.md`) |
| **acceptance** | 5 xref callers mapped; prove blit/table or IJG role |

## Status

**DONE** — All 5 xrefs mapped with live Ghidra MCP proof. **Not** blit, dispatch-table, or IJG/JPEG codec code: MSVC CRT default empty thread attach/detach hook (`void __cdecl`, single `RET`). Misclassification traced to adjacent `.rdata` layout (`std::bad_alloc` vftable ends at `0x004895c8`; standalone `PTR_FUN` pair follows). Ghidra already renamed to `__crt_default_thread_hook_nop` (R9); no mutations this session.

## Functions / Struct

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00448a97` | `FUN_00448A97` | `__crt_default_thread_hook_nop` | CRT null thread hook: `void __cdecl(void)` body is single `RET`; default target stored at `PTR___crt_default_thread_hook_nop_004895cc` (thread start) and `PTR___crt_default_thread_hook_nop_004895d0` (thread end) | Disasm `00448a97: RET`; decompile `return`; `read_memory` @ `0x004895cc` → `97 8a 44 00` (`0x00448a97`) duplicated at `0x004895d0` |
| `0x00449672` | `_threadstartex` | `_threadstartex` | Caller #1 — indirect hook invoke after FLS/ptd setup | Decompile @ `0x004496e6`: `if (PTR_…895cc != 0) { if (__IsNonwritableInCurrentImage(&PTR_…895cc)) (*(code *)PTR_…895cc)(); }` |
| `0x004495f8` | `__endthreadex` | `__endthreadex` | Caller #2 — indirect hook invoke before `__freeptd` / `ExitThread` | Decompile @ `0x00449610`: same guard pattern for `PTR_…895d0` |
| `0x0044c436` | `__init_pointers` | `__init_pointers` | Caller #3 — direct call during CRT encoded-pointer table init | Decompile @ `0x0044c463`: `__crt_default_thread_hook_nop();` after encoded-null helper batch, before `__initp_eh_hooks` |
| `0x004895cc` | `PTR___crt_default_thread_hook_nop_004895cc` | *(unchanged)* | DATA xref #1 — thread-start function pointer slot | `get_xrefs_to` DATA; PUSH from `_threadstartex` @ `0x004496d7` |
| `0x004895d0` | `PTR___crt_default_thread_hook_nop_004895d0` | *(unchanged)* | DATA xref #2 — thread-end function pointer slot | `get_xrefs_to` DATA; PUSH from `__endthreadex` @ `0x00449601` |

### Xref closure (5, Ghidra MCP `get_xrefs_to 0x00448a97`)

| # | From | Kind | Caller / context |
|---|------|------|------------------|
| 1 | `0x004895cc` | DATA | `PTR___crt_default_thread_hook_nop_004895cc` — dword `0x00448a97` |
| 2 | `0x004895d0` | DATA | `PTR___crt_default_thread_hook_nop_004895d0` — dword `0x00448a97` |
| 3 | `0x004496e6` | COMPUTED_CALL | `_threadstartex` — guarded indirect call through slot #1 |
| 4 | `0x00449610` | COMPUTED_CALL | `__endthreadex` — guarded indirect call through slot #2 |
| 5 | `0x0044c463` | UNCONDITIONAL_CALL | `Runtime::MSVCRT::__init_pointers` — direct call (warm-up / identity on encoded-null batch) |

### Blit / table / IJG role — **disproven**

| Hypothesis | Finding |
|------------|---------|
| IJG / JPEG codec | **No** — no libjpeg symbols, no `jpeg_*` callees, no CDSJpegImage vtable reachability. IJG cluster lives `0x0045d000+` / `0x00466000+` per `formats/jpeg_decoder.md`; this VA is CRT `.text` stub. |
| Blit / raster dispatch | **No** — no framebuffer pointers, no stride/rect args, no CDSBlit or palette paths in xref closure. |
| Dispatch / vtable table | **Layout artifact only** — `master_vtable_catalog.csv` lists `FUN_00448a97` as `std::bad_alloc` slots [3][4] at `0x004895c0`, but live memory shows vtable ends at slot [2]: |

```
0x004895c0  08 7c 44 00  → sub_447C08 / bad_alloc dtor
0x004895c4  84 bf 44 00  → sub_44BF84
0x004895c8  f8 8a 44 00  → __fpmath
0x004895cc  97 8a 44 00  → __crt_default_thread_hook_nop  (PTR_FUN, not vfunc)
0x004895d0  97 8a 44 00  → __crt_default_thread_hook_nop  (PTR_FUN, not vfunc)
0x004895d4  45 6e 63 6f  → ".rdata" string continuation ("EncodePointer…")
```

Actual role: **MSVC VS2005 CRT thread lifecycle hook table** — same pattern as documented `__init_pointers` / `_threadstartex` / `__endthreadex` trio in `bulanci.ghidra.exe.c`.

### Disasm / decompile

```
00448a97  RET
```

```c
void __cdecl _Globals::__crt_default_thread_hook_nop(void) { return; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(none)* | — | R9 rename + comments intact; live state verified via MCP |

## Decomp corrections (IDA vs Ghidra)

| Topic | IDA (`bulanci.ida.exe.c`) | Ghidra | Resolution |
|-------|---------------------------|--------|------------|
| `0x00448a97` body | **Not defined** — gap between `sub_447C2A` (`0x00447c2a`) and `sub_448ED6` (`0x00448ed6`); 1-byte stub omitted | `__crt_default_thread_hook_nop` with plate comment | Ghidra correct; IDA export incomplete at this VA |
| `std::bad_alloc::vftable` @ `0x4895C0` | Declared weak; ctor/dtor refs only (`sub_447BFD`, `sub_447C08`, `sub_447C2A`) | 3-slot vtable + adjacent PTR_FUN pair | IDA does not mislabel `0x448a97` as vfunc; Ghidra `master_vtable_catalog.csv` row is wrong (fixed interpretation in this report) |
| `_threadstartex` / `__endthreadex` / `__init_pointers` | `_endthreadex` declared; CRT internals not exported in IDA C | Full decompile with hook guard + direct call | Ghidra ground truth for caller chain |

## Frida

**none** — static xref closure + `.rdata` dword proof + CRT decompile sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Official MSVC export name | No unique PE export; descriptive `__crt_default_thread_hook_nop` is evidence-based, not COFF symbol |
| `master_vtable_catalog.csv` row `std::bad_alloc@0x4895c0` slots [3][4] | Catalog alignment bug — out of scope for single-address task; recommend catalog trim to 3 slots |
| IDA stub recovery | IDA could define `sub_448A97` as empty function; no Ghidra mutation required |

## Evidence paths

- [ROUND10_DEEP_PROTOCOL.md](../ROUND10_DEEP_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [round8_fun_task_14_report.md](../fun_recovery/round8_fun_task_14_report.md), [r9_globals_task_016_report.md](../fun_recovery/r9_globals_task_016_report.md)
- `bulanci/bulanci.ghidra.exe.c` (`__crt_default_thread_hook_nop`, `_threadstartex`, `__endthreadex`, `__init_pointers`)
- `bulanci/bulanci.ida.exe.c` (`std::bad_alloc::vftable` @ `0x4895C0`; no `0x448a97` symbol)
- `config/bulanci/mapping.csv` (`;_Globals::__crt_default_thread_hook_nop;0x448a97;0x1;__cdecl;;void`)
- `ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl` (PUSH `0x4895cc` / `0x4895d0`)
