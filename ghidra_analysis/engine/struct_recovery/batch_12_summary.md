# Struct recovery batch 12/50

**Batch index:** 12 (`batches_50.json`)  
**Structs:** `CItemInfo`, `CLevelList`  
**Protocol:** evidence-only (`AGENT_PROTOCOL.md`)

## Results

| Struct | Status | Size | Ghidra apply |
|--------|--------|------|--------------|
| `CItemInfo` | PARTIAL | `0x10` | `create_struct` → 16 bytes verified |
| `CLevelList` | VERIFIED | `0xe4` | `create_struct` (`CListBox` base) → 228 bytes verified |

## Evidence highlights

- **CItemInfo:** `CreateObject@0x404c50` allocates `0x10`; installs `IDSEventHandler` vtable `0x47faa4`; clears `+0x08`/`+0x0c`. Parent meta `CDSObject` (`0x4b7bfc`). No gameplay field consumers found — shell/metadata class.
- **CLevelList:** `CreateObject@0x40ba30` and `CMenu` embed use `OperatorNew(0xe4)`; ctor calls `CListBox_BuildAt` with fixed rect `(30,270,230,440)` and font `0xbe`; five level-specific vtables; `RenderItem` reuses `CListBox` style fields at `+0xd0`/`+0xe0`.

## Deliverables

- `CItemInfo.md`
- `CLevelList.md`
- `save_program bulanci.exe` (end of batch)

## UNK carried forward

- `CItemInfo+0x04` and meaning of `+0x08`/`+0x0c` without list-row semantics.
- `CListBox` / `CListViewer` interior field naming inside `CLevelList.base`.
