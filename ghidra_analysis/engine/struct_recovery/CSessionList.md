# CSessionList

## Status

**VERIFIED** — size `0x7c` (124 bytes); dialog prefix through `+0x6C` field-split (same offsets as [CPauseDlg.md](./CPauseDlg.md)); child pointers `pSessionListBox` / `pCaptionStatic` / `pJoinButton` at `+0x70..+0x78`. `CSessionList_BuildDialog` decompiles with typed `this->` stores (slice 18).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Heap allocation `0x7c` | `0x0040e980` | `_Globals::CreateObject` (CSessionList factory): `OperatorNewWithBadAlloc(0x7c)` then `CSessionList_BuildDialog` |
| Stack instance `0x7c` | `0x00414170` | `CMenu_PickSession`: `CSessionList local_94 [124]` + `CSessionList_BuildDialog(local_94)` |
| Ghidra struct size | — | `get_struct_layout` → **124 bytes** after apply |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_primary` | `CSessionList_BuildDialog@0x0040c2d0` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | same |
| `0x10` | 4 | `void *` | `pVftable_IDSEventHandler` | same |
| `0x18` | 4 | `void *` | `pVftable_field18` | same |
| `0x14` | 2 | `ushort` | `wViewFlags` | `CWindow_BuildAt@0x00405560` `\|= 0x77f` |
| `0x20..0x2C` | 16 | `int`×4 | `nBbox_*` | `CWindow_BuildAt(0,0,0x14f,0xf4,1)`; width forced to `300` in `BuildDialog` |
| `0x46` | 2 | `ushort` | `wContainerFlags` | `BuildDialog` `\|= 8` |
| `0x68` | 1 | `byte` | `bModalFlag` | `CWindow_BuildAt` (`'\x01'`) |
| `0x6C` | 4 | `void *` | `pDefaultFocusChild` | `CWindow_BuildAt`; join via `CDSView_SetAsDefaultFocusChild` |
| `0x70` | 4 | `CListBox *` | `pSessionListBox` | `CSessionList_BuildDialog@0x0040c2d0` store; `CSessionList_OnEvent@0x0040adc0`, `CSessionList_GetPick@0x0040c550`, `CSessionList_AppendEnumSession@0x0040f380` |
| `0x74` | 4 | `CStaticText *` | `pCaptionStatic` | `CSessionList_BuildDialog@0x0040c2d0` store; `CSessionList_SetStatusFromStringHandle@0x0040c5d0` read |
| `0x78` | 4 | `CButton *` | `pJoinButton` | `CSessionList_BuildDialog@0x0040c2d0` store; `CSessionList_OnEvent@0x0040adc0` enable/default-focus via `CDSView_SetAsDefaultFocusChild` |

## BuildDialog child wiring (evidence)

| Child | Build | Stored on `this`? |
|-------|-------|-------------------|
| Caption `CStaticText` | `+0x74` | Yes |
| Session `CListBox` | `+0x70` | Yes |
| Join `CButton` | `+0x78` | Yes |
| Cancel `CButton` | built, `CDSView__AddChild` only | **No** dedicated member (not proven) |

Dialog initial bounds from `CWindow_BuildAt(this,0,0,0x14f,0xf4,1)` — **window rect**, not object size.

## Key functions

| Symbol | Address | Prototype (Ghidra) |
|--------|---------|-------------------|
| `CSessionList_BuildDialog` | `0x0040c2d0` | `CSessionList * __fastcall CSessionList_BuildDialog(CSessionList *this)` |
| `CSessionList_OnEvent` | `0x0040adc0` | `void __thiscall CSessionList_OnEvent(CSessionList *this, short, uint, uint)` |
| `CSessionList_GetPick` | `0x0040c550` | `CSessionItem * __fastcall CSessionList_GetPick(CSessionList *this)` |
| `CSessionList_SelectBySessionGuid` | `0x0040c570` | `void __thiscall CSessionList_SelectBySessionGuid(CSessionList *this, uchar *sessionKey16)` — compares 16 B at `CSessionItem+0x14` |
| `CSessionList_SetStatusFromStringHandle` | `0x0040c5d0` | `void __thiscall CSessionList_SetStatusFromStringHandle(CSessionList *this, int stringHandle)` — writes `pCaptionStatic` |
| `CSessionList_AppendEnumSession` | `0x0040f380` | `void __cdecl CSessionList_AppendEnumSession(DPEnumSessionInfo *, CSessionList *)` |
| `CMenu_PickSession` | `0x00414170` | stack `CSessionList` + modal + `GetPick` → four dwords |

## Ghidra apply

```
get_struct_layout CSessionList → Size: 124 bytes
  +0x00  CWindow win (112 B dialog prefix — see CWindow.md)
  +0x70 pSessionListBox, +0x74 pCaptionStatic, +0x78 pJoinButton
```

Slice 18: `set_function_prototype` on BuildDialog / GetPick / AppendEnumSession / SelectBySessionGuid / SetStatusFromStringHandle; `CMenu_PickSession` local `CSessionList`. **Agent todo 11 (2026-05-30):** `delete_data_type` + `create_struct` with embedded `CWindow win`; `CSessionList_BuildDialog` decompiles `this->win.*` / `this->pSessionListBox`. `save_program bulanci.exe`.

## UNK

- `CDSChained` dwords in `+0x08..+0x64` gap (inherited from dialog pattern; no `CSessionList`-specific xrefs in `BuildDialog`).
- Cancel `CButton` — confirmed **no** `this` member (`BuildDialog` only `CDSView__AddChild`).

## Scheduler `this` typing (agent todo 19, R3 2026-05-30)

| Item | Result |
|------|--------|
| `set_function_this_type` | **Done** — `CSessionList_SelectBySessionGuid@0x0040c570`, `CSessionList_SetStatusFromStringHandle@0x0040c5d0`, `CSessionList_GetPick@0x0040c550` moved into class `CSessionList`; decompiler shows `CSessionList *this`, `this->pSessionListBox`, `this->pCaptionStatic` |
| `CSessionList_BuildDialog@0x0040c2d0` | Already `CSessionList *` (`__fastcall`); `this->win` / child pointers typed |
| Scheduler | `CGame__SchedulerDispatch@0x00416030` case 1 — PRE comments @ `0x004162bd` / `0x0041630d` document `(CSessionList *)pActiveCGaming` during lobby EnumSessions |
| Prior blocker | `set_parameter_type` on ECX was unnecessary once `set_function_this_type` applied |

Lobby scheduler case 1 loads `CGame.pActiveCGaming` and passes it to session-list helpers; during lobby that pointer is the stack/heap **`CSessionList*`**, not in-match **`CGaming*`**.

## Cross-links

- [CDSView.md](./CDSView.md) — `win` is the `0x70` `CWindow` prefix; child pointers alias `CDSView` extension @ `+0x70..+0x78`
