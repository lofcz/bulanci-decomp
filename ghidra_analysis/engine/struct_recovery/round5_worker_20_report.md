# Round 5 — Worker 20 Report

## Task

| Field | Value |
|-------|-------|
| **worker** | 20 / 50 |
| **title** | FUN_* 0x4F0000–0x510000 misc |
| **mode** | WRITE (evidence-only) |
| **program** | `bulanci.exe` via user-ghidra-mcp |

## Status

**BLOCKED** — assigned virtual-address window `0x004F0000`–`0x00510000` is **not mapped** in `bulanci.exe`. Ghidra lists **zero** `FUN_*` (zero functions of any name) with entry points in that range. No evidence-backed rename or `set_function_this_type` actions are possible without expanding scope.

## Evidence

| Claim | func@addr / tool | Evidence |
|-------|------------------|----------|
| Image ends before worker range | Ghidra memory map | Last mapped block `.rsrc` ends `0x004bcfff`; worker lo `0x004F0000` is **+0x33000** past image tail |
| No executable section in range | Memory blocks | `.text` = `0x00401000`–`0x0047efff` only; nothing executable at `≥ 0x004F0000` |
| `read_memory` fails in range | `read_memory@0x004f0000`, `@0x00500000` | `Unable to read bytes at ram:004f0000` / `ram:00500000` |
| Zero functions in range | `run_script_inline` enumerate `0x004F0000`–`0x00510000` | `FN_COUNT=0`; `get_function_by_address@0x004f0000` → no function |
| No `FUN_004f*` / `FUN_0050*` symbols | `search_functions` patterns `FUN_004f`, `FUN_0050` | No matches |
| Highest remaining `FUN_*` below range | `search_functions_enhanced` sort by address | Last `FUN_*` = `FUN_0047ee90@0x0047ee90` (tail CRT/atexit stub cluster); last overall fn = `std_bad_alloc_atexit_teardown@0x0047ee91` |
| Nearest mapped regions | Memory map overlap query | `.rdata 0x0047f000–0x004adfff`, `.data 0x004ae000–0x004ba1fb`, `.rsrc 0x004bb000–0x004bcfff` — all **below** `0x004F0000` and contain **no** `FUN_*` entry points |
| Total unrename `FUN_*` in binary | `search_functions_enhanced name_pattern=FUN_` | **565** symbols (all at addresses `< 0x00480000`; none in worker slice) |

### Memory map (bulanci.exe)

```
Headers  0x00400000–0x00400fff  exec=false
.text    0x00401000–0x0047efff  exec=true
.rdata   0x0047f000–0x004adfff  exec=false
.data    0x004ae000–0x004ba1fb  exec=false
.rsrc    0x004bb000–0x004bcfff  exec=false
tdb      0xffdff000–0xffdfffff  exec=false
(gap — unmapped from 0x004bd000 through worker range and beyond)
```

### Boundary sample (last `FUN_*` in `.text`, outside this worker)

| Symbol | Address | Xrefs | Note |
|--------|---------|-------|------|
| `FUN_0047ee90` | `0x0047ee90` | 1 (DATA) | atexit no-op stub; xref `PUSH 0x0047ee90` @ `0x0047de29` |
| `std_bad_alloc_atexit_teardown` | `0x0047ee91` | — | last function body in `.text` |

Decompile (`FUN_0047ee90`): empty return; plate comment already marks uncertain atexit dtor stub. **Not renamed** (insufficient semantic proof; also **outside** worker address slice — belongs to worker 49 range `0x460000–0x490000`).

## Ghidra deltas

none — no mutations; `save_program` not invoked.

## Struct doc updates

none

## Remaining UNK / coordinator follow-up

- **Slice definition:** Worker 20 VA window appears mis-scoped relative to `bulanci.exe` layout. Likely intent was either (a) tail `.text` CRT/misc stubs already assigned to worker 49 (`0x460000–0x490000`), or (b) a `.rdata`/`.data` data-label pass (no `FUN_*` there today).
- **Suggested remap (coordinator):** `0x0047F000–0x004BD000` for read-only/data misc symbols, **or** close worker 20 as empty/no-op.
- **565 `FUN_*` remain** elsewhere; this worker cannot act on them without violating assigned scope.
