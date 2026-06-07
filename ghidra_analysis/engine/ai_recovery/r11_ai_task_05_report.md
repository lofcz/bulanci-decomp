# Round 11 — AI Task 05 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **title** | `CBulanek_ArmFireDelayScheduler` — post-fire delay ms |
| **seed_address** | `0x00417260` |
| **archetype** | shooting_ai (campaign / generic AI `CBulanek`, not humans / coop / dummies) |
| **acceptance** | Exact `_rand` → ms formula for scheduler slot **2**; all callers |

## Status

**DONE** — Live Ghidra MCP decompile + disasm prove MSVC mul-shift scaling, millisecond multiply, scheduler slot index, slot-kind guard, and three static callers. Python range check confirms **11** distinct delays (5.0–15.0 s, 1 s steps).

## AI archetype

| Slots excluded | Reason |
|----------------|--------|
| `0x00..0x03` | Human players |
| `0x20..0x23` | Co-op vampire allies |
| `0x24..0x27` | Stationary practice dummies |

All other `bPlayerSlot > 3` AI (`0x04..0x1f`, `0x28..0x63`, campaign `0x7d..`, etc.) receive the random fire-delay arm when a caller invokes this helper.

## Algorithm

### Slot guard (`0x00417260`–`0x00417275`)

```c
byte slot = entity->bPlayerSlot;  // CBulanek+0x70
if (slot <= 3) return;
if (slot >= 0x20 && slot < 0x24) return;  // coop
if (slot >= 0x24 && slot < 0x28) return;  // dummies
// else arm delay
```

Disasm: `CMP AL,3 / JBE ret`; `CMP AL,0x20 / JC arm`; `CMP AL,0x24 / JC ret`; `CMP AL,0x24 / JC arm`; `CMP AL,0x28 / JC ret`.

### Exact delay formula (`0x0041727e`–`0x00417295`)

```c
// MSVC LCG _rand @ 0x004477ec: returns (holdrand>>16) & 0x7fff  => 0..32767
int r = _rand();
int k = (int)(((unsigned)r * 11u) >> 15);   // asm: IMUL 0xb; CDQ; AND EDX,0x7fff; ADD; SAR 0xf
int delay_sec = k + 5;                       // asm: ADD EAX,5
uint delay_ms = (uint)delay_sec * 1000u;     // asm: IMUL EAX,0x3e8

Scheduler_SetEventDelayMs(&entity->scheduler, /*slot=*/2, delay_ms);   // 0x0042f2d0
Scheduler_SetEventLastFireMs(&entity->scheduler, 2, -1);               // 0x0042f290 — arm from epoch
if (Scheduler_GetEventSlot(&entity->scheduler, 2)->flags & 1)
    Scheduler_AckSlot(&entity->scheduler, 2, -1);                      // 0x0042f330 — re-arm if already live
```

**Proven range:** `k ∈ {0..10}` → `delay_sec ∈ {5..15}` → **`delay_ms ∈ {5000, 6000, …, 15000}`** (verified by exhaustive simulation over `r ∈ [0,32767]`).

**Note:** The multiply-shift idiom is MSVC’s standard lowering of `_rand() % 11` (biased toward low values vs uniform mod); reimplementation should use the same `((unsigned)r * 11) >> 15` form for byte match, not `r % 11` with a different RNG scale.

### Scheduler registration (ctor context)

`CBulanekCtor@0x0041e4b0` registers scheduler slot **2** with `delayMs=0`, `eventKind=7` for non-human / non-coop / non-dummy AI (`uVar9=2` branch). `ArmFireDelayScheduler` only **reprograms** the delay on slot 2; it does not register the slot.

### Slot 2 dispatch (downstream, proven static)

`CBulanek_WeaponSchedulerCallback@0x00420b30` **case 2** (`0x00420bc6`): if **weapon** `Scheduler_GetEventSlot(weapon, 0)` has `+0x8` bit0 set (armed), calls `CBulanek_OnTakeDamage(this, -1, -1, 0)`. Ties fire-delay expiry to the weapon scheduler slot-0 armed gate.

## Functions table

| Address | Symbol | Role |
|---------|--------|------|
| `0x00417260` | `_Globals::CBulanek_ArmFireDelayScheduler` | **Seed.** AI-only guard + `_rand` scaling + arm scheduler slot 2 |
| `0x004477ec` | `Runtime::MSVCRT::_rand` | LCG; return `0..32767` |
| `0x0042f2d0` | `CDSUpdatedItem::Scheduler_SetEventDelayMs` | Writes `slot[+4] = delay_ms` |
| `0x0042f290` | `CDSUpdatedItem::Scheduler_SetEventLastFireMs` | Stamps last-fire (`-1` = fire immediately on next due check) |
| `0x0042f1e0` | `CDSUpdatedItem::Scheduler_GetEventSlot` | Heap slot node pointer |
| `0x0042f330` | `CDSUpdatedItem::Scheduler_AckSlot` | Clears armed bit / recycles slot |
| `0x00417380` | `CBulanek::CBulanek_OnShow` | After team palette rebuild when shown |
| `0x0041db00` | `CBulanek::CBulanek_OnTakeDamage` | After successful hit-stun path |
| `0x0041f770` | `_Globals::CGaming_RespawnPlayer` | Local-authority respawn branch (`param_1==0`, AI slot) |
| `0x00420b30` | `CBulanek::CBulanek_WeaponSchedulerCallback` | `eventKind=7` handler; **case 2** consumes slot-2 expiry |
| `0x0041e4b0` | `CBulanek::CBulanekCtor` | Registers slot 2 (`eventKind=7`) for eligible AI archetypes |

### Callers (`get_function_callers@0x00417260`, live MCP)

| Caller | Call site context |
|--------|-------------------|
| `CBulanek_OnShow` | `param_1!=0` visibility hook: palette LUT + `ArmFireDelayScheduler` + `ArmTournamentSchedulerDelays` |
| `CBulanek_OnTakeDamage` | After pain SFX + `bHitStun=1` when teleport-pair creation succeeds |
| `CGaming_RespawnPlayer` | Local respawn (`param_1==0`) after safe spawn + net `0x10`; skipped for remote packet respawn |

**Not a caller:** `CBulanekCtor` does not call `ArmFireDelayScheduler` directly (prior round5 note was imprecise).

## Struct fields

| Offset | Field | Use in this function |
|--------|-------|----------------------|
| `CBulanek+0x70` | `bPlayerSlot` | Archetype guard |
| `CBulanek+0x88` | `scheduler` (`CDSUpdatedItem`) | Target facet; slot index **2** |
| Slot heap `+0x4` | `delayMs` | Written by `Scheduler_SetEventDelayMs` |
| Slot heap `+0x8` bit0 | armed flag | Tested before `Scheduler_AckSlot` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00417260` | Corrected slot guard + **5000..15000 ms** range + callers |
| `set_decompiler_comment` | `0x0041727e` | Documented mul-shift `% 11` idiom and `*0x3e8` |
| `save_program` | `bulanci.exe` | Saved |

## Decomp fixes

| Issue | Before | After (proven) |
|-------|--------|----------------|
| Delay range comment | “5–16 s” / vague `%11` | **5–15 s** (11 values); formula `(((unsigned)r*11)>>15)+5)*1000` |
| `% 11` semantics | Treated as Euclidean mod | MSVC biased mul-shift on 15-bit `_rand` output |

## Frida

**none** — formula fully proven from disasm (`IMUL 0xb`, `SAR 0xf`, `ADD 5`, `IMUL 0x3e8`) and exhaustive integer range check.

## Remaining UNK

1. Whether gameplay intent is “post-damage refractory” vs “post-show AI think delay” (call-site semantics only; formula closed).
2. Exact weapon scheduler slot-0 armed precondition in case 2 (static proof only; no runtime trace).
