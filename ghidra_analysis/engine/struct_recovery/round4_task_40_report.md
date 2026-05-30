# Round 4 — agent todo 40 report (R4 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 40 |
| **title** | CDSChain pAuxHeap allocator and CDSSafeStream vs CDSFilterStream MI |
| **source** | `todos_gather_r4_3.json` / `agent_todos_50_r4.json` (handoff; supersedes R3 todo 40) |
| **types** | CDSSafeStream, CDSChain, CDSFilterStream, CDSChained |
| **addresses** | `0x0042f800`, `0x0042f9b3`, `0x00433ab0`, `0x00446ea0`, `0x00430ca0`, `0x00434160` |
| **acceptance** | Confirm `pAuxHeap` allocator or document teardown-only; document MI field-sharing vs `CDSFilterStream`; `save_program` |

## Status

**DONE**

## Evidence

### `pAuxHeap` / `m_chain_auxHeap` — teardown-only (no allocator)

| Claim | Func @ addr | Finding |
|-------|-------------|---------|
| Release helper | `CDSChain_ReleaseAuxHeap@0x0042f800` | `if (pChain->pAuxHeap) Runtime_Free(...)`; decompile comment updated R3/R4 |
| Callers | xrefs `0x0042f800` | `CDSChained_AppendChild`, `CDSChain_RemoveListNode`, `CDSChained_PrependChild@0x0042f980`, `FUN_0042f9d0`, `FUN_0042fa20`, `FUN_0042fa50` |
| Embedded safe-stream field | `CDSSafeStream_ctor@0x00433ab0` | Sole `MOV [obj+0x24]` = zero @ `0x00433b19` |
| Full chain list-head | `CDSChain_ctor@0x0040a680` | Sole `MOV [obj+0x70]` = zero @ `0x0040a6ea` (`CDSChain_full.pAuxHeap`) |
| Program-wide stores | `search_instructions` `MOV` + `0x24]` / chain-scoped `+0x70]` | No non–zero-init store to list-head aux slot; R4 re-run matches R3 task 27/40 |
| Sentinel disambiguation | `CDSChain_RemoveListNode@0x0042f940` | `CMP [ECX+0xc], ECX` is intrusive **node link**, not `pAuxHeap` (R3 task 27) |

**Verdict:** `pAuxHeap` is a legacy/teardown hook in this build — always NULL at runtime; `ReleaseAuxHeap` is defensive before list splices.

### CDSSafeStream vs CDSFilterStream — MI header vs body

Both inherit the `IDSStream` cluster through `CDSFilterStream` RTTI (`stream_hierarchy.md` §5). **Shared prefix** through `IDSChained` @ `+0x14` (56-bit stream header):

| Offset | `CDSFilterStream` (`0x38`) | `CDSSafeStream` (`0x48`) |
|--------|---------------------------|---------------------------|
| `+0x00` | `pVftable_IDSReferenced` | `vf_IDSReferenced` |
| `+0x04` | `pVftable_IDSEventHandler` | `vf_IDSEventHandler` |
| `+0x08` | `dwStreamState` | `dwM_streamFlags` (write-only; R3) |
| `+0x0c` | `pVftable_IDSStream` | `vf_IDSStream` |
| `+0x10` | `dwIdsStream_state` | `nM_refCount` |
| `+0x14` | `pVftable_IDSChained` | `vf_IDSChained` |
| `+0x18` | **Filter body:** `dwCursorLo` … `pInnerStream` @ `+0x30` | **Embedded `CDSChain`:** `vf_chain_*`, `m_chain_head`, `m_chain_auxHeap`, `nM_chain_count` |
| `+0x2c` | — | `CRITICAL_SECTION lock` |
| `+0x44` | — | `m_streamName` |

**Composition (not single inheritance):** `CDSStreamStorage_CreateFilterSafeStream@0x00434760` allocates **two** objects — `OperatorNew(0x38)` filter + `OperatorNew(0x48)` safe stream — and passes `&filter->pVftable_IDSStream` into `CDSSafeStream_ctor`. Safe stream **wraps** the filter’s `IDSStream` face; it does **not** embed filter cursor fields.

| Address | Symbol | Role |
|---------|--------|------|
| `0x00434760` | `CDSStreamStorage_CreateFilterSafeStream` | Dual alloc; `CDSSafeStream_ctor(safe, &filter->pVftable_IDSStream)` |
| `0x00434160` | `CBulanci::FUN_00434160` | Game path: `OperatorNew(0x48)` + ctor with stream iface arg |
| `0x00430ca0` | `CDSFilterStream_BindSource` | Window/cursor @ filter `+0x18..+0x30` |
| `0x00433ab0` | `CDSSafeStream_ctor` | Chain + lock + name; no `BindSource` |

`get_struct_layout` (R4): `CDSFilterStream` **56 B**; `CDSSafeStream` **72 B** — field names at decimal offsets 24/36 = hex `+0x18` / `+0x24` for embedded chain.

## Ghidra deltas

- `set_decompiler_comment` @ `0x00433b19` (auxHeap teardown-only, R4 todo40)
- `set_decompiler_comment` @ `0x00430dd0` (MI body divergence @ `+0x18`, R4 todo40)
- Label `FUN_0042f980` → **`CDSChained_PrependChild`** (ReleaseAuxHeap + count bump + `InsertListNode`; was FUN in R3 tables)
- `save_program bulanci.exe`

## Struct doc updates

- [CDSSafeStream.md](./CDSSafeStream.md) — MI comparison table; `pAuxHeap` UNK closed (teardown-only, build-wide)
- [CDSFilterStream.md](./CDSFilterStream.md) — cross-ref safe-stream composition @ `CreateFilterSafeStream`
- [CDSChain.md](./CDSChain.md) — `CDSChained_PrependChild` xref name

## Remaining UNK

- `CDSSafeStream_Write` unsupported — no write buffer fields (unchanged).
- `FUN_0042f9d0` / `FUN_0042fa20` / `FUN_0042fa50` — still FUN_*; out of scope unless chain splice batch resumes.
- `RegisterThreadSlice` decompiler may still alias `CDSSafeStreamInfo*` at flag store (`+0x08`); plate/PRE from R3 stand.
