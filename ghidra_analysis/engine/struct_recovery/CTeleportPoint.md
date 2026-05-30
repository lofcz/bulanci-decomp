# CTeleportPoint

## Status

**PARTIAL** — `sizeof` **0xf8** proven; **`CAnim` prefix `0xf0`** with documented field map for round-3 gaps (`+0x6a..+0x73`, `+0x85..+0xa3`, `+0xa8..+0xd3`); **8 B** extension `partner_node` + `gate_orientation`. Several gameplay fields are **aliases** into `CAnim` storage.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CTeleportPoint) == 0xf8` | `0x0041d15e` | `CBulanek::CBulanek_CreateRespawnTeleportPair` — `OperatorNewWithBadAlloc(0xf8)` before first `CTeleportPoint_Ctor` |
| `sizeof(CTeleportPoint) == 0xf8` | `0x0041d1b0` | same function — second `OperatorNewWithBadAlloc(0xf8)` for paired gate |
| CAnim base `== 0xf0` | `0x004112a3` | `CBulanci::CScoreCtor` — `OperatorNewWithBadAlloc(0xf0)` + `CAnim::CAnim_ctor` (inheritance anchor) |

## Object model

```
CTeleportPoint (0xf8)
├── canim_base   CAnim prefix          0x000 .. 0x0ef
├── partner_node void*                 0x0f0
└── gate_orientation byte              0x0f4
```

Gameplay uses **`canim_base` offsets directly** (same numeric offsets as `CAnim`). Extension fields are only at `+0xf0`.

## Layout table (full object)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 0xf0 | `CAnim` | `canim_base` | `CTeleportPoint_Ctor@0x0041c9a0` → `CAnim_SubobjectCtor`; vtable patch `+0x88/+0x8c/+0x98` |
| 0xf0 | 4 | `void *` | `partner_node` | `CTeleportPoint_Ctor` — `MOV [ESI+0xf0], EAX` @ `0x0041ca13` |
| 0xf4 | 1 | `byte` | `gate_orientation` | `MOV [ESI+0xf4], CL` @ `0x0041ca19`; `CMP byte [ESI+0xf4], 0` @ `0x0041cad7` |

### CAnim core (vtable / anim; always present)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `vft_primary` | `CTeleportPoint_Ctor` — `*(this+0)=g_pCTeleportPoint_vftable_primary` |
| 0x04 | 4 | `void *` | `vft_chain` | `g_pCTeleportPoint_vftable_IDSChained` |
| 0x10 | 4 | `void *` | `vft_event` | ctor |
| 0x18 | 4 | `void *` | `vft_ref` | ctor |
| 0x20 | 4 | `int` | `bbox_left` / `origin_x` | ctor reads `+0x20..+0x2c` for gate sizing |
| 0x24 | 4 | `int` | `bbox_top` / `origin_y` | same |
| 0x28 | 4 | `int` | `bbox_right` | same |
| 0x2c | 4 | `int` | `bbox_bottom` | same |
| 0x88 | 4 | `void *` | `vft_update` | ctor @ `0x0041c9f5` |
| 0x8c | 4 | `void *` | `vft_anim` | ctor @ `0x0041c9ff` |
| 0x98 | 4 | `void *` | `vft_anim_sub` | ctor @ `0x0041ca09`; `TM_BindSequence(this+0x98,…)` @ `0x0041ca67` |

### CAnim decomposition — `+0x6a..+0x73` (round 3 task 48)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x68 | 1 | `byte` | `view_state_68` | `FUN_00416590@0x00416590` |
| 0x69 | 1 | `byte` | `draw_flag` | `CTeleportPoint_Ctor` — `MOV [ESI+0x69], 1` @ `0x0041cb37` |
| 0x6a | 1 | `byte` | `view_state_6a` | `FUN_00416590@0x00416590` |
| 0x6b | 1 | `byte` | `view_flag_6b` | `CRadio_OnMouseDown@0x00402ff0` (CDSView family) |
| 0x6c | 4 | `uint` | `dwView_aux_6c` | `CRadio_Render@0x00403ba3` |
| 0x6d | 3 | — | *(pad / part of `+0x6c`)* | no isolated xref |
| 0x70 | 1 | `byte` | `gaming_slot_id` | `FUN_00416590` — `= 0xff` |
| 0x71 | 3 | — | `pPad_71` | no xref in game `.text` |

### Gameplay aliases in `+0x64..+0x84` (share CAnim bytes)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x64 | 4 | `CBulanek *` | `overlap_entity` | `CTeleportPoint_OnEvent@0x0041fed0` — `MOV ECX,[ESI+0x64]` @ `0x0041feff` (consumer). **Init:** `CDSChained_ctor` `param_1[0x19]=0` @ `0x00403366` (CAnim ctor chain). **No** store in `CTeleportPoint_Ctor` / `CBulanek_CreateRespawnTeleportPair`; owner CBulanek* → `gaming_host` @ `+0x84` via `CBulanek_AddEntity@0x0041a390` (`MOV [entity+0x84],ESI` @ `0x0041a3a6`) |
| 0x68 | 1 | `byte` | `gate_active` | `OnEvent` — `if (this[0x68]==0)` |
| 0x74 | 4 | `int` | `dest_x` | `CTeleportPoint_Ctor` — `MOV [ESI+0x74], …` @ `0x0041cb18` |
| 0x78 | 4 | `int` | `dest_y` | @ `0x0041cb1b` |
| 0x7c | 4 | `int` | `src_x` | @ `0x0041cb1e` |
| 0x80 | 4 | `int` | `src_y` | @ `0x0041cb21` |
| 0x84 | 4 | `void *` | `game_context` | `TriggerTeleportFX` — `MOV ECX,[ECX+0x84]` @ `0x0041fcc5` (alias of `gaming_host`) |

### CAnim decomposition — `+0x85..+0xa3`

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x84 | 4 | `void *` | `gaming_host` | `FUN_00416590` — `= 0` (see `game_context` alias above) |
| 0x88 | 4 | `void *` | `vftable_IDSUpdated` | `CTeleportPoint_Ctor` |
| 0x8c | 4 | `void *` | `vftable_IDSAnim` | `CTeleportPoint_Ctor` |
| 0x90 | 8 | — | `pPad_90` | no `CAnim`-sized consumer |
| 0x98 | 4 | `void *` | `vftable_anim_sub` | ctor + `TM_BindSequence` / `TM_SetTrack` |
| 0x9c | 4 | `int` | `anim_inner_field_9c` | `CAnim_SubobjectCtor@0x00419870` |
| 0xa0 | 4 | `int` | `anim_inner_field_a0` | same |
| 0xa4 | 4 | `int` | `fx_coord` | `TriggerTeleportFX@0x0041fca0` — read/write `+0xa4` (overlays `anim_inner_field_a4`) |

### CAnim decomposition — `+0xa8..+0xd3` (`track_mgr` ctor blob)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0xa8 | — | `CDSObject` head | `track_mgr` | `ConstructTrackManager@0x00439c70` on `this+0xa8` from `CAnim_SubobjectCtor` |
| 0xc4 | 4 | `int` | `track_mgr.field_c4` | `ConstructTrackManager` — `param_1[7]=0` |
| 0xc8 | 4 | `int` | `track_mgr.field_c8` | `param_1[8]=0` |
| 0xcc | 4 | `int` | `track_mgr.field_cc` | `param_1[9]=0` |
| 0xd0 | 4 | `int` | `track_mgr.kind_d0` | `param_1[10]=8` |
| 0xa8..0xc3 | 28 | — | `track_mgr.head` | ctor only |
| 0xd1..0xd3 | 3 | — | *(high bytes of `+0xd0`)* | — |

| 0xd4 | 4 | `int` | `fx_threshold` | `TriggerTeleportFX` — `CMP [ECX+0xd4], 2` @ `0x0041fca0`; init `0xffffffff` in `ConstructTrackManager` (`param_1[0xb]`) |

## Ghidra apply

```
get_struct_layout CTeleportPoint → Size: 248 (0xf8)
  canim_base      CAnim               @ 0
  pPartner_node   CTeleportPoint *    @ 0xf0
  bGate_orientation byte              @ 0xf4
```

**Slice 21 (2026-05-30):** `canim_base` typed as embedded **`CAnim` (240 B)** (was `undefined1[240]`). `CAnim` repaired to **0xf0** (`pTrack_manager` @ `+0xa8`); `pLinked_bulanek` @ `+0x64`, teleport rects `nDest_x/y`, `nSrc_x/y` @ `+0x74..+0x80`, `nFx_coord` @ `+0xa4`. Prototypes: `CTeleportPoint::OnEvent` @ `0x0041fed0`, `TriggerTeleportFX` @ `0x0041fca0`, `CBulanek_CreateRespawnTeleportPair` @ `0x0041d090`. `save_program bulanci.exe`.

**Agent todo 29 (2026-05-30):** `pPartner_node` retyped **`CTeleportPoint *`**; `CTeleportPoint_Ctor(CTeleportPoint *this, CTeleportPoint *partner, CBulanek *host, uchar gate_orientation)` @ `0x0041c9a0`; `CTeleportPoint_OnEvent` @ `0x0041fed0`. Partner visibility (`param_2 == -1`, `0x0041feeb`): asm `MOV EAX,[ESI-8]` loads **predecessor gate’s** `pPartner_node` when paired gates are heap-adjacent (`sizeof == 0xf8`); decompile `in_EAX = this[-1].pPartner_node` then `(in_EAX->canim_base).dwView_flags` @ `+0x44`. Own partner for ctor/vtable: `[ESI+0xf0]` @ `0x0041ca77`. Plate + insn comments document paired stride; `save_program`.

Leaf path: `CBulanek_OnTakeDamage` → `CBulanek_CreateRespawnTeleportPair` — dual `OperatorNew(0xf8)` + paired `CTeleportPoint_Ctor(partner, host, orient 1|0)` (`0x0041d175`, `0x0041d1c7`).

## Follow-up

- `OnEvent` `param_2 == 0` branch still uses `this[-1].canim_base.*` for predecessor partner vtable (`0x0041ff16`); same paired-alloc convention.
- See [round3_task_48_report.md](./round3_task_48_report.md).

## UNK

- `track_mgr` interior (`+0xac..+0xc3`, `CDSUpdatedItem` layout).
- `pOverlap_entity` @ `+0x64` (hex) — **no `.text` store** after respawn ctor/`AddEntity` (R4: only `CDSChained_ctor` / `InitWithRect` init `.text` writers). `OnEvent param_2==0` consumes slot as `CBulanek*`; runtime fill UNK (collision/net/script). **`CBulanek_UpdateStateFromParams` does not write this field** (writes CBulanek `pHeader_tail_58+0xc`).
- `pPartner_node` @ `+0xf0` — respawn: `CreateRespawnTeleportPair` stores **damaged `CBulanek*`** (both gates); map pairs may store other `CTeleportPoint*` (todo 29).
- `+0x6d..+0x6f`, `+0x71..+0x73`, `+0x90..+0x97` — no dedicated consumers on `CAnim`-sized instances.

**Agent todo 22 (2026-05-30):** R3: overlap vs `gaming_host` split; [round3_task_22_report.md](./round3_task_22_report.md). **R4:** Respawn `pPartner_node` = damaged `CBulanek*`; no respawn writer to `pOverlap_entity`; `UpdateStateFromParams` clarified; OnEvent `0x0041ff16` = predecessor `+0x6c`; [round4_task_22_report.md](./round4_task_22_report.md). `save_program`.
