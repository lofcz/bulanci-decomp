# Round 3 — agent todo 06 report (`agent_todos_50_r3.json`)

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 6 |
| **title** | Xref CDeath `dwM_modeFlags` and `bM_tourneyFlag` consumers |
| **priority** | high |
| **source** | blocker |
| **structs** | `CDeath`, `CAnim`, `CBulanek` |
| **acceptance** | Close xref on tail `+0xf4` / `+0xfc` with func@addr writers + readers; document spawn→bind path |

**Seed addresses:** `0x0041a860`, `0x00419a00`, `0x00417a70`, `0x0041fa21`, `0x0041c790`

## 2. Status

**DONE** — both fields have a single writer (`CDeath_ctor`) and a single reader function (`CBulanek_ResolveAndBindAnimTrack`) when `ESI` is a `CDeath*` corpse (`sizeof 0x108`). No other `CDeath`-sized object uses `+0xfc` as a byte flag.

## 3. Evidence

### Field semantics (from `CDeath_ctor` disasm @ `0x00419a00`)

| Field | Offset | Writer | Instruction |
|-------|--------|--------|-------------|
| `dwM_modeFlags` | `+0xf4` | `CDeath_ctor@0x00419a6d` | `MOV [ESI+0xf4], ECX` after `AND ECX, 3` on `deathFlags` stack arg |
| `bM_tourneyFlag` | `+0xfc` | `CDeath_ctor@0x00419a67` | `MOV [ESI+0xfc], AL` after `SHR deathFlags, 7` / `AND AL, 1` |

`deathFlags` source: `CBulanek_OnDeath@0x0041fa21` passes `param_2` (death byte); enemy slots `0x20..0x23` OR in `0x80` before ctor (`@0x0041f9xx` region).

### Readers — sole consumer `CBulanek_ResolveAndBindAnimTrack@0x0041c790`

| Offset | Addresses | Behavior |
|--------|-----------|----------|
| `+0xfc` | `@0x0041c641`, `@0x0041c6fe` | `CMP byte [ESI+0xfc], 0` — **0:** walk anim sheet scan using `dwM_modeFlags`; **non-zero:** sit facing resource from `g_dwResId_player_sit_*` / enemy variant |
| `+0xf4` | `@0x0041c649`, `@0x0041c66c` | **Sit path:** index into sit resource tables. **Walk path:** base index into `gABulanekWalkAnimSheet` with forward/back scan when `dwM_modeFlags < 2` vs `>= 2` |
| `+0xf8` | `@0x0041c70a`, `@0x0041c725` | Written here only (not ctor): `3` if tourney flag else bound track frame height×3/4 |
| `+0x100/+0x104` | `@0x0041c790`–`0x0041c7b0` | Placement subtract into `nOrigin_*` / `nDraw_pos_*` (zeroed in ctor; no pre-bind writer) |

### Spawn chain

```
CBulanek_OnDeath@0x0041fa21
  OperatorNew(0x108) → CDeath_ctor(victim, worldHost, deathFlags)
  CBulanek_ResolveAndBindAnimTrack(corpse)
  AddEntity(...)
```

Tournament tombstone uses **`CDeath2`** (`0xfc`) + `CBulanek_BindDeathTombstoneAnim` — that path uses `+0xf4` as **placement X**, not `dwM_modeFlags` (`SUB [ESI+0xf4]` @ `0x0041c93d`).

### Program-wide `+0xf4]` / `+0xfc]` filter

Many hits are **`CBulanek*`** (`pGaming_host` cluster) or unrelated types. **CDeath-only** pattern: **write** `+0xf4`/`+0xfc` only in `CDeath_ctor`; **read** `+0xf4`/`+0xfc` only in `CBulanek_ResolveAndBindAnimTrack` (plus ctor write). `CDeath2` reuses `+0xf4` for placement — different type/size.

### Ghidra struct (verified)

`CDeath` 264 B (`0x108`): `pHost@+0xf0`, `dwM_modeFlags@+0xf4`, `dwM_animHeightBias@+0xf8`, `bM_tourneyFlag@+0xfc`, `nM_placementOffsetX/Y@+0x100/+0x104`.

## 4. Ghidra deltas

- `modify_struct_field` — `bM_tourneyFlag` @ `CDeath+0xfc` (was unnamed byte)
- `set_decompiler_comment` @ `0x0041c641`, `0x0041c649`, `0x0041c66c` (anim-bind consumers)
- Existing: `CDeath_ctor` plate + tail comments; `CBulanek_OnDeath` / `CBulanek_ResolveAndBindAnimTrack` comments
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CDeath.md](./CDeath.md) — xref closure table (unchanged facts; report cross-link)

## 6. Remaining UNK

- Pre-bind writers for `nM_placementOffsetX/Y` (still zero from ctor/subobject; only anim bind reads)
- Full `CAnim` interior `+0x28..+0x67` — see [CAnim.md](./CAnim.md)
- `CDeath2` `+0x30..+0x67` interior — separate struct recovery

**Note:** Prior `round3_task_06_report.md` content described `CMsgDialog` RTTI (`round3_tasks.json` id 6). That material lives in [round3_task_05_report.md](./round3_task_05_report.md) and [round3_task_07_report.md](./round3_task_07_report.md) for pause/`0x808`; CPauseDlg cross-links updated.
