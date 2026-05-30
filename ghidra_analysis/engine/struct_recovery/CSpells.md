# CSpells

## Status

**VERIFIED** (R3 task 20) — heap/factory size `0x80` verified; extends `CDSBitmap` (`0x78`) with `spell_active_mask` (`uint` @ `+0x78`) and `bPlayerIndex` (`byte` @ `+0x7c`) typed in Ghidra. Primary gameplay instances are **embedded** in `CPanel` via `CSpells_ctor`, not standalone factory alloc.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CSpells) == 0x80` | `CSpells_Allocate@0x00427b10` | `OperatorNewWithBadAlloc(0x80)` |
| `CDSBitmap` prefix `0x78` | `CSpells_ctor@0x00427d40` | `CDSBitmap_ctor` first; `0x78 + 8 == 0x80` |
| Tail `+0x78`, `+0x7c` | `CSpells_OnEvent@0x00426da0` | `*(uint*)(this+0x78)` mask; `this[0x7c]` player filter |
| Ammo UI bytes `+0x78..+0x7a` | `CSpells_SetAmmoEmptyAndInvalidate@0x00426d70` | byte writes before vtable invalidate |

## Class registry (CDS engine)

| Claim | Address | Evidence |
|-------|---------|----------|
| `GetClassTable` → meta blob | `CSpells_GetClassTable@0x004274b0` | `return &g_CSpells_classMeta` (`0x004b3a30`) |
| **classId = 2061 (`0x80d`)** | static init xref `@0x0047c279` | Plate comment on `FUN_0047e480` after `HandleClassRegister` |
| Factory | `CSpells_Allocate@0x00427b10` | Minimal `CDSBitmap` subobject + six `CSpells` vtable writes |
| Gameplay embed | `CSpells_ctor@0x00427d40` | Called from `CPanel` ctor loop; loads bitmap `0x1004e`; stores `(byte)x` @ `+0x7c` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00–0x77 | — | `CDSBitmap` | *(base)* | `CSpells_ctor@0x00427d40` → `CDSBitmap_ctor`; [CDSBitmap.md](./CDSBitmap.md) |
| 0x68 | 0x10 | `ODSImage` | `trackImage` | `ODSImage__SetImage` on spell atlas `0x1004e` in `CSpells_ctor` |
| 0x78 | 4 | `uint` | `spell_active_mask` | `CSpells_OnEvent@0x00426da0` `\|=` / `&= ~(1<<id)`; `CSpells_Draw@0x004278c0` tests `(mask & 7)`; Ghidra field may export as `dwSpell_active_mask` |
| 0x7C | 1 | `byte` | `bPlayerIndex` | `CSpells_OnEvent` compares `(byte)param_2`; `CSpells_ctor` `bPlayerIndex=(byte)x` |

### Spell mask bits (`CSpells_Draw` + `powerups.md`)

| Bit | Effect | Source rect (atlas) |
|-----|--------|---------------------|
| 0 | Shield / armor | `(25,0)–(36,14)` |
| 1 | Hourglass | `(0,0)–(11,14)` |
| 2 | Invisibility eye | `(13,0)–(23,14)` |

Events: **`0xee`** enable bit `(param_2>>8)`; **`0xef`** disable — `CSpells_OnEvent@0x00426da0`.

## Key methods

| Method | Address | Role |
|--------|---------|------|
| `CSpells_ctor` | `0x00427d40` | Per-player HUD widget; resource `0x1004e` |
| `CSpells_OnEvent` | `0x00426da0` | Network/scheduler spell on/off |
| `CSpells_Draw` | `0x004278c0` | Blit active icons via drawable @ `+0x74`, origin `+0x30/+0x34` |
| `CSpells_SetAmmoEmptyAndInvalidate` | `0x00426d70` | Ammo-empty overlay bytes + invalidate |
| `CSpells_Allocate` | `0x00427b10` | CDS factory shell (`0x80`) |

## Parent linkage

| Container | Offset | Evidence |
|-----------|--------|----------|
| `CPanel` | `+0xbc + player*4` | Comment @ `CSpells_ctor`: inner loop slot |
| `CGaming` | indirect | `CPanel_OnEvent@0x00426ce0` routes weapon/spell updates to `CSpells` slots |

## Ghidra apply

```
get_struct_layout CSpells → 128 bytes (0x80)
  trackImage        ODSImage @ 0x68
  spell_active_mask uint     @ 0x78  (layout label may be dwSpell_active_mask)
  bPlayerIndex      byte     @ 0x7c
set_function_this_type CSpells * @ CSpells_ctor@0x00427d40, CSpells_OnEvent@0x00426da0,
  CSpells_SetAmmoEmptyAndInvalidate@0x00426d70
save_program bulanci.exe
```

Applied R3 task 20 — see [round3_task_20_report.md](./round3_task_20_report.md).

**R4 task 20:** `CSpells_SetAmmoEmptyAndInvalidate@0x00426d70` ends with **tail-call** `vf_primary+0x24` → `CDSView_InvalidateRectClipped(this, NULL, 0)` @ `0x0042ca30` (slot 9 on `CSpells` CDSView vtable) — **not** a switch jumptable @ `0x00426d97` ([round4_task_20_report.md](./round4_task_20_report.md)).

## UNK

- Dual use of `+0x78..+0x7a` as spell **dword** mask vs per-byte ammo-empty state (`CSpells_SetAmmoEmptyAndInvalidate` vs `OnEvent`) — may share storage with disjoint lifetimes.
- Drawable pointer at `+0x74` inside `CDSBitmap` view header (used by `CSpells_Draw`; not a separate tail field).
- Full `CDSBitmap` / `CDSView` field map inside `0x08..0x67`.
- Whether `InitializeByClassId(2061)` is used outside `CSpells_Allocate` registration.
