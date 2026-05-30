# Round 5 — Worker 48 Report

## Task

| Field | Value |
|-------|-------|
| **worker** | 48 / 50 |
| **title** | Top-xref `FUN_*` rename in `0x00430000`–`0x00460000` |
| **mode** | WRITE — Ghidra MCP evidence-only |
| **program** | `bulanci.exe` via user-ghidra-mcp |

## Status

**DONE** — Ghidra inline survey found **98** `FUN_*` entry points in slice; **top 5 by xref count** renamed (or confirmed) with disasm + caller proof; `save_program bulanci.exe`.

## Survey (xref-ranked `FUN_*`, slice `0x430000`–`0x460000`)

| Rank | Address | Xrefs | Prior Ghidra name | Action |
|------|---------|------:|-------------------|--------|
| 1 | `0x004344c0` | 185 | `CDSObject_EhReleaseOwnedPtr` | Renamed → `eh_dtor_CDSObject_ptr` |
| 2 | `0x00449b1b` | 18 | `__unlock` | **Already named** — prototype/comment refreshed |
| 3 | `0x00448b50` | 11 | `CShotCounter_RoundBulletSpacing` | Renamed → `CShotCounter_RoundFloatBankers` |
| 4 | `0x0045f880` | 10 | `FUN_0045f880` | Renamed → `IJG_jzero_far` |
| 5 | `0x0045a220` | 9 | `FUN_0045a220` | Renamed → `III_requantize` |

**Slice total:** 98 unrename `FUN_*` at survey time; **93** remain for later codec/CRT passes.

## Evidence — renames

### 1. `eh_dtor_CDSObject_ptr` @ `0x004344c0` (185 xrefs)

| Proof | Detail |
|-------|--------|
| Body | `if (*ppOwned) (**(code**)(*(*ppOwned)+8))()` — vtable slot **+8** = `CDSObject_ReleaseViaVtable` @ `0x004245c0` |
| Xrefs | **185** — exclusively MSVC **`Unwind@0x474xxx`** EH epilogues (no gameplay direct `CALL`) |
| Docs | `main_menu.md` / `gameplay/main_menu.md`: `eh_dtor_iterator(slot_views, 4, 2, …)` |
| mapping.csv | `_Globals::FUN_004344c0` |

### 2. `__unlock` @ `0x00449b1b` (18 xrefs)

| Proof | Detail |
|-------|--------|
| Body | `LeaveCriticalSection((LPCRITICAL_SECTION)(&DAT_004b1038)[lockIndex * 2])` |
| Pair | `Runtime::MSVCRT::__lock@0x00449bf3` — same `DAT_004b1038` lock table, `EnterCriticalSection` |
| Callers | CRT SEH stubs (`doexit`, `type_info__UnlockRegistry`, `FUN_0044c3e1`, …) with plate comments `_unlock(N)` |
| mapping.csv | `_Globals::FUN_00449b1b` |

### 3. `CShotCounter_RoundFloatBankers` @ `0x00448b50` (11 xrefs)

| Proof | Detail |
|-------|--------|
| Callers | `CShotCounter::Render@0x00426f42/0x00426f9f` — discrete bullet X spacing; JPEG paths `0x0046a70b`, `0x0046d7xx` |
| Body | Reads **`ST0`**; when `DAT_004ba1d8 == 0`, banker's-round via `ROUND(in_ST0)` + tie-break logic |
| Docs | `scoring_hud.md`: “rounding helper (`0x00448b50`)” |
| mapping.csv | `CShotCounter::FUN_00448b50` |

### 4. `IJG_jzero_far` @ `0x0045f880` (10 xrefs)

| Proof | Detail |
|-------|--------|
| Body | `Runtime::MSVCRT::_memset(ptr, 0, len)` — zero-fill only |
| Callers | `jpeg_decompress_data`, `jpeg_compress_data`, `compress_first_pass`, `FUN_004666a0` cluster — **no** gameplay `.text` |
| Upstream | IJG `jzero_far` / zero-block helper (memset wrapper in this build) |
| mapping.csv | `_Globals::FUN_0045f880` |

### 5. `III_requantize` @ `0x0045a220` (9 xrefs)

| Proof | Detail |
|-------|--------|
| Callers | **All 9** in `_Globals::III_huffdecode@0x0045a2c0` |
| Algorithm | Matches libmad `layer3.c::III_requantize`: `frac = exp % 4`, `exp /= 4`, mantissa from **`rq_table`** @ `0x0048bee0`, optional `mad_f_mul` via **`root_table`** @ `0x0049bf64` |
| Distinct | `CDSMpx::mad_layer_III_requantize@0x0045cfc0` (106 B, cdecl) — called from `III_decode@0x0045d030`, not huff hot path |
| mapping.csv | `_Globals::FUN_0045a220` |

## Ghidra deltas

- `rename_function_by_address` — `eh_dtor_CDSObject_ptr`, `CShotCounter_RoundFloatBankers`, `III_requantize`, `IJG_jzero_far` (`__unlock` unchanged)
- `set_function_prototype` — all five addresses (see Evidence)
- `set_decompiler_comment` — all five entry points
- `save_program bulanci.exe`

## Struct doc updates

none (codec/CRT/EH helpers only)

## Remaining UNK

- **93** other `FUN_*` in `0x430000`–`0x460000` (jpeg/libmad/zlib tails, small CRT stubs, filter-stream helpers) — defer to workers 47/49 band passes or targeted codec tasks
- `eh_dtor_CDSObject_ptr` jumptable @ `0x004344cb` still “too many branches” in decompiler (indirect vcall artifact)
