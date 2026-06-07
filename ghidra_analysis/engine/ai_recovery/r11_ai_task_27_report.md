# Round 11 — Task 27 report

## Task

| Field | Value |
|-------|-------|
| **id** | 27 |
| **worker** | 27 / 30 |
| **title** | Script `TranslateTo` — teleport/move `CBulanek` |
| **archetype** | `teleport` |
| **seed_address** | `0x004188e0` |
| **addresses** | `0x004188e0`, `0x0042cc80` |
| **acceptance** | Bytecode args; `SetPosition` call; used by level scripts for AI reposition |

## Status

**DONE** — Extended-script opcode handler, three `ReadSubExpr` arguments, null-guarded `CDSView__SetPosition` call, and rect math verified from live Ghidra decompile + disasm. Level-script usage in overlay `res_0000065856` correlates slot-9 script mover with global-var-relative coordinates.

## AI archetype

**Script teleport / instant reposition** for views bound to gaming slots (`0x04..0x1f`, `0x28..0x63`, `0x6c..0x7c` per [map_slots_spawner.md](../../gameplay/map_slots_spawner.md)). This is **not** the walk-step path (`FUN_0041af70` / `CBulanek_StepMovementAndCollision`); it snaps world bbox origin in one frame — same helper used by portal teleports, respawn, and net position sync.

Typical targets: `GetSlot(n)` → `CGaming.pEntitySlots[n]` (`CBulanek*`, `CObstacle*`, `CAnim*`, …); `CreateObstacle(...)` before `InsertView` / `BindToSlot`.

## Algorithm

### Script dispatch

| Field | Value |
|-------|-------|
| Extended opcode index | **58** (`tools/bulanci_unpack/bulanci_unpack.py`) |
| Sub-op in ext table | **13** |
| Handler | `CLevelScriptOpExt_TranslateTo` @ `0x004188e0` |
| Arg spec | `sub, sub, sub` — three nested bytecode expressions |
| Dispatch xref | `0x004af04c` (ext table DATA only) |

### `CLevelScriptOpExt_TranslateTo` (`0x004188e0`)

```c
CDSView *__cdecl CLevelScriptOpExt_TranslateTo(CDSScript *script);
```

**Disasm-proven control flow** (`0x004188e9`–`0x00418908`):

```text
TranslateTo(script):
  target = ReadSubExpr(script)          // ESI — often GetSlot(k) or CreateObstacle(...)
  x      = ReadSubExpr(script)          // EBX
  y      = ReadSubExpr(script)          // EAX (top of stack at call)
  if target != NULL:
    CDSView__SetPosition(target, x, y)  // __thiscall: ECX=target; stack (x, y)
  return target                         // EAX = target (enables InsertView(TranslateTo(...)))
```

`ReadSubExpr` @ `0x004384c0`: reads next opcode byte, dispatches `script->pDispatchTable[opcode](script)`, returns evaluated `int`/pointer in `EAX`.

### `CDSView__SetPosition` (`0x0042cc80`)

```c
void __thiscall CDSView__SetPosition(CDSView *this, int x, int y);
```

Preserves **width** and **height** from the current logical bbox (`+0x20..+0x2c`), moves **top-left** anchor to `(x, y)`:

```text
width  = nBbox_right  - nBbox_left   // [ECX+0x28] - [ECX+0x20]
height = nBbox_bottom - nBbox_top    // [ECX+0x2c] - [ECX+0x24]

rect.left   = x
rect.top    = y
rect.right  = nBbox_right  + (x - nBbox_left)
rect.bottom = height + y

(*this->pVftable_primary[8])(&rect)   // CDSView_AdaptDisplaySize @ 0x0042cae0
```

`AdaptDisplaySize` compares proposed rect via vfn+0x30, then applies vfn+0x2c (`CDSView_SetRect`) when changed — may deactivate/reactivate hidden widgets (`+0x44` sign bit path).

### Level-script examples (overlay `res_0000065856`)

| PC | Expression | Role |
|----|------------|------|
| `@0x08d5` | `TranslateTo(GetSlot(9), Add(GetGlobalVar(5), 13), Add(GetGlobalVar(6), -25))` | Repositions **slot-9 gun-mouse** anim relative to globals 5/6 (player-tracked coords) |
| `@0x0239` | `InsertView(TranslateTo(CreateObstacle(...), GetLocalVar(1), GetLocalVar(2)))` | Place obstacle at computed local position before world insert |
| `@0x02a2` | `BindToSlot(TranslateTo(CreateObstacle(...), GetLocalVar(1), GetLocalVar(2)), 7)` | Position then bind to slot 7 |

`GetSlot` @ `0x00416b50`: `ReadSubExpr` → slot index → `CGaming_GetObjectAtSlotSafe(gaming, idx)` where `gaming = script[1].field` (context pointer).

Slot **9** in script 65856: bedtime gun-mouse bound via `BindToSlot(..., 9)`; main loop uses `SetAnim(GetSlot(9), …)` for clip index — `TranslateTo` moves that bound view without walk animation ([agent_verify_01_mouse_clips.md](../../asset_catalog/naming/agent_verify_01_mouse_clips.md)).

### Relation to other teleport paths

| Caller | Address | Target |
|--------|---------|--------|
| `CLevelScriptOpExt_TranslateTo` | `0x00418908` | Script-bound views / obstacles |
| `CTeleportPoint_OnEvent` | `0x0041ff79` | Overlapping `CBulanek` via portal |
| `CGaming_RespawnPlayerAtSafeLocation` | `0x0041a23e`, `0x0041a257` | Random safe spawn rect |
| `CGaming_OnNetMsg_t0d_PlayerState` | `0x00420a14`, `0x00420a5a` | Remote peer position |
| `CBulanek_SnapPositionToFacingAxis` | `0x00417934`, `0x00417948` | Axis-aligned facing snap (movement release) |

All share the same **`CDSView__SetPosition`** rect builder — script `TranslateTo` is the **level-designer-facing** entry for movers/hazards.

## Functions table

| Symbol | Address | Role |
|--------|---------|------|
| `CLevelScriptOpExt_TranslateTo` | `0x004188e0` | Script opcode 58 — read target,x,y; optional SetPosition |
| `CDSScript::ReadSubExpr` | `0x004384c0` | Recursive bytecode arg evaluator |
| `CLevelScriptOpExt_GetSlot` | `0x00416b50` | Common arg0: `CGaming_GetObjectAtSlotSafe` |
| `CGaming_GetObjectAtSlotSafe` | `0x00416810` | `pEntitySlots[idx]` if `idx < 0x80` |
| `CDSView__SetPosition` | `0x0042cc80` | Absolute top-left snap preserving extent |
| `CDSView_AdaptDisplaySize` | `0x0042cae0` | Primary vfn[8] — clamp + `SetRect` |
| `CDSView_SetRect` | `0x0042c480` | Writes `+0x20..+0x2c`, walks child chain |

## Struct fields

| Offset | Field | Use in `SetPosition` |
|--------|-------|----------------------|
| `+0x20` | `nBbox_left` / `origin_x` | Subtracted from `x` to preserve width |
| `+0x24` | `nBbox_top` / `origin_y` | Subtracted for height term |
| `+0x28` | `nBbox_right` | `right + (x - left)` |
| `+0x2c` | `nBbox_bottom` | `(bottom - top) + y` |
| `+0x00` | `pVftable_primary` | Slot 8 → `AdaptDisplaySize` |
| `+0x70` | `bPlayerSlot` | Set by `BindToSlot`; `GetSlot` returns bound entity |

`CBulanek` shares the `CDSView`/`CGameView` prefix through `+0x87` ([CBulanek.md](../struct_recovery/CBulanek.md)).

## Ghidra deltas

| Action | Address | Result |
|--------|---------|--------|
| `set_decompiler_comment` | `0x004188e0` | Documents ReadSubExpr→ESI/EBX/EAX wiring and SetPosition call |
| `save_program` | `bulanci.exe` | Saved after comment |

Prior rounds already named `CLevelScriptOpExt_TranslateTo` and `CDSView::SetPosition` / `CDSView__SetPosition`.

## Decomp fixes

| Issue | Evidence | Fix status |
|-------|----------|------------|
| Ghidra leaves `this` uninitialized after first `ReadSubExpr` | Disasm: `MOV ESI,EAX` @ `0x004188f0`; `TEST ESI,ESI` @ `0x00418900`; `MOV ECX,ESI` before call @ `0x00418906` | **Documented** via plate comment; decompiler still shows `extraout_EAX` for x/y — manual pseudocode required |
| Return type `CBulanek*` in headers vs `CDSView*` in Ghidra | Handler returns any view pointer; obstacles/anim use same path | Accept `CDSView*` as canonical; `CBulanek*` is a common dynamic type |

Corrected pseudocode for reimplementation:

```c
CDSView *CLevelScriptOpExt_TranslateTo(CDSScript *script) {
    CDSView *target = (CDSView *)CDSScript_ReadSubExpr(script);
    int x = CDSScript_ReadSubExpr(script);
    int y = CDSScript_ReadSubExpr(script);
    if (target)
        CDSView_SetPosition(target, x, y);
    return target;
}
```

## Frida

Not run — static disasm + script disassembly sufficient for opcode args and SetPosition math.

## Remaining UNK

| Item | Notes |
|------|-------|
| Net replication from script-only `TranslateTo` | No direct `NetSend` in handler; whether `AdaptDisplaySize`→`SetRect` propagates to `CGaming_OnNetMsg` path for human slots **not traced** this task |
| `AdaptDisplaySize` clamp rect source | Decompile shows `CDSRect_ClampToBounds` on stack locals — parent bounds interaction for off-map script coords **open** |
| Non-`CBulanek` targets | `CreateObstacle` + `TranslateTo` proven in script asm; collision/spatial refresh after obstacle move **not traced** |
