# Struct recovery — batch 28 follow-up (round 2)

**Prior:** [batch_28_summary.md](./batch_28_summary.md)  
**Structs:** `CDSDirectXException`, `CDSEasyMemStream`  
**Status:** `FOLLOWUP_COMPLETE`  
**Program:** `bulanci.exe` — `save_program` after Ghidra edits

## Prior batch follow-ups

| Item | Resolution |
|------|------------|
| Recover `CDSException` base; replace `pad_0x14`; upgrade `CDSDirectXException` | **Done (doc + Ghidra on subclass)** — `pInlineMessage` (`wchar_t[20]` @ `+0x14`) matches `CDSException_GetMessageW@0x00434b80`; `CDSDirectXException` **VERIFIED** in Ghidra (72 B, 10 fields). New [CDSException.md](./CDSException.md) documents the `0x3C` prefix. Standalone `CDSException` Ghidra struct still **1-byte placeholder** (replace blocked: type exists; inline script failed). |
| Name `CDSEasyMemStream` MI vtable bases | **Done (Ghidra)** — `pVftable_typeinfo` → `vf_IDSReferenced`, `pVftable_eventHandler` → `vf_IDSEventHandler`, `pVftable_IDSStream` → `vf_IDSStream` (dtor @ `0x00409270` restores `g_pCDSObject_vftable_IDSReferenced` on `+0`). **Not** `CDSObject` embed: heap size `0x2c` vs `CDSObject` `0x48`/`0x60`. |

Note: batch 28 summary cited “batch 29” for `CDSException`; batch 29 recovered `CDSStreamStorage` / `CDSStrmResInfo`. Base layout evidence is from **batch 36** (`CDSSimpleException`, `CDSException_InitFields` / `GetMessageW`).

## Actions taken

| Action | Result |
|--------|--------|
| Decompile `CDSException_InitFields`, `GetMessageW`, `CDSDirectXException_*`, `CDSEasyMemStream_ctor` / dtor / `CloseStream` / `IDSStream_Release` | Confirmed `0x3C` prefix and MI offsets (`IDSStream` @ `+0x0c`) |
| `get_struct_layout` | `CDSDirectXException` 72 B with `pInlineMessage`; `CDSEasyMemStream` 44 B with `vf_*` names |
| `modify_struct_field` on `CDSEasyMemStream` | Renamed three vtable pointer fields |
| Added `CDSException.md`; updated `CDSDirectXException.md`, `CDSEasyMemStream.md` | Status / MI naming aligned with evidence |
| `save_program bulanci.exe` | Once |

## Ghidra deltas

- **CDSDirectXException:** already had `pInlineMessage` and aligned base names (no further change this round).
- **CDSEasyMemStream:** `vf_IDSReferenced`, `vf_IDSEventHandler`, `vf_IDSStream`.
- **CDSException:** no type-manager change (placeholder remains).

## Remaining UNK

| Struct | Item |
|--------|------|
| `CDSDirectXException` | `What` uses `message_handle` / HRESULT helper, not inline `GetMessageW` on thrown instances |
| `CDSEasyMemStream` | `dwIdsStream_state == 7` after `FUN_00430d60`; full `IDSEventHandler` slot semantics |
| `CDSException` | Replace 1-byte Ghidra placeholder with `0x3C` struct (tooling) |

## Deliverables updated

- [CDSException.md](./CDSException.md) (new)
- [CDSDirectXException.md](./CDSDirectXException.md)
- [CDSEasyMemStream.md](./CDSEasyMemStream.md)
- [batch_28_followup_summary.md](./batch_28_followup_summary.md) (this file)
