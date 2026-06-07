# Round 11 — AI Task 10 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 10 |
| **worker** | 10 / 30 |
| **title** | `CBulanek_ClampMoveRectByCollision` + `SpatialQuery` |
| **archetype** | `moving_bot` |
| **seed_address** | `0x004195f0` |
| **addresses** | `0x004195f0`, `0x00418300`, `0x00404730`, `0x0042f7c0`, `0x0042f920`, `0x00433280`, `0x00417210` |
| **acceptance** | Collision query algorithm; obstacle bitmask; campaign slot usage |

## Status

**DONE** — Live Ghidra decompile/disasm confirms axis clamp after `SpatialQuery` with movement flags `(param_4=0, param_5=0)`, solid-tile gate via `CDSRect_Overlaps`, and coop/dummy-only `0x102` bot reroll on block. Campaign slots `0x7d..0x7f` use the same movement query but **do not** post `TryBotRandomAction` on collision.

## AI archetype

**All `CBulanek` movers** (humans, coop `0x20..0x23`, practice `0x24..0x27`, campaign `0x7d..0x7f`) pass through `CBulanek_ClampMoveRectByCollision` from `CBulanek_StepMovementAndCollision@0x0041af70`. Only **coop vampires and practice dummies** (`bPlayerSlot` in `0x20..0x27`) receive an AI reroll when an axis clamp actually applies.

## Algorithm

### `SpatialQuery` (`0x00418300`)

```c
CDSView *SpatialQuery(
    CGaming *this,           // ECX — pGaming_host from mover (+0x84)
    tagRECT *pQueryRect,     // proposed world AABB
    tagRECT *pTileOverlapRect, // mover collision footprint at origin (for solid-tile test)
    tagRECT *pEntityBoundsOut, // output: first hit entity bounds
    char bSkipTileOverlapGate, // 0 = movement mode (run CDSRect_Overlaps)
    char bSkip6aEntities);     // 0 = include entities with +0x6A set
```

**Walk loop** (disasm @ `0x00418330`):

```
head = CDSChained_GetFirstChildView(*(CGaming+0x31C) + 0x54)
for (view = head; view; view = CDSChained_GetNextSiblingView(view)):
  if view[+0x69] == 0: continue                    // inactive
  view->vftable[+0x70](pEntityBoundsOut)            // CGameView_GetWorldCollisionRect
  if !rect_Intersect(pQueryRect, pEntityBoundsOut): continue
  if !bSkipTileOverlapGate:
    if CDSRect_Overlaps(pTileOverlapRect, pEntityBoundsOut): continue  // solid overlap → reject
  if bSkip6aEntities && view[+0x6A]: continue
  return view
return NULL
```

| Flag combo | Mode | Callers |
|------------|------|---------|
| `(0, 0)` | Movement — tile gate **on**, include `+0x6A` | `CBulanek_ClampMoveRectByCollision` @ `0x0041965e` |
| `(1, 0)` | Respawn — skip tile gate | `CGaming_RespawnPlayerAtSafeLocation` |
| `(1, 1)` | Projectile — skip tile gate, skip `+0x6A` | `CShot_TraceCollision`, `CShot_Ctor` |

### Obstacle / collision “bitmask” (proven semantics)

There is **no per-tile bitmask dword** in this path. Solid blocking uses:

| Mechanism | Address | Role |
|-----------|---------|------|
| `CGameView_GetWorldCollisionRect` | `0x00417210` | Vtable slot **28** (`+0x70`) on all gameplay views (`CObstacle`, `CBulanek`, `CShot`, …). Returns `nCollisionLeft..Bottom` **only when** `(wChainInit44 & 1)` @ `+0x44`; else degenerate rect. |
| `CDSRect_Overlaps` | `0x00404730` | Non-empty intersection of `pTileOverlapRect` (mover origin collision AABB) vs hit entity bounds → entity **rejected** when `param_4==0`. |
| `rect_Intersect` | `0x00433280` | Query rect must overlap entity bounds (separate from tile gate). |

`SpatialQuery` therefore returns the first entity whose **visible bounds** intersect the proposed move **and** whose **solid footprint** does not already overlap the mover's origin collision box (allows sliding along edges).

### `CBulanek_ClampMoveRectByCollision` (`0x004195f0`)

```c
void CBulanek_ClampMoveRectByCollision(
    CBulanek *this,
    int *pOrigin,              // &nOrigin_x @ +0x20 (updated at end)
    tagRECT *pProposed,        // proposed world rect (clamped in place)
    int *pCollisionAtOrigin,   // local_20 — facing-offset collision at current origin
    tagRECT *pHitBoundsOut);   // scratch; also receives SpatialQuery hit bounds
```

**Early outs** (disasm @ `0x004195fc`, `0x00419632`):

- `pGaming_host@+0x84 == NULL` → return.
- `pProposed` equals `pCollisionAtOrigin` (no delta on all four edges) → return.

**Query** @ `0x0041965e`:

```c
hit = SpatialQuery(this->pGaming_host, pProposed, pCollisionAtOrigin, pHitBoundsOut, 0, 0);
```

**Map extents** from `pChainParent@+0x4C` (playfield view — `CGaming` shell bbox layout):

```c
mapW = parent->nBbox_right  - parent->nBbox_left;   // +0x28 - +0x20  (ctor default 0x320 = 800)
mapH = parent->nBbox_bottom - parent->nBbox_top;   // +0x2C - +0x24  (ctor default 0x258 = 600)
```

**Facing axis** = `nCurrentTrackIdx@+0xD4` (`CDSVideoPlayer` field — walk track 0..3). Switch jump table @ `0x00419685` → `0x0041979c`.

| `nCurrentTrackIdx` | Direction | Clamp rule (disasm) | On `hit` snap |
|--------------------|-----------|---------------------|---------------|
| **0** | Left (−X) | `left = max(0, proposed.left)`; if `hit && proposed.left < collisionAtOrigin.left`: `left = hitBounds.right` | `blocked = true` |
| **1** | Right (+X) | `right = min(mapW, proposed.right)`; if `hit && collisionAtOrigin.left < proposed.left`: `right = hitBounds.left` | `blocked = true` |
| **2** | Up (−Y) | `top = max(0, proposed.top)`; if `hit && proposed.top < collisionAtOrigin.top`: `top = hitBounds.bottom` | `blocked = true` |
| **3** | Down (+Y) | `bottom = min(mapH, proposed.bottom)`; if `hit && collisionAtOrigin.top < proposed.top`: `bottom = hitBounds.top` | `blocked = true` |

Opposite edge shifted by the same delta to preserve width/height.

**Origin commit** @ `0x00419743`–`0x0041975c`: `pOrigin[0..3]` += `(pProposed − pProposed_saved)`.

**AI reroll** @ `0x00419763`–`0x0041978b`:

```c
slot = bPlayerSlot@+0x70;
if (((0x20 <= slot && slot < 0x24) || (0x24 <= slot && slot < 0x28)) && blocked)
  CDSView_PostMessage(&this->vftable_IDSEventHandler, 0x200, 0x102, 0, 0);
// → CBulanek_OnEvent case 0x102 → CBulanek_TryBotRandomAction
```

| Slot band | Post `0x102` on block? | Notes |
|-----------|------------------------|-------|
| `0x00..0x03` humans | **No** | Local input handles stop |
| `0x20..0x23` coop vampires | **Yes** | AI picks new idle/action |
| `0x24..0x27` practice dummies | **Yes** | Tournament targets reroll |
| `0x7d..0x7f` campaign AI | **No** | Same clamp math; no `TryBotRandomAction` hook here |

Campaign enemies still collide via `SpatialQuery` identically; AI steering for `0x7d..0x7f` is driven by scheduler / `TryBotRandomAction` elsewhere (weapon scheduler, not wall-hit message).

### Caller context (`CBulanek_StepMovementAndCollision`)

Sole xref to clamp @ `0x0041b147`:

```c
CBulanek_ClampMoveRectByCollision(this, &this->nOrigin_x, &proposed, collisionAtOrigin, collisionScratch);
```

`collisionAtOrigin` / `collisionScratch` are built by `CBulanek_CopyCollisionRectLocal` + `CBulanek_OffsetCollisionRectByFacing` using `nCurrentTrackIdx@+0xD4` (see [r11_ai_task_09_report.md](./r11_ai_task_09_report.md)).

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x00418300` | `_Globals::SpatialQuery` | Entity-tree collision query | Decompile + disasm loop @ `0x00418330`; 5 caller xrefs |
| `0x004195f0` | `CBulanek_ClampMoveRectByCollision` | Facing-axis clamp + AI wall hook | Decompile; disasm switch @ `0x00419685`; 1 caller `0x0041b147` |
| `0x00404730` | `CDSRect_Overlaps` | Solid-tile overlap test | Called when `param_4==0` @ `0x0041837b` |
| `0x00417210` | `CGameView_GetWorldCollisionRect` | Vfn+0x70 bounds fill | `vftable_methods.csv` slot 28 all combat views |
| `0x00433280` | `rect_Intersect` | Query ∩ entity bounds | @ `0x00418360` |
| `0x0042f7c0` | `CDSChained_GetFirstChildView` | Entity list head | `CGaming+0x31C` chain `+0x54` |
| `0x0042f920` | `CDSChained_GetNextSiblingView` | Sibling walk | `view+0x4C` chain link |
| `0x0041af70` | `CBulanek_StepMovementAndCollision` | Invokes clamp | CALL @ `0x0041b147` |
| `0x00420d40` | `CBulanek_OnEvent` | `case 0x102` → `TryBotRandomAction` | Decompile switch |
| `0x00420a90` | `CBulanek_TryBotRandomAction` | AI idle / random action 0..3 | Triggered by wall block for `0x20..0x27` |

### `SpatialQuery` callers (all modes)

| Caller | Site | `(param_4, param_5)` |
|--------|------|----------------------|
| `CBulanek_ClampMoveRectByCollision` | `0x0041965e` | `(0, 0)` |
| `CGaming_RespawnPlayerAtSafeLocation` | `0x0041a208` | `(1, 0)` |
| `CBulanek_CreateRespawnTeleportPair` | `0x0041d146` | *(verify same as respawn)* |
| `CShot_TraceCollision` | `0x0041def9` | `(1, 1)` |
| `CShot_Ctor` | `0x0041ef45` | `(1, 1)` |

## Struct fields

| Offset | Field | Used by |
|--------|-------|---------|
| `+0x20..+0x2C` | `nOrigin_x/y`, spatial/bounds band | Origin commit; parent bbox when `pChainParent` is playfield |
| `+0x44` | `wChainInit44` LSB | `GetWorldCollisionRect` collision enable |
| `+0x4C` | `pChainParent` | Playfield bbox pointer for mapW/mapH |
| `+0x69` | `bActiveInWorld` | `SpatialQuery` skip when zero |
| `+0x6A` | `bSkipSpatialWhenNonZero` | Included when `param_5==0` |
| `+0x70` | `bPlayerSlot` | AI wall-message slot filter `0x20..0x27` |
| `+0x74..+0x80` | `nCollisionLeft..Bottom` | Origin collision rect source |
| `+0x84` | `pGaming_host` | `SpatialQuery` host (`CGaming *`) |
| `+0xD4` | `nCurrentTrackIdx` | Facing axis 0..3 for clamp switch |
| `CGaming+0x31C` | `pEntityViewRoot` | Entity tree for query |
| `CGaming+0x20..+0x2C` | `nBbox_*` | Default 800×600 playfield |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x004195f0` | `void __thiscall CBulanek_ClampMoveRectByCollision(CBulanek *, int *pOrigin, tagRECT *pProposed, int *pCollisionAtOrigin, tagRECT *pHitBoundsOut)` |
| `set_decompiler_comment` | `0x004195f0` | R11 task 10 summary (facing, map bbox, slot hook) |
| `set_decompiler_comment` | `0x0041965e` | SpatialQuery movement flags |
| `set_decompiler_comment` | `0x0041977a` | PostMessage → `TryBotRandomAction` path |
| `save_program` | `bulanci.exe` | saved |

`SpatialQuery` already named with entity-walk PRE comment @ `0x00418330` (R6 damage pipeline pass).

## Decomp fixes

| Location | Issue | Correct |
|----------|-------|---------|
| `CBulanek_ClampMoveRectByCollision` return type | `_Globals.h` / stub lists `uchar` | **void** — no `AL` assignment; `RET 0x10` only |
| `param_1` ghost arg | Old decompile showed unused `param_1` | Call site pushes `&nOrigin_x` as first stack arg after `this` — maps to `pOrigin` |
| `+0x4C` parent bbox | Easy to misread as unrelated pointer | Dereference uses `+0x20..+0x2C` RECT — matches `CGaming.nBbox_*` layout |

## Frida

**none** — static disasm + decompile sufficient for query loop, facing clamp, and slot-gated `PostMessage`.

## Remaining UNK

- Exact runtime value of `pChainParent@+0x4C` during match (expected `CGaming*` playfield shell; not re-traced from spawn this session).
- Whether `0x258` (600) bottom clamp vs gameplay `0x204` (516) walk band is intentional or inherited bbox overshoot.
- `CBulanek_CreateRespawnTeleportPair` SpatialQuery flag bytes not re-disassembled this session (likely respawn `(1,0)` by analogy).
