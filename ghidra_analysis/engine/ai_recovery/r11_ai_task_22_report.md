# Round 11 — Task 22 report

## Task

| Field | Value |
|-------|-------|
| **id** | 22 |
| **worker** | 22 / 30 |
| **title** | `apAiTrackHolders` @+0x178 — AllocAiTrackHolder |
| **archetype** | `moving_bot` |
| **seed_address** | `0x0041b420` |
| **addresses** | `0x0041b420`, `0x0041e4b0` |
| **acceptance** | Array size; ctor init; consumers in movement/weapon paths |

## Status

**DONE** — live Ghidra decompile/disasm proves `apAiTrackHolders` is a **4-element** array of `CDangerZoneNode*` (0x20 B heap blocks) allocated via **`CGaming_AppendDangerZoneNode@0x0041b420`** from `CBulanekCtor` when `slotKind > 3`. There is **no** separate `CBulanek_AllocAiTrackHolder` symbol (stale R5 worker-03 name). Consumers: `CBulanek_Update` (bbox sync + trace refresh), `CBulanek_OnEvent` `0xF2`/`0xF3`, `CBulanek_OnDeath`, `CBulanek_WeaponSchedulerCallback` case 4.

## AI archetype

All **non-human player-like** slots (`slotKind > 3`: campaign AI `0x7d..0x7f`, co-op `0x20..0x23`, practice `0x24..0x27`, script movers) get four per-entity **directional trace/pickup zones** stored in `apAiTrackHolders`. Humans (`slotKind ≤ 3`) leave the array all-`NULL` — `OnDeath` uses `pAiTrackHolders[0]==NULL` as the human-pickup branch gate.

## Algorithm

### `apAiTrackHolders` layout

| Offset | Size | Type | Name | Evidence |
|--------|------|------|------|----------|
| `CBulanek+0x178` | 16 | `void*[4]` | `apAiTrackHolders` | `LEA EAX,[ESI+0x178]` @ `0x0041e654`; Ghidra `get_struct_layout CBulanek` |

Ghidra field label may read `pAiTrackHolders`; doc name `apAiTrackHolders` per [CBulanek.md](../struct_recovery/CBulanek.md).

### Allocator — `CGaming_AppendDangerZoneNode` (`0x0041b420`)

**Not** a CBulanek method. `__thiscall` on **`CGaming *`**:

```c
CDangerZoneNode* __thiscall CGaming_AppendDangerZoneNode(
    CGaming *this,
    int zoneIndex,          // param_1 — ctor passes 0..3
    int *rect,              // param_2 — left/top/right/bottom → node+0x08..+0x14
    int flags,              // param_3 — ctor passes 0
    CBulanek *owner);       // param_4 — ctor passes `this`; stored node+0x1C
```

**Node body** (`OperatorNewWithBadAlloc(0x20)`):

| Node offset | Field | Init |
|-------------|-------|------|
| `+0x00` | `zoneIndex` | `param_1` |
| `+0x04` | list link | `0` |
| `+0x08..+0x14` | `rect` | copied from `*param_2` |
| `+0x18` | `flags` | `param_3` |
| `+0x1C` | `pOwnerBulanek` | `param_4`; if non-null, `owner->wViewFlags \|= 0x200` |

Tail: `CIntListInsertSortedOrAppend(&this->dangerZoneSlotVec, node, …)` (`CGaming+0x2D8`); then `CMina_UpdateTraceAreas`.

**Callers (live xrefs):** `CLevelScriptOpExt_DefineDangerZone`, `DefineTraceArea`, `CMina_RegisterDangerZone`, **`CBulanekCtor@0x0041e69c`**.

### Ctor init — `CBulanekCtor@0x0041e4b0`

```text
bIsAiVariant = (slotKind > 3)                    // byte @ +0x199
ppv = &apAiTrackHolders[0]                       // +0x178
apAiTrackHolders[0..3] = NULL

if slotKind > 3:
    zeroRect = {0,0,0,0}
    for i in 0..3:
        apAiTrackHolders[i] = CGaming_AppendDangerZoneNode(
            pGamingHostScratch,  // ctor param_4 = CGaming* host (ECX @ 0x0041e697)
            i,
            &zeroRect,
            0,
            this)
```

**Asm proof:** `CMP CL,3` / `JBE 0x0041e6b3` @ `0x0041e677`; loop `EDI=0..3` `CALL 0x0041b420` @ `0x0041e690`–`0x0041e6ad`; `MOV [EBP],EAX` stores return into `[ESI+0x178+EDI*4]`.

After weapon create, ctor clears `pGamingHostScratch@+0x84` to `0` (`MOV [ESI+0x84],EBX` @ `0x0041e911`). Runtime repopulation: `CGaming_AddEntity@0x0041a390` `MOV [entity+0x84], CGaming*`.

### Runtime bbox sync — `CBulanek_Update@0x0041fb90`

When `apAiTrackHolders[0] != NULL`:

1. `GetScreenRect` via `vftable_primary+0x70` → `left, top, right, bottom`.
2. Write four node rects (`node+0x08` base) — directional strips around player screen bbox:

| Index | `left` | `top` | `right` | `bottom` | Asm @ |
|-------|--------|-------|---------|----------|-------|
| `[2]` | `left` | `0` | `right` | `top` | `0x0041fbe5` |
| `[3]` | `left` | `bottom` | `right` | `0x203` | `0x0041fc12` |
| `[0]` | `0` | `left` | `top` | `bottom` | `0x0041fc37` |
| `[1]` | `top` | `left` | `0x31f` (799) | `bottom` | `0x0041fc49` |

3. If `pGamingHostScratch@+0x84 != NULL`, loop `i=0..3`: **`CMina_UpdateTraceAreas(host, apAiTrackHolders[i])`** — disasm `PUSH node; MOV ECX,[ESI+0x84]; CALL 0x00419fd0` @ `0x0041fc80`–`0x0041fc95`. Callee **`RET 0x4`** (stack arg = node); ECX = `CGaming*` (not `CMina*`).

### Pickup / facing consumers

| Site | Condition | Behavior |
|------|-----------|----------|
| `CBulanek_OnEvent` **`0xF2`** @ `0x00421047` | `pAiTrackHolders[0]!=NULL`, `dwView_flags&1`, `dwPickupFacingLatch==0`, `dwView_flags&0x40` | latch facing dword; `TryApplyPickupMask(mask, data)`; `SetFacingTrack(*latch)`; arm scheduler slot 1 |
| `CBulanek_OnEvent` **`0xF3`** | `dwPickupFacingLatch!=0` | free scheduler slot 1; clear latch |
| `CBulanek_WeaponSchedulerCallback` **case 4** @ `0x00420c8b` | loop `i=0..3` while holders exist | mask `holder[i].flags & 0xF`; `TryApplyPickupMask` → `SetFacingTrack(**holder)`; arm slot 1 with speed-scaled delay |
| `CBulanek_OnDeath` @ `0x0041fb11` | `pAiTrackHolders[0]==NULL` && human && `pWeapon->bWeaponKind!=0` | `ApplyPickupEffect` (human weapon drop) |

`CGaming_RetestTraceAreasForEntity@0x0041b5a0` walks the shared `dangerZoneSlotVec` for script mute/unmute (`OnEvent 0xEE`/`0xEF`); per-AI nodes from ctor share that vector.

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041b420` | `CGaming_AppendDangerZoneNode` | Alloc 0x20 B node; append `CGaming+0x2D8`; **AI ctor alloc** | Live decompile; xrefs incl. `CBulanekCtor@0x0041e69c` |
| `0x0041e4b0` | `CBulanekCtor` | Zero/init `apAiTrackHolders`; AI loop when `slotKind>3` | Disasm `0x0041e654`–`0x0041e6ad` |
| `0x0041fb90` | `CBulanek_Update` | Sync 4 node rects from screen bbox; refresh trace | Disasm `0x0041fbb4`–`0x0041fc95` |
| `0x00419fd0` | `CMina_UpdateTraceAreas` | `CGaming+0x0C` gate; slot sweep with node arg | `RET 0x4`; disasm uses `[EDI+0x84]` entity ring |
| `0x0041b5a0` | `CGaming_RetestTraceAreasForEntity` | Walk all danger-zone nodes vs entity | `dangerZoneSlotVec@+0x2D8` |
| `0x0041b4a0` | `CMina_ReleaseDangerZoneNode` | Remove node from vec; `_free` | Mine dtor path |
| `0x00420d40` | `CBulanek_OnEvent` | `0xF2`/`0xF3` pickup-facing | `CMP [ESI+0x178],0` @ `0x00421047` |
| `0x0041f900` | `CBulanek_OnDeath` | Human vs AI pickup gate on `[0]` | `0x0041fb11` |
| `0x00420c00` | `CBulanek_WeaponSchedulerCallback` | Case 4: AI pickup mask loop | `0x00420c8b` |
| `0x0041a390` | `CGaming_AddEntity` | Sets `entity+0x84 = CGaming*` | Comment @ `0x0041a3a6` |

## Struct fields

| Struct | Offset | Name | Notes |
|--------|--------|------|-------|
| `CBulanek` | `+0x178` | `apAiTrackHolders[4]` | `NULL` for humans; 4×`CDangerZoneNode*` for AI |
| `CBulanek` | `+0x188` | `dwPickupFacingLatch` | `0xF2`/`0xF3` pickup rotation latch |
| `CBulanek` | `+0x199` | `bIsAiVariant` | `slotKind > 3` at ctor |
| `CBulanek` | `+0x84` | `pGamingHostScratch` | `CGaming*` host; set by `AddEntity` |
| `CGaming` | `+0x2D8` | `dangerZoneSlotVec` | Shared node list |
| `CGaming` | `+0x0C` | trace-areas gate | `CMina_UpdateTraceAreas` / `RetestTraceAreas` test |
| `CDangerZoneNode` | `+0x00` | `zoneIndex` | 0..3 per facing slot |
| `CDangerZoneNode` | `+0x08` | `rect` | Updated each `CBulanek_Update` for AI |
| `CDangerZoneNode` | `+0x1C` | `pOwnerBulanek` | Back-pointer to owning player |

## Ghidra deltas

| Action | Target | Detail |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0041b420` | AI `apAiTrackHolders` alloc path |
| `set_decompiler_comment` | `0x0041e690` | Ctor loop `i=0..3` → `+0x178` |
| `set_decompiler_comment` | `0x0041fbc6` | Update bbox sync + trace refresh |
| `save_program` | `bulanci.exe` | Applied |

**No rename** — `0x0041b420` is already `CGaming_AppendDangerZoneNode` (R5 worker 44). Stale alias `CBulanek_AllocAiTrackHolder` in older docs is **incorrect**.

## Decomp fixes

| Function | Issue | Fix |
|----------|-------|-----|
| `CMina_UpdateTraceAreas@0x00419fd0` | Ghidra `__fastcall (CMina*)` only | Disasm `RET 0x4`; arg0=`CGaming*` ECX, arg1=`CDangerZoneNode*` stack; reads `CGaming+0x0C` / `+0x84` not `CMina.animBase` |
| `CBulanek_Update@0x0041fb90` | Decompiler `unaff_EBP` noise in rect stores | Use disasm @ `0x0041fbe5`–`0x0041fc6a` for proven rect constants (`0`, `0x203`, `0x31f`) |
| `CBulanek_WeaponSchedulerCallback` case 4 | Broken `this`/`pActiveAnim` typing | Loop bound 4 matches `apAiTrackHolders`; mask from node scheduler band — re-type pending |

## Frida

Not run — static disasm + decompile sufficient for array size, ctor init, and consumer sites.

## Remaining UNK

| Item | Notes |
|------|-------|
| `CDangerZoneNode` interior field names | Size `0x20` and offsets proven; only `zoneIndex` / `rect` / `pOwner` mapped |
| Case 4 decompiler typing | Confirm `TryApplyPickupMask` third arg is node `+0x00` band vs scheduler embed |
| Practice dummy (`0x24..0x27`) vs campaign AI trace behavior | Both get ctor alloc; dummy may never arm scheduler case 4 |
| Node `flags@+0x18` runtime values | Ctor passes `0`; script `DefineTraceArea` may set non-zero |
| Whether `0x203` / `0x31f` rect constants are map-space magic or display-scaled | Values hardcoded in `CBulanek_Update` disasm |
