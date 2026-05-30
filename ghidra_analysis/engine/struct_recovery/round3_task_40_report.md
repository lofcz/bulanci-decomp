# Round 3 — task 40 report

## Task

| Field | Value |
|-------|--------|
| **id** | 40 |
| **title** | CDSSafeStream auxHeap allocator + VERIFIED pass |
| **one_liner** | Xref allocator for `m_chain_auxHeap` @ `CDSSafeStream+0x24` (`CDSChain+0x0c`); confirm `dwM_streamFlags` write-only; plate `ECX=CDSSafeStream*` @ `RegisterThreadSlice` / `GetThreadSlice`. |
| **acceptance** | Update `CDSSafeStream.md`; Ghidra rename `FUN_0042f800` → `CDSChain_ReleaseAuxHeap`; `save_program`; `status.md` confidence |

## Status

**DONE** — `CDSSafeStream` layout **`0x48`** holds on Ghidra struct; `m_chain_auxHeap` is **teardown-only** (no allocator xref on the safe-stream path); `dwM_streamFlags` is **write-only** in all `CDSSafeStream_*` vtable methods searched.

## Evidence

| Claim | Func @ addr | Finding |
|-------|-------------|---------|
| `m_chain_auxHeap` @ **`P+0x24`** (= embedded `CDSChain+0x0c`) | `CDSSafeStream_ctor@0x00433ab0` | Sole store: `MOV dword ptr [ESI+0x24], EDI` @ `0x00433b19` with `EDI=0` |
| Aux heap free before list splice | `CDSChain_ReleaseAuxHeap@0x0042f800` | `if (pChain->pAuxHeap) Runtime_Free(...)`; renamed from `FUN_0042f800` |
| Free helper callers | xrefs to `0x0042f800` | `CDSChained_AppendChild@0x0042f9b3`, `CDSChain_RemoveListNode@0x0042f943`, `FUN_0042f980`, `FUN_0042f9d0`, `FUN_0042fa20`, `FUN_0042fa50` |
| No non-zero store to safe-stream `+0x24` | `search_instructions` MOV `0x24` | Only `CDSSafeStream_ctor` among `CDSSafeStream_*`; other `MOV [reg+0x24]` hits are unrelated types / stack slots |
| `dwM_streamFlags` writers | ctor / `InitBase` / `Close` / `RegisterThreadSlice` | `0x20` @ `0x00433ad9`, `0x00446d33`, `Close`; `1` @ `0x00446f60` after `CDSChained_AppendChild` |
| `dwM_streamFlags` not read in methods | `search_instructions` CMP/TEST `+0x8` | Zero hits in `CDSSafeStream_Read`, `Close`, `InitBase`, `ctor`, `GetThreadSlice` |
| `RegisterThreadSlice` true `this` | `CDSSafeStream_RegisterThreadSlice@0x00446ea0` | Plate + PRE: `ECX=CDSSafeStream*`; decompiler still shows `CDSSafeStreamInfo*` alias at `+0x08` for flag store |
| `GetThreadSlice` walks chain @ `P+0x18` | `CDSSafeStream_GetThreadSlice@0x00446d90` | Decompile `CDSSafeStream*`; `CDSChained_GetFirstChildView(&vf_chain_IDSReferenced)`; thread id @ child `+0x10` |
| Game alloc site | `CBulanci::FUN_00434160@0x004341b2` | `OperatorNewWithBadAlloc(0x48)` → `CDSSafeStream_ctor` |
| Ghidra struct | `get_struct_layout CDSSafeStream` | **72 B**; `m_chain_auxHeap` @ **+0x24**; `nM_chain_count` @ **+0x28**; `lock` @ **+0x2c**; `m_streamName` @ **+0x44** |

### `m_chain_auxHeap` lifecycle

1. **Ctor** — field zeroed (`CDSSafeStream+0x24`).
2. **Runtime** — no program xref stores a non-NULL pointer into this slot on `CDSSafeStream` instances (allocator for `CDSChain+0x0c` on other chain heads remains UNK).
3. **Mutation paths** — `CDSChained_AppendChild` / `CDSChain_RemoveListNode` / sibling helpers call `CDSChain_ReleaseAuxHeap` first, which frees `pAuxHeap` if set, then splices the intrusive list.

### `dwM_streamFlags` (`+0x08`)

| Value | Writer | Role |
|-------|--------|------|
| `0x20` | ctor, `InitBase`, `Close` | Closed / default (`stream_hierarchy.md` §2.2) |
| `1` | `RegisterThreadSlice@0x00446f60` | Active after first per-thread `CDSSafeStreamInfo` node |

Lifecycle sentinel only — mirrors `CDSFilterStream::dwStreamState` pattern; not consulted in `Read`/`Seek`/`Tell`/`GetSize`/`GetThreadSlice`.

## Ghidra deltas

- `CDSChain_ReleaseAuxHeap@0x0042f800` (pre-renamed in batch 38 follow-up; verified decompile + comment).
- Plate / PRE comments @ `0x00446ea0`, `0x00446d90`; decompiler header on `ReleaseAuxHeap`.
- `get_struct_layout CDSSafeStream` → 72 bytes (unchanged).
- `save_program bulanci.exe` — OK (per `batch_38_followup_summary.md` / worker 40).

## Struct doc updates

- [CDSSafeStream.md](./CDSSafeStream.md) — status **VERIFIED** for `0x48` layout, write-only flags, teardown-only `m_chain_auxHeap`.

## Remaining UNK

- Allocator that would populate `CDSChain+0x0c` on non–safe-stream chain heads (if ever non-NULL).
- Exact MI field-sharing vs `CDSFilterStream` beyond shared vtable cluster.
- `CDSSafeStream_Write` unsupported — no write buffer fields.
