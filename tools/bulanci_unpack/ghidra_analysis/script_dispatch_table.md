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

Naming key:
- "editor" = name from `editor_il_spy/Editor.Scripts/Opcode.cs`
- "verified" = argument shape recovered by hand-disassembling the handler
- "?" = handler exists, semantics are not yet documented here

All 58 entries below have been verified by hand-disassembling the
handler. The column "shape" is the argument list the handler reads off
the bytecode stream before producing a result; "notes" hints at the
runtime behaviour we recovered.

| op  | ext# | handler      | shape                | notes                                                          |
|----:|-----:|--------------|----------------------|----------------------------------------------------------------|
|  45 |    0 | `0x0041d6e0` | sub,sub,u8,u8,i32[n] | CreateAnim (frames=[…]) — also writes a CAnim at this+0xF0    |
|  46 |    1 | `0x0041d820` | sub,sub,i32          | CreateImage(x, y, imageID); allocates a CGameView wrapper      |
|  47 |    2 | `0x00418800` | sub,sub,sub,sub      | CreateObstacle (editor name)                                   |
|  48 |    3 | `0x004187a0` | sub*5                | SetObstacleBounds                                              |
|  49 |    4 | `0x0041a470` | sub,sub              | SetOrderAxis                                                   |
|  50 |    5 | `0x00420610` | —                    | InsertBulanci (no args)                                        |
|  51 |    6 | `0x0041a4a0` | sub                  | InsertView (1 sub = view object)                               |
|  52 |    7 | `0x00416ae0` | sub                  | SetInsertMode                                                  |
|  53 |    8 | `0x0041d8f0` | i32                  | LoadPreface(imageID)                                           |
|  54 |    9 | `0x0041bb00` | i32,i32              | SetMusic(trackID, volume)                                      |
|  55 |   10 | `0x00416ab0` | sub,sub              | obj.flag@0x69 = (sub2 > 0) — bool setter on the view object    |
|  56 |   11 | `0x00418770` | sub,sub              | links sub1 (view) and sub2 (int tag) via FUN_00418080          |
|  57 |   12 | `0x004188b0` | sub                  | calls CGameView::FUN_004184a0 on this+0x458 with sub — used as bridge for InsertView-shape calls |
|  58 |   13 | `0x004188e0` | sub,sub,sub          | CBulanci::FUN_0042cc80(sub1, sub2, sub3)                       |
|  59 |   14 | `0x00416b10` | sub                  | _Globals::FUN_0042c990(sub) — destroys a managed object        |
|  60 |   15 | `0x00416b30` | sub                  | simple 1-sub setter                                            |
|  61 |   16 | `0x00418920` | sub,sub,sub          | 3 subs consumed; returns first                                 |
|  62 |   17 | `0x00416b50` | sub                  | simple 1-sub setter (different field than op60)                |
|  63 |   18 | `0x00416b70` | sub,sub,sub          | 3-arg view setter                                              |
|  64 |   19 | `0x00416bb0` | sub                  | sub→ESI; CAnim::FUN_00438fb0 on ESI+0x98 — animation-start    |
|  65 |   20 | `0x00418950` | sub,sub              | calls FUN_00418000(sub1, sub2)                                 |
|  66 |   21 | `0x00416bd0` | sub,sub              | CAnim::FUN_00438fd0 on sub1+0x98 with sub2 — anim-set-frame    |
|  67 |   22 | `0x0041f610` | sub*5                | view-state mutator (sub3, sub4 used as bools)                  |
|  68 |   23 | `0x00416c00` | sub,sub,sub          | 3-sub view setter                                              |
|  69 |   24 | `0x00416c40` | sub                  | this+0x440 container method `0x0042f330(sub, -1)`              |
|  70 |   25 | `0x00416c70` | sub                  | this+0x440 container method `0x0042f300(sub)`                  |
|  71 |   26 | `0x00416c90` | sub                  | this+0x440 container method `0x0042eac0(sub)`                  |
|  72 |   27 | `0x00416cb0` | sub,sub              | this+0x440 container method `0x0042f2d0(sub1, sub2)`           |
|  73 |   28 | `0x0041bb30` | sub*5 (via 0x416ce0) | reads 5 subs into a packed struct, forwards to `0x0041b420`    |
|  74 |   29 | `0x00416d30` | —                    | IsServer                                                       |
|  75 |   30 | `0x00416d50` | sub                  | StrmSend                                                       |
|  76 |   31 | `0x00416d70` | sub                  | SetCommStrm                                                    |
|  77 |   32 | `0x00416d90` | —                    | IsNet                                                          |
|  78 |   33 | `0x00418980` | sub,sub              | view+0x94 (anim slot?) setter                                  |
|  79 |   34 | `0x0041d990` | sub,i32              | spawn-and-bind: sub = parent view, i32 = CMenu image ID        |
|  80 |   35 | `0x00416dc0` | sub,sub              | sets a flag on sub1                                            |
|  81 |   36 | `0x004189b0` | sub,sub              | this+0x440 map: `map[sub1].@0x18 = sub2` (set)                 |
|  82 |   37 | `0x004189e0` | sub                  | this+0x440 map: returns `map[sub].@0x18` (get)                 |
|  83 |   38 | `0x0041bb80` | sub*6                | DefineTraceArea                                                |
|  84 |   39 | `0x0041e3e0` | sub                  | type-tests sub (must be ClassID 0x7ec) and calls `0x0041db00`  |
|  85 |   40 | `0x0041e430` | sub*4                | TeleportPlayerTo                                               |
|  86 |   41 | `0x0041a4d0` | sub,sub              | bind sub1 (view) via FUN_00419ca0 and optionally release       |
|  87 |   42 | `0x00418a00` | sub                  | type query: invokes sub.vt[0] with `0x4b3768` and compares     |
|  88 |   43 | `0x00416df0` | sub,sub              | obj.FUN_00439a30(idx) — anim/sound at sub2 within sub1         |
|  89 |   44 | `0x00418a50` | —                    | allocates a new 0x18-byte "list" object (vtable `0x47f700`)   |
|  90 |   45 | `0x00418a90` | sub                  | sub != 0 ? sub.@0xc : 0 — read a field from sub                |
|  91 |   46 | `0x00416e40` | sub,sub,sub          | obj.FUN_00431000(idx, bool)                                    |
|  92 |   47 | `0x00418ab0` | sub,sub              | array index: returns `((u32*)sub1.@0x8)[sub2]`                 |
|  93 |   48 | `0x00418ae0` | sub,sub,sub          | array set: `((u32*)sub1.@0x8)[sub2] = sub3`                    |
|  94 |   49 | `0x00416e80` | sub*4                | obj.FUN_00431100(sub2, sub3, sub4 != 0)                        |
|  95 |   50 | `0x00418b20` | sub,sub,sub          | obj.FUN_004310b0(sub3, sub2_or_obj_default)                    |
|  96 |   51 | `0x00416ee0` | sub                  | view-state op (already verified earlier)                       |
|  97 |   52 | `0x00418b60` | i32                  | CMenu::FUN_00413b20(i32) image lookup                          |
|  98 |   53 | `0x0041f6a0` | sub*6                | 6-arg helper that ultimately calls `0x00416810` (object lookup) |
|  99 |   54 | `0x00416f00` | sub,sub              | EnableFireThrough                                              |
| 100 |   55 | `0x00420630` | —                    | InsertVampires                                                 |
| 101 |   56 | `0x0041f730` | sub*4                | InsertOpponent                                                 |
| 102 |   57 | `0x0041da50` | sub,sub              | CreateMine                                                     |

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
- Opcodes 69..72 all access the same `this+0x440` container (different
  `FUN_0042ex…` member methods); 81..82 are a get/set pair on that same
  container's `[key].@0x18` slot, suggesting it's a string-keyed object
  map. 92..93 are an array get/set pair through `sub.@0x8`. Multiple
  handlers (55, 59, 86, 88, 90, 91, 95) operate on the "view" object the
  expression returns at `sub.@0xXX`.
- The base table also contains the "rare" expression helpers (`Clamp`,
  `Select`, `Negate`, the eight comparison forms, the bitwise group, and
  the `Strm*` family) that the editor's UI never offers. Most of them
  are used heavily by the native master pack.

## What's still unknown

- Semantic role / human-readable name of the 26 `opNN` handlers (55..73
  except `*4`/`5`/`8`/`9`, plus 78..82, 84, 86..91, 94..98). The shapes
  are pinned down but the runtime behaviour they map to in the editor's
  intent has to be inferred from caller patterns.
- Some handlers (89 in particular — allocates a fresh 0x18-byte list
  object with vftable `0x47f700`) look like constructors for nested
  scripting types; the resulting objects' own dispatch tables are
  separate work.
