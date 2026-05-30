# CDSChain

## Status

**PARTIAL** — two distinct memory shapes:

1. **Embedded list-head** (`CDSChain`, **20** / `0x14` bytes) inside `CDSStreamStorage` / `CDSSafeStream` — **VERIFIED**.
2. **Full config + chain object** (`CDSChain_full`, **164** / `0xa4` bytes) embedded in `CGame` at **`CGame+0x31`** — ctor, registry load, and MI list-head at **`+0x64`** proven (round 3 task 36).

The historical name `CDSChain_ctor` @ `0x0040a680` initializes the **full** object (not the 20-byte slice). Ghidra type: `CDSChain_full`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Embedded span `0x14` before parent `CRITICAL_SECTION` @ `+0x48` | `CDSStreamStorage_ctor@0x00401790` | Chain vtables at parent `+0x34`/`+0x38`; `InitializeCriticalSection(parent+0x48)` → `0x48−0x34 = 0x14` |
| Full object span `0xa4` | `mapping.csv` / `CDSChain_ctor@0x0040a680` | Function size `0x40a724−0x40a680 = 0xa4`; Ghidra `CDSChain_full` size **164** |
| Full object embedded in `CGame` @ `+0x31` | `CGame_ctor@0x00414aeb` | `CALL CDSChain_ctor(this+0x31)`; tail fields continue before `CGame+0xD5` (`0x31+0xa4`) |

## Layout — embedded list-head (`CDSChain`, parent `CDSStreamStorage+0x34`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CDSStreamStorage_ctor@0x00401790` → `g_pCDSChain_vftable_IDSReferenced` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | ctor → `g_pCDSChain_vftable_IDSChained` |
| `0x08` | 4 | `void *` | `pFirstChild` | `CDSChained_ClearChildren@0x0042fab0` drain uses `*(this+0x8)` |
| `0x0C` | 4 | `void *` | `pAuxHeap` | `CDSChain_ReleaseAuxHeap@0x0042f800` frees before list mutation; ctor zero |
| `0x10` | 4 | `uint` | `dwChildCount` | `CDSChained_ClearChildren@0x0042fab0` loop on `*(this+0x10)` |

## Layout — full object (`CDSChain_full`, `CGame+0x31`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x04` | 1 | `byte` | `bConfigSignature` | `CDSChain_LoadConfigFromRegistry@0x0040a440` `ReadBytes(..., &this+0x04, 1)`; ctor does not write |
| `0x08` | 5 | `byte[5]` | `pad_08` | Registry load writes key tuples at `this+0x08 + i*6 + 5` for `i in 0..5` (`@0x0040a440`) |
| `0x0D` | 36 | `byte[36]` | `keyBindings` | `CDSChain_ctor@0x0040a680` `_eh_vector_constructor_iterator_(this+0xd, 6, 6, FUN_004097d0, CGame_PlayerRec_dtor)` |
| `0x00` | 4 | `uint` | `dwLocalPlayerDpid` | Runtime DirectPlay local ID: `CGame_ProcessNetMessage` compares `*(int *)&this->chain` / sets `(chain).field_0x5` from roster; **not** in gzip blob |
| `0x31` | 4 | `uint` | `dwOptionsDword` | Gzip R/W `CDSChain_LoadConfigFromRegistry@0x0040a440` / `SaveConfigToRegistry` (`dwOptionsDword` on `CBulanciConfigStore`); persisted options dword between key bindings and option block |
| `0x35` | 12 | `CDSIntPtrListHead` | `levelResourceList` | **CIntList layout** (`+0` pData, `+4` cap, `+8` count) — scalars: `pLevelResourceTable` / `dwLevelResourceCapacity` / `dwLevelResourceCount`; writer `CBulanci_EnumerateLevelScripts@0x00409f60` @ `0x0040a062`; reader `CGame_FindResourceByName@0x00413560` (`CGame+0x66`/`+0x6e`) |
| `0x35` | 4 | `void *` | `pLevelResourceTable` | `levelResourceList.pData` — `CLevelScriptResource` (20 B) row pointers |
| `0x39` | 4 | `uint` | `dwLevelResourceCapacity` | `levelResourceList.nCapacity` — ctor zero `@0x0040a6cd` (was misnamed `dwProfileCount`) |
| `0x3D` | 4 | `uint` | `dwLevelResourceCount` | `levelResourceList.nCount` — `CGame_FindResourceByName` walk bound |
| `0x41` | 4 | `uint` | `dwProfileCapacity` | ctor `= 8` `@0x0040a6d3`; registry profile loop |
| `0x45` | 4 | `void *` / `CDSString` | `strConfigTail` | ctor zero `@0x0040a6d6`; `FUN_0042e2f0(this+0x45, …)` after registry read (`@0x0040a440`) |
| `0x49` | 4 | `int` | `nPanBindingIndex` | Registry R/W `CDSChain_LoadConfigFromRegistry@0x0040a440`; **runtime read** `CBulanci_RegistryLoadFinally_ApplyPanPreview@0x0040a654` → `CDSAudio_SetPanPreview` (signed stereo pan binding, ×50 dB100 per channel) — see [FUN_0040a650.md](./FUN_0040a650.md) |
| `0x4D` | 4 | `int` | `nReservedAudioDword1` | Registry load/save/defaults only; **no runtime consumer** |
| `0x51` | 4 | `int` | `nReservedAudioDword2` | Registry load/save/defaults only; **no runtime consumer** |
| `0x55` | 1 | `byte` | `bRegistryCommitFlag` | registry load sets `= 1` before IDSChained vfn dispatch (`@0x0040a440`) |
| `0x56` | 14 | `byte[14]` | `optionBlock` | `ReadBytes(..., this+0x56, 0xe)` (`@0x0040a440`) |
| `0x64` | 4 | `void *` | `pVftable_IDSReferenced` | `CDSChain_ctor@0x0040a6d9` → `g_pCDSChain_vftable_IDSReferenced` (`0x0047f6d4`) |
| `0x68` | 4 | `void *` | `pVftable_IDSChained` | `CDSChain_ctor@0x0040a6e0` → `g_pCDSChain_vftable_IDSChained` (`0x0047f6b8`) |
| `0x6C` | 4 | `void *` | `pFirstChild` | ctor zero; `CDSChained_ClearChildren` via `CDSChain_Append` passes `this−4` → head at `+0x6c` |
| `0x70` | 4 | `void *` | `pAuxHeap` | list-head slice: same as embedded `CDSChain+0x0c`; `CDSChain_ReleaseAuxHeap` |
| `0x74` | 4 | `uint` | `dwChildCount` | ctor zero; `CDSChained_ClearChildren@0x0042fab0` uses `*(base+0x10)` with `base=this−4` on MI `this=@+0x68` |
| `0x78` | 1 | `byte` | `bConfigHydrated` | ctor `= 0` `@0x0040a709`; set `= 1` on registry success `@0x0040a5db` or catch `InstallFactoryDefaults` `@0x0040a643` — **write-only** on `CDSChain_full` (byte `MOV` sweep: only those three sites + unrelated structs) |
| `0x79` | 12 | `CDSIntPtrListHead` | `profileList` | **12-byte `CIntList` head** (no `nM_growthChunk`): `pData`/`nCapacity`/`nCount` @ `+0`/`+4`/`+8`; gzip 6-byte heap nodes via `CIntListInsertSortedOrAppend@0x00407e20` in `CDSChain_LoadConfigFromRegistry@0x0040a53b`; save/load `SaveConfigToRegistry@0x00409cd0` / `CBulanci_ClearProfileKeyList@0x004090c0` |
| `0x79` | 4 | `void *` | `pProfileList_data` | `profileList.pData` — pointer table to `OperatorNew(6)` profile records |
| `0x7D` | 4 | `uint` | `dwProfileList_capacity` | `profileList.nCapacity` — `CIntList_EnsureCapacity` via insert helper |
| `0x81` | 4 | `uint` | `dwProfileList_count` | `profileList.nCount` — **low byte** used as gzip profile count (`SaveConfigToRegistry` write, `ClearProfileKeyList` drain); runtime index `CGame_GetControlBindingTable@0x00413160` |
| `0x85` | 4 | `uint` | `dwProfileCapacityMirror` | ctor `= 8` `@0x0040a6fc` (mirrors `dwProfileCapacity` @ `+0x41`); **ctor-only** (sole `MOV [ESI+0x85]` in program — R5 worker 28) |
| `0x89` | 27 | `byte[27]` | `pad_89` | **Structural padding** to `0xa4`; Ghidra `pPad_89[26]` + `bPad_end` @ `+0xa3`; no ctor/RW xrefs on `CDSChain_full` base (R5 worker 28) |

**List-head equivalence:** bytes `+0x64..+0x77` match the embedded `CDSChain` field map (vtables, `pFirstChild`, `dwField_0c`/`dwField_70`, `dwChildCount`). `CDSChain_Append@0x0042fb70` / `CDSChain_Remove@0x0042fc30` use `ECX−4` so `CDSChained_*` helpers see the same `+0x8`/`+0x10` layout anchored at `+0x64`.

## Ghidra apply

```
get_struct_layout CDSChain      → 20 bytes (embedded list-head)
get_struct_layout CDSChain_full → 164 bytes (0xa4 full object)
```

Round 3 task 36: created `CDSChain_full`; renamed `LoadConfigFromRegistry` → `CDSChain_LoadConfigFromRegistry` with `CDSChain_full *` parameter (sole caller `CDSChain_ctor@0x0040a70c`).

**SEH slices** (same parent, not `CBulanci`):

| Symbol | Address | Size | Role |
|--------|---------|------|------|
| `CDSChain_LoadConfigFromRegistry` | `0x0040a440` | `0x1ef` | Main body (`__fastcall`, `CDSChain_full *`) — full pipeline: [`CDSChain_LoadConfigFromRegistry.md`](CDSChain_LoadConfigFromRegistry.md) |
| `Catch@0040a62f` | `0x0040a62f` | `0x1e` | MSVC catch handler |
| `FUN_0040a650` / `…_finally` | `0x0040a650` | `0x21` | Success-path finally: `CDSAudio_SetPanPreview(dwPalette0)` + `ExceptionList` restore; see [`FUN_0040a650.md`](FUN_0040a650.md) |

Slice **26** (2026-05-30):

- `CDSChain_ctor` prototype → `CDSChain_full * __fastcall CDSChain_ctor(CDSChain_full *this)` — decompiler now names `pKeyBindings`, list head @ `+0x64`, `pRegistryProfileList` @ `+0x79`.
- `modify_struct_field` `dwField_79` → `pRegistryProfileList` on `CDSChain_full`.
- Renamed `FUN_0042f940` → `CDSChain_RemoveListNode` (intrusive list unlink for `ClearChildren` / `Append`).
- **R5 worker 04:** `FUN_0042fae0` → `CDSChain_SortChildrenWithComparator` (sole caller `CLevelScore_AddPlayerScore` + `CScoreItem_CompareByNetScore`); `FUN_0042f8b0` → `CDSChain_GetChildAtIndex` (score prune loop). Report: [round5_worker_04_report.md](./round5_worker_04_report.md).

**Round 3 task 27** (2026-05-30):

- **`pAuxHeap` @ `CDSChain+0x0c` / `CDSChain_full+0x70`:** **teardown-only** — `CDSChain_ReleaseAuxHeap@0x0042f800` → `Runtime_Free` when non-NULL; called from `CDSChained_AppendChild`, `CDSChain_RemoveListNode`, `CDSChained_PrependChild`, `CDSChained_InsertChildAtAnchor`, `CDSChained_InsertBeforeWithHeadFixup`, `CDSChained_RemoveWithHeadFixup`. Ctor zeros `@0x0040a6ea`. **No program-wide store** to list-head `+0x0c`/`+0x70` besides zero-init (R3/R4 todos 27/40 — allocator absent in `bulanci.exe`).
- **Sentinel vs `pAuxHeap`:** `CDSChained_InsertListNode@0x0042f820` self-links **`pNode+0x8` / `pNode+0xc`** when `pNode==NULL` (circular intrusive node). `CDSChain_RemoveListNode@0x0042f940` tests **`CMP [ECX+0xc], ECX`** @ `0x0042f951` (singleton head) — **not** `pAuxHeap`. Disasm comment @ `0x0042f954`.
- **Profile/registry tail (`+0x79..+0x85`):** embedded **`CIntList`** (`profileList_data` / `profileList_capacity` / `profileList_count`); gzip + factory defaults append 6-byte nodes via `CIntListInsertSortedOrAppend@0x00407e20`. `CBulanci_ClearProfileKeyList@0x004090c0` / `SaveConfigToRegistry@0x00409cd0` use the same byte layout on **`CBulanciConfigStore`** (heap; `DestroyConfigStore` also calls `CDSChain_dtor` on embedded list @ `+0x64`). **`CGame.chain`** is the in-session embed (`CGame_ctor` → `CDSChain_ctor(this+0x31)` only).
- **`bConfigHydrated` @ `+0x78`:** set after successful registry read or catch defaults (`InstallFactoryDefaults`).
- **`dwField_85` @ `+0x85`:** ctor `= 8` only; not CIntList capacity (capacity dword @ `+0x7d` stays 0 until `CIntList_EnsureCapacity`).
- Ghidra: renamed tail fields; comments @ `0x0042f820`, `0x0040a53b`; `save_program bulanci.exe`.

**Agent todo 21** (2026-05-30):

- Rebuilt **`CGame`** (584 B): embedded **`CDSChain_full chain`** @ **`+0x31`** (164 B); lobby slot bytes @ `+0xD8..+0xDB` (`bTotalSlots` … `bLocalSenderSlot`) follow the chain object.
- `CGame_ctor@0x00414a80` prototype `CGame * __fastcall CGame_ctor(CGame *this)`; decompile shows `CDSChain_ctor(&this->chain)`.
- `modify_struct_field` `offset:121` → `pRegistryProfileList` on `CDSChain_full`.
- `save_program bulanci.exe`.

**Pass R4 — CDSChain_full UNK sweep** (2026-05-30): see [pass_r4_CDSChain_full_report.md](./pass_r4_CDSChain_full_report.md).

- `dwLocalPlayerDpid` @ `+0x00`; `dwOptionsDword` @ `+0x31` (gzip; not runtime DP ID).
- `dwLevelResourceCapacity` @ `+0x39` (CIntList cap for level table); `levelResourceList` vs `profileList` @ `+0x79` documented.
- `CDSIntPtrListHead` (12 B) created for future nested fields; scalars retained (Ghidra pad conflict).
- `bConfigHydrated` write-only; `dwProfileCapacityMirror` ctor-only; `pAuxHeap` teardown-only (R4 todo 40); sentinel @ `RemoveListNode` `CMP [node+0xc],node`.
- `CDSChain_dtor` list-head-only @ `+0x64`; full teardown `CBulanci_DestroyConfigStore` on `CGame.chain`.
- `set_function_this_type` on `CDSChain_*` / `CDSChained_InsertListNode`; `save_program bulanci.exe`.

**Round 5 worker 28** (2026-05-30) — profile/registry tail `+0x78..+0xa3`: see [round5_worker_28_report.md](./round5_worker_28_report.md).

- **`profileList` @ `+0x79`:** 12-byte `CIntList` head; 6-byte heap nodes; runtime `CGame_GetControlBindingTable`.
- **`bConfigHydrated` @ `+0x78`:** write-only (ctor zero, load success, catch defaults).
- **`dwProfileCapacityMirror` @ `+0x85`:** ctor-only `MOV [ESI+0x85],8` @ `0x0040a6fc`.
- **`pad_89`:** structural padding; Ghidra field renamed from `pPad_a4`.
- Ghidra comments @ `0x0040a6fc`, `0x0040a5db`, `0x0040a643`, `0x004090c0`, `0x00407e20`; `save_program bulanci.exe`.

### Leaf helpers (intrusive list)

| Symbol | Address | Role |
|--------|---------|------|
| `CDSChained_LinkIntrusiveNode` | `0x0042f780` | Splice node at `+0x8`/`+0xc` prev/next pointers |
| `CDSChained_UnlinkIntrusiveNode` | `0x0042f7a0` | Unlink doubly-linked node; xref `CDSChain_RemoveListNode` |
| `CDSChained_ResetHeadOrSpliceBefore` | `0x0042f850` | Self-link head or splice before anchor |
| `CDSChained_InsertChildAtAnchor` | `0x0042f9d0` | View-tree insert; `ADD [pChain+0x10]` @ `0x0042f9e1`; caller `CDSView::AddChildInternal` |
| `CDSChained_InsertBeforeAnchor` | `0x0042f880` | Unlink + `CDSChained_InsertListNode` |
| `CDSChained_UnlinkAndSpliceNode` | `0x0042f890` | Unlink + `CDSChained_ResetHeadOrSpliceBefore` |
| `CDSChained_InsertBeforeWithHeadFixup` | `0x0042fa20` | `ReleaseAuxHeap` + insert; fixes head @ `+0x08` |
| `CDSChained_RemoveWithHeadFixup` | `0x0042fa50` | Remove node; fixes head @ `+0x08` |
| `CDSChain_Append` | `0x0042fb70` | `ClearChildren` then deserialize children via `CDSChained_AppendChild` |
| `CDSChain_Remove` | `0x0042fc30` | `for` over `CDSChained_GetFirstChildView` / `GetNextSiblingView` on `&this[-1].dwChildCount` |
| `CDSChained_ClearChildren` | `0x0042fab0` | Drain `pFirstChild` / `dwChildCount` |
| `CDSChain_RemoveListNode` | `0x0042f940` | Pop head, dec count, optional release |
| `CDSChained_GetFirstChildView` | `0x0042f7c0` | First view under `pChain->pFirstChild` intrusive node |
| `CDSChained_GetNextSiblingView` | `0x0042f920` | Sibling link at `pCurrentView+0xc` (ECX=`pChain`) |

## Follow-up

- Type `keyBindings` as six 6-byte records (ctor/dtor pair `FUN_004097d0` / `CGame_PlayerRec_dtor`) once `CGame_PlayerRec` is verified.
- Nest `CDSIntPtrListHead` at `+0x35` / `+0x79` (requires replacing adjacent scalars in Ghidra).
- Replace stale `CBulanciConfigStore` (133 B) with `CDSChain_full` typedef or aligned layout.
- Fix `CDSChain_RemoveListNode` decompiler singleton test (`pAuxHeap==pChain` display bug; disasm `CMP [ECX+0xc],ECX` @ `0x0042f951` is ground truth).

## UNK

- ~~`+0x00..+0x03`~~ → **`dwLocalPlayerDpid`** (runtime DP ID on `CGame.chain`).
- ~~`dwProfileUserDword` / level table naming~~ → **`dwOptionsDword`**, **`levelResourceList`** scalars.
- ~~`CDSChain_dtor` scope~~ → list-head @ `+0x64` only ([round4_task_27_report.md](./round4_task_27_report.md)).
- ~~`pAuxHeap`~~ → teardown-only ([round4_task_40_report.md](./round4_task_40_report.md)).
- ~~`dwField_85`~~ → **`dwProfileCapacityMirror`** (ctor-only; R5 worker 28).
- ~~`bConfigHydrated` consumers~~ → write-only hydration flag (R5 worker 28).
- ~~`pad_89`~~ → structural padding to `0xa4` (R5 worker 28).
- ~~`profileList` tail semantics~~ → 12-byte `CIntList` + 6-byte heap nodes (R5 worker 28).
- Decompiler `pAuxHeap==pChain` on singleton branch @ `CDSChain_RemoveListNode` (disasm correct).
- Nest `CDSIntPtrListHead` at `+0x79` in Ghidra (scalar triplets still required for adjacent `bConfigHydrated`).
- Type 6-byte profile record (`CBulanciProfileRec`?) and align `CBulanciConfigStore` (133 B) with `CDSChain_full`.
