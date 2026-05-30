# CDSSimpleException

## Status

**VERIFIED** (size 0x3C; base layout through `inlineMessage`)

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Instance size **0x3C** (60) | `CDSSimpleException_Throw` @ `0x00434c20` | `OperatorNewWithBadAlloc(0x3c)` before `CDSException_InitFields` |
| Vtable **0x0048754c** | `CDSSimpleException_Throw` @ `0x00434c20` | `*(undefined4 *)local_4 = 0x48754c` after init |
| RTTI / class table | `CDSSimpleException_GetClassTable` @ `0x00434ac0` | Returns `&DAT_004b81fc` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------:|-----:|------|------|----------------------|
| `+0x00` | `0x3C` | `CDSException` | `base` | Embedded prefix only (`get_struct_layout`); field names in [CDSException.md](./CDSException.md) |
| (in `base`) | 4 | `void *` | `pVftable` | `CDSSimpleException_Throw@0x00434c20` → `0x48754c` after `CDSException_InitFields` (`0x487520` transient) |
| (in `base`) | `0x14..0x3b` | `wchar_t[20]` | `pInlineMessage` | `CDSException_GetMessageW@0x00434b80`; alloc `0x3c` in `CDSSimpleException_Throw` — no tail past `+0x3b` |

## Ghidra apply

```
get_struct_layout("CDSSimpleException") → Size: 60 bytes
```

Applied via `create_struct` (2026-05-30 batch 36). **Agent todo 30 (2026-05-30):** verified `get_struct_layout` → embedded `CDSException base` @ `+0` only (60 B); no flat duplicate to `delete_data_type`. Decompiler comment @ `CDSSimpleException_Throw@0x00434c20`. **Agent todo 39:** same embed pattern as `CDSApiException`.

## UNK

- **`__swprintf` length:** `CDSException_GetMessageW@0x00434b80` calls `__swprintf` into `+0x14` without an explicit count; safe use depends on format strings from `g_pCDSApp_vftable[10]` (not bounded in decompiler output).

## Follow-up resolved (batch 36 round 2)

- **IDSChained @ `0x0048754c`:** Five slots are the **primary** vptr at `+0` (`vftable_methods.csv`); no embedded secondary vtable subobject or `AdjustThisOffset` on this type. Catalog label `IDSChained` is the interface name for that table, not an extra member offset.
- **Vtable slots (proven):** `[0]` `CDSSimpleException_GetClassTable@0x00434ac0` → `&DAT_004b81fc`; `[1]` `CDSException_DtorScalar@0x00434ae0`; `[2]` `CDSException_ReleaseViaFlag@0x0042fff0`; `[3]` `CDSSimpleException_What@0x00434ad0` returns `param_1` unchanged (used from `GetMessageW` static-text path via `*(vftable+0xc)`); `[4]` `CDSObject_GetThis@0x00434b10` returns `(uint)this`.
- **`inlineMessage` span:** `0x14 + 0x28 = 0x3C` matches `OperatorNew(0x3c)` in `CDSSimpleException_Throw@0x00434c20`; no fields past `+0x13` on this subclass.
- **R4 throw local:** `local_4` is `CDSSimpleException *`; `CDSException_InitFields` call still shows `(CDSException *)` cast (decompiler limitation). See [round4_task_39_report.md](./round4_task_39_report.md).

## IDSChained vtable @ `0x0048754c`

| Slot | Address | Symbol |
|------|---------|--------|
| 0 | `0x00434ac0` | `CDSSimpleException_GetClassTable` |
| 1 | `0x00434ae0` | `CDSException_DtorScalar` (base) |
| 2 | `0x0042fff0` | `CDSException_ReleaseViaFlag` |
| 3 | `0x00434ad0` | `CDSSimpleException_What` |
| 4 | `0x00434b10` | `CDSObject_GetThis` |

## Notes

- Thrown from BMP validation (`9, 0xB`), stream unsupported ops (`4, 6`), DirectPlay host drop (`2000, 0x1b`), etc. (`CDSSimpleException_Throw` xrefs in export).
- Shares the same **0x3C** CDSException prefix as other lightweight subclasses; does not carry `CDSString` handles at `+0x3C` (contrast `CDSApiException` @ `0x44`, `CDSStreamException` @ `0x50`).
