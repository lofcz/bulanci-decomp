# Round 4 — Task 03 Report (R4 worker todo id=3)

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **title** | CAdvertising heap children + dtor destroy path (R3 follow-up) |
| **one_liner** | Close R3 UNK: document heap backdrop (`OperatorNew(0x6c)`) and splash `CDSBitmap` (`0x78`) via `CDSView__AddChild`; plate/comment `CWindow_dtor` on embedded `win` @ `+0`. |
| **acceptance** | Ghidra comments on ctor `0x6c` alloc + first `AddChild`; `LoadSplashImage` `0x78` alloc + second `AddChild`; dtor asm `CALL 0x00401910` with `ECX=this` documented; `CAdvertising` layout unchanged at 144 B. |
| **source** | `agent_todos_50_r3.json` id 3 + [round3_task_03_report.md](./round3_task_03_report.md) Remaining UNK (R4 manifest `agent_todos_50_r4.json` not present in repo at worker start) |
| **structs** | CAdvertising, CWindow, CDSBitmap, CDSChained |
| **addresses** | `0x0040e5f0`, `0x00401a10`, `0x00401910`, `0x0040fe30`, `0x0040aca0`, `0x00402490` |

## Status

**DONE**

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| White backdrop heap **108 B** (`0x6c`) | `CAdvertising_ctor@0x0040e5f0` | `PUSH 0x6c` @ `0x0040e64e`; `CDSChained_InitWithRect` full 800×600; primary vtable `0x47fb04`; `this_00[1].pVftable_primary = 0xffffff` |
| First child chained on dialog | `CAdvertising_ctor@0x0040e6e0` | `MOV ECX,ESI`; `CALL 0x0042d0b0` (`CDSView__AddChild`) — backdrop only in ctor |
| Splash **120 B** (`0x78`) heap `CDSBitmap` | `LoadSplashImage@0x0040fe30` | `PUSH 0x78` @ `0x0040fe91`; `CDSBitmap_ctor` with centered origin `(800-w)/2`, `(600-h)/2` |
| Second child = splash bitmap | `LoadSplashImage@0x0040fee0` | `CDSView__AddChild(this, pCVar3, 0)`; then `ArmDismissTimer` |
| Dtor order: `updatedItem` then `win` | `CAdvertising_dtor@0x00401a10` | `LEA ECX,[ESI+0x70]` → `CDSUpdatedItem_dtor`; `MOV ECX,ESI` → `CWindow_dtor@0x00401910` |
| Embedded prefix destroy | `CWindow_dtor@0x00401910` | Plate: ECX = object base; `CAdvertising` passes `this` (`win` @ offset 0) |
| Stack splash unchanged | `CBulanci_OnEvent_MenuStateMachine@0x00402490` | Still `CAdvertising splash [144]` on event `0xf7` |
| Layout **144 B** retained | `get_struct_layout CAdvertising` | `win` 112 + `updatedItem` 24 + tail |

## Ghidra deltas

- `set_decompiler_comment` @ `0x0040e64e`, `0x0040e6e0`, `0x00401a54`, `0x0040fe30`, `0x0040fe91`, `0x0040fee0`
- `set_plate_comment` @ `CWindow_dtor@0x00401910`
- `modify_struct_field` attempted `bM_bBlockDismiss` → `m_bBlockDismiss` (Ghidra layout still lists `bM_bBlockDismiss` — byte auto-prefix)
- `force_decompile` @ `0x0040e5f0`, `0x00401a10`, `0x0040fe30`
- `save_program bulanci.exe`

## Struct doc updates

- [CAdvertising.md](./CAdvertising.md) — R4 child-view table + dtor note

## Remaining UNK

- `win` interior / `CDSChained` chain band: inherited from `CWindow`; no `CAdvertising`-specific field xrefs (deferred to `CWindow`/`CDSView` batches).
- Decompiler still prints `_Globals::CWindow_dtor((int)this)` despite `CWindow_dtor@0x00401910` name + PRE comment (namespace thunk alias).
- Exact type of **108 B** backdrop: typed as `CDSChained*` in decompile; may be a trimmed view shell (108 B < `CWindow` 112 B) — not promoted to a named Ghidra struct without size proof beyond alloc tag.
