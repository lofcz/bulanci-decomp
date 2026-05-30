# CSetupDlg

## Status

**VERIFIED** (R3 task 20) — heap size `0x78` verified; extends `CWindow` (`0x70`) with `CVolume *pVolume` @ `+0x70` and `CStaticText *pVolumeLabel` @ `+0x74` in Ghidra. Dialog chrome children are heap siblings via `CDSView__AddChild`, not embedded past `+0x78`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CSetupDlg) == 0x78` | `CSetupDlg_CreateObject@0x0040f1e0` | `OperatorNewWithBadAlloc(0x78)` → `CSetupDlg_ctor` |
| Tail `+0x70`, `+0x74` written in ctor | `CSetupDlg_ctor@0x0040e290` | `param_1[1].pVftable_primary` = `CVolume*`; `param_1[1].pVftable_IDSChained` = `CStaticText*` (decompiler field alias) |
| `CWindow` base `0x70` | `CSetupDlg_ctor` | `CWindow_BuildAt` before tail stores; `0x70 + 8 == 0x78` |

## Class registry (CDS engine)

| Claim | Address | Evidence |
|-------|---------|----------|
| `GetClassTable` → meta blob | `CSetupDlg_GetClassTable@0x0040e4d0` | `return &g_CSetupDlg_classMeta` (`0x004b3690`) |
| **classId = 2058 (`0x80a`)** | `PUSH 0x80a` @ `0x0047b95a` | `HandleClassRegister` in static-init cluster before `FUN_0047e160` atexit stub |
| Factory | `CSetupDlg_CreateObject@0x0040f1e0` | `OperatorNew(0x78)` → `CSetupDlg_ctor` |
| Parent meta | dialog tier | Neighbor `CPauseDlg` **`0x808`** @ `0x0047b920`; `CVolume` **`0x80b`** @ `0x0047b999` ([CPauseDlg.md](./CPauseDlg.md), [round3_task_05_report.md](./round3_task_05_report.md)) |
| Registry storage | `g_CSetupDlg_classMeta@0x004b3690` | Ghidra label applied (was `LAB_004b3690`) |

Live path: `CDSApp_ShowSetupDialog` stack-allocates / shows setup UI (uses `CSetupDlg_SetVolumeBinding@0x0040e590` for music/SFX volume binding).

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00–0x6B | — | `CWindow` | *(base)* | `CSetupDlg_ctor@0x0040e290` → `CWindow_BuildAt(0,0,0x122,0x8c,'\x01')` |
| 0x6C | 4 | `void *` | `pDefaultFocusChild` | `CWindow` layout ([CPauseDlg.md](./CPauseDlg.md) parity) |
| 0x70 | 4 | `CVolume *` | `pVolume` | `CSetupDlg_ctor` stores `CVolume_BuildAt` child; `CSetupDlg_SetVolumeBinding@0x0040e590` `this->pVolume`; `CSetupDlg_OnVolumeFocus@0x0040e5b0` |
| 0x74 | 4 | `CStaticText *` | `pVolumeLabel` | `CSetupDlg_ctor` stores `CStaticText_BuildAtAuto`; `CSetupDlg_UpdateVolumeLabel@0x0040e4e0` `this->pVolumeLabel` |

## Key methods

| Method | Address | Role |
|--------|---------|------|
| `CSetupDlg_ctor` | `0x0040e290` | Build modal dialog; volume slider uses `g_volumeSliderBitmapIds` |
| `CSetupDlg_UpdateVolumeLabel` | `0x0040e4e0` | Format binding name into `pVolumeLabel` static text |
| `CSetupDlg_SetVolumeBinding` | `0x0040e590` | `CScrollBar_SetValue(pVolume, idx)` + label refresh |
| `CSetupDlg_OnVolumeFocus` | `0x0040e5b0` | Focus notify `7`: audio pan preview from `pVolume+0xb0` |

## Ghidra apply

```
get_struct_layout CSetupDlg → 120 bytes (0x78)
  pVolume      CVolume *     @ 0x70
  pVolumeLabel CStaticText * @ 0x74
set_function_this_type CSetupDlg * @ CSetupDlg_SetVolumeBinding@0x0040e590
save_program bulanci.exe
```

Applied R3 task 20 — see [round3_task_20_report.md](./round3_task_20_report.md).

## UNK

- Full `CWindow` / `CDSChained` field naming inside `0x00..0x6B` (shared with other dialogs).
- OK/Cancel `CButton` children are heap-only (ctor calls `CDSView__AddChild` without storing pointers in tail).
- Whether any runtime path calls `InitializeByClassId(2058)` vs direct ctor/factory.
- `CVolume.p_scrollbar_base+0xb0` scroll binding index (used by `CSetupDlg_OnVolumeFocus` pan preview).
