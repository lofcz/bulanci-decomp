# Round 4 — Task 46 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 46 |
| **round** | 4 |
| **title** | InitRootSafeStream vcall through collection facet |
| **source** | handoff |
| **supersedes_todo_id** | 46 |
| **prior** | [round3_task_46_report.md](./round3_task_46_report.md) |
| **structs** | CDSStreamStorage, CDSCollection, CDSChain |

## Status

**DONE** — R3 UNK on the collection-facet indirect call closed by disasm; offset capture reclassified as `CDSSafeStream_Tell` (not `IStream::Seek`). Ghidra PRE comments + `save_program`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Mystery indirect call site | `0x004341d8`–`0x004341e6` | `LEA ECX,[ESI+0x20]`; `MOV EDX,[ECX]` → `g_pCDSCollection_vftable_IDSChained@0x0047f6e4`; `PUSH EDI`; `CALL [EDX+0x10]` → `CDSCollection_Load@0x00431360` |
| IDSChained `this` for call | `0x004341d8` | `ECX` = address of `collection.m_pVtable_IDSChained` (`CDSStreamStorage+0x20`), not `CDSCollection+0x1c` base |
| Stack arg to mystery call | `0x004341e2` | `EDI` = `LEA [CDSSafeStream_ctor_result+4]` = `&vf_IDSEventHandler` (same value stored to `pRootSafeStream@+0x18`) |
| Not literal pack `Load` | `CDSCollection_Load@0x00431360` | Decompile begins with `CDSCollection_Resize(..., 0)` — would clear embed during `CDSStreamStorage_ctor` → `InitRootSafeStream` path |
| Offset capture call | `0x004341e8`–`0x004341f5` | `ECX=[ESI+0x18]`; `CALL [vtable+0x20]`; stores `EAX`/`EDX` → `dwStreamBaseOffsetLo/Hi` @ `+0x10` |
| Vtable slot +0x20 on stored facet | `g_pCDSApp` shared cluster / `0x00487434` | Slot `+0x20` = `CDSSafeStream_Tell@0x00446fe0` (IDSEventHandler face; `param_1+(-4)` adjustor) |
| Sole caller | `CDSStreamStorage_ctor@0x00401827` | `get_function_xrefs` → unconditional call into `InitRootSafeStream` |
| R3 this-type fix holds | `0x00434160` | Decompile `CDSStreamStorage::InitRootSafeStream(CDSStreamStorage *this, int *pUnderlyingStream)` |

### Call sequence (verified disasm)

1. `OperatorNew(0x48)` + `CDSSafeStream_ctor(pUnderlyingStream)` — thread slice already registered in ctor.
2. Release prior `pRootSafeStream` via `IDSReferenced` vfn `+0x8` when non-NULL (`0x004341ca`–`0x004341d6`).
3. Store `EDI` (`&vf_IDSEventHandler`) → `pRootSafeStream@+0x18` (`0x004341db`).
4. Indirect call through **collection chained vtable** slot `+0x10` (`0x004341d8`–`0x004341e6`) — Ghidra symbol `CDSCollection_Load`; **semantic mismatch** documented as decompiler/vfunc artifact.
5. `CDSSafeStream_Tell` on stored facet → `dwStreamBaseOffsetLo/Hi` (`0x004341e8`–`0x004341f5`).

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00434160` | Function header: R4 summary (collection call artifact + Tell) |
| `set_decompiler_comment` | `0x004341d8` | PRE on IDSChained `this=@+0x20` call |
| `set_decompiler_comment` | `0x004341e8` | PRE on Tell → `dwStreamBaseOffsetLo/Hi` |
| `force_decompile` | `0x00434160` | Comments visible; collection call line unchanged (expected) |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSStreamStorage.md](./CDSStreamStorage.md) — R4 task 46 table; method row clarifies Tell vs Seek; `pRootSafeStream` stores IDSEventHandler facet.

## Remaining UNK

- **True runtime callee** behind collection chained slot `+0x10` at `0x004341e6` if not `CDSCollection_Load` (would need trace / override beyond static vtable label).
- Whether MSVC intended `PUSH pUnderlyingStream` instead of `PUSH EDI` for that site (static disasm shows `EDI`; game path works with ctor-time stream already wired in `CDSSafeStream_ctor`).
