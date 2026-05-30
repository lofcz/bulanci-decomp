# CLevelScore

## Status

**VERIFIED** — size `0x28` (40 bytes). Per-level high-score container: `CDSChained` header, level name at `+0x10`, embedded `CDSChain` (`0x14` B) holding `CScoreItem` children (`0x1c` each). Heap-allocated by `CGame_GetOrCreateLevelScore`; parent list on `CGame+0x95`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CLevelScore) == 0x28` | `0x004142b4` | `CGame_GetOrCreateLevelScore@0x00414280`: `PUSH 0x28` → `OperatorNewWithBadAlloc` |
| Last field ends at `0x27` | — | `CDSChain` embed @ `+0x14` (20 B) → `0x14+0x14 = 0x28` |
| `CScoreItem` children | `0x00409638` | `CLevelScore_InitializeDefaultScores`: `OperatorNewWithBadAlloc(0x1c)` × 6, `CDSChained_AppendChild(parent+0x14, …)` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CGame_GetOrCreateLevelScore@0x004142c8` → `0x48045c`; `GetClassIdentifier@0x00409310` → `DAT_004b34d4` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | `CGame_GetOrCreateLevelScore@0x004142ce` → `0x480440` |
| `0x08` | 4 | `pointer` | `m_link_next` | ctor path zeroes `+0x08..+0x0f` @ `0x004142c2`–`0x004142c5` |
| `0x0c` | 4 | `pointer` | `m_link_prev` | same zero-init |
| `0x10` | 4 | `CDSString` / handle | `m_levelName` | `CDsStringAssignFromHandle` @ `0x004142f9` (`LEA ECX,[EDI+0x10]`); `CLevelScore_dtor@0x00409368` releases `[ESI+0x10]`; `CGame_FindLevelScoreInChain@0x00413980` compares `(child+0x10)` |
| `0x14` | 20 | `CDSChain` | `scoreChain` | `CLevelScore_dtor@0x00409358` `CDSChain_dtor(this+0x14)`; ctor installs chain vtables @ `+0x14`/`+0x18` @ `0x004142d5`–`0x004142ec`; `Deserialize@0x00409050` virtual on `this+0x18` (IDSChained facet) |

**IDSChained API note:** `Serialize` / `Deserialize` (`0x00409690` / `0x00409050`) are invoked with **`this` = `IDSChained` subobject (`+4`)**, so decompiler shows `this+0xc` for the level name (absolute `+0x10`) and `this+0x14` for the chain facet (absolute `+0x18`).

## Key methods

| Symbol | Address | Role |
|--------|---------|------|
| `CLevelScore::Deserialize` | `0x00409050` | Read `m_levelName`; chain `Deserialize` |
| `CLevelScore::Serialize` | `0x00409690` | Write `m_levelName`; chain `Serialize` |
| `CLevelScore_dtor` | `0x00409330` | `scoreChain` dtor; release `m_levelName` |
| `CLevelScore_scalar_deleting_dtor` | `0x00409a40` | `CLevelScore_dtor`; `_free(this)` when `param_1 & 1` |
| `CLevelScore_ScalarDeletingDtor_thunk_n0x4` | `0x00409320` | MI slot+3 @ `0x480440`: `SUB ECX,4` → `JMP 0x00409a40` |
| `CLevelScore_InitializeDefaultScores` | `0x00409620` | Seed six default `CScoreItem` rows on empty chain |
| `CLevelScore_AddPlayerScore` | `0x00409b10` | Append `CScoreItem`, sort, prune when `scoreChain.dwChildCount > 6` |
| `CGame_GetOrCreateLevelScore` | `0x00414280` | Find/create level bucket in `CGame+0x95` chain (`PUSH 0x28` @ `0x004142b4`) |

## Ghidra apply

**R5 worker 04** (2026-05-30): layout evidence addresses updated to `CGame_GetOrCreateLevelScore@0x00414280` disasm; `CDSChain_SortChildrenWithComparator` / `CDSChain_GetChildAtIndex` renamed for score sort/prune. [round5_worker_04_report.md](./round5_worker_04_report.md).

Agent todo **16** (2026-05-30):

```
delete_data_type CLevelScore   # was 1 B placeholder
create_struct CLevelScore      # 40 B (0x28): MI vtables, m_link_*, m_levelName, scoreChain @ +0x14
get_struct_layout CLevelScore  # verified 6 fields, CDSChain embed @ +0x14
save_program bulanci.exe
```

Post-apply `get_struct_layout`: size **40**; `scoreChain` is nested **CDSChain** (20 B) at offset `0x14`.

Round 3 todo **14** (2026-05-30): re-verified Ghidra — `CLevelScore` **40 B** unchanged; `CGame_GetOrCreateLevelScore@0x004142b4` still `OperatorNewWithBadAlloc(0x28)`; `CListBoxItem+0x04` renamed `dwPad_04` (was `dwField_04`). `save_program bulanci.exe`.

Round 4 todo **28** (2026-05-30, bonus): `set_function_this_type(CLevelScore *)` @ `CLevelScore_InitializeDefaultScores@0x00409620`, `CLevelScore_dtor@0x00409330` — decompile uses `this->scoreChain`, `this->m_levelName`. Report: [round4_task_28_report.md](./round4_task_28_report.md).

Round 4 todo **14** (2026-05-30): `set_function_this_type` + `__thiscall` fix on helpers deferred from R3:

```
set_function_prototype __thiscall @ CLevelScore_dtor@0x00409330, InitializeDefaultScores@0x00409620 (void* bridge)
set_function_this_type CLevelScore * @ 0x00409330, 0x00409620, 0x00409b10
set_function_prototype + set_function_this_type CGame * @ CGame_GetOrCreateLevelScore@0x00414280
force_decompile + save_program bulanci.exe
```

Decompile: `CLevelScore::CLevelScore_dtor` → `&this->scoreChain`, `this->m_levelName`; `InitializeDefaultScores` → `CDSChained_AppendChild(&this->scoreChain, …)`; `CGame::CGame_GetOrCreateLevelScore(CGame *this)` → typed `pLevelScore->scoreChain` / `this->chain`. See [round4_task_14_report.md](./round4_task_14_report.md).

## UNK

- `m_link_next` / `m_link_prev`: zero-init only on create path; splice xrefs not traced this slice (same pattern as `CScoreItem`).
- Exact `CGame+0x95` field name — lives in `CGame` recovery (parent chain for level-score siblings).
- `CLevelScore_AddPlayerScore`: heap row locals still alias as `CDSChain *` (true type `CScoreItem *`); sort/index helpers renamed R5 worker 04 — `CDSChain_SortChildrenWithComparator` / `CDSChain_GetChildAtIndex` @ `0x0042fae0` / `0x0042f8b0`.
- `CGame_FindLevelScoreInChain` still under `CBulanci::` in decompiler (signature pass separate from R4 todo 14).

## Related

- Children: `CScoreItem.md` (`0x1c`, appended to `scoreChain`).
- List rows (unrelated type): `CListBoxItem.md`.
