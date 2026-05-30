# CScoreItem

## Status

**VERIFIED** — size `0x1c` (28 bytes). `CDSChained` / `IDSReferenced` dual vtables; name + kill/death pair at `+0x10..+0x18`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CScoreItem) == 0x1c` | `0x00409638` | `CLevelScore_InitializeDefaultScores`: `OperatorNewWithBadAlloc(0x1c)` per default row |
| Same alloc in live insert | `0x00409b2a` | `CLevelScore_AddPlayerScore`: `OperatorNewWithBadAlloc(0x1c)` before field init |
| Last field ends at `0x18` | — | `m_deaths` @ `+0x18` (4 bytes) → object ends `0x1c` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CLevelScore_AddPlayerScore@0x00409b10` → `*puVar2 = 0x480488` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | same → `puVar2[1] = 0x48046c` |
| `0x08` | 4 | `pointer` | `m_link_next` | `CLevelScore_AddPlayerScore@0x00409b10` `puVar2[2]=0`; `FUN_0042f820@0x0042f820` / `FUN_0042f780@0x0042f780` splice at `this+8` (same pattern as `CDSSafeStreamInfo`) |
| `0x0c` | 4 | `pointer` | `m_link_prev` | init `puVar2[3]=0` @ `0x00409b10`; `CDSChained_InsertListNode@0x0042f820` / `FUN_0042f780` splice at full-object `+0xc` |
| `0x10` | 4 | `CDSString` / handle | `m_name` | `CDsStringAssignFromHandle(puVar3+4)` @ `0x00409b10`; `CScoreItem::Destructor@0x004093xx` releases `param_1[4]`; `FUN_00409080@0x00409080` name compare at `param_1+0x10` |
| `0x14` | 4 | `int` | `m_kills` | `puVar3[5]=param_2` @ `0x00409b10`; `Serialize`/`Deserialize` (IDSChained `this`) write/read `this+0x10` → absolute `+0x14`; `CScoreItem_CompareByNetScore@0x00408fd0` |
| `0x18` | 4 | `int` | `m_deaths` | `puVar3[6]=param_3` @ `0x00409b10`; serialize pair; compare uses `+0x18`; `FUN_00409080` equality on `+0x14`/`+0x18` |

**Serialize wire schema (agent todo 18 r3, disasm-verified):** `Serialize` / `Deserialize` (`0x00408f50` / `0x00408f90`) use **`ECX = CScoreItem*`** — `LEA [ECX+0xc]` WString, then two 4-byte stream ops at **`[ECX+0x10]`** and **`[ECX+0x14]`** only (`00408f68` / `00408f7a`; no `+0x18` in either function). Absolute **`m_deaths` @ +0x18 is omitted** from persistence; set at runtime via `CLevelScore_AddPlayerScore@0x00409b6a` (`MOV [row+0x18], deaths`). Still used for ranking (`CScoreItem_CompareByNetScore@0x00408fd0`) and SP highlight (`CScoreItem_MatchesKillsDeathsAndName@0x00409080`). Decompiler may label `&this->m_link_prev` / `m_name` / `m_kills` when `this` typing is wrong — trust disasm offsets above.

## Function map (leaf)

| Symbol | Address | Role |
|--------|---------|------|
| `CScoreItem::Deserialize` | `0x00408f50` | WString @ `+0xc`; read dwords @ `+0x10`, `+0x14` only (`m_deaths` omitted) |
| `CScoreItem::Serialize` | `0x00408f90` | WString @ `+0xc`; write dwords @ `+0x10`, `+0x14` only (`m_deaths` omitted) |
| `CScoreItem_CompareByNetScore` | `0x00408fd0` | Sort key: `(kills - deaths)` per row |
| `CScoreItem_MatchesKillsDeathsAndName` | `0x00409080` | `int __thiscall (CScoreItem *pHighlightTemplate, CScoreItem *pRow)` — K/D/name equality for SP high-score highlight (`CScore_ctor@0x004119d5`) |
| `CScoreItem_dtor` | `0x004093c0` | Release `m_name` @ `+0x10` (`CDsStringReleaseHeader`); restore IDSReferenced vtable |
| `CScoreItem_ScalarDeletingDtor` | `0x00409a90` | `CScoreItem_dtor`; `_free(this)` when `param_1 & 1` |
| `CScoreItem_ScalarDeletingDtor_thunk_n0x4` | `0x004093b0` | MI slot+3 @ `0x48046c`: `SUB ECX,4` → `JMP 0x00409a90` |
| `CLevelScore_InitializeDefaultScores` | `0x00409620` | Seed six default `0x1c` rows |
| `CLevelScore_AddPlayerScore` | `0x00409b10` | Alloc/init/append one row; sort via `CDSChain_SortChildrenWithComparator`; cap at 6 via `CDSChain_GetChildAtIndex` + `CDSChain_RemoveListNode` |

## Ghidra apply

`create_struct CScoreItem` (batch 17). **Agent todo 17 r2 (2026-05-30):** reconciled `m_link_prev` vs stream — prototypes `CScoreItem_{Serialize,Deserialize}(CDSChained *this, …)`; plate/decompiler comments @ `0x00408f50`/`0x00408f90`; `CDSChained_InsertListNode@0x0042f820`; `save_program bulanci.exe`.

**Agent todo 18 r3 (2026-05-30):** documented `m_deaths` omit from wire — plate/decompiler comments @ `0x00408f50`/`0x00408f90`/`0x00408fd0`/`0x00411010`; `search_instructions` confirms no `+0x18` in Serialize/Deserialize; `save_program bulanci.exe`.

**R5 worker 18 (2026-05-30):** renamed generic `Destructor` / `ScalarDeletingDestructor` / `DeletingDestructorThunk_*` on score/poem paths — `CScoreItem_dtor`, `CScoreItem_ScalarDeletingDtor`, `CScoreItem_ScalarDeletingDtor_thunk_n0x4` (+ sibling types in worker report); `save_program bulanci.exe`.

**Agent todo 18 r4 (2026-05-30):** retyped highlight helper — `CScoreItem::CScoreItem_MatchesKillsDeathsAndName` (was `CBulanci*` `this`); `set_function_prototype` + `set_function_this_type`; disasm @ `0x004119d5` proves `ECX` = template row, stack = chain row; decompiler uses `m_kills`/`m_deaths`/`m_name`; `save_program bulanci.exe`.

Post-apply `get_struct_layout`:

- **CScoreItem** — Size: **28** (`0x1c`); fields `pVftable_*`, `m_link_*`, `m_name`, `m_kills`, `m_deaths`
- Rename: `FUN_00409080` → `CScoreItem_MatchesKillsDeathsAndName`

## UNK

- ~~`m_link_prev` vs stream I/O at `+0xc`~~ — **resolved** (agent todo 17 r2): list fields list-only; stream uses `+0xc`/`+0x10`/`+0x14` as wired above (not `m_link_prev` when `ECX` is full row pointer).
- ~~`m_deaths` on wire~~ — **resolved** (agent todo 18 r3): **not** in `Serialize`/`Deserialize`; runtime + compare/highlight only.
- Whether `m_kills` / `m_deaths` are always K/D vs generic score columns (behavior matches net ranking: compare uses `kills - deaths`).
- ~~`CScoreItem_MatchesKillsDeathsAndName` wrong `this` (`CBulanci*`)~~ — **resolved** (agent todo 18 r4): `__thiscall` with `CScoreItem *` template in `ECX`, `pRow` on stack; see `round4_task_18_report.md`.
