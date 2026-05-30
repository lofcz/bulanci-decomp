# CDSException

## Status

**VERIFIED** — logical size `0x3C` (60 bytes); canonical Ghidra base for all engine exception prefixes `0x00..0x3b`. Field geometry matches `CDSSimpleException` and the flat prefix documented on `CDSApiException`, `CDSResourceException`, `CDSDirectXException`, etc.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Base object ends at `0x3C` | `CDSSimpleException_Throw@0x00434c20` | `OperatorNewWithBadAlloc(0x3c)` — no tail fields |
| Same prefix on larger subclasses | `CDSApiException_ThrowFromGetLastError@0x00434d00` | `InitFields` then `*(obj+0x3c)=0`, `*(obj+0x40)=error` |
| Inline buffer extent | `CDSException_GetMessageW@0x00434b80` | `_Dest = (wchar_t *)(param_1 + 5)` → `this+0x14`; `0x3c - 0x14 = 0x28` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `pVftable` | `CDSException_InitFields@0x00434a40` → `0x487520` (overridden by subclasses) |
| `+0x04` | 1 | `byte` | `bDeleteOnRelease` | `CDSException_InitFields` (`*(byte *)(this+4) = param_4`); `CDSException_ReleaseViaFlag@0x0042fff0` |
| `+0x05` | 3 | `byte[3]` | `pPad_05` | Structural padding (no ctor writes) |
| `+0x08` | 4 | `void *` | `pMessageCache` | `CDSException_InitFields` zeroes `this[2]`; `CDSException_GetMessageW` lazy-fills |
| `+0x0c` | 4 | `uint` | `dwCodePrimary` | `CDSException_InitFields` (`this[3] = param_2`); `GetMessageW` `swprintf` arg |
| `+0x10` | 4 | `uint` | `dwStaticTextIndex` | `CDSException_InitFields` (`this[4] = param_3`); indexes `g_apCDSStaticTextsSingleton` |
| `+0x14` | `0x28` | `wchar_t[20]` | `pInlineMessage` | `CDSException_GetMessageW@0x00434b80` formats into `this+0x14` when static text slot is zero |

## Ghidra apply

```
get_struct_layout("CDSException") → Size: 60 bytes
  pVftable, bDeleteOnRelease, pPad_05, pMessageCache,
  dwCodePrimary, dwStaticTextIndex, pInlineMessage (wchar_t[20])
```

Applied round 3 task 27 (2026-05-30): grew the former 1-byte placeholder via `add_struct_field` (seven fields, same names as `CDSSimpleException`). `CDSApiException` now embeds `CDSException base` at `+0` with tail at `+0x3c` / `+0x40`.

Slice 29 (2026-05-30): re-verified `get_struct_layout` → 60 B; prototypes on `CDSException_InitFields@0x00434a40` and `CDSException_GetMessageW@0x00434b80`.

**Agent todo 29 (2026-05-30):** Confirmed 60 B canonical base (1-byte placeholder fully replaced). `CDSDirectXException` now embeds `CDSException base` @ `+0`. `CDSEasyMemStream.dwStreamState`: `7`=OPEN after `InitBackingBuffer@0x00430d60`, `0x20`=CLOSED in ctor/`CloseStream@0x00409200`.

**Agent todo 39 (2026-05-30):** Re-verified `CDSException` 60 B; embedded `CDSException base` @ `+0` on `CDSMemoryException`, `CDSResourceException`, `CDSStreamException` (matches `CDSApiException` pattern). `CDSException_InitFields` / `CDSException_GetMessageW` prototypes refreshed.

## Shared IDSChained helpers

| Slot role | Address | Notes |
|-----------|---------|-------|
| `ReleaseViaFlag` | `0x0042fff0` | Slot `[2]` on exception vtables; calls scalar dtor when `bDeleteOnRelease` |
| `DtorScalar` (base) | `0x00434ae0` | Used as slot `[1]` on `CDSSimpleException`; subclass-specific dtors elsewhere |
| `CDSObject_GetThis` | `0x00434b10` | Slot `[4]` on exception siblings |

**Agent todo 39 r4 (2026-05-30):** exception throw-site locals + per-class vtable tables — [round4_task_39_report.md](./round4_task_39_report.md).

## UNK

- Secondary `IDSChained` / `IDSEventHandler` faces beyond the primary 5-slot table at `+0` (no separate subobject offsets in throw paths).
- `CDSSimpleException` embeds `CDSException base` @ `+0` (agent todos 30 / 39, 2026-05-30) — see `round3_task_30_report.md`.

## Notes

- MSVC game exception type (`__CxxThrowException` + per-class type info), not CRT `std::exception`.
- Subclasses (`CDSApiException` `0x44`, `CDSDirectXException` `0x48`, `CDSStreamException` `0x50`, …) append fields at `+0x3c` or later without shrinking the prefix.

## Follow-up resolved (round 3 task 27)

- Formal `CDSException` Ghidra struct replaces the 1-byte placeholder (deferred since batch 28 / round 3 task 10).
- `CDSApiException` prefix `0x00..0x3b` deduplicated via embedded `base` component.
