# Round 4 — Task 09 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 9 |
| **round** | 4 |
| **title** | Fix CGameTypeDlg_OnNotify decompile widget tail indexing |
| **source** | backlog |
| **supersedes_todo_id** | 9 (R3) |
| **prior** | [round3_task_09_report.md](./round3_task_09_report.md) |
| **structs** | `CGameTypeDlg`, `CWindow`, `CGameCounter` |

## Status

**DONE** — `CGameTypeDlg` tail groups modeled as `pLabelGroup[3]` / `pNumEditGroup[3]`; `OnNotify` show path and `BuildUi` stores decompile with named arrays; `RefreshFocusOrEnable` retyped `__thiscall` with `this->pBtnOk`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Show uses `EDI+ESI*4+0x78` / `+0x84` | `0x0040ab4d`, `0x0040ab56` | Disasm: label @ `this+index*4+0x78`, numedit @ `+0x84` (not through `CWindow`) |
| Hide loop pairs label/numedit | `0x0040ab27`–`0x0040ab47` | `LEA ESI,[EDI+0x84]`; `[ESI-0xc]` + `[ESI]` ×3 → `pNumEditGroup[i]` + `pLabelGroup[i]` |
| Prior decompiler used `&win+1` math | R3 blocker | `(bVar1+2)*4` from one-past-`win` ≡ offset `0x78` — cosmetic only |
| `RefreshFocusOrEnable` targets OK btn | `0x0040aae1` | `MOV ECX,[ESI+0x90]` → `pBtnOk` |
| `BuildUi` stores triplet arrays | `0x0040d930` | Decompile: `this->pLabelGroup[0..2]`, `this->pNumEditGroup[0..2]` |
| `CGameCounter_OnEvent` unchanged | `0x0040b140` | Still `pProgressSlot0..3` + `strPackFrame`/`strPackCancelBtn` (out of R4 scope) |

### Decompile after fix (excerpt)

```c
bVar1 = *(byte *)(param_2 + 0x68);
_Globals::CDSView__Show((int *)this->pLabelGroup[bVar1]);
_Globals::CDSView__Show((int *)this->pNumEditGroup[bVar1]);
CGameTypeDlg_RefreshFocusOrEnable(this);
```

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `recreate_struct` | `CGameTypeDlg` 148 B | `pLabelGroup[3]` @0x78, `pNumEditGroup[3]` @0x84; radios/OK unchanged |
| `force_decompile` | `0x0040ab00` | `pLabelGroup[bVar1]`, `pNumEditGroup[bVar1]`; hide loop `ppCVar3 = this->pNumEditGroup` |
| `set_function_prototype` | `0x0040aad0` | `void __thiscall …(CGameTypeDlg *this)` |
| `set_function_this_type` | `0x0040ab00`, `0x0040aad0` | Class `CGameTypeDlg`; `RefreshFocusOrEnable` uses `this->pBtnOk` |
| `set_decompiler_comment` | `0x0040ab49`, `0x0040aae1` | R4 array-index + pBtnOk notes |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CGameTypeDlg.md](./CGameTypeDlg.md) — array tail fields; R4 Ghidra apply log.

## Remaining UNK

- `CGameCounter.strPackFrame` / `strPackCancelBtn` still `pointer` in Ghidra (no `CDSString` component type).
- Secondary OK `CButton` in `BuildUi` not cached on `this` (child only).
- `CRadio` notify byte at `radio+0x68` semantics (rules-group index).
