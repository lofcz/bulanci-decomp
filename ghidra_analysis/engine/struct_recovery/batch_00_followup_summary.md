# Struct recovery follow-up — batch 00/50

**Program:** `bulanci.exe`  
**Prior:** [batch_00_summary.md](./batch_00_summary.md)  
**Status:** **HANDOFF_PARTIAL** (one cleanup applied; deferred items unchanged)

## Follow-ups from prior batch

| Item | Source | Action |
|------|--------|--------|
| `CDSObject` prefix `+0x04..+0x10` refcount stubs (+3 byte skew) | `batch_00_summary` apply notes | **Fixed** — `delete_data_type` + `create_struct` with `CDSUpdatedItem scheduler` @ `+4` |
| Embedded `CGame` @ `CBulanci+0x284` (`0x22c`) | `batch_00_summary` “Next batch” | **Done** — agent todo 1: `CGame_embedded` on `CBulanci.game`; tail @ `+0x4b0` |
| `CBulanci` Ghidra size 1288 vs proven `0x4cc` | `batch_00_summary` | **Deferred** — needs `CGame` recovery + `CDSApp` reconciliation |
| `CBulanci` UNK `field_280` / `field_4b4` / `field_4c4` | [CBulanci.md](./CBulanci.md) | **Partial** — `+0x280` applied as `pMainMenu` `CMenu *` (agent todo 7); `field_4b4` / `field_4c4` still UNK |
| `CDSObject` image MI / `+0x48..+0x4f` | [CDSObject.md](./CDSObject.md) | **Fixed** (agent todo 9) — `nEmbeddedImage_field_44` @ `+0x48`, `pEmbeddedImage_vf_primary` @ `+0x4c` |

## Ghidra deltas

| Struct | Before | After |
|--------|--------|-------|
| `CDSObject` | 95 bytes; `dwRefField04`..`pPad_11` stub prefix; track fields at +31 | **96** bytes; `CDSUpdatedItem scheduler` @ +4; track fields @ +28 (`0x1c`) |

**Decompile check:** `ConstructTrackManager@0x00439c86` now uses `param_1[7]`..`[0x11]` on dword indices matching layout; `CDSObject_CtorWithImage@0x0042563a` uses `&this->scheduler` for `CDSImage_ctor`.

**Saved:** yes (`save_program bulanci.exe` once)

## Deliverables updated

- [CDSObject.md](./CDSObject.md) — layout `scheduler` @ `+4`, Ghidra apply note, UNK tightened

## Remaining UNK

- `CBulanci`: `CGame` blob, tail field semantics, Ghidra size reconciliation
- `CDSObject`: `+0x34` play flags vs `m_copyHeight` alias on image hosts (documented in union comment; shared dword at `embed_overlay` tail)
