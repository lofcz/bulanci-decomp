# CDirectKeyb

## Status

**VERIFIED** — `sizeof == 0x20c`; all five fields have ctor write + poll/dtor consumer.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDirectKeyb) == 0x20c` | `0x00412410` | `CreateObject` → `OperatorNewWithBadAlloc(0x20c)` then `CDirectKeyb__CDirectKeyb_ctor` |
| `sizeof(CDirectKeyb) == 0x20c` | `0x00413e96` | `CGame_StartGame` → `OperatorNewWithBadAlloc(0x20c)` then ctor |
| Last field ends at 0x20c | `0x004121d0` | ctor stores `IDirectInputDevice*` at `param_1[0x82]` (= offset `0x208`) |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x000 | 4 | `pointer` | `vftable` | ctor `*param_1 = g_pCDirectKeyb_vftable_IDSEventHandler@0x004121d0`; dtor restores same `@0x00412350` |
| 0x004 | 0x100 | `byte[256]` | `m_keyState` | ctor `_memset(param_1+1,0,0x100)@0x004121d0`; poll `GetDeviceState(0x100, this+4)@0x00412160`; Ghidra `m_keyState` (was `pM_keyState`, slice 06) |
| 0x104 | 0x100 | `byte[256]` | `m_prevKeyState` | poll copies `this+4` → `this+0x104` `@0x00412160`; Ghidra `m_prevKeyState` (slice 06) |
| 0x204 | 4 | `pointer` | `m_pDI` | ctor `DirectInputCreateW(..., param_1+0x81, ...)@0x004121d0`; dtor `Release` on `param_1[0x81]@0x00412350` |
| 0x208 | 4 | `pointer` | `m_pKeyboard` | ctor `CreateDevice` into `param_1[0x82]@0x004121d0`; dtor `Unacquire`+`Release` on `param_1[0x82]@0x00412350` |

## Ghidra apply

```
get_struct_layout CDirectKeyb  → size 524 (0x20c)
```

Applied: vtable + `m_keyState` + `m_prevKeyState` + `m_pDI` + `m_pKeyboard` (`get_struct_layout` size 524). Slice 06 renamed key buffers; `CDirectKeyb_PollKeyboard@0x00412160` decompile uses `CDirectKeyb*`.

## GetKeyEdge / PollKeyboard `this` typing (R3 todo 7, R4 todo 7)

- `CDirectKeyb_GetKeyEdge@0x004121a0`: **disasm-proven** ECX=`CDirectKeyb*` (`ADD ECX,scanCode`; `[ECX+4]` vs `[ECX+0x104]` = `m_keyState` / `m_prevKeyState`). Caller `CGame__SchedulerDispatch@0x004160e3` passes `this->pDirectKeyb`.
- `CDirectKeyb_PollKeyboard@0x00412160`: same ECX base (`LEA +0x4/+0x104`; `GetDeviceState` on `[EAX+0x208]`). Was wrongly `CGame::CDirectKeyb_PollKeyboard(CGame*)` until R4.
- **Ghidra R3:** `set_function_this_type(CDirectKeyb *)` on GetKeyEdge — [round3_task_07_report.md](./round3_task_07_report.md).
- **Ghidra R4:** `set_function_this_type(CDirectKeyb *)` on PollKeyboard; `CGame.pDirectKeyb` → `CDirectKeyb *`; scheduler decompile `PollKeyboard(this->pDirectKeyb)` / `GetKeyEdge(this->pDirectKeyb, scanCode)`. Decompiler still emits `pM_keyState` / `pM_prevKeyState` for `byte[256]` fields (Ghidra array decay). Report: [round4_task_07_report.md](./round4_task_07_report.md).
