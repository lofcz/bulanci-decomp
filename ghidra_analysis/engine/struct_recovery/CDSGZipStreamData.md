# CDSGZipStreamData

## Status

**VERIFIED** — instance size `0x20` (32 bytes). Refcounted chunk-index sidecar for `CDSGZipStream`; allocated only via `OperatorNewWithBadAlloc(0x20)` in `CDSGZipStream__Open`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSGZipStreamData) == 0x20` | `CDSGZipStream__Open` @ `0x004356e0` | `OperatorNewWithBadAlloc(0x20)`; last field `m_chunks` at `+0x1c` (4 bytes) → object ends at `0x20` |
| `m_chunks` freed in dtor | `CDSGZipStreamData_dtor` @ `0x00434fc0` | `Runtime_Free` on `param_1[7]` → offset `+0x1c` |
| `m_lSize` read/written at `+0x10` | `CDSGZipStream__Open` @ `0x004356e0` | `Read` into `*(iVar1 + 0x10)`; writer path stores `0` at `+0x10`/`+0x14` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_primary` | `CDSGZipStream__Open@0x004356e0` (`*puVar3 = 0x4875b4`); `CDSGZipStreamData_dtor@0x00434fc0` restore |
| `0x04` | 4 | `void *` | `vf_secondary` | `CDSGZipStream__Open@0x004356e0` (`puVar3[1] = 0x4875a0`); same dtor |
| `0x08` | 4 | `int` | `refcount` | `CDSGZipStream__Open@0x004356e0` (`puVar3[2] = 1`) |
| `0x0c` | 4 | `int` | `reserved_0c` | *(not written in Open ctor path — treat as padding until proven)* |
| `0x10` | 8 | `longlong` | `m_lSize` | `CDSGZipStream__Open@0x004356e0` (`Read`/`__alldiv` on `+0x10`); writer init `0` at `+0x10`/`+0x14` |
| `0x18` | 4 | `int` | `m_nChunkCount` | `CDSGZipStream__Open@0x004356e0` (`*(iVar1 + 0x18) = …`); `puStack_8 = count * 8` for index bytes |
| `0x1c` | 4 | `void *` | `m_chunks` | `CDSGZipStream__Open@0x004356e0` (`*(iVar1 + 0x1c) = pvVar5`); `CDSGZipStreamData_dtor@0x00434fc0` free |

## Ghidra apply

```
create_struct CDSGZipStreamData fields=[...]  # applied — 32 B
get_struct_layout CDSGZipStreamData → Size: 32
set_function_prototype CDSGZipStreamData_dtor@0x00434fc0 → void __fastcall CDSGZipStreamData_dtor(CDSGZipStreamData *this)
set_plate_comment CDSGZipStream__Open@0x004356e0 — this=CDSGZipStream*, m_pData→CDSGZipStreamData*
save_program bulanci.exe
```

**Decompiler (batch 31 re-run):** `CDSGZipStreamData_dtor` uses `this->pM_chunks`, `this->pVf_primary`. `CDSGZipStream__Open` still types `this` as `CBulanci*` (Ghidra `__thiscall` ECX limitation); field access on `m_pData` uses raw `+0x10`/`+0x18`/`+0x1c` offsets matching this layout.

## UNK

- `+0x0c..+0x0f` not touched in the `Open` allocation path (may be padding or filled elsewhere).
- Exact semantic names of the two vtable slots beyond “`CDSChained` house pattern” (`gzip_stream.md`).
