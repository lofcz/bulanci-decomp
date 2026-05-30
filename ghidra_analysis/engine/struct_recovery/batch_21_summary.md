# Struct recovery — batch 21/50

**Index:** `batches_50.json[21]` → `CTeleportPoint`, `CDSAnim`  
**Program:** `bulanci.exe`  
**Protocol:** evidence-only (`AGENT_PROTOCOL.md`)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSAnim` | PARTIAL | **0xd0** | Applied — 208 B, named vtables + anim/track core |
| `CTeleportPoint` | PARTIAL | **0xf8** | Applied — 248 B, `CAnim[0xf0]` + 8 B extension |

## Size evidence (independent)

- **CDSAnim:** `OperatorNewWithBadAlloc(0xd0)` @ `CreateObject` (`0x004396c0`) and `CBulanci::CScoreCtor` (`0x00411010`).
- **CTeleportPoint:** `OperatorNewWithBadAlloc(0xf8)` ×2 @ `CBulanek_CreateRespawnTeleportPair` (`0x0041d090`); base `CAnim` alloc `0xf0` in same scoreboard ctor.

## Notable findings

1. **CTeleportPoint : CAnim** (not `CDSAnim`) — ctor calls `CAnim::CAnim_SubobjectCtor`; seven vtables patched at CAnim offsets (`+0x88/+0x8c/+0x98` IDSAnim face).
2. **CDSAnim** — seven vtables @ `+0x00/04/10/18/68/6c/78`; track manager constructed at `+0x88`; factory + scoreboard both use `0xd0` bytes.
3. Paired gates store `partner_node` @ `+0xf0` and `gate_orientation` @ `+0xf4`; `OnEvent` reads `pLinked_bulanek` @ `+0x64` (`MOV ECX,[ESI+0x64]`).
4. **Slice 21 Ghidra:** `CAnim` 188→240 B; `CTeleportPoint.canim_base` embedded `CAnim`; `CDSAnim` chain fields `+0x08/+0x0c`; decompile `OnEvent` uses `pLinked_bulanek` (partner `this[-1]` remains).

## Ghidra actions (slice 21)

- `CAnim`: `pTrack_manager` @ 168; `pLinked_bulanek` @ 100; `nDest_x/y`, `nSrc_x/y`, `nFx_coord` renames
- `CTeleportPoint`: `canim_base` → `CAnim`; `partner_node`, `gate_orientation`
- `CDSAnim`: `dwChainField_08`, `dwChainField_0c`
- Prototypes: `OnEvent`, `TriggerTeleportFX`, `CBulanek_CreateRespawnTeleportPair`
- `save_program bulanci.exe`

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CDSAnim.md`
- `ghidra_analysis/engine/struct_recovery/CTeleportPoint.md`

## Next batch

`batches_50.json[22]` → `CDSApiException`, `CDSAudioBank`
