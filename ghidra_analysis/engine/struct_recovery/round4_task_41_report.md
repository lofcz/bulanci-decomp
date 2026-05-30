# Round 4 — Task 41 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 41 |
| **round** | 4 |
| **title** | CBulanek pre-ammo band semantics + apWalk/apAi array names |
| **priority** | critical (blocker) |
| **prior** | [round3_task_41_report.md](./round3_task_41_report.md) |
| **structs** | CBulanek, CGameView, CGaming |
| **addresses** | `0x0041e4b0`, `0x00420d40`, `0x0041f900`, `0x004205a0` |

## Status

**DONE** — pre-ammo band classified write-only; death-voice slot and script tally named; audio quip pointers restored in decompile; `save_program`.

## Evidence

| Offset | Name | Size | Evidence (func@addr) |
|--------|------|------|----------------------|
| `+0x104..+0x11B` | `abReserved_preAmmo` | 24 | `CBulanekCtor@0x0041e4b0` zeros all 24 B; `search_instructions` shows no non-ctor reads on `[ESI+0x104]` |
| `+0x12C` | `anOpponentScriptTally` | `int[4]` | Ctor `LEA EDX,[ESI+0x12c]` zero loop @ `0x0041e848`; `CBulanek_OnEvent` `0xD8` → `ADD [ESI+EAX*4+0x12c],1` @ `0x00420dfa` (slot `<4`) |
| `+0x16C` | `pDeathVoicePlayer` | 4 | `CBulanek_OnDeath@0x0041fa13` `MOV [ESI+0x16c],EAX` after `TriggerBankSample`; `CBulanek_OnEvent` event `1` `CMP [ESI+0x16c],EAX` @ `0x00420e2f`; `CBulanek_dtor` `ReleaseAudioPlayerRef` @ `param_1+0x5b` |
| `+0x170` / `+0x174` | `pHitQuipPlayer` / `pDelayedQuipPlayer` | 4 each | `CBulanek_OnEvent` cases `0xdb` / `1` (hit, death-voice chain, delayed clear) |
| `+0x14C` | `apWalkTrackSources` | `void*[4]` | Ctor `MOV [ESI+0x14c],EBX` @ `0x0041e59e`; walk `AddTrackSource` loop |
| `+0x178` | `apAiTrackHolders` | `void*[4]` | Ctor AI branch `FUN_0041b420`; `OnEvent` `0xF2` tests `[0]` |
| `+0x15C..+0x167` | `abPad_postWalkEmbed` | 12 | Ctor zero only (between walk sources and `bField_168`) |

### Pre-ammo vs death / weapon bands

- **`+0x100`** remains **`pDeath2Tombstone`** (`CDeath2 *`) — separate from pre-ammo band.
- **`+0x121..+0x125`**: `CBulanek_OnDeath` backward byte clear from `&wField_120+1` (weapon HUD slots), not part of `abReserved_preAmmo`.

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `dwField_16C` | → `pDeathVoicePlayer` (`void *`) |
| `modify_struct_field` | `pPad_pre13C` | → `anOpponentScriptTally` (`int[4]`) — layout may export `pOpponentScriptTally` |
| `modify_struct_field` | `pPad_preAmmoBand` | → `abReserved_preAmmo` (`byte[24]`) — layout may export `pReserved_preAmmo` |
| `modify_struct_field` | `offset:368/372` | → `pHitQuipPlayer` / `pDelayedQuipPlayer` |
| `modify_struct_field` | walk/AI arrays | rename attempts; Ghidra may keep `pWalkTrackSources` / `pAiTrackHolders` in `get_struct_layout` |
| `set_decompiler_comment` | ctor / OnEvent / OnDeath | offset anchors for tally, pre-ammo, death-voice |
| `force_decompile` | `0x0041e4b0`, `0x00420d40`, `0x0041f900` | `pDeathVoicePlayer`, quip players, tally indexed access |
| `get_struct_layout` | `CBulanek` | **412 B** unchanged |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CBulanek.md](./CBulanek.md) — R4 todo 41 field table + Ghidra apply log.

## Remaining UNK

- `abReserved_preAmmo` — no consumer xrefs; may be reserved for unlinked build feature or inlined struct not yet recovered.
- Ghidra cosmetic `p*` prefix on `void *[4]` / `int[4]` export names vs doc `ap*` / `an*`.
- Prefix `+0x48..+0x87` `pChain_pad_*` / `pHeader_tail_*` (shared `CGameView` mirror cleanup — other todos).

## Blockers cleared

- Round 3 blocker: `dwField_16C` death-voice compare in `CBulanek_OnEvent` now **`pDeathVoicePlayer`**.
- Round 3 blocker: `pPad_pre13C` misnamed pad at **`+0x12C`** now typed/script-proven tally array.
