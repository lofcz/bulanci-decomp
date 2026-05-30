# CHistoryScript



## Status



**VERIFIED** — size `0x444`; embedded `CDSScript script` @ 0 (follow-up 09) plus MI tail `0x430`–`0x443`. Round-3 task 46: `+0x438` is a **5-slot `IDSChained` MI facet** (`vtable@0x00483088`); `+0x43c` is optional **`pSubObjStash`** cleared by `FUN_00434250` (same pattern as `CHelpScript` / `CDSMpxStream`).



## Size proof



| Claim | Address | Evidence |

|-------|---------|----------|

| `sizeof(CHistoryScript) == 0x444` | `0x004229b0` | `CreateObject` → `OperatorNewWithBadAlloc(0x444)` then `CHistoryScript::ctor` |

| Derived tail ends before 0x444 | `0x004226c0` | `CHistoryScript::ctor` writes last field at `this+0x440`; allocation is 0x444 bytes |

| MI subobject at +0x430 | `0x00422780` | `ScalarDeletingDtor_thunk` → `CHistoryScript_ScalarDeletingDtor(this + -0x430, …)` |

| Base is `CDSScript` | `0x004226c0` | `CHistoryScript::ctor` calls `CDSScript::ctor`; Ghidra struct embeds `CDSScript` @ 0 |



## Layout



| Offset | Size | Type | Name | Evidence (func@addr) |

|-------|------|------|------|----------------------|

| 0x000 | 0x430 | `CDSScript` | `script` | `CHistoryScript::ctor@0x004226c0` → `CDSScript::ctor`; `InstallOpcodeTable(...,0x2d,...)` |

| 0x430 | 4 | `void *` | `pVftable_IDSEventHandler` | ctor `MOV [ESI+0x430],0x4830a0@0x00422720`; dtor thunk `-0x430@0x00422780` |

| 0x434 | 4 | `int` | `nNestedRefCount` | ctor `= 1@0x004226c0`; shared with `CHelpScript` / `CLevelScript` tail |

| 0x438 | 4 | `void *` | `vf_IDSChained_tail` | ctor `MOV [ESI+0x438],0x483088@0x0042272a`; Ghidra label `g_pCHistoryScript_vftable_IDSChained_tail`; slot 3 `CHistoryScript_ScalarDeletingDtor_thunk_Sub438@0x00422790` (`SUB ECX,0x438` disasm, R5 w05) |

| 0x43C | 4 | `IDSChainedTailStash *` | `pSubObjStash` | ctor `= NULL@0x004226fe` (sole `[ESI+0x43c]` store); cleared by `IDSChainedTail_ClearSubObjStash(&vf_IDSChained_tail)` — never non-null on history path |

| 0x440 | 4 | `void *` | `pBoundView` | ctor zero-init; `CDSScript_SetBoundParentView@0x00422620`; `CHistoryDlg_LoadHistoryPage@0x00422f70` |



### MI tail (round-3 task 46)



`CHistoryScript` shares the `CDSScript` + `0x430..0x443` tail layout with `CHelpScript` / `CLevelScript`. After base `CDSScript::ctor`, history ctor patches script primary faces and installs the subclass opcode splice:



| Absolute | Inst@ctor | Value | Role |

|----------|-----------|-------|------|

| `+0x00` | `0x00422713` | `0x4830d0` | `IDSReferenced` on script (`CHistoryScript_GetClassTable` path) |

| `+0x04` | `0x00422719` | `0x4830b4` | `IDSChained` 6-slot on embedded `CDSScript` (`CDSChain` helpers) |

| `+0x430` | `0x00422720` | `0x4830a0` | `IDSEventHandler` 4-slot |

| `+0x438` | `0x0042272a` | `0x483088` | **`IDSChained` 5-slot** tail facet (parallel to `CHelpScript` `0x4829f4` @ `+0x438`) |



`IDSChainedTail_ClearSubObjStash@0x00434250` is **not** a virtual dtor: given `&vf_IDSChained_tail` (`this+0x438`), it loads `pSubObjStash` at `+0x43c`; if non-null, zeroes `stash+0x10` / `stash+0x14` and nulls the stash pointer (18 callers — `CDSObject`, `CDSMpxStream`, help/history script dtors, etc.).



## Ghidra apply



```

get_struct_layout CHistoryScript → size 0x444 (1092)

  script: CDSScript @ 0

  pVftable_IDSEventHandler @ 0x430

  nNestedRefCount @ 0x434

  vf_IDSChained_tail @ 0x438

  pSubObjStash @ 0x43c (void *)

  pBoundView @ 0x440

```



Round-3 task 46: renamed `pVftable_sub438` → `vf_IDSChained_tail`, `nField_43c` → `pSubObjStash` (`void *`).

Slice 11: `FUN_00422620` → `CDSScript_SetBoundParentView`; `FUN_00434250` → `IDSChainedTail_ClearSubObjStash`; `CHistoryScript_dtor` prototype fixed.



## Follow-up

- **Batch 09:** embedded `CDSScript` @ 0; ctor prototype @ `0x004226c0`.

- **Round 3 / task 46:** `+0x438` typed as `IDSChained` MI vptr (`0x483088`); `+0x43c` as `pSubObjStash` per `IDSChainedTail_ClearSubObjStash` protocol.

- **Slice 11 (2026-05-30):** Ghidra `CHistoryScript` @ 0x444 verified; renamed `CDSScript_SetBoundParentView`, `IDSChainedTail_ClearSubObjStash`; `CHistoryScript_dtor` typed `CHistoryScript *`. Class id **2050** (`0x802`) per ctor plate comment.
- **R3 task 12 (2026-05-30):** Proved stash protocol: `pSubObjStash@+0x43c` cleared by `IDSChainedTail_ClearSubObjStash(&vf_IDSChained_tail)` in dtor and `CHistoryDlg_LoadHistoryPage` (`LEA ECX,[script+0x438]` @ `0x0042302c`); class id **`0x802` (2050)** from static `HandleClassRegister` @ `0x0047bfc0` and `CHistoryDlg_ctor` pool filter @ `0x0042329e`.
- **R4 task 11 (2026-05-30):** Shared opcode 51 `HhAddChildToParentView@0x004217e0` → **cdecl** `CHelpScript *script` on stack; `LoadHistoryPage` `pCVar5` typed `CHistoryScript *` → `IDSChainedTail_ClearSubObjStash(&pCVar5->vf_IDSChained_tail)`.
- **R4 task 12 (2026-05-30):** Program-wide byte scan: **only** `CHelpScript_ctor` / `CHistoryScript_ctor` write `[reg+0x43c]` (both zero). Created **`IDSChainedTailStash`** (`pClearField_10`/`pClearField_14` per `IDSChainedTail_ClearSubObjStash`); retyped `pSubObjStash` → `IDSChainedTailStash *`. Extra consumer: `CGaming_ctor@0x004202cf` clears stash on menu-loaded level script before `CallExport`.



## UNK



- Whether `+0x434` matches nested-refcount semantics on level/help script paths (no direct `CHistoryScript` consumer beyond ctor `=1`).

- ~~Non-null writers on history/help path~~ — **closed (R4-12):** no non-null store in binary; stash always `NULL` on CHistory/CHelp scripts.
- Non-null **`pSubObjStash`** on other CDS types (`CDSObject`, streams) and meaning of `IDSChainedTailStash.pClearField_10/14` (R4 todo 38).

- ~~Class ID `0x800` / `0x802` decode~~ — **done (R3 task 12):** `ClassRegEntry` @ `0x004b38e8` (`CHistoryScript_GetClassTable`); static init `@0x0047bfc0` → `HandleClassRegister(..., classId=0x802, factory=CHistoryScript_CreateObject@0x4229b0)`; `CHistoryDlg_ctor` filters app script-pool nodes with `*(node+0xc)==0x802`, page id `*(node+8)`.


