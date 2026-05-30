# Struct recovery batch 8/50

**Index:** 8 (`batches_50.json`)  
**Structs:** `CGunMouse`, `CHelpDlg`  
**Program:** `bulanci.exe` (saved)

## Results

| Struct | Status | Size | Ghidra apply |
|--------|--------|------|--------------|
| `CGunMouse` | VERIFIED size / PARTIAL layout | `0x218` (536) | Yes — `get_struct_layout` confirms 536 B |
| `CHelpDlg` | VERIFIED size / PARTIAL layout | `0x9c` (156) | Yes — `get_struct_layout` confirms 156 B |

## Size proofs (primary)

| Struct | Evidence |
|--------|----------|
| `CGunMouse` | `CGunMouse_CreateObject@00426500` → `OperatorNewWithBadAlloc(0x218)` |
| `CHelpDlg` | `_Globals::CreateObject@004222c0` → `OperatorNewWithBadAlloc(0x9c)` |

## Notable layout (evidence-backed)

**CGunMouse** — menu cursor controller (`ODSImage::CGunMouse_ctor@00426060`):

- Four embedded `CDSImage` blobs at `+0x10`, `+0x70`, `+0xd0`, `+0x130` (crosshair arms, dot, ring).
- Cursor lag ring at `+0x208`; `CGunMouse_Draw@00424610` documents mouse/target/dot offsets `+0x190..+0x1ac`.
- Track manager at `+0x1b0`; idle track roll in `CGunMouse_OnAnimTick@00423bd0`.

**CHelpDlg** — help browser dialog (`CHelpDlgCtor@00421e40`):

- Same shell pattern as `CHistoryDlg_ctor@004231d0` (window `0xe7,0x24,0x2fa,0x239`, four nav `CIcon` at `+0x8c..+0x98`).
- Page table at `+0x70`, filters `CHelpScript` class id `0x81c`.
- `CHelpDlg_LoadHelpPage@00421c10` / `CHelpDlg_OnSyntheticPageNav@00421dd0` use `+0x78` (count), `+0x80` (index).

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CGunMouse.md`
- `ghidra_analysis/engine/struct_recovery/CHelpDlg.md`

## UNK / deferred

- `CDSImage` / track-manager sub-structs still `byte[]` placeholders until `CDSImage` recovery lands.
- `CHelpDlg` `base_CWindow_MI` (`0x00..0x6f`) not field-split (MI + `CWindow`).
