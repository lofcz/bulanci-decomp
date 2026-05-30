# Round 5 — worker 35 report (`CDeath` / `CDeath2` tombstone bands)

## 1. Task

| Field | Value |
|-------|--------|
| **worker** | 35 / 50 |
| **title** | Map and close tombstone bind offset bands on `CDeath` (corpse) vs `CDeath2` (tournament tombstone) |
| **types** | `CDeath`, `CDeath2`, `CAnim`, `CBulanek` |
| **mode** | WRITE — Ghidra MCP evidence-only |

**Seed addresses:** `0x0041c790`, `0x0041c860`, `0x00417a70`, `0x00417af0`, `0x00417b10`, `0x0041f900`

**Inputs:** [CDeath.md](./CDeath.md), [CDeath2.md](./CDeath2.md), [pass_r4_CDeath2_report.md](./pass_r4_CDeath2_report.md), [round4_task_06_report.md](./round4_task_06_report.md)

## 2. Status

**DONE** — tombstone/corpse bind bands instruction-mapped; net-band offset split (`CDeath` @ `+0x74..+0x80` vs `CDeath2` @ `+0x68/+0x6c`) proven; `pOds_drawable` rename applied; `CDeath2_HideViewIfParentShown` prototyped as `CDSUpdatedItem *` callback (not `CDeath2 *`).

## 3. Evidence

### Bind helpers (decompile verified 2026-05-30)

| Address | Symbol | Object | Bands touched |
|---------|--------|--------|---------------|
| `0x0041c790` | `CBulanek_ResolveAndBindAnimTrack` | `CDeath *` corpse | `+0x20..+0x2c` coords; `+0x69` view byte; `+0x74..+0x80` net/src; `+0x94` palette; `+0x98` TM; tail `+0xf0..+0x104` |
| `0x0041c860` | `CBulanek_BindDeathTombstoneAnim` | `CDeath2 *` tombstone | `+0x20..+0x2c` coords; `+0x94` palette; `+0x98` TM; tail `+0xf0..+0xf8` only |
| `0x00417a70` | `CDeath_UpdateStateFromParams` | `CDeath` | writes `dwM_netStateWord0/1` @ **`+0x74/+0x78`** |
| `0x00417af0` | `CDeath2_UpdateStateFromParams` | `CDeath2` | writes `dwM_netStateWord0/1` @ **`+0x68/+0x6c`** |
| `0x00417b10` | `CDeath2_HideViewIfParentShown` | — | `CDSUpdatedItem *` arg; parent `view_flags` @ `+0x44`; `CDSView__Hide` on view @ `arg−0x8c` |
| `0x0041f900` | `CBulanek_OnDeath` | — | `OperatorNew(0x108)` corpse + optional `OperatorNew(0xfc)` tombstone → bind chain |

### Corpse-only net reposition band (`CDeath` @ `+0x74..+0x80`)

After coord adjust @ `0x0041c790`, corpse bind:

1. Calls host vtable `+0x70` → stack rect; subtracts origins into stack temps.
2. `dwM_netStateWord0 = -nOrigin_x`; `dwM_netStateWord1 = stack_y`.
3. `nSrc_x = stack_x`; `nSrc_y = stack_y2` (fields @ `+0x7c/+0x80`).
4. `bView_state_69` (`pPad_68[1]`) = 1.

**Tombstone bind** performs steps through coord adjust + entity type only — **no** writes @ `+0x68..+0x80`.

### Placement tail offset split (R3/R4 confirmed, R5 bind evidence)

| Offset | `CDeath2` tombstone | `CDeath` corpse |
|--------|---------------------|-----------------|
| `+0xf4` | `nM_placementOffsetX` | `dwM_modeFlags` |
| `+0xf8` | `nM_placementOffsetY` | `dwM_animHeightBias` |
| `+0xfc` | *(end)* | `bM_tourneyFlag` |
| `+0x100/+0x104` | — | `nM_placementOffsetX/Y` |

Both bind helpers negate the **type-local** placement dwords into `nOrigin_*` / `nDraw_pos_*`.

### Bands with no death-path consumer

| Band | Offsets | Finding |
|------|---------|---------|
| CDSView bounds | `+0x30..+0x67` | No xref in `BindDeathTombstoneAnim` or death ctors — inherited [CAnim.md](./CAnim.md) rect header |
| `CDeath2` pad | `+0x70..+0x83` | No tombstone/death xref; generic `CAnim` gaming/dest cluster |

## 4. Ghidra deltas

- `modify_struct_field` — `pM_pPalette` → `pOds_drawable` (`CDSImage *`) on `CDeath`, `CDeath2`
- `modify_struct_field` — `CDeath` @ `+0x7c/+0x80` → `nSrc_x` / `nSrc_y` (`int`)
- `set_function_prototype` — `CDeath2_HideViewIfParentShown(CDSUpdatedItem *updatedItem)` @ `0x00417b10`
- `set_decompiler_comment` — bind band notes @ `0x0041c790`, `0x0041c860`, `0x00417af0`
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CDeath.md](./CDeath.md) — **Tombstone bind bands** section; R5 status link
- [CDeath2.md](./CDeath2.md) — **Tombstone bind bands** section; `pOds_drawable`; UNK trimmed to evidence-backed gaps

## 6. Remaining UNK

- `CDeath::pHost` Ghidra type still `-BAD-` (offset `+0xf0` correct)
- `CDeath2::track_manager` @ `+0xa8..` — Ghidra shows byte blob; logical layout matches `CAnim` 72 B embed (no separate death-path field map beyond `TM_*` on `+0x98`)
- Pre-bind writers for placement offsets (both types remain zero from ctor until future xref)
- `CDeath2_HideViewIfParentShown` negative-index decompile cosmetic (`CDSUpdatedItem` layout vs manual `param_1−0x8`/`−0x8c` from disasm)
