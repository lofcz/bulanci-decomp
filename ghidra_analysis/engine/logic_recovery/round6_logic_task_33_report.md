# Round 6 logic — Task 33 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 33 |
| **title** | Logic sim_429_436: 0x00433df0–0x00434440 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — Ghidra MCP (`user-ghidra-mcp`) returned `Not connected` / `Connection closed` for the entire session (no live `decompile`, `disassemble`, `get_xrefs_to`, or `save_program`). Per-function logic below is sourced from **prior Ghidra-verified** struct rounds (R3 tasks 45–47, R4–R5 workers 8/37, batch 29/37/40) plus `master_vtable_catalog.csv`, `vftable_methods.csv`, and `config/bulanci/mapping.csv`. No Frida: pack-storage lifecycle, resource-key I/O, and exception paths are statically closed in existing docs.

## 3. Functions

Slice is the **`.eap` / pack stream container** cluster: `CDSStreamStorage` init/teardown, **IDSStorage** vtable slots (+4/+5 open/index), per-thread loader splice, `CDSResInfo` / `CDSStrmResInfo` key I/O, ClassID **94** sign-record stream helpers, and `CDSResourceException` construction.

### 3.1 `CDSStreamStorage` object lifecycle

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00433df0` | `CDSStreamStorage_InitObjectFields` | Shared **0x60 B** init (no root safe-stream): triple vtables; embedded `CDSCollection` @ `+0x1c` (growth chunk `0x20`); `CDSChain` @ `+0x34`; `InitializeCriticalSection` @ `+0x48` | R5 worker 8 disasm @ `0x00433df5` / `0x00433e3a`; [CDSStreamStorage.md](../struct_recovery/CDSStreamStorage.md) |
| `0x00433e60` | `CDSStreamStorage_dtor` | `__fastcall` teardown: drain embedded chain/collection; release `pRootSafeStream` @ `+0x18`; `DeleteCriticalSection` @ `+0x48`; restore vtables | [CDSStreamStorage.md](../struct_recovery/CDSStreamStorage.md) @ `0x00433e60`; batch 29/40 |
| `0x004340f0` | `CDSStreamStorage_CreateObject` | IDS factory: `PUSH 0x60` → `OperatorNewWithBadAlloc` → `JMP InitObjectFields` | R5 worker 8 @ `0x004340f2`; DATA xref `0x0047d010` |
| `0x00434160` | `CDSStreamStorage_InitRootSafeStream` | In-place ctor path only: `OperatorNew(0x48)` `CDSSafeStream`; store **IDSEventHandler facet** @ `+0x18`; `CDSSafeStream_Tell` → `dwStreamBaseOffsetLo/Hi` @ `+0x10` | R3/R4 task 46; [CDSStreamStorage.md](../struct_recovery/CDSStreamStorage.md) |
| `0x0043420c` | `Catch@0043420c` | MSVC **SEH** landing pad (20 B) adjacent to `InitRootSafeStream` failure path | `mapping.csv` `Catch@0043420c`; not game logic |

### 3.2 Loader chain + stream table (out-of-slice callee `CreateFilterSafeStream` @ `0x00434760`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00433f00` | `CDSStreamStorage_GetThreadLoaderNode` | Walk `this->chain` children; match `GetCurrentThreadId` to node `+0x10`; return per-thread loader node | R3 task 46; [CDSStreamStorage.md](../struct_recovery/CDSStreamStorage.md) |
| `0x00433f70` | `CDSStreamStorage_AppendOrReuseStream` | `EnterCriticalSection` @ `+0x48`; if `entry->streamExtent==0` → append path reads `entry->dwFilterSliceAddend` @ `+0x20` → `CreateFilterSafeStream`; else **`AddRefHeldObject`** | R3 task 47 asm @ `0x00433fad` / `0x0043405b`; [CDSStrmResInfo.md](../struct_recovery/CDSStrmResInfo.md) |
| `0x00434380` | `CDSStreamStorage_AddRefHeldObject` | **`__fastcall`** helper on **reuse** branch when `streamExtent != 0` (22 B); bumps held stream entry refcount | R3 task 47 branch; `mapping.csv`; callee from `AppendOrReuseStream` only (static) |
| `0x004340c0` | `CDSStrmStgLoadingInfo_CreateObject` | Factory for **`CDSStrmStgLoadingInfo`** (stream-loading metadata node on storage chain); vtables `0x00487484` / `0x004874a0` | [CDSFlxFile.md](../struct_recovery/CDSFlxFile.md) rename; `vftable_methods.csv` |

### 3.3 `IDSStorage` face (`g_pCDSStreamStorage_vftable_IDSStorage` @ `0x0047f7a4`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00434110` | `CDSStreamStorage_OpenStream` | Vtable **slot +4**: `__thiscall` `uchar (uint, void*)` — dispatches **resource open** on a keyed/windowed stream (e.g. `CDSDsmFile::HandleOpenStream` via resource `IDSStorage` face) | `vftable_methods.csv` slot 4; `mapping.csv`; [dsm_file_format.md](../../formats/dsm_file_format.md) `IDSStorage::Open` xref from `CreateFilterSafeStream` |
| `0x00434140` | `CDSStreamStorage_GetStreamByIndex` | Vtable **slot +5**: `__thiscall` `uchar (int index, void* out)` — indexed stream lookup on container | `vftable_methods.csv` slot 5; `mapping.csv` |
| *(related, prior slice)* | `GetStreamCount` / `GetStreamEntry` @ `0x004339c0` / `0x004339d0` | Slots +6/+7: returns `collection.m_items` ptr vs dead stub indexing `m_pVtable_IDSChained` | R3 task 46 |

**Note:** `report.json` / `vftable_methods.csv` still label `0x00434110` / `0x00434140` as `FUN_*` in some exports; manifest and `mapping.csv` use `OpenStream` / `GetStreamByIndex`.

### 3.4 `CDSResInfo` / `CDSStrmResInfo` key facet (IDSChained `this`)

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00434270` | `CDSResInfo_ReleaseEmbeddedResource` | If `*(this+0x14)` non-null, release via `IDSReferenced` vfn `+8` — **base `CDSResInfo` only** (not `CDSStrmResInfo.streamExtent`) | [CDSResInfo.md](../struct_recovery/CDSResInfo.md); PRE @ `0x00434270` (R3 task 47) |
| `0x00434290` | `CDSResInfo_Load` | `IDSStream::Read` **8 B** into chained facet → `dwResourceId` / `dwClassId` (`+0x08`/`+0x0c` on object) | [CDSResInfo.md](../struct_recovery/CDSResInfo.md); vtable slot 4 `CDSResInfo` |
| `0x004342c0` | `CDSResInfo_Save` | Symmetric **8 B** write of key dwords | [CDSResInfo.md](../struct_recovery/CDSResInfo.md); vtable slot 5 |
| `0x004342f0` | `CDSStrmResInfo_CompareKey` | `__cdecl` `int **, int **`: sort/compare on `dwResourceId` @ entry `+0x08` | R5 worker 37 asm; used by `CloseStreamByKey` / `CDSCollection_FindKeyIndex` |

### 3.5 ClassID 94 sign record + resource errors

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00434310` | `CDSResourceSign_WriteToStream` | Persist `dwPublishDate` @ `+0x14`, `wszContent`/`wszCopyright` @ `+0x18`/`+0x1c`, `bFlagByte` @ `+0x20` | [CDSResourceSign.md](../struct_recovery/CDSResourceSign.md); [sign_record.md](../../formats/sign_record.md) |
| `0x004343a0` | `CDSResourceSign_ReadFromStream` | Deserialize sign record from pack stream | [CDSResourceSign.md](../struct_recovery/CDSResourceSign.md) |
| `0x00434350` | `CDSResourceException_What` | Lazy-format message into `pszFormatted` @ `+0x40` via `CDsStringFormatV` | [CDSResourceException.md](../struct_recovery/CDSResourceException.md) |
| `0x00434400` | `CDSResourceException_ctor` | `InitFields(6,8,1)`; vtable `0x4874b0`; store `dwResourceId` @ `+0x3C` | [CDSResourceException.md](../struct_recovery/CDSResourceException.md) |
| `0x00434430` | `CDSResourceException_GetClassTable` | Returns `&DAT_004b7f14` (class metadata) | [CDSResourceException.md](../struct_recovery/CDSResourceException.md); `ghidra_xrefs.jsonl` @ `0x00434430` |
| `0x00434440` | `CDSResourceException_dtor` | `CDsStringReleaseHeader` on `pszFormatted` when non-null | [CDSResourceException.md](../struct_recovery/CDSResourceException.md) |

### 3.6 Shared chained-tail helper

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00434250` | `IDSChainedTail_ClearSubObjStash` | `stash = *(facet+4)`; if non-null zero `stash+0x10/+0x14`, null facet+4 | R4 task 12; 18 xrefs (script/MPx/jpeg dtors); not a writer |

### Control-flow summary (pack storage)

```text
OpenPackStream / factory:
  CreateObject (0x60) → InitObjectFields
  CBulanci ctor path: CDSStreamStorage_ctor @ 0x401790 → InitRootSafeStream (+ Catch@43420c on failure)

Load resource into table:
  GetThreadLoaderNode (per-thread chain node)
  AppendOrReuseStream (CS @ +0x48):
    streamExtent==0 → CreateFilterSafeStream (uses filterSliceAddend @ entry+0x20)
    else → AddRefHeldObject
  IDSStorage vtable consumers:
    OpenStream (+4) → resource class HandleOpenStream (e.g. CDSDsmFile)
    GetStreamByIndex (+5) → fetch stream by slot

Persisted keys (CDSStrmResInfo / CDSResInfo):
  Load/Save (+8/+0xc dwords) + CompareKey for collection lookup
  CloseStreamByKey (prior slice) → FindKeyIndex + CompareKey → may throw CDSResourceException

Sign metadata (class 94):
  WriteToStream / ReadFromStream on CDSResourceSign
```

## 4. Ghidra deltas

**None applied** — MCP unavailable (`Not connected`).

**Already applied in prior rounds (not re-run):**

- `CDSStreamStorage` / `CDSStrmResInfo` / `CDSResInfo` / `CDSResourceSign` / `CDSResourceException` structs and field names (batches 29/37/40, R3 tasks 45–47, R5 worker 8)
- Renames: `InitObjectFields`, `CreateObject`, `GetThreadLoaderNode`, `AppendOrReuseStream`, `InitRootSafeStream`, `CDSStrmResInfo_CompareKey`, `CDSStrmStgLoadingInfo_CreateObject`, sign/exception symbols
- `set_function_this_type` `CDSStreamStorage*` @ `InitRootSafeStream@0x00434160` (R3 task 46)
- PRE comments on `AppendOrReuseStream` / serialize paths / `GetStreamCount` stubs

**Queued when MCP returns (cosmetic — static proof exists):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `0x00434110`, `0x00434140`, `0x00434380` | Align with `mapping.csv` if Ghidra still shows `FUN_*` |
| `set_function_prototype` | `OpenStream`, `GetStreamByIndex`, `AddRefHeldObject` | Match `mapping.csv` `__thiscall` / `__fastcall` |
| `force_decompile` | `0x00434110`, `0x00434140`, `0x00434380` | Close UNK on internal branch logic (not yet disassembled in R6) |

## 5. Frida

**none** — Container layout (`0x60`), loader splice (`AppendOrReuseStream` / `filterSliceAddend`), collection key I/O, sign-record fields, and exception ctor are statically proven in struct-recovery rounds. Runtime would only re-confirm `IDSStorage::Open` dispatch already documented in [dsm_file_format.md](../../formats/dsm_file_format.md).

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| Live Ghidra re-verify of this slice | MCP down; could not refresh decompile/disasm/xrefs in R6 session |
| `OpenStream` / `GetStreamByIndex` internal bodies | 33 B / 24 B functions; vtable slot + prototype known; **branch logic not disassembled** in this session |
| `AddRefHeldObject` refcount target | Caller proven (`AppendOrReuseStream` reuse path); held-object field offset not re-disassembled here |
| `InitRootSafeStream` spurious `CDSCollection_Load` vcall in decompile | R3 task 46 documents as Ghidra artifact |
| `GetStreamEntry` IDSStorage slot +7 | Dead stub (vtable-only) — unchanged |

## Cross-links

- [`CDSStreamStorage.md`](../struct_recovery/CDSStreamStorage.md) — VERIFIED `0x60` layout, loader helpers, IDSStorage stub audit
- [`CDSStrmResInfo.md`](../struct_recovery/CDSStrmResInfo.md) — stream entry tail, `filterSliceAddend`
- [`CDSResInfo.md`](../struct_recovery/CDSResInfo.md) — base key + `pEmbeddedResource` vs derived tail
- [`CDSResourceSign.md`](../struct_recovery/CDSResourceSign.md) — ClassID 94 colophon
- [`CDSResourceException.md`](../struct_recovery/CDSResourceException.md) — throw from `CloseStreamByKey`
- [`stream_hierarchy.md`](../../formats/stream_hierarchy.md) — `IDSStream` vs `IDSStorage` nomenclature
- [`round5_worker_08_report.md`](../struct_recovery/round5_worker_08_report.md) — factory/init split
- [`round3_task_47_report.md`](../struct_recovery/round3_task_47_report.md) — append vs reuse branch
