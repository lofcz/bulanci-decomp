# CDSEasyMemStream

## Status

`VERIFIED`

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSEasyMemStream) == 0x2c` | `0x00430e70` | `CDSEasyMemStream_CreateFromStreamSlice` → `OperatorNewWithBadAlloc(0x2c)` |
| Last field ends at `0x28+4` | `0x00409270` | `CDSEasyMemStream_dtor` frees `param_1[10]` → offset `0x28` |
| Ghidra struct size | — | `get_struct_layout("CDSEasyMemStream")` → **44 bytes** |

## Layout table

Offsets are on the **outer** object. `IDSStream` vtable methods receive `this == outer + 0x0c`; field accesses in those methods use `this+0x8` → outer `+0x14`, etc.

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `vf_IDSReferenced` | `CDSEasyMemStream_ctor@0x00409170` writes `0x00480430`; dtor @ `0x00409270` restores `g_pCDSObject_vftable_IDSReferenced` |
| `+0x04` | 4 | `void *` | `vf_IDSEventHandler` | ctor writes `0x0048041c`; dtor restores same slot |
| `+0x08` | 4 | `uint` | `dwRefcount` | ctor writes `1`; `IDSStream_Release@0x00409490` → `IDSStream_ReleaseRefcount(outer)` with `outer = idsStreamThis - 0xc` |
| `+0x0c` | 4 | `void *` | `vf_IDSStream` | ctor writes `0x004803dc`; dtor restores; stream methods use `this == outer + 0x0c` |
| `+0x10` | 4 | `uint` | `dwStreamState` | ctor / `CloseStream@0x00409200` → `0x20` **CLOSED**; `InitBackingBuffer@0x00430d60` → `7` **OPEN** (backing_heap valid); errno `8` if `backing_heap` null on I/O |
| `+0x14` | 4 | `uint` | `dwCursor` | `ReadBytes@0x004307f0`, `WriteBytes@0x004308c0`, `SeekPosition@0x00430980`, `TellPosition@0x00409230` (`IDSStream` `this`) |
| `+0x18` | 4 | `uint` | `dwLogical_size` | `GetSize@0x00409220`, `WriteBytes@0x004308c0`, `SetStreamSize@0x004309f0` |
| `+0x1c` | 4 | `uint` | `dwCapacity_field` | `CDSEasyMemStream_InitBackingBuffer@0x00430d60` stores initial capacity; `CDSEasyMemStream_EnsureCapacity@0x004306e0` compares/grows |
| `+0x20` | 4 | `uint` | `dwGrowth_chunk` | `InitBackingBuffer` stores align chunk; `EnsureCapacity` uses for realloc step |
| `+0x24` | 4 | `uint` | `dwRing_head_offset` | `InitBackingBuffer` clears; ring branches in `ReadBytes` / `WriteBytes` |
| `+0x28` | 4 | `void *` | `backing_heap` | `InitBackingBuffer` malloc; `CDSEasyMemStream_dtor@0x00409270` `Runtime_Free` |

## Ghidra apply

```
get_struct_layout("CDSEasyMemStream") → Size: 44 bytes
  vf_IDSReferenced, vf_IDSEventHandler, vf_IDSStream (batch 28 follow-up renames)
```

**Slice 28 (2026-05-30):** Leaf renames — `CDSEasyMemStream_InitBackingBuffer@0x00430d60`, `CDSEasyMemStream_EnsureCapacity@0x004306e0`, `CDSEasyMemStream_CreateFromStreamSlice@0x00430e70`; prototypes set.

## Leaf functions

| Address | Name | Role |
|---------|------|------|
| `0x00409170` | `CDSEasyMemStream_ctor` | MI vtables + `InitBackingBuffer` |
| `0x00409270` | `CDSEasyMemStream_dtor` | Free `backing_heap`, restore `vf_IDSReferenced` |
| `0x004306e0` | `CDSEasyMemStream_EnsureCapacity` | Grow/repack ring or linear buffer |
| `0x004307f0` | `ReadBytes` | `IDSStream` slot; `this` = outer `+0x0c` |
| `0x004308c0` | `WriteBytes` | Calls `EnsureCapacity(outer, cursor+count)` via `this-0xc` adjust |
| `0x00430d60` | `CDSEasyMemStream_InitBackingBuffer` | Malloc backing; sets `dwIdsStream_state = 7` |
| `0x00430e70` | `CDSEasyMemStream_CreateFromStreamSlice` | Read source stream into new `0x2c` object |

## UNK

- Full `IDSEventHandler` slot map on `vf_IDSEventHandler` beyond ctor/dtor restore pattern.
- **`IDSStream` plate struct (40 B)** shares offset bands across implementers; filter `ReadBytes` uses `+0x0c..+0x24` (R4 todo 30). Mem stream keeps `dwCursor` @ `+0x8` and casts `dwSizeCapLo` @ `+0x1c` to the backing pointer. `CDSQueueStream` plate @ outer `+0x04` remains separate.

## Follow-up resolved (agent todo 29)

- **`dwStreamState`** (outer `+0x10`, decompiler alias `dwIdsStream_state` on `IDSStream+4`): `0x20` = **CLOSED** (default in ctor @ `0x00409170`, restored by `CloseStream@0x00409200`); `7` = **OPEN** after `InitBackingBuffer@0x00430d60` malloc succeeds — same `0x20` closed sentinel as `CDSFileStream` / `CDSFilterStream` / `CDSGZipStream` `CloseStream`. I/O throws stream errno `8` when `backing_heap` is null (pre-init or after `ReleaseBackingBuffer`). Ghidra PRE_COMMENT on ctor, `InitBackingBuffer`, and `CloseStream`.

**R4 todo 29 (2026-05-30):** Created Ghidra **`IDSStream`** (32 B plate: `pVftable` … `pBacking_heap`). `set_function_this_type` on `ReadBytes@0x004307f0`, `WriteBytes@0x004308c0`, `SeekPosition@0x00430980`, `TellPosition@0x00409230`, `GetSize@0x00409220`, `SetStreamSize@0x004309f0`, `CloseStream@0x00409200` — decompile now uses `this->dwCursor`, `pBacking_heap`, `dwRing_head_offset`, etc. `WriteBytes` calls `CDSEasyMemStream_EnsureCapacity((CDSEasyMemStream *)&this[-1].dwGrowth_chunk, …)` for outer recovery. `save_program`.

**R4 todo 30 (2026-05-30):** Grew `IDSStream` to **40 B** and renamed `+0x0c..+0x24` for filter passthrough (`dwCursorLo`/`Hi`, `nSizeCapHi`, `pInnerStream`). Mem `ReadBytes` still type-checks via cast from `dwSizeCapLo`; see [round4_task_30_report.md](./round4_task_30_report.md).

## Notes (follow-up batch 26)

- **Not** a `CDSFilterStream` subclass: RTTI lists both as separate `IDSStream` implementers (`stream_hierarchy.md` §2); heap size is only `0x2c` (filter uses inner stream at `+0x24` and a larger object — see batch 19 `CDSFilterStream`).
- `dwRefcount` at `+0x08` is the field `IDSStream_ReleaseRefcount@0x00401660` mutates when `IDSStream_Release` passes `outer = idsStreamThis - 0xc`.
