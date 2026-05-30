# Round 5 — Worker 19 Report

## Task

| Field | Value |
|-------|-------|
| **worker** | 19 / 50 |
| **title** | FUN_* vtable thunks / MI adjustors — document slot targets with proof |
| **mode** | WRITE (evidence-only; no rename without disasm + vtable slot) |
| **program** | `bulanci.exe` via user-ghidra-mcp |
| **slice** | Sorted unique `FUN_*` targets in `vftable_methods.csv` chunk **19/50** → **11** symbols @ `0x0041a510`–`0x0041a620` (`CDSWav` / `CDSWavStream` / shared meta) |

## Status

**DONE** — All **11** catalog `FUN_*` entries in the worker slice are **already named** in Ghidra; disassembly + vtable slot + DATA xref proof recorded; **8** MI adjustor plates + **3** meta-stub plates applied; `save_program bulanci.exe`.

## Evidence (slot targets)

### MI adjustors (`SUB ECX, imm` + `JMP` primary)

| Ghidra symbol | Address | Vtable (class / iface) | Slot | Disasm | Primary target |
|---------------|---------|------------------------|------|--------|----------------|
| `CDSWav_ScalarDeletingDtor_thunk` | `0x0041a530` | `CDSWav` / `IDSEventHandler` `0x48230c` | 3 | `SUB ECX,0x18; JMP 0x41bbe0` | `CDSWav_ScalarDeletingDtor@0x0041bbe0` |
| `CDSWav_ScalarDeletingDtor_thunk_Sub4` | `0x0041a540` | `CDSWav` / `face_8slots` `0x482320` | 3 | `SUB ECX,4; JMP 0x41bbe0` | same |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub4` | `0x0041a5d0` | `CDSWavStream` / `face_8slots` `0x48239c` | 3 | `SUB ECX,4; JMP 0x41bc00` | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` |
| `CDSWav_ReleaseChild_thunk_Sub30` | `0x0041a5e0` | `CDSWavStream` / `IDSChained6` `0x48236c` | 2 | `SUB ECX,0x30; JMP 0x433040` | `CDSWav_ReleaseRefcount@0x00433040` |
| `CDSWav_ReleaseChild_thunk_Sub34` | `0x0041a5f0` | `CDSWavStream` / `IDSChained5` `0x482354` | 2 | `SUB ECX,0x34; JMP 0x433040` | same |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub30` | `0x0041a600` | `CDSWavStream` / `IDSChained6` `0x48236c` | 3 | `SUB ECX,0x30; JMP 0x41bc00` | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` |
| `CDSChain_AdjustThisOffset_ThisMinus30` | `0x0041a610` | `CDSWavStream` / `IDSChained5` `0x482354` | 1 | `SUB ECX,0x30; JMP 0x42ac90` | `CDSChain_AdjustThisOffset@0x0042ac90` (`LEA EAX,[ECX-4]; RET`) |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub34` | `0x0041a620` | `CDSWavStream` / `IDSChained5` `0x482354` | 3 | `SUB ECX,0x34; JMP 0x41bc00` | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` |

**Vtable DATA proof (sample):** `get_xrefs_to@0x0041a530` → pointer stored at `0x00482318` (= `0x48230c` + slot3×4).

### Meta stubs (not MI adjustors)

| Ghidra symbol | Address | Vtable @ slot | Disasm | Returns |
|---------------|---------|---------------|--------|---------|
| `CDSWav_GetClassMeta` | `0x0041a510` | `face_8slots` `0x48239c` slot **0**; also `CDSAudioBankSample` `0x486f10` slot **0** | `MOV EAX,0x4b83c0; RET` | `&DAT_004b83c0` |
| `CDSWav_GetTypeInfo` | `0x0041a520` | `CDSWav` `IDSReferenced` `0x482344` slot **0** | `MOV EAX,0x4b8410; RET` | `&DAT_004b8410` |
| `CDSWavStream_GetTypeInfo` | `0x0041a5c0` | `CDSWavStream` `IDSReferenced` `0x4823c0` slot **0** | `MOV EAX,0x4b843c; RET` | `&DAT_004b843c` (class **43** factory typeinfo) |

## Ghidra deltas

| Action | Targets |
|--------|---------|
| `set_plate_comment` | `0x0041a510`–`0x0041a620` (11 functions) — vtable iface + slot + primary |
| `save_program` | `bulanci.exe` |

**No `rename_function_by_address`** — symbols already match `config/bulanci/mapping.csv`; stale `FUN_*` names remain only in committed `vftable_methods.csv` / `master_vtable_catalog.csv` (regenerate catalog separately).

## Struct doc updates

- [CDSWav.md](./CDSWav.md) — MI adjustor table extended with vtable@slot; meta stub table
- [CDSWavStream.md](./CDSWavStream.md) — full MI adjustor / meta stub table for class-43 vtables

## Remaining UNK

- **504** other `FUN_*` vtable targets in `vftable_methods.csv` (workers 1–18, 20–49 slices) — out of scope for worker 19.
- ROM-only vtable `0x00482348` (light `CDSWav_ScalarDeletingDtor` slot) — no factory write; see [round4_task_50_report.md](./round4_task_50_report.md).
- `agent_todos_50_r5.json` task **id 19** (CGame scheduler typing) is a **different** manifest row — handled by parallel R5 spawn prompt, not this worker slice.

## References

- [ROUND5_TASK_PROTOCOL.md](../ROUND5_TASK_PROTOCOL.md)
- [round4_task_50_report.md](./round4_task_50_report.md) — delete-path ownership
