# Round 4 — Task 20 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 20 |
| **title** | Decompile CSpells ammo invalidate jumptable; CScrollBar+0xb0 |
| **source** | `agent_todos_50_r4.json` (blocker; supersedes R3 todo 20) |
| **types** | `CSpells`, `CSetupDlg`, `CVolume`, `CScrollBar` |
| **addresses** | `0x00426d70`, `0x00426d97`, `0x0040e590`, `0x0040e5b0` |
| **prior** | [round3_task_20_report.md](./round3_task_20_report.md) |

## Status

**DONE** — `0x00426d97` is a **vtable tail-call** to `CDSView_InvalidateRectClipped` (not a switch jumptable); disasm + plate/EOL comments document `(this, NULL, 0)`. `CScrollBar` **204 B** layout names `nCurrentValue` @ `+0xb0`; `CVolume` embeds `CScrollBar scrollbar` @ `+0x00`.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Ammo overlay bytes before invalidate | `CSpells_SetAmmoEmptyAndInvalidate@0x00426d70` | `MOV [ECX+0x78/0x79/0x7a]` then vfn dispatch |
| `vf_primary+0x24` slot 9 | `0x00426d7b`–`0x00426d97` | `MOV EAX,[ECX]`; `MOV EDX,[EAX+0x24]`; stack `(0,0)`; `JMP EDX` |
| Target = `CDSView_InvalidateRectClipped` | vftable | `CSpells` primary CDSView vtable @ `0x00483acc` slot **9** → `0x0042ca30` ([vftable_methods.csv](../vftable_methods.csv)) |
| Not a switch table | `0x00426d97` | Single indirect `JMP EDX`; no `switchdataD_*`; `clear_instruction_flow_override` had no prior switch fixup |
| Volume focus reads slider value | `CSetupDlg_OnVolumeFocus@0x0040e5b0` | `notifyCode==7` && `hwndFrom==pVolume`; load @ `CVolume+0xb0` (= `CScrollBar+0xb0`) |
| `+0xb0` = current value | [widgets.md](../../widgets.md) §12 | `nMin`/`nMax` @ `+0xa8`/`+0xac`; **`nCurrentValue` @ `+0xb0`** |
| Ghidra `CScrollBar` | `get_struct_layout` | **204 B**; `nCurrentValue` **int @ 176 (0xb0)** |
| Ghidra `CVolume` | `get_struct_layout` | **208 B**; `CScrollBar scrollbar` @ 0; `audio_preview` @ 204 |

### Disasm (`CSpells_SetAmmoEmptyAndInvalidate`)

```
00426d78  MOV  byte ptr [ECX+0x78], AL
00426d7b  MOV  EAX, dword ptr [ECX]
00426d7d  MOV  byte ptr [ECX+0x79], DL
00426d80  MOV  EDX, dword ptr [EAX+0x24]
00426d83  MOV  byte ptr [ECX+0x7a], 1
00426d87  MOV  dword ptr [ESP+0x8], 0
00426d8f  MOV  dword ptr [ESP+0x4], 0
00426d97  JMP  EDX
```

## Ghidra deltas

- `recreate_struct` **`CScrollBar`** 204 B — `nMinValue`/`nMaxValue`/`nCurrentValue` @ `+0xa8..+0xb0` per widgets.md
- `recreate_struct` **`CVolume`** — `CScrollBar scrollbar` @ 0, `audio_preview` @ 204
- `set_decompiler_comment` @ `0x00426d80`, `0x00426d97` (vtable invalidate tail-call)
- `set_plate_comment` @ `0x00426d70`, `0x0040e5b0`
- `set_function_this_type` `CSetupDlg *` @ `0x0040e5b0`
- `force_decompile` @ `0x00426d70`, `0x0040e5b0`
- `save_program bulanci.exe`

## Struct doc updates

- [CScrollBar.md](./CScrollBar.md) — new layout slice (R4 task 20)
- [CSetupDlg.md](./CSetupDlg.md) — `nCurrentValue` / R4 Ghidra apply
- [CSpells.md](./CSpells.md) — invalidate tail-call note; R3 jumptable UNK closed

## Remaining UNK

- Decompiler may still emit `UNRECOVERED_JUMPTABLE` / `(*UNRECOVERED_JUMPTABLE)()` at `0x00426d97` (indirect tail-call limitation); use disasm + comments above.
- `CSetupDlg_OnVolumeFocus` may show `*(int *)(this->pVolume + 0xb0)` instead of `this->pVolume->scrollbar.nCurrentValue` (pointer-offset form; offset matches struct).
- Full `CScrollBar` bitmap band `+0x80..+0xa3` and `CDSChained` prefix `+0x00..+0x67` still sparse in Ghidra.
