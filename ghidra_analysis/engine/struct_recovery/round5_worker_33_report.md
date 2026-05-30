# Round 5 — Worker 33 report (`CMenu` 228 B UNKs)

## Task

| Field | Value |
|-------|-------|
| **worker** | 33 / 50 |
| **mode** | WRITE |
| **target** | [CMenu.md](./CMenu.md) — close layout UNKs on **228 B** (`0xe4`) UI shell |
| **prior** | [pass_r4_CMenu_report.md](./pass_r4_CMenu_report.md), R4 struct apply |

## Status

**DONE** — `+0x40..+0x67` expanded to proven **`CWindow` / `CDSChained` chain band**; `+0xa8..+0xaf` and `+0xbc..+0xbf` proven **alignment padding** (no stores). Ghidra `CMenu` rebuilt (removed bogus `nScreenBbox` @ `+0x30` duplicate and monolithic `pPad_40_67`).

## Evidence

### Chain band `+0x40..+0x67` (was `pPad_40_67`)

| Offset | Name | Function @ address | Evidence |
|--------|------|-------------------|----------|
| `+0x40` | `dwChainHead_40` | `CDSChained_ResetChainCounters@0x0042beb0` | `*(param_1+0x40)=0` |
| `+0x44` | `wViewStateFlags` | same | `*(ushort*)(param_1+0x44)=1` |
| `+0x46` | `wWidgetFlags` | `CMenu_ctor_with_ui@0x00426683` | `OR word ptr [ESI+0x46],0x21` |
| `+0x48/4a` | `wChainCounter_*` | `ResetChainCounters` | cleared to `0` |
| `+0x4c` | `pParent` | same | `=0` |
| `+0x50` | `dwChainField_50` | same | `=0` |
| `+0x54/58` | CDSChain vtables | `CDSChained_ctor@0x004032d0` | `param_1[0x15/0x16]` stores |
| `+0x5c..64` | `dwField_5c..64` | `CDSChained_ctor` | zero init |

Prefix `+0x00..+0x3f` and bbox `+0x20..+0x2c` unchanged from R4; `+0x30..+0x3c` are **`dwField_30..3c`** (ctor zeros), not a second screen bbox.

### Padding `+0xa8..+0xaf`, `+0xbc..+0xbf`

| Claim | Evidence |
|-------|----------|
| No stores to `+0xa8` | `search_byte_patterns` `89 86 a8 00 00 00` → **no matches** in module |
| Gap before buttons | Last icon ptr `pIconQuitHighlight` @ `+0xa4`; `pBtnStart` @ `+0xb0` (`MOV [ESI+0xb0]` @ `0x00426744`) |
| Gap before audio | `pBtnQuit` @ `+0xb8`; `pBackgroundMusicPlayer` @ `+0xc0` (`MOV [ESI+0xc0]` @ `0x00426643`) |
| Splash auto-click uses pad as anchor | `CMenu_ctor_with_ui@0x00426c8e` — `Button_Click` via `pad_a8_af + displayDirty*4 + 8` → `pBtnStart..pBtnQuit` |

### Size

| Claim | Evidence |
|-------|----------|
| `sizeof(CMenu)==0xe4` | `get_struct_layout CMenu` → **228 bytes** after `recreate_struct` |

## Ghidra deltas

1. `recreate_struct CMenu` — 228 B; chain band `+0x40..+0x67`; `pad_a8_af` / `pad_bc_bf`; removed erroneous `nScreenBbox` block
2. `set_decompiler_comment@0x00426683` — `wWidgetFlags |= 0x21`
3. `set_decompiler_comment@0x00426c8e` — splash indexes buttons through `pad_a8_af`
4. `force_decompile@0x004265e0` — `this->wWidgetFlags` visible (was `pPad_40_67+6`)
5. `save_program bulanci.exe`

## Struct doc updates

- [CMenu.md](./CMenu.md) — full layout `+0x00..+0xe2`; UNK section trimmed to shared `CDSChained` dwords + cosmetic symbol rename

## Remaining UNK

- Per-field semantics for shared `dwField_08/0c/30..3c/5c..64` (documented under [CWindow.md](./CWindow.md), not menu-specific).
- Cosmetic: rename mis-prefixed `CMenu_OpenNetworkSession` → `CGame_*`; `CBulanci::CMenu_ctor_with_ui` display name.
