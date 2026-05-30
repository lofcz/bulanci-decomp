# Round 5 — worker 47 report (R5 worker 47/50)

## Task

| Field | Value |
|-------|-------|
| **worker** | 47 |
| **round** | 5 |
| **mode** | WRITE |
| **title** | FUN_* xref sweep `0x00401000`–`0x00430000` (top 5 rename) |
| **acceptance** | Ghidra search `FUN_` by xref count in band; rename top 5 with disasm/xref proof or document blockers |

## Status

**DONE** — all five in-band leaders renamed, prototyped, and commented; `bulanci.exe` saved.

## Search methodology

| Step | Result |
|------|--------|
| `search_functions_enhanced` (`^FUN_`, sort `xref_count`) | Global #1 `FUN_004344c0` @ `0x4344c0` (**185** xrefs) — **outside** band (≥ `0x430000`) |
| Inline Ghidra script (`FUN_` entry ∈ [`0x401000`,`0x430000`)) | **137** symbols; top 5 below |

### Top 5 in band (by xref count)

| Rank | Old name | Address | Xrefs | New name | Outcome |
|-----:|----------|---------|------:|----------|---------|
| 1 | `eh_CDsStringReleaseHeader` / `FUN_00401340` | `0x00401340` | 141 | `CDSString_handle_EH_dtor` | **RENAMED** |
| 2 | `eh_Runtime_FreeMemberPtr` / `FUN_00401520` | `0x00401520` | 35 | `Runtime_HeapPtr_EH_dtor` | **RENAMED** |
| 3 | `FUN_00407e10` | `0x00407e10` | 31 | `CDSPtrSlotVec_EH_dtor` | **RENAMED** |
| 4 | `Int32_ZeroInit_fastcall` / `FUN_00409010` | `0x00409010` | 21 | `eh_ctor_ZeroDword` | **RENAMED** |
| 5 | `FUN_0042e8f0` | `0x0042e8f0` | 19 | `ClassRegEntry_PrependListHead` | **RENAMED** |

**Out-of-band note:** program-wide #1 `FUN_004344c0` @ `0x4344c0` (185 xrefs) is the EH dtor for `CDSObject*` (`vtable+8` release); excluded by upper bound `0x430000`.

## Evidence (per rename)

### 1 — `CDSString_handle_EH_dtor` @ `0x00401340`

| Claim | Address | Evidence |
|-------|---------|----------|
| EH unwind helper | `0x00401340` | All xrefs from `Unwind@*` stubs (e.g. `0x00473f53`) — MSVC `eh_vector_destructor_iterator` glue |
| Release path | `0x00401346`–`0x00401349` | `MOV EAX,[ECX]`; `TEST EAX,EAX`; `LEA ECX,[EAX-0xc]`; `JMP CDsStringReleaseHeader` |
| Element size | call sites | Passed with **stride 4** to `_eh_vector_destructor_iterator_` (CDSString handle slots) |

### 2 — `Runtime_HeapPtr_EH_dtor` @ `0x00401520`

| Claim | Address | Evidence |
|-------|---------|----------|
| Frees `*pHeapPtr` | `0x00401527` | `MOV ECX,0x4b7c94` → global allocator object (`DAT_004b7c94`) |
| Callee | `0x0040152x` | `CALL Runtime_Free` (same pattern as `CDSImage_dtor`, `CDSPtrSlotVec_Resize`, etc.) |
| Role | xref set | 35 `Unwind@` callers — SEH cleanup for embedded raw heap pointer fields |

### 3 — `CDSPtrSlotVec_EH_dtor` @ `0x00407e10`

| Claim | Address | Evidence |
|-------|---------|----------|
| `this` = `CDSPtrSlotVec*` | `0x00407e19` | `CALL CDSPtrSlotVec_Resize` @ `0x00406340` with `ECX` unchanged |
| Pre-clear | `0x00407e12` | `MOV dword ptr [ECX+0x8],0` (dword past 8-byte `{pSlots,cCapacity}` — tail field on embedded vector storage) |
| Resize teardown | `0x00407e10` | `PUSH 0`; `CALL CDSPtrSlotVec_Resize(pSlotVec,0)` |
| Prior mis-type | `_Globals.h` | Was declared `CDSAudioBank*`; disasm proves **`CDSPtrSlotVec*`** argument |

### 4 — `eh_ctor_ZeroDword` @ `0x00409010`

| Claim | Address | Evidence |
|-------|---------|----------|
| Default-init | `0x00409012` | `MOV dword ptr [EAX],0` (`EAX=ECX` = element pointer) |
| EH vector ctor callback | DATA xrefs | e.g. `CGameCounter_Constructor@0x0040bc52`, `CPanel_ctor@0x00427542`, `CGaming_ctor@0x00420000` — 4-byte stride `_eh_vector_constructor_iterator_` |
| 6-byte analogue | `0x004097d0` | `CGame_PlayerRec_ctor` zeros `*(elem+2)`; paired dtor `CGame_PlayerRec_dtor@0x004097e0` |

### 5 — `ClassRegEntry_PrependListHead` @ `0x0042e8f0`

| Claim | Address | Evidence |
|-------|---------|----------|
| Intrusive prepend | `0x0042e8f0` body | `*(void**)this = DAT_004b7be0`; `DAT_004b7be0 = this`; `*(this+4) = param` |
| Static init consumer | `0x0047c857` | `CALL ClassRegEntry_PrependListHead` in `CDSStaticTexts` registration block (class id **8**) |
| Doc cross-ref | `static_texts.md` | `ClassRegEntry { next@+0, parent_meta_ptr@+4, … }`; list head `DAT_004b7be0` |
| Distinct from factory register | `0x0042e910` | `HandleClassRegister` pushes full node — different helper |

## Ghidra deltas

- `rename_function_by_address` ×5 (addresses above)
- `set_function_prototype` ×5 (`__fastcall` EH helpers; `__thiscall` list prepend)
- `set_decompiler_comment` ×5 (entry points)
- `save_program bulanci.exe`

## Struct doc updates

None (runtime/EH helpers only; no layout fields).

## Remaining UNK

| Symbol | Why not in top-5 band |
|--------|------------------------|
| `FUN_004344c0` @ `0x4344c0` | 185 xrefs but **above** `0x430000` — follow-up band `0x430000`–`0x470000` (worker 08 territory) |
| `FUN_004075f0` @ `0x4075f0` | Rank #6 in band (16 xrefs) — `CRadio_AddOption` per worker 17; out of scope for this slice |

## Files

- [round5_worker_47_results.jsonl](./round5_worker_47_results.jsonl)
- Coordinator jsonl: `ghidra_analysis/engine/agent_todos_50_r5_results.jsonl` (appended)
