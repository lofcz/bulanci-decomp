# Round 3 — Task 18 Report

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 18 |
| **title** | Document CScoreItem m_deaths omit from Serialize schema |
| **types** | `CScoreItem`, `CScore`, `CLevelScore` |
| **source** | handoff (supersedes R2 todo 17) |
| **acceptance** | Prove or document in `CScoreItem.md` with func@addr evidence; Ghidra comments if supported |

## 2. Status

**DONE** — `m_deaths` at absolute `+0x18` is a live struct field but **not** part of the `CScoreItem` IDSChained stream schema.

## 3. Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Wire: WString then two dwords | `CScoreItem::Deserialize@0x00408f50` | Disasm: `LEA ECX,[EDI+0xc]` → `CDsString_ReadWStringFromStream`; stream read @ `[EDI+0x10]` (`00408f68`); read @ `[EDI+0x14]` (`00408f7a`) |
| Wire: symmetric write path | `CScoreItem::Serialize@0x00408f90` | `CDsString_WriteWStringToStream@[+0xc]`; stream write @ `+0x10`, `+0x14` only |
| **No `+0x18` on wire** | `0x00408f50`, `0x00408f90` | `search_instructions` operand `0x18` → **0 matches** in both functions (24 insns each) |
| Runtime deaths init | `CLevelScore_AddPlayerScore@0x00409b10` | `MOV dword ptr [ESI+0x18], ECX` @ `0x00409b6a` after `puVar3[5]=kills`, `CDsStringAssignFromHandle(puVar3+4)` |
| Sort uses deaths | `CScoreItem_CompareByNetScore@0x00408fd0` | `(kills@+0x14 - deaths@+0x18)` per row pointer |
| UI highlight uses deaths | `CScoreItem_MatchesKillsDeathsAndName@0x00409080` | Compares `param_2+0x14` / `+0x18` with template row; xref from `CScore_ctor@0x00411010` SP high-score loop |
| Post-match ctor note | `CScore_ctor@0x00411010` | Decompiler comment: rows get `m_deaths` at runtime; `Serialize` omits `+0x18` |

### Wire schema (`ECX` = full `CScoreItem*`)

| Order | Abs offset | Persisted | Ghidra decomp label (misleading) |
|-------|------------|-----------|----------------------------------|
| 1 | `+0xc` | WString | `&this->m_link_prev` |
| 2 | `+0x10` | 4-byte dword | `&this->m_name` |
| 3 | `+0x14` | 4-byte dword | `&this->m_kills` |
| — | `+0x18` | **omitted** | `m_deaths` (runtime / compare / highlight only) |

**IDSChained vtable note:** slot 4 @ `0x00408f50` = **Deserialize** (read path); slot 5 @ `0x00408f90` = **Serialize** (write path) — names in export/decompiler match `master_vtable_catalog.csv`.

Disassembly (`Deserialize@0x00408f50`):

```text
00408f59  LEA ECX,[EDI+0xc]     ; WString
00408f68  LEA ECX,[EDI+0x10]    ; dword #1
00408f77  ADD EDI,0x14          ; dword #2 — no reference to +0x18
```

## 4. Ghidra deltas

- Plate/decompiler comments @ `0x00408f50`, `0x00408f90`, `0x00408fd0`, `0x00411010` (wire schema + runtime `m_deaths`).
- `search_instructions` confirms no `+0x18` in Serialize/Deserialize.
- `save_program bulanci.exe` (batch 17 follow-up / r3 worker 18).

## 5. Struct doc updates

- [CScoreItem.md](./CScoreItem.md) — **Serialize wire schema** paragraph; UNK `m_deaths` on wire marked resolved.
- [batch_17_followup_summary.md](./batch_17_followup_summary.md) — cross-ref.

## 6. Remaining UNK

- Whether `+0x10`/`+0x14` dwords are strictly K/D vs generic score columns (behavior matches net ranking `kills - deaths`).
- `CScoreItem_MatchesKillsDeathsAndName` still typed with wrong `this` (`CBulanci*` vs highlight template row) — separate prototype task.
