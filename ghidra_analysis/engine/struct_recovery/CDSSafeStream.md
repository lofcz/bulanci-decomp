# CDSSafeStream

## Status

**VERIFIED** — heap size **`0x48`**; per-thread stream slices via embedded **`CDSChain`** at `+0x18`, `CRITICAL_SECTION` at `+0x2c`, resource name at `+0x44`. Round 3 task 40: `dwM_streamFlags` write-only; `m_chain_auxHeap` teardown-only (`CDSChain_ReleaseAuxHeap`). MI vtables `+0x00..+0x14` match `CDSFilterStream` RTTI parentage (`stream_hierarchy.md` §5) but the object **does not** embed filter cursor fields — it uses `CDSChain` + thread-local `CDSSafeStreamInfo` nodes instead.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x48` | `0x004341b2` | `CBulanci::FUN_00434160`: `OperatorNewWithBadAlloc(0x48)` → `CDSSafeStream_ctor` |
| Same alloc | `0x0043483a` / `0x00434846` | `FUN_00434760` (stream wrap path): two `0x48` allocs + ctor |
| Last field ends at `0x48` | `CDSSafeStream_ctor@0x00433ab0` | `InitializeCriticalSection(this+0x2c)`; `*(this+0x44)=0` — CS is 24 bytes → name handle at `+0x44` |
| Dtor spans `+0x2c..+0x44` | `CDSSafeStream_dtor@0x00433bc0` | `CDsStringReleaseHeader` on `+0x44`; `DeleteCriticalSection(this+0x2c)`; `CDSChain_dtor(this+0x18)` |

## Layout table

Ghidra flattens the embedded **`CDSChain`** (`0x14`) at `+0x18` into named fields (same offsets as `CDSChain.md` list-head).

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vf_IDSReferenced` | `CDSSafeStream_ctor@0x00433ab0` `= 0x487474`; dtor restores `g_pCDSObject_vftable_IDSReferenced` |
| `+0x04` | 4 | `pointer` | `vf_IDSEventHandler` | ctor; `CBulanci::FUN_00434160@0x004341b2` returns `ctor_result+4` as `IDSEventHandler*` |
| `+0x08` | 4 | `uint` | `dwM_streamFlags` | ctor `= 0x20`; `CDSSafeStream_RegisterThreadSlice@0x00446ea0` sets `= 1`; `CDSSafeStream_Close` sets `= 0x20` |
| `+0x0c` | 4 | `pointer` | `vf_IDSStream` | ctor `= 0x487420` |
| `+0x10` | 4 | `int` | `nM_refCount` | ctor `= 1`; `CDSSafeStream_AddRef@0x00446ca0` |
| `+0x14` | 4 | `pointer` | `vf_IDSChained` | ctor `= 0x487408`; `CDSSafeStream_ReleaseViaChained@0x00433d70` |
| `+0x18` | 4 | `pointer` | `vf_chain_IDSReferenced` | ctor `= g_pCDSChain_vftable_IDSReferenced`; dtor `CDSChain_dtor(&vf_chain_IDSReferenced)` |
| `+0x1c` | 4 | `pointer` | `vf_chain_IDSChained` | ctor `= g_pCDSChain_vftable_IDSChained` |
| `+0x20` | 4 | `pointer` | `m_chain_head` | ctor `= 0`; `CDSSafeStream_GetThreadSlice@0x00446d90` walks via `CDSChained_GetFirstChildView(this+0x18)` |
| `+0x24` | 4 | `pointer` | `m_chain_auxHeap` | ctor `= 0` |
| `+0x28` | 4 | `int` | `nM_chain_count` | ctor `= 0`; `CDSChained_AppendChild` / clear paths (`CDSChain.md`) |
| `+0x2c` | 24 | `CRITICAL_SECTION` | `lock` | ctor `InitializeCriticalSection`; dtor `DeleteCriticalSection`; thread helpers enter/leave |
| `+0x44` | 4 | `pointer` | `m_streamName` | ctor `= 0`; `CDSSafeStream_RegisterThreadSlice` `CDsStringAssignFromHandle`; dtor `CDsStringReleaseHeader` |

## `dwM_streamFlags` (`+0x08`)

**Verdict (agent todo 40, 2026-05-30): write-only** — no `CMP`/`TEST` on `CDSSafeStream+0x08` in any `CDSSafeStream_*` vtable method (`search_instructions` per function: ctor, `InitBase`, `Close`, `ClearThreadSlices`, `GetThreadSlice`, `RegisterThreadSlice`, `Read`/`Seek`/`Tell`/`GetSize`). Lifecycle sentinel only (mirrors `CDSFilterStream::dwStreamState` @ `+0x08`).

| Value | Writer | Meaning (evidence) |
|-------|--------|-------------------|
| `0x20` | `CDSSafeStream_ctor@0x00433ad9` (`MOV [ESI+0x8],0x20`), `CDSSafeStream_InitBase@0x00446d33`, `CDSSafeStream_Close` (`*(iface+4)=0x20` → outer `+0x08`) | Closed/default (`stream_hierarchy.md` §2.2) |
| `1` | `CDSSafeStream_RegisterThreadSlice@0x00446f60` (`MOV [EDI+0x8],1`) after `CDSChained_AppendChild` | Active after first per-thread `CDSSafeStreamInfo` node |

Prototypes: `CDSSafeStream_RegisterThreadSlice(CDSSafeStream*, int*)` @ `0x00446ea0`, `CDSSafeStream_GetThreadSlice(CDSSafeStream*)` @ `0x00446d90`. Decompiler may still show `CDSSafeStreamInfo*` / `this->m_link_next = 1` at the flag store — ECX `this` retype blocked by Ghidra API; plate + PRE comments document true `CDSSafeStream*`.

## Ghidra apply

```
get_struct_layout CDSSafeStream → Size: 72 bytes (0x48), 13 fields (+0x18..+0x28 = embedded CDSChain list-head)
```

Applied in batch 38; verified slice 38 (2026-05-30).

## `m_chain_auxHeap` (`+0x24`, embedded `CDSChain+0x0c`)

**Verdict (agent todo 40, VERIFIED):** teardown-only — `CDSChain_ReleaseAuxHeap@0x0042f800` calls `Runtime_Free` on `pAuxHeap` when non-NULL before list splices. Callers: `CDSChained_AppendChild`, `CDSChain_RemoveListNode`, `FUN_0042f980`, `FUN_0042f9d0`, `FUN_0042fa20`, `FUN_0042fa50`. Ctor zeros @ `CDSSafeStream_ctor+0x00433b19` (sole program `MOV` to `+0x24`). **No allocator xref** in program-wide `search_instructions` for `[reg+0x24]` beyond zero-init.

## UNK

- Exact MI hierarchy field-sharing vs `CDSFilterStream` beyond shared vtable cluster (safe stream replaces filter body with `CDSChain`).
- `CDSSafeStream_Write` always raises unsupported (`@0x00446c00`) — no write buffer fields.
- Who allocates the heap block freed at `CDSChain+0x0c` on non–safe-stream chain heads (if ever non-NULL).

## References

- `CDSSafeStream_ctor` — `0x00433ab0`
- `CDSSafeStream_dtor` — `0x00433bc0`
- `CDSSafeStream_GetThreadSlice` — `0x00446d90` — thread-local slice lookup / lazy `CDSSafeStreamInfo` node
- `CDSSafeStream_RegisterThreadSlice` — `0x00446ea0` — ctor registration of owning thread node + name copy
- `CDSSafeStream_ClearThreadSlices` — `0x00446c30` — clears chain + stream name under `lock`
- `CBulanci::FUN_00434160` — `OperatorNew(0x48)` @ `0x004341b2`
- `CDSStreamStorage_CreateFilterSafeStream` — dual `0x48` allocs @ `0x0043483a` / `0x00434846`
- `CDSFilterStream.md` / `CDSQueueStream.md` — sibling `IDSStream` implementers
- `CBulanci::FUN_00434160` — primary game attachment site
