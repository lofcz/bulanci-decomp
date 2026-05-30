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
| `0x31` | 4 | `uint` | `dwProfileUserDword` | `CDSChain_LoadConfigFromRegistry@0x0040a440` `ReadBytes(..., this+0x31, 4)` |
| `0x35` | 4 | `void *` | `pLevelResourceTable` | `CDSChain_ctor@0x0040a680` zero; `CGame_FindResourceByName@0x00413560` walks `CGame+0x66` (= chain+0x35) |
| `0x39` | 4 | `uint` | `dwProfileCount` | `CDSChain_ctor` `dwProfileCount = 0` |
| `0x3D` | 4 | `uint` | `dwLevelResourceCount` | ctor zero; `CGame_FindResourceByName` count @ `CGame+0x6e` (= chain+0x3d) |
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
| `0x78` | 1 | `byte` | `bConfigHydrated` | ctor `= 0`; registry success/catch sets `= 1` (`@0x0040a440` / `FUN_0040a650`) |
| `0x79` | 4 | `int *` | `profileList_data` | **CIntList** head: `m_data`; gzip load `CIntListInsertSortedOrAppend(&+0x79, …)` @ `0x0040a53b` |
| `0x7D` | 4 | `int` | `profileList_capacity` | CIntList `m_capacity`; ctor zero `@0x0040a6f3` |
| `0x81` | 4 | `int` | `profileList_count` | CIntList `m_count`; `CBulanci_ClearProfileKeyList@0x004090c0` frees `m_data[i]` then zeros |
| `0x85` | 4 | `uint` | `dwField_85` | ctor `= 8` `@0x0040a6fc` (same constant as `dwProfileCapacity`); **no other xrefs** |
| `0x89` | 27 | `byte[27]` | `pad_89` | No ctor writes; pads object to `0xa4` |

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

**Round 3 task 27** (2026-05-30):

- **`pAuxHeap` @ `CDSChain+0x0c` / `CDSChain_full+0x70`:** **teardown-only** — `CDSChain_ReleaseAuxHeap@0x0042f800` → `Runtime_Free` when non-NULL; called from `CDSChained_AppendChild`, `CDSChain_RemoveListNode`, `FUN_0042f980`/`0x9d0`/`0xa20`/`0xa50`. Ctor zeros `@0x0040a6ea`. **No program-wide store** to list-head `+0x0c`/`+0x70` besides zero-init (same verdict as `CDSSafeStream+0x24`, agent todo 40).
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

### Leaf helpers (intrusive list)

| Symbol | Address | Role |
|--------|---------|------|
| `CDSChain_Append` | `0x0042fb70` | `ClearChildren` then deserialize children via `CDSChained_AppendChild` |
| `CDSChain_Remove` | `0x0042fc30` | `for` over `CDSChained_GetFirstChildView` / `GetNextSiblingView` on `&this[-1].dwChildCount` |
| `CDSChained_ClearChildren` | `0x0042fab0` | Drain `pFirstChild` / `dwChildCount` |
| `CDSChain_RemoveListNode` | `0x0042f940` | Pop head, dec count, optional release |
| `CDSChained_GetFirstChildView` | `0x0042f7c0` | First view under `pChain->pFirstChild` intrusive node |
| `CDSChained_GetNextSiblingView` | `0x0042f920` | Sibling link at `pCurrentView+0xc` (ECX=`pChain`) |

## Follow-up

- Recover `+0x00..+0x03` (never written in ctor; may be allocator/vtable prefix for another face).
- Name `dwProfileField_31` / `dwProfileField_3d` and `dwField_70` / tail `dwField_79`..`dwField_85` from consumers outside ctor/registry.
- Type `keyBindings` as six 6-byte records (ctor/dtor pair `FUN_004097d0` / `CGame_PlayerRec_dtor`) once `CGame_PlayerRec` is verified.
- ~~Re-type `CGame` to embed `CDSChain_full chain` @ `+0x31`~~ — **done** (agent todo 21).

## UNK

- Semantic name for `dwProfileUserDword` @ `+0x31` (registry 4-byte read between key bindings and option block).
- `pLevelResourceTable` @ `+0x35` vs `profileList_*` @ `+0x79` — distinct; gzip profile records only touch the CIntList.
- Whether `CDSChain_dtor@0x0042fcd0` runs on full `CDSChain_full` or only the list-head slice (used from `CBulanci_DestroyConfigStore` on `param_1+0x64`).
- `pAuxHeap` allocator — **no store xref** in `bulanci.exe`; field may be legacy/unused in this build.
- Semantic name for `dwField_85` @ `+0x85` (ctor `= 8`, no consumers).
- Nest `CIntList` type in Ghidra vs three scalar fields at `+0x79..+0x81`.
- Decompiler still prefixes `CBulanci::` on `CDSChain_RemoveListNode` call sites (`__thiscall` ECX retype limit).
