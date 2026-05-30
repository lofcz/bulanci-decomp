# CDSRegKeyException

## Status

**VERIFIED** — `sizeof == 0x48` (72 bytes); registry-specific `CDSException` subclass thrown from `ThrowRegKeyException` on every `advapi32` failure path.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSRegKeyException) == 0x48` | `CDSRegKeyException_ThrowFromWin32@0x00437cd0` | `OperatorNewWithBadAlloc(0x48)` then `CDSRegKeyException_Ctor` |
| Same size on EH default alloc | `CDSRegKeyException_AllocDefault@0x004380c0` | `OperatorNewWithBadAlloc(0x48)`; zeroes `+0x3c`/`+0x40`/`+0x44` (`puVar1[0xf..0x11]`) |
| vtable | `CDSRegKeyException_Ctor@0x00437ba0` | `*(this+0) = 0x487630` after `CDSException_InitFields(this,8,10,1)` |
| Base ends at `0x3C` | `CDSException_GetMessageW@0x00434b80` | Shared inline buffer at `+0x14` (batch 36 / `CDSException` prefix) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable` | `CDSRegKeyException_Ctor@0x00437ba0` → `0x487630` |
| `0x04` | 1 | `byte` | `bDeleteOnRelease` | `CDSException_InitFields@0x00434a40` (`param_4 = 1`) |
| `0x05` | 3 | `byte[3]` | `pPad_05` | Structural padding (no ctor writes) |
| `0x08` | 4 | `void *` | `pMessageCache` | `CDSException_InitFields` |
| `0x0C` | 4 | `uint` | `dwCodePrimary` | `CDSException_InitFields` (`8`) |
| `0x10` | 4 | `uint` | `dwStaticTextIndex` | `CDSException_InitFields` (`10`) |
| `0x14` | `0x28` | `wchar_t[20]` | `pInlineMessage` | `CDSException_GetMessageW@0x00434b80` |
| `0x3C` | 4 | `uint` | `dwWin32Error` | `CDSRegKeyException_Ctor@0x00437ba0` (`*(this+0x3c) = param_1`); `CDSRegKeyException_What@0x00437c40` → `CDsStringFromWin32ErrorCode(..., *(this+0x3c), ...)` |
| `0x40` | 4 | `wchar_t *` | `pRegPath` | `CDSRegKeyException_Ctor` → `CDsStringAssignFromHandle(this+0x40, &param_2)`; dtor `CDSRegKeyException_dtor@0x00437b30` releases `param_1[0x10]` |
| `0x44` | 4 | `wchar_t *` | `pszFormatted` | Ctor zeroes `+0x44`; `CDSRegKeyException_What` → `CDsStringFormatV(this+0x44, format)`; dtor releases `param_1[0x11]`; returns `*(this+0x44)` or `g_pRegSubKeyDefault` |

## Ghidra apply

```
get_struct_layout("CDSRegKeyException") → Size: 72 bytes
  pVftable … pInlineMessage, dwWin32Error, pRegPath, pszFormatted (Ghidra label: pPszFormatted)
```

Slice 36 (2026-05-30): deleted 1-byte placeholder, `create_struct`, renamed `FUN_004380c0` → `CDSRegKeyException_AllocDefault`, prototypes on ctor/What, `save_program bulanci.exe`.

## UNK

- `CDSRegKeyException_AllocDefault` only referenced from EH registration (`DATA xref @ 0x0047d5c0`); live throws use `CDSRegKeyException_ThrowFromWin32` inline ctor path.
- Full `IDSChained` vtable slot map at `0x487630` not expanded.

## Cross-refs

- Registry wrapper: `ghidra_analysis/engine/registry.md` (`ThrowRegKeyException@0x00437ed0` formats `subkey\value` then calls `CDSRegKeyException_ThrowFromWin32`).
