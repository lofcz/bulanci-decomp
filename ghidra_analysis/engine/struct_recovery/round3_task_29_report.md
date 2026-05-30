# Round 3 — Agent todo 29 report

## Task

| Field | Value |
|-------|-------|
| **id** | 29 |
| **title** | Replace CDSException 1-byte placeholder; decode CDSEasyMemStream dwIdsStream_state==7 |
| **types** | `CDSException`, `CDSEasyMemStream`, `CDSDirectXException`, `CDSSimpleException` |
| **ghidra_actions** | `create_struct`, `delete_data_type`, `modify_struct_field`, `set_decompiler_comment` |
| **source** | blocker (supersedes prior todo 29) |

## Status

**DONE**

## Evidence

### CDSException — 60-byte canonical base (placeholder resolved)

| Claim | Address | Evidence |
|-------|---------|----------|
| Base size `0x3C` | `CDSSimpleException_Throw@0x00434c20` | `OperatorNewWithBadAlloc(0x3c)`; `CDSException_InitFields`; vtable `0x48754c` |
| Prefix init | `CDSException_InitFields@0x00434a40` | `pVftable=0x487520`, `bDeleteOnRelease`, `pMessageCache=0`, `dwCodePrimary`, `dwStaticTextIndex` |
| Inline buffer `+0x14` | `CDSException_GetMessageW@0x00434b80` | lazy `swprintf` into `pInlineMessage` when static-text slot is zero |
| Ghidra struct | — | `get_struct_layout("CDSException")` → **60 bytes** (7 fields; not 1-byte placeholder) |
| Subclass embed | `get_struct_layout` | `CDSSimpleException` / `CDSDirectXException` → `CDSException base` @ `+0` |

### CDSEasyMemStream — `dwStreamState` @ outer `+0x10`

| Value | Name | Writers | Evidence |
|-------|------|---------|----------|
| `0x20` | **CLOSED** | ctor, `CloseStream` | `CDSEasyMemStream_ctor@0x00409170` asm `MOV [ESI+0x10],0x20` before `InitBackingBuffer`; `CloseStream@0x00409200` → `*(IDSStream+4)=0x20` after `ReleaseBackingBuffer` |
| `7` | **OPEN** | `InitBackingBuffer` | `CDSEasyMemStream_InitBackingBuffer@0x00430d60` asm `MOV [ESI+0x10],0x7` after successful malloc @ `+0x28` (`backing_heap`) |
| errno `8` on I/O | — | `ReadBytes@0x004307f0` | null backing → `ThrowStreamErrorNoReturn(8,…)` (pre-init or post-release) |

**MI note:** `IDSStream` facet is at outer `+0x0c`; decompiler field `dwStreamState` is outer `+0x10` (= `IDSStream+4`). Legacy alias `dwIdsStream_state` in batch notes refers to the same dword.

### CDSDirectXException cross-check

| Claim | Address | Evidence |
|-------|---------|----------|
| Alloc `0x48` | `CDSDirectXException_ThrowFromHresult@0x0043b820` | `OperatorNewWithBadAlloc(0x48)`; `CDSException_InitFields(this,0xf,3,1)`; tail via embedded base+`0x3c` |
| `What` vs inline | `CDSDirectXException_What@0x0043b7c0` | formats `message_handle` @ `+0x3c`; does **not** use `GetMessageW` / `pInlineMessage` on throw path |

## Ghidra deltas (verification pass)

- **`CDSException`:** 60 B — `pVftable`, `bDeleteOnRelease`, `pPad_05`, `pMessageCache`, `dwCodePrimary`, `dwStaticTextIndex`, `pInlineMessage wchar_t[20]`.
- **`CDSSimpleException`:** embeds `CDSException base` @ `+0` (60 B total).
- **`CDSDirectXException`:** embeds `CDSException base` @ `+0`; tail `pMessage_handle`, `dwContext_code`, `dwHresult` @ `+0x3c..+0x47` (72 B).
- **`CDSEasyMemStream`:** 44 B; `dwStreamState` @ `+0x10`; PRE_COMMENT on ctor, `InitBackingBuffer`, `CloseStream`.
- Prototypes/decompile: `CDSException_InitFields`, `CDSException_GetMessageW`, `CDSEasyMemStream_*` use struct fields (no re-apply needed this slice).

## Struct doc updates

- [CDSException.md](./CDSException.md) — VERIFIED; agent todo 29 note on 60 B base + subclass embeds.
- [CDSEasyMemStream.md](./CDSEasyMemStream.md) — `dwStreamState` OPEN/CLOSED semantics resolved.
- [CDSDirectXException.md](./CDSDirectXException.md) — embedded base @ `+0`.

## Remaining UNK

- `ReadBytes`/`WriteBytes` on `IDSStream` ECX still mis-map outer fields unless `this` is typed at `outer+0x0c` (known Ghidra `__thiscall` limitation).
- Other exception siblings (`CDSResourceException`, `CDSStreamException`, …) may still use flat prefix fields in decompile while byte-equivalent to embed (optional dedup — agent todo 39).
