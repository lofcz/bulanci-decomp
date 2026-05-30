# Round 5 — worker 28 report

## Task

| Field | Value |
|-------|-------|
| **worker** | 28 / 50 |
| **round** | 5 |
| **title** | CDSChain.md profile tail UNKs (`CDSChain_full` `+0x78..+0xa3`) |
| **types** | `CDSChain`, `CDSChain_full`, `CGame`, `CBulanciConfigStore` |
| **addresses** | `0x0040a680`, `0x0040a440`, `0x00409cd0`, `0x004090c0`, `0x0040a380`, `0x00413160` |

## Status

**DONE** — profile/registry tail fields verified; Ghidra padding rename + comments; `save_program bulanci.exe`.

## Evidence

| Field / topic | func@addr | Verdict |
|---------------|-----------|---------|
| `profileList` @ `+0x79` | `CIntListInsertSortedOrAppend@0x00407e20` | 12-byte head: `pData@+0`, `nCapacity@+4`, `nCount@+8` (no `nM_growthChunk`) |
| Profile heap node | `CDSChain_LoadConfigFromRegistry@0x0040a440` | `OperatorNew(6)` + `ReadBytes(..., 6)` per gzip profile |
| Factory defaults | `CBulanci_InstallFactoryDefaults@0x0040a290` | Same 6-byte layout from `g_dwScrollBarBitmapIds` template |
| Save count byte | `SaveConfigToRegistry@0x00409cd0` | `(byte)dwProfileCount` written before 6-byte records |
| Clear / free | `CBulanci_ClearProfileKeyList@0x004090c0` | **Low byte** at `config+0x81`; frees `*(pData + i*4)` |
| Teardown | `CBulanci_DestroyConfigStore@0x0040a380` | `ClearProfileKeyList` then `dwProfileList_count=0`; `CDSPtrSlotVec_Resize(&pProfileList_data,0)` (8-byte vec cast — frees slot array only) |
| Runtime consumer | `CGame_GetControlBindingTable@0x00413160` | `*(pProfileList_data + playerRec[slot].byte*4)` — indexes pointer table |
| `bConfigHydrated` @ `+0x78` | `CDSChain_ctor@0x0040a709`, `CDSChain_LoadConfigFromRegistry@0x0040a5db`, `Catch_0040a62f@0x0040a643` | Writes only (`MOV byte [base+0x78], imm`) |
| `dwProfileCapacityMirror` @ `+0x85` | `CDSChain_ctor@0x0040a6fc` | Sole program write: `MOV dword [ESI+0x85], ECX` (ECX=8); **no reads** of `[reg+0x85]` |
| `pad_89` @ `+0x89..+0xa3` | object size `0xa4` | 27 bytes padding; no ctor/RW on `CDSChain_full` base |
| False-positive alias | `FUN_00417c80@0x00417c80` | Decompiler `chain.dwProfileCapacityMirror+2` is **`CBulanci+0x33c`** ambient-anim dword, not chain tail |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CDSChain_full.pPad_a4` | → `pad_89` |
| `set_decompiler_comment` | `0x0040a6fc`, `0x0040a5db`, `0x0040a643`, `0x004090c0`, `0x00407e20` | tail semantics documented |
| `save_program` | `bulanci.exe` | saved |

Nested `CDSIntPtrListHead` at `+0x79` **not applied** (adjacent named scalars + `bConfigHydrated` byte at `+0x78`).

## Struct doc updates

- [CDSChain.md](./CDSChain.md) — layout table, R5 worker 28 section, UNK list narrowed

## Remaining UNK

- Nest `CDSIntPtrListHead` at `+0x79` after Ghidra field consolidation.
- Name 6-byte profile record type (`CBulanciProfileRec`?) and document byte layout (factory template vs gzip).
- Replace / align stale `CBulanciConfigStore` (133 B) with `CDSChain_full` (164 B) for save/load prototypes.
- `CDSChain_RemoveListNode` decompiler singleton test (`pAuxHeap==pChain`; disasm `CMP [ECX+0xc],ECX` @ `0x0042f951`).
