# Round 4 — Task 18 Report

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 18 |
| **title** | Retype CScoreItem_MatchesKillsDeathsAndName highlight this |
| **types** | `CScoreItem`, `CScore`, `CLevelScore`, `CBulanci` |
| **source** | blocker (`todos_gather_r4_1.json`; supersedes r3 todo 18) |
| **acceptance** | `CScoreItem*` prototype; decompiler shows `m_kills`/`m_deaths`/`m_name`; caller `CScore_ctor` call site typed |

## 2. Status

**DONE** — helper is a `CScoreItem` `__thiscall` with template row in `ECX` and enumerated row on stack; no longer attributed to `CBulanci*`.

## 3. Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Compare uses row fields @ +0x14/+0x18/+0x10 | `CScoreItem_MatchesKillsDeathsAndName@0x00409080` | Disasm: `[EAX+0x18]` vs `[ECX+0x18]`; `[EAX+0x14]` vs `[ECX+0x14]`; `LEA ECX,[EAX+0x10]` / `ADD ECX,0x10` + `CALL 0x0042d7a0` (wide-string compare on `m_name`) |
| `ECX` = highlight template, stack = current row | `0x004119d5` in `CScore_ctor@0x00411010` | `MOV ECX,[ESP+0x60]`; `PUSH ESI`; `CALL 0x00409080` — `ESI` is chain walk cursor (`CDSChained_GetFirstChildView` / `GetNextSiblingView`) |
| Not a `CBulanci` method | `0x00409080` | No `CBulanci` field access; only `CScoreItem` offsets; `set_function_this_type` moved symbol into class `CScoreItem` |
| Returns 1 on full K/D/name match | `0x00409080` | `MOV EAX,1` @ `0x004090a4`; caller `TEST AL,AL` @ `0x004119db` |
| r3 wire-schema unchanged | `0x00408f50` / `0x00408f90` | Per [round3_task_18_report.md](./round3_task_18_report.md); `m_deaths` still omitted on wire, used here at runtime |

### Calling convention (disasm-verified)

```text
CScore_ctor@0x004119d5:
  MOV ECX, [ESP+0x60]    ; pHighlightTemplate (level-score row / template; NULL skips call)
  PUSH ESI               ; pRow — current CScoreItem* from high-score list walk
  CALL CScoreItem_MatchesKillsDeathsAndName
```

Intended prototype:

```c
int __thiscall CScoreItem_MatchesKillsDeathsAndName(
    CScoreItem *pHighlightTemplate,  // ECX
    CScoreItem *pRow);               // [ESP+4] at entry
```

## 4. Ghidra deltas

- `set_function_prototype@0x00409080` → `int __thiscall CScoreItem_MatchesKillsDeathsAndName(CScoreItem *, CScoreItem *)`
- `set_function_this_type` → class `CScoreItem`; decompiler uses `m_kills` / `m_deaths` / `m_name` (not `CBulanci::field_0x*`)
- Decompiler/plate comments @ `0x00409080`, `0x004119d5`
- `force_decompile` @ `0x00409080`, `0x00411010`
- `save_program bulanci.exe`

**Note:** Ghidra may still list a phantom third parameter in the decompiler signature; trust disasm (2-arg `__thiscall`) above.

## 5. Struct doc updates

- [CScoreItem.md](./CScoreItem.md) — function map + r4 Ghidra apply; UNK on wrong `this` marked resolved.
- [CScore.md](./CScore.md) — call-site note @ `0x004119d5`.

## 6. Remaining UNK

- `0x0042d7a0` still labeled `CStartGame2_CompareLevelName` in decompiler; behavior is `CDsString` handle compare on `m_name` — rename is a separate hygiene task.
- `CScore_ctor` stack slot `[ESP+0x60]` vs local names (`local_434` / reused temps) — decompiler may alias template pointer with unrelated locals in the wide ctor; disasm at `0x004119c0` is authoritative.
