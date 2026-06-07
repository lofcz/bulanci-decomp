# Round 11 — AI Task 06 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 6 |
| **title** | `CBulanek_ArmTournamentSchedulerDelays` — slots 3/4 |
| **seed_address** | `0x004172d0` |
| **addresses** | `0x004172d0` |
| **acceptance** | Prove `slotKind` `0x20..0x27` branch; `_rand` delay scaling; callers `WeaponSchedulerCallback` case 3 |

## Status

**DONE** — Gate on `bPlayerSlot@+0x70` in `0x20..0x23` ∪ `0x24..0x27`, scheduler slot **3** random **1..10 s**, slot **4** fixed **2000 ms**, both callers, and ctor slot registration cross-checked via live Ghidra decompile + disasm (`0x004172d0`–`0x00417374`, `0x00420bf6`–`0x00420c23`, `0x004173d1`–`0x004173d8`). No Ghidra mutations required (symbol named R5).

## AI archetype

| Slots (hex) | Role | Ctor branch | This function |
|-------------|------|-------------|---------------|
| `0x20..0x23` | Co-op / **InsertVampires** allies | `bVar2 = (slotKind-0x20)<4` → registers scheduler slots **3**, **4** (delay 0), **5** (delay 100 ms) | Re-arms slots **3** / **4** when shown or after bot tick |
| `0x24..0x27` | Solo **practice dummies** | `bVar3 = (slotKind-0x24)<4` → registers slots **3**, **4** (delay 0) | Same re-arm path |

**Inverse:** `CBulanek_ArmFireDelayScheduler@0x00417260` arms scheduler slot **2** only when `bPlayerSlot` is **outside** `0x20..0x27` (campaign AI `0x7d..0x7f`, etc.). Tournament targets never get the 5–16 s fire-delay slot.

Shared helper `CBulanek_IsInKnockdownAnimBand@0x00416490` uses the **same** `+0x70` range test (`0x20..0x27`) — name is historical; it gates tournament-target AI, not anim-band alone.

## Algorithm

Pseudocode (proven constants from disasm):

```c
// __fastcall; ECX = CBulanek*
uchar CBulanek_ArmTournamentSchedulerDelays(CBulanek* self) {
    byte slot = self->bPlayerSlot;  // +0x70

    // 0x004172d3..0x004172e5: two half-open ranges
    bool isCoop   = (slot >= 0x20 && slot < 0x24);
    bool isDummy  = (slot >= 0x24 && slot < 0x28);
    if (!isCoop && !isDummy)
        return 0;

    CDSUpdatedItem* sched = &self->scheduler;  // +0x88, LEA [ECX+0x88] @ 0x004172ec

    // Slot 3 — random think delay 1000..10000 ms (1 s steps)
    int r = _rand();  // CALL 0x004477ec @ 0x004172f2
    // LEA EAX,[EAX+EAX*4]; ADD EAX,EAX  => r*10
    // SAR 15 after sign fixup => (r*10)/32768 == r%10 for r in 0..32767
    int delay3_ms = (((r * 10) >> 15) + 1) * 1000;  // IMUL 0x3e8 @ 0x0041730b

    Scheduler_SetEventDelayMs(sched, 3, delay3_ms);      // 0x0042f2d0
    Scheduler_SetEventLastFireMs(sched, 3, -1);          // 0x0042f290
    if (Scheduler_GetEventSlot(sched, 3)->armed & 1)      // TEST [EAX+8],1
        Scheduler_AckSlot(sched, 3, -1);                 // 0x0042f330

    // Slot 4 — fixed 2000 ms (PUSH 0x7d0 @ 0x00417340)
    Scheduler_SetEventDelayMs(sched, 4, 2000);
    Scheduler_SetEventLastFireMs(sched, 4, -1);
    if (Scheduler_GetEventSlot(sched, 4)->armed & 1)
        Scheduler_AckSlot(sched, 4, -1);

    return 1;  // implicit; no explicit return value used by callers
}
```

### `_rand` scaling (slot 3)

| Step | Disasm | Formula |
|------|--------|---------|
| Multiply | `LEA EAX,[EAX+EAX*4]; ADD EAX,EAX` @ `0x004172f7`–`0x004172fa` | `r * 10` |
| Mod 32768 | `CDQ; AND EDX,0x7fff; ADD; SAR 15` @ `0x004172fc`–`0x00417305` | `(r*10)/32768` → **0..9** |
| Bias + scale | `ADD EAX,1; IMUL EAX,0x3e8` @ `0x00417308`–`0x0041730b` | **(1..10) × 1000 ms** |

MSVCRT LCG and `SAR 0xf` pattern documented in [round5_worker_03_report.md](../struct_recovery/round5_worker_03_report.md).

### Scheduler slot semantics (downstream)

| Slot | Set by this fn | `WeaponSchedulerCallback` case | Behavior |
|------|----------------|--------------------------------|----------|
| **3** | Random 1–10 s | **3** @ `0x00420bf6` | Requires **weapon** `trackManager.scheduler` slot **0** armed; `IsInKnockdownAnimBand` → `TryBotRandomAction` → **this fn** (re-arm) |
| **4** | Fixed 2000 ms | **4** @ `0x00420c30` | Scans `pActiveAnim` chain (up to 4) for pickup mask `& 0xf`; on hit → `SetFacingTrack` + arm slot **1** (primary fire) |

Ctor seeds both slots at **delay 0** for `0x20..0x27` (`CBulanekCtor@0x0041ea18` region); this function is the **runtime re-arm** after visibility or bot action.

### Call graph

```
CBulanek_OnShow(param_1!=0) @ 0x004173d8
  after CGame_BuildTeamPaletteLut + CBulanek_ArmFireDelayScheduler

CBulanek_WeaponSchedulerCallback case 3 @ 0x00420c23
  when weapon scheduler slot0 armed
    && CBulanek_IsInKnockdownAnimBand (slot 0x20..0x27)
    → CBulanek_TryBotRandomAction
    → CBulanek_ArmTournamentSchedulerDelays
```

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x004172d0` | `CBulanek_ArmTournamentSchedulerDelays` | **Seed.** Tournament-target scheduler re-arm (slots 3/4) | Decompile; disasm `0x004172d0`–`0x00417374` |
| `0x00417260` | `CBulanek_ArmFireDelayScheduler` | Complementary: slot **2** for non-`0x20..0x27` | Inverse gate disasm `0x00417266`–`0x0041727b` |
| `0x00416490` | `CBulanek_IsInKnockdownAnimBand` | Same `+0x70` range gate as seed | Decompile; case 3 @ `0x00420c11` |
| `0x00420a90` | `CBulanek_TryBotRandomAction` | Random idle / action 0..3 before re-arm | CALL @ `0x00420c1c` |
| `0x00420b30` | `CBulanek_WeaponSchedulerCallback` | Dispatches scheduler slots 0..5 (`eventKind` 7) | Switch table `0x00420d24`; case 3 → seed |
| `0x00417380` | `CBulanek_OnShow` | Visibility: palette + fire-delay + tournament delays | CALL @ `0x004173d8` |
| `0x0041e4b0` | `CBulanekCtor` | Registers slots 3/4 (and 5 for coop) at spawn | Decompile `bVar2`/`bVar3` branches |
| `0x004477ec` | `Runtime::MSVCRT::_rand` | LCG `0x7fff` max | CALL @ `0x004172f2` |
| `0x0042f2d0` | `Scheduler_SetEventDelayMs` | Writes delay @ slot+8 region | CALL @ `0x00417316`, `0x00417349` |
| `0x0042f290` | `Scheduler_SetEventLastFireMs` | `lastFire = -1` reset | CALL @ `0x00417321`, `0x00417354` |
| `0x0042f1e0` | `Scheduler_GetEventSlot` | Slot pointer for armed test | CALL @ `0x0041732a`, `0x0041735d` |
| `0x0042f330` | `Scheduler_AckSlot` | Ack if already armed | CALL @ `0x0041733b`, `0x0041736e` |

## Struct fields

| Offset | Name | Use in this function |
|--------|------|----------------------|
| `+0x70` | `bPlayerSlot` | Gate: must be `0x20..0x23` or `0x24..0x27` (`MOV AL,[ECX+0x70]` @ `0x004172d0`) |
| `+0x88` | `scheduler` (`CDSUpdatedItem`) | `LEA ESI,[ECX+0x88]` — target for slots 3 and 4 |
| `+0x124` | `bSlotKind` | Ctor spawn kind (`0x20..0x27` branches); runtime gate uses **`bPlayerSlot`** not `bSlotKind` |

## Ghidra deltas

**None** — function already named and commented (R5 worker 02). No `save_program`.

## Decomp fixes

| Issue | Resolution |
|-------|------------|
| Seed shows `param_1` instead of `this` | Known `__fastcall` artifact; `ECX` = `CBulanek*` (disasm `MOV AL,[ECX+0x70]`) |
| `WeaponSchedulerCallback` case 3 decompiler uses `bPlayerSlot` offset noise | Disasm proves: `[ESI+0x70]` = `pWeapon`, `[pWeapon+0xc]` = weapon scheduler slot 0 armed check |

## Frida

**Not required** — all branches closed statically.

## Remaining UNK

- Exact level-design intent of **2000 ms** slot 4 vs ctor **0 ms** initial (pickup poll rate tuning only; behavior proven).
- Whether practice dummies ever reach case 3 (`TryBotRandomAction` also requires `IsHumanPlayer` — task **7**); dummies still get slot re-arm from **`OnShow`** unconditionally when visible.
