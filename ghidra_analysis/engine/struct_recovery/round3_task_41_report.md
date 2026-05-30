# Round 3 — Task 41 report

## Task

| Field | Value |
|-------|-------|
| **id** | 41 |
| **title** | Clean CBulanek tail Ghidra renames + pre-ammo pads |
| **source** | `agent_todos_50_r3.json` (critical blocker) |
| **types** | `CBulanek`, `CGameView`, `CGaming` |
| **addresses** | `0x0041e4b0`, `0x004205a0`, `0x00420910`, `0x0041aed0` |
| **acceptance** | `CBulanek` 412 B in Ghidra; flat `CGameView` prefix through `+0x87`; tail + pre-ammo field names per `CBulanek.md`; `save_program` |

## Status

**DONE** — `get_struct_layout CBulanek` → **412 bytes**; `pCdsViewPrefix` blob removed; pre-ammo band split; quip pointers and track arrays at doc offsets; decompile on ctor / `CBulanek_OnEvent` uses typed fields.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Heap size `0x19c` | spawn path | `OperatorNewWithBadAlloc(0x19c)` before `CBulanekCtor` (`CBulanek.md`) |
| Flat header (no `pCdsViewPrefix`) | `CBulanekCtor@0x0041e4b0` | `CDSChained_ctor` + `CGameView_InitGamingFields`; `this->nOrigin_x`, `wViewFlags`, `bPlayerSlot`, `nCollisionLeft`..`pGamingHostScratch` |
| `scheduler` @ `+0x88` | `CBulanekCtor@0x0041e4b0` | `CDSUpdatedItem_ctor(&this->scheduler)`; `Scheduler_RegisterEventSlot(&this->scheduler,…,7)` |
| `videoTrackManager` @ `+0xA8` | `CBulanekCtor` / `CBulanek_ApplyAction@0x00420910` | `ConstructTrackManager(&this->videoTrackManager)`; `AddTrackSource(&this->videoTrackManager,…)` |
| `vftable_event` @ `+0xA0` | `CBulanekCtor@0x0041e4b0` | `this->vftable_event = g_pCBulanek_vftable_event` |
| `pDeath2Tombstone` @ `+0x100` | `CBulanek_OnDeath@0x0041f900` | Ctor `this->pDeath2Tombstone = 0`; tourney path `*(this+0x100)` release (`bulanci.ghidra.exe.c` @ `0x88915`) |
| `pPad_preAmmoBand` @ `+0x104..+0x11B` | `CBulanekCtor@0x0041e4b0` | 24 B zero loop on `this->pPad_preAmmoBand[0..0x17]` before `bAmmoKind0@+0x11C` |
| Walk tracks @ `+0x14C` | `CBulanekCtor@0x0041e4b0` | `this->pWalkTrackSources[i]=0`; `AddTrackSource(&videoTrackManager,…)` over `gABulanekWalkAnimsNormal/Special` |
| AI holders @ `+0x178` | `CBulanekCtor@0x0041e4b0` | `ppvVar10 = this->pAiTrackHolders`; `FUN_0041b420(…,(int)this)` when `3 < slotKind` |
| Hit / delayed quip @ `+0x170` / `+0x174` | `CBulanek_OnEvent@0x00420d40` | `this->pHitQuipPlayer`, `this->pDelayedQuipPlayer`; event `0xdb` arms hit; event `1` chains delayed |
| `bPlayerSlot` @ `+0x70` | `CBulanek_OnEvent` | `bVar7 == this->bPlayerSlot` on script events `0xee` / `0xef` |

### Tail / pre-ammo map (primary base)

| Offset | Name | Size | Notes |
|--------|------|------|-------|
| `+0x00..+0x87` | `CGameView` mirror | 136 | Vtables, origins, bounds, `dwView_flags`, gaming bytes, collision, `pGamingHostScratch` |
| `+0x88` | `scheduler` | 24 | `CDSUpdatedItem` |
| `+0xA0` | `vftable_event` | 4 | Event / ODSImage facet |
| `+0xA8` | `videoTrackManager` | 72 | `CDSVideoPlayer` |
| `+0xF0..+0xFB` | anim / game / weapon | 12 | `pActiveAnim`, `pGame`, `pWeapon`, `pCorpseAnim` |
| `+0x100` | `pDeath2Tombstone` | 4 | `CDeath2 *` |
| `+0x104..+0x11B` | `pPad_preAmmoBand` | 24 | Ctor zero band; semantics UNK |
| `+0x11C` | `bAmmoKind0` / `bAmmoKind1` | 2 | `GetMaxAmmoForKind` |
| `+0x14C` | `apWalkTrackSources` | 16 | `void *[4]` — Ghidra layout label may read `pWalkTrackSources` |
| `+0x178` | `apAiTrackHolders` | 16 | `void *[4]` — Ghidra layout label may read `pAiTrackHolders` |
| `+0x170` / `+0x174` | `pHitQuipPlayer` / `pDelayedQuipPlayer` | 4 each | `CDSAudioPlayer` completion targets |

## Ghidra deltas

- Rebuilt / verified **`CBulanek` 412 B** with flat prefix (no `pCdsViewPrefix[112]`).
- `add_struct_field` / prior batch: `pDeath2Tombstone@+0x100`, `pPad_preAmmoBand@+0x104`, `pHitQuipPlayer@+0x170`, `pDelayedQuipPlayer@+0x174`.
- `modify_struct_field` / `recreate_struct`: tail names `vftable_event`, `bPlayerSlot`, `pGamingHostScratch`, `scheduler`, `videoTrackManager`.
- `modify_struct_field` rename attempts on `apWalkTrackSources` / `apAiTrackHolders` (Ghidra may keep `p*` on `void *[4]` in `get_struct_layout` listing).
- `force_decompile` `CBulanekCtor@0x0041e4b0`, `CBulanek_OnEvent@0x00420d40`.
- `save_program bulanci.exe`.

**Note:** A failed `remove_struct_field` on the track arrays briefly shrank the struct to 380 B; `recreate_struct` restored the full 412 B layout.

## Struct doc updates

- [CBulanek.md](./CBulanek.md) — Ghidra apply note (agent todo 41, 2026-05-30).

## Remaining UNK

- `pPad_preAmmoBand@+0x104..+0x11B` — only ctor zero proven; not the death-voice slot (`dwField_16C` / event-1 `lParam` compare in decompile still needs separate naming pass).
- Ghidra `get_struct_layout` may list `pWalkTrackSources` / `pAiTrackHolders` instead of doc `ap*` names for `void *[4]` components.
- `+0x9C..+0xEF` inside `CDSVideoPlayer` / anim subgraph (defer to track batch).
- Merge `bStateByte` vs `bPlayerSlot` naming on `+0x70` (follow-up in `CBulanek.md`).
