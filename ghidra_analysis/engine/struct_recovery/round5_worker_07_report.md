# Round 5 worker 07 — FUN_* rename (CDSObject / ODSImage / bitmap / view band)

## Task

| Field | Value |
|-------|-------|
| **Worker** | 7 / 50 |
| **Mode** | WRITE |
| **Address slice** | `0x00430320` .. `0x00430E60` (band `0x0042C000`..`0x00450000`, 50-way split) |
| **Type cluster** | `CDSObject`, `ODSImage`, bitmap (`CDSImage` / `CDSBmpImage` / `CDSJpegImage`), `CDSView` |
| **Scope rule** | Rename `FUN_*` only with xref / vtable / IJG slot / decompile-body proof |

## Status

**DONE** — All six `FUN_*` in this slice are JPEG memory-manager or `CDSJpegImage` IDSChained stubs; six renames applied in Ghidra, `save_program bulanci.exe`.

No `FUN_*` in-slice for `CDSView`, `ODSImage`, or `CDSObject` (those types already named or live outside this slice).

## Evidence

| Old symbol | New symbol | Address | Proof |
|------------|------------|---------|-------|
| `FUN_00431ae8` | `CDSJpegMemPool_InitMethodPointers` | `0x00431ae8` | Sole callee from `jinit_memory_mgr@0x00431a70` after `OperatorNew(0x48)`; writes `jpeg_memory_mgr` slots: `[2]=alloc_sarray`, `[3]=alloc_barray`, `[6]=jpeg_realize_virt_arrays`, `[9]=free_pool` — matches `ref/libjpeg6b/jmemmgr.c` `jinit_memory_mgr` fill order |
| `FUN_00431890` | `CDSJpegMemPool_alloc_sarray` | `0x00431890` | Installed at vtable index 2 above; body: two `CDSJpegMemPool_BumpAlloc` calls, row pointer stride `param_3`, `param_4` rows — IJG `alloc_sarray` |
| `FUN_004318f0` | `CDSJpegMemPool_alloc_barray` | `0x004318f0` | Vtable index 3; row stride `param_3 * 0x80` (DCT block size) — IJG `alloc_barray` |
| `FUN_004315d0` | `CDSJpegMemPool_free_pool` | `0x004315d0` | Vtable index 9; frees singly-linked chunks at `cinfo->mem+0x34+pool_id*4`, `Runtime_Free(&DAT_004b7c94,…)` — IJG `free_pool` |
| `FUN_00431851` | `CDSJpegMemPool_LinkNewChunk` | `0x00431851` | Tail call from `CDSJpegMemPool_BumpAlloc@0x00431790` after `Runtime_MallocOrThrow`; links `*piVar4` into pool slot array, bumps `pool+0x44` free-space counter |
| `FUN_00431d60` | `CDSJpegImage_AlwaysReturnsOne` | `0x00431d60` | Data xref `CDSJpegImage` IDSChained vftable `0x00487194` slot **[4]**; decompile `return 1`; peer image types use shared `AlwaysReturnsZero@0x00409480` at same slot (`master_vtable_catalog.csv`, `CDSBmpImage@0x004871f0`) |

### Slice inventory (FUN_* only)

| Address | Disposition |
|---------|-------------|
| `0x004315d0` | Renamed → `CDSJpegMemPool_free_pool` |
| `0x00431851` | Renamed → `CDSJpegMemPool_LinkNewChunk` |
| `0x00431890` | Renamed → `CDSJpegMemPool_alloc_sarray` |
| `0x004318f0` | Renamed → `CDSJpegMemPool_alloc_barray` |
| `0x00431ae8` | Renamed → `CDSJpegMemPool_InitMethodPointers` |
| `0x00431d60` | Renamed → `CDSJpegImage_AlwaysReturnsOne` |

Band totals (full `0x0042C000`..`0x00450000`): **118** `FUN_*`, **1058** already named (Ghidra `list_functions` script, 2026-05-30).

## Ghidra deltas

- `rename_function_by_address` × 6 (addresses above)
- `set_function_prototype` `void __cdecl CDSJpegMemPool_free_pool(void * cinfo, int pool_id)` @ `0x004315d0`
- `set_decompiler_comment` @ `0x00431ae8`, `0x00431d60`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSJpegImage.md](./CDSJpegImage.md) — R5 worker 07 note on `CDSJpegImage_AlwaysReturnsOne` @ slot 4

## Remaining UNK (this slice)

- `CDSJpegMemPool_LinkNewChunk@0x00431851` — compiler-split tail; no standalone COFF symbol; stdcall register glue not re-prototyped
- IJG helpers still `LAB_00431880`, `LAB_00431950`, etc. — out of worker slice or already labeled; not renamed without per-body proof
- `JSAMPARRAY` / `j_common_ptr` Ghidra types absent — `alloc_sarray` / `alloc_barray` left default return `int`

## Out of scope (slice empty)

| Type | Note |
|------|------|
| `CDSView` | Named methods in band (`CDSView_DispatchEvent@0x0042c040`, etc.) — no `FUN_*` in `0x00430320`..`0x00430E60` |
| `ODSImage` | Methods @ `0x00439050` / `0x00439100` — below slice |
| `CDSObject` | Track-manager / ctor cluster @ `0x00439c70+` — above slice |
