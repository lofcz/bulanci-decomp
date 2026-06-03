# Round 6 — Logic Task 02 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **title** | Logic cluster: FUN_0041af70 + scheduler/gaming neighbors |
| **range** | `cluster_41ab` (`0x0041a`–`0x0041b`) |
| **seed_address** | `0x0041af70` |

## Status

**PARTIAL** — Seed `FUN_0041af70` control flow, callers, and `CBulanek*` typing are **proven** (Ghidra decompile + disasm + `bulanci.ghidra.exe.c`). Neighbor addresses `0x0041a520`–`0x0041a600` are **audio vtable thunks** already named (R5 worker 19). **Ghidra MCP disconnected** before `rename_function_by_address` / `save_program`; recommended mutations are listed below.

## Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0041af70` | `FUN_0041af70` | **`CBulanek` movement/collision step** — weapon/track anchor delta → `rect_Intersect` normalize → update walk embed (`+0x104`..`+0x118`, `+0x15c` zero) → copy/offset collision rects → `CBulanek_ClampMoveRectByCollision` → primary vtable `+0x20` / `+0x24` position commit | **Disasm:** `MOV ESI,ECX` @ `0x0041af76`; callees `0x00433200`, `0x00417410`, `0x00417460`, `0x0042f1e0`, `0x004195f0`; `RET` with `AL=1`. **Decompile:** `__fastcall` `CBulanek *param_1`. **Xrefs (3):** see callers table |
| `0x0041a520` | `CDSWav_GetTypeInfo` | `IDSReferenced` meta stub — `MOV EAX,0x4b8410; RET` | R5 w19 vtable `0x482344` slot 0; no code xrefs |
| `0x0041a530` | `CDSWav_ScalarDeletingDtor_thunk` | MI adjustor `ECX-0x18` → `CDSWav_ScalarDeletingDtor@0x0041bbe0` | R5 w19: `SUB ECX,0x18; JMP 0x41bbe0`; DATA @ `0x00482318` |
| `0x0041a540` | `CDSWav_ScalarDeletingDtor_thunk_Sub4` | MI adjustor `ECX-4` → same scalar dtor | R5 w19: `face_8slots` `0x482320` slot 3 |
| `0x0041a550` | `CDSObject::CDSObject_dtor` | Generic chained subobject destructor (not gaming) | Xref `MOV [ESI],0x47f6a8` @ `0x0041a5a5` in export; out of CBulanek band |
| `0x0041a5c0` | `CDSWavStream_GetTypeInfo` | Class-43 factory typeinfo stub — `MOV EAX,0x4b843c; RET` | R5 w19: `IDSReferenced` `0x4823c0` slot 0 |
| `0x0041a5d0` | `CDSWavStream_ScalarDeletingDtor_thunk_Sub4` | MI adjustor `ECX-4` → `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | R5 w19 disasm |
| `0x0041a5e0` | `CDSWav_ReleaseChild_thunk_Sub30` | MI adjustor `ECX-0x30` → `CDSWav_ReleaseRefcount@0x00433040` | R5 w19: `IDSChained6` slot 2 |
| `0x0041a5f0` | `CDSWav_ReleaseChild_thunk_Sub34` | MI adjustor `ECX-0x34` → same release | R5 w19: `IDSChained5` slot 2 |
| `0x0041a600` | `CDSWavStream_ScalarDeletingDtor_thunk_Sub30` | MI adjustor `ECX-0x30` → `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | R5 w19: `IDSChained6` slot 3 |

### Seed callers (`get_xrefs_to@0x0041af70`)

| Caller | Site | Context |
|--------|------|---------|
| `CBulanek_ApplyPickupEffect` | `0x0041eb28` | After weapon swap / pickup path when human player applies effect — `CALL` with `ECX = CBulanek*` (`bulanci.ghidra.exe.c` ~`0x0041ea90`) |
| `CWeapon_OnSchedulerEvent` | `0x0041b186` | Scheduler event on **`CBulanek+0xA0`** facet: `FUN_0041af70((CBulanek *)(param_1 - 0xa0))` (`0x0041b180`) |
| `CWeapon::Update` | `0x0041bf61` | When `pWeapon` track params (`dwParamA/B`, `pTrackHolder`) change and `bField_168 == 0`: `CALL` with `ECX = CBulanek*` (`0x0041bf00`) |

### Seed control flow (`0x0041af70`)

```
CBulanek* this (ECX)
  ├─ Read CWeapon* (+0xF8) → pTrackHolder anchor (pTrackHolder+4/+8) minus embed (+0x104/+0x108)
  ├─ Add CBulAnim* (+0xF0) chain fields → proposed top-left (local_38/34) + delta (local_30/2c)
  ├─ rect_Intersect / FUN_00433200(&origin, &delta)  → normalize; store -origin in +0x114/+0x118
  ├─ Fold origin (+0x20/+0x24), collision ints (+0x7C..+0x80), embed (+0x10C/+0x110, +0x15C)
  ├─ CBulanek_CopyCollisionRectLocal(this, local_10)
  ├─ Persist embed deltas; zero +0x15C..+0x167 (8 dwords)
  ├─ CBulanek_CopyCollisionRectLocal(this, local_20)
  ├─ CBulanek_OffsetCollisionRectByFacing(this, &origin, local_10, nInitialTrack@+0x148)
  ├─ nInitialTrack = videoTrackManager.nCurrentTrackIdx (+0xA8 subgraph)
  ├─ Scheduler_GetEventSlot(&videoTrackManager.scheduler, 0): if slot+8 bit0 → set nFacingAxisExtent@+0xA4
  ├─ CBulanek_OffsetCollisionRectByFacing(this, &proposed, local_20, nInitialTrack)
  ├─ CBulanek_ClampMoveRectByCollision(this, &origin, &proposed, local_20, local_10)
  ├─ vftable_primary+0x20(&proposed) — commit position if true
  └─ else vftable_primary+0x24(0,0) — fallback sync
  return 1 (AL)
```

**This typing:** `MOV ESI,ECX` @ entry; Ghidra already types `undefined4 __fastcall FUN_0041af70(CBulanek *param_1)`. **No `set_function_this_type` change required.**

**Rename (pending Ghidra):** `FUN_0041af70` → `CBulanek_StepMovementAndCollision` — matches callees `CBulanek_CopyCollisionRectLocal`, `CBulanek_OffsetCollisionRectByFacing`, `CBulanek_ClampMoveRectByCollision` and all three caller proofs above.

## Ghidra deltas

**None applied** (MCP `Not connected` after initial read-only batch).

**Recommended when MCP is back:**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `0x0041af70` → `CBulanek_StepMovementAndCollision` | Disasm + callee/caller names |
| `set_function_prototype` | `uchar __fastcall CBulanek_StepMovementAndCollision(CBulanek *this)` | Always returns `AL=1`; not `_Globals` free function |
| `set_decompiler_comment` | `0x0041af70` | Replace UNCERTAIN block with caller/callee list above |
| `save_program` | `bulanci.exe` | Once per agent |

Neighbors `0x0041a520`–`0x0041a600`: **skip** — R5 worker 19 already applied plates + save.

## Frida

**none** — static proof sufficient for movement/collision step and audio thunk neighbors.

## Remaining UNK

- Primary vtable slots **`+0x20` / `+0x24`** on `CBulanek` (`0x00481e54`): exact method names not resolved in this task (decompile shows indirect calls only).
- Ghidra field aliases `pReserved_preAmmo` / `pPad_postWalkEmbed` vs doc `abReserved_preAmmo` @ `+0x104` / `abPad_postWalkEmbed` @ `+0x15C` — offsets match disasm; naming consolidation deferred to struct batch.
- `CDSObject::CDSObject_dtor@0x0041a550` purpose in slice (generic dtor) not re-verified live in Ghidra this session.

## Struct doc updates

**none** — no new proven layout beyond existing [CBulanek.md](../struct_recovery/CBulanek.md) collision/origin fields.
