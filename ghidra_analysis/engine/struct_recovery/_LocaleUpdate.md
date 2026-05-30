# _LocaleUpdate

## Status

**SKIP** (apply) — MSVC VS2005 CRT stack RAII guard for per-thread locale; **layout documented** below (size/fields proven). Not heap-allocated by game code. Do not `create_struct` per CRT stub rule.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Last field ends at offset 0x0C | `0x004487c8` | Ctor: `this[0xc] = 0` / `this[0xc] = 1` |
| Minimum size 0x0D (13 bytes) | `0x004487c8` | Contiguous writes at 0, 4, 8, 0xC |
| Stack slot layout | `0x004489de` | `FID_conflict:__wcslwr_s_l`: `local_14` base, `local_c` @ +8, `local_8` @ +0xC (flag) |
| Consumer uses +8 → PTD +0x70 | `0x0044c8ab` | `getSystemCP`: `*(uint *)(local_c + 0x70) &= ~2` when `local_8 != 0` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `pthreadlocinfo` | locinfo | `_LocaleUpdate@0x004487c8`: `*(pthreadlocinfo *)this = ...` |
| 0x04 | 4 | `pthreadmbcinfo` | mbcinfo | `_LocaleUpdate@0x004487c8`: `*(pthreadmbcinfo *)(this + 4) = ...` |
| 0x08 | 4 | `_ptiddata` | ptd | `_LocaleUpdate@0x004487c8`: `*(_ptiddata *)(this + 8) = p_Var2` (PTD-null path) |
| 0x0C | 1 | `byte` | updated | `_LocaleUpdate@0x004487c8`: `this[0xc]` set 0/1; `getSystemCP@0x0044c8ab` tests `local_8` |

**Effective size:** 0x0D bytes (13). No trailing padding proven; callers use raw stack arrays.

### Ctor branches (`param_1` = `localeinfo_struct *`)

- **`param_1 == NULL`:** snapshot current thread `ptlocinfo` / `ptmbcinfo` from `__getptd()`, optionally refresh via `___updatetlocinfo` / `___updatetmbcinfo`, may OR `0x2` into `ptd+0x70` and set `updated=1`.
- **`param_1 != NULL`:** copy `param_1->locinfo` and `param_1->mbcinfo` only (no PTD field use in ctor).

### Teardown (no dedicated dtor symbol)

Callers clear flag manually, e.g. `getSystemCP@0x0044c8ab`:

```c
if (local_8 != '\0')
  *(uint *)(local_c + 0x70) &= 0xfffffffd;
```

## Ghidra apply

Not applied (CRT SKIP). Verified 2026-05-30 (agent slice 45):

```
get_struct_layout _LocaleUpdate  → Size: 1 (placeholder; /Demangler/_LocaleUpdate)
```

Decompile confirms field writes at `this+0`, `this+4`, `this+8`, `this[0xc]` (`_LocaleUpdate@0x004487c8`). Consumer `getSystemCP@0x0044c8ab` uses stack `local_14[2]` + `local_c` (+8) + `local_8` (+0xC) for manual teardown.

Proven layout could be applied later as 13-byte struct if CRT typing is desired; omitted to avoid polluting game struct namespace.

## UNK

- Explicit `~_LocaleUpdate` (inlined epilogue pattern only).
- Padding after 0x0C if any caller assumes 16-byte alignment (not observed).

## Cross-refs

Ctor `_LocaleUpdate@0x004487c8` — **24+** unconditional CRT callers (sample xrefs): `__woutput_l`, `__mbtowc_l`, `getSystemCP`, `___crtLCMapStringA`, `strtoxl`, `wcstoxl`, `FID_conflict:__wcslwr_s_l`, `__cftoa_l`, etc. All MSVCRT; none are Bulanci gameplay types.

Related: `__updatetlocinfoEx_nolock@0x0044cecd`, `___updatetlocinfo@0x0044cf20` (namespace `_LocaleUpdate` in export).
