# Round 11 — AI Task 26 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 26 |
| **title** | DefineDangerZone + CMina danger zones (hazard AI) |
| **archetype** | hazard |
| **seed_address** | `0x0041bb30` |
| **addresses** | `0x0041bb30`, `0x0041b420`, `0x00419fd0`, `0x0041c0d0`, `0x00419f70`, `0x00419e90`, `0x0041b5a0`, `0x00417dd0` |
| **acceptance** | Script opcode → `CGaming+0x2d8` vector; `CMina_RegisterDangerZone`; trace areas |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`): decompile + disasm for script opcode, zone allocator, mine registration, and slot-sweep collider. Script usage correlated with `res_0000065856_2026_Script` gun-mouse hazard. Decompiler comments applied @ `0x0041bb30`, `0x00419fd0`; `save_program bulanci.exe`.

## AI archetype

**Script-driven hazard zones** — not `CBulanek` scheduler AI. Level scripts define axis-aligned “danger” rectangles that participate in the same `CGaming.dangerZoneSlotVec` (`+0x2D8`) as **armed landmines** (`CMina`). Used for ambient threats (e.g. gun-mouse shoot sectors) and mine blast radii. Distinct from **`DefineTraceArea`** (opcode 38), which shares the allocator but sets trace-transition flags and drives `OnEnter` / `OnLeave` script exports.

| Source | Entry | Zone kind / flags | Owner view |
|--------|-------|-------------------|------------|
| Script `DefineDangerZone` | `CLevelScriptOpExt_DefineDangerZone@0x0041bb30` | script `kind`, `flags=1` | `NULL` |
| Script `DefineTraceArea` | `CLevelScriptOpExt_DefineTraceArea@0x0041bb80` | script `kind`, `flags = ReadSubExpr \| 1` | `NULL` |
| Armed `CMina` | `CMina_RegisterDangerZone@0x0041c0d0` (vtable IDSChained slot 4) | `kind=0`, `flags=10` (`0xA`) | `CMina.animBase` (`OR 0x200` on `wViewFlags`) |

## Algorithm

### 1. Script `DefineDangerZone` (export **#28**)

**Disasm @ `0x0041bb30`–`0x0041bb74`:**

```
void CLevelScriptOpExt_DefineDangerZone(CDSScript *script)
{
    RECT rect = {0};
    int kind = CDSScript_ReadKindAndRect4(&rect, script);   // CALL 0x00416ce0
    CGaming *pGaming = *(CGaming **)((byte *)script + 0x458);  // MOV ECX,[ESI+0x458] @ 0x0041bb61
    CGaming_AppendDangerZoneNode(pGaming, kind, &rect, /*flags=*/1, /*owner=*/0);
    return 0;
}
```

**Bytecode operands** (`CDSScript_ReadKindAndRect4@0x00416ce0`): five sub-expressions → `kind`, `left`, `top`, `right`, `bottom`.

**Worked example** — `place_gun_mouse` in [res_0000065856_2022_Script.lua](../../../open_bulanci/assets/levels/res_0000065856_2026_Script.lua) (server only):

| Zone id | Rect (level coords) | Role |
|---------|---------------------|------|
| `0` | `(0, y−3)–`(x, y+27)` | Left shoot sector |
| `1` | `(x+66, y−3)–`(danger_w, y+27)` | Right shoot sector toward player |

Bound to slot **9** gun-mouse anim + timer **0** (flags `7`) for scripted mimic flow.

### 2. `CGaming_AppendDangerZoneNode` @ `0x0041b420`

**Signature (live Ghidra):**

```c
void *__thiscall CGaming_AppendDangerZoneNode(
    CGaming *this,
    int kind,
    int *rect,      // left, top, right, bottom
    int flags,
    void *ownerView);
```

**Pseudocode (disasm @ `0x0041b422`–`0x0041b49d`):**

```
node = OperatorNew(0x20);
node->kind     = kind;           // [+0x00]
node->field_4  = 0;              // [+0x04]
node->rect     = *rect;          // [+0x08..+0x14]
node->flags    = flags;          // [+0x18]
node->owner    = ownerView;      // [+0x1C]
if (ownerView)
    *(ushort *)(ownerView + 0x14) |= 0x200;
CIntListInsertSortedOrAppend(&this->dangerZoneSlotVec, node, ...);  // LEA ECX,[EDI+0x2d8] @ 0x0041b486
CMina_UpdateTraceAreas(this);   // MOV ECX,EDI @ 0x0041b492 — ECX is CGaming*
return node;
```

**Callers (live xrefs):**

| From | Context |
|------|---------|
| `0x0041bb68` | `DefineDangerZone` |
| `0x0041bbc4` | `DefineTraceArea` |
| `0x0041c122` | `CMina_RegisterDangerZone` |
| `0x0041e69c` | `CBulanekCtor` (AI track-holder setup; out of scope here) |

### 3. `CMina_RegisterDangerZone` @ `0x0041c0d0`

**Virtual entry:** `g_pCMina_vftable_IDSChained` slot **4** @ `0x004824dc` (`master_vtable_catalog.csv`).

**`this` = `CDSUpdatedItem *` facet** (`CMina+0xF0`). **Disasm @ `0x0041c0d0`–`0x0041c12f`:**

```
*(byte *)(updatedItem + 0x24) = 1;   // CMina+0x114 bArmed (facet-relative +0x24)
rect.left   = animBase.nOrigin_x  - 10;   // [ESI-0xD0]
rect.top    = animBase.nOrigin_y  - 10;   // [ESI-0xCC]
rect.right  = animBase.nSpatial_bucket_x + 10;  // [ESI-0xC8]
rect.bottom = animBase.nSpatial_bucket_y + 10;  // [ESI-0xC4]
pGaming = *(CGaming **)(updatedItem + 0x20);    // CMina+0x110 nDeployOrOwnerCtx (weapon path = CGaming*)
node = CGaming_AppendDangerZoneNode(pGaming, 0, &rect, 10, animBase);
CMina.pDangerZoneNode = node;   // [updatedItem+0x1C] ≡ CMina+0x10C @ 0x0041c128
```

Mine danger rect is **origin ±10** on both corner pairs (20×20 px class around deploy point). Released by `CMina_ReleaseDangerZoneNode@0x0041b4a0` on dtor (invalidates rect to `0xFFFFFFF6`, removes from `dangerZoneSlotVec`, `_free`).

### 4. Trace / danger collision sweep

After any append, **`CMina_UpdateTraceAreas@0x00419fd0`** re-tests entities:

```
if ([this+0x0C] == 0) return;
hostByte = *(byte *)(*[this+0x84] + 0xD8);
if (hostByte != 0) {
    for (slot = hostByte - 1; slot >= 0; --slot)
        CMina_CheckTraceAreasForSlot(this, slot);
}
for (slot = 0x80; slot > 0x7D; --slot)
    CMina_CheckTraceAreasForSlot(this, slot);
```

**`CMina_CheckTraceAreasForSlot@0x00419f70`** (disasm): `entity = *(CGaming **)(this + slot*4 + 0xC8)` → `apEntitySlots`; vtable `+0x70` fills entity bbox; calls **`CGaming_CheckTraceAreasForEntity@0x00419e90`**.

**`CGaming_CheckTraceAreasForEntity`:** compares entity bbox vs zone node rect; on overlap/containment calls **`CGaming_DispatchTraceAreaTransitionIfMasked@0x00417dd0`** → `CLevelScript_DispatchTraceAreaTransition` (drives script `OnEnter` / `OnLeave` for **trace areas** with bitmask `node[1]`).

**`CGaming_RetestTraceAreasForEntity@0x0041b5a0`:** per-entity hook when `nDangerZoneSlotCount` (`+0x2E0`) non-zero — walks `dangerZoneSlotVec` reverse, same `CheckTraceAreasForEntity` path (entity team @ `+0x70`, mute @ `+0x198`).

**Also invoked from** `CBulanek_Update@0x0041fc8a` when `apAiTrackHolders` (`+0x178`) non-empty: loops track holders, passes `pGamingHost` (`CBulanek+0x84`) as `this` to `UpdateTraceAreas`.

## Functions table

| Address | Symbol | Role |
|---------|--------|------|
| `0x0041bb30` | `CLevelScriptOpExt_DefineDangerZone` | Script export #28; `pGaming` @ script `+0x458` |
| `0x0041bb80` | `CLevelScriptOpExt_DefineTraceArea` | Script export #38; same allocator, extra flag expr |
| `0x00416ce0` | `CDSScript_ReadKindAndRect4` | Parse kind + 4 rect coords from VM stack |
| `0x0041b420` | `CGaming_AppendDangerZoneNode` | `OperatorNew(0x20)`; append `CGaming+0x2D8`; refresh sweep |
| `0x0041c0d0` | `CMina_RegisterDangerZone` | Mine vtable hook; ±10 rect; `flags=10` |
| `0x0041b4a0` | `CMina_ReleaseDangerZoneNode` | Teardown node + list remove |
| `0x00419fd0` | `CMina_UpdateTraceAreas` | Slot sweep `0x80..0x7E` (+ optional `0..hostByte−1`) |
| `0x00419f70` | `CMina_CheckTraceAreasForSlot` | Load `apEntitySlots[slot]`; bbox + zone test |
| `0x00419e90` | `CGaming_CheckTraceAreasForEntity` | Rect overlap / contain; dispatch transition |
| `0x00417dd0` | `CGaming_DispatchTraceAreaTransitionIfMasked` | Bitmask enter/leave → level script |
| `0x0041b5a0` | `CGaming_RetestTraceAreasForEntity` | Entity move hook over all zone nodes |
| `0x0041b210` | `CMina_OnSchedulerHook` | Mine scheduler (separate from zone list; arms via vtable) |

## Struct fields

| Object | Offset | Name | Evidence |
|--------|--------|------|----------|
| `CGaming` | `+0x2D8` | `dangerZoneSlotVec` (`CDSPtrSlotVec`) | `LEA ECX,[EDI+0x2d8]` @ `0x0041b486` |
| `CGaming` | `+0x2E0` | `nDangerZoneSlotCount` | `CGaming_RetestTraceAreasForEntity` @ `0x0041b5a0` |
| `CGaming` | `+0xC8` | `apEntitySlots[128]` | `CMina_CheckTraceAreasForSlot` @ `[EDI+EBX*4+0xc8]` |
| `CGaming` | `+0xD8` | game-mode byte (sweep bound) | `MOVZX ESI,byte [EAX+0xd8]` @ `0x00419fe5` |
| `CLevelScript` | `+0x458` | `pGaming` | `MOV ECX,[ESI+0x458]` @ `0x0041bb61` |
| `CMina` | `+0x0C` | `animBase.bTraceAreasActive` | gate @ `0x00419fd3` when `this` is mine view |
| `CMina` | `+0x84` | `animBase.gaming_host` | `CGaming *` for host byte path |
| `CMina` | `+0x10C` | `pDangerZoneNode` | `MOV [ESI+0x1c],EAX` @ `0x0041c128` |
| `CMina` | `+0x110` | `nDeployOrOwnerCtx` | `MOV ECX,[ESI+0x20]` @ `0x0041c10f` (weapon → `CGaming *`) |
| Zone node | `+0x00` | `kind` | `MOV [ESI],EDX` @ `0x0041b475` |
| Zone node | `+0x08` | `rect` (4×`int`) | `+0x08..+0x14` stores |
| Zone node | `+0x18` | `flags` | script `1` / mine `10` |
| Zone node | `+0x1C` | `ownerView` | mine → `animBase`; script → `0` |

## Ghidra deltas

| Action | Target | Proof |
|--------|--------|-------|
| `set_decompiler_comment` | `0x0041bb30` | `pGaming` from `[script+0x458]` @ `0x0041bb61` |
| `set_decompiler_comment` | `0x00419fd0` | Callers pass `CGaming*`; slot index via `+0xC8` |
| `save_program` | `bulanci.exe` | After comments |

No renames — symbols already applied R5/R6.

## Decomp fixes

| Site | Issue | Correction |
|------|-------|------------|
| `DefineDangerZone` decompile | `(CGaming *)param_1[1].nOpcodeCount` | Use `*(CGaming **)(script + 0x458)` (disasm @ `0x0041bb61`) |
| `CMina_UpdateTraceAreas` prototype | Named `CMina *this` | Slot sweep uses `this+0xC8` = `CGaming::apEntitySlots`; **callers pass `CGaming*`** (`AppendDangerZoneNode`, `CBulanek_Update`). `ReleaseDangerZoneNode` passes `CMina*` but sweep is gated on `[this+0x0C]` (`bTraceAreasActive`). |
| `CMina_CheckTraceAreasForSlot` | Decompiler `track_manager` path | Disasm: `[ECX + slot*4 + 0xC8]` entity load @ `0x00419f7c` |
| `CMina_RegisterDangerZone` | `int param_1` | `CDSUpdatedItem *` facet (`__fastcall`); gaming from facet `+0x20` = `CMina+0x110` |

## Frida

**None new** — static disasm + script correlation sufficient. Optional hook:

- `CGaming_AppendDangerZoneNode@0x0041b420` — log `kind`, rect, `flags`, `this+0x2E0` count after `place_gun_mouse` on level 5856.

## Remaining UNK

- Exact semantics of zone `flags` dword (`1` script danger vs `0xA` mine vs `DefineTraceArea \| 1`) on each branch inside `CGaming_CheckTraceAreasForEntity` (bits @ node `+0x18` bytes `+0x19..+0x1B` interpreted as `param_1+6` in decompiler).
- Whether `DefineDangerZone` zones ever fire `OnEnter`/`OnLeave` without `DefineTraceArea` flag layout (danger zones may be mine-detection / AI-aim only).
- `CMina_ReleaseDangerZoneNode` → `UpdateTraceAreas` with `CMina*` `this` when `bTraceAreasActive!=0` — would index wrong `+0xC8` unless gate is always clear on teardown (needs runtime confirm).
- `CDangerZoneNode` Ghidra struct name — layout proven `0x20` B, formal type not created.
