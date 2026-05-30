# Round 3 — task 14 report

## Task

| Field | Value |
|-------|-------|
| **id** | 14 |
| **title** | Verify CLevelScore 0x28 Ghidra apply; name CListBoxItem+0x04 |
| **priority** | critical |
| **source** | blocker (supersedes todo 16) |
| **types** | CLevelScore, CListBoxItem, CScoreItem, CGame |
| **addresses** | `0x00477679`, `0x00409620`, `0x00409b10`, `0x0040b640` |

## Status

**DONE**

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `sizeof(CLevelScore) == 0x28` | `CGame_GetOrCreateLevelScore@0x004142b4` | `PUSH 0x28` → `OperatorNewWithBadAlloc(0x28)`; only match in function |
| `CLevelScore` Ghidra layout | `get_struct_layout` | **40 bytes**: `pVftable_IDSReferenced`, `pVftable_IDSChained`, `m_link_next`, `m_link_prev`, `m_levelName`, `scoreChain` (CDSChain 20 B @ `+0x14`) |
| `scoreChain` dtor | `CLevelScore_dtor@0x00409330` | `CDSChain_dtor((CDSChain *)(param_1 + 5))` → `+0x14`; releases `param_1[4]` → `m_levelName` @ `+0x10` |
| Default rows on `scoreChain` | `CLevelScore_InitializeDefaultScores@0x00409620` | `OperatorNewWithBadAlloc(0x1c)` × 6; `CDSChained_AppendChild(param_1 + 0x14, …)` |
| Live score insert | `CLevelScore_AddPlayerScore@0x00409b10` | `OperatorNewWithBadAlloc(0x1c)`; append/sort/prune via `this+0x14` chain (`field_0x24` = `dwChildCount`) |
| List row size `0x14` | `CListBoxItem_ctor@0x0040b640` | Decompiler: `CListBoxItem * __thiscall`; writes `pVftable`, `dwAssocOrNext`, `dwRowFlags`, `dwLabelStringHandle` |
| `+0x04` not written | `CListBoxItem_ctor@0x0040b640`, `CListBoxItem_ctorWithAssoc@0x0040b6d0` | No store to `this+4`; plate comment documents `dwPad_04` |
| `+0x04` not read in render | `CLevelList_RenderItem@0x0040d490` (doc) | Only `(item+0xc) & 1` and label handle — aligns with `CListBoxItem.md` |
| Subclass tail @ `+0x14` | `CSessionItem_Initialize@0x0040e8b0` (doc) | `0x24` alloc = `0x14` base + `0x10` session fields |

**Note:** Todo seed `0x00477679` lies inside an older symbol range; live `CGame_GetOrCreateLevelScore` entry is **`0x004142b4`** (same `PUSH 0x28` alloc proof).

## Ghidra deltas

- Verified `/CLevelScore` **40 B** (no recreate needed; not 1-byte placeholder).
- `modify_struct_field CListBoxItem`: `dwReserved_04` → **`dwPad_04`** @ `+0x04`.
- `get_struct_layout CListBoxItem` → 20 B: `pVftable`, `dwPad_04`, `dwAssocOrNext`, `dwRowFlags`, `dwLabelStringHandle`.
- `CListBoxItem_ctor@0x0040b640` decompiler uses named fields; existing R3 plate comment retained.
- `save_program bulanci.exe`.

## Struct doc updates

- `CLevelScore.md` — Ghidra apply section (round 3 todo 14 re-verify).
- `CListBoxItem.md` — `dwPad_04` naming (already documented).

## Remaining UNK

- `CGame_GetOrCreateLevelScore@0x004142b4` still casts `pLevelScore` through `CBulanci *` for field stores (`field_0x8` …) — struct type is correct; ECX/`this` typing deferred to task 28 bundle.
- `CLevelScore_AddPlayerScore` decompiler `this` = `CBulanci *` with `field_0x14` chain view — same ECX pass.
- `dwPad_04` / `dwAssocOrNext`: no consumer read in list `RenderItem` paths (documented in `CListBoxItem.md`).
