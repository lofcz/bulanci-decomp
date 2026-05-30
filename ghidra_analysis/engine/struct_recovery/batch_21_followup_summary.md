# Struct recovery — batch 21 follow-up (round 2)

**Prior:** `batch_21_summary.md` → `CDSAnim`, `CTeleportPoint`  
**Status:** **FOLLOWUP_COMPLETE**  
**Program:** `bulanci.exe` (saved once)

## Actions

### CDSAnim

| Item | Result |
|------|--------|
| `+0xb8` `cached_sequence` | **Proven** — `AnimInner_Init` / `TM_SetTrack` use anim-inner `+0x40` (= outer `+0xb8` when inner @ `+0x78`) |
| `+0x44` `visibility_mask` | **Proven** — `TM_SetTrack` @ `0x0043924c` tests `byte [outer+0x44] & 0x40` |
| `+0x70` subscriber key | Still **UNK** (no direct write xref in anim cluster) |
| `+0x08..0x67` drawable prefix | **Done** (agent todo **27**) — Ghidra fields mirror `CDSChained` on `CDSAnim` / `CDSBitmap` |
| `track_manager` innards | Still **UNK** at field level (`ConstructTrackManager` only) |

**Ghidra:** `add_struct_field` → `bVisibility_mask` @ 68, `pCached_sequence` @ 184.

### CTeleportPoint

| Item | Result |
|------|--------|
| `OnEvent` decompiler cleanup | **Improved** — struct fields at `+0x64/68/69/74..` ; renamed `CTeleportPoint::OnEvent` |
| `OnEvent` partner typing (todo 29) | **Done** — `pPartner_node` → `CTeleportPoint *`; visibility probe `this[-1].pPartner_node` + `(partner->canim_base).dwView_flags`; paired-alloc `[this-8]` documented |
| `linked_player` → `overlap_entity` | Renamed; **consumer** xrefs confirmed; **writer** not found in game `.text` |
| Full `CAnim` nest | **Partial** — gameplay offsets in Ghidra; `pCanim_base[0x64]` blob + extension fields |

**Ghidra:** Rebuilt `CTeleportPoint` with overlap, gate flags, teleport rects, FX fields, partner/orientation; saved program.

## Ghidra deltas

```
CDSAnim: +bVisibility_mask (68), +pCached_sequence (184)
CTeleportPoint: overlap/gate/rect/FX/partner fields; OnEvent renamed
save_program bulanci.exe
```

## Remaining UNK (both structs)

- CDSAnim drawable band `+0x28..+0x43` and `field_70`; CDSObject fields inside `+0x88`.
- CTeleportPoint `overlap_entity` writer; middle `CAnim` bytes; partner-node decompile adjacency (`this[-1]`).

## Next batch (unchanged)

`batches_50.json[22]` → `CDSApiException`, `CDSAudioBank`
