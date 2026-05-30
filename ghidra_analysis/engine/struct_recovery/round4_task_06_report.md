# Round 4 — agent todo 06 report (`agent_todos_50_r4.json`)

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 6 |
| **title** | Verify R3 closure: Xref CDeath `dwM_modeFlags` and `bM_tourneyFlag` consumers |
| **source** | verify |
| **supersedes_todo_id** | 6 |
| **types** | `CDeath`, `CAnim`, `CBulanek` |
| **acceptance** | R3 xref closure still holds after `get_struct_layout` + fresh decompile on seed sites |

**Seed addresses:** `0x0041a860`, `0x00419a00`, `0x00417a70`, `0x0041fa21`, `0x0041c790`

**Prior round:** [round3_task_06_report.md](./round3_task_06_report.md)

## 2. Status

**DONE** — R3 closure verified; `CDeath` Ghidra layout **264 B** with tail @ `+0xf4`/`+0xfc`/`+0x100`/`+0x104`. Bonus fix: `CDeath_ctor` moved from `CBulanek::` (`CBulanek *this`) to `CDeath::` so decompile names `dwM_modeFlags` / `bM_tourneyFlag` writes (was `pGame` / `pCorpseAnim` aliases).

## 3. Evidence

### Struct layout (`get_struct_layout CDeath`)

| Offset | Name | R3 claim | Verified |
|--------|------|----------|----------|
| `0xf0` (240) | `pHost` | `CBulanek *` victim | present (`-BAD-` type in Ghidra listing; field offset correct) |
| `0xf4` (244) | `dwM_modeFlags` | `uint` | yes |
| `0xf8` (248) | `dwM_animHeightBias` | `uint` | yes |
| `0xfc` (252) | `bM_tourneyFlag` | `byte` | yes |
| `0x100` (256) | `nM_placementOffsetX` | `int` | yes |
| `0x104` (260) | `nM_placementOffsetY` | `int` | yes |
| total | — | `0x108` | **264 bytes** |

### Decompile verification (post-`force_decompile`)

| Address | Symbol | R3 xref role | Verified behavior |
|---------|--------|--------------|-------------------|
| `0x00419a00` | `CDeath_ctor` | sole writer `+0xf4`/`+0xfc` | `this->dwM_modeFlags = deathFlags & 3`; `this->bM_tourneyFlag = (deathFlags>>7)&1`; `this->pHost = victim` |
| `0x0041a860` | `CDeath_SubobjectCtor` | zero `+0x100/+0x104` | `nM_placementOffsetX/Y = 0` |
| `0x00417a70` | `CDeath_UpdateStateFromParams` | net copy | `dwM_netStateWord0/1` from param buffer |
| `0x0041c790` | `CBulanek_ResolveAndBindAnimTrack` | sole reader | `corpseAnim->bM_tourneyFlag` branches sit vs walk; `corpseAnim->dwM_modeFlags` indexes sheets; writes `dwM_animHeightBias`, adjusts `nM_placementOffset*` |
| `0x0041fa21` | `CBulanek_OnDeath` | spawn chain | `OperatorNew(0x108)` → `CDeath::CDeath_ctor` → `CBulanek_ResolveAndBindAnimTrack(corpseAnim, …)` |

R3 writer/reader closure unchanged: no new consumers of `+0xf4`/`+0xfc` on `CDeath`-sized objects.

## 4. Ghidra deltas

- `get_struct_layout CDeath` — 264 B; tail offsets match R3 table
- `set_function_this_type` `CDeath *` @ `0x00419a00` (verify-driven: ctor decompile must show tail field names)
- `set_function_prototype` `CDeath_ctor(CDeath *, CBulanek *victim, void *worldHost, uint deathFlags)` @ `0x00419a00`
- `set_function_prototype` + `set_function_this_type` @ `0x00416650` (`CDeath_UpdateLastManStandingState`)
- `modify_struct_field` `pHost` type attempt @ `+0xf0`
- `force_decompile` @ `0x00419a00`, `0x0041a860`, `0x00416650`
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CDeath.md](./CDeath.md) — R4 verify + ctor `this` typing notes

## 6. Remaining UNK

- `pHost` still lists as `-BAD-` in `get_struct_layout` (offset correct; `CBulanek *` type resolution in Ghidra)
- `CDeath_OnCorpseHideRequestRespawn@0x00417a90` — `__fastcall` / interface-offset `this`, not `CDeath*` base
- `CDeath_UpdateLastManStandingState` — passes `(void *)this->dwM_modeFlags` to `CGame_IsPlayerAliveForLastMan` (confirm intent vs `pHost`)
- `CDeath2` reuses `+0xf4` as placement X — separate type (R3 note; unchanged)
