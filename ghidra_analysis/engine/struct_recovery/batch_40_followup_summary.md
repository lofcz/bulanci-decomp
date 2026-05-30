# Struct recovery batch 40 — follow-up round 2

**Agent:** 40/50  
**Input:** `batch_40_summary.md`, `CDSStreamStorage.md`, `CDSStrmResInfo.md`  
**Status:** **FOLLOWUP_COMPLETE**

## Prior batch follow-ups

| Item | Result |
|------|--------|
| Recover **`CDSCollection`** for `GetStreamEntry` / `GetStreamCount` vs `FindKeyIndex(this+0x1c)` | **Resolved** — [CDSCollection.md](./CDSCollection.md) already **VERIFIED** (`0x18`). Asm: `CDSStreamStorage_ctor@0x00401790` embeds collection at **`this+0x1c`**; `CloseStreamByKey@0x00433cb0` `LEA ECX,[ESI+0x1c]` → `FindKeyIndex`; `FindKeyIndex@0x00431170` uses collection `+0x8`/`+0xc` (`m_items`/`m_count`); live entry table `[ESI+0x24]` → `m_items[i]`. `GetStreamCount` reads **`+0x24`** (returns **`m_items` pointer**, misnamed); `GetStreamEntry` reads **`+0x20`** (vtable slot, not `m_items`) — vtable-only, likely dead. |
| Factory **`0x00433d80`** for `CDSStrmResInfo` `OperatorNew` size | **Resolved** — `CDSStrmResInfo_factory`: `OperatorNewWithBadAlloc(0x28)`; vtables `0x4873f8`/`0x4873dc`; zeros `+0x10`/`+0x14` dwords. |
| Disambiguate **`CDSResInfo` `+0x14`** vs stream tail | **Resolved** — base `CDSResInfo_ReleaseEmbeddedResource@0x00434270` releases embedded pointer; `CDSStrmResInfo` uses same offset for **`streamExtent`** persistence (8-byte serialize); factory/stack keys leave zero → dtor release no-op. |

## Actions taken

| Action | Result |
|--------|--------|
| Ghidra decompile / disasm | `CDSStreamStorage_ctor`, `CloseStreamByKey`, `FindKeyIndex`, `GetStreamCount`, `GetStreamEntry`, `CDSStrmResInfo_factory`, `Serialize`/`Deserialize`, `CDSResInfo_ReleaseEmbeddedResource` |
| `get_struct_layout` | `CDSStreamStorage` 96 B; `CDSStrmResInfo` 40 B (unchanged) |
| `create_struct CDSStrmResInfo` | **Skipped** — type already exists; cosmetic field rename deferred |
| Doc updates | [CDSStreamStorage.md](./CDSStreamStorage.md), [CDSStrmResInfo.md](./CDSStrmResInfo.md) |
| `save_program bulanci.exe` | **Not called** (no program mutations) |

## Ghidra deltas

None.

## Remaining UNK

- ~~`GetStreamEntry` / `GetStreamCount` naming and `IDSAnim_SelectRandomTrack` cast~~ — **Resolved** agent todo 46 (2026-05-30): stubs documented; `GetStreamEntry` vtable-only; `SelectRandomTrack` reads `dwChainRoot` @ `+0x40` via fake storage cast.
- ~~Ghidra `CDSStrmResInfo.pEmbeddedResource` label at `+0x14`~~ **Resolved (agent todo 47)** — `streamExtent` `ulonglong` @ `+0x14`; `pEmbeddedResource` is **CDSResInfo** base only.
- Stream-loader helpers `FUN_00433f00` / `FUN_00433f70` / `FUN_00434760` (out of batch 40 scope).

## Handoff

Batch 40 stream-storage structs documented; size `0x28` for `CDSStrmResInfo` and collection embedding at `+0x1c` for `CDSStreamStorage` are evidence-closed. Optional: rename Ghidra struct fields and realign `CDSStreamStorage` Ghidra layout if decompiler offsets drift from `+0x1c` collection base.
