# CDSApiException

## Status

**VERIFIED** — `sizeof == 0x44` (68 bytes); inherits [CDSException](./CDSException.md) through `+0x3b`, Win32 tail at `+0x3c` / `+0x40`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSApiException) == 0x44` | `CDSApiException_ThrowFromGetLastError@0x00434d00` | `OperatorNewWithBadAlloc(0x44)` |
| Same size on manual throw | `CDSApiException_ThrowFromWin32@0x00434d50` | `OperatorNewWithBadAlloc(0x44)` |
| Base ends at `0x3c` | `CDSApiException_ThrowFromGetLastError@0x00434d00` | After `CDSException_InitFields`, `local_4[0xf]=0` (`+0x3c`), `local_4[0x10]=GetLastError()` (`+0x40`) |
| Inline buffer span `0x14..0x3b` | `CDSException_GetMessageW@0x00434b80` | `_Dest = (wchar_t *)(param_1 + 5)` → `+0x14`; shared with `CDSException` / `CDSSimpleException` |

## Layout

### Inherited `CDSException` (`+0x00` .. `+0x3b`)

See [CDSException.md](./CDSException.md). Initialized by `CDSException_InitFields@0x00434a40`; throw paths pass code `0xe`, text index `2`, `bDeleteOnRelease=1`, then override vtable to `0x487564`.

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 60 | `CDSException` | `base` | `ThrowFromGetLastError@0x00434d00` → `CDSException_InitFields(local_4,0xe,2,1)`; `*local_4 = 0x487564` |

### Derived tail

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x3C | 4 | `wchar_t *` | `pFormattedMessage` | `CDSApiException_What@0x00434c70` → `CDsStringFormatV(&this->pFormattedMessage,…)`; `CDSApiException_dtor@0x00434b20` frees `param_1[0xf]` |
| 0x40 | 4 | `uint` | `dwWin32Error` | `ThrowFromGetLastError` → `GetLastError` into `+0x40`; `What` reads `this->dwWin32Error` |

## Ghidra apply

```
get_struct_layout("CDSApiException") → Size: 68 bytes
  CDSException base @ 0
  wchar_t * pFormattedMessage @ 0x3c
  uint dwWin32Error @ 0x40
```

Round 3 task 27 (2026-05-30): replaced flat duplicate prefix with embedded `CDSException base`; `save_program bulanci.exe`. Slice 22 agent (2026-05-30): re-verified `get_struct_layout` 68 B; decompile `What` uses `pFormattedMessage` / `dwWin32Error`.

## UNK

- None within `0x00..0x43` for instances constructed via documented throw helpers.
- Full `CDSException` hierarchy vtable slot map at `+0x00` beyond stored pointer `0x487564` not expanded here (round 3 task 34).

## Follow-up resolved (round 3 task 27)

- **Done:** formal `CDSException` Ghidra struct; `CDSApiException` embeds `base` instead of re-declaring `0x00..0x3b`.
- **Deferred:** other `0x44` siblings (`CDSResourceException`, …) still use flat prefix in Ghidra — same geometry, optional embed when touched.
