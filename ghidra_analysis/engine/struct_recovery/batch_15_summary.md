# Struct recovery — batch 15/50

**Batch index:** 15 (`batches_50.json`)  
**Structs:** `CMsgDialog`, `CPauseDlg`  
**Program:** `bulanci.exe`  
**Saved:** yes (`save_program` at end of batch)

## Results

| Struct | Status | Size | Derived fields |
|--------|--------|------|----------------|
| `CMsgDialog` | VERIFIED | `0x70` (112) | none (pure `CWindow` shell; children external) |
| `CPauseDlg` | VERIFIED / PARTIAL layout | `0x7c` (124) | `pGame@0x70`, `pBtnContinue@0x74`, `pBtnAbort@0x78` |

## Evidence highlights

- **Allocators:** `CMsgDialog_Allocate@0x0040e780` (`malloc 0x70`), `CPauseDlg_Allocate@0x0040f090` (`malloc 0x7c`).
- **Constructors:** `CMsgDialog_ctor@0x0040be80` builds modal “connecting” UI (auto-sized `CStaticText` + optional cancel `CButton`). Pause menu logic in `CPauseDlg_OnKeyDown` / `OnCommand` / `OnNotify` / `FUN_0040ac50`.
- **Inheritance:** Both are `CWindow` / `CDSView` dialogs (four vtables at `+0x0/+0x4/+0x10/+0x18`), same pattern as `CExitDlg` / `CHelpDlg` per `widgets.md` §16.
- **Gameplay:** `CMsgDialog` used from `CMenu_ShowConnectingDialog@0x004137b0`; `CPauseDlg` tied to in-game pause / kick / setup (`post_match_lobby.md`).

## Ghidra

- Replaced placeholder `CMsgDialog` / `CPauseDlg` (size 0) with sized structures; `get_struct_layout` confirms 112 and 124 bytes.
- `CWindow` component remains 105 B in DB; `pad_to_0x70` bridges to allocator-proven 112 B.

## Per-struct notes

- [CMsgDialog.md](./CMsgDialog.md)
- [CPauseDlg.md](./CPauseDlg.md)

## Next batch (16)

`CScore`, `CScoreItem` (`batches_50.json` index 16).
