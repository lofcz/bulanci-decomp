# Struct recovery — batch 26 follow-up (round 2)

**Prior:** `batch_26_summary.md`  
**Structs:** `CDSDirectXException`, `CDSEasyMemStream`  
**Status:** `FOLLOWUP_COMPLETE`  
**Program:** `bulanci.exe` — `save_program` after Ghidra edits

## Actions

| Struct | Prior | After | Ghidra deltas |
|--------|-------|-------|----------------|
| `CDSDirectXException` | `PARTIAL` | `VERIFIED` | `pPad_0x14` → `pInlineMessage` (`wchar_t[20]`); base fields renamed to match `CDSSimpleException` (`bDeleteOnRelease`, `pMessageCache`, `dwCodePrimary`, `dwStaticTextIndex`) |
| `CDSEasyMemStream` | `VERIFIED` | `VERIFIED` | `dwStream_state` → `dwIdsStream_state` (docs + type manager) |

## Evidence resolved (UNK from batch 26)

### CDSDirectXException — `+0x14..+0x3b`

- **Resolved:** inherited `CDSException` inline message buffer (`wchar_t[20]` @ `+0x14`, 0x28 bytes), same as batch 36 `CDSSimpleException` / `CDSException_GetMessageW@0x00434b80` (`_Dest = param_1 + 0x14`).
- Throw path still only touches tail (`+0x3c..+0x44`); `What` formats into `message_handle`, not the inline buffer.

### CDSDirectXException — `CDSException` base vs siblings

- **Resolved for this type:** prefix `0x00..0x3b` matches `CDSException_InitFields` + `GetMessageW` contract; `0x48` alloc matches `CDSRegKeyException` / `CDSResourceException` geometry.
- **Deferred:** formal `CDSException` Ghidra struct (manifest batch 29).

### CDSEasyMemStream — refcount / stream state names

- **`dwRefcount` @ `+0x08`:** `IDSStream_Release@0x00409490` → `IDSStream_ReleaseRefcount(outer)` with `outer = idsStreamThis - 0xc`; refcount field at `outer+8`.
- **`dwIdsStream_state` @ `+0x10`:** `CloseStream@0x00409200` writes closed sentinel `0x20` to `IDSStream+4`.

### CDSEasyMemStream — `CDSFilterStream` prefix

- **Rejected:** parallel `IDSStream` implementers in RTTI (`stream_hierarchy.md` §2); `OperatorNew(0x2c)` cannot host `CDSFilterStream` layout (inner stream @ `+0x24`, u64 cursor/size, etc.).

## Remaining UNK

- `CDSDirectXException_What` vs `GetMessageW` / inline buffer usage on thrown instances.
- `dwIdsStream_state == 7` semantics after `FUN_00430d60` init.
- `IDSReferenced` / `IDSEventHandler` subobjects on `CDSEasyMemStream` (vtable catalog only).
- Standalone `CDSException` type in Ghidra.

## Deliverables updated

- `CDSDirectXException.md`
- `CDSEasyMemStream.md`
- `batch_26_followup_summary.md` (this file)
