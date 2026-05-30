# Pass R4 — CDSChain_full UNK sweep

## Scope

Map remaining unknowns in **`CDSChain_full`** (164 B / `0xa4`): profile/registry tail `+0x78..`, `pAuxHeap`, intrusive-list sentinel, `dwField_85`, level-resource table `+0x35`. Builds on [round3_task_27_report.md](./round3_task_27_report.md) and [round4_task_27_report.md](./round4_task_27_report.md) (dtor / `CGame.chain` parity).

## Status

**DONE** — field semantics renamed in Ghidra; helper `this` types applied; `save_program bulanci.exe`.

## Evidence summary

| Field / topic | func@addr | Verdict |
|---------------|-----------|---------|
| `dwLocalPlayerDpid` @ `+0x00` | `CGame_ProcessNetMessage@0x0041570b` | Compares / assigns `*(int *)&this->chain` (DirectPlay local ID); not written by `CDSChain_ctor` |
| `dwOptionsDword` @ `+0x31` | `CDSChain_LoadConfigFromRegistry@0x0040a581` | Gzip `ReadBytes` 4 bytes after key bindings; `SaveConfigToRegistry` writes `dwOptionsDword` on `CBulanciConfigStore` |
| Level resource table @ `+0x35` | `CBulanci_EnumerateLevelScripts@0x0040a062` | `CIntListInsertSortedOrAppend(&pLevelResourceTable, …)` — **CIntList head** `{pData@+0x35, cap@+0x39, count@+0x3d}`; rows `CLevelScriptResource` (20 B) |
| Profile list @ `+0x79` | `CDSChain_LoadConfigFromRegistry@0x0040a53b` | Separate CIntList head; 6-byte gzip profile nodes; **not** level table |
| `bConfigHydrated` @ `+0x78` | `0x0040a5db`, `0x0040a643`, `0x0040a709` | Set on success/catch; **no read** xrefs in program |
| `dwProfileCapacityMirror` @ `+0x85` | `CDSChain_ctor@0x0040a6fc` | Sole `MOV [ESI+0x85], imm` — value `8` (= `dwProfileCapacity`); **no reads** |
| `pAuxHeap` @ `+0x70` | `CDSChain_ReleaseAuxHeap@0x0042f800` | Teardown-only; always NULL at runtime ([round4_task_40_report.md](./round4_task_40_report.md)) |
| Sentinel | `CDSChained_InsertListNode@0x0042f820` | `pNode==NULL` → self-link `@+0x8/+0xc` |
| Singleton head pop | `CDSChain_RemoveListNode@0x0042f951` | `CMP dword ptr [ECX+0xc], ECX` — **node link**, not `pAuxHeap` |
| `CDSChain_dtor` | `CBulanci_DestroyConfigStore@0x0040a3d9` | `LEA ECX,[ESI+0x64]` → 20-byte list-head dtor only |
| Full embed teardown | `CGame_dtor@0x00414d8a` | `LEA ECX,[ESI+0x31]` → `CBulanci_DestroyConfigStore` on full `CDSChain_full` |

## Ghidra deltas

| Action | Target |
|--------|--------|
| `create_struct` | `CDSIntPtrListHead` (12 B: `pData`, `nCapacity`, `nCount`) |
| `modify_struct_field` | `offset:0` → `dwLocalPlayerDpid`; `dwProfileUserDword` → `dwOptionsDword`; `dwProfileCount` → `dwLevelResourceCapacity` |
| `set_function_this_type` | `CDSChain_RemoveListNode`, `CDSChained_InsertListNode`, `CDSChain_Append`/`Remove`, `CDSChain_ReleaseAuxHeap`, `CDSChained_ClearChildren` |
| `set_function_prototype` | `CDSChain_ReleaseAuxHeap`, `CDSChained_ClearChildren` → `__fastcall` / `__thiscall` + `CDSChain *` |
| Comments | `0x0040a5db`, `0x0040a6fc`, `0x0040a062`, `0x0042f951` |
| `save_program` | `bulanci.exe` |

Nested `CDSIntPtrListHead` at `+0x35` / `+0x79` **not applied** — Ghidra reported insufficient contiguous undefined bytes (adjacent named scalars). Layout documented as scalar triplets matching `CIntList` offsets 0/4/8.

## Struct doc updates

- [CDSChain.md](./CDSChain.md) — pass R4 section; layout table; UNK list narrowed

## Remaining follow-up

- Nest `CDSIntPtrListHead` after struct field consolidation in Ghidra.
- `CBulanciConfigStore` (133 B) vs `CDSChain_full` (164 B) typedef alignment for save/load helpers.
- `CDSChain_RemoveListNode` decompiler still shows `pAuxHeap == pChain` on singleton branch; trust disasm @ `0x0042f951`.
