# Round 4 — Task 47 Report

## Task

| Field | Value |
|-------|--------|
| **id** | 47 |
| **round** | 4 (R3 closure verify) |
| **title** | Verify CDSStrmResInfo pad_tail @ +0x20 AppendOrReuseStream read |
| **one_liner** | R4 re-check: `+0x20` is `dwFilterSliceAddend` (sole consumer `AppendOrReuseStream`); `+0x14` on `CDSStrmResInfo` is `streamExtent`, not `CDSResInfo::pEmbeddedResource`. |
| **prior** | [round3_task_47_report.md](./round3_task_47_report.md) |
| **types** | CDSStrmResInfo, CDSResInfo, CDSStreamStorage |

## Status

**DONE** — R4 MCP verification confirms R3 struct layout, decompiler field names, and asm at `0x0043405b`. No Ghidra mutations required.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `CDSStrmResInfo` 40 B layout | Ghidra | `streamExtent` `ulonglong` @ `+0x14` (20); `dwStreamFlags` @ `+0x1c` (28); `dwFilterSliceAddend` @ `+0x20` (32); `dwPad_reserved_hi` @ `+0x24` (36) |
| Base `CDSResInfo` 24 B | Ghidra | `pEmbeddedResource` `void *` @ `+0x14` (20) — base only |
| Sole `[EDI+0x20]` read | `0x0043405b` | Disasm `MOV EDX,dword ptr [EDI + 0x20]` inside `CDSStreamStorage_AppendOrReuseStream@0x00433f70`; pushed 4th arg before `CALL 0x00434760` |
| Decompiler names addend | `0x00433f70` | `entry->dwFilterSliceAddend` passed to `CDSStreamStorage_CreateFilterSafeStream`; PRE comment @ load site |
| Append vs reuse branch | `0x00433fad` | `CMP dword ptr [EDI+0x14],EBP` — zero low `streamExtent` → append path using `+0x20` |
| Filter ctor adds slice | `0x00434760` | `CDSFilterStream_Ctor(..., sliceOffsetLo + sliceExtentLo, ...)` — `sliceExtentLo` is addend param |
| Serialize tail only through `+0x1f` | `0x00433940` | `CDSResInfo_Load` then `Read` 8 B `@streamExtent`, 4 B `@dwStreamFlags`; no `+0x20` I/O |
| Deserialize symmetric | `0x00433980` | `CDSResInfo_Save` then 8+4 B write; PRE distinguishes extent vs `pEmbeddedResource` |
| Release uses base pointer | `0x00434270` | `CDSResInfo_ReleaseEmbeddedResource` on `pEmbeddedResource`; PRE warns not for stream entries |

### Offset map (verified)

| Struct | `+0x14` | `+0x20` |
|--------|---------|---------|
| `CDSResInfo` (24 B) | `pEmbeddedResource` | *(N/A — object ends @ 24)* |
| `CDSStrmResInfo` (40 B) | `streamExtent` (8 B) | `dwFilterSliceAddend` (4 B) |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `get_struct_layout` | `CDSStrmResInfo`, `CDSResInfo` | Matches R3 report |
| `force_decompile` | `0x00433f70`, `0x00433940`, `0x00433980`, `0x00434270`, `0x00434760` | Field names and PRE comments intact |
| `disassemble_function` | `0x00433f70` | Confirmed `8b5720` @ `0x0043405b` |
| `save_program` | `bulanci.exe` | saved |

No struct/comment changes — R3 apply already correct.

## Struct doc updates

- [CDSStrmResInfo.md](./CDSStrmResInfo.md) — R4 todo 47 verify log.

## Remaining UNK

- `dwPad_reserved_hi` @ `+0x24`: no consumer (alignment / reserved).
- Whether `dwFilterSliceAddend` is ever non-zero from pack data (not in serialize path; may be stack-key / loader only).
