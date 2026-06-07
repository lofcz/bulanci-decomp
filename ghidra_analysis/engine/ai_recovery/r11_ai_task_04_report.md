# Round 11 — Task 04 report

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **worker** | 04 / 30 |
| **title** | `CBulanek_ApplyAction` — actions 0..5 dispatch |
| **archetype** | `core_input` |
| **seed_address** | `0x00420910` |
| **acceptance** | Map each action index to facing / fire / weapon; human vs AI paths; net send sites |

## Status

**DONE** — disasm @ `0x00420910`–`0x004209a2`, decompiler, five caller xrefs, and callee bodies verified against [`player_controls.md`](../player_controls.md) and [`CBulanek.md`](../struct_recovery/CBulanek.md). No Ghidra mutations required (symbols and `CBulanek *` typing already correct).

## AI archetype

`CBulanek_ApplyAction` is the **shared motor** for all player-like entities (humans `0x00..0x03`, co-op vampires `0x20..0x23`, practice dummies `0x24..0x27`, campaign AI `0x7d..0x7f`). It does **not** branch on `bIsAiVariant`; human-vs-remote distinction is gated by `CBulanek_IsHumanPlayer` for **network emit only**. Gameplay (facing, fire, weapon cycle) runs identically for AI and human callers.

## Algorithm

### Prototype

```c
uchar __thiscall CBulanek_ApplyAction(CBulanek *this, int actionIndex, char pressed);
```

| Arg | Meaning |
|-----|---------|
| `actionIndex` | `0..3` movement/facing tracks, `4` primary fire, `5` weapon cycle |
| `pressed` | `0` = key/action **release**, non-zero = **press** |

Precondition (callers): `CBulanek_CanDispatchPlayerAction(this, actionIndex, pressed)` must return true (see § Gating).

### Control flow (proven)

```
ApplyAction(this, action, pressed):
  if pressed == 0:                          // RELEASE branch @ 0x00420918 → 0x0042096a
    SnapPositionToFacingAxis(this)         // 0x00417910 — TM_Pause + axis snap @ +0x20/+0xa4
    BeginCurrentTrackPlayback(&videoTrackManager)  // &this+0xa8 → 0x00439b40
    if IsHumanPlayer(this):                // 0x00416720
      NetSendPlayerState_t0d(pGame, bPlayerSlot, action + 4, &origin_x)  // 0x00412b10
    return

  // PRESS branch @ 0x0042091a
  if action == 5:                           // CMP EDI,5 @ 0x0042091f
    CycleWeaponPickup(this)                 // 0x0041eca0 — no net in ApplyAction
    return

  if action == 4:                           // CMP EDI,4 @ 0x0042092e
    TriggerPrimaryActionAndBroadcast(this)    // 0x004208c0 — own IsHumanPlayer → NetEvent1
    return

  // actions 0..3
  if IsHumanPlayer(this):
    NetSendPlayerState_t0d(pGame, bPlayerSlot, action, &origin_x)
  SetFacingTrack(this, action, sendNet=1)    // 0x004197b0 — TM_Play path, no duplicate net
  return
```

### Per-action table

| `actionIndex` | `pressed` | Gameplay effect | Net from `ApplyAction` | Net elsewhere |
|---------------|-----------|-----------------|------------------------|---------------|
| **0** | press | `SetFacingTrack(0,1)` — facing track 0 + `TM_Play` | `NetSend_t0d(state=0)` if human | — |
| **1** | press | `SetFacingTrack(1,1)` | `NetSend_t0d(state=1)` if human | — |
| **2** | press | `SetFacingTrack(2,1)` | `NetSend_t0d(state=2)` if human | — |
| **3** | press | `SetFacingTrack(3,1)` | `NetSend_t0d(state=3)` if human | — |
| **0..3** | release | `SnapPositionToFacingAxis` + `BeginCurrentTrackPlayback` | `NetSend_t0d(state=action+4)` if human | — |
| **4** | press | `TriggerPrimaryActionAndBroadcast` → weapon fire if `view_flags&1` and weapon sched slot 0 armed | *(none here)* | `NetSendPlayerEvent1_t0e` if human @ `0x004208f5` |
| **5** | press | `CycleWeaponPickup` — advance `pWeapon->bWeaponKind` with ammo mask `pPad_preDw128[bVar2-9]` | *(none)* | — |

### `SetFacingTrack` side effects (actions 0..3 press)

Called with `sendNet=1` from `ApplyAction`:

1. Early-out if `bHitStun != 0` (`+0x16A`).
2. If video TM scheduler slot 0 **not** armed: snap axis + `BeginCurrentTrackPlayback`.
3. If `nCurrentTrackIdx != track`: latch `bField_168`, `SetFacingFromByte(pWeapon, track)`, `SetCurrentTrack(&videoTrackManager, track, 1)`, `Scheduler_FreeSlotIfLive(&scheduler, 1)`.
4. `sendNet==1` → `TM_Play(&videoTrackManager, 1)` (footstep / walk audio path).

### Human vs AI caller paths

| Caller | Address | Who invokes | `pressed` / `action` pattern |
|--------|---------|-------------|------------------------------|
| `CGame_DispatchPlayerAction` | `0x004209d9` | Local keyboard (scheduler event 7, actions 0..5 loop) | edge `1`→press, edge `2`→release |
| `CBulanek_TryBotRandomAction` | `0x00420aea` | AI idle branch | `action = nCurrentTrackIdx`, `pressed=0` (release / stop walk) |
| `CBulanek_TryBotRandomAction` | `0x00420b1c` | AI move branch after `_rand`→`0..3` + `CanDispatch` | random `action`, `pressed=1` |
| `CGaming_OnNetMsg_t0d_PlayerState` | `0x00420a21` | Remote peer (recv `NET_MSG_0x0D`) | `param_2 < 4`: set position then `ApplyAction(param_2, 1)` |
| `CGaming_OnNetMsg_t0d_PlayerState` | `0x00420a3b` | Remote peer release | `4 ≤ param_2 < 8`: `ApplyAction(param_2-4, 0)` |

**AI note:** `TryBotRandomAction` itself is wrapped in `IsHumanPlayer` @ `0x00420a9a` — that symbol gates **local autonomous tick** (campaign/co-op AI on this machine), not “is this entity a human player slot”. Full `IsHumanPlayer` semantics deferred to R11 task 7.

### Gating (`CBulanek_CanDispatchPlayerAction` @ `0x004174a0`)

| Condition | Rejects |
|-----------|---------|
| `(view_flags @ +0x44) & 1 == 0` | All actions |
| Actions **4, 5** press | Weapon scheduler slot 0 not armed (`pWeapon+0xc` sched, slot byte `&1`) |
| Actions **0..3** release | `actionIndex != nCurrentTrackIdx` @ `+0xd4` (only release current facing) |
| Actions **0..3** press | Always allowed if alive bit set |

Keyboard path: `CGame::CGame__SchedulerDispatch` case **7** @ `0x00416030` polls `CDirectKeyb`, loops `actionIndex 0..5`, calls `CGame_DispatchPlayerAction(cgaming, playerSlot, actionIndex, pressed)`.

### Network encoding (`NET_MSG_0x0D`, 7 bytes)

Send @ `CGame_NetSendPlayerState_t0d` (`0x00412b10`): `[0x0d, slot, animState, s16 x, s16 y]` from `bPlayerSlot` @ `+0x70`, `origin_x/y` @ `+0x20`.

| Path | `animState` byte |
|------|------------------|
| Press action `n` (0..3) | `n` |
| Release action `n` (0..3) | `n + 4` |
| Recv `animState` 0..3 | position + press `ApplyAction` |
| Recv `animState` 4..7 | release `ApplyAction(animState-4, 0)` |
| Recv `animState` ≥ 8 | `SetFacingTrack(animState-8, 0)` (no `ApplyAction`) |

## Functions table

| Symbol | Address | Role in task |
|--------|---------|-------------|
| `CBulanek_ApplyAction` | `0x00420910` | Subject — 0x95 B, `__thiscall` |
| `CBulanek_SetFacingTrack` | `0x004197b0` | Actions 0..3 press |
| `CBulanek_SnapPositionToFacingAxis` | `0x00417910` | Release path axis align |
| `BeginCurrentTrackPlayback` | `0x00439b40` | Release path TM resume |
| `CBulanek_TriggerPrimaryActionAndBroadcast` | `0x004208c0` | Action 4 |
| `CBulanek_CycleWeaponPickup` | `0x0041eca0` | Action 5 |
| `CBulanek_IsHumanPlayer` | `0x00416720` | Net-send gate |
| `CBulanek_CanDispatchPlayerAction` | `0x004174a0` | Caller precondition |
| `CGame_DispatchPlayerAction` | `0x004209b0` | Keyboard → ApplyAction |
| `CGame_NetSendPlayerState_t0d` | `0x00412b10` | Human press/release state |
| `CGaming_OnNetMsg_t0d_PlayerState` | `0x004209f0` | Remote → ApplyAction |
| `CBulanek_TryBotRandomAction` | `0x00420a90` | AI → ApplyAction |

## Struct fields

| Offset | Field | Use in `ApplyAction` |
|--------|-------|----------------------|
| `+0x20` | `origin_x` (+ `origin_y` @ +0x24) | Net packet position; snap uses `nFacingAxisExtent` @ +0xa4 |
| `+0x44` | `view_flags` | Indirect: fire path checks bit 0 in `TriggerPrimaryActionAndBroadcast` |
| `+0x70` | `bPlayerSlot` | Net `slot` byte |
| `+0xa8` | `videoTrackManager` | `BeginCurrentTrackPlayback`, `nCurrentTrackIdx` for AI idle release |
| `+0xd4` | `nCurrentTrackIdx` (weapon/facing latch) | `CanDispatch` release match |
| `+0xf4` | `pGame` | Net send `this` pointer |
| `+0xf8` | `pWeapon` | Fire delay sched @ `+0xc`; cycle weapon reads `bWeaponKind` |

## Ghidra deltas

None this pass — prior passes already applied:

- `CBulanek_ApplyAction` rename @ `0x00420910`
- `set_function_this_type(CBulanek *)` on ctor / ApplyAction path
- Decompiler comment on action 0 / `videoTrackManager` @ `+0xa8`

## Decomp fixes

None required. Ghidra decompile matches disasm:

- `CMP byte [ESP+8],0` ↔ `param_2 == 0` release branch
- `CMP EDI,5` / `CMP EDI,4` ↔ weapon / fire ordering
- `ADD AL,4` @ `0x0042098e` ↔ release net state `action+4`
- `PUSH 1` before `CALL 0x004197b0` ↔ `SetFacingTrack(..., sendNet=1)`

## Frida

Not run — static proof sufficient for dispatch map and net gates.

## Remaining UNK

| Item | Notes |
|------|-------|
| `CBulanek_IsHumanPlayer` exact semantics | Slot `<4` → `CGame_IsLocalPlayerSlot`; else `CGame+0x36==0`. Task 7. |
| Binding **flag byte** for duplicate DIK on actions 1–2 | Setup only; does not alter `ApplyAction`. |
| `CycleWeaponPickup` ammo mask layout | Uses `pGamingHostScratch+0x368` count and `pPad_preDw128[b-9]`; full pickup struct in weapon task. |
| Whether action 5 should net-sync | No send in original; remote weapon changes likely use other msgs. |
