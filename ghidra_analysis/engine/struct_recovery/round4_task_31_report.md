# Round 4 — Task 31 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 31 |
| **title** | Trace FrameTimeHint u16 and IDSAnim slots 0..3 |
| **source** | `agent_todos_50_r4.json` (handoff from R3 todo 31) |
| **supersedes_todo_id** | 31 |
| **prior** | [round3_task_31_report.md](./round3_task_31_report.md) |
| **types** | `CDSFlxFile`, `ODSImage`, `CBitmap`, `CGunMouse`, `CWeapon`, `IDSAnim` |

## Status

**DONE** — Decompiled all five `vfn[4]` subscribers plus shared `IDSAnim` thunks at `CBitmap::vftable` (`0x00481f00`); pinned per-class `u16` semantics; Ghidra disasm labels on slots `[0..3]`.

## Evidence

### FLX opcode `0x0C` → `u16` source

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Chunk body is one `u16` | `CDSFlxFile::DecodeFrame` @ `0x00432dc5` | `case 0x0c`: `BroadcastFrameTimeHint(param_3, (uint)*puVar1)` — first `u16` after 5-byte inner header |
| Fan-out signature | `BroadcastFrameTimeHint` @ `0x00436ef0` | `CALL [subscriber.vfn+0x10](consumer, param_1)` — `param_1` is that `u16` |
| Not track delay | whole binary | No `trackMgr+0x44` writer on this callgraph (R3); unpacker names field `durationTicks` but documents side-channel only |

### `u16` consumption by subscriber (`vfn[4]`)

| Subscriber | func@addr | `u16` semantics |
|------------|-----------|-----------------|
| Default / embed | `CDSView_OnMouseStub` @ `0x00416770` | Ignored (empty return) |
| `CBitmap` / `CGameView` | `CBitmap_FireOnBitmapEvtFromView` @ `0x00419280` | **`eventCode`** → `CLevelScript_FireOnBitmapEvt_FromView` → `CDSScript::CallExport(..., 3, 2, …)` **Local1**; **Local0** = view `gaming_slot_id` @ `+0x70` |
| `CGunMouse` | `CGunMouse_OnAnimTick` @ `0x00423bd0` | **Only `0xFFFF` (`-1`)** — random `SetCurrentTrack` on dot sprite + optional `TM_Play`; other values no-op |
| `CWeapon` | `CWeapon::Fire` @ `0x004212b0` | **Weapon-kind dispatch:** kind `2` + `u16==0` → spawn `CMina`; kind `4` + `u16!=0xFFFF` → `CShot` with shot kind `(u16+3)`; kind `5` + `u16==0` → pistol shot; **`u16==0xFFFF`** → ammo-empty / bot-damage branch |

### `IDSAnim` vtable slots `[0..3]` @ `0x00481f00` (`CBitmap` / `CGameView` `pVf_odsimage`)

| Slot | func@addr | Role |
|------|-----------|------|
| `[0]` | `IDSAnim_NotifyEvents` @ `0x00438f20` | Walk `param_3` × 16-byte records; `CALL [IDSEventHandler+0x24]` per record |
| `[1]` | `IDSAnim_BindUserData` @ `0x00438f60` | `FUN_0042cc30` on chained face with `(param_2[0], param_2[1])` |
| `[2]` | `IDSAnim_SetSequence_thunk` @ `0x004391d0` | `ODSImage__SetImage(odsImage, (CDSImage *)param_2)` — binds decode consumer |
| `[3]` | `CDSView_EmptyHook27` @ `0x00438f80` | No-op stub |

**Note:** `CGunMouse` `pVf_odsimage` @ `0x00483794` repatches slot `[1]` to `CGunMouse_OnMouseMove` @ `0x00423900` (FLX opcode **0x0A** `NotifyMove` path), not `IDSAnim_BindUserData` — class-specific vtable.

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_disassembly_comment` | `0x00481f00`..`0x00481f0c` | IDSAnim `vfn[0..3]` labels on `CBitmap::vftable` dwords |
| `set_decompiler_comment` | `0x00419280`, `0x00423bd0`, `0x004212b0` | Per-subscriber `u16` semantics |
| `decompile_function` | broadcast / decode / subscribers / `IDSAnim_*` | Verified tables above |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSFlxFile.md](./CDSFlxFile.md) — `u16` subscriber semantics table
- [CBitmap.md](./CBitmap.md) — IDSAnim slots `[0..3]` resolved
- [ODSImage.md](./ODSImage.md) — R4 `u16` cross-ref
- [CGunMouse.md](./CGunMouse.md) — `OnAnimTick` `0xFFFF` gate; custom `vfn[1]`
- [CWeapon.md](./CWeapon.md) — `Fire` FrameTimeHint `param_2` matrix
- [anim_runtime.md](../anim_runtime.md) — opcode `0x0C` subscriber semantics (brief)

## Remaining UNK

- Authoring intent of `eventCode` values in `OnBitmapEvt` (per-map script constants; not enumerated in binary).
- Whether any shipped sprite relies on non-`0`/`1`/`0xFFFF` `u16` values beyond the master-pack histogram (values are mostly `0` and `1` in assets).
- Full `CGunMouse` / `CWeapon` `pVf_odsimage` slots `[0..3]` naming beyond `CBitmap` canonical five-slot row (repatched per class).
