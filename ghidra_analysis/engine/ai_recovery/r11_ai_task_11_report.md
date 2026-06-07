# Round 11 — AI Task 11 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **title** | Facing / rotation — SetFacingTrack, SnapPositionToFacingAxis |
| **archetype** | `rotation` |
| **seed_address** | `0x004197b0` |
| **addresses** | `0x004197b0`, `0x00417910`, `0x0041c610` |
| **acceptance** | Track index ↔ facing byte; walk anim table selection; AI pickup case 4 path |

## Status

**DONE** — Track index ↔ facing mapping, axis snap, walk-table ctor bind, corpse bind, net encoding, and AI pickup scheduler case 4 documented from live Ghidra decompile + export correlation. Ghidra plate comments applied; `anim_runtime.md` extended.

## AI archetype

**Rotation** applies to every `CBulanek` (humans, campaign AI `0x7d..0x7f`, vampires `0x20..0x23`, practice dummies). Facing is not a separate quaternion — it is the **`videoTrackManager.nCurrentTrackIdx`** (`+0xA8` embed, field `+0x2C` within track manager) selecting one of four walk sequences bound at ctor.

## Algorithm

### Track index ↔ facing byte

Facing is a **byte 0..3** identical to the CDSAnim track index on `CBulanek+0xA8`:

| Track | Input action (`player_controls.md`) | `gABulanekWalkAnimsNormal[i]` | Atlas motion (catalog) | Collision / snap axis |
|------:|------------------------------------|------------------------------|------------------------|------------------------|
| **0** | 0 (left key) | `player_walk_normal_left` `0x100c8` @ sheet `[0]` | `[-59, 0]` horizontal | **Y** locked (`track ≤ 1`) |
| **1** | 1 (right) | `player_walk_normal_right` `0x100ca` @ `[1]` | `[+59, 0]` horizontal | **Y** locked |
| **2** | 2 (down) | down/away `0x100c6` @ `[2]` | `[0, +52]` vertical | **X** locked (`track > 1`) |
| **3** | 3 (up) | up/toward @ `[3]` | `[0, -52]` vertical | **X** locked |

Co-op / special slots (`slotKind ∈ 0x20..0x23`) load **`gABulanekWalkAnimsSpecial[0..3]`** instead (`CBulanekCtor@0x0041e726` branch on `bVar2`).

Spawn picks random facing: `_rand()` → index in `[0,3]` via `(rand*4)>>15`, then `SetFacingTrack` (`CGaming_SpawnAndInitializePlayer@0x0041f5b1`).

### `CBulanek_SnapPositionToFacingAxis` (`0x00417910`)

```text
TM_PauseAndStampClock(&videoTrackManager)          // freeze walk anim clock
if (nCurrentTrackIdx > 1)                          // facings 2,3 — vertical walk
    CDSView__SetPosition(this, nFacingAxisExtent, nOrigin_y)   // snap X
else                                               // facings 0,1 — horizontal walk
    CDSView__SetPosition(this, nOrigin_x, nFacingAxisExtent)   // snap Y
```

`nFacingAxisExtent` @ `CBulanek+0xA4` stores the grid-line coordinate on the **orthogonal** axis (from `AdaptDisplaySize` / `StepMovementAndCollision` collision rect). Callers: `SetFacingTrack`, `ApplyAction` action-0 (movement release).

`OffsetCollisionRectByFacing@0x00417465` uses the same `track > 1` split to bias collision rects.

### `CBulanek_SetFacingTrack` (`0x004197b0`)

```text
SetFacingTrack(CBulanek* this, int track /*0..3*/, char sendNet)
  if (bHitStun) return false
  slot0 = Scheduler_GetEventSlot(&videoTrackManager.scheduler, 0)
  if (!(slot0.flags & 1))                         // walk anim not currently playing
      SnapPositionToFacingAxis(this)
      BeginCurrentTrackPlayback(&videoTrackManager)
  if (nCurrentTrackIdx != track)
      bField_168 = 1
      SetFacingFromByte((CWeapon*)pWeapon, track) // sync weapon trackManager @ weapon+0x08
      bField_168 = 0
      SetCurrentTrack(&videoTrackManager, track, autoplay=1)
      Scheduler_FreeSlotIfLive(&scheduler, 1)    // cancel pickup-delay slot
  if (sendNet == 0)
      if (human) NetSend_t0d(slot, track|8, &nOrigin_x)
  else
      TM_Play(&videoTrackManager, oneshot=1)
  return true
```

`SetFacingFromByte@0x004178f0` is shared glue: when called with `pWeapon` it updates **`CWeapon::trackManager`** (`SetCurrentTrack` at weapon `+0x08`) if the cached facing byte changed — keeps held-weapon sprite aligned with body facing.

### Walk anim table selection (ctor)

`CBulanekCtor@0x0041e4b0` loop `i = 0..3`:

1. `LoadResource(gABulanekWalkAnimsNormal[i])` (or Special for coop slots).
2. `AddTrackSource(&videoTrackManager, seq)`.
3. `SetCurrentTrack(&videoTrackManager, initialTrack, 0)` after weapon ctor.
4. `nSpeedParam != 100` → `TM_SetFrameDelayOverrideMs` with `round(47.25 / (speed/100))`.

`apWalkTrackSources@+0x14C` mirrors the four sequence pointers (zeroed then filled in ctor).

### Corpse bind — `CBulanek_ResolveAndBindAnimTrack` (`0x0041c610`)

Called from death path before adding corpse entity. Two resource paths:

**Walk sheet** (`CDeath::bM_tourneyFlag == 0`):

```text
idx = dwM_modeFlags + nCurrentTrackIdx * 4        // dword index into gABulanekWalkAnimSheet
if (dwM_modeFlags < 2)
    while (sheet[idx] == 0) idx -= 4              // scan backward
else
    while (sheet[idx] == 0) idx += 4              // scan forward
resId = sheet[idx]
facingTrack = idx >> 2
```

**Sit sheet** (`bM_tourneyFlag != 0`):

```text
facingTrack = dwM_modeFlags
resId = g_dwResId_player_sit_facing_down[facingTrack]
if (slot in 0x20..0x23) resId = g_dwResId_player_enemy_sit_facing_down[facingTrack]
```

Then: `SetFacingTrack(host, facingTrack, sendNet=0)` → load resource → `TM_BindSequence` + `TM_SetTrack(0)` on `CDeath` anim subobject → team palette → anchor adjust → net band `+0x74..+0x80`.

### Network `0x0D` animState byte (`CGaming_OnNetMsg_t0d@0x00420a10`)

| `animState` | Handler |
|-------------|---------|
| `0..3` | `SetPosition` → `ApplyAction(action, pressed=1)` → movement facing |
| `4..7` | `ApplyAction(action-4, pressed=0)` → release / idle snap |
| `≥8` | `SetFacingTrack(action-8, sendNet=0)` → pure facing sync |

Local human facing-only updates send **`track | 8`** from `SetFacingTrack` when `sendNet=0`.

### AI pickup case 4 path (scheduler, not `ApplyAction`)

**`CBulanek_ApplyAction` action 4 = fire** (`TriggerPrimaryActionAndBroadcast`). Pickup-facing rotation for AI uses **`CBulanek_WeaponSchedulerCallback` case 4** (`0x00420c8b`):

```text
for i in 0..3 while apAiTrackHolders[i] != NULL
    mask = (aiHolder[i].scheduler.flags & 0xF)
    if TryApplyPickupMask(player, mask, holderData)
        SetFacingTrack(player, *(int*)apAiTrackHolders[i], sendNet=0)
        arm scheduler slot 1 delay = round(200 / (nSpeedParam/100))
        return
```

Script mirror: `CBulanek_OnEvent` case **`0xF2`** — requires `pAiTrackHolders[0]`, active + drawable flags, `dwPickupFacingLatch==0`; stores latch, `TryApplyPickupMask`, `SetFacingTrack(*(int*)latch)`, then arms slot 1 (knockdown uses speed-scaled delay, else `RandInRange(0,3)*500`). Case **`0xF3`** clears latch and frees slot 1.

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x004197b0` | `CBulanek_SetFacingTrack` | Facing byte → walk track + weapon sync + optional net | Live decompile; xrefs: ApplyAction, spawn, net, OnEvent, OnTakeDamage, ResolveAndBindAnimTrack, WeaponSchedulerCallback |
| `0x00417910` | `CBulanek_SnapPositionToFacingAxis` | Pause TM; snap origin to grid on orthogonal axis | Live decompile; `nCurrentTrackIdx>1` ↔ X snap |
| `0x004178f0` | `CBulanek_SetFacingFromByte` | Mirror facing onto `CWeapon::trackManager` | Called from SetFacingTrack with `pWeapon` cast |
| `0x00417465` | `CBulanek_OffsetCollisionRectByFacing` | Collision rect bias by track | Same `>1` threshold |
| `0x00417615` | `CBulanek_AdaptDisplaySize` | Writes `nFacingAxisExtent` from track bounds | Track `>1` → width; else height |
| `0x0041c610` | `CBulanek_ResolveAndBindAnimTrack` | Death corpse resource + facing rebind | `gABulanekWalkAnimSheet` scan; sit tables |
| `0x00420910` | `CBulanek_ApplyAction` | Input 0..3 → SetFacingTrack(sendNet=1); 4=fire; 5=cycle weapon | `0x00420960` CALL |
| `0x00420c00` | `CBulanek_WeaponSchedulerCallback` | Scheduler slot 4 = AI pickup facing | Case 4 @ `0x00420c8b` |
| `0x00439eb0` | `SetCurrentTrack` | Track-manager playlist swap | `anim_runtime.md` |
| `0x0041e4b0` | `CBulanekCtor` | `AddTrackSource` ×4 from walk tables | Loop @ `0x0041e723` |
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | Random initial `SetFacingTrack` | `_rand` @ `0x0041f599` |

### `SetFacingTrack` xrefs (live Ghidra)

| Caller | Site | Context |
|--------|------|---------|
| `CBulanek_ApplyAction` | `0x00420960` | Actions 0..3, `sendNet=1` |
| `CBulanek_ResolveAndBindAnimTrack` | `0x0041c6ca` | Corpse facing before anim bind |
| `CBulanek_OnTakeDamage` | `0x0041db99` | Knockdown band → re-assert current track |
| `CGaming_SpawnAndInitializePlayer` | `0x0041f5b1` | Random spawn facing |
| `CGaming_OnNetMsg_t0d` | `0x00420a48` | `animState ≥ 8` |
| `CGaming_RespawnPlayer` | `0x0041f7fe`, `0x0041f854` | Respawn facing restore |
| `CBulanek_OnEvent` | `0x004210a5` | Script `div0xF2` pickup mask |
| `CBulanek_WeaponSchedulerCallback` | `0x00420c8b` | Scheduler case 4 AI pickup |

## Struct fields

| Offset | Name | Use in facing pipeline |
|--------|------|------------------------|
| `+0x20`, `+0x24` | `nOrigin_x`, `nOrigin_y` | Position; one axis overwritten by snap |
| `+0xA4` | `nFacingAxisExtent` | Grid coordinate on orthogonal axis |
| `+0xA8` | `videoTrackManager` | Walk CDSAnim player; `nCurrentTrackIdx@+0xD4` |
| `+0x14C` | `apWalkTrackSources[4]` | Cached sequence refs from ctor |
| `+0x168` | `bField_168` | Re-entrancy guard during weapon facing sync |
| `+0x16A` | `bHitStun` | Blocks `SetFacingTrack` while stunned |
| `+0x178` | `apAiTrackHolders[4]` | AI pickup facing latch pointers (case 4) |
| `+0x188` | `dwPickupFacingLatch` | Script `0xF2`/`0xF3` pickup facing guard |
| `+0xF8` | `pWeapon` | `SetFacingFromByte` target for weapon sprite track |

## Ghidra deltas

| Action | Target |
|--------|--------|
| `set_decompiler_comment` | `0x004197b0` — facing track 0..3, weapon mirror, hit-stun gate |
| `set_decompiler_comment` | `0x00417910` — axis snap threshold `track>1` |
| `set_decompiler_comment` | `0x0041c610` — walk sheet scan + SetFacingTrack before corpse bind |
| `save_program` | `bulanci.exe` |

## Decomp fixes

| Issue | Correction |
|-------|------------|
| `SetFacingFromByte((CBulanek*)pWeapon, …)` | Intentional — updates `CWeapon::trackManager` at `+0x08`, not CBulanek fields. `nBounds_top` in decomp is a mis-typed read of weapon track index. |
| `SnapPositionToFacingAxis` uses `dwFacingAxisExtent` | Same field as `nFacingAxisExtent@+0xA4` per `CBulanek.md` R5 w21. |
| `ResolveAndBindAnimTrack(CDeath*)` | `this` recovered via `corpseAnim->pHost` → owning `CBulanek*`. |

## Frida

Not required — axis snap threshold, track table order, and scheduler case 4 branch closed statically.

## Remaining UNK

- Exact `gABulanekWalkAnimSheet` dword layout / mode-flag semantics for campaign skin variants beyond forward/back scan (`dwM_modeFlags < 2` vs `≥ 2`).
- Whether `bField_168` suppresses a specific `IDSAnim` callback during weapon track swap (only write sites in `SetFacingTrack` proven).
- Footstep / facing SFX hook inside `TM_Play` path vs separate audio bank (not in seed addresses).

## Doc updates

- `ghidra_analysis/engine/anim_runtime.md` — new section **CBulanek facing ↔ track index (R11 task 11)**.
