# Slice 28 — parallel agent report

**Manifest:** `batches_50.json` index 28 → `CDSDirectXException`, `CDSEasyMemStream`  
**Program:** `bulanci.exe`

## Types mapped

| Type | Size | Status | Ghidra |
|------|------|--------|--------|
| `CDSDirectXException` | `0x48` (72 B) | VERIFIED | 10 fields; `pInlineMessage` @ `+0x14` |
| `CDSEasyMemStream` | `0x2c` (44 B) | VERIFIED | 11 fields; MI vtables `vf_*` |

## Ghidra actions

| Action | Target |
|--------|--------|
| `rename_function_by_address` | `0x0043b720` → `CDSDirectXException_ResolveMessageStringId` |
| | `0x0043b8b0` → `CDSDirectXException_AllocEmpty` |
| | `0x00430d60` → `CDSEasyMemStream_InitBackingBuffer` |
| | `0x004306e0` → `CDSEasyMemStream_EnsureCapacity` |
| | `0x00430e70` → `CDSEasyMemStream_CreateFromStreamSlice` |
| `set_function_prototype` | Above five + existing `LookupStringIdForHresult`, throw/what/dtor |
| `get_struct_layout` | Confirmed layouts (no `create_struct` needed) |
| `save_program` | `bulanci.exe` |

## Files changed

- `ghidra_analysis/engine/struct_recovery/CDSDirectXException.md`
- `ghidra_analysis/engine/struct_recovery/CDSEasyMemStream.md`
- `ghidra_analysis/engine/gameplay_struct_backlog.md`
- `ghidra_analysis/engine/struct_recovery/slice_28_summary.md` (this file)

## Blockers / UNK

1. `CDSException` standalone Ghidra type still 1-byte placeholder (subclass layouts OK).
2. `IDSStream` slot methods (`ReadBytes`/`WriteBytes`) — decompiler `this` not retyped to embedded subobject (`__thiscall` ECX limitation).
3. `CDSDirectXException_What` — return value of `ResolveMessageStringId` not visible in decompile before `CDsStringFormatV`.
4. `dwIdsStream_state == 7` semantics (initialized backing buffer).
