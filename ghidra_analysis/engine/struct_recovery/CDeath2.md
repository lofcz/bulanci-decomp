# CDeath2

## Status

**PARTIAL** — `sizeof == 0xfc` (252 B); tournament tombstone view (`CAnim` prefix `0xf0` + 8-byte tail). Ghidra tail: `pHost@+0xf0`, `nM_placementOffsetX/Y@+0xf4/+0xf8` (same names as `CDeath` placement dwords, **different offsets** — see sibling table). R3 [round3_task_06_report.md](./round3_task_06_report.md): `+0xf4` on `CDeath2` is placement, not `CDeath` `dwM_modeFlags`. **R4:** `set_function_this_type` on `CDeath2_ctor` / `CDeath2_SubobjectCtor`; typed `CBulanek_OnDeath` tombstone path ([pass_r4_CDeath2_report.md](./pass_r4_CDeath2_report.md)). **R5:** tombstone bind band closure ([round5_worker_35_report.md](./round5_worker_35_report.md)).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDeath2) == 0xfc` | `0x0041bcb0` | `CreateObject` → `OperatorNewWithBadAlloc(0xfc)` → `CDeath2_SubobjectCtor` |
| `sizeof(CDeath2) == 0xfc` | `0x0041f900` | `CBulanek_OnDeath`: `OperatorNew(0xfc)` → `CDeath2::CDeath2_ctor` (slots `0x20..0x23`) |
| Tail ends at 0xfc | `0x00419aa0` / `0x0041a8c0` | Last tail writes `nM_placementOffsetY` @ `+0xf8`; `0xf8+4 == 0xfc` |
| Sibling corpse larger | `0x0041f900` | Same `OnDeath`: `OperatorNew(0x108)` + `CDeath::CDeath_ctor` for freestanding corpse |

## Layout (proven tail + CAnim header)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` … `+0xEF` | — | `CAnim` | *(base)* | `CDeath2_SubobjectCtor` / `CDeath2_ctor` → `CAnim_SubobjectCtor`; vtables @ `+0`, `+4`, `+0x10`, `+0x18`, `+0x88`, `+0x8c`, `+0x98` |
| `+0x20` | 4 | `int` | `nOrigin_x` | `CBulanek_BindDeathTombstoneAnim@0x0041c860` |
| `+0x24` | 4 | `int` | `nOrigin_y` | same |
| `+0x28` | 4 | `int` | `nDraw_pos_x` | same; adjusted vs `nM_placementOffsetX` |
| `+0x2c` | 4 | `int` | `nDraw_pos_y` | same |
| `+0x68` | 4 | `uint` | `dwM_netStateWord0` | `CDeath2_UpdateStateFromParams@0x00417af0` |
| `+0x6c` | 4 | `uint` | `dwM_netStateWord1` | same |
| `+0x94` | 4 | `CDSImage *` | `pOds_drawable` | tombstone bind → `GetPaletteBuffer` (R5: renamed from `pM_pPalette`) |
| `+0x98` | 4 | `pointer` | `vftable_anim_sub` | `TM_BindSequence` / `TM_SetTrack` target |
| `+0xf0` | 4 | `CBulanek *` | `pHost` | `CDeath2_ctor`: `this->pHost = victim` |
| `+0xf4` | 4 | `int` | `nM_placementOffsetX` | zeroed ctor/subobject; negated into origins in tombstone bind |
| `+0xf8` | 4 | `int` | `nM_placementOffsetY` | same |

Pads `pPad_08`, `pPad_30`, `pPad_70`, `pPad_90` — interior `CAnim`/drawable header; see [CAnim.md](./CAnim.md).

## CDeath vs CDeath2 (corpse `0x108` vs tombstone `0xfc`)

Both embed `CAnim` and install death vtables. **`+0xf4` is not interchangeable** between types (R3 xref closure).

| Region | `CDeath2` (`0xfc`) | `CDeath` (`0x108`) |
|--------|-------------------|-------------------|
| Victim ptr | `+0xf0` `pHost` | `+0xf0` `pHost` |
| `+0xf4` | `nM_placementOffsetX` (zeroed; tombstone anchor) | `dwM_modeFlags` (`deathFlags & 3`) |
| `+0xf8` | `nM_placementOffsetY` | `dwM_animHeightBias` (set in anim bind) |
| `+0xfc` | *(end)* | `bM_tourneyFlag` |
| `+0x100`/`+0x104` | — | `nM_placementOffsetX/Y` (corpse anim bind) |
| Net state copy | `+0x68`/`+0x6c` (`CDeath2_UpdateStateFromParams`) | `+0x74`/`+0x78` (`CDeath_UpdateStateFromParams`) |
| Bind helper | `CBulanek_BindDeathTombstoneAnim` | `CBulanek_ResolveAndBindAnimTrack` |

## Key methods

| Address | Symbol | Role |
|---------|--------|------|
| `0x00419aa0` | `CDeath2::CDeath2_ctor` | Tombstone; zeros placement tail; `pHost = victim` |
| `0x0041a8c0` | `CDeath2::CDeath2_SubobjectCtor` | Factory subobject; same vtables + zero `+0xf4/+0xf8` |
| `0x00417af0` | `CDeath2_UpdateStateFromParams` | Net dwords → `+0x68/+0x6c` |
| `0x0041c860` | `CBulanek_BindDeathTombstoneAnim` | Sole reader of `nM_placementOffset*`; adjusts `nOrigin_*` / `nDraw_pos_*` |
| `0x0041bcb0` | `CDeath2::CreateObject` | `OperatorNew(0xfc)` + subobject ctor |
| `0x00417b10` | `CDeath2_HideViewIfParentShown` | Inner view pointer arg (not raw `this`) |

## Spawn path (`CBulanek_OnDeath`)

```
CBulanek_OnDeath@0x0041f900
  OperatorNew(0x108) → CDeath::CDeath_ctor(victim, worldHost, deathFlags)
  CBulanek_ResolveAndBindAnimTrack(corpse) → AddEntity(pCorpseAnim)
  if bPlayerSlot in 0x20..0x23:
    OperatorNew(0xfc) → CDeath2::CDeath2_ctor(tombstone, this)
    CBulanek_BindDeathTombstoneAnim(tombstone) → AddEntity(pDeath2Tombstone @ CBulanek+0x100)
```

Tournament slots OR `0x80` into `deathFlags` before **corpse** ctor (affects `CDeath` `bM_tourneyFlag`, not `CDeath2` tail).

## Ghidra apply (R4)

```
get_struct_layout CDeath2  → 252 B (0xfc)
modify_struct_field pHost @ +0xf0 (CBulanek *)
modify_struct_field nM_placementOffsetX/Y @ +0xf4/+0xf8 (int)
set_function_this_type CDeath2 * @ 0x00419aa0, 0x0041a8c0
set_function_prototype CDeath2_ctor(CDeath2 *, CBulanek *victim)
set_function_prototype __thiscall CDeath2_SubobjectCtor(CDeath2 *)
set_function_prototype CBulanek_BindDeathTombstoneAnim(CDeath2 *tombstone)
force_decompile @ 0x00419aa0, 0x0041a8c0, 0x0041c860, 0x0041f900
save_program bulanci.exe
```

## Xref closure (`nM_placementOffset*`)

| Field | Writers | Readers (`CDeath2` `this`) |
|-------|---------|---------------------------|
| `nM_placementOffsetX` @ `+0xf4` | `CDeath2_ctor`, `CDeath2_SubobjectCtor` (=0) | `CBulanek_BindDeathTombstoneAnim` only |
| `nM_placementOffsetY` @ `+0xf8` | same | same |
| `pHost` @ `+0xf0` | `CDeath2_ctor` | tombstone bind (team color, entity type helper) |

No pre-bind writers for placement offsets (remain zero unless future xref).

## Tombstone bind bands (R5 worker 35)

Sole bind consumer: `CBulanek_BindDeathTombstoneAnim@0x0041c860` (RES `0x100da`, tournament slots `0x20..0x23`).

| Band | Offsets | Fields | Tombstone bind | Other death-path |
|------|---------|--------|----------------|------------------|
| **Coord** | `+0x20..+0x2c` | `nOrigin_x/y`, `nDraw_pos_x/y` | read/write anchor adjust | — |
| **Bounds pad** | `+0x30..+0x67` | `pPad_30` (56 B) | *(no access)* | inherited [CAnim.md](./CAnim.md) CDSView rect band |
| **Net dwords** | `+0x68/+0x6c` | `dwM_netStateWord0/1` | *(no access)* | `CDeath2_UpdateStateFromParams@0x00417af0` only |
| **Gaming / ODS** | `+0x84`, `+0x94`, `+0x98` | `pGaming_host`, `pOds_drawable`, `vftable_anim_sub` | palette + TM bind | ctor vtables |
| **Track mgr** | `+0xa8..+0xef` | `CAnim::track_manager` (72 B) | `TM_*` on `+0x98` inner | `ConstructTrackManager` in subobject ctor |
| **Tombstone tail** | `+0xf0..+0xfb` | `pHost`, `nM_placementOffsetX/Y` | read/write placement | ctors zero |

**Corpse contrast (`CDeath` @ `0x108`):** corpse bind also writes net band @ **`+0x74..+0x80`** and `bView_state_69` @ `+0x69`; tombstone bind skips that band entirely.

**Hide callback:** `CDeath2_HideViewIfParentShown@0x00417b10` — prototype `void __fastcall (CDSUpdatedItem *updatedItem)`; **`updatedItem` is not `CDeath2*`** (scheduler item inside `track_manager`; reads parent `view_flags` @ `+0x44`, calls `CDSView__Hide` on view @ `updatedItem−0x8c`). Slot 4 death vtable only hides (no respawn).

## UNK

- `+0x30..+0x67` — no `CDeath2`-specific consumer; confirmed inherited CDSView bounds band ([CAnim.md](./CAnim.md)).
- `+0x70..+0x83` — `pPad_70` (20 B); no tombstone/death-specific xref (CAnim `bGaming_slot_id` / dest-src cluster).
- `pOds_drawable` typed `CDSImage *` (was `pM_pPalette`); `GetPaletteBuffer` consumer only.
- Corpse sibling: [CDeath.md](./CDeath.md).
