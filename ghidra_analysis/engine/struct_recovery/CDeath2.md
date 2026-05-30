# CDeath2

## Status

**PARTIAL** — `sizeof == 0xfc`; extends `CAnim` (`CAnim_SubobjectCtor` in both `CDeath2_SubobjectCtor` and `CDeath2_ctor`). Tombstone placement, net state, parent pointer, and `CAnim` inner at `+0x98` are proven; `ODSImage` header gap `+0x30..+0x67` (beyond origin/draw ints) and track-manager tail remain opaque.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDeath2) == 0xfc` | `0x0041bcb0` | `CreateObject` → `OperatorNewWithBadAlloc(0xfc)` then `CDeath2_SubobjectCtor` |
| `sizeof(CDeath2) == 0xfc` | `0x0041fa21` | `CBulanek_OnDeath` tournament path → `OperatorNewWithBadAlloc(0xfc)` then `CDeath2_ctor` |
| Tail ends before 0xfc | `0x0041a8c0` / `0x00419aa0` | Subobject/ctor last tail writes `+0xf8`; `0xf8+4 == 0xfc` |
| Sibling `CDeath` is larger | `0x0041fa21` / `0x0041a860` | Corpse uses `OperatorNew(0x108)` + `CDeath_ctor`; `CDeath_SubobjectCtor` zeros `+0x100`/`+0x104` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x000 | 4 | `pointer` | `vftable_primary` | `CDeath2_SubobjectCtor@0x0041a8c0` |
| 0x004 | 4 | `pointer` | `vftable_IDSChained` | same |
| 0x010 | 4 | `pointer` | `vftable_slot10` | same |
| 0x018 | 4 | `pointer` | `vftable_slot18` | same |
| 0x020 | 4 | `int` | `origin_x` | `CBulanek_BindDeathTombstoneAnim@0x0041c860` read/write `+0x20` |
| 0x024 | 4 | `int` | `origin_y` | same `+0x24` |
| 0x028 | 4 | `int` | `draw_pos_x` | same adjusts `+0x28` vs `origin_x` / `m_placementOffsetX` |
| 0x02c | 4 | `int` | `draw_pos_y` | same `+0x2c` |
| 0x068 | 4 | `uint` | `m_netStateWord0` | `CDeath2_UpdateStateFromParams@0x00417af0` |
| 0x06c | 4 | `uint` | `m_netStateWord1` | same |
| 0x088 | 4 | `pointer` | `vftable_slot88` | `CDeath2_SubobjectCtor@0x0041a8c0` |
| 0x08c | 4 | `pointer` | `vftable_slot8c` | same |
| 0x094 | 4 | `pointer` | `m_pPalette` | `CBulanek_BindDeathTombstoneAnim` → `GetPaletteBuffer(*(this+0x94))` |
| 0x098 | 4 | `pointer` | `vftable_anim_sub` | ctor chain; `TM_BindSequence`/`TM_SetTrack(this+0x98)` in tombstone bind |
| 0x0f0 | 4 | `pointer` | `m_pParent` | `CDeath2_ctor@0x00419aa0` `*(this+0xf0)=param_1`; Ghidra `m_pParent` (slice 06) |
| 0x0f4 | 4 | `int` | `m_placementOffsetX` | ctor zero; tombstone bind negates into `origin_x` / `draw_pos_x` |
| 0x0f8 | 4 | `int` | `m_placementOffsetY` | ctor zero; tombstone bind negates into `origin_y` / `draw_pos_y` |

Inherited `CAnim` slots at `+0x9c..+0xa7` / `+0xa8` track manager — see `CAnim.md` (`CAnim_SubobjectCtor@0x00419870`).

## CDeath sibling comparison (`0x108`)

Both embed `CAnim` and install death-specific vtables at `+0x00`, `+0x04`, `+0x10`, `+0x18`, `+0x88`, `+0x8c`, `+0x98`.

| Region | `CDeath2` (`0xfc`) | `CDeath` (`0x108`) | Notes |
|--------|-------------------|-------------------|--------|
| Parent | `+0xf0` `m_pParent` | `+0xf0` (ctor `param_1`) | Same slot; both death ctors |
| Tail dwords | `+0xf4`/`+0xf8` placement offsets | `+0x100`/`+0x104` zeroed in `CDeath_SubobjectCtor` | **+12** shift matches size delta |
| Extra | — | `+0xf4` flags (`param_3 & 3`), `+0xfc` byte (`param_3>>7`) | `CDeath_ctor@0x00419aa0` only |
| Factory | `CreateObject` / tourney `OnDeath` | `OnDeath` corpse `0x108` | `CBulanek_OnDeath@0x0041fa21` |

Shared naming: use `m_pParent` at `+0xf0`; use `m_placementOffsetX/Y` on `CDeath2` tail — do **not** alias to `CDeath` `+0xf4` flag byte without separate proof on `CDeath2`.

## Ghidra apply

```
get_struct_layout CDeath2  → size 252 (0xfc)
modify_struct_field m_pParent, m_placementOffsetX, m_placementOffsetY  (slice 06)
```

`CDeath2_UpdateStateFromParams@0x00417af0` decompile: `this->dwM_netStateWord0/1` @ `+0x68`/`+0x6c`.

## UNK

- `+0x30..+0x67` between `draw_pos_y` and `m_netStateWord0` (no `CDeath2`-specific consumer; may be `ODSImage` / drawable header — see `CAnim.md` `+0x28..+0x67`).
- `+0x68` as `CAnim` view bytes vs dword net state — death path uses **dword** copy only (`UpdateStateFromParams`).
- `CDeath2_HideViewIfParentShown@0x00417b10` — `param_1` is inner view pointer (`-0x8`, `-0x8c`), not raw `this`.
- Full `track_manager` (`+0xa8..`) field map — defer to `CAnim` / `anim_runtime.md`.
- Corpse sibling layout: [CDeath.md](./CDeath.md) (`0x108`, net @ `+0x74`).
