# Struct recovery batch 38 follow-up (round 2)

**Prior:** `batch_38_summary.md`  
**Status:** **FOLLOWUP_COMPLETE** (Ghidra save attempted; see note below)

## Actions taken

### Function naming (batch 38 follow-up items)

| Address | Ghidra name | Role |
|---------|-------------|------|
| `0x00446c30` | `CDSSafeStream_ClearThreadSlices` | CS-guarded drain of embedded `CDSChain` at `+0x18` (`FUN_0042fab0`) + `m_streamName` clear |
| `0x00446d90` | `CDSSafeStream_GetThreadSlice` | `GetCurrentThreadId` match / lazy `CDSSafeStreamInfo` alloc; used by `Read`/`Seek`/`Tell`/`GetSize` |
| `0x00446ea0` | `CDSSafeStream_RegisterThreadSlice` | Ctor path: clear slices, copy resource name, append first per-thread node, set `dwM_streamFlags=1` |

Prototypes updated to take `CDSSafeStream *` where applicable (`RegisterThreadSlice`, `ClearThreadSlices`, `GetThreadSlice`).

### `CDSSafeStream+0x28` resolved

Prior batch placed `nM_chain_count` at `+0x24` and left `+0x28` as UNK. Embedded `CDSChain` at `+0x18` uses **`+0x10` relative to chain base** for the child count:

- `FUN_0042f9b0@0x0042f9e1` — `*(this+0x10) += 1` with `this = parent+0x18` → **`CDSSafeStream+0x28`**
- `FUN_0042fab0` — drain loop reads the same count field

Ghidra struct fields renamed:

- `+0x24` → `m_chain_auxHeap` (`CDSChain+0x0c`, freed by `FUN_0042f800`)
- `+0x28` → `nM_chain_count` (`CDSChain+0x10`)

### `dwM_streamFlags` (`+0x08`)

- Ctor: `0x20` (`CDSSafeStream_InitBase`)
- `CDSSafeStream_RegisterThreadSlice@0x00446f60`: `MOV dword ptr [EDI+0x8], 1` after first thread node is chained

### Docs updated

- `CDSSafeStream.md` — layout table, UNK, references
- `CDSSafeStreamInfo.md` — xref names to renamed helpers

## Ghidra deltas

- `modify_struct_field` on `CDSSafeStream`: `m_chain_auxHeap`, `nM_chain_count` at correct offsets
- Function renames + prototypes at `0x446c30`, `0x446d90`, `0x446ea0`
- `get_struct_layout CDSSafeStream` → still **72 bytes (0x48)**

## Remaining UNK

- `m_chain_auxHeap` — only `Runtime_Free` on teardown path; allocator not traced in this scope
- Decompiler still types `RegisterThreadSlice` as `CDSSafeStreamInfo *this` (ECX retype API limitation); true object is `CDSSafeStream *`

## Agent todo 40 (2026-05-30) — VERIFIED

- **`dwM_streamFlags`:** write-only — writers @ `0x00433ad9`, `0x00446d33`, `0x00446f60`, `CDSSafeStream_Close`; zero `CMP`/`TEST` on `+0x08` in `CDSSafeStream_*` methods (`search_instructions`).
- **`m_chain_auxHeap`:** `CDSChain_ReleaseAuxHeap@0x0042f800` (renamed); sole `MOV [ESI+0x24]` is ctor zero @ `0x00433b19`; no allocator xref program-wide.
- **Ghidra:** plate @ `0x446ea0`/`0x446d90`; decompiler comment @ `0x0042f800`; `save_program bulanci.exe` **OK** (r3-worker-40).

## Save

`save_program bulanci.exe` returned **“Unable to lock due to active transaction”** on slice 38 follow-up; **agent todo 40** save succeeded (2026-05-30).
