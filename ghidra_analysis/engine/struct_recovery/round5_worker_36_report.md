# Round 5 — worker 36 report (CAnim / CBulAnim UNKs)

## Task

| Field | Value |
|-------|-------|
| **worker** | 36 / 50 |
| **round** | 5 |
| **mode** | WRITE |
| **scope** | Resolve or narrow UNKs in [CAnim.md](./CAnim.md) and [CBulAnim.md](./CBulAnim.md) |
| **types** | `CAnim`, `CBulAnim`, `CDSChained`, `CDSVideoPlayer`, `CMina`, `CTeleportPoint` |

## Status

**PARTIAL** — Ghidra struct renames and comments applied; several chain-header dwords remain ctor-zero-only with no game-specific writer on the anim paths.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `dwChainRoot` @ CAnim `+0x40` | `CDSChained_ResetChainCounters@0x0042beb0` | Same offset as `CDSChained.dwChainRoot`; was mislabeled `dwView_pad_40` in Ghidra |
| `nSpatial_bucket_x/y` used as extent corners | `CMina_RegisterDangerZone@0x0041c0d0` | `updatedItem` base: reads `animBase+0x20..0x24` and `+0x28..+0x2c` with ±10 for danger rect |
| Spatial sort key | `CGameView_GetSpatialBucketKey@0x00416f50` | `return dwView_aux_6c + nSpatial_bucket_y` |
| Teleport gate writes buckets | `CTeleportPoint_Ctor@0x0041c9a0` region | Adjusts `nSpatial_bucket_x/y` from partner `GetBounds` (`vfn+0x70`) vs `nOrigin` |
| `dwChainField_0c` trace gate | `CMina_UpdateTraceAreas@0x00419fd0` | `if (animBase.dwChainField_0c != 0)` then sweep via `pGaming_host` |
| `dwChainField_08/0c` init | `CDSChained_ctor@0x004032d0` | Via `CBitmap_ViewHeader_Init@0x00419070` → `CDSChained_ctor`; no CAnim-only writer found |
| `pChain_pad_48` | `CLevelScript_FireOnBitmapEvt_FromView@0x00422f20` region | Decompiler `this[5].pChain_pad_48` is **CBitmap array-index artifact**; script slot read is `*(byte *)(view+0x70)` per disasm comment |
| CBulAnim screen blit rect | `CDSView__UpdateScreenCoordinates@0x0042bf40` | Copies `bbox_*` @ `+0x20..+0x2c` → `nScreenBbox_*` @ `+0x30..+0x3c` (CDSChained names) |
| `bbox_right/bottom` zero-init | `CDSChained_ctor@0x004032d0` | Anim path via `CDSAnim::DefaultCtor@0x00439470`; no CBulAnim-specific bbox writer |
| Random track bound | `IDSAnim_SelectRandomTrack@0x004392a0` | Fake `CDSStreamStorage*` @ `+0x1c`; asm `GetStreamCount` reads **`dwChainRoot` @ +0x40** |
| `wChainInit44` visibility byte | `TM_SetTrack@0x004391e0` | `TEST byte ptr [outer+0x44], 0x40` @ `0x0043924c` — **byte** at `+0x44`, not `+0x45` alone |
| `trackManager.trackVector` | `ConstructTrackManager@0x00439c70` | Nested `CDSTrackVector` @ `trackManager+0x1c` (72 B `CDSVideoPlayer`) |
| `pOds_owner` / `pOwner` | `CBitmap_ViewHeader_Init@0x00419070`, `ODSImage::SetOwner` | `CBulanci *` at `CAnim+0x90` / `CBulAnim+0x70` (Ghidra `-BAD-` cleared) |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CAnim.dwView_pad_40` | → `dwChainRoot` |
| `modify_struct_field` | `CAnim.pOds_owner` | → `CBulanci *` |
| `modify_struct_field` | `CBulAnim.dwField_30..3c` | → `nScreenBbox_left/top/right/bottom` |
| `modify_struct_field` | `CBulAnim.nBbox_right/bottom` | → `bbox_right` / `bbox_bottom` |
| `modify_struct_field` | `CBulAnim.pOwner` | → `CBulanci *` |
| `set_decompiler_comment` | `0x00416f50`, `0x0041c0d0`, `0x00419fd0`, `0x004392a0`, `0x0043924c` | R5 worker 36 notes |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CAnim.md](./CAnim.md) — UNK narrowed; `dwChainRoot` @ `+0x40`; spatial-bucket semantics
- [CBulAnim.md](./CBulAnim.md) — CDSChained-aligned prefix names; `SelectRandomTrack` / `+0x44` visibility note

## Remaining UNK

- `dwChainField_08` / `dwChainField_0c` — CDSChained mirror; `+0x0c` read as CMina trace gate only; **no** dedicated writer on anim objects (inherits ctor zero).
- `pChain_pad_48` @ CAnim `+0x48` — Ghidra still 12 B opaque; should be `wChainInit44` + chain ushort band (`CDSChained` `+0x44..+0x4f`) in a future struct split.
- `nSpatial_bucket_x/y` naming — used as **extent / partner-bounds aux**, not only spatial-hash buckets; consider rename after wider CDSView pass.
- `CDSTrackVector` Ghidra field names `dwTracks*` vs logical `cTracks*` ([CDSTrackVector.md](./CDSTrackVector.md)).
- RTTI / COL records — see [anim_runtime.md](../anim_runtime.md).
