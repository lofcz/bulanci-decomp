# CDSResourceException

## Status

**VERIFIED** — `sizeof == 0x44`; shared `CDSException` prefix through `pInlineMessage` (batch 36 geometry) plus `dwResourceId` / `pszFormatted`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSResourceException) == 0x44` | `0x00439394` | `_Globals::CreateObject` → `OperatorNewWithBadAlloc(0x44)` then `CDSResourceException_ctor` |
| `sizeof(CDSResourceException) == 0x44` | `0x004346f0` | Throw helper → `OperatorNewWithBadAlloc(0x44)` then `CDSResourceException_ctor(this, id)` → `__CxxThrowException` |
| vtable | `0x004874b0` | `CDSResourceException_ctor` writes `*(this+0) = 0x4874b0` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable` | `CDSResourceException_ctor@0x004344a0` → `0x4874b0` |
| 0x04 | 1 | `byte` | `bDeleteOnRelease` | `CDSException_InitFields@0x00434a40` via ctor (`this[4] = 1`) |
| 0x05 | 3 | `byte[3]` | `pPad_05` | Structural padding (no ctor writes); same as `CDSSimpleException` |
| 0x08 | 4 | `void *` | `pMessageCache` | `CDSException_InitFields@0x00434a40` (`*(this+8) = 0`); lazy fill via `CDSException_GetMessageW@0x00434b80` using `this+0x14` |
| 0x0C | 4 | `uint` | `dwCodePrimary` | `CDSException_InitFields@0x00434a40` (`*(this+0xC) = 6`) |
| 0x10 | 4 | `uint` | `dwStaticTextIndex` | `CDSException_InitFields@0x00434a40` (`*(this+0x10) = 8`) |
| 0x14 | 40 | `wchar_t[20]` | `pInlineMessage` | `CDSException_GetMessageW@0x00434b80` formats into `param_1 + 0x14`; base ends at `0x3C` (same as `CDSSimpleException` / `CDSStreamException`) |
| 0x3C | 4 | `uint` | `dwResourceId` | `CDSResourceException_ctor@0x00434400` (`*(this+0x3C) = param_1`); throw `FUN_004346f0@0x004346f0` |
| 0x40 | 4 | `void *` | `pszFormatted` | `CDSResourceException_ctor@0x004344a0` zeroes `+0x40`; `CDSResourceException_What@0x00434350` `CDsStringFormatV(&this->pszFormatted, …)`; dtor `param_1[0x10]` release @ `0x00434440` |

## Key functions (leaf map)

| Symbol | Address | Role |
|--------|---------|------|
| `CDSResourceException_ctor` | `0x00434400` | `InitFields(6,8,1)`; `pVftable=0x4874b0`; `dwResourceId=param`; clears `pszFormatted` dword via `this[1].bDeleteOnRelease=0` @ `+0x40` |
| `CDSResourceException_ctor_default` | `0x004344a0` | Same init/vtable; zeroes tail dword at `+0x40` only (factory default ctor) |
| `CDSResourceException_What` | `0x00434350` | `CDsStringFormatV(&pszFormatted, …)` |
| `CDSResourceException_dtor` | `0x00434440` | `CDsStringReleaseHeader` on `param_1[0x10]` (`+0x40`) when non-null |
| `CDSResourceException_ThrowFromResourceId` | `0x004346f0` | `OperatorNewWithBadAlloc(0x44)` → `CDSResourceException_ctor(id)` → `__CxxThrowException`; xref from `CDSStreamStorage_CloseStreamByKey@0x00433d1c` |

## Ghidra apply

```
get_struct_layout("CDSResourceException") → Size: 68 bytes
  CDSException base @ +0 (60 B),
  dwResourceId @ +0x3C,
  pszFormatted @ +0x40
```

Slice **37** (2026-05-30): struct confirmed in Ghidra; renamed `FUN_004346f0` → `CDSResourceException_ThrowFromResourceId`; split default ctor rename `0x4344a0` → `CDSResourceException_ctor_default`; prototypes on ctor/What/dtor/throw.

**Agent todo 39 (2026-05-30):** Rebuilt with embedded `CDSException base`; `CDSResourceException_ctor` prototype set (`CDSResourceException *` this). Decompiler `this` typing still limited by Ghidra `__thiscall` API.

## UNK

- `CDSResourceException_What` format string / `CDsStringFormatV` template id not recovered.
- Secondary vtable slots on `0x4874b0` not expanded (same deferral as other `CDSException` subclasses).
