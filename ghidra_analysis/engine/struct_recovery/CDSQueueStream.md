# CDSQueueStream

## Status

**PARTIAL** — `IDSStream` ring-buffer view over an external `CDSMemQueue` at primary **`+0x14`**. Nine dword fields through `+0x20`; stream I/O uses **`+0x14`..`+0x1c`** on the primary base (`+0x10` is **`nRefcount`** for `IDSReferenced` / `ReleaseRefcount`). `sizeof` **0x24** in Ghidra. DSM handles embed a **`CDSMemQueue`** at `handle+0x0c` (**0x20** bytes), not a full `CDSQueueStream`; `chunkCounter` is a separate handle field at `+0x2c`.

### IDSStream mdisp `+0x04` (decompiler pitfall)

`ReadBytes` / `WriteBytes` / `SeekPosition` / `SetStreamSize` / `GetSize` live on the **`IDSStream`** vtable at outer **`+0x04`**. Several bodies are typed with `this` at **`outer+4`**, so pseudocode may reference `this->nRefcount` for a **queue** null-guard even though **`nRefcount` on the primary object is at `+0x10`** (`CDSQueueStream_ReleaseRefcount@0x00433c60`, `CreateObject` stores `1` there). Prefer disasm operands **`*(outer+0x14)`** (queue), **`*(outer+0x18)`** (logical size), **`*(outer+0x1c)`** (cursor) and the `this != 4` guard idiom over misleading local field names.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Fields through `+0x1c` | `0x0043c000` | `ReadBytes` / `WriteBytes`: `CDSMemQueue*` at `+0x10`, `dwRingOrigin` at `+0x14`, `dwLogicalSize` at `+0x18`, `dwCursor` at `+0x1c` |
| Tail dword `+0x20` | `0x004289a0` | `InitializeQueueStreamView`: `headerStructA[6] = 0` ⇒ `CDSQueueStream+0x20`; no Read/Seek/Set consumer |
| `sizeof == 0x24` | Ghidra | `get_struct_layout` → 36 bytes (9 fields) |
| DSM handle embed `0x20` | `0x004290c0` | `HandleAcquireResource`: `CDSMemQueue_InitDefault(handle+0x0c)`; `chunkCounter` at `handle+0x2c` (not `CDSQueueStream+0x20`) |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_IDSReferenced` | `CDSQueueStream_dtor@0x00428a50` |
| 0x04 | 4 | `void *` | `pVftable_IDSStream` | `CDSQueueStream_dtor@0x00428a50`; `IDSStream` mdisp `+4` (`stream_hierarchy.md`) |
| 0x08 | 4 | `uint` | `dwStreamState` | `CloseStream@0x00428960` writes `0x20`; `InitializeQueueStreamView@0x004289a0` writes `0x20` on view init |
| 0x0c | 4 | `void *` | `pChainedParent` | `ReleaseRefcount@0x00433c60` via `*(this+0xc)` |
| 0x10 | 4 | `int` | `nRefcount` | `ReleaseRefcount@0x00433c60` tests `*(this+0x10)`; `ReadBytes@0x0043c000` null-guard (decompiler: queue `*` at outer `+0x10` when `this` is `IDSStream+4`) |
| 0x14 | 4 | `CDSMemQueue *` | `pMemQueue` | `ReadBytes@0x0043c000` null-guard / ring via `queue+4/+0xc/+0x1c`; `CDSQueueStream_DetachQueue@0x0043be90` unlock+null |
| 0x18 | 4 | `uint` | `dwLogicalSize` | `ReadBytes` bound; `SetStreamSize@0x0043bf10` writes size; `SeekPosition@0x0043bf80` end-relative |
| 0x1c | 4 | `uint` | `dwCursor` | `ReadBytes` / `SeekPosition` / `SetStreamSize` |
| 0x20 | 4 | `uint` | `dwReserved` | `InitializeQueueStreamView@0x004289a0` zeros via `headerStructA[6]`; **no** Read/Write/Seek/Set/GetSize xref |

### `CDSDsmFile` handle embed (`CDSDsmFile_Handle`, 0x30)

| Handle offset | Role |
|---------------|------|
| `+0x0c` | `CDSMemQueue` (**0x20** bytes) — `CDSMemQueue_InitDefault@0x004290c0` |
| `+0x2c` | `chunkCounter` — `HandleAcquireResource` / `HandleRecordRead@0x00428c40` spin-wait; **not** the same as standalone `CDSQueueStream+0x20` |

## Function leaf map

| Symbol | Address | Role |
|--------|---------|------|
| `CDSQueueStream_CreateObject` | `0x0043c160` | Registry factory class **0x4b**; `OperatorNew(0x24)` |
| `CDSQueueStream_StaticClassRegister` | `0x0047daa0` | `HandleClassRegister` entry |
| `CDSQueueStream_dtor` | `0x00428a50` | Restore vtables; `CDSQueueStream_DetachQueue` |
| `CDSQueueStream_DetachQueue` | `0x0043be90` | Unlock/null `pMemQueue@+0x14` |
| `CDSQueueStream::ReadBytes` | `0x0043c000` | Ring read; IDSStream slot |
| `CDSQueueStream::WriteBytes` | `0x0043c0b0` | Ring write |
| `CDSQueueStream::SetStreamSize` | `0x0043bf10` | Logical size `@+0x18` |
| `CDSQueueStream::SeekPosition` | `0x0043bf80` | Cursor `@+0x1c` |
| `CDSQueueStream::GetSize` | `0x0043beb0` | Null-queue guard only |
| `CDSQueueStream_ReleaseRefcount` | `0x00433c60` | Primary `nRefcount@+0x10` |
| `CDSDsmFile::InitializeQueueStreamView` | `0x004289a0` | Stack `0x24` view; zeros `+0x20` |
| `CDSDsmFile::InitializeSourceRegion` | `0x0043be50` | Binds queue; sets view size/cursor |

## Ghidra apply

```
get_struct_layout CDSQueueStream → size 36
modify_struct_field pMemQueue → CDSMemQueue *
rename_function 0x0043be90 → CDSQueueStream_DetachQueue
set_function_prototype CDSQueueStream_CreateObject → CDSQueueStream *
save_program bulanci.exe
```

## UNK

- `InitializeQueueStreamView@0x004289a0` / `InitializeSourceRegion@0x0043be50` full `CDSDsmFile` header overlay vs standalone layout.
- `CDSMemQueue` companion layout (`stream_hierarchy.md`).

## Follow-up

- Round 3 task 18 (`round3_task_18_report.md`): **`dwReserved` @ `+0x20`** — init-only tail, not handle `chunkCounter`.
