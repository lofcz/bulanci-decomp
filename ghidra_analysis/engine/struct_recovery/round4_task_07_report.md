# Round 4 — task 07 report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **title** | CDirectKeyb R3 follow-up: PollKeyboard ECX + CGame.pDirectKeyb typing |
| **priority** | high |
| **manifest** | `agent_todos_50_r4.json` id 7 (derived from R3 remaining UNK) |
| **supersedes** | R3 todo 7 ([round3_task_07_report.md](./round3_task_07_report.md)) |
| **types** | `CDirectKeyb`, `CGame` |
| **addresses** | `0x00412160`, `0x004121a0`, `0x00416030` |

## Status

**DONE** — PollKeyboard `this` corrected; `CGame.pDirectKeyb` typed `CDirectKeyb *`; scheduler keyboard path decompiles with named member calls.

## Evidence

### R3 carry-over (unchanged)

GetKeyEdge disasm @ `0x004121a0`: `ADD ECX,scanCode`; compare `[ECX+4]` vs `[ECX+0x104]`. Caller `CGame__SchedulerDispatch` loads `[ESI+0x204]` before both Poll and GetKeyEdge (see R3 report).

### R4 blocker: PollKeyboard wrong `this` (pre-fix)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x00412160` | `MOV EAX,ECX` | `this` = `CDirectKeyb*` |
| `0x00412164` | `LEA EDX,[EAX+0x4]` | `m_keyState` |
| `0x00412167` | `LEA EDI,[EAX+0x104]` | `m_prevKeyState` |
| `0x0041216d` | `MOV EAX,[EAX+0x208]` | `m_pKeyboard` |

Pre-R4 decompile: `CGame::CDirectKeyb_PollKeyboard(CGame *this)` — copied wrong object layout (`this+0x4` interpreted as `CDSUpdatedItem`, `GetDeviceState` target from `bPollKeyboard` byte).

### CGame owner field

| Offset | Writer | Consumer |
|--------|--------|----------|
| `CGame+0x204` (`pDirectKeyb`) | `CGame_StartGame` allocates `0x20c` + ctor | `CGame__SchedulerDispatch@0x004160a0` Poll; `@0x004160e3` GetKeyEdge |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `CDirectKeyb *` @ `0x00412160` | `CDirectKeyb::CDirectKeyb_PollKeyboard(CDirectKeyb *this)` |
| `modify_struct_field` | `CGame.pDirectKeyb` | `CDirectKeyb *` @ offset 516 |
| `modify_struct_field` | `CGame.pActiveCGaming` | restored `CGaming *` @ 496 (name lost during pDirectKeyb retype) |
| `force_decompile` | `0x00412160`, `0x004121a0`, `0x00416030` | Poll uses `m_pKeyboard`; scheduler uses `this->pDirectKeyb` |
| `save_program` | `bulanci.exe` | saved |

### Decompile after fix (excerpts)

**PollKeyboard:**

```c
void __thiscall CDirectKeyb::CDirectKeyb_PollKeyboard(CDirectKeyb *this)
{
  // copy m_keyState → m_prevKeyState; GetDeviceState(0x100, m_keyState)
  piVar1 = (int *)this->m_pKeyboard;
  ...
}
```

**Scheduler case 7:**

```c
CDirectKeyb::CDirectKeyb_PollKeyboard(this->pDirectKeyb);
...
cVar4 = CDirectKeyb::CDirectKeyb_GetKeyEdge(this->pDirectKeyb, *(uchar *)(uVar6 + iVar5));
```

## Struct doc updates

- [CDirectKeyb.md](./CDirectKeyb.md) — R4 PollKeyboard typing + scheduler xref
- [CGame.md](./CGame.md) — `pDirectKeyb` type `CDirectKeyb *`; Ghidra apply line

## Remaining UNK

- Decompiler field ids `pM_keyState` / `pM_prevKeyState` persist for `byte[256]` members (Ghidra array-to-pointer decay); logical names remain `m_keyState` / `m_prevKeyState` in struct docs.
- `CDirectKeyb` ctor/dtor `this` typing (`__fastcall` / scalar dtor) — separate pass if needed.
