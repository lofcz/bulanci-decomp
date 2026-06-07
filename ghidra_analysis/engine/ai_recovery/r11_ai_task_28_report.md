# Round 11 — AI Task 28 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 28 |
| **worker** | 28 / 30 |
| **title** | Script `SetAnimDirection` — rotation / facing |
| **archetype** | `rotation` |
| **seed_address** | `0x00418980` |
| **addresses** | `0x00418980`, `0x0041c610` |
| **acceptance** | How script sets anim direction on bound views; link to track manager |

## Status

**DONE** — Extended-script opcode handler disasm/decompile verified; contrasted with opcode **63** `SetAnim` (master-pack facing path) and `CBulanek_ResolveAndBindAnimTrack` (player corpse bind). Ghidra plate comment applied @ `0x00418980`.

## AI archetype

**`rotation` / script-facing** for **script-bound anim views** in slot gaps `0x04..0x1f`, `0x28..0x63`, `0x6c..0x7c` ([map_slots_spawner.md](../../gameplay/map_slots_spawner.md)). These movers (`CreateAnim` + `BindToSlot`) have **no** `CBulanek::SetFacingTrack` path — facing is bytecode-driven.

**Important:** shipped level scripts use **`SetAnim`** (opcode 63), **not** `SetAnimDirection` (opcode 78). Zero `SetAnimDirection` tokens in unpacked `*.script.asm` corpus.

## Algorithm

### Script dispatch

| Field | Value |
|-------|-------|
| Extended opcode index | **78** (`tools/bulanci_unpack/bulanci_unpack.py`) |
| Sub-op in ext table | **33** |
| Handler | `CLevelScriptOpExt_SetAnimDirection` @ `0x00418980` |
| Arg spec | `sub, sub` — view expression + direction expression |
| Dispatch xref | `0x004af09c` (ext table DATA only) |
| Size | `0x28` bytes (`mapping.csv`) |

### `CLevelScriptOpExt_SetAnimDirection` (`0x00418980`)

```c
int __cdecl CLevelScriptOpExt_SetAnimDirection(CDSScript *script);
```

**Disasm-proven control flow** (`0x00418980`–`0x004189a7`):

```text
SetAnimDirection(script):
  view      = ReadSubExpr(script)              // ESI — often GetSlot(k)
  direction = ReadSubExpr(script)              // AL = low byte of EAX
  if view != NULL:
    drawable = *(void**)(view + 0x94)          // pOds_drawable on CAnim
    *(uint8_t*)(drawable + 0x18) = direction
  return view                                // EAX = ESI
```

`ReadSubExpr` @ `0x004384c0`: reads opcode byte → `pDispatchTable[opcode](script)`; result in `EAX`. Second call’s direction is consumed as **`AL`** only (`MOV [ECX+0x18], AL` @ `0x004189a0`).

**Does not:**

- Call `TM_SetTrack` / `SetCurrentTrack` on `view+0x98` anim inner
- Touch `CAnim::track_manager` @ `+0xA8` (`CDSVideoPlayer`)
- Invoke `CBulanek_SetFacingTrack` @ `0x004197b0`

### Contrast: opcode 63 `SetAnim` — **actual** master-pack facing API

| | `SetAnimDirection` (78) | `SetAnim` (63) |
|--|---------------------------|----------------|
| Handler | `0x00418980` | `0x00416b70` |
| Args | `sub, sub` | `sub, sub, sub` |
| Effect | Byte store only | Full track switch + optional play |
| Target | `*(view+0x94)+0x18` | `TM_SetTrack(view+0x98, trackIdx, flags, NULL)` |

**`CLevelScriptOpExt_SetAnim` (`0x00416b70`)** — decompile:

```c
ReadSubExpr(); ReadSubExpr(); ReadSubExpr();
TM_SetTrack((void*)(view + 0x98), trackIdx, flags, NULL);
return view;
```

`TM_SetTrack` @ `0x004391e0` → `SetCurrentTrack(animInner+0x10, trackIdx, autoplay)` on the embedded `CDSVideoPlayer` inside the anim subobject ([anim_runtime.md](../anim_runtime.md)).

### Master-pack examples (opcode 63, not 78)

| Script | PC | Call | Role |
|--------|-----|------|------|
| `res_0000065856` | `@0x0675` | `SetAnim(GetSlot(4), GetLocalVar(0), 1)` | Butterfly slot 4 — random clip 0..1 |
| `res_0000065856` | `@0x069b` | `SetAnim(GetSlot(6), Rand(0,3), 1)` | Enemy decor — 4 facing clips |
| `res_0000065856` | `@0x091c` | `SetAnim(GetSlot(9), GetGlobalVar(7), 1)` | Gun-mouse — clip index 0..7 (facing L/R × action) |
| `res_0000065856` | `@0x09e8` | `SetAnim(GetSlot(4), IntConst(4), 1)` | Mimic reveal — clip index 4 |

Luau reimplementation: `engine.set_anim(view, trackIdx, flags)` maps to **`SetAnim`**, not `SetAnimDirection`.

### `CreateAnim` initial facing (opcode 45)

`CLevelScript__CLevelScriptOpExt_CreateAnim` @ `0x0041d6e0` reads initial track from **`ReadU8`** after origin args, binds FLX sequences via `TM_BindSequence(&view->vftable_anim_sub, …)`, then:

```text
TM_SetTrack(&view->vftable_anim_sub, trackIdx /*u8*/, initialPlayMode /*u8*/, NULL)
```

Same `view+0x98` anim-inner path as `SetAnim`.

### Link to `CBulanek_ResolveAndBindAnimTrack` (`0x0041c610`)

Shared concept: **facing index 0..3** (sometimes extended 0..7 for multi-clip script anims). **Different mechanism:**

| Path | Entity | Facing update |
|------|--------|---------------|
| Script `SetAnim` | `CAnim` / slot-bound decor | `TM_SetTrack` → `nCurrentTrackIdx` @ `track_manager+0x2C` (`CAnim+0xD4`) |
| Script `SetAnimDirection` | `CAnim` (rare / unused in pack) | `*(uint8_t*)(pOds_drawable+0x18) = dir` — **no track-manager call** |
| Death bind | `CDeath` corpse (`CAnim` prefix) | `SetFacingTrack(host, facingTrack, 0)` → `CBulanek+0xA8` `videoTrackManager`; then `TM_BindSequence` / `TM_SetTrack` on corpse `+0x98` |

**`CBulanek_ResolveAndBindAnimTrack`** (R11 task 11, live decompile):

```text
ResolveAndBindAnimTrack(CDeath* corpse):
  host = corpse->pHost                    // CBulanek victim
  if corpse->bM_tourneyFlag == 0:         // walk death sheet
      idx = corpse->dwM_modeFlags + host->videoTrackManager.nCurrentTrackIdx * 4
      scan gABulanekWalkAnimSheet forward/back for non-zero resId
      facingTrack = idx >> 2
  else:                                   // sit death
      facingTrack = corpse->dwM_modeFlags
      resId = g_dwResId_player_sit_facing_down[facingTrack]  // enemy table if slot 0x20..0x23
  SetFacingTrack(host, facingTrack, sendNet=0)
  load resId → TM_BindSequence(&corpse->vftable_anim_sub) → TM_SetTrack(0)
  palette / anchor / net band +0x74..+0x80
```

Corpse bind **reuses victim walk facing** (`nCurrentTrackIdx` at death) to pick the correct death-FLX row, then mirrors facing onto the live `CBulanek` track manager before animating the `CDeath` view. Script `SetAnimDirection` does **not** participate in this pipeline.

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x00418980` | `CLevelScriptOpExt_SetAnimDirection` | Ext opcode 78 — byte poke on drawable | Disasm `0x0041899a`–`0x004189a0` |
| `0x004384c0` | `CDSScript::ReadSubExpr` | Bytecode arg reader | Two calls in handler |
| `0x00416b70` | `CLevelScriptOpExt_SetAnim` | Ext opcode 63 — `TM_SetTrack` | Master-pack facing |
| `0x004391e0` | `TM_SetTrack` | Anim-inner track switch + optional `TM_Play` | Called from SetAnim |
| `0x00439eb0` | `SetCurrentTrack` | Updates `nCurrentTrackIdx@+0x2C` | Track-manager core |
| `0x0041d6e0` | `CLevelScriptOpExt_CreateAnim` | Alloc `CAnim`, bind FLX tracks | `TM_SetTrack` on `+0x98` |
| `0x00416b50` | `CLevelScriptOpExt_GetSlot` | `CGaming_GetObjectAtSlotSafe` | Typical arg0 |
| `0x004197b0` | `CBulanek_SetFacingTrack` | Player-like facing + weapon sync | Called from `ResolveAndBindAnimTrack` |
| `0x0041c610` | `CBulanek_ResolveAndBindAnimTrack` | Corpse FLX bind + facing reassert | `SetFacingTrack` @ `0x0041c6ca` |

## Struct fields

### `CAnim` view (script mover)

| Offset | Field | `SetAnimDirection` | `SetAnim` / track manager |
|--------|-------|--------------------|---------------------------|
| `+0x94` | `pOds_drawable` (`CDSImage*`) | **Dereferenced** — byte written at `drawable+0x18` | ODSImage mixin; `TM_SetCurrentSequence` consumer |
| `+0x98` | `vftable_anim_sub` | Not used | `TM_SetTrack` `this` pointer |
| `+0xA8` | `track_manager` (`CDSVideoPlayer`, 72 B) | Not used directly | `nCurrentTrackIdx` @ `+0xD4` abs |
| `+0x70` | `bGaming_slot_id` | Set by `BindToSlot` | `GetSlot` / `OnBitmapEvt` key |

### `CBulanek` (corpse bind only — not `SetAnimDirection` target)

| Offset | Field | `ResolveAndBindAnimTrack` use |
|--------|-------|------------------------------|
| `+0xA8` | `videoTrackManager` | Read `nCurrentTrackIdx` for walk-sheet index |
| `+0xA4` | `nFacingAxisExtent` | Used by `SnapPositionToFacingAxis` (task 11) |

## Ghidra deltas

| Action | Address | Result |
|--------|---------|--------|
| `set_decompiler_comment` | `0x00418980` | Opcode 78 semantics; contrast with SetAnim; zero pack usage |
| `save_program` | `bulanci.exe` | Saved after comment |

## Decomp fixes

| Issue | Evidence | Correction |
|-------|----------|------------|
| Decompiler shows `extraout_EAX` / `extraout_AL` | Disasm: `MOV ESI,EAX` @ `0x0041898f`; `MOV [ECX+0x18],AL` @ `0x004189a0` | Manual pseudocode: `view = ReadSubExpr(); dir = (uint8_t)ReadSubExpr();` |
| Dispatch note `view.@0x94 (anim).@0x18` | Asm loads **pointer** at `view+0x94`, writes `drawable+0x18` | Not a direct `view+0xAC` store; equals `pOds_drawable` field on `CAnim` ([CAnim.md](../struct_recovery/CAnim.md)) |
| Return type `int` | `MOV EAX,ESI` @ `0x004189a4` | Returns **view pointer** (same idiom as `SetAnim` / `TranslateTo`) |

Corrected pseudocode:

```c
void *CLevelScriptOpExt_SetAnimDirection(CDSScript *script) {
    void *view = (void *)CDSScript_ReadSubExpr(script);
    uint8_t dir = (uint8_t)CDSScript_ReadSubExpr(script);
    if (view) {
        void *drawable = *(void **)((uint8_t *)view + 0x94);
        *(uint8_t *)((uint8_t *)drawable + 0x18) = dir;
    }
    return view;
}
```

## Frida

Not run — handler is 6 instructions; master-pack corpus confirms `SetAnim` supersedes this opcode for all curated levels.

## Remaining UNK

| Item | Notes |
|------|-------|
| **Consumer of `drawable+0x18` byte** | No `.text` reader found that loads this byte after `SetAnimDirection`; may be dead opcode, editor-only, or consumed via indirect anim/ODS path not closed statically |
| **Semantic of `drawable+0x18`** | On `CDSFlxFile` meta face, `+0x18` is `nSeqFrameCountMinusOne` ([CDSFlxFile.md](../struct_recovery/CDSFlxFile.md)) — overwriting low byte would be hazardous; may only apply when `pOds_drawable` points at a different face |
| **Opcode 78 in non-master scripts** | Zero hits in `unpacked/`; CHistory/CHelp ext tables not audited |
| **Relation to `CBulanek` rotation** | No xref from `0x00418980` to `SetFacingTrack` / `ResolveAndBindAnimTrack`; player facing remains task-11 path |

## Doc cross-links

- R11 task **11** — `CBulanek_SetFacingTrack`, walk tables, corpse bind ([r11_ai_task_11_report.md](./r11_ai_task_11_report.md))
- R11 task **29** — slot gaps, `BindToSlot`, `OnBitmapEvt` ([r11_ai_task_29_report.md](./r11_ai_task_29_report.md))
- [script_dispatch_table.md](../script_dispatch_table.md) — opcode 78 tagged **e** (estimated name)
