# Struct recovery batch 40/50 (agent slice 40)

**Structs:** `CDSStreamStorage`, `CDSStrmResInfo`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CDSStreamStorage` | **VERIFIED** | `0x60` | 19 flat fields + embedded `CDSCollection` @ `+0x1c`, `CDSChain` @ `+0x34`, `CRITICAL_SECTION` @ `+0x48` |
| `CDSStrmResInfo` | **VERIFIED** | `0x28` | Keys `+0x8`/`+0xc`, `loaderAux` @ `+0x10`, `streamExtent` @ `+0x14`, `streamFlags` @ `+0x1c`, `pad_tail` @ `+0x20` |

## Key evidence anchors

- `CBulanci::CDSStreamStorage_ctor` @ `0x00401790` — xref `CBulanci_OpenPackStream@0x00401f36`; `OperatorNew(0x60)` on standalone path.
- `CDSStreamStorage_dtor` @ `0x00433e60` — chain/collection/safe-stream + critical section.
- `CDSStreamStorage_FindKeyIndex` @ `0x00431170` — `CDSCollection` at `this+0x1c`.
- `CDSStreamStorage_CloseStreamByKey` @ `0x00433cb0` — stack `CDSStrmResInfo` keys (`0x4873f8` / `0x4873dc`).
- `CDSStreamStorage_InitRootSafeStream` @ `0x00434160` — `pRootSafeStream` + `streamBaseOffset`.
- `CDSStreamStorage_GetThreadLoaderNode` @ `0x00433f00` — per-thread node on `CDSChain` @ `+0x34`.
- `CDSStreamStorage_AppendOrReuseStream` @ `0x00433f70` — locked append; typed `CDSStrmResInfo *entry`.
- `CDSStrmResInfo_factory` @ `0x00433d80` — `OperatorNew(0x28)`.
- `CDSStrmResInfo_Serialize` / `Deserialize` @ `0x00433940` / `0x00433980`.

## Ghidra actions (slice 40)

- [x] `get_struct_layout CDSStreamStorage` → 96 B (`0x60`)
- [x] `get_struct_layout CDSStrmResInfo` → 40 B (`0x28`)
- [x] Renamed `FUN_00433f70` → `CDSStreamStorage_AppendOrReuseStream`
- [x] Renamed `FUN_00433f00` → `CDSStreamStorage_GetThreadLoaderNode`
- [x] Renamed `FUN_00434160` → `CDSStreamStorage_InitRootSafeStream`
- [x] Prototypes set on the three renamed methods (`CDSStreamStorage *` / `CDSStrmResInfo *` where applicable)
- [x] Decompiler comment @ `GetStreamCount` (`0x004339c0`)
- [x] `save_program bulanci.exe`

## Blockers / UNK

- `CBulanci::CDSStreamStorage_ctor` still types `this` as `CBulanci *` (same address as standalone storage ctor) — MCP cannot retype `__thiscall` ECX.
- `GetStreamCount` / `GetStreamEntry` IDSStorage stubs remain misaligned with live `m_items` path.
- Ghidra struct field cosmetics: `dwStreamFlags`, `pPad_tail` vs doc `streamFlags` / `pad_tail`; nested `CDSCollection`/`CDSChain` not applied as sub-structs (flat offsets only).
