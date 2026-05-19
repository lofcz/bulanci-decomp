# CDSScript / CLevelScript dispatch table

A `CLevelScript` instance dispatches script opcodes through a per-instance
function-pointer table at `this+0x2c`. The table is built by two
back-to-back `memcpy`-style splices:

1. `CDSScript::CDSScript` (`0x00438390`)
   calls
   `FUN_00438310(this, 0, &PTR_LAB_004b0118, 0x2d)` to install opcodes 0..44
   from the base table at **`0x004b0118`** (45 entries, 180 bytes).
2. `CLevelScript::CLevelScript` (`0x004185c0`)
   calls
   `FUN_00438310(this, 0x2d, &PTR_FUN_004af018, 0x3a)` to install opcodes
   45..102 from the extension table at **`0x004af018`** (58 entries, 232 bytes).

Net dispatch table = 103 entries (opcodes 0..102 inclusive). Higher opcode
numbers seen in disassembled scripts (216, 217, 255, ...) are always parser
artifacts caused by mis-counting the previous opcode's arguments.

The dispatcher itself is `FUN_004384c0`:

```
uVar1 = FUN_00438380(this);                 // read next opcode byte
(*(this+0x2c + (uVar1 & 0xff)*4))(this);    // call handler
```

Argument readers used by the handlers:

| address      | reads        |
|--------------|--------------|
| `0x004384c0` | a sub-expression (recursive `FUN_004384c0`) |
| `0x00438350` | one `i32` from the bytecode stream |
| `0x00438360` | one `u16` |
| `0x00438380` | one `u8`  |

## Base table @ `0x004b0118` (opcodes 0..44)

| op | handler      | name (verified)   |
|---:|--------------|-------------------|
|  0 | `0x00438490` | IntConst          |
|  1 | `0x00438cb0` | Rand              |
|  2 | `0x004384a0` | GetGlobalVar      |
|  3 | `0x00438520` | GetLocalVar       |
|  4 | `0x00438cf0` | Add               |
|  5 | `0x00438d10` | Sub               |
|  6 | `0x00438d30` | Mul               |
|  7 | `0x00438d50` | Div               |
|  8 | `0x00438d70` | Clamp(min,max,v)  |
|  9 | `0x00438da0` | Min               |
| 10 | `0x00438dc0` | Max               |
| 11 | `0x004384e0` | SetGlobalVar      |
| 12 | `0x00438540` | SetLocalVar       |
| 13 | `0x00438510` | Goto              |
| 14 | `0x00438de0` | Call              |
| 15 | `0x004382c0` | ThisId (no args)  |
| 16 | `0x00438590` | GetLocalVarIdx    |
| 17 | `0x00438570` | Return            |
| 18 | `0x004385b0` | IfEqual           |
| 19 | `0x004385f0` | IfLess            |
| 20 | `0x00438630` | IfGreater         |
| 21 | `0x00438670` | IfLessEq          |
| 22 | `0x004386b0` | IfGreaterEq       |
| 23 | `0x004386f0` | IfNotEqual        |
| 24 | `0x00438730` | Switch            |
| 25 | `0x00438790` | Select (Switch-shape, returns value of matching pair) |
| 26 | `0x004387e0` | StrDup            |
| 27 | `0x00438830` | StrConst          |
| 28 | `0x00438850` | StrFree           |
| 29 | `0x00438870` | StrLen            |
| 30 | `0x004388a0` | StrCharAt         |
| 31 | `0x004388c0` | ShiftRight        |
| 32 | `0x004388e0` | ShiftLeft         |
| 33 | `0x00438900` | And (bitwise)     |
| 34 | `0x00438920` | Or (bitwise)      |
| 35 | `0x00438940` | Not (bitwise)     |
| 36 | `0x00438950` | Negate (arith.)   |
| 37 | `0x00438960` | StrmCreateMem     |
| 38 | `0x004389f0` | StrmDestroy       |
| 39 | `0x00438a10` | StrmWrite         |
| 40 | `0x00438a50` | StrmRead          |
| 41 | `0x00438a90` | StrmSeek          |
| 42 | `0x00438ac0` | StrmSetSize       |
| 43 | `0x00438af0` | StrmGetPos        |
| 44 | `0x00438b10` | StrmGetSize       |

## CLevelScript extension @ `0x004af018` (opcodes 45..102)

All 58 entries below have been verified by hand-disassembling the
handler. The "name" column is what the unpacker's disassembler prints
for each opcode; the column "shape" is the argument list the handler
reads off the bytecode stream before producing a result; "notes" hints
at the runtime behaviour we recovered.

Names without a footnote come from
`editor_il_spy/Editor.Scripts/Opcode.cs` (the editor only emits these
opcodes from its UI). Names tagged **e** are reverse-engineered: they
describe the engine call the handler forwards to. They are best-effort
labels rather than authoritative names from the original source — the
shape and the runtime effect are pinned down, but the original C++
identifier is unknown.

| op  | ext# | handler      | name                  | shape                | notes                                                          |
|----:|-----:|--------------|-----------------------|----------------------|----------------------------------------------------------------|
|  45 |    0 | `0x0041d6e0` | `CreateAnim`          | sub,sub,u8,u8,i32[n] | also writes a CAnim at this+0xF0; the `i32[n]` are CMenu image IDs |
|  46 |    1 | `0x0041d820` | `CreateImage`         | sub,sub,i32          | (x, y, imageID); allocates a CGameView wrapper                 |
|  47 |    2 | `0x00418800` | `CreateObstacle`      | sub,sub,sub,sub      | rectangular CObstacle (0x88 bytes)                             |
|  48 |    3 | `0x004187a0` | `SetObstacleBounds`   | sub*5                | mutates an existing CObstacle's bounds                         |
|  49 |    4 | `0x0041a470` | `SetOrderAxis`        | sub,sub              | depth-sort axis configuration                                  |
|  50 |    5 | `0x00420610` | `InsertBulanci`       | —                    | spawn the local player avatar                                  |
|  51 |    6 | `0x0041a4a0` | `InsertView`          | sub                  | register a view object in the CGaming world                    |
|  52 |    7 | `0x00416ae0` | `SetInsertMode`       | sub                  | sets the insertion bucket for subsequent `InsertView` calls    |
|  53 |    8 | `0x0041d8f0` | `LoadPreface`         | i32                  | preload an image bundle                                        |
|  54 |    9 | `0x0041bb00` | `SetMusic`            | i32,i32              | (trackID, volume)                                              |
|  55 |   10 | `0x00416ab0` | `SetActive` **e**     | sub,sub              | `view.@0x69 = (level > 0)` — collision/active flag             |
|  56 |   11 | `0x00418770` | `BindToSlot` **e**    | sub,sub              | `CGaming.views[slot] = view` and `view.@0x70 = slot` (0xFF unbinds) |
|  57 |   12 | `0x004188b0` | `ResortDepth` **e**   | sub                  | re-sort `view` in `CGaming::FUN_004184a0`'s ordered list       |
|  58 |   13 | `0x004188e0` | `TranslateTo` **e**   | sub,sub,sub          | `CBulanek::FUN_0042cc80(target, x, y)` — coordinate translate  |
|  59 |   14 | `0x00416b10` | `HideView` **e**      | sub                  | `view.flags |= 1` + freeze + `FUN_0042c290(view, 1)`           |
|  60 |   15 | `0x00416b30` | `ShowView` **e**      | sub                  | `view.flags &= ~1` + wake + `FUN_0042c290(view, 0)` (inverse of HideView) |
|  61 |   16 | `0x00418920` | `EvalSeq3` **e**      | sub,sub,sub          | evaluate three sub-exprs, return first (rare combinator)       |
|  62 |   17 | `0x00416b50` | `GetSlot` **e**       | sub                  | `CGaming::GetSlot(idx)` — returns the view bound to a slot     |
|  63 |   18 | `0x00416b70` | `SetAnim` **e**       | sub,sub,sub          | `view+0x98 → FUN_004391e0(animID, flags, 0)`                   |
|  64 |   19 | `0x00416bb0` | `AnimResume` **e**    | sub                  | `view+0x98 → FUN_00438fb0` — clear paused flag and restart    |
|  65 |   20 | `0x00418950` | `SpawnAtView` **e**   | sub,sub              | spawns a projectile/decal at the view's position; sub2 = kind  |
|  66 |   21 | `0x00416bd0` | `SetAnimFrame` **e**  | sub,sub              | `view+0x98 → FUN_00439710` writes anim frame index             |
|  67 |   22 | `0x0041f610` | `SpawnEnemyAt` **e**  | sub*5                | 5-arg spawn (slot+team+flags+pos+state); cousin of `InsertOpponent` |
|  68 |   23 | `0x00416c00` | `RegisterTimer` **e** | sub,sub,sub          | allocate a 0x1c-byte entry in `this+0x440` timer table         |
|  69 |   24 | `0x00416c40` | `TimerStop` **e**     | sub                  | decrement timer's refcount; reschedule when zero               |
|  70 |   25 | `0x00416c70` | `TimerStart` **e**    | sub                  | set running bit (1) and increment refcount                     |
|  71 |   26 | `0x00416c90` | `TimerRelease` **e**  | sub                  | free the timer slot                                            |
|  72 |   27 | `0x00416cb0` | `TimerSetData` **e**  | sub,sub              | store payload at `slot.@4`                                     |
|  73 |   28 | `0x0041bb30` | `DefineDangerZone` **e** | sub*5             | (kind, x1, y1, x2, y2) — adds a CMine-shape zone to `this+0x2d8` |
|  74 |   29 | `0x00416d30` | `IsServer`            | —                    |                                                                |
|  75 |   30 | `0x00416d50` | `StrmSend`            | sub                  |                                                                |
|  76 |   31 | `0x00416d70` | `SetCommStrm`         | sub                  |                                                                |
|  77 |   32 | `0x00416d90` | `IsNet`               | —                    |                                                                |
|  78 |   33 | `0x00418980` | `SetAnimDirection` **e** | sub,sub           | `view.@0x94 (anim) .@0x18 = (byte)direction`                   |
|  79 |   34 | `0x0041d990` | `SetViewImage` **e**  | sub,i32              | look up image by i32 in CMenu and bind to the view             |
|  80 |   35 | `0x00416dc0` | `PlayAnim` **e**      | sub,sub              | `view+0x98 → FUN_004390d0(loop?)` — start + optional loop      |
|  81 |   36 | `0x004189b0` | `MapSet` **e**        | sub,sub              | `this+0x440` map: `map[key].@0x18 = value`                     |
|  82 |   37 | `0x004189e0` | `MapGet` **e**        | sub                  | `this+0x440` map: returns `map[key].@0x18`                     |
|  83 |   38 | `0x0041bb80` | `DefineTraceArea`     | sub*6                |                                                                |
|  84 |   39 | `0x0041e3e0` | `KillObject` **e**    | sub                  | type-check sub against ClassID 0x7ec then `CBulanek::FUN_0041db00(-1,-1,0)` |
|  85 |   40 | `0x0041e430` | `TeleportPlayerTo`    | sub*4                |                                                                |
|  86 |   41 | `0x0041a4d0` | `RemoveView` **e**    | sub,sub              | unbind from CGaming via `FUN_00419ca0`; if sub2 != 0 also release the view |
|  87 |   42 | `0x00418a00` | `IsViewKind` **e**    | sub                  | `IsKindOf(sub, ClassID 0x7ef = 2031)` — true for `CLevelScript` view |
|  88 |   43 | `0x00416df0` | `SeekAnim` **e**      | sub,sub              | type-check sub then `view+0xa8 → FUN_00439a30(frameIdx)` — advance to frame |
|  89 |   44 | `0x00418a50` | `NewCollection` **e** | —                    | allocate a fresh `CDSCollection` (vtable `0x47f700`, init capacity 32) |
|  90 |   45 | `0x00418a90` | `GetField0C` **e**    | sub                  | returns `sub.@0xC` (anim flags on a view, or item 1 on a tuple) |
|  91 |   46 | `0x00416e40` | `CollResize` **e**    | sub,sub,sub          | `CDSCollection::Resize(coll, newSize, freeItems != 0)`         |
|  92 |   47 | `0x00418ab0` | `ArrayGet` **e**      | sub,sub              | `((u32*)coll.@0x8)[idx]`                                       |
|  93 |   48 | `0x00418ae0` | `ArraySet` **e**      | sub,sub,sub          | `((u32*)coll.@0x8)[idx] = value`                               |
|  94 |   49 | `0x00416e80` | `CollRemove` **e**    | sub*4                | `CDSCollection::Remove(coll, idx, count, freeItems != 0)`      |
|  95 |   50 | `0x00418b20` | `CollInsert` **e**    | sub,sub,sub          | `CDSCollection::Insert(coll, value, index_or_default)`         |
|  96 |   51 | `0x00416ee0` | `FreeObject` **e**    | sub                  | virtual `Release` via `sub.vt[2]` — refcounted destroy        |
|  97 |   52 | `0x00418b60` | `GetImage` **e**      | i32                  | `CMenu::FUN_00413b20(id)` — look up a CMenu image by ID        |
|  98 |   53 | `0x0041f6a0` | `SpawnOpponentEx` **e** | sub*6              | 6-arg enemy spawn (x, y, kind, team, flags, parentSlot); calls `0x0041f230` |
|  99 |   54 | `0x00416f00` | `EnableFireThrough`   | sub,sub              |                                                                |
| 100 |   55 | `0x00420630` | `InsertVampires`      | —                    |                                                                |
| 101 |   56 | `0x0041f730` | `InsertOpponent`      | sub*4                |                                                                |
| 102 |   57 | `0x0041da50` | `CreateMine`          | sub,sub              | single mine at (x, y); allocates 0x118 bytes                   |

## Notes

- The `Editor.Scripts.Opcode.cs` enum names roughly half of the 103-entry
  table. Everything else is a game-only opcode that the editor never
  emits — they appear in the master pack's compiled scripts because the
  original developers used a richer dialect than what they later exposed
  in the editor UI. The argument-shape column above is recovered by
  counting calls to the four bytecode readers (`FUN_004384c0` for `sub`,
  `FUN_00438350` for `i32`, `FUN_00438360` for `u16`, `FUN_00438380`
  for `u8`) inside each handler, and is enough to parse every script in
  the master pack without `<UNKNOWN op=N>` markers.
- Several CLevelScript handlers (notably `0x0041d6e0` for opcode 45 and
  `0x0041d820` for opcode 46) allocate a fresh object via the global
  `FUN_00447c42` allocator and forward its constructor to a
  `CGameView::FUN_004191a0` / `CAnim::FUN_00419940`-shaped helper. The
  return value of those helpers is what `InsertView` (51) consumes from
  its sub-expression.
- Opcodes 68..72 all access the same `this+0x440` container, which the
  engine treats as a small refcounted-slot timer table; each slot is a
  0x1c-byte record managed by `FUN_0042ex…` helpers. 81..82 are a
  get/set pair on that container's `[key].@0x18` slot, so the same
  structure also doubles as a key→object map. 92..95 are the four
  collection mutators (`Resize`, `Get`, `Set`, `Remove`, `Insert`) on a
  `CDSCollection` (vtable `0x47f700`, the type that opcode 89 returns).
- Multiple handlers (55, 57..60, 63..66, 78, 80, 84, 86..88) operate on
  a "view" object, accessing fields at `view+0x14` (flags),
  `view+0x69` (active), `view+0x70` (slot), `view+0x94`/`+0x98`/`+0xa8`
  (anim component) and `view+0x440` (timer/map table). The view itself
  is typically created by `CreateAnim`/`CreateImage`/`InsertOpponent`
  and retrieved later by `GetSlot(slotID)`.
- The base table also contains the "rare" expression helpers (`Clamp`,
  `Select`, `Negate`, the eight comparison forms, the bitwise group, and
  the `Strm*` family) that the editor's UI never offers. Most of them
  are used heavily by the native master pack.

## What's still unknown

- A handful of names tagged **e** in the extension table are best
  guesses for the runtime behaviour they implement and may not match the
  original developer's identifier exactly. Concretely:
  - `EvalSeq3` (61) discards two of its three sub-expressions; its only
    observable effect is evaluating them for side-effects.
  - `SetAnimDirection` (78) and `GetField0C` (90) touch single bytes /
    `@0x0C` slots whose semantic role is inferred from the surrounding
    animation/view structure.
  - `SpawnEnemyAt` (67) and `SpawnOpponentEx` (98) share a deep helper
    with `InsertOpponent` (101); the precise difference in semantics is
    a handful of additional fields that are passed through unchanged.
- The CBulPicture animation component used by 63..66, 78, 80 and the
  CDSAnim sub-object at `+0x98` have their own vtables; tagging each
  slot is separate work.
