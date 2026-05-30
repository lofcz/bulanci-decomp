# Round 5 — Worker 08 report (stream / chain / collection band)

## Task

| Field | Value |
|-------|--------|
| **worker** | 8 / 50 |
| **mode** | WRITE |
| **scope** | `FUN_*` tied to **CDSStream** / **CDSChain** / **CDSCollection** / **CDSStreamStorage** in `.text` band **`0x00430000`–`0x00470000`** (focus `0x0042f700`–`0x00434700`) |
| **note** | Parent band assignment overrides R5 manifest todo 8 (CGaming entity_list); this report covers the stream/chain sweep only. |

**Types:** `CDSStreamStorage`, `CDSCollection`, `CDSChain`, `CDSChained`, `CDSSafeStream`

## Status

**DONE** — Twelve `FUN_*` helpers in the stream/chain cluster renamed with disasm + xref proof; `CDSStreamStorage` heap factory split out; `bulanci.exe` saved.

## Evidence

| Claim | Function @ address | Disasm / xref proof |
|-------|-------------------|---------------------|
| Heap factory alloc `0x60` (= `CDSStreamStorage` size) | `CDSStreamStorage_CreateObject@0x004340f0` | `PUSH 0x60` @ `0x004340f0`; `CALL OperatorNewWithBadAlloc` @ `0x004340f2`; `JMP CDSStreamStorage_InitObjectFields` @ `0x00434100` |
| Vtable + embedded collection + chain + CS init | `CDSStreamStorage_InitObjectFields@0x00433df0` | `MOV [ECX], CDSStreamStorage::vftable` @ `0x00433df5`; collection faces @ `+0x1c`/`+0x20` (`param_1[7/8]`); `nM_growthChunk=0x20` @ `0x00433e31`; chain vtables @ `+0x34`/`+0x38` (`param_1[0xd/0xe]`); `LEA ECX,[EAX+0x48]` + `InitializeCriticalSection` @ `0x00433e3a` |
| Factory vtable slot | DATA `0x0047d010` | `get_xrefs_to 0x004340f0` → vtable install (IDS factory path) |
| Intrusive doubly-linked splice | `CDSChained_LinkIntrusiveNode@0x0042f780` | `MOV [ECX+0xc],*prev`; `MOV [ECX+0x8],*next`; `MOV *prev,ECX`; `MOV *next,ECX` @ `0x0042f780`–`0x0042f797` |
| Unlink node (+0x8/+0xc) | `CDSChained_UnlinkIntrusiveNode@0x0042f7a0` | Rewires `*(node+8)` / `*(node+0xc)` neighbors; zeros node links @ `0x0042f7b2`–`0x0042f7b7`; xref from `CDSChain_RemoveListNode@0x0042f965` |
| Reset circular head or splice before anchor | `CDSChained_ResetHeadOrSpliceBefore@0x0042f850` | `param_1==0` → `MOV [ECX+8],ECX` / `MOV [ECX+0xc],ECX` @ `0x0042f858`; else `CALL CDSChained_LinkIntrusiveNode` @ `0x0042f86b` |
| View-tree insert at anchor | `CDSChained_InsertChildAtAnchor@0x0042f9d0` | Sole caller `CDSView::AddChildInternal@0x0042bfd4`; branches to `CDSChained_AppendChild` / `CDSChained_PrependChild`; `ADD dword ptr [ECX+0x10],1` @ `0x0042f9e1` (`dwChildCount`) |
| Insert-before with head fixup | `CDSChained_InsertBeforeWithHeadFixup@0x0042fa20` | Caller `FUN_0042c160`; `CALL CDSChain_ReleaseAuxHeap` then `CDSChained_InsertBeforeAnchor`; updates head dword @ `this+0x08` when anchor is head |
| Remove with head fixup | `CDSChained_RemoveWithHeadFixup@0x0042fa50` | Caller `FUN_0042c190`; head advance then `CDSChained_UnlinkAndSpliceNode` |
| Thin wrappers | `CDSChained_InsertBeforeAnchor@0x0042f880` / `CDSChained_UnlinkAndSpliceNode@0x0042f890` | `CALL CDSChained_UnlinkIntrusiveNode` + `CDSChained_InsertListNode` / `CDSChained_ResetHeadOrSpliceBefore` |
| MSVC EH `CDSObject` release | `CDSObject_EhReleaseOwnedPtr@0x004344c0` | `TEST ECX,ECX`; `MOV EAX,[ECX]`; indirect `CALL [EAX+8]` @ `0x004344c8` (`IDSReferenced` release); 100+ `Unwind@0047xxxx` DATA xrefs |
| `CDSStreamStorage` layout | `get_struct_layout` | **96 B** — `CDSCollection collection` @ `+0x1c`, `CDSChain chain` @ `+0x34`, `CRITICAL_SECTION lock` @ `+0x48` |

### Already named (verified, no change)

| Address | Symbol | Role |
|---------|--------|------|
| `0x00401790` | `CDSStreamStorage_ctor` | In-place ctor + `InitRootSafeStream` |
| `0x00433f00` | `CDSStreamStorage_GetThreadLoaderNode` | Per-thread loader node on `chain` |
| `0x00433f70` | `CDSStreamStorage_AppendOrReuseStream` | CS + filter splice |
| `0x00434160` | `CDSStreamStorage_InitRootSafeStream` | `OperatorNew(0x48)` safe-stream |
| `0x00434760` | `CDSStreamStorage_CreateFilterSafeStream` | Filter wrapper |
| `0x00431000`–`0x00431360` | `CDSCollection_*` | Vector keyed collection |
| `0x0042f800`–`0x0042fc30` | `CDSChain_*` / `CDSChained_*` | List-head core |

### Out of band (not renamed)

| Address | Reason |
|---------|--------|
| `FUN_00433200` | RECT intersect helper; callers `CBulanek_CheckSlotCollision`, `CDSApp_AddDirtyRectCoalesced` |
| `FUN_004339e0` | SEH `LeaveCriticalSection` unwind stub only |
| `FUN_004466a0` | `CDSMpx` vtable release slot (MPx, not stream storage) |

## Ghidra deltas

- `create_function` + `rename` → `CDSStreamStorage_CreateObject@0x004340f0`
- `rename` `FUN_00433df0` → `CDSStreamStorage_InitObjectFields`
- `rename` `FUN_0042f780` → `CDSChained_LinkIntrusiveNode`
- `rename` `FUN_0042f7a0` → `CDSChained_UnlinkIntrusiveNode`
- `rename` `FUN_0042f850` → `CDSChained_ResetHeadOrSpliceBefore`
- `rename` `FUN_0042f9d0` → `CDSChained_InsertChildAtAnchor`
- `rename` `FUN_0042f880` / `FUN_0042f890` → `CDSChained_InsertBeforeAnchor` / `CDSChained_UnlinkAndSpliceNode`
- `rename` `FUN_0042fa20` / `FUN_0042fa50` → `CDSChained_InsertBeforeWithHeadFixup` / `CDSChained_RemoveWithHeadFixup`
- `rename` `FUN_004344c0` → `CDSObject_EhReleaseOwnedPtr`
- `set_function_prototype` on `InitObjectFields`, `CreateObject`, `InsertChildAtAnchor`
- `set_disassembly_comment` @ `0x004340f0`, `0x00433e3a`, `0x0042f9e1`, `0x004344c8`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSStreamStorage.md](./CDSStreamStorage.md) — factory/init split; leaf table
- [CDSChain.md](./CDSChain.md) — intrusive-list helper names; `pAuxHeap` caller list

## Remaining UNK

- `CDSChained_InsertBeforeWithHeadFixup` / `RemoveWithHeadFixup` still typed with `CBulanek *this` in decompiler (embedded list @ `+0x08`); needs `set_function_this_type` on a shared list-head struct when that type exists.
- `FUN_0042c160` / `FUN_0042c190` callers of head-fixup helpers still `FUN_*`.
- R5 manifest todo **8** (`CGaming` `entity_list` @ `+0x31c`, `CExplosion_OnEvent`) — **not executed** in this band sweep; assign to a dedicated worker or merge with parent plan.
