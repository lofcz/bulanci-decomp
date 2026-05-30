# CScore

## Status

**PARTIAL** — size `0x74` (116 bytes) verified; dialog prefix through `+0x6b` field-split in Ghidra (agent todo 17, mirrors `CExitDlg`); four MI vtables; tail `pDefaultFocusChild` @ `+0x6c`, `pEndMatchAudio` @ `+0x70`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CScore) == 0x74` | `0x0040efd0` | `CScore::Create` → `OperatorNewWithBadAlloc(0x74)` |
| Vtable writes through `+0x18` | `0x00411010` | `CScoreCtor`: stores vtables at `this+0`, `+4`, `+0x10`, `+0x18` after `CWindow_BuildAt` |
| Audio field at `+0x70` | `0x00411010` / `0x0040e120` | `CScoreCtor` stores sample @ `this+0x70`; `CScore::Destructor` stops/releases `param_1[0x1c]` (= `+0x70`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_primary` | `CScoreCtor@0x00411010`, `CScore::Create@0x0040efd0` → `g_pCScore_vftable_primary` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | same → `g_pCScore_vftable_IDSChained` |
| `0x08` | 4 | `dword` | `dwField_08` | `CDSChained_ctor` via `CWindow_BuildAt@0x00405560` |
| `0x0c` | 4 | `dword` | `dwTimelineFlags` | same (`CDSChained` chain head) |
| `0x10` | 4 | `void *` | `pVftable_IDSReferenced` | `CScore_ctor` / `Create` / `Destructor` restore `+0x10` vtable |
| `0x14` | 2 | `ushort` | `wViewFlags` | `CWindow_BuildAt` `\|= 0x77f` |
| `0x16` | 2 | `ushort` | `wPad_16` | — |
| `0x18` | 4 | `void *` | `pVftable_IDSEventHandler` | `CScore_ctor` / `Create` / `Destructor` |
| `0x1c` | 4 | `dword` | `dwField_1c` | `CDSChained_ctor` |
| `0x20` .. `0x2c` | 16 | `int`×4 | `nBbox_*` | `CScore_ctor@0x00411010` — `CWindow_BuildAt(this,0,0,800,600,0)` |
| `0x30` .. `0x3c` | 16 | `dword`×4 | `dwField_30` .. `dwField_3c` | `CDSChained_ctor` |
| `0x46` | 2 | `ushort` | `wKbFocusFlag` | `CWindow_BuildAt` `\|= 1` on container |
| `0x4c` | 4 | `void *` | `pParent` | shared dialog pattern (`CExitDlg.md`) |
| `0x54` .. `0x64` | — | `void *` / `dword` | `pVftable_CDSChain_*`, `dwField_5c/60/64` | `CDSChained_ctor` |
| `0x68` | 1 | `byte` | `bModalFlag` | `CWindow_BuildAt` (`0` in score ctor) |
| `0x69` .. `0x6b` | 3 | — | `pPad_69` | alignment |
| `0x6c` | 4 | `void *` | `pDefaultFocusChild` | `CWindow_BuildAt@0x00405560` `this+0x6c=0`; `CScore::Create@0x0040efd0` `puVar1[0x1b]=0` (same index as `CExitDlg`) |
| `0x70` | 4 | `void *` | `pEndMatchAudio` | `CScoreCtor@0x00411010` `pResourcePool` / `param_1[0x1c]`; `CScore::Destructor@0x0040e120` `CDSAudioPlayer_Stop` + release |

## Function map (leaf → modal)

| Symbol | Address | Role |
|--------|---------|------|
| `CScore::Create` | `0x0040efd0` | Heap factory: `OperatorNew(0x74)`, MI vtables, `pEndMatchAudio=0` |
| `CScore_ctor` | `0x00411010` | Stack modal ctor: `CWindow_BuildAt(800×600)`, rank rows, SP high-score branch, trophy `CAnim`, audio @ `+0x70` |
| `CScore_dtor` | `0x0040e120` | Stop/release `pEndMatchAudio`, `CWindow_dtor` |
| `CScore_ScalarDeletingDtor` | `0x0040f070` | `CScore_dtor`; `_free(this)` when `param_1 & 1` |
| `CScore_ScalarDeletingDtor_thunk_n0x18` | `0x0040bde0` | MI @ `0x481000` slot+3: `SUB ECX,0x18` → `JMP 0x0040f070` |
| `CScore_ScalarDeletingDtor_thunk_n0x10` | `0x0040be00` | MI @ `0x481014` slot+3: `SUB ECX,0x10` → `JMP 0x0040f070` |
| `CScore_ScalarDeletingDtor_thunk_n0x4` | `0x0040bdf0` | MI @ `0x48102c` slot+3: `SUB ECX,4` → `JMP 0x0040f070` |
| `CScore::OnKeyPress` | `0x0040abe0` | Esc/Enter → `CDSView__EndModal(0x8003)` |
| `CScore_PlayerRankComparator` | `0x0040abd0` | `qsort` comparator for live match ranking |
| `CScore_RenderHighScoreRow` | `0x00409bc0` | SP branch: five `CScore_RenderHighScoreColumn` columns per `CScoreItem` row |
| `CScoreItem_MatchesKillsDeathsAndName` | `0x00409080` | `CScoreItem` helper: `ECX`=highlight template, `pRow`=chain row; call @ `CScore_ctor+0x9c5` (`0x004119d5`) |

Parent: `CBulanci_ShowPostMatchScoreModal` / `CGame` end-of-level → stack `CScore_ctor` + `CDSView_DoModal`.

## Ghidra apply

`delete_data_type CScore` → `create_struct` (batch 17 slice). **Agent todo 17 (2026-05-30):** recreated `CScore` — replaced `pBase_CWindow_MI` / `base_to_6c` blob with `CExitDlg`-aligned dialog prefix through `+0x6b`. Post-apply `get_struct_layout`:

- **CScore** — Size: **116** (`0x74`); **31 named fields** (`nBbox_*`, `wViewFlags`, `pParent`, `bModalFlag`, …); tail `pDefaultFocusChild` @ `+0x6c`, `pEndMatchAudio` @ `+0x70`
- Renames: `CScoreCtor` → `CScore_ctor`; `FUN_00409bc0` → `CScore_RenderHighScoreRow`
- `save_program bulanci.exe`

**R5 worker 45 (2026-05-30):** `modify_struct_field` `pEndMatchAudio` → **`CDSAudioPlayer *`** @ `+0x70`; `set_function_this_type` `CScore_ctor@0x00411010`, `CScore::Destructor@0x0040e120` (`__thiscall`); decompile uses `this->pEndMatchAudio`, `CDSAudioPlayer_Play` / `CDSAudioPlayer_Stop`. Evidence: `TriggerBankSample` return cast + `param_1[0x1c]` in `Create@0x0040efd0`. `save_program bulanci.exe`.

## UNK

- `CDSChained` interior dwords (`dwField_08`, `dwTimelineFlags`, `dwField_30`..`dwField_64`) — shared ctor pattern only; no `CScore`-specific consumers beyond `CWindow_BuildAt`.
- Stack `CScore` instance in `CScoreCtor` is separate from heap `Create` path; modal uses ctor on stack then `CDSView_DoModal` (see `post_match_lobby.md`).
- ~~`pEndMatchAudio` type (`void *`)~~ — **closed R5 w45** (`CDSAudioPlayer *`, ctor/dtor/play/stop xrefs).
