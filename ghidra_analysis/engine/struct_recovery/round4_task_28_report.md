# Round 4 — task 28 report

## Task

| Field | Value |
|-------|-------|
| **id** | 28 |
| **title** | CDSDirectSound InitPrimary app-shell helper; InsertKeyed FindKeyIndex this cast |
| **priority** | medium |
| **manifest** | `agent_todos_50_r4.json` id 28 |
| **supersedes** | R3 todo 28 ([round3_task_28_report.md](./round3_task_28_report.md)) |
| **types** | `CDSDirectSound`, `CDSCollection`, `CDSApp`, `CDSStreamStorage` |
| **addresses** | `0x0043cbc0`, `0x004312c0`, `0x0042a2fa`, `0x00433d09` |

## Status

**DONE** — `FindKeyIndex` renamed and call sites show `CDSCollection *`; `InsertKeyed` callee no longer `CDSStreamStorage_FindKeyIndex`. InitPrimary `CDSApp_CreateSoundBuffer((CDSApp *)this, …)` documented as layout-correct embed cast (unchanged in decompiler).

## Evidence

### R3 carry-over (verified still applied)

| Function | Address | ECX / `this` |
|----------|---------|----------------|
| `CDSCollection_InsertKeyed` | `0x004312c0` | `CDSCollection *` |
| `CDSDirectSound_InitPrimary` | `0x0043cbc0` | `CDSDirectSound *` |
| `CListBoxItem_ctor` / `ctorWithAssoc` | `0x0040b640` / `0x0040b6d0` | `CListBoxItem *` (R3 bundle; unchanged) |

### R4: FindKeyIndex callee naming + typed body

| Item | Before | After |
|------|--------|-------|
| Symbol @ `0x00431170` | `CDSStreamStorage_FindKeyIndex` | `CDSCollection_FindKeyIndex` |
| Class header | `CDSCollection::CDSStreamStorage_FindKeyIndex` | `CDSCollection::CDSCollection_FindKeyIndex` |
| `InsertKeyed` call @ `0x004312dc` | `CDSStreamStorage_FindKeyIndex(this, …)` | `CDSCollection_FindKeyIndex(this, …)` |
| `CloseStreamByKey` @ `0x00433d09` | cast / wrong static type (R3 residual) | `CDSCollection_FindKeyIndex(&this->collection, …)` |

`get_function_variables` @ `0x00431170`: `this` = `CDSCollection *`, `ECX:4 (auto)`; body uses `this->pM_items`, `this->nM_count`.

### R4: InitPrimary app-shell helper

| Site | Evidence |
|------|----------|
| `CDSDirectSound_InitPrimary@0x0043cbc0` | Decompile: `CDSApp::CDSApp_CreateSoundBuffer((CDSApp *)this, …)` — `CDSDirectSound` embed @ `CDSApp+0x200`; cast recovers parent `CDSApp*` for shell helper |
| Comment @ `0x0043cc4a` | Documents intentional `(CDSApp *)this` (not mis-typed ECX on InitPrimary) |

Call-site asm unchanged: `CDSApp_OnCreate` `LEA ECX,[ESI+0x200]` @ `0x0042a2fa` → InitPrimary.

### Bonus (related R3/R4 handoff, same session)

| Function | Address | Action |
|----------|---------|--------|
| `CLevelScore_InitializeDefaultScores` | `0x00409620` | `set_function_this_type(CLevelScore *)` — was `__fastcall` unassigned ECX |
| `CLevelScore_dtor` | `0x00409330` | `set_function_this_type(CLevelScore *)` — decompile `CDSChain_dtor(&this->scoreChain)` |

## Ghidra deltas

- `rename_function_by_address` `CDSCollection_FindKeyIndex` @ `0x00431170`
- `set_decompiler_comment` @ `0x0043cc4a` (CreateSoundBuffer embed cast)
- `set_function_this_type` `CLevelScore *` @ `0x00409620`, `0x00409330`
- `force_decompile` @ `0x004312c0`, `0x00431170`, `0x00433cb0`, `0x00409620`, `0x00409330`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSCollection.md](./CDSCollection.md) — R4: `FindKeyIndex` rename + caller decompile
- [CDSDirectSound.md](./CDSDirectSound.md) — R4: InitPrimary CreateSoundBuffer cast note
- [CLevelScore.md](./CLevelScore.md) — R4: ECX on `InitializeDefaultScores` / `dtor`

## Remaining UNK

- `CDSDirectSound_InitPrimary`: decompiler will keep `(CDSApp *)this` for `CDSApp_CreateSoundBuffer` until a shared embed-base prototype exists (cosmetic; offsets on `CDSDirectSound *` are correct).
- `CLevelScore_AddPlayerScore`: chain sort helpers still cast `&this->scoreChain` through `CBulanci *` (R4 todo 14 scope).
