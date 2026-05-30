# Round 4 — Task 21 report

## Task

| Field | Value |
|-------|-------|
| **id** | 21 |
| **title** | Prove CSwitch/CDSChained dwField_30..64 writers beyond ctor zero |
| **source** | handoff (`todos_gather_r4_2.json`) |
| **types** | `CSwitch`, `CDSChained`, `CTcpIpConfig`, `CDSChain` |
| **addresses** | `0x00424bc0`, `0x00424d30`, `0x0042beb0`, `0x0040c060`, `0x0042bf40`, `0x004032d0` |

## Status

**DONE** — `+0x30..+0x3c` are **screen-space blit rects** (`nScreenBbox_*`), not ctor-only padding. Sole non-ctor writer on the shared `CDSChained` shell is `CDSView__UpdateScreenCoordinates@0x0042bf40` (copies `nBbox_*`, adds parent origin). `+0x64` renamed `pOverlapEntity`: ctor zero on dialog/`CSwitch`; runtime writes only on gameplay views (`CBulanek_UpdateStateFromParams`, `CTeleportPoint_OnEvent`). `+0x5c`/`+0x60` remain ctor-zero on `CSwitch` / `CTcpIpConfig`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `nScreenBbox_*` writer | `CDSView__UpdateScreenCoordinates@0x0042bf40` | Copies `+0x20..+0x2c` → `+0x30..+0x3c`; if `pParent@+0x4c` set, adds parent `+0x30`/`+0x34` origins |
| Blit consumers read `+0x30` | `CScrollBar_Render@0x004035ad` | `local_10 = *(param_1+0x30)` … `CPoemScroller::BlitDispatch` |
| Ctor zero init | `CDSChained_ctor@0x004032d0` | `MOV [esi+0x30], edi` (edi=0) … `MOV [esi+0x64], edi` |
| No other `+0x30` stores on shell | program search | No `MOV [reg+0x30]` stores outside ctor/`UpdateScreenCoordinates`/unrelated structs |
| `pOverlapEntity` gameplay write | `CBulanek_UpdateStateFromParams@0x004178f7` | `MOV [ecx+0x64], edx` on view objects |
| `pOverlapEntity` gate read | `CTeleportPoint_OnEvent@0x0041feff` | `MOV ecx, [esi+0x64]` |
| `CSwitch` no `+0x64` writer | search + menu path | Menu `CSwitch_ctor@0x00424bc0` never stores `+0x64`; uses `trackManager` blit, not `+0x30` render band |
| `CTcpIpConfig` inherits band | `CTcpIpConfig@0x0040c060` | `CWindow` 112 B; `nScreenBbox_*` when dialog in view tree; no tail past `0x70` |
| Chain band unchanged | `CDSChained_ResetChainCounters@0x0042beb0` | Still sole writer of `wChainInit44=1` @ `+0x44` (`Button_Click` gate on `CSwitch`) |

## Ghidra deltas

- `CDSChained`, `CSwitch`, `CWindow`, `CTcpIpConfig`: `dwField_30..3c` → `nScreenBbox_left/top/right/bottom` (`int`); `dwField_64` → `pOverlapEntity` (`void *`)
- `set_decompiler_comment` @ `0x0042bf40`, `0x00403346`, `0x0040c060`
- `force_decompile` `CDSView__UpdateScreenCoordinates`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSChained.md](./CDSChained.md) — screen bbox + overlap entity semantics
- [CSwitch.md](./CSwitch.md) — shell band table + R4 apply note
- [CTcpIpConfig.md](./CTcpIpConfig.md) — inherits renamed band

## Remaining UNK

- `dwField_5c` / `dwField_60` — no non-ctor `.text` stores on `CDSChained` minimal shell (`CSwitch` / dialogs).
- `dwField_50` — only `ResetChainCounters` zero writer.
- `CTcpIpConfig` `extends CWindow` typedef vs duplicated field list (unchanged).
- Which heap `CEdit` child owns post-modal IP string (`CDSView_SaveData` walk).
