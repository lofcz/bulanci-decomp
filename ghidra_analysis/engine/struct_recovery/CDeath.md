# CDeath

## Status

**PARTIAL** — `sizeof == 0x108`; freestanding corpse view (`CAnim` prefix `0xf0` + 24-byte tail). Ghidra **`CDeath` (264 B)** with tail fields named; R3 agent todo **#6** xref closed on `dwM_modeFlags` / `bM_tourneyFlag` ([round3_task_06_report.md](./round3_task_06_report.md)). Tournament tombstone sibling: **`CDeath2`** (`0xfc`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDeath) == 0x108` | `0x0041f900` | `CBulanek_OnDeath`: `OperatorNewWithBadAlloc(0x108)` → `CDeath_ctor` |
| `sizeof(CDeath) == 0x108` | `0x0041a860` | `CDeath_SubobjectCtor` on freestanding block (paired with alloc sites) |
| Tail ends at 0x108 | `0x0041a860` | `CDeath_SubobjectCtor` last writes `*(this+0x100)=0`, `*(this+0x104)=0`; `0x104+4 == 0x108` |
| Sibling `CDeath2` smaller | `0x0041f900` | `CBulanek_OnDeath` allocates `CDeath2` with `0xfc` for slots `0x20..0x23` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` … `+0xEF` | — | `CAnim` | `anim` | `CDeath_SubobjectCtor` / `CDeath_ctor` call `CAnim_SubobjectCtor` first; vtable patches @ `+0`, `+4`, `+0x10`, `+0x18`, `+0x88`, `+0x8c`, `+0x98` @ `0x0041a860` / `0x00419a00` |
| `+0xf0` | 4 | `CBulanek *` | `pHost` | `CDeath_ctor@0x00419a00` `=param_1` (victim) |
| `+0xf4` | 4 | `uint` | `dwM_modeFlags` | `CDeath_ctor@0x00419a00` `=param_3 & 3`; anim sheet index in `CBulanek_ResolveAndBindAnimTrack` |
| `+0xf8` | 4 | `uint` | `dwM_animHeightBias` | `CBulanek_ResolveAndBindAnimTrack@0x0041c790` — sheet frame height `*3/4` or `3` when `bM_tourneyFlag` |
| `+0xfc` | 1 | `byte` | `bM_tourneyFlag` | `CDeath_ctor@0x00419a00` `=(param_3>>7)&1` |
| `+0x100` | 4 | `int` | `nM_placementOffsetX` | `CDeath_SubobjectCtor` zero; `CBulanek_ResolveAndBindAnimTrack` negates into `nOrigin_x`/`nDraw_pos_x` (same role as `CDeath2` `dwM_placementOffsetX` @ `+0xf4`) |
| `+0x104` | 4 | `int` | `nM_placementOffsetY` | same pattern for Y (`CDeath2` @ `+0xf8`) |

## CDeath vs CDeath2 tail (12-byte extension)

| Offset | `CDeath2` (`0xfc`) | `CDeath` (`0x108`) |
|--------|-------------------|-------------------|
| `+0xf0` | `pHost` @ `CDeath2_ctor` | `pHost` @ `CDeath_ctor` |
| `+0xf4` | `dwM_placementOffsetX` (zeroed subobject) | `m_modeFlags` (`param_3 & 3`) |
| `+0xf8` | `dwM_placementOffsetY` (zeroed; tombstone bind) | `dwM_animHeightBias` (set in anim bind) |
| `+0xfc` | *(object end)* | `bM_tourneyFlag` byte |
| `+0x100` | — | `nM_placementOffsetX` (zeroed; anim bind) |
| `+0x104` | — | `nM_placementOffsetY` (zeroed; anim bind) |

**Net state offset differs:** `CDeath` copies dwords at **`+0x74`/`+0x78`** (`CDeath_UpdateStateFromParams@0x00417a70`); `CDeath2` at **`+0x68`/`+0x6c`** (`CDeath2_UpdateStateFromParams@0x00417af0`).

## Key methods

| Address | Symbol | Role |
|---------|--------|------|
| `0x00419a00` | `CDeath_ctor` | Freestanding corpse; `deathFlags` bit7 → tourney |
| `0x0041a860` | `CDeath_SubobjectCtor` | Vtable install + zero `+0x100/+0x104` |
| `0x00417a70` | `CDeath_UpdateStateFromParams` | Copies net words → `+0x74/+0x78` (`CAnim` fields) |
| `0x00417a90` | `CDeath_OnCorpseHideRequestRespawn` | Hide view; human → respawn msg `0x200/0xf6` |
| `0x00416650` | `CDeath_UpdateLastManStandingState` | Last-player standing gate |
| `0x00417a40` | `CDeath_IDSAnim_NotifyEvents` | Was `FUN_00417a40`; clears death flag when timer elapsed |

## Spawn path

`CBulanek_OnDeath@0x0041f900`: hide player, `OperatorNew(0x108)` → `CDeath_ctor(victim, worldHost, flags)` → `CBulanek_ResolveAndBindAnimTrack` → `AddEntity`. Tournament slots `0x20..0x23` also spawn `CDeath2` (`0xfc`) @ `this+0x40`.

## Ghidra apply

```
get_struct_layout CDeath  → size 264 (0x108)
set_function_prototype CDeath_ctor@0x00419a00 (CDeath*, CBulanek* victim, void* worldHost, uint deathFlags)
set_function_prototype CDeath_SubobjectCtor@0x0041a860
set_function_prototype CDeath_UpdateStateFromParams@0x00417a70
set_function_prototype CDeath_IDSAnim_NotifyEvents@0x00417a40
modify_struct_field pHost @ +0xf0 (CBulanek*)
modify_struct_field nM_placementOffsetX/Y @ +0x100/+0x104; dwM_animHeightBias @ +0xf8
set_decompiler_comment CDeath_SubobjectCtor / CBulanek_ResolveAndBindAnimTrack
save_program bulanci.exe
```

Decompile (2026-05-30): `CDeath_SubobjectCtor` zeros `nM_placementOffsetX/Y`; `CBulanek_ResolveAndBindAnimTrack@0x0041c790` is sole non-ctor consumer (anchor adjust, mirrors `CBulanek_BindDeathTombstoneAnim` on `CDeath2`). `CDeath_ctor` plate comment documents `CDeath*` ECX (decompiler may still show `CBulanek*` — `__thiscall` API limit). `CDeath_IDSAnim_NotifyEvents` @ `0x00417a40` (was `FUN_00417a40`).

## Xref closure (R3 todo #6, 2026-05-30)

| Field | Writers | Readers (CDeath `ESI` base) |
|-------|---------|----------------------------|
| `dwM_modeFlags` @ `+0xf4` | `CDeath_ctor@0x00419a6d` (`deathFlags & 3`) | `CBulanek_ResolveAndBindAnimTrack@0x0041c649`, `@0x0041c66c` only |
| `bM_tourneyFlag` @ `+0xfc` | `CDeath_ctor@0x00419a67` (`(deathFlags>>7)&1`) | `CBulanek_ResolveAndBindAnimTrack@0x0041c641`, `@0x0041c6fe` only |
| `dwM_animHeightBias` @ `+0xf8` | *(none in ctor)* | Written in anim bind `@0x0041c70a` (tourney→3) / `@0x0041c725` (sheet height×3/4) |
| `nM_placementOffsetX/Y` @ `+0x100/+0x104` | Zeroed `CDeath_SubobjectCtor` / `CDeath_ctor` | Subtracted into `nOrigin_*` / `nDraw_pos_*` in anim bind `@0x0041c790`–`0x0041c7b0` |

Spawn: `CBulanek_OnDeath@0x0041fa21` → `OperatorNew(0x108)` → `CDeath_ctor` → `CBulanek_ResolveAndBindAnimTrack(corpse)`.

## UNK

- Writers of `nM_placementOffsetX/Y` before anim bind (remain zero unless future xref).
- Full interior `CAnim` field map — see [CAnim.md](./CAnim.md).
