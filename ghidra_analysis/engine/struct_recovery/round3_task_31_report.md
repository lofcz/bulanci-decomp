# Round 3 — task 31 report

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 31 |
| **title** | Name FLX opcode 0x0C subscriber vfn[4] in Ghidra |
| **source** | `agent_todos_50_r3.json` (backlog; supersedes R2 todo 31 trace) |
| **types** | `CDSFlxFile`, `ODSImage`, `CDSImage`, `CBitmap`, `CGunMouse`, `CWeapon` |

**Ghidra actions (from todo):** plate comments on subscriber vfn[4] targets; rename any remaining `FUN_*` slot[4]; decompiler comment on `BroadcastFrameTimeHint@0x00436ef0`; `save_program`.

## 2. Status

**DONE** — All five `pVf_odsimage` / IDSChained slot[4] targets were already named from R2; R3 added plate/disassembly catalog comments on subscribers and vtable dword sites; struct docs cross-linked.

## 3. Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Opcode 0x0C handler | `CDSFlxFile_DecodeFrame` @ `0x00432dc5` | `BroadcastFrameTimeHint(consumer, u16)` — R2 PRE_COMMENT |
| Fan-out dispatch | `BroadcastFrameTimeHint@0x00436ef0` | `for` over `consumer+0x40`; `CALL [subscriber.vfn+0x10](consumer, u16)` — decompile confirms `+0x10` = slot[4] |
| Subscriber registration | `ODSImage__SetImage@0x00439100` | Inserts `ODSImage+4` into `CDSImage.m_slotVector` @ `+0x38` |
| Default vf slot[4] | data `@0x00481ac4` | dword `0x00416770` → `CDSView_OnMouseStub` |
| CBitmap / CGameView vf | data `@0x00481f10` | dword `0x00419280` → `CBitmap_FireOnBitmapEvtFromView` |
| CDSBitmap / CMovieView vf | `@0x004830f0`, `@0x004831d8` | both `0x00416770` (`vf` `0x4830e0` / `0x4831c8`) |
| CGunMouse vf | data `@0x004837a4` | dword `0x00423bd0` → `CGunMouse_OnAnimTick` |
| CWeapon vf | data `@0x00481ee4` | dword `0x004212b0` → `CWeapon_Fire` |
| Not timing override | whole binary | No store to track-mgr `+0x44` on this callgraph (R2 sweep) |

### Subscriber catalog (vfn[4] @ `vtable+0x10`)

| `pVf_odsimage` | slot[4] | Ghidra symbol | Role |
|----------------|---------|---------------|------|
| `0x00481ab4` | `CDSView_OnMouseStub@0x00416770` | plate: FrameTimeHint no-op stub | default `ODSImage_ctor` |
| `0x00481f00` | `CBitmap_FireOnBitmapEvtFromView@0x00419280` | plate: OnBitmapEvt bridge | `CGameView_ctor` repatch `+0x8c` |
| `0x004830e0` / `0x004831c8` | `CDSView_OnMouseStub@0x00416770` | same stub | anim/movie embed |
| `0x00483794` | `CGunMouse_OnAnimTick@0x00423bd0` | plate: cursor anim tick | `CGunMouse_ctor` |
| `0x00481ed4` | `CWeapon_Fire@0x004212b0` | plate: weapon fire dispatch | weapon overlay |

## 4. Ghidra deltas (R3)

- Verified plate + PRE comments on `BroadcastFrameTimeHint`, `CDSView_OnMouseStub`, `CBitmap_FireOnBitmapEvtFromView`, `CGunMouse_OnAnimTick`, `CWeapon_Fire` (from R2; no `FUN_*` renames needed)
- `set_disassembly_comment` @ `0x00481ac4`, `0x00481f10`, `0x004830f0`, `0x004831d8`, `0x004837a4`, `0x00481ee4` — vfn[4] FrameTimeHint subscriber labels on vtable dwords
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CDSFlxFile.md](./CDSFlxFile.md) — opcode 0x0C subscriber table (pre-existing this session)
- [ODSImage.md](./ODSImage.md) — new § FLX opcode 0x0C / `vf_odsimage` vfn[4]
- [CBitmap.md](./CBitmap.md) — IDSAnim slot [4] FrameTimeHint cross-ref
- [CWeapon.md](./CWeapon.md) — vtable slot 4 FrameTimeHint note; `CWeapon_Fire` symbol name

## 6. Remaining UNK

- True runtime semantics of the u16 hint (profiling / script cadence / deprecated) — fan-out proven, purpose not.
- IDSAnim slots `[0..3]` at `0x00481f00` unchanged.
- Whether `CGunMouse_OnAnimTick` uses `param_2` (u16) or ignores it (not traced this task).
