# Round 5 — worker 41/50 report

## Task

| Field | Value |
|-------|-------|
| **worker** | 41 / 50 |
| **mode** | WRITE |
| **title** | A–C struct_recovery scan: prove or BLOCK remaining `UNK` / `field_*` |
| **slice rule** | Struct names `^[ABC][A-Za-z0-9_]*$` with `UNK` or `field_*` in deliverable; assign worker **N** items at sorted index `i` where `i % 50 == N - 1` |
| **this worker** | `CDSMpxStream`, `CSessionList` (indices 40, 90 of 100 flagged structs) |
| **prior** | [round4_task_38_report.md](./round4_task_38_report.md), [round5_worker_29_report.md](./round5_worker_29_report.md), [CSessionList.md](./CSessionList.md) |

### Scan summary (A–C band)

| Metric | Count |
|--------|------:|
| Struct deliverables `^[ABC]…` (excl. `batch_*` / `round*` / `pass_*`) | 103 |
| With `UNK` section or `field_*` tokens | 100 |
| Assigned to worker 41 (`index % 50 == 40`) | 2 |

Other workers own the remaining 98 flagged A–C structs under the same modulo rule.

## Status

**DONE** — one allocation-split **proven**, one dialog tail **proven**, three items **BLOCKED** with negative or inherited-class evidence; Ghidra comments + `save_program`.

## Evidence

### CDSMpxStream

| Claim | Status | Function @ address | Evidence |
|-------|--------|-------------------|----------|
| Registry shell ≠ decode body | **PROVEN** | `CDSMpxStream_CreateObject@0x00433110` | `OperatorNew(0x48)`; MI vtables only; `pSubObjStash` zero @ `0x0043312d` |
| Decode instance is separate alloc | **PROVEN** | `CDSMpx::CreateFromHandle@0x00446b00` | `OperatorNew(0x98c8)`; reads `ctx+0x1c..0x2c` from persist object at `ctx@P+4`; returns new `CDSMpx*` |
| Dtor targets decode primary | **PROVEN** | `CDSMpxStream_dtor@0x004330b0` | `IDSChainedTail_ClearSubObjStash(P+0x3c)` then `CDSMpx_dtor(P)` on full layout |
| `pSubObjStash` payload type | **BLOCKED** | `CDSMpxStream_CreateObject@0x0043312d` | Only store to `+0x40` in factory is **NULL**; no non-null writer on MPx path in binary |
| WAVEFORMAT sub-byte in `mpxFormatTail` | **BLOCKED** | `SaveMpxFile` / `LoadMpxFile` | 12-byte persist tail vs `mad_stream` overlay documented (R5 w29); no xref isolates `WAVEFORMATEX` bit fields |

### CSessionList

| Claim | Status | Function @ address | Evidence |
|-------|--------|-------------------|----------|
| Cancel button not stored on `this` | **PROVEN** | `CSessionList_BuildDialog@0x0040c2d0` | Join → `this->pJoinButton`; cancel built then `CDSView__AddChild` only |
| `win.+0x08..+0x64` not session-specific | **BLOCKED** | All `CSessionList_*` | Decompile uses `win` bbox/flags + `@+0x70..+0x78` children only; chain band semantics = [CWindow.md](./CWindow.md) |

## Ghidra deltas

- `set_decompiler_comment` @ `0x00433110` — registry `0x48` vs decode `0x98c8`
- `set_decompiler_comment` @ `0x00446b00` — fresh decoder alloc; persist read from sibling object
- `set_decompiler_comment` @ `0x0040c2d0` — inherited `CWindow` chain band
- `save_program bulanci.exe`

## Struct doc updates

- [CDSMpxStream.md](./CDSMpxStream.md) — UNK → PROVEN/BLOCKED; R5 apply note
- [CSessionList.md](./CSessionList.md) — UNK → PROVEN/BLOCKED; R5 apply note

## Remaining UNK (this slice only)

- `mpxFormatTail` WAVEFORMAT bit-level alias during decode overlay (**BLOCKED**).
- `stashFacet.pSubObjStash` concrete type on MPx path (**BLOCKED** — never populated).
- `CSessionList` / `win` chain-band dword semantics (**BLOCKED** — resolve via `CWindow` / `CDSChained`, not session dialog).

## Note vs manifest task 41

Round-5 **todo id 41** in [agent_todos_50_r5.json](../agent_todos_50_r5.json) is **CBulanek** prefix band (separate worker stream). This file is the **alphabet-scan worker 41/50** deliverable (`round5_worker_41_*`).
