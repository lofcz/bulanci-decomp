# Round 5 — worker 30/50 report

## Task

| Field | Value |
|-------|-------|
| **worker** | 30 / 50 |
| **round** | 5 |
| **title** | CDSObject image embed tail `+0x50` band and remaining UNKs |
| **types** | `CDSObject`, `CDSImage` |
| **addresses** | `0x00425620`, `0x00425670`, `0x0042568a`, `0x004322f0`, `0x004256d8` |
| **prior** | [round4_task_01_report.md](./round4_task_01_report.md), [round3_task_01_report.md](./round3_task_01_report.md) |

## Status

**DONE** — `+0x50` proven as outer `nImageRefcount` (not opaque tag); full `+0x48..+0x5c` embed-vs-standalone alias table in `CDSObject.md`; Ghidra renamed + commented; `save_program`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Ctor init `+0x50 := 1` | `CDSObject_CtorWithImage@0x00425670` | Asm `MOV dword ptr [ESI+0x50],0x1` after `CDSImage_ctor` on `EDI=ESI+4` |
| MI patch order | `0x00425670`–`0x00425698` | `[+0x50]=1` → `[+0x5c]=0` → outer/scheduler vtables → `[+0x4c]` primary → `[+0x54]` IDSChained → `[+0x58]` streamHost |
| Release refcount consumer | `CDSImage_ReleaseRefcount@0x004322f0` | `[ECX+0x50]` compare/dec; delete path `(**(code**)(*(ECX+0x4c)+4))()` |
| Embed `this` for release | `CDSImage_ReleaseRefcount_thunk_Sub58@0x00432320` | `SUB ECX,0x58; JMP ReleaseRefcount` — streamHost `@+0x58` → `CDSObject*` base |
| Dtor stash facet | `CDSObject_dtor@0x004256c0` | `IDSChainedTail_ClearSubObjStash(param_1+0x16)` → `+0x58` |
| Ghidra struct | MCP | `nImageRefcount` `int` @ 80 (`+0x50`); 96 B unchanged |
| Standalone contrast | `CDSImage_InitDefaults@0x00425580` | Inner `nRefcount=1` @ `CDSImage+0x50`; embed ctor overwrites host `+0x54` with IDSChained vtable instead |

### Decompile after R5 (ctor tail)

```c
this->nImageRefcount = 1;
this->pImage_tail_5c = NULL;
/* … vtable patches … */
this->pEmbeddedImage_vf_primary = CDSImage::vftable;
this->pImage_vf_IDSChained = CDSImage::g_pCDSImage_vftable_IDSChained;
this->pImage_vf_streamHost = CDSImage::vftable;
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CDSObject.dwImageField_50` | → `nImageRefcount` (`int` @ `+0x50`) |
| `set_decompiler_comment` | `0x00425670`, `0x004322f0` | Outer vs inner refcount / thunk `-0x58` |
| `force_decompile` | `0x00425620`, `0x004322f0` | Ctor uses `nImageRefcount`; release comments |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSObject.md](./CDSObject.md) — embed tail band table `+0x48..+0x5c`; variant overlay rows; UNK cleared for `+0x50`; R5 Ghidra apply note.

## Remaining UNK

- `pEmbeddedImage_vf_primary` left as `void *` (dispatch target proven; facet struct optional).
- Scalar-deleting dtor / full MI vtable slot map — `CDSImage.md`, `bmp_decoder.md`.
- Track-manager path: fields `+0x3c..+0x44` in 96 B superset still alias TM semantics on non-image allocs (`0x48` only).
