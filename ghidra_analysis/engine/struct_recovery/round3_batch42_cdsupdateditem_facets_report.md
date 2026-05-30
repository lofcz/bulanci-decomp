# Round 3 — CDSUpdatedItem facet @+0x00 (batch 42 carryover)

> **Note:** This work was briefly filed as `round3_task_25_report.md` before R3 todo **id=25** was reassigned to `CDSAudioVideoPlayer`. Kept here for cross-links from [CDSUpdatedItem.md](./CDSUpdatedItem.md) / [CAdvertising.md](./CAdvertising.md).

## Task scope

| Field | Value |
|-------|-------|
| **topic** | `CDSUpdatedItem+0x00` on hosts without facet vtable write (e.g. `CAdvertising+0x70`) |
| **acceptance** | Prove or document UNK in CDSUpdatedItem.md with func@addr evidence |

## Status

**DONE**

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| `CDSUpdatedItem_ctor` does **not** write embed `+0x00` | `CDSUpdatedItem_ctor@0x0042f060` | First store is `*(param_1+4)=0x4870ac`; no write to `param_1+0` |
| `CAdvertising` embed at `this+0x70` | `CAdvertising_ctor@0x0040e5f0` | `LEA EDI,[ESI+0x70]` @ `0x0040e640`; `CDSUpdatedItem_ctor` with `ECX=EDI` @ `0x0040e649` |
| Host **does** write facet vtable at embed `+0x00` | `CAdvertising_ctor@0x0040e670` | `MOV dword ptr [EDI],0x4817b8` (`EDI=this+0x70`) immediately after ctor |
| Facet vtable identity | data `@0x004817b8` | 5-slot `IDSUpdated` face; slot `[4]=OnTimerTick@0x0040b4b0`, `[3]=deleting_destructor_thunk_0x70@0x0040e730` (`vftable_methods.csv`) |
| Scheduler uses `IDSEventHandler` subobject (`embed+4`) | `CDSUpdatedItem_ctor@0x0042f060` | Task list insert key is `param_1+4` (`puVar1`); `Scheduler_RegisterEventSlot(&this->updatedItem,...)` passes embed base but ctor registered `+4` facet |
| Timer dispatch | `ArmDismissTimer@0x0040aca0` | `Scheduler_RegisterEventSlot(&this->updatedItem,0,param_1,6)` |
| Timer callback / `this` adjust | `OnTimerTick@0x0040b4b0` | `CAdvertising_Dismiss((int)param_1 - 0x70)` — `param_1` is scheduler `CDSUpdatedItem*` |
| Parallel host pattern (not CAdvertising) | `CShot_Ctor@0x0041edf0` | `MOV [EDI],0x481c5c` with `EDI=this+0x88` after ctor @ `0x0041ee61` |
| Parallel host pattern | `ConstructTrackManager@0x00439c70` | `MOV [EDI],0x487770` with `EDI=this+4` after ctor @ `0x00439cab` |

**Conclusion:** Batch 42 listed `CAdvertising+0x70` as a host *without* facet vtable write; disassembly shows the opposite — the write is **host-owned** (not `CDSUpdatedItem_ctor`) but **present** at `0x0040e670`. Field `+0x00` is the MSVC **IDSUpdated** MI facet pointer; it is always supplied by the enclosing class ctor after `CDSUpdatedItem_ctor` among sampled embed sites (`CAdvertising`, `CWindow`, `CEdit`, `CMina`, `CGaming`, `CGame`, `CShot`, `CDSDirectSound`, `CDSVideoPlayer`).

## Ghidra deltas

- `CDSUpdatedItem.pVftable_IDSUpdated` @ offset 0 (renamed from `dwField_0x00`, type `void *`)
- EOL comment @ `0x0040e670` (`CAdvertising_ctor`: facet vtable store)
- `save_program bulanci.exe`

## Remaining UNK

- None for **`CAdvertising+0x70`** / this scope.
- Optional follow-up (out of scope): catalog per-host `IDSUpdated` vtable addresses for all 28 `CDSUpdatedItem_ctor` call sites; `CDSImage` path uses `CDSImage_ctor` on the same byte span without calling `CDSUpdatedItem_ctor` (`CDSObject.md`).
