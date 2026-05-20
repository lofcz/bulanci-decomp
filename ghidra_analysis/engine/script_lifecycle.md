# `CLevelScript` lifecycle — every export and exactly when it fires

A `Script` resource (ClassID 2026) on disk is just a fixed-shape blob:

```
int32 codeLen
int32 nExports
int32 nVars
byte[codeLen] bytecode
int32[nExports] exports     ; bytecode offset of each entry point
```

The exports table is **positional**: the engine looks up an export by its
slot number, not by any name, and every slot has a fixed lifecycle
contract determined by the engine call-site that uses it. The compiler
does **not** sort the table by address — for example
`res_0000065859_2026_Script` (`V zajetí 8-bitu`) has its
`OnGameStart` body (`exports[10]`) at offset 1178, *between* `OnDeinit`
(`exports[2]`, offset 1149) and `OnBitmapEvt` (`exports[3]`, offset
1212). Treating the table as anything other than a positional array
shuffles the lifecycle labels apart from the bodies.

## Calling primitive

```
+0x00..+0x28   five vtable pointers (CDSObject / CDSScript / CLevelScript)
+0x08          pc                  current bytecode offset (saved/restored per call)
+0x10          bytecode            raw bytes
+0x1c          nExports            length of the exports table
+0x20          exports             int32* into bytecode for each entry
+0x24          returnFlag          set by opcode 17 (Return)
+0x2c..+0x1ec  dispatch table      void(*)(this) for opcodes 0..102
+0x42c         framePtr            current call's frame descriptor (sub-struct
                                   whose +0xC field is the locals buffer)
+0x434         argCount            active call's argc
+0x440         timerTable          refcounted timer/map slot container
                                   (opcodes 68..72, 81..82)
```

The only public entry to the bytecode is `CDSScript::CallExport`
(`FUN_00438c40`):

```c
int CDSScript::CallExport(this, int idx, int argc, void* argv) {
    if (idx < 0 || idx >= this->nExports) return 0;
    return CDSScript::Run(this, this->exports[idx], argc, argv);  // FUN_00438b30
}
```

`CDSScript::Run` (`FUN_00438b30`) does the function prologue:

```c
void CDSScript::Run(this, pc, argc, argv) {
    save_pc();
    this->pc = pc;
    int varCount = bytecode[pc++];                       // u8 prologue
    if (varCount + argc != 0) alloca((varCount + argc) * 4);
    if (argc != 0) memcpy(&locals[varCount], argv, argc * 4);
    push_frame({locals_ptr=&locals[0], varCount, argc});  // -> this->framePtr
    this->returnFlag = 0;
    while (!this->returnFlag) FUN_004384c0(this);         // dispatch one opcode
    pop_frame();
}
```

So inside a function `GetLocalVar(0)..GetLocalVar(varCount-1)` are
private locals (uninitialized), and
`GetLocalVar(varCount)..GetLocalVar(varCount+argc-1)` are the arguments
passed by the caller. Every export in the master pack happens to
declare `varCount=0`, so `GetLocalVar(0)` is the first argument.

## Engine call-site map — verified xref-by-xref

`FUN_00438c40` has exactly 13 callers in `bulanci.exe`. Eleven distinct
`(idx, argc)` pairs cover the 11 positional slots of a
`CLevelScript`. The other two callers are the help/history page
dialogs reusing slot 0 — see the bottom of this section.

Every row below was verified by reading the raw `PUSH idx; PUSH argc;
PUSH argv` sequence ahead of each `CALL FUN_00438c40` (the calling
convention is `__thiscall` with `this` in `ECX` and the remaining three
arguments pushed right-to-left in `cdecl` order, so the immediates seen
are `idx`, `argc`, and either an `argv` pointer or `NULL`).

| idx | argc | caller (engine fn)                         | trigger                                                                                          | argv shape (script side)                                          |
|----:|-----:|--------------------------------------------|--------------------------------------------------------------------------------------------------|-------------------------------------------------------------------|
|  0  |   1  | `CBulanci::FUN_00409f60` @ `0x00409fee`<br>`CHelpDlg::FUN_00421c10` @ `0x00421d33`<br>`CHistoryDlg::FUN_00422f70` @ `0x00423093` | Pre-game enumeration: each script in the level/help/history list is asked to describe itself. The single arg is a language id (`1` = Czech, anything else = English in the master pack scripts). | `Local0 = language`. The handler MUST write `globals[0..2]` with the localised display name, level type id, and GUID. |
|  1  |   0  | `CBulanci::FUN_0041ff90` @ `0x004202f1`    | Fires exactly once, very late in `CBulanci`'s constructor (right after `CGaming` is wired up).   | no args. Typical body: `LoadPreface`, `SetMusic`, `CreateImage` / `CreateAnim` / `CreateObstacle` / `InsertView` chains, then `DefineTraceArea` and `InsertBulanci`. |
|  2  |   0  | `CGaming::~CGaming` @ `0x0041b8b8`         | Fires exactly once during `CGaming`'s destructor (level teardown).                               | no args. Typical body: `FreeObject(GetGlobalVar(coll))`, `StrmDestroy(GetGlobalVar(stream))`. |
|  3  |   2  | `CBitmap::FUN_00418260` @ `0x00418285` (via vtable slot 4 thunk `FUN_00419280`) | A `CBitmap`-shaped child view (created by `CreateAnim` / `CreateImage`) reports an event — typically "animation playback hit a marker" or "frame completed". | `Local0 = view.@0x70` (the CGaming slot the view is bound to via `BindToSlot`), `Local1 = ushort eventCode`. Scripts usually `Switch(GetLocalVar(0); slot=>...)` over the slots they registered. |
|  4  |   3  | `CGameView::FUN_00418290` @ `0x004182be`   | A view dispatcher delivered message **`0xD7`** to a view (sent by `FUN_00417e80` after a `CExplosion` fragment lands in a slot, or by `CGame::ProcessNetMessage` case `0x0f`). The "*something just got placed here*" event. | `Local0 = receivingViewSlot`, `Local1 = highByte of the spawn parameter`, `Local2 = lowByte & 3` of that parameter. |
|  5  |   2  | `CGameView::FUN_004182d0` @ `0x004182f2`   | A view dispatcher delivered message **`0xD8`** — the companion of `OnSlotPlaced`, sent to a *second* slot when a placement displaces an existing occupant. | `Local0 = displacedSlot`, `Local1 = slotThatDisplaced`. |
|  6  |   1  | `FUN_00416a90` (timer-table vtable slot 4 thunk for `this+0x440`) @ `0x00416a9f` | A countdown started by `RegisterTimer(slotId, delay, flags)` (opcode 68) hit zero. The thunk subtracts `0x440` from `this` so the real `CLevelScript` receives the call. | `Local0 = slotId` (the same id passed to `RegisterTimer`). |
|  7  |   2  | `FUN_00417d20` (entered-branch) @ `0x00417d6d`<br>called via `FUN_00417dd0` from `FUN_00419e90` (per-frame trace test) | A player/entity moved **into** a rectangle previously registered with `DefineTraceArea(traceId, x1, y1, x2, y2, flags)` (opcode 83), and the bitmask bit for that entity was not set yet. | `Local0 = traceId` (first arg of the `DefineTraceArea` that defined this rectangle), `Local1 = entitySlot` (player slot 0..3, or `0x88 - slot` for slots 4..7, which is how the engine encodes the "outer" players). |
|  8  |   2  | `FUN_00417d20` (left-branch) @ `0x00417db0` | Symmetric to slot 7 — the entity left the rectangle (bitmask bit transitioned from set to clear). | Same shape as slot 7. |
|  9  |   1  | `CGame::FUN_004185a0` @ `0x004185b7` (the case-`0x15` body of the net dispatcher `CGame::FUN_00415290`; see `../netcode/net_protocol.md`) | Network peer sent a packet of type `0x15` (the "custom user payload" channel). The case-body re-uses the `CChainedStrm` at `cgame+0x1ac`, writes the message bytes into it, and fires this export so the script can parse the body itself. | `Local0 = streamHandle`. Scripts almost always start with `Switch(StrmRead(GetLocalVar(0), 1); ...)` to dispatch their own sub-opcode. |
| 10  |   0  | `CGaming::FUN_0041c140` @ `0x0041c163` (running-state transition, `param_1 != 0`) | The level transitions from "paused/loaded" to "running". Fires the first time at level start (right after `OnInit` has placed everything) **and again every time the game resumes from pause**. Right after this returns the engine starts the music (`this+0x354 → FUN_0043a9d0`) and arms engine-side timer slots 1 and 2 (`5000ms` / `10000ms` in solo / `7000ms` for level `0x10144`). | no args. Typical body: `IsServer? RegisterTimer(0, delay, flags)` to schedule the level's own first heartbeat. |

The xref table is exhaustive: dumping `get_xrefs_to FUN_00438c40` gives
exactly 13 sites, and after deduplicating the help/history dialogs onto
slot 0 there are no other (idx, argc) combinations. The engine therefore
exposes 11 lifecycle slots and no more — anything past `exports[10]`
would never fire.

## Disassembler labelling — positional, not address-sorted

`tools/bulanci_unpack/bulanci_unpack.py` walks the exports table in its
file order so the `fn export#N` label always matches the engine's slot
number. The end of each function body is computed from the *sorted*
address list (the next bytecode offset that any export references), so
out-of-order tables like `res_0000065859`'s — where `exports[10]` is
at offset 1178, well below `exports[3]` at offset 1212 — still get the
right body extents.

Side effect of the positional layout: the labels in the disassembly can
appear in any order — the listing follows the exports table, not
address ascending. The `@0xNNNN` after each label is the absolute
bytecode offset, which is what `Call` opcodes and switch-case targets
point at, so cross-references stay readable.

## Worked example — `res_0000065859_2026_Script.script.asm` ("V zajetí 8-bitu")

```
exports[ 0] = 0x0000  GetInfo       — switches Czech/English on Local0
exports[ 1] = 0x009f  OnInit        — places ~30 views, defines 7 trace areas
exports[ 2] = 0x047d  OnDeinit      — frees collection at global 12, kills stream at global 11
exports[ 3] = 0x04bc  OnBitmapEvt   — Switch(Local0; slot 9/10/12/14/16/20 => animation/anim-paired-view logic)
exports[ 4] = 0x0914  OnSlotPlaced  — Server-only: bridges Local0 (the spawned slot) into helper at 0x6e8
exports[ 5] = 0x092e  OnSlotDisplaced — empty Return(0)
exports[ 6] = 0x0935  OnTimer       — RNG-driven enemy spawn chain (scheduled by exports[10]'s RegisterTimer(0))
exports[ 7] = 0x0c5f  OnEnter       — Switch(Local0; traceId 0..6 => increment counters / spawn / teleport)
exports[ 8] = 0x0d4a  OnLeave       — Switch(Local0; 3..6 => decrement the counters incremented by OnEnter)
exports[ 9] = 0x0db5  OnNetCustom   — Switch(StrmRead(Local0, 1); 1..6 => dispatch this level's RPC sub-protocol)
exports[10] = 0x049a  OnGameStart   — IsServer? RegisterTimer(0, 3000, 6)
```

Notice how `OnGameStart` arms the slot-0 timer with flags `6 = autostart |
running`, which is what fires `OnTimer(0)` three seconds later and
kicks the enemy spawn loop.  The `OnEnter` zones 3..6 with their
counter-bookkeeping then keep `OnTimer`'s spawn logic gated by how many
players have crossed each gameplay-significant area, while `OnLeave`
keeps the counters in sync. The whole level is a tight little state
machine wired entirely through the lifecycle slots.

## Things that are NOT script lifecycle slots (commonly mistaken)

- **Engine-side timers.** `CGaming::FUN_0041c140` arms timer slots 1
  (5/10/7000 ms) and 2 (7000 ms for level `0x10144`) inside its own
  `this+0x440` table immediately after invoking `OnGameStart`. Those
  fire `CGaming::FUN_004168d0` callbacks, **not** script exports.
  Scripts that need their own ticks have to call `RegisterTimer`
  themselves and pick up `OnTimer(slotId)`.
- **Animation playback callbacks.** A `CDSAnim` calls back through its
  own vtable when a frame event tag fires; only when the receiving
  view is a `CBitmap` (created via `CreateAnim` / `CreateImage`) does
  the engine forward that into the script as `OnBitmapEvt`. Pure
  drawing primitives like `CreateObstacle` do not.
- **Net messages other than 0x15.** Other `CGame::ProcessNetMessage`
  cases (placement sync `0x0f` → `OnSlotPlaced`, generic state sync,
  chat, etc.) reach the script only indirectly. The only direct
  "custom" net hook the script has is `OnNetCustom` (slot 9).
- **`exports[0]` for help/history pages.** Help and history pages are
  also `Script` resources but only ever populate `exports[0]`; the
  remaining slots are absent because their ClassID never makes it
  through `CBulanci::FUN_00409f60`'s "is a level" branch.
