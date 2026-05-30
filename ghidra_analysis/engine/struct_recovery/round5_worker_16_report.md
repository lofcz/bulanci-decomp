# Round 5 — worker 16/50 report (FUN_* / `_Globals::` / `g_*` band)

## Task

| Field | Value |
|-------|-------|
| **worker** | 16 / 50 |
| **mode** | WRITE — evidence-only `rename_function_by_address` |
| **band** | `0x00401000`–`0x004F0000` (main `.text` + tail misc) |
| **slice** | `0x00448b26`–`0x0044d79f` (`chunk 0x4c7a` = `(0xEF000/50)`) |
| **filter** | High-xref `FUN_*` / `_Globals::FUN_*` only; rename when caller chain understood |
| **program** | `bulanci.exe` via user-ghidra-mcp |

## Status

**DONE** — slice inventory **30** `FUN_*` / thunk symbols; **2** high-xref renames applied; **28** left as CRT/stdlib (already named or insufficient game semantics). `save_program bulanci.exe`.

## Slice inventory (high-xref candidates)

| Address | Symbol (after) | Xrefs | Disposition |
|---------|----------------|------:|-------------|
| `0x00448b50` | **`CShotCounter_RoundBulletSpacing`** | 11 | **Renamed** — HUD bullet X spacing; `CShotCounter::Render@0x426e10` |
| `0x00449b1b` | **`__unlock`** | 18 | **Renamed** — MSVC lock table unlock; pairs `Runtime::MSVCRT::__lock@0x449bf3` |
| `0x00448ed6` | `CNumEdit_ParseWideDigitsToUint` | 1 | Already named (R4/prior); below high-xref threshold |
| `0x0044b490` | `_Globals::FUN_0044b490` | 1 | CRT EH catch epilogue — skip |
| `0x0044c482` | `FUN_0044c482` | 1 | `__VEC_memcpy` helper — skip |
| `0x0044c19e` | `__crtCorExitProcess` | 1 | CRT exit — already named |
| `0x00449386`–`0x0044cc24` | assorted `FUN_*` | ≤1 each | Tiny `__unlock` wrappers / `type_info` / `doexit` — skip |
| `0x0044b047` | `std::bad_exception::FUN_*` | — | EH vtable — skip |
| `0x0044bf84` | `std::bad_alloc::FUN_*` | — | EH vtable — skip |

**Boundary (worker 15):** `_Globals::FUN_00448a97@0x448a97` (5 xrefs, `LeaveCriticalSection` probe) is **below** slice lo `0x448b26`.

## Evidence

### `CShotCounter_RoundBulletSpacing` @ `0x00448b50`

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Caller chain | `CShotCounter::Render@0x00426e10` | Xrefs @ `0x00426f42`, `0x00426f9f`; decompile loop uses return low dword as bullet `local_20` X |
| Semantics | decompile @ `0x448b50` | When `DAT_004ba1d8==0`, banker's-round x87 `ST0`; else truncate to `(int)ST0` |
| Doc cross-ref | [scoring_hud.md](../../gameplay/scoring_hud.md) §3.3 | Discrete bullet spacing helper |

### `__unlock` @ `0x00449b1b`

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Implementation | `__unlock@0x449b1b` | `LeaveCriticalSection((&DAT_004b1038)[param_1*2])` |
| Paired acquire | `Runtime::MSVCRT::__lock@0x449bf3` | `EnterCriticalSection` on same table; Ghidra library match on `__lock` |
| Callers | `doexit`, `type_info__UnlockRegistry`, slice micro-stubs | 18 xrefs — CRT file/atexit paths |

### No `g_*` relabel targets in slice

Data band `0x004ae000`–`0x004ba1fb` overlaps slice VA range but holds **no** executable `FUN_*` entry points. `DAT_004ba1d8` (SSE memcpy path flag) is read by `CShotCounter_RoundBulletSpacing` but written only from CRT init @ `0x0044ff0c` — not renamed (MSVCRT global, multi-consumer).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00448b50` | `CShotCounter_RoundBulletSpacing` |
| `rename_function_by_address` | `0x00449b1b` | `__unlock` |
| `set_plate_comment` | `0x448b50`, `0x449b1b` | R5w16 pairing notes |
| `force_decompile` | `0x00426e10` | `Render` shows `CShotCounter_RoundBulletSpacing` calls |
| `save_program` | `bulanci.exe` | saved |

## Struct / doc updates

- [scoring_hud.md](../../gameplay/scoring_hud.md) — symbol name for bullet-spacing helper

## Remaining UNK

- Move `__unlock` into `Runtime::MSVCRT` namespace to match `__lock` (cosmetic Ghidra namespace; behavior proven).
- Slice micro-stubs (`FUN_00449386`, `FUN_004494ec`, …) — one-instruction forwards to `__unlock`; rename only if coordinator wants `_unlock` thunk aliases.
- `FUN_0044ab44` @ `0x44ab44` — TLS thread-start indirect call (1 xref); out of high-xref scope.
- JPEG / image paths referencing `0x448b50` from `0x46a70b` / `0x46d7xx` — mid-function in unmapped or stale xref list; live game path is `CShotCounter::Render` only in mapped `.text`.
