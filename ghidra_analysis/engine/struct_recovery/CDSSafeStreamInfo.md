# CDSSafeStreamInfo

## Status

**VERIFIED** — per-thread chain node allocated at **`0x18`**; fields proven from heap ctor paths in `CDSSafeStream_RegisterThreadSlice` and `CDSSafeStream_GetThreadSlice`, dtor, and list walk. Doubly-linked list links at `+0x08` / `+0x0c` proven by `FUN_0042f780` / `FUN_0042f820` via `CDSChained_AppendChild`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x18` | `CDSSafeStream_RegisterThreadSlice@0x00446ea0` | `OperatorNewWithBadAlloc(0x18)` before field init |
| Same alloc | `CDSSafeStream_GetThreadSlice@0x00446d90` | Lazy node creation on missing `GetCurrentThreadId` match |
| Six dwords | both ctors | `puVar3[0..5]` written; dtor uses `param_1[5]` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vf_IDSReferenced` | `CDSSafeStream_RegisterThreadSlice` / `CDSSafeStream_GetThreadSlice`: `*puVar3 = CDSSafeStreamInfo::vftable` |
| `+0x04` | 4 | `pointer` | `vf_IDSChained` | `puVar3[1] = g_pCDSSafeStreamInfo_vftable_IDSChained` |
| `+0x08` | 4 | `pointer` | `m_link_next` | init `puVar3[2]=0`; `FUN_0042f780@0x0042f780` splices `param_1+0xc` / `+0x08` |
| `+0x0c` | 4 | `pointer` | `m_link_prev` | init `puVar3[3]=0`; `FUN_0042f820` uses `param_1+0xc` as back-link |
| `+0x10` | 4 | `dword` | `dwM_threadId` | `puVar3[4]=GetCurrentThreadId()`; `CDSSafeStream_GetThreadSlice` compares `*(node+0x10)` |
| `+0x14` | 4 | `pointer` | `m_streamSlice` | `puVar3[5]=param_1` or cast stream from `CheckedVirtualBaseCast`; dtor calls `(*[5]+8)` release |

## Ghidra apply

```
get_struct_layout CDSSafeStreamInfo → Size: 24 bytes (0x18), 6 fields
```

Verified slice 38 (2026-05-30). Lazy alloc in `CDSSafeStream_GetThreadSlice@0x00446d90` and `CDSSafeStream_RegisterThreadSlice@0x00446ea0`.

## UNK

- **`m_streamSlice` (`+0x14`) type on lazy alloc** — **partial (R5 worker 42):** `CDSSafeStream_GetThreadSlice@0x00446d90` walks chain for `dwM_threadId` match; on miss, loads first child stream, `CheckedVirtualBaseCast(pv, 2)`, calls chained vfn slot `+0x10`, then `CheckedVirtualBaseCast(..., 7)` into `puVar4[5]` (`m_streamSlice`). Treat as **`IDSStream *`** face (engine interface index `7`); exact RTTI token not resolved to a named struct this pass.
- Virtual methods on `CDSSafeStreamInfo` beyond dtor / `GetTypeInfo` (`0x00446cb0`) — not required for layout.

## References

- `CDSSafeStream_RegisterThreadSlice` — `0x00446ea0`
- `CDSSafeStream_GetThreadSlice` — `0x00446d90`
- `CDSSafeStreamInfo_dtor` — `0x00446cd0`
- Parent list head: `CDSSafeStream.chain` @ `+0x18` (`CDSChain.md`)
