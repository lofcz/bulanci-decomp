# Round 3 — Task 27 report

## Task

| Field | Value |
|-------|--------|
| **id** | 27 |
| **title** | CDSChain_full profile/registry tail + pAuxHeap allocator; sentinel vs InitCircularNode |
| **acceptance** | Resolve sentinel semantics; profile/registry tail naming; pAuxHeap allocator or document teardown-only |

**Types:** `CDSChain`, `CDSChain_full`, `CDSChained`, `CGame`  
**Addresses:** `0x0040a680`, `0x0040a440`, `0x00414aeb`, `0x0042f800`, `0x0042f820`, `0x0042fcd0`

## Status

**DONE** — `pAuxHeap` teardown-only (no allocator); intrusive circular sentinel disambiguated from list-head field; registry tail `+0x79..+0x85` typed as embedded `CIntList` + `bConfigHydrated`.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `pAuxHeap` freed before list splice | `CDSChain_ReleaseAuxHeap@0x0042f800` | `if (pChain->pAuxHeap) Runtime_Free(...)`; xrefs from AppendChild / RemoveListNode / `FUN_0042f980`/`9d0`/`a20`/`a50` |
| No non-zero store to head `+0x0c`/`+0x70` | `CDSChain_ctor@0x0040a680` | `MOV [ESI+0x70], 0` @ `0x0040a6ea`; program-wide search: no other `MOV` to chain-head aux offset |
| Circular sentinel on **child** node | `CDSChained_InsertListNode@0x0042f820` | `pNode==NULL` → `*(this+8)=this`, `*(this+0xc)=this` |
| Remove head singleton test | `CDSChain_RemoveListNode@0x0042f940` | `CMP ECX,[ESI+8]` then `CMP [ECX+0xc],ECX` @ `0x0042f94c`–`0x0042f951` (node link, not `pAuxHeap`) |
| Gzip profiles → CIntList | `CDSChain_LoadConfigFromRegistry@0x0040a440` | `LEA ECX,[EBX+0x79]` → `CIntListInsertSortedOrAppend` @ `0x0040a53b`; 6-byte `OperatorNew(6)` records |
| Profile list teardown | `CBulanci_ClearProfileKeyList@0x004090c0` | Reads `profileList_count` @ `+0x81`, frees pointers from `profileList_data` @ `+0x79` |
| Level resources separate | `CGame_FindResourceByName@0x00413560` | Walks `pLevelResourceTable` @ `CGame+0x66` (= chain `+0x35`), not profile list |
| Full object only in `CGame` | `CGame_ctor@0x00414aeb` | Sole `CDSChain_ctor` embed: `this+0x31` |
| Config hydrated flag | `CDSChain_LoadConfigFromRegistry@0x0040a440` | `MOV byte [EBX+0x78],1` @ `0x0040a5db` before IDSChained vfn |

## Ghidra deltas

- `modify_struct_field` on `CDSChain_full`: `pRegistryProfileList` → `profileList_data`; `dwField_7d/81` → `profileList_capacity` / `profileList_count`; `bField_78` → `bConfigHydrated`
- `CDSChain` / `CDSChain_full`: `pAuxHeap` @ `+0x0c` / `+0x70` (already present)
- Decompiler/disasm comments: `CDSChained_InsertListNode@0x0042f820`, `CDSChain_RemoveListNode@0x0042f951`, `CDSChain_LoadConfigFromRegistry@0x0040a53b`
- `save_program bulanci.exe`

## Struct doc updates

- **`CDSChain.md`** — round 3 task 27 section; tail layout table; UNK narrowed for `pAuxHeap` allocator

## Remaining UNK

- `pAuxHeap` never allocated in this binary (legacy field or build variant).
- `dwField_85` @ `+0x85`: ctor writes `8` (matches `dwProfileCapacity` constant) but no reads.
- Ghidra nested `CIntList` type vs three scalars at `+0x79..+0x81`.
- `CDSChain_dtor@0x0042fcd0` on full `0xa4` object vs 20-byte list-head slice only.
- Relationship between **`CGame.chain`** (`CDSChain_full`) and heap **`CBulanciConfigStore`** (same tail offsets; app save/load path).
