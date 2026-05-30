# Round 3 — Task 20 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 20 |
| **title** | Type CSetupDlg CSpells dialog tails and spell mask bytes |
| **source** | handoff (R3) |
| **types** | `CSetupDlg`, `CSpells`, `CVolume`, `CStaticText` |
| **addresses** | `0x0040e290`, `0x0040e590`, `0x00427d40`, `0x00426da0` |

## Status

**DONE**

## Problem

Manifest batch 19 (`CSetupDlg`, `CSpells`) had markdown layouts but Ghidra still used `void *` dialog tails and raw `this+0x78` in spell helpers. `CSetupDlg_SetVolumeBinding` and `CSpells_SetAmmoEmptyAndInvalidate` were global `void *this` stubs; `CSpells_ctor` used `field_0x78` / `field_0x7c` aliases.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `sizeof(CSetupDlg) == 0x78` | `CSetupDlg_CreateObject@0x0040f1e0` | `OperatorNewWithBadAlloc(0x78)` → `CSetupDlgCtor` |
| Dialog tail `pVolume` / `pVolumeLabel` | `CSetupDlgCtor@0x0040e290` | `CWindow_BuildAt` then stores `CVolume*` @ `+0x70`, `CStaticText*` @ `+0x74`; decompile `this->pVolume` / `this->pVolumeLabel` |
| Volume binding uses tail | `CSetupDlg_SetVolumeBinding@0x0040e590` | `CScrollBar_SetValue(this->pVolume, bindingIndex)`; `CSetupDlg_UpdateVolumeLabel` |
| Focus preview reads slider value | `CSetupDlg_OnVolumeFocus@0x0040e5b0` | `notifyCode==7` && `hwndFrom==pVolume`; pan index @ `pVolume->p_scrollbar_base+0xb0` |
| `sizeof(CSpells) == 0x80` | `CSpells_Allocate@0x00427b10` | `OperatorNewWithBadAlloc(0x80)` |
| Spell mask + player byte | `CSpells_ctor@0x00427d40` | `spell_active_mask=0`; `bPlayerIndex=(byte)x` |
| Mask enable/disable events | `CSpells_OnEvent@0x00426da0` | `0xee`: `\|=` bit `(param_2>>8)`; `0xef`: `&= ~`; gated by `(byte)param_2==bPlayerIndex` |
| Draw tests low 3 bits | `CSpells_Draw@0x004278c0` | `(spell_active_mask & 7)` loop; blit via `trackImage.pDrawable` + view origin `pViewHeader_28+8/+0xc` |
| Ammo overlay shares mask storage | `CSpells_SetAmmoEmptyAndInvalidate@0x00426d70` | Writes bytes at `&spell_active_mask` .. `+2` (`+0x78..+0x7a`); then invalidate vfn+0x24 |
| Ghidra struct sizes | layout | `CSetupDlg` **120 B** (`pVolume@0x70`, `pVolumeLabel@0x74`); `CSpells` **128 B** (`uint@0x78`, `bPlayerIndex@0x7c`) |

### Spell mask bits (gameplay)

| Bit | Icon | Atlas src (from `g_spellIconSrcRects`) |
|-----|------|----------------------------------------|
| 0 | Shield | `(25,0)–(36,14)` |
| 1 | Hourglass | `(0,0)–(11,14)` |
| 2 | Invisibility | `(13,0)–(23,14)` |

Events **`0xee`** / **`0xef`** carry spell id in `param_2>>8`, player filter in low byte of `param_2`.

## Ghidra deltas

- Verified `CSetupDlg` **120 B**: `CVolume *pVolume@0x70`, `CStaticText *pVolumeLabel@0x74` (typed tails; flat `CWindow` prefix `0x00..0x6b`).
- Verified `CSpells` **128 B**: `uint` mask @ `0x78`, `byte bPlayerIndex@0x7c`, `ODSImage trackImage@0x68`.
- `set_function_prototype` + `set_function_this_type`: `CSetupDlg_SetVolumeBinding`, `CSpells_ctor`, `CSpells_OnEvent`, `CSpells_SetAmmoEmptyAndInvalidate` — members under classes `CSetupDlg` / `CSpells`.
- Decompile after apply: `CSetupDlg_SetVolumeBinding` uses `this->pVolume`; `CSpells_SetAmmoEmptyAndInvalidate` uses `this->spell_active_mask` byte overlay (Ghidra field name may still list `dwSpell_active_mask` in layout export).
- `set_decompiler_comment` @ `0x0040e590`, `0x00426d78` (mask vs ammo byte sharing).
- `save_program bulanci.exe`

## Struct doc updates

- [CSetupDlg.md](./CSetupDlg.md) — status **VERIFIED** for tail; Ghidra apply block refreshed.
- [CSpells.md](./CSpells.md) — status **VERIFIED** for tail + mask; Ghidra member names aligned.

## Remaining UNK

- `CSetupDlg` OK/Cancel `CButton` children: heap-only via `CDSView__AddChild`, no stored pointers.
- `CVolume.p_scrollbar_base+0xb0` binding index field not named on `CScrollBar` embed.
- `spell_active_mask` Ghidra component rename to canonical `spell_active_mask` (layout tool may retain `dwSpell_active_mask` label).
- `CSpells_SetAmmoEmptyAndInvalidate` jumptable @ `0x00426d97` still blocks clean invalidate decompile tail.
- Full `CDSBitmap` / `CWindow` interior field naming inside prefixes (shared dialog/bitmap batches).
