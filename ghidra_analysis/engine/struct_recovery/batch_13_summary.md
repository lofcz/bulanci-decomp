# Struct recovery batch 13/50

**Structs:** `CMsgDialog`, `CPauseDlg`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / handler xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CMsgDialog` | PARTIAL | `0x70` | Full `CWindow`/`CDSChained` dialog header through `pDefaultFocusChild`; no tail past `CWindow` |
| `CPauseDlg` | PARTIAL | `0x7c` | Same base as `CMsgDialog` + `pGame`, `pBtnPrimary`, `pBtnSecondary` at `+0x70..+0x78` |

## Key evidence anchors

- `CMsgDialog_Allocate` @ `0x0040e780` — `OperatorNew(0x70)`; installs `CMsgDialog` vtables; clears `+0x6c`.
- `CMsgDialog_ctor` @ `0x0040be80` — `CWindow_BuildAt` then vtables `0x480844` / `0x480824` / `0x48080c` / `0x4807f8`; resizes bbox from static text child; `\|= 0xc` on `+0x46`.
- `CPauseDlg_Allocate` @ `0x0040f090` — `OperatorNew(0x7c)`.
- `CPauseDlg_Build` @ `0x00411df0` — modal dialog `306×90`; stores `CGame*` @ `+0x70`, buttons @ `+0x74` / `+0x78`; optional lobby chat widens `+0x2c`.
- `CPauseDlg_OnKeyDown` @ `0x0040ac20` — Esc → `CGame_NetSendKick_t0a(*(this+0x70),0)`.
- `CPauseDlg_OnCommand` @ `0x0040b410` — kick/resume commands via `+0x70`; posts to parent `+0x4c`.
- `CPauseDlg_OnNotify` @ `0x0040b2f0` — host-migration UI toggles `+0x74` / `+0x78` using `CGame` bytes `@+0xda/+0xdb`.
- `CWindow_BuildAt` @ `0x00405560` — documents shared dialog layout through `+0x6c` (~`0x70` bytes).

## Ghidra actions

- [x] `create_struct` / inline layout — `CMsgDialog` size `0x70`
- [x] `create_struct` / inline layout — `CPauseDlg` size `0x7c`
- [x] `get_struct_layout` verified size > 1 for both
- [x] `save_program bulanci.exe`

## Follow-ups

- Recover `CDSChained` / pad `0x40–0x4b` across all `CWindow` dialogs.
- Name `pGame` as `CGame *` once `CGame` layout is recovered.
- Distinguish heap `CPauseDlg` (`0x7c`) from stack `CPauseDlg_Build` targets embedded in `CBulanci` if a separate path exists.
