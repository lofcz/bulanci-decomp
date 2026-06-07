# Round 11 — Task 25 report

## Task

| Field | Value |
|-------|-------|
| **id** | 25 |
| **worker** | 25 / 30 |
| **title** | `CBulanek_TriggerPrimaryActionAndBroadcast` |
| **archetype** | `shooting_ai` |
| **seed_address** | `0x004208c0` |
| **related** | `CBulanek_DispatchCurrentWeaponAction` @ `0x00420650` (manifest `0x00420721` is **inside** `CGaming_OnCustomEvent`, not dispatch) |
| **acceptance** | Scheduler slot 1 path; network broadcast; human vs AI |

## Status

**DONE** — live Ghidra MCP decompile, disasm (`search_instructions`), three caller xrefs, and IDA `sub_4208C0` correlation. No Ghidra mutations required (rename + plate comments already applied).

## AI archetype

`CBulanek_TriggerPrimaryActionAndBroadcast` is the **single choke point** for “use current weapon primary” on all player-like entities. It runs for:

| Entry path | Who | Scheduler slot 1? | Sends `NET_MSG_0x0E`? |
|------------|-----|-------------------|------------------------|
| `CBulanek_ApplyAction` action **4** press | Local keyboard / remote `0x0D` press | **No** — immediate | Only if `IsHumanPlayer` |
| `CBulanek_WeaponSchedulerCallback` case **1** | AI / pickup delayed fire | **Yes** — slot 1 fired | Only if `IsHumanPlayer` |
| `CGame_OnNetMsg_t0e_PrimaryAction` | All peers on recv `0x0E` | **No** | **No** — recv path |

Gameplay dispatch (`DispatchCurrentWeaponAction`) is **identical** for human and AI. Network broadcast is **human-gated** inside this function; AI entities never originate `0x0E` from here unless `IsHumanPlayer` returns true (local human slots `0..3` on this machine).

## Algorithm

### Prototype

```c
void __fastcall CBulanek_TriggerPrimaryActionAndBroadcast(int param_1);  /* CBulanek* in ECX/ESI */
```

Ghidra types as `_Globals::` free function with `CBulanek*` passed as `int` (`__fastcall` — `this` in `ECX` on x86 MSVC).

### Control flow (proven)

```
TriggerPrimaryActionAndBroadcast(player):
  if (player.view_flags & 1) == 0:          // +0x44 bit0 — alive/active gate
    return

  slot0 = Scheduler_GetEventSlot(&player.pWeapon->trackManager.scheduler, 0)
          // embed: pWeapon+0xF8, scheduler at weapon+0x0C (trackManager+0x04)
  if (slot0 == NULL || (slot0[+8] & 1) == 0):   // weapon fire-delay slot NOT armed
    return

  CBulanek_DispatchCurrentWeaponAction(player.pWeapon)
    switch (pWeapon.bWeaponKind @ +0x64):
      0 → CWeapon_FirePistol
      1 → CWeapon_FireGrenade
      2,5 → CWeapon_PlayFireAnim (mine place)
      3 → DetonatePlayerMines
      4 → CWeapon_FireMachineGunBurstStart

  if CBulanek_IsHumanPlayer(player):
    CGame_NetSendPlayerEvent1_t0e(player.pGame, player.bPlayerSlot)
      // 2 bytes: [0x0E, slot] — weapon kind implicit on all peers
  return
```

### Disasm proof (`0x004208c0`–`0x00420907`, 22 insns)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x004208c3` | `TEST byte [ESI+0x44], 1` | `view_flags` alive bit |
| `0x004208cf` | `PUSH 0` + `CALL 0x0042f1e0` | `Scheduler_GetEventSlot(weaponSched, 0)` |
| `0x004208d9` | `TEST byte [EAX+0x8], 1` | Weapon scheduler slot **0** armed |
| `0x004208e5` | `CALL 0x00420650` | `DispatchCurrentWeaponAction` |
| `0x004208ec` | `CALL 0x00416720` | `IsHumanPlayer` |
| `0x004208f1` | `TEST AL, AL` | Branch net send |
| `0x00420900` | `CALL 0x00412b60` | `CGame_NetSendPlayerEvent1_t0e(pGame, bPlayerSlot)` |

### Scheduler slot **1** — AI delayed fire path

Slot **1** on `CBulanek.scheduler` @ `+0x88` is **not** registered in ctor for humans (only slot **0** + conditional 2..5 per `slotKind` — task 01). It is armed at runtime, then dispatches **primary fire**:

```
Arm slot 1 (examples):
  CBulanek_OnEvent case 0xF2 @ 0x00420f2:
    after TryApplyPickupMask + SetFacingTrack success:
      if IsInKnockdownAnimBand:
        delay_ms = round((1.0 / (nSpeedParam/100.0)) * 200.0)
      else:
        delay_ms = RandInRange(0,3) * 500
      Scheduler_RegisterEventSlot(&scheduler, slot=1, delay_ms, eventKind=2)

  CBulanek_WeaponSchedulerCallback case 4 @ 0x00420c8b:
    after pickup mask + SetFacingTrack:
      delay_ms = round((1.0 / (nSpeedParam/100.0)) * 200.0)
      Scheduler_RegisterEventSlot(..., slot=1, delay_ms, eventKind=2)

Fire slot 1:
  CBulanek_WeaponSchedulerCallback @ 0x00420b30, case 1 @ 0x00420bb9:
    CBulanek_TriggerPrimaryActionAndBroadcast(player)
```

**Human keyboard fire** never waits on slot 1 — `CGame_DispatchPlayerAction` → `ApplyAction(4, press)` → direct call @ `0x00420933`.

**Gating alignment:** `CBulanek_CanDispatchPlayerAction` @ `0x004174a0` requires the **same** weapon scheduler slot **0** armed bit for action **4** press before `ApplyAction` is reached — so keyboard and scheduler paths share the weapon cooldown gate.

### Human vs AI — `IsHumanPlayer` @ `0x00416720`

```c
if (player.bPlayerSlot < 4)
  return CGame_IsLocalPlayerSlot(player.pGame, player.bPlayerSlot);
return (*(char*)(player.pGame + 0x36) == 0);
```

| Entity | `DispatchCurrentWeaponAction` | `NetSendPlayerEvent1_t0e` |
|--------|------------------------------|---------------------------|
| Local human `0..3` | yes | **yes** — originator broadcasts `0x0E` |
| Remote human (recv `0x0E`) | yes | **no** — already remote-driven |
| Campaign AI `0x7d..` / co-op `0x20..` on host | yes | **no** — `bPlayerSlot ≥ 4` branch |
| Practice dummy `0x24..` | yes (if gates pass) | **no** |

### Network (`NET_MSG_0x0E`)

| Direction | Function | Payload |
|-----------|----------|---------|
| **Send** | `CGame_NetSendPlayerEvent1_t0e` @ `0x00412b60` | `[0x0E, bPlayerSlot]` — 2 bytes |
| **Recv** | `CGame_OnNetMsg_t0e_PrimaryAction` @ `0x00420a70` | `CGaming_GetObjectAtSlotUnchecked(cg, slot)` → `TriggerPrimaryActionAndBroadcast` |

Peers apply weapon via `bWeaponKind` already mirrored (`0x0D` / state sync) — no weapon kind in `0x0E` packet ([`net_protocol.md`](../../netcode/net_protocol.md)).

## Functions table

| Symbol | Address | Role |
|--------|---------|------|
| `CBulanek_TriggerPrimaryActionAndBroadcast` | `0x004208c0` | **Subject** — gate + dispatch + optional net |
| `CBulanek_DispatchCurrentWeaponAction` | `0x00420650` | `switch(bWeaponKind)` weapon effects |
| `CBulanek_IsHumanPlayer` | `0x00416720` | Net-send gate |
| `CGame_NetSendPlayerEvent1_t0e` | `0x00412b60` | Send `0x0E` |
| `CGame_OnNetMsg_t0e_PrimaryAction` | `0x00420a70` | Recv → trigger |
| `CBulanek_ApplyAction` | `0x00420910` | Caller — action 4 press @ `0x00420933` |
| `CBulanek_WeaponSchedulerCallback` | `0x00420b30` | Caller — case 1 @ `0x00420bb9` |
| `Scheduler_GetEventSlot` | `0x0042f1e0` | Weapon sched slot 0 poll |
| `CBulanek_CanDispatchPlayerAction` | `0x004174a0` | Pre-gate for keyboard path (action 4) |

### Caller xrefs (`get_xrefs_to 0x004208c0`)

| From | Function | Context |
|------|----------|---------|
| `0x00420933` | `CBulanek_ApplyAction` | `actionIndex == 4`, press branch |
| `0x00420a7c` | `CGame_OnNetMsg_t0e_PrimaryAction` | Remote primary action |
| `0x00420bb9` | `CBulanek_WeaponSchedulerCallback` | **Scheduler slot 1** fire |

## Struct fields

| Offset | Field | Use |
|--------|-------|-----|
| `+0x44` | `view_flags` | Bit **0** must be set (alive) — `TEST [ESI+0x44],1` |
| `+0x70` | `bPlayerSlot` | Net `0x0E` slot byte |
| `+0x88` | `scheduler` | Slot **1** armed by `OnEvent 0xF2` / callback case 4; fires → case 1 |
| `+0xF4` | `pGame` | Net send `this` |
| `+0xF8` | `pWeapon` | Dispatch target; sched @ `pWeapon+0x0C` |
| `+0x64` (on `CWeapon`) | `bWeaponKind` | `DispatchCurrentWeaponAction` switch |

## Ghidra deltas

None this pass — prior net/combat pass already renamed @ `0x004208c0` and commented `DispatchCurrentWeaponAction` @ `0x00420650`.

## Decomp fixes (IDA vs Ghidra)

| Topic | IDA (`sub_4208C0`) | Ghidra | Resolution |
|-------|-------------------|--------|------------|
| Calling convention | `__thiscall` | `__fastcall` global | Same body; MSVC member/static lowering — use `CBulanek*` fastcall |
| Alive gate offset | `this+68` (dec) | `param_1+0x44` | **Match** (`0x44`) |
| Weapon sched embed | `*(this+248)+12` | `*(param_1+0xf8)+0xc` | **Match** (`0xF8` + `0x0C`) |
| Net game ptr | `this+244` | `param_1+0xf4` | **Match** |
| Manifest addr `0x420721` | N/A | `CGaming_OnCustomEvent` body | **Wrong seed** for dispatch — use `0x420650` |

## Frida

Not run — static xref + disasm + IDA export sufficient.

## Remaining UNK

| Item | Notes |
|------|-------|
| `eventKind=2` vs `7` on slot 1 registration | Slot 1 arms use flags **2** (`OnEvent 0xF2`, callback case 4); ctor slots use **7**. Dispatch still lands in `WeaponSchedulerCallback` by slot index — exact `CDSUpdatedItem` routing deferred to scheduler internals doc. |
| Post-fire slot 0 re-arm | Which weapon anim path re-arms weapon `trackManager.scheduler` slot 0 after fire — weapon `Fire` / `TM_Play` chain (task 23 / 24). |
| Practice dummy fire | Stationary targets may never arm slot 1; direct `ApplyAction(4)` path untested in-game. |

## Evidence paths

- [`ROUND11_AI_PROTOCOL.md`](../ROUND11_AI_PROTOCOL.md), [`GHIDRA_MCP.md`](../GHIDRA_MCP.md)
- [`player_controls.md`](../player_controls.md) § action 4
- [`CBulanek.md`](../struct_recovery/CBulanek.md), [`CWeapon.md`](../struct_recovery/CWeapon.md)
- [`net_protocol.md`](../../netcode/net_protocol.md) § `0x0E`
- [`r11_ai_task_01_report.md`](./r11_ai_task_01_report.md) (scheduler slot taxonomy), [`r11_ai_task_04_report.md`](./r11_ai_task_04_report.md) (ApplyAction action 4)
- `bulanci/bulanci.ida.exe.c` (`sub_4208C0`, `sub_420B30` case 1)
- `bulanci/bulanci.ghidra.exe.c` @ ~90798
- `config/bulanci/mapping.csv` (`CBulanek_TriggerPrimaryActionAndBroadcast;0x4208c0;0x47`)
