# Round 8 FUN — Task 14 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 14 |
| **round** | 8 |
| **band** | sim |
| **seed_address** | `0x00448a97` |
| **title** | FUN recovery: FUN_00448A97 @ 0x00448a97 (xrefs=5) |
| **prior_hint** | R6 — high-xref sim/other |

## Status

**PARTIAL** — Role proven (MSVC CRT default empty thread attach/detach hook); **no rename** (no unique COFF/export symbol; not a game or IJG name). Ghidra comments + `save_program` applied.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00448a97` | `FUN_00448A97` | `FUN_00448A97` | `void __cdecl(void)`: single **`RET`** no-op; default target for encoded thread hooks at `PTR_FUN_004895cc` (start) and `PTR_FUN_004895d0` (end) | Disasm `00448a97: RET`; decompile body is `return`; 5 xrefs (see below); callers gate with `__IsNonwritableInCurrentImage` before indirect call |

### Xrefs (5)

| From | Kind | Context |
|------|------|---------|
| `0x004895cc` | DATA | `PTR_FUN_004895cc` — `_threadstartex@0x004496e6` pushes this slot, calls through encoded pointer when image section writable |
| `0x004895d0` | DATA | `PTR_FUN_004895d0` — `__endthreadex@0x00449610` same pattern for thread teardown hook |
| `0x004496e6` | CALL | `_threadstartex` — after FLS/ptd setup, optional user thread-start callback |
| `0x00449610` | CALL | `__endthreadex` — before `__freeptd` / `ExitThread` |
| `0x0044c463` | CALL | `Runtime::MSVCRT::__init_pointers` — direct call during CRT pointer-table init (alongside other `__encoded_null` helpers) |

### Disasm / decompile

```
00448a97  RET
```

Decompile (post-comment): empty function; plate comment documents CRT thread-hook role.

### Corrections vs prior hints

| Source | Claim | R8 finding |
|--------|-------|------------|
| R5 worker 16 boundary note | `LeaveCriticalSection` probe | **Incorrect for this VA** — body is only `RET`; unlock lives in separate `__unlock@0x00449b1b` |
| `master_vtable_catalog.csv` | `std::bad_alloc` vftable slots [3][4] | **Layout artifact** — `0x004895cc`/`0x004895d0` are standalone `PTR_FUN` globals used by thread CRT paths, not `bad_alloc` virtual methods (slot [2] wrongly lists `__fpmath` in same table) |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00448a97` | CRT thread-hook role + PTR_FUN pair |
| `set_plate_comment` | `0x00448a97` | `CRT null thread attach/detach callback (PTR_FUN_004895cc/d0)` |
| `force_decompile` | `0x00448a97` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static CRT wiring sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Official MSVC symbol | No library match / export name in PE; cannot assign `__threadattach`-class name without upstream CRT label |
| `PTR_FUN_004895cc` / `PTR_FUN_004895d0` labels | Data symbols left as-is; only function body annotated |
| `_Globals::FUN_00448a97` in `mapping.csv` / stubs | Export-tree sync out of scope for R8 single-FUN task |

## Evidence paths

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- `bulanci.ghidra.exe.c` (`__init_pointers`, `_threadstartex`, `__endthreadex`)
- `ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl` (PUSH `0x4895cc` / `0x4895d0`)
- `config/bulanci/mapping.csv` (`0x448a97`, size `0x1`, `void`)
