# CDSMemoryException

## Status

**PARTIAL** — `sizeof == 0x240` allocation-proven (factory + static singleton span); prefix via embedded `CDSException base` @ `+0` (includes unused `base.pInlineMessage` @ `+0x14..+0x3B`); derived tail `dwFormatArg` @ `+0x3C`, inline `pFormatted` @ `+0x40`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSMemoryException) == 0x240` | `CDSMemoryException_Factory` @ `0x00434aa0` | `PUSH 0x240` → `OperatorNewWithBadAlloc` (`0x00447c42`) then `CDSMemoryException_ctor` |
| Static singleton span `0x240` | `DAT_004b7f80` … next blob `0x004b81c0` | `0x004b81c0 − 0x004b7f80 == 0x240`; ctor target @ `StaticInit_CDSMemoryExceptionSingleton` @ `0x0047d290` |
| `Runtime_ThrowBadAlloc` uses static object | `Runtime_ThrowBadAlloc` @ `0x004349e0` | `__CxxThrowException_8(&param_1, &DAT_004ab3fc)` with `param_1 = &DAT_004b7f80`; writes throw arg to `+0x3C` (`DAT_004b7fbc`) |
| vtable `0x00487534` | `CDSMemoryException_ctor` @ `0x00434a70` | `*(this+0) = 0x487534` after `CDSException_InitFields` |
| Class registry | `CDSMemoryException_GetClassTable` @ `0x00434a90` | Returns `&DAT_004b81d4`; factory `0x00434aa0` registered classId **5** @ `0x0047d2d0` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 60 | `CDSException` | `base` | `CDSMemoryException_ctor@0x00434a70` — `CDSException_InitFields(&base,1,1,0)`; `(base).pVftable = 0x487534`; unused `base.pInlineMessage` |
| 0x3C | 4 | `uint` | `dwFormatArg` | Ctor zeroes; `CDSMemoryException_What@0x004349c0` → `__swprintf`; `Runtime_ThrowBadAlloc@0x004349e0` on static singleton |
| 0x40 | 512 | `wchar_t[256]` | `pFormatted` | `CDSMemoryException_What@0x004349c0` — `__swprintf(this->pFormatted, fmt, arg)`; returns `this->pFormatted` |

## Ghidra apply

```
get_struct_layout("CDSMemoryException") → Size: 576 bytes
  CDSException base @ +0 (60 B),
  dwFormatArg @ +0x3C,
  pFormatted wchar_t[256] @ +0x40
```

Applied slice 33: `create_struct` (576 B); `apply_data_type` @ `CDSMemoryException_004b7f80`; prototypes on ctor/`What`/factory; renamed `StaticDtor_CDSMemoryExceptionSingleton_atexit` @ `0x0047ea50`.

**Agent todo 39 (2026-05-30):** Rebuilt struct with embedded `CDSException base` (replaces flat prefix + `pUnkBaseTail`); `CDSMemoryException_What` decompile uses `this->dwFormatArg` / `this->pFormatted`.

## UNK

- **`base.pInlineMessage` / `+0x14..+0x3B`**: Inherited `CDSException` slot — not written by `CDSMemoryException_ctor`, not read by `What` (formats into `pFormatted` @ `+0x40` instead).
- **`wchar_t[256]` capacity** at `+0x40` (`pFormatted`): derived from `0x240 − 0x40`; only consumer is `__swprintf` in `What` (no bounds check).
- **`StaticDtor_CDSMemoryExceptionSingleton_atexit` @ `0x0047ea50`**: `_atexit` from `StaticInit_CDSMemoryExceptionSingleton`; overwrites `CDSMemoryException_004b7f80.pVftable` with `g_pCDSObject_vftable_IDSReferenced` at process exit.
- Secondary vtables on `IDSChained` facet (`master_vtable_catalog.csv` lists 5 slots @ `0x00487534`).

## Notes

- Vtable slot 3 = `CDSMemoryException_What` (`0x004349c0`); slot 4 = `CDSObject_GetThis` (`0x00434b10`, shared thunk).
- Differs from sibling `0x44` exceptions (`CDSApiException`, `CDSResourceException`): larger heap allocation for **inline** formatted message buffer vs pointer at `+0x40`.
- Shares `CDSException_DtorScalar` / `CDSException_ReleaseViaFlag` with other engine exceptions.
