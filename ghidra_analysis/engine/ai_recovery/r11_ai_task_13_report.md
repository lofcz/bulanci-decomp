# Round 11 — AI Task 13 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 13 |
| **title** | InsertVampires / SpawnCoopPartnerSlots (slots `0x20..0x23`) |
| **archetype** | `vampire_coop` |
| **seed_address** | `0x00420630` |
| **addresses** | `0x00420630`, `0x00420550`, `0x00420530` |
| **acceptance** | Prove vampire == coop `CBulanek` slots; hide slots 1..3; `TickRoundStateAndScoring` side effects |

## Status

**DONE** — Live Ghidra MCP disasm/decomp proves the script opcode `InsertVampires` is a thin wrapper into `CGaming_SpawnCoopPartnerSlots`, which exclusively allocates `CBulanek` entities at slots `0x20..0x23`. Slots `0x21..0x23` are visually hidden and marked inactive immediately after spawn; slot `0x20` remains visible.

## AI archetype

| Slots (hex) | Class | Role | Spawn entry |
|-------------|-------|------|-------------|
| `0x20..0x23` | `CBulanek` (`0x7ec`) | Co-op “vampire” AI allies (script name) | `CLevelScriptOpExt_InsertVampires` → `CGaming_SpawnCoopPartnerSlots` |

“Vampire” is the level-script / designer name (`InsertVampires`, Lua `engine.insert_vampires()`). Engine-side they are the same **co-op partner slot band** documented in `map_slots_spawner.md` — not a separate entity type.

## Algorithm

### Script entry — `CLevelScriptOpExt_InsertVampires` @ `0x00420630`

Opcode **100** (extension **55**) per `script_dispatch_table.md`. Zero stack args.

```c
// Disasm @ 0x00420630:
//   MOV EAX,[ESP+4]        ; pLevelScript
//   MOV ECX,[EAX+0x458]    ; pGaming
//   CALL CGaming_SpawnCoopPartnerSlots
//   XOR EAX,EAX ; return 0

undefined4 CLevelScriptOpExt_InsertVampires(CLevelScript *pScript) {
    CGaming *pGaming = *(CGaming **)((byte *)pScript + 0x458);
    CGaming_SpawnCoopPartnerSlots(pGaming);
    return 0;
}
```

**Caller:** script VM dispatch only (no native xrefs besides opcode table wiring).

**Levels calling `InsertVampires()`:** `res_0000065860` (Exitus), `res_0000100015` (svet_3005), `res_0000100011` (Valka), `res_0000100007` (smrticizajezd), plus generated Luau mirrors.

### Co-op spawn loop — `CGaming_SpawnCoopPartnerSlots` @ `0x00420550`

```c
void CGaming_SpawnCoopPartnerSlots(CGaming *this) {
    CBulanek **pSlot = &this->m_apEntitySlots[0x20];  // this+0x148 (= 0xC8 + 0x20*4)

    for (int i = 0; i < 4; i++) {
        byte slotKind = (byte)(0x20 + i);
        CGaming_SpawnPlayerAtSlot(this, slotKind);

        if (i > 0) {  // slots 0x21, 0x22, 0x23 only
            CBulanek *entity = *pSlot;
            CDSView__Hide((CDSView *)entity);
            entity->bActiveInWorld = 0;   // +0x69
            CGaming_TickRoundStateAndScoring(this, (int *)entity);
        }
        pSlot++;
    }
}
```

**Disasm proof (key sites):**

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x00420558` | `LEA EBP,[EDI+0x148]` | `pSlot = &m_apEntitySlots[0x20]` |
| `0x00420563` | `ADD AL,0x20` | `slotKind = i + 0x20` |
| `0x00420568` | `CALL 0x00420530` | `CGaming_SpawnPlayerAtSlot` |
| `0x0042056d` | `CMP EBX,0x1` / `JL` | skip hide path when `i==0` (slot `0x20`) |
| `0x00420577` | `CALL 0x0042d040` | `CDSView__Hide` |
| `0x0042057f` | `MOV byte [ESI+0x69],0` | `bActiveInWorld = 0` |
| `0x00420583` | `CALL 0x0041f350` | `CGaming_TickRoundStateAndScoring` |
| `0x0042058e` | `CMP EBX,0x4` | loop `i = 0..3` |

**Slot visibility outcome:**

| Slot | Decimal | After spawn |
|------|---------|-------------|
| `0x20` | 32 | Visible (`CDSView__Hide` not called); `bActiveInWorld` stays `1` (ctor @ `0x0041e98c`) |
| `0x21` | 33 | Hidden + inactive |
| `0x22` | 34 | Hidden + inactive |
| `0x23` | 35 | Hidden + inactive |

Reveal / unhide mechanism for slots `0x21..0x23`: **UNK** (not referenced in these three functions).

### Per-slot spawn wrapper — `CGaming_SpawnPlayerAtSlot` @ `0x00420530`

Hardcoded deathmatch parameters (same wrapper used for human slots `0..N-1` from `CBulanek_SpawnPlayerAndCampaignSlots`):

```c
void CGaming_SpawnPlayerAtSlot(CGaming *this, byte slotKind) {
    CGaming_SpawnAndInitializePlayer(
        this,
        slotKind,
        (void *)0xFFFFFFFF,  // pTeamColorOverride — random team for AI band
        1,                   // nLives
        100,                 // nSpeedParam (neutral; ctor may clamp)
        0                    // skinPaletteId — random when < 0 path not taken
    );
}
```

`CGaming_SpawnAndInitializePlayer` @ `0x0041f500`: `OperatorNew(0x19c)` → `CBulanekCtor` → `CGaming_AddEntity` → `CGaming_RegisterObjectAtSlot` → `CGaming_RespawnPlayerAtSafeLocation` → random facing `0..3` via `_rand`.

### `CBulanekCtor` branch for `slotKind ∈ {0x20,0x21,0x22,0x23}`

Predicate: `(slotKind - 0x20) <= 3` — disasm `SUB AL,0x20` / `CMP AL,0x3` / `SETBE` @ `0x0041e5e7..0x0041e5eb`.

| Field / behavior | Value | Proof |
|------------------|-------|-------|
| `nSpeedParam` | forced `0x28` (40) | `MOV EDI,0x28` @ `0x0041e62d` when coop branch |
| `nLivesRemaining` / snapshot | `1` | `MOV [ESP+0x54],1` @ `0x0041e631` |
| `skinPaletteId` (weapon ctor) | `4` | `MOV [ESP+0x5c],4` @ `0x0041e639` |
| `bIsAiVariant` | `1` | `CMP CL,0x3` / `SETA` → store @ `+0x199` |
| Walk anim table | `gABulanekWalkAnimsSpecial` | `MOV [ESP+0x40],0x4af2dc` @ `0x0041e723` (else `0x4af2ec` normal) |
| `bTeamColor` | `7` | `MOV byte [ESI+0x123],7` @ `0x0041e821` |
| Danger-zone AI nodes | 4× `CGaming_AppendDangerZoneNode` | loop @ `0x0041e690..0x0041e6ad` when `slotKind > 3` |
| Scheduler (`CDSUpdatedItem` @ `+0x88`) | slots **3**, **4** delay `0`; slot **5** delay `100` ms; `eventKind=7` | `CALL 0x0042f210` trio @ `0x0041ea23..0x0041ea41` |

Co-op band shares the **`slotKind > 3`** AI infrastructure (danger zones, `bIsAiVariant`) with practice dummies (`0x24..0x27`) and campaign AI (`0x7d..0x7f`), but ctor constants above are **unique to `0x20..0x23`**.

### `CGaming_TickRoundStateAndScoring` side effects (per hidden ally)

Called with `param_1 =` hidden `CBulanek*` immediately after hide + `bActiveInWorld=0`:

1. **`CGaming_TickPlayerCollisions(this, entity)`** — collision pass for that entity.
2. **`CGame_SetTeamScore(pGame, entity.bPlayerSlot, entity.dwScriptKillScore)`** — `bPlayerSlot` read via `*(byte*)(entity+0x70)`; score field @ `+0x13c`.
3. If `CGameGetModeAndScoreLimit` mode `!= 0` and HUD counter exists (`CGaming+0x320`): **`CGameCounter_UpdatePlayerScore`** for that team byte @ `entity+0x70` path (decomp uses `param_1+0x1c` as `int*` → byte offset `0x70`).
4. If mode `== 2` (last-man): scans active player slots, updates round timer label, may post win message `0x100/0x80cc` when one survivor remains.

Invoking this per hidden spawn synchronizes collision/score state for each co-op slot as it is registered, even while visually hidden.

## Functions table

| Address | Ghidra symbol | Role | Callers | Callees |
|---------|---------------|------|---------|---------|
| `0x00420630` | `CLevelScriptOpExt_InsertVampires` | Script opcode 100 handler | Script VM | `CGaming_SpawnCoopPartnerSlots` |
| `0x00420550` | `CGaming_SpawnCoopPartnerSlots` | Spawn 4 co-op `CBulanek` @ `0x20..0x23`; hide `0x21..0x23` | `InsertVampires` | `CGaming_SpawnPlayerAtSlot`, `CDSView__Hide`, `CGaming_TickRoundStateAndScoring` |
| `0x00420530` | `CGaming_SpawnPlayerAtSlot` | `nLives=1` spawn wrapper | `SpawnCoopPartnerSlots`, `CBulanek_SpawnPlayerAndCampaignSlots` | `CGaming_SpawnAndInitializePlayer` |
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | Alloc/register/respawn/facing | `SpawnPlayerAtSlot`, `SpawnPracticeDummy`, … | `CBulanekCtor`, `CGaming_RegisterObjectAtSlot`, … |
| `0x0041e4b0` | `CBulanek::CBulanekCtor` | Slot-kind-specific AI setup | `SpawnAndInitializePlayer` | `CGaming_AppendDangerZoneNode`, `Scheduler_RegisterEventSlot`, … |
| `0x0041f350` | `CGaming_TickRoundStateAndScoring` | Collisions + HUD + last-man | `SpawnCoopPartnerSlots`, game tick | `CGaming_TickPlayerCollisions`, `CGame_SetTeamScore`, … |
| `0x0042d040` | `CDSView__Hide` | Clear view visible flag | `SpawnCoopPartnerSlots`, … | `CDSView_SetActive`, vfn `+0x24` |

## Struct fields

| Struct | Offset | Field | Use in this task |
|--------|--------|-------|------------------|
| `CLevelScript` | `+0x458` | `pGaming` (CGaming*) | `InsertVampires` loads before spawn |
| `CGaming` | `+0xC8` + `slot*4` | `m_apEntitySlots[128]` | slot `0x20` cell @ `+0x148` |
| `CBulanek` / `CGameView` | `+0x69` | `bActiveInWorld` | cleared to `0` for hidden co-op slots |
| `CBulanek` | `+0x70` | `bPlayerSlot` | registered slot id (`0x20..0x23`) |
| `CBulanek` | `+0x123` | `bTeamColor` | forced `7` for `0x20..0x23` |
| `CBulanek` | `+0x13c` | `dwScriptKillScore` | passed to `CGame_SetTeamScore` in tick |
| `CBulanek` | `+0x18c` | `nLivesRemaining` | forced `1` in ctor coop branch |
| `CBulanek` | `+0x194` | `nSpeedParam` | forced `0x28` in ctor coop branch |
| `CBulanek` | `+0x199` | `bIsAiVariant` | `1` for all `slotKind > 3` |

## Ghidra deltas

None — symbols, prototypes, and plate comments for all three seed addresses were already correct from prior recovery passes. No `save_program` this task.

## Decomp fixes

None required. Prior comments on `CGaming_SpawnCoopPartnerSlots` and `CGaming_SpawnPlayerAtSlot` match disasm.

## Frida

Not run. Static disasm closes acceptance criteria.

## Remaining UNK

| Item | Notes |
|------|-------|
| Unhide / activate slots `0x21..0x23` | No xref from these spawn functions; likely scheduler event kind `7` or separate gameplay hook |
| Scheduler slot indices 3/4/5 meaning | Registered only for coop band (`0x0041ea23+`); ties to `TryBotRandomAction` path — task 16+ |
| Gameplay reason for 4 spawns / 1 visible | Designer pattern in co-op campaign maps; engine behavior proven, intent inferred from level scripts only |
