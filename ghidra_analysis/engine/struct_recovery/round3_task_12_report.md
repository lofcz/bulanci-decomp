# Round 3 — Task 12 report

## Task

| Field | Value |
|-------|--------|
| **id** | 12 |
| **title** | Prove CHistoryScript +0x438 stash and decode classId 0x802 |
| **types** | `CHistoryScript`, `CHistoryView`, `CDSChained`, `CDSScript` |
| **addresses** | `0x004226c0`, `0x00422760`, `0x00422620`, `0x00434250` |
| **acceptance** | Update `CHistoryScript.md`; Ghidra mutation if evidence supports |

## Status

**DONE**

## Evidence

### `+0x438` / `pSubObjStash` protocol (extends task 46)

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Tail is **8-byte MI facet**: vptr `@+0x438`, stash `@+0x43c` | `CHistoryScript::ctor@0x004226c0` | `MOV [ESI+0x438],0x483088` @ `0x0042272a`; `MOV [ESI+0x43c],EAX` with `EAX=0` @ `0x004226fe`; decompile sets `vf_IDSChained_tail`, `pSubObjStash = NULL` |
| Clear helper takes **`&vf_IDSChained_tail`**, reads stash at **facet+4** | `IDSChainedTail_ClearSubObjStash@0x00434250` | `iVar1 = *(int *)(param_1 + 4)`; if non-null clears `stash+0x10`, `stash+0x14`, nulls facet+4 |
| Dtor teardown | `CHistoryScript_dtor@0x004227b0` | `IDSChainedTail_ClearSubObjStash(&this->vf_IDSChained_tail)` |
| **Page switch** clears stash before rebind | `CHistoryDlg_LoadHistoryPage@0x00422f70` | Asm `LEA ECX,[EAX+0x438]` @ `0x0042302c` then `CALL IDSChainedTail_ClearSubObjStash`; then `CDSScript_SetBoundParentView` → `pBoundView@+0x440` |
| No non-zero **writer** on history path | program search | Sole `MOV [reg+0x43c]` in `CHistoryScript::ctor` is zero-init; no other history-script function stores `pSubObjStash` |
| Parallel help path | `CHelpScript_dtor@0x00421728`, `CHelpDlg_LoadHelpPage@0x00421cd8` | Same `IDSChainedTail_ClearSubObjStash` on `this+0x438` |

**Conclusion:** `+0x438` is not an opaque sub-object blob — it is the **`IDSChained` 5-slot tail vptr** (`0x00483088`); `+0x43c` is the engine-wide **`pSubObjStash`** slot consumed by `IDSChainedTail_ClearSubObjStash`. History uses ctor zero-init, dtor clear, and **LoadHistoryPage** clear when swapping active scripts; no allocator for stash contents was found on this path.

### Class id **`0x802` (decimal 2050)**

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Vtable slot 0 returns registry blob | `CHistoryScript_GetClassTable@0x00422760` | `MOV EAX,0x4b38e8`; plate/decompiler: `&g_CHistoryScript_classMeta` |
| Static registration | static init @ `0x0047bfc0` | `PUSH 0x4229b0` (factory `CHistoryScript_CreateObject`); `PUSH 0x4b82dc` (parent meta); `PUSH 0x802`; `MOV ECX,0x4b38e8`; `CALL HandleClassRegister@0x0042e910` |
| `HandleClassRegister` layout | `HandleClassRegister@0x0042e910` | `*(entry+4)=parent`, `*(entry+8)=classId`, `*(entry+0xc)=factory` |
| Factory allocates `0x444` + ctor | `CHistoryScript_CreateObject@0x004229b0` | `OperatorNewWithBadAlloc(0x444)` → `CHistoryScript::ctor` |
| Runtime menu filter | `CHistoryDlg_ctor@0x004231d0` | `CMP dword ptr [EAX+0xc],0x802` @ `0x0042329e`; on match `CIntListInsertSortedOrAppend(m_pageIds, *(entry+8), …)` — **app script-pool nodes** use `+0xc` for class id (same pattern as `CHelpDlg` / `0x81c`) |
| Matches dispatch table doc | `script_dispatch_table.md` | `CHistoryScript` = **2050** alongside `CLevelScript`=2026, `CHelpScript`=2076 |

**Correction:** Prior notes citing class id **`0x800`** were wrong; binary uses **`0x802`**.

## Ghidra deltas

Already applied (verified this task):

- `CHistoryScript` @ **0x444**: `vf_IDSChained_tail` @ `0x438`, `pSubObjStash` (`void *`) @ `0x43c` (task 46).
- `IDSChainedTail_ClearSubObjStash@0x00434250`; `CHistoryScript_GetClassTable@0x00422760` with class-id comments; `CHistoryScript_CreateObject@0x004229b0`.
- Decompiler comments on ctor, dtor, `CHistoryDlg_ctor`, `CHistoryDlg_LoadHistoryPage`.

No additional `save_program` required for task 12.

## Struct doc updates

- [CHistoryScript.md](./CHistoryScript.md) — R3 task 12 follow-up: stash consumers (dtor + LoadHistoryPage); class id `0x802` / `ClassRegEntry` vs script-pool enumeration; removed stale `0x800` UNK.

## Remaining UNK

- `pSubObjStash`: no **non-null writer** on history/help script paths (only zero-init + clear); pointed-to stash object type still unknown if ever populated.
- `nNestedRefCount@+0x434`: no history-specific consumer beyond ctor `=1`.
- COL / MSVC RTTI graph for `CHistoryScript` (registry uses CDS `ClassRegEntry`, not COL).
