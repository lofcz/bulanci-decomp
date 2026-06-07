# Round 11 — AI Task 09 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 9 |
| **title** | CBulanek_StepMovementAndCollision (FUN_0041af70) |
| **archetype** | moving_bot |
| **seed_address** | `0x0041af70` |
| **addresses** | `0x0041af70`, `0x00417410`, `0x00417460`, `0x004195f0`, `0x00433200`, `0x0042f1e0` |
| **prior_evidence** | [round6_logic_task_02_report.md](../logic_recovery/round6_logic_task_02_report.md), [r10_deep_task_01_report.md](../deep_recovery/r10_deep_task_01_report.md) |

## Status

**DONE** — Live Ghidra MCP confirms seed already named `CBulanek_StepMovementAndCollision` with `uchar __thiscall` prototype (R10). Full movement math documented below with disasm/IDA correlation. Three code xrefs proven. Vtable commit/fallback slots identified (`+0x20` → `CBulanek_AdaptDisplaySize`, `+0x24` → `CDSView_InvalidateRectClipped`). No Ghidra mutations required this session.

## AI archetype

**All player-like AI** (campaign slots `0x7d..0x7f`, coop vampires `0x20..0x23`, tournament bots) share this movement step on **`CBulanek`** instances. Humans use the same function when weapon track parameters change or on scheduler weapon-aim ticks. Practice dummies (`0x24..0x27`) typically skip random walk but still inherit the code path if weapon/track state changes.

Invocation gates:

| Caller | When | AI relevance |
|--------|------|--------------|
| `CWeapon_OnSchedulerEvent@0x0041b186` | Scheduler slot 0 armed on weapon facet (`CBulanek+0xA0`) | **Primary AI walk tick** — fires after scheduler dispatch each frame when slot bit0 clear |
| `CWeapon::Update@0x0041bf61` | `pWeapon->dwParamA/B/pTrackHolder` differ from cached `+0x154/+0x158/+0x164` and `bField_168==0` | Weapon/track retarget (pickup, AI track holder swap) |
| `CBulanek_ApplyPickupEffect@0x0041eb28` | After new `CWeapon` ctor on pickup | Immediate reposition after weapon change |

## Algorithm

Pseudocode with **proven offsets** (live decompile + disasm @ `0x0041af70`, IDA `sub_41AF70` @ `bulanci.ida.exe.c:92943`).

```c
// CBulanek* this;  returns 1 always
uchar CBulanek_StepMovementAndCollision(CBulanek *this)
{
    CWeapon *w = this->pWeapon;                          // +0xF8
    CBulanek *holder = w->pTrackHolder;                  // weapon+0x50
    int holderW = *(int*)(holder + 4);                   // track-holder anchor width/extent
    int holderH = *(int*)(holder + 8);

    // Weapon walk-track params minus walk-embed base
    int deltaX = w->dwParamA - *(int*)&this->abWalkEmbedBase[0];  // +0x104 (dword)
    int deltaY = w->dwParamB - *(int*)&this->abWalkEmbedBase[4];  // +0x108 (dword)

    // Add active anim chain translation
    CBulAnim *anim = this->pActiveAnim;                  // +0xF0
    int propLeft = deltaX + *(int*)(anim + 4);
    int propTop  = deltaY + *(int*)(anim + 8);

    tagRECT origin = { 0, 0, holderW, holderH };
    tagRECT delta  = { propLeft, propTop, propLeft + holderW, propTop + holderH };

    CDSRect_IntersectInPlace(&origin, &delta);           // 0x00433200

    int clipLeft = propLeft - origin.left;
    int clipTop  = propTop  - origin.top;
    this->nWalkClipNegX = -origin.left;                  // +0x114
    this->nWalkClipNegY = -origin.top;                   // +0x118

    // Build proposed world rect from origin + collision + embed accumulators
    int propTopFinal = this->nOrigin_y + this->nWalkEmbedAccY + this->abPad_postWalkEmbed[0]
                       - clipTop;                         // +0x24, +0x110, +0x15C
    int propLeftFinal = this->nOrigin_x - clipLeft + this->nWalkEmbedAccX + this->abPad_postWalkEmbed[4];
                       // +0x20, +0x10C, +0x160
    tagRECT proposed = {
        propLeftFinal,
        propTopFinal,
        origin.right + (propLeftFinal - origin.left),
        (origin.bottom - origin.top) + propTopFinal
    };

    int rectOrigin[4], rectProposed[4];
    CBulanek_CopyCollisionRectLocal(this, rectOrigin);   // 0x00417410 — collision + embed offset

    // Persist embed step; clear transient pad dwords
    this->nWalkEmbedAccY = clipTop;                      // +0x110
    this->nWalkEmbedAccX = clipLeft;                     // +0x10C
    *(uint*)&this->abPad_postWalkEmbed[0] = 0;           // +0x15C
    *(uint*)&this->abPad_postWalkEmbed[4] = 0;           // +0x160  (2 dwords only — not 8)

    CBulanek_CopyCollisionRectLocal(this, rectProposed);
    CBulanek_OffsetCollisionRectByFacing(this, &this->nOrigin_x, rectOrigin,
                                         this->nInitialTrack);  // +0x148, 0x00417460

    this->nInitialTrack = this->videoTrackManager.nCurrentTrackIdx;  // +0xD4

    void *slot = Scheduler_GetEventSlot(&this->videoTrackManager.scheduler, 0);  // +0xAC
    if ((*(byte*)(slot + 8) & 1) != 0) {
        if (this->nInitialTrack < 2)
            this->nFacingAxisExtent = proposed.top;      // +0xA4
        else
            this->nFacingAxisExtent = proposed.left;
    }

    CBulanek_OffsetCollisionRectByFacing(this, &proposed.left, rectProposed,
                                         this->nInitialTrack);

    CBulanek_ClampMoveRectByCollision(this, &this->nOrigin_x, &proposed.left,
                                      rectProposed, rectOrigin);  // 0x004195f0

    // Commit proposed position via primary vtable slot 8 (+0x20)
    if (!this->vftable_primary[8](&proposed))            // CBulanek_AdaptDisplaySize → bool
        this->vftable_primary[9](0, 0);                  // CDSView_InvalidateRectClipped fallback

    return 1;
}
```

### Phase breakdown

| Phase | Operation | Key fields |
|-------|-----------|------------|
| 1. Track delta | `dwParamA/B − embedBase(+0x104/+0x108)` | Weapon aim/walk source from `CWeapon` |
| 2. Anim offset | Add `pActiveAnim+4/+8` chain fields | Walk animation root motion |
| 3. Clip normalize | `CDSRect_IntersectInPlace(origin, delta)` | Clamp proposed delta to holder extents; store `-origin` @ `+0x114/+0x118` |
| 4. Proposed rect | Fold `nOrigin_x/y`, collision band, embed acc `+0x10C/+0x110`, pad `+0x15C/+0x160` | World-space target AABB |
| 5. Collision copy | `CBulanek_CopyCollisionRectLocal` ×2 | `nCollisionLeft/Top/Right/Bottom` (+0x7C..+0x80) shifted by embed |
| 6. Facing offset | `CBulanek_OffsetCollisionRectByFacing` ×2 | Uses `nFacingAxisExtent@+0xA4`; track idx `<2` → horizontal axis else vertical |
| 7. Scheduler gate | `Scheduler_GetEventSlot(scheduler@+0xAC, 0)` byte `+8` bit0 | When armed, refresh `nFacingAxisExtent` from proposed axis |
| 8. Collision clamp | `CBulanek_ClampMoveRectByCollision` | `SpatialQuery` + facing switch on `nCurrentTrackIdx@+0xD4`; may post `WM_USER` for coop slots |
| 9. Commit | vtable `+0x20` (`CBulanek_AdaptDisplaySize`) else `+0x24` invalidate | Position commit vs dirty-rect-only sync |

### Callee algorithms (task band)

**`CBulanek_CopyCollisionRectLocal@0x00417410`** — copies `nCollisionLeft/Top/Right/Bottom`, then adds embed pointer deltas `pReserved_preAmmo+8/+0xC` (same band as walk embed base).

**`CBulanek_OffsetCollisionRectByFacing@0x00417460`** — if `trackIdx > 1`: offset rect by `(nFacingAxisExtent, origin.y)` on horizontal pass; else `(origin.x, nFacingAxisExtent)`.

**`CBulanek_ClampMoveRectByCollision@0x004195f0`** — if `pGamingHostScratch@+0x84` set and proposed≠origin collision rects: calls `SpatialQuery(..., param4=0, param5=0)`; switch on `nCurrentTrackIdx` (0..3) clamps one axis against map bounds and obstacle hit rect; shifts `proposed` rect by clamp delta; for slot kinds `0x20..0x23` or `0x24..0x27` on blocked move posts `CDSView_PostMessage(..., 0x200, 0x102, 0, 0)`.

## Functions table

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0041af70` | `CBulanek_StepMovementAndCollision` | **Seed** — full walk/collision step; always returns `1` | `MOV ESI,ECX` @ `0x0041af76`; `MOV AL,0x1` @ `0x0041b16c`; 7 callees; 3 xrefs |
| `0x00433200` | `CDSRect_IntersectInPlace` | In-place AABB intersect / clip | `CALL 0x00433200` @ `0x0041b01e` |
| `0x00417410` | `CBulanek_CopyCollisionRectLocal` | Collision rect + embed shift | Called @ `0x0041b09c`, `0x0041b0c4` |
| `0x00417460` | `CBulanek_OffsetCollisionRectByFacing` | Facing-axis collision offset | Called @ `0x0041b0db`, `0x0041b130` |
| `0x0042f1e0` | `Scheduler_GetEventSlot` | Read scheduler slot 0 state | `CALL` @ `0x0041b0f3`; `TEST [EAX+8],1` @ `0x0041b0f8` |
| `0x004195f0` | `CBulanek_ClampMoveRectByCollision` | Map/obstacle clamp + proposed shift | `CALL` @ `0x0041b147` |
| `0x004175f0` | `CBulanek_AdaptDisplaySize` | Vtable slot 8 — position/size commit; returns bool via `CDSView_AdaptDisplaySize` | Indirect `CALL [EDX+0x20]` @ `0x0041b158` |
| `0x0042ca30` | `CDSView_InvalidateRectClipped` | Vtable slot 9 — fallback `(0,0)` invalidate | Indirect `CALL [EAX+0x24]` @ `0x0041b167` |
| `0x0041b180` | `CWeapon_OnSchedulerEvent` | MI adjustor `ECX−0xA0` → seed | `ADD ECX,0xFFFFFF60; CALL 0x0041af70` @ `0x0041b180` |
| `0x0041bf00` | `CWeapon::Update` | Conditional seed when track params change | `CALL 0x0041af70` @ `0x0041bf61` when `bField_168==0` |
| `0x0041ea90` | `CBulanek_ApplyPickupEffect` | Post-pickup weapon reposition | `CALL 0x0041af70` @ `0x0041eb28` |

### Callers (`get_xrefs_to@0x0041af70`)

| Caller | Site | Context |
|--------|------|---------|
| `CBulanek_ApplyPickupEffect` | `0x0041eb28` | After `CWeapon_ctor` on pickup — IDA `sub_41AF70(this)` @ ~95745 |
| `CWeapon_OnSchedulerEvent` | `0x0041b186` | Scheduler weapon facet; IDA `sub_41AF70(this-40)` @ 93040 |
| `CWeapon::Update` | `0x0041bf61` | Track param sync; IDA `sub_41AF70(this)` @ ~93729 when `bField_168==0` |

## Struct fields (proven this task)

| Offset | Name | Role in movement step |
|--------|------|------------------------|
| `+0x20` / `+0x24` | `nOrigin_x`, `nOrigin_y` | Current position; input to proposed rect and facing offset |
| `+0x7C`..`+0x80` | `nCollisionLeft/Top/Right/Bottom` | Hit box copied by `CopyCollisionRectLocal` |
| `+0x84` | `pGamingHostScratch` | `ClampMoveRectByCollision` early-out if NULL |
| `+0xA4` | `nFacingAxisExtent` | Walk-axis extent; updated from scheduler slot + proposed rect |
| `+0xAC` | `videoTrackManager.scheduler` | `Scheduler_GetEventSlot(..., 0)` |
| `+0xD4` | `videoTrackManager.nCurrentTrackIdx` | Facing index; copied to `nInitialTrack` each step |
| `+0xF0` | `pActiveAnim` | Anim chain offset added to weapon delta |
| `+0xF8` | `pWeapon` | Source of `dwParamA/B`, `pTrackHolder` |
| `+0x104` / `+0x108` | walk embed base (dwords in preAmmo band) | Subtracted from weapon params @ `0x0041afcb`/`0x0041afc5` |
| `+0x10C` / `+0x110` | walk embed accumulators | Persisted clip deltas each step |
| `+0x114` / `+0x118` | walk clip neg origin | `-origin.left/top` after intersect |
| `+0x148` | `nInitialTrack` | Facing track idx for offset passes; synced from `+0xD4` mid-function |
| `+0x15C` / `+0x160` | transient pad (2 dwords) | Zeroed each step @ `0x0041b0b8`/`0x0041b0be` |
| `+0x168` | `bField_168` | `CWeapon::Update` skips seed when non-zero |
| `+0x154`/`+0x158`/`+0x164` | cached weapon params | Compared in `CWeapon::Update` before calling seed |

**Struct doc correction:** `abReserved_preAmmo@+0x104` is documented as ctor-only, but movement **reads dwords @ +0x104/+0x108** every step — treat as walk-embed base for reimplementation.

## Ghidra deltas

**None** — rename and prototype applied in R6/R10:

| Prior action | Target | State |
|--------------|--------|-------|
| `rename_function_by_address` | `FUN_0041af70` → `CBulanek_StepMovementAndCollision` | Already applied |
| `set_function_prototype` | `uchar __thiscall CBulanek_StepMovementAndCollision(void *this)` | Live signature confirmed |
| `set_decompiler_comment` | `0x0041af70` | R10 comment block present |

## Decomp fixes

| Issue | Resolution |
|-------|------------|
| R6 stale `FUN_0041af70` / `__fastcall` | Fixed R10 — live name + `__thiscall` |
| `_Globals::` prefix on member | Decompiler artifact; offsets in body correct |
| R6 claim “zero +0x15C..+0x167 (8 dwords)” | **Corrected:** disasm zeros **2 dwords** @ `+0x15C`, `+0x160` only |
| Vtable `+0x20/+0x24` unnamed | **Resolved:** slot 8 = `CBulanek_AdaptDisplaySize@0x004175f0` (bool commit); slot 9 = `CDSView_InvalidateRectClipped@0x0042ca30` |
| `abReserved_preAmmo` “no runtime reads” | **Contradicted** — movement reads `+0x104/+0x108` as int embed base |

## Frida

**none** — static disasm + live Ghidra decompile + IDA `sub_41AF70` correlation sufficient.

## Remaining UNK

- Exact semantic names for walk-embed dwords @ `+0x104..+0x118` vs struct doc `abReserved_preAmmo` — offsets proven; field rename deferred to struct batch.
- `CBulanek_AdaptDisplaySize` bool return path inside `CDSView_AdaptDisplaySize@0x0042cae0` (bounds clamp + parent resize) — commit failure conditions not fully traced this task.
- `CWeapon::Update` Ghidra types `in_ECX` as `int*` with index names (`0x55` = `+0x154`) — reimplementation should use explicit field names from layout above.
