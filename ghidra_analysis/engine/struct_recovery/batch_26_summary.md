# Struct recovery batch 26/50

**Index:** 26 (`batches_50.json`)  
**Structs:** `CDSDirectXException`, `CDSEasyMemStream`  
**Protocol:** evidence-only (`AGENT_PROTOCOL.md`)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSEasyMemStream` | `VERIFIED` | `0x2c` (44) | Layout applied; trimmed duplicate tail field → 44 B |
| `CDSDirectXException` | `PARTIAL` | `0x48` (72) | Layout present; `+0x14..+0x3b` padding only |

## Evidence highlights

### CDSEasyMemStream

- Allocation: `OperatorNewWithBadAlloc(0x2c)` (e.g. factory @ `0x004306b0`).
- `IDSStream` subobject at `+0x0c` (`mdisp`); stream I/O uses adjusted `this` (see `WriteBytes` calling `FUN_004306e0(this - 0xc)` @ `0x004308c0`).
- Backing store: `FUN_00430d60@0x00430d60` sets `+0x28`; dtor frees `param_1[10]` @ `0x00409270`.

### CDSDirectXException

- Allocation: `OperatorNewWithBadAlloc(0x48)` in `CDSDirectXException_ThrowFromHresult@0x0043b820` and factory `FUN_0043b8b0@0x0043b8b0`.
- `CDSException_InitFields(this, 0xf, 3, 1)` then subclass fields `+0x3c` (message), `+0x40` (HRESULT class), `+0x44` (HRESULT).
- `FUN_0043b720@0x0043b720` maps HRESULT → string id; default string from `g_apCDSStaticTextsSingleton[2]+0x50`.

## Deliverables

- `CDSEasyMemStream.md`
- `CDSDirectXException.md`
- `save_program bulanci.exe` (end of batch)

## Notes

- Prior analysis in `ghidra_analysis/formats/stream_hierarchy.md` §2.3 aligns with MCP-decompiled slot implementations (`ReadBytes@0x004307f0`, etc.).
- `CDSEasyMemStream` Ghidra field names (`pVftable_*`, `dwCursor`, …) kept; semantics documented in per-struct MD.
