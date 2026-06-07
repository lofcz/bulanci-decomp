# Round 11 — AI Task 18 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 18 |
| **title** | SpawnEnemyAt — 5-arg Rand spawn |
| **archetype** | `script_spawn` |
| **seed_address** | `0x0041f610` |
| **related** | `0x0041f6a0` (`SpawnOpponentEx`), `0x0041f230` (`CGaming_SpawnBulletAndPlaySound`) |
| **acceptance** | Diff vs `SpawnOpponentEx`; position/state args; shared helper `0x0041f230` |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`) + overlay script correlation (`65856`, `65857`, `65859`). Opcode semantics proven from disasm; **not** CBulanek campaign-AI spawn.

## AI archetype

| Label | Actual engine behavior |
|-------|------------------------|
| Editor name `SpawnEnemyAt` | **Script-fired `CShot` spawn** at world `(x,y)` with facing/direction byte, owner slot for audio + hit attribution, and weapon-strength/SFX flags |
| Misleading “enemy” | Does **not** call `CGaming_SpawnAndInitializePlayer`, `InsertOpponent`, or allocate `CBulanek`. Same pipeline as weapon fire (`CShot` @ `0xB0`). |

Used for level hazards that **look** like emerging opponents (mimic ambush, 8-bit zombie peek) but are implemented as short-lived shot entities with custom frame strips.

## Algorithm

### `CLevelScriptOpExt_SpawnEnemyAt` (`0x0041f610`)

```
SpawnEnemyAt(x, y, direction, ownerSlot, flags):
  x, y, direction, ownerSlot, flags = CDSScript::ReadSubExpr() × 5   // stack locals [x,y] at ESP+0x10/+0x14
  if ownerSlot == 0xFF:
    audioPos = NULL
    ownerSlotForHelper = 0xFF
  else:
    view = CGaming_GetObjectAtSlotSafe(pGaming, ownerSlot)   // 0x00416810
    audioPos = view + 0x20                                   // world rect origin for attenuation
    ownerSlotForHelper = ownerSlot
  CGaming_SpawnBulletAndPlaySound(
    pGaming,
    &coords,           // LEA [ESP+0x24] → {x,y} from first two reads
    direction,         // 3rd byte → CShot::bDirection
    ownerSlotForHelper,// 4th byte → CShot::bOwnerSlotId
    flags,             // 5th byte → weaponStrength=(flags&3)-1; SFX from flags&0xF0
    audioPos,
    0                  // frameId — always 0 (default LookupBulletFrameStrip table)
  )
  return 0
```

### Diff vs `CLevelScriptOpExt_SpawnOpponentEx` (`0x0041f6a0`)

| | **SpawnEnemyAt** (op 67) | **SpawnOpponentEx** (op 98) |
|--|--------------------------|------------------------------|
| Script arity | 5 sub-expressions | 6 sub-expressions |
| `ReadSubExpr` count | 5 (`0x004384c0`) | 6 |
| `frameId` (7th C arg) | **Hardcoded `0`** (`PUSH 0` @ `0x0041f672`) | **6th script arg** (`PUSH EDI` @ `0x0041f70c`) |
| Parent-slot / audio path | Identical: 4th byte → `GetObjectAtSlotSafe`; `0xFF` → null | Same |
| Shared helper | `CGaming_SpawnBulletAndPlaySound` @ `0x0041f230` | Same |

**Only proven delta:** `SpawnOpponentEx` passes a **collection handle / pool index** as `frameId` into `CShot_Ctor` → `CGameView_LookupBulletFrameStrip(gaming, direction, frameId, weaponStrength)`. When `frameId==0`, lookup uses the built-in `+0x88` direction table; when non-zero, `HandleVirtualBaseCast(*(coll+8)[direction], 0xD)` (CollInsert pool — global 12 on level 65859).

### Shared helper `CGaming_SpawnBulletAndPlaySound` (`0x0041f230`)

```
PostMessage(gaming+0x10, 0x200, 0xF4, …)
if (flags & 3) != 0:
  shot = new CShot(0xB0)
  CShot_Ctor(shot, &coords, gaming, direction, ownerSlot, (flags&3)-1, frameId)
  CIntListInsertSortedOrAppend(gaming+0x2C8, shot, …)
  CGaming_AddEntity(gaming, shot, 1)
switch (flags & 0xF0):
  0x10 → sample 0x15 (21)
  0x20 → sample 0x0D (13)
  0x40 → sample 0x13 (19)
  else → no extra SFX
```

`CShot_Ctor` (`0x0041edf0`) copies `coords` → `shot+0x20` world rect, sets entity type `0x0F`, registers 50 ms scheduler slot, runs immediate `SpatialQuery` (instant resolve if blocked).

### Flag semantics (proven)

| `flags & 3` | `CShot::bWeaponStrength` (`(flags&3)-1`) | Overlay examples |
|-------------|------------------------------------------|------------------|
| `1` | `0` | `IntConst(1)` — mimic ambush, 8-bit timer spawn |
| `2` | `1` | `IntConst(2)` — `SpawnOpponentEx` on 65859 |
| `0` | *(no `CShot` alloc)* | — |

High nibble `flags & 0xF0` selects optional fire SFX only; overlay spawns use low bits 1 or 2.

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041f610` | `CLevelScriptOpExt_SpawnEnemyAt` | Opcode 67 handler; 5× `ReadSubExpr`; calls helper with `frameId=0` | Disasm `0x0041f610`–`0x0041f693`; dispatch row 67 |
| `0x0041f6a0` | `CLevelScriptOpExt_SpawnOpponentEx` | Opcode 98; 6× `ReadSubExpr`; passes 6th arg as `frameId` | Disasm `0x0041f6a0`–`0x0041f72d`; `PUSH EDI` @ `0x0041f70c` |
| `0x0041f230` | `CGaming_SpawnBulletAndPlaySound` | Alloc/register `CShot` + optional SFX | Disasm `0x0041f230`–`0x0041f344`; `RET 0x18` (6 stack args) |
| `0x0041edf0` | `CShot_Ctor` | World position, direction, owner slot, frame strip | `OperatorNew(0xB0)` @ `0x0041f277`; ctor @ `0x0041f2af` |
| `0x00417f40` | `CGameView_LookupBulletFrameStrip` | `frameId==0` → `+0x88` table; else CollInsert entry | Decompile; sole caller `CShot_Ctor` |
| `0x00416810` | `CGaming_GetObjectAtSlotSafe` | Bounds-checked slot read for audio ref | Call @ `0x0041f662` / `0x0041f6fc` |
| `0x004384c0` | `CDSScript::ReadSubExpr` | Sub-expression evaluator | Five/six calls per handler |
| `0x0041a390` | `CGaming_AddEntity` | View-tree attach; mode `1` from helper | Call @ `0x0041f2d7` |

**Xrefs:** both opcode handlers referenced only from script dispatch data (`0x004af070`, `0x004af0ec`).

## Struct fields (touched)

| Struct | Offset | Field | Use in this path |
|--------|--------|-------|------------------|
| `CLevelScript` | `+0x458` | `pGaming` | `MOV ECX,[ESI+0x458]` before helper call |
| `CGameView` / entity | `+0x20` | world origin | Audio attenuation input; `CShot` placement |
| `CShot` | `+0xA4` | `bDirection` | 3rd script arg |
| `CShot` | `+0xA5` | `bOwnerSlotId` | 4th script arg (editor “team” / mimic slot **9**) |
| `CShot` | `+0xA6` | `bWeaponStrength` | `(flags & 3) - 1` |
| `CGaming` | `+0x2C8` | bullet list | `CIntListInsertSortedOrAppend` |

## Overlay scripts (evidence)

| Resource | Level | Pattern | Proven args |
|----------|-------|---------|-------------|
| `65856` | Bedtime story (`res_0000065856`) | `mimic_spawn_init` | `(g5±offset, g6+10, dir∈{0,1}, slot **9**, flags **1`) — mimic peek projectile from slot-9 view |
| `65857` | (`res_0000065857`) | `fn_0x05d9` / `Rand(...)` | `(x,y,kind, **high_byte(p3)**, **1**)` — timer spawn; `SpawnAtView(2, ownerSlot+10)` follows |
| `65859` | 8-bit captivity | `SpawnOpponentEx` (contrast) | `(221,136,3,9,2,g12)` — same owner slot **9**, `flags=2`, **6th** arg `gvar12` CollInsert pool |

Luau reimplementation: `engine.spawn_enemy_at` / `engine.spawn_opponent_ex` in `open_bulanci/assets/levels/res_*_Script.lua`.

## Ghidra deltas

| Action | Target | Detail |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0041f610` | Documented 5-arg mapping, `CShot` path, overlay IDs |
| `save_program` | `bulanci.exe` | Once |

(`SpawnOpponentEx` already had pool comment from asset agent 05.)

## Decomp fixes

| Issue | Resolution |
|-------|------------|
| `extraout_AL` / `extraout_DL` register artifacts on `ReadSubExpr` | Map by **push order** from disasm (see Algorithm), not decompiler temps |
| Dispatch table text “enemy spawn” | **Hygiene:** handlers spawn `CShot`, not `CBulanek`; cousin of `InsertOpponent` only at bytecode level |
| `CGaming_SpawnBulletAndPlaySound` name | Correct for `0x0041f230`; shared by weapon fire **and** these script ops |

## Frida

Not required — static disasm + overlay literals close all branches for this task.

## Remaining UNK

| Item | Blocker |
|------|---------|
| Exact `+0x88` frame table indices for `direction` 0/1/3 without `frameId` | Needs per-level `CGameView` asset dump (task 19 / frame catalog) |
| Semantic names for `flags & 0xF0` on these script spawns | No overlay uses non-zero high nibble on these opcodes |
| Whether `ownerSlot` 4th arg ever uses campaign slots `125..127` | No overlay hit; only slot **9** and Rand high-byte values observed |

## Cross-links

- `ghidra_analysis/engine/script_dispatch_table.md` — opcodes 67 / 98
- `ghidra_analysis/gameplay/combat_projectiles.md` — `CGaming_SpawnBulletAndPlaySound` pipeline
- `ghidra_analysis/asset_catalog/naming/agent_05.md` — `SpawnOpponentEx` pool on 65859
- R11 task 17 — `SpawnOpponentEx` (6-arg / CollInsert)
- R11 task 19 — deep `0x0041f230` / slot assignment (shared)
