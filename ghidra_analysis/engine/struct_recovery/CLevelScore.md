# CLevelScore

## Status

**VERIFIED** — size `0x28` (40 bytes). Per-level high-score container: `CDSChained` header, level name at `+0x10`, embedded `CDSChain` (`0x14` B) holding `CScoreItem` children (`0x1c` each). Heap-allocated by `CGame_GetOrCreateLevelScore`; parent list on `CGame+0x95`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CLevelScore) == 0x28` | `0x00477693` | `CGame_GetOrCreateLevelScore`: `OperatorNewWithBadAlloc(0x28)` |
| Last field ends at `0x27` | — | `CDSChain` embed @ `+0x14` (20 B) → `0x14+0x14 = 0x28` |
| `CScoreItem` children | `0x00409638` | `CLevelScore_InitializeDefaultScores`: `OperatorNewWithBadAlloc(0x1c)` × 6, `CDSChained_AppendChild(parent+0x14, …)` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CGame_GetOrCreateLevelScore@0x00477693` → `0x48045c`; `GetClassIdentifier@0x00409310` → `DAT_004b34d4` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | `CGame_GetOrCreateLevelScore@0x00477707` chained vtable |
| `0x08` | 4 | `pointer` | `m_link_next` | ctor path zeroes `+0x08..+0x0f` @ `0x00477698` |
| `0x0c` | 4 | `pointer` | `m_link_prev` | same zero-init |
| `0x10` | 4 | `CDSString` / handle | `m_levelName` | `CDsStringAssignFromHandle(&this+0x10, …)` @ `0x00477718`; `CLevelScore_dtor@0x00409368` releases `[ESI+0x10]`; `CGame_FindLevelScoreInChain@0x00477139` compares `(child+0x10)` |
| `0x14` | 20 | `CDSChain` | `scoreChain` | `CLevelScore_dtor@0x00409358` `CDSChain_dtor(this+0x14)`; ctor installs chain vtables @ `+0x14`/`+0x18` @ `0x00477709`; `Deserialize@0x00409050` virtual on `this+0x18` (IDSChained facet) |

**IDSChained API note:** `Serialize` / `Deserialize` (`0x00409690` / `0x00409050`) are invoked with **`this` = `IDSChained` subobject (`+4`)**, so decompiler shows `this+0xc` for the level name (absolute `+0x10`) and `this+0x14` for the chain facet (absolute `+0x18`).

## Key methods

| Symbol | Address | Role |
|--------|---------|------|
| `CLevelScore::Deserialize` | `0x00409050` | Read `m_levelName`; chain `Deserialize` |
| `CLevelScore::Serialize` | `0x00409690` | Write `m_levelName`; chain `Serialize` |
| `CLevelScore_dtor` | `0x00409330` | `scoreChain` dtor; release `m_levelName` |
| `CLevelScore_InitializeDefaultScores` | `0x00409620` | Seed six default `CScoreItem` rows on empty chain |
| `CLevelScore_AddPlayerScore` | `0x00409b10` | Append `CScoreItem`, sort, prune when `scoreChain.dwChildCount > 6` |
| `CGame_GetOrCreateLevelScore` | `0x00477679` | Find/create level bucket in `CGame+0x95` chain |

## Ghidra apply

Agent todo **16** (2026-05-30):

```
delete_data_type CLevelScore   # was 1 B placeholder
create_struct CLevelScore      # 40 B (0x28): MI vtables, m_link_*, m_levelName, scoreChain @ +0x14
get_struct_layout CLevelScore  # verified 6 fields, CDSChain embed @ +0x14
save_program bulanci.exe
```

Post-apply `get_struct_layout`: size **40**; `scoreChain` is nested **CDSChain** (20 B) at offset `0x14`.

Round 3 todo **14** (2026-05-30): re-verified Ghidra — `CLevelScore` **40 B** unchanged; `CGame_GetOrCreateLevelScore@0x004142b4` still `OperatorNewWithBadAlloc(0x28)`; `CListBoxItem+0x04` renamed `dwPad_04` (was `dwField_04`). `save_program bulanci.exe`.

## UNK

- `m_link_next` / `m_link_prev`: zero-init only on create path; splice xrefs not traced this slice (same pattern as `CScoreItem`).
- Exact `CGame+0x95` field name — lives in `CGame` recovery (parent chain for level-score siblings).

## Related

- Children: `CScoreItem.md` (`0x1c`, appended to `scoreChain`).
- List rows (unrelated type): `CListBoxItem.md`.
