# Round 3 — task 07 report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **title** | Disasm-prove CDirectKeyb_GetKeyEdge ECX is CDirectKeyb* |
| **priority** | high (blocker) |
| **addresses** | `0x004121a0`, `0x00412160`, `0x00416030` |
| **manifest** | `agent_todos_50_r3.json` id 7 |
| **note** | Manifest table text incorrectly says `CDSApp*`; disasm and `CGame+0x204` caller prove **`CDirectKeyb*`**. |

## Status

**DONE** — disasm proof recorded; `set_function_this_type(CDirectKeyb *)` @ `0x004121a0` succeeded (prior MCP blocker cleared); decompiler shows `CDirectKeyb*` and `m_keyState` / `m_prevKeyState` indexing.

## Disasm proof (`CDirectKeyb_GetKeyEdge@0x004121a0`)

| Step | Address | Instruction | Meaning |
|------|---------|-------------|---------|
| 1 | `0x004121a0` | `MOVZX EAX, byte ptr [ESP+0x4]` | `scanCode` stack arg |
| 2 | `0x004121a5` | `ADD ECX, EAX` | `this` base + scan index |
| 3 | `0x004121a7` | `MOV AL, byte ptr [ECX+0x4]` | current key byte |
| 4 | `0x004121aa` | `CMP AL, byte ptr [ECX+0x104]` | vs previous snapshot |
| 5 | `0x004121b0`–`0x004121c5` | edge / press / release | returns 0 / 1 / 2 |

Offsets **`+0x4`** and **`+0x104`** match `CDirectKeyb::m_keyState` and `m_prevKeyState` (see [CDirectKeyb.md](./CDirectKeyb.md)). They are **not** `CGame` or `CDSApp` layout fields.

### Control: `CDirectKeyb_PollKeyboard@0x00412160`

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x00412164` | `LEA EDX, [EAX+0x4]` | `m_keyState` |
| `0x00412167` | `LEA EDI, [EAX+0x104]` | `m_prevKeyState` |
| `0x0041216d` | `MOV EAX, [EAX+0x208]` | `m_pKeyboard` → `GetDeviceState` |

Same `ECX`/`EAX` object base with `+0x4` / `+0x104` / `+0x208` keyboard device field.

## Caller proof (`CGame__SchedulerDispatch`)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x0041609a` | `MOV ECX, [ESI+0x204]` | load `pDirectKeyb` before `CDirectKeyb_PollKeyboard` |
| `0x004160a0` | `CALL 0x00412160` | poll |
| `0x004160e3` | `MOV ECX, [ESI+0x204]` | same pointer before `GetKeyEdge` |
| `0x004160e9` | `CALL 0x004121a0` | `PUSH` scan code on stack |

`ESI` = `CGame*`; **`[CGame+0x204]`** is the owned `CDirectKeyb*`, not `this` for `GetKeyEdge`.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x004121a0` | Moved into class `CDirectKeyb`; `this` = `CDirectKeyb *` |
| `force_decompile` | `0x004121a0` | `char __thiscall CDirectKeyb::CDirectKeyb_GetKeyEdge(CDirectKeyb *this, uchar scanCode)` |
| (existing) plate / PRE comments | `0x004121a0` | Document disasm + caller |
| `save_program` | `bulanci.exe` | saved |

### Decompile after fix (excerpt)

```c
if (this->pM_keyState[scanCode] == this->pM_prevKeyState[scanCode]) {
  return '\0';
}
return ((this->pM_keyState[scanCode] & 0x80) != 0x80) + '\x01';
```

(Ghidra field ids still `pM_keyState` / `pM_prevKeyState` from slice 06; logical names `m_keyState` / `m_prevKeyState` in struct doc.)

## Struct doc updates

- [CDirectKeyb.md](./CDirectKeyb.md) — UNK cleared; `set_function_this_type` applied R3 todo 7.

## Remaining UNK

- Rename decompiler fields `pM_keyState` → `m_keyState` in Ghidra UI (cosmetic; struct layout already correct).
- `CGame+0x204` field label `pDirectKeyb` (out of task scope unless `CGame` struct pass).

## Xrefs

- Sole code caller: `CGame::SchedulerDispatch@0x004160e9` → `CDirectKeyb_GetKeyEdge`.
