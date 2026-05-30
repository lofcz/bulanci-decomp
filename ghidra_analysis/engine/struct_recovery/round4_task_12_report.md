# Round 4 — Task 12 report

## Task

| Field | Value |
|-------|-------|
| **id** | 12 |
| **title** | Trace CHistoryScript pSubObjStash writers and target type |
| **source** | `agent_todos_50_r4.json` / `todos_gather_r4_1.json` (handoff; supersedes R3 todo 12) |
| **types** | `CHistoryScript`, `CDSChained`, `CHistoryDlg`, `CDSScript` |
| **addresses** | `0x00434250`, `0x004229b0`, `0x0042302c`, `0x004226fe`, `0x004202cf` |

## Status

**DONE**

## Evidence

### Writers to `pSubObjStash@+0x43c` (CHistoryScript / CHelpScript tail)

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| **Only two** program stores to `[reg+0x43c]` | byte scan `89 ?? 3c 04 00 00` | Hits **`0x0042161e`** (`CHelpScript_ctor`) and **`0x004226fe`** (`CHistoryScript_ctor`) only |
| Both are **zero-init** | `CHistoryScript_ctor@0x004226c0` | `MOV [ESI+0x43c],EAX` with `EAX=0` @ `0x004226fe`; decompile `this->pSubObjStash = (void *)0x0` |
| No history/help **non-null** writer | `search_instructions` MOV `0x43c` | 5 MOV hits total; only 2 are `[ESI+0x43c]` stores; others are stack/frame false positives (`ESP+0x43c`) |
| Tail vtable slots do not allocate stash | `CHistoryScript` vtable `0x00483088` | Slots 0–4: `GetResourceName`, `CDSChain_AdjustThisOffset_ThisMinus434`, `CLevelScript_ReleaseChild_thunk_Sub438`, `CHistoryScript_ScalarDeletingDtor_thunk_Sub438`, `AlwaysReturnsZero` — adjustors/refcount only |
| **Consumers** (clear, not write) | multiple | `IDSChainedTail_ClearSubObjStash@0x00434250` — 18 xrefs including `CHistoryScript_dtor`, `CHistoryDlg_LoadHistoryPage`, `CHelpScript_dtor`, `CHelpDlg_LoadHelpPage` |
| LoadHistoryPage passes `&vf_IDSChained_tail` | `CHistoryDlg_LoadHistoryPage@0x00422f70` | `IDSChainedTail_ClearSubObjStash((int)&pCVar4[1].dwPc)` ≡ script+`0x438` (CDSScript size `0x430`, `dwPc` @ +8 in synthetic `pCVar4[1]`) |
| Gameplay also clears on **level script** bind | `CGaming_ctor@0x004202cf` | After `CMenuGetResourceById` loads class-2026 script: `IDSChainedTail_ClearSubObjStash(iVar10 + 0x438)` before `CopyNetSessionFields` / `CallExport` |

**Conclusion (writers):** In this binary, **`pSubObjStash` is never assigned a non-null heap pointer** on the CHistory/CHelp script path. Lifecycle is ctor `NULL`, defensive clear on page switch / dtor / gaming script bind, and (for other CDS types) the same helper at different MI offsets.

### Target type when stash is non-null (`IDSChainedTail_ClearSubObjStash`)

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Facet argument is **`&vptr_IDSChained_tail`** | `IDSChainedTail_ClearSubObjStash@0x00434250` | `stash = *(void **)(facet + 4)`; if non-null: `*(stash+0x10)=0`, `*(stash+0x14)=0`, `facet+4=0` |
| Minimal payload layout | same | Proven touched fields **`+0x10`** and **`+0x14`** (8 bytes); size ≥ **`0x18`** |
| Ghidra struct | type manager | **`IDSChainedTailStash`** (24 B): `pad_00[4]`, `pClearField_10@0x10`, `pClearField_14@0x14` |
| `CHistoryScript.pSubObjStash` retyped | `CHistoryScript` | **`IDSChainedTailStash *`** (always `NULL` on help/history scripts in practice) |

Shared engine pattern: same helper and `facet+4` stash slot on `CDSObject` (`+0x58`), `CDSMpxStream` (`+0x3c`/`+0x40`), `CDSWavStream` (`+0x34`), etc. — full writer map deferred to R4 todo **38** (CDSMpxStream).

## Ghidra deltas

- `create_struct` **`IDSChainedTailStash`** (payload cleared at `+0x10`/`+0x14`)
- `modify_struct_field` `CHistoryScript.pSubObjStash` → **`IDSChainedTailStash *`**
- `set_decompiler_comment` @ `0x00434250`, `0x004226fe`, `0x004202cf`
- `save_program bulanci.exe` (×2)

## Struct doc updates

- [CHistoryScript.md](./CHistoryScript.md) — R4 todo 12: writer scan, `IDSChainedTailStash`, `CGaming_ctor` clear xref; UNK narrowed

## Remaining UNK

- **Non-null `pSubObjStash` writer** for any class (not observed on help/history; may exist only on `CDSObject` / stream paths — R4 todo 38).
- Semantic names for `pClearField_10` / `pClearField_14` (released handles vs. refcount cookies).
- Whether `nNestedRefCount@+0x434` ever pairs with a populated stash on level scripts (refcount thunks @ `0x483088` slot 2/3; no stash store found).
