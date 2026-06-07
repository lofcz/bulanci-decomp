# Round 11 — AI Task 19 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 19 |
| **title** | Deep spawn helper @0x0041f230 |
| **archetype** | spawn |
| **seed_address** | `0x0041f230` |
| **addresses** | `0x0041f230` |
| **acceptance** | All callers; slot assignment; AddEntity path; initialTrack/facing |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`): full decompile + disasm for `CGaming_SpawnBulletAndPlaySound`; **program-wide `CALL 0x0041f230` scan** closes xref set at **2 sites** (script opcodes only). **Critical correction:** this address is the **`CShot` projectile factory + SFX helper**, not `CGaming_SpawnAndInitializePlayer` (`0x0041f500`). Script opcodes named `SpawnEnemyAt` / `SpawnOpponentEx` spawn **projectiles** at script coordinates with an **owner slot byte** on the shot — they do **not** allocate `CBulanek` or call `CGaming_RegisterObjectAtSlot`.

## AI archetype

| Context | Role |
|---------|------|
| **Script hazard / ambush fire** | Level bytecode drives aimed shots near moving views (e.g. slot **9** parent in `res_0000065856` / `65859`) |
| **Not player-like AI spawn** | Campaign / practice / coop **characters** use `CGaming_SpawnAndInitializePlayer` (task 12) or `CGaming_SpawnPracticeDummy` |

## Algorithm

### `CGaming_SpawnBulletAndPlaySound` @ `0x0041f230`

**Signature (live Ghidra / `mapping.csv`):**

```c
uchar __thiscall CGaming_SpawnBulletAndPlaySound(
    CGaming *this,
    int *pPosition,      // world x,y int[2]
    uchar kind,          // → CShot_Ctor bDirection (+0xA4)
    uchar ownerSlot,     // → CShot_Ctor bOwnerSlotId (+0xA5); 0xFF = neutral
    uchar flags,         // low 2 bits: spawn+strength; high nibble: SFX class
    int *pAttenuationRef,// parent entity+0x20 or NULL → ComputeSpatialAttenuationDb100
    int frameLookup);    // CShot_Ctor param_6 (bullet frame strip id)
```

**Pseudocode (disasm `0x0041f230`–`0x0041f346`):**

```
CDSView__PostMessage(this+0x10, 0x200, 0xF4, 0, 0)     // @ 0x0041f267

if ((flags & 3) != 0) {
  shot = OperatorNewWithBadAlloc(0xB0)                   // @ 0x0041f277
  if (shot)
    CShot_Ctor(
      shot,
      pPosition,
      this,
      kind,                                              // bDirection
      ownerSlot,                                         // bOwnerSlotId
      (flags & 3) - 1,                                   // bWeaponStrength 0..2
      frameLookup);
  CIntListInsertSortedOrAppend(this+0x2C8, shot, …, 1) // bullet list @ CGaming+0x2C8
  CGaming_AddEntity(this, shot, register_flag = 1)       // @ 0x0041f2d7 — depth insert mode
}

switch (flags & 0xF0) {
  case 0x10: sampleId = 0x15; break;   // pistol shot SFX
  case 0x20: sampleId = 0x0D; break;   // shotgun SFX
  case 0x40: sampleId = 0x13; break;   // dry-fire click
  default:   return;                   // no audio
}
attenDb = ComputeSpatialAttenuationDb100(pAttenuationRef)  // uses midpoint x of RECT@+0x20
TriggerBankSample(1, 0, sampleId, attenDb, …)
CDSAudioPlayer_PlayAndRelease(…)
```

**`flags` low 2 bits → projectile (proven @ `0x0041f272`–`0x0041f2a1`):**

| `flags & 3` | `bWeaponStrength` (`+0xA6`) | Effect |
|-------------|----------------------------|--------|
| `0` | — | **No** `CShot` alloc; SFX branch may still run if high nibble set |
| `1` | `0` | Single pellet / basic shot |
| `2` | `1` | Multi-pellet (`bPelletMask` loop) |
| `3` | `2` | Explosive rocket path in `CShot_ResolveHit` |

### Caller argument wiring

#### `CLevelScriptOpExt_SpawnEnemyAt` @ `0x0041f610` → call @ `0x0041f687`

Bytecode reads **5** sub-expressions (`script_dispatch_table` opcode **67**):

| Read order | Script role | Helper arg |
|------------|-------------|------------|
| 1 | `x` | `pPosition[0]` (stack local) |
| 2 | `y` | `pPosition[1]` |
| 3 | `kind` | `kind` → shot direction |
| 4 | `parentSlot` | `ownerSlot`; if `0xFF` → `pAttenuationRef = NULL` else `GetObjectAtSlotSafe(slot)+0x20` |
| 5 | `flags` | `flags` |
| — | — | `frameLookup = 0` (`PUSH 0` @ `0x0041f672`) |

**Corpus example** (`res_0000065856`):

```
SpawnEnemyAt(x+65, y+10, kind=1, parentSlot=9, flags=1)
```

→ `CShot` at computed world pos, `bDirection=1`, `bOwnerSlotId=9`, `bWeaponStrength=0`.

#### `CLevelScriptOpExt_SpawnOpponentEx` @ `0x0041f6a0` → call @ `0x0041f720`

Same as above plus **6th** sub-expression → `frameLookup` (`extraout_EAX` / `PUSH EDI` @ `0x0041f70c`).

**Corpus example** (`res_0000065859`):

```
SpawnOpponentEx(221, 136, kind=3, parentSlot=9, flags=2, GetGlobalVar(12))
```

→ `bWeaponStrength = 1` (`(2&3)-1`); frame strip from global-12 CollInsert skin pool.

### Slot assignment (proven negative + positive)

| Mechanism | Used here? | Detail |
|-----------|------------|--------|
| `CGaming_RegisterObjectAtSlot` (`apEntitySlots@+0xC8`) | **NO** | Disasm: no `CALL 0x00417fb0` in body |
| `CShot.bOwnerSlotId` @ `+0xA5` | **YES** | Ctor stores `ownerSlot` param — **shooter attribution**, not registry index |
| `CShot.bPlayerSlot` @ `+0x70` | default `0xFF` | View-shell init; distinct from `+0xA5` |
| `CGaming_AddEntity` | **YES** | `register_flag=1` → `CGaming_SetEntityRegisterMode` + depth-sorted child insert |

**Contrast — character spawn (task 12):** `CGaming_SpawnAndInitializePlayer` calls **both** `AddEntity(-1)` and `RegisterObjectAtSlot(slotKind)` plus `CBulanek_SetFacingTrack` from `_rand()`.

### initialTrack / facing

| Field | `CGaming_SpawnBulletAndPlaySound` | `CGaming_SpawnAndInitializePlayer` |
|-------|-----------------------------------|-------------------------------------|
| `initialTrack` | **N/A** | hardcoded `0` in ctor call |
| Facing | **`kind` → `CShot.bDirection` (+0xA4)** only | `_rand()%4` → `CBulanek_SetFacingTrack` |

No `CBulanek` scheduler or `videoTrackManager` involvement in this helper.

### Weapon-fire path (not a caller)

`CWeapon_Fire` @ `0x00421300` **inlines** `OperatorNew(0xB0)` + `CShot_Ctor` + `CGaming_AddEntity` for weapon kinds 4 and 5 — **does not** `CALL 0x0041f230` (`search_instructions` mnemonic `CALL` operand `0x0041f230` → **2 matches total**). Inlined path **skips** bullet-list insert (`+0x2C8`) and this helper's SFX dispatch.

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041f230` | `CGaming_SpawnBulletAndPlaySound` | `CShot` factory + bullet-list + optional bank SFX | Decompile; disasm `PUSH 0xB0` @ `0x0041f277`, `CALL 0x0041edf0`, `CALL 0x0041a390` @ `0x0041f2d7` |
| `0x0041edf0` | `CShot_Ctor` | Alloc `0xB0` entity; sets `+0xA4` direction, `+0xA5` owner slot, `+0xA6` strength | Decompile |
| `0x0041a390` | `CGaming_AddEntity` | View attach; `shot+0x84 = CGaming*`; mode from `+0x341` | R10 task 03 |
| `0x00407e20` | `CIntListInsertSortedOrAppend` | Active bullet tracking list `CGaming+0x2C8` | Call @ `0x0041f2cd` |
| `0x00422500` | `ComputeSpatialAttenuationDb100` | Stereo attenuation from parent `RECT+0x20` midpoint | Call @ `0x0041f313` |
| `0x00422430` | `TriggerBankSample` | Audio bank sample trigger | Call @ `0x0041f321` |
| `0x0041f610` | `CLevelScriptOpExt_SpawnEnemyAt` | Opcode 67 — 5-arg script wrapper | `CALL 0x0041f230` @ `0x0041f687` |
| `0x0041f6a0` | `CLevelScriptOpExt_SpawnOpponentEx` | Opcode 98 — 6-arg script wrapper | `CALL 0x0041f230` @ `0x0041f720` |
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | **Separate** — real `CBulanek` spawn (task 12) | No xref to `0x0041f230` |
| `0x00417fb0` | `CGaming_RegisterObjectAtSlot` | Slot table — **not used** by this helper | Absent from `0x0041f230` body |

## Struct fields

| Offset (CShot) | Name | Set by this helper |
|----------------|------|--------------------|
| `+0x20` | `pWorldRect` | From `pPosition` in ctor |
| `+0x84` | `pGaming_host` | `CGaming_AddEntity` |
| `+0xA4` | `bDirection` | `kind` arg |
| `+0xA5` | `bOwnerSlotId` | `ownerSlot` arg (script parent slot) |
| `+0xA6` | `bWeaponStrength` | `(flags & 3) - 1` |
| `+0xA0` | `pBulletFrames` | `CGameView_LookupBulletFrameStrip(..., frameLookup, strength)` |

| Offset (CGaming) | Name | Used |
|------------------|------|------|
| `+0x2C8` | bullet int-list head | `CIntListInsertSortedOrAppend` |
| `+0x341` | `bEntityRegisterMode` | Preset to `1` via AddEntity arg |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0041f230` | Clarify CShot helper; only 2 script callers; not CBulanek spawn |
| `set_decompiler_comment` | `0x0041f687` | SpawnEnemyAt 5-arg → helper arg map |
| `set_decompiler_comment` | `0x0041f720` | SpawnOpponentEx 6th arg → frameLookup |
| `set_decompiler_comment` | `0x0041f2d7` | AddEntity without RegisterObjectAtSlot |
| `save_program` | `bulanci.exe` | Saved |

## Decomp fixes

| Issue | Ground truth | Notes |
|-------|--------------|-------|
| Task title "spawn helper" vs symbol | Live symbol = **`CGaming_SpawnBulletAndPlaySound`** | `mapping.csv` `0x41f230`; size `0x117` |
| `script_dispatch_table` "shares helper with InsertOpponent" | **False for `0x0041f230`** | `InsertOpponent` → `CGaming_SpawnPracticeDummy` → `CGaming_SpawnAndInitializePlayer` |
| R6 "CWeapon_Fire calls `0x0041f230`" | **Superseded** | `search_instructions CALL 0x0041f230` → 2 sites only; weapon inlines ctor |
| SpawnEnemyAt "enemy character" | Script spawns **`CShot`** when `flags&3 != 0` | Disasm + ctor alloc `0xB0` |

## Frida

**none** — static closure sufficient. Optional hook: `0x0041f230` entry log `{pPosition, kind, ownerSlot, flags, frameLookup}` on script-heavy maps (`65856`, `65859`).

## Remaining UNK

- `CDSView__PostMessage(..., 0x200, 0xF4, …)` purpose — sole site in binary; may gate render/audio (not proven).
- Whether `flags&3==0` script calls intend **audio-only** (high nibble) with no projectile — no `65856`/`65859` example with `flags` low bits clear yet observed in corpus.
- Full semantic map of `kind` byte vs `CGameView_LookupBulletFrameStrip` for script-driven shots (weapon kind table `DAT_004829b0` family used by `CWeapon_Fire`).
