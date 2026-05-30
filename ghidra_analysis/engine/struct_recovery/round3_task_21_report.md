# Round 3 — Task 21 report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 21 |
| **title** | Name CSwitch CDSChained tail +0x30..+0x64 and CTcpIpConfig dialog chain band |
| **types** | `CSwitch`, `CTcpIpConfig`, `CDSChained`, `CDSChain` |
| **addresses** | `0x00424bc0`, `0x00424d30`, `0x0042beb0`, `0x0040c060` |
| **acceptance** | Update CSwitch.md / CTcpIpConfig.md; Ghidra `modify_struct_field`, comments, prototypes |

## 2. Status

**DONE** — `CSwitch` drawable shell `+0x30..+0x64` mirrors `CDSChained` field names; `Button_Click` decompiles `wChainInit44`, `trackManager.nCurrentTrackIdx` / `dwCurrentFrameIdx`, `pParent`, `wCmd`. `CTcpIpConfig` duplicates `CWindow` chain band `+0x40..+0x4a` (`dwChainHead_40`, `wViewStateFlags`, `wWidgetFlags`, `wChainCounter_*`, `pParent`, `dwChainField_50`) plus modal tail `bModalFlag` / `pDefaultFocusChild`. `CDSChained_ResetChainCounters` comment documents shared writer.

## 3. Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CSwitch) == 0xc8` | menu build region | `OperatorNewWithBadAlloc(200)` → `CSwitch_ctor` |
| Shell band `+0x30..+0x3c` ctor zero | `CSwitch_ctor@0x00424bc0` | Same dword band as `CDSChained` (`dwField_30`…`dwField_3c`) |
| Chain band `+0x40..+0x50` | `CDSChained_ResetChainCounters@0x0042beb0` | `dwChainRoot@+0x40`, `wChainInit44=1`, flags cleared, `pParent`/`dwField_50` zero |
| Click gate `(wChainInit44 & 5) == 1` | `Button_Click@0x00424d30` | Decompile: `btn->wChainInit44` (ushort @ `+0x44`) |
| Track indices not flat `+0xa8`/`+0xb4` | `Button_Click@0x00424d30` | `trackManager.nCurrentTrackIdx` / `dwCurrentFrameIdx` (`CDSVideoPlayer` @ `+0x7c`) |
| Press tail `bPressedState` / `wCmd` | `CSwitch_ctor@0x00424bc0` | `+0xc4` zero, `+0xc6` = cmd; `Button_Click` sets state, posts `wCmd` |
| `sizeof(CTcpIpConfig) == 0x70` | `CreateObject@0x00472464` | `OperatorNew(0x70)`; stack modal `CMenu_OpenNetworkSession` |
| Dialog chain band named | `CTcpIpConfig@0x0040c060` | Post-build `nBbox_*` shift; `wWidgetFlags \|= 8`; layout matches `CWindow` 112 B |
| No heap children in `this` | `CTcpIpConfig@0x0040c060` | Four `OperatorNew` children via `CDSView__AddChild` only |
| Shared reset helper | `CDSChained_ResetChainCounters@0x0042beb0` | Sole writer of `wChainInit44=1`; used by window + drawable widgets |

## 4. Ghidra deltas

- `CSwitch` **200 B**: `dwField_30`…`dwField_3c`, `dwChainRoot`, `wChainInit44`…`wChainFlag4a`, `pParent`, `dwField_50`, embedded `CDSChain` MI @ `+0x54`, `dwField_5c`…`64`, `trackManager` @ `+0x7c`, `bPressedState` / `wCmd` tail
- `CTcpIpConfig` **112 B**: mirrors `CWindow` through `pDefaultFocusChild` (identical layout to `get_struct_layout CWindow`)
- `set_function_prototype` `CSwitch_ctor@0x00424bc0`, `Button_Click@0x00424d30`
- `set_function_this_type` `CSwitch_ctor` → `CSwitch *`
- `set_decompiler_comment` @ `0x0042beb0`, `0x00424d30`, `0x0040c060` (plate comments on ctor / reset / dialog)
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CSwitch.md](./CSwitch.md) — shell `+0x30..+0x64` parity table, agent-21 Ghidra apply block
- [CTcpIpConfig.md](./CTcpIpConfig.md) — chain band `+0x40..+0x4a`, `CWindow` base / no extension tail
- [CDSChained.md](./CDSChained.md) — reference for shared `ResetChainCounters` band (unchanged size `0x68`)

## 6. Remaining UNK

- Semantic meaning of `dwField_30`…`dwField_3c` / `dwField_50` / `dwField_5c`…`64` beyond ctor zero (shared across `CDSChained` derivatives).
- `CSwitch_ctor` decompile may still show legacy `field_0x*` on some stores until re-decompile / sibling `ODSImage` subobject path is retyped.
- Which modal child owns the IP string after `CDSView_SaveData` (heap walk — not a `CTcpIpConfig` member offset).
- `CTcpIpConfig` vs `typedef extends CWindow` (today: duplicated `CWindow` field list for decompiler `this`).
