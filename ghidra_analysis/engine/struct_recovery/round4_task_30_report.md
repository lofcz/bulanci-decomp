# Round 4 — Task 30 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 30 |
| **round** | 4 |
| **title** | CDSFilterStream_ReadBytes retype IDSStream* plate at embed +0x0c |
| **source** | blocker (`agent_todos_50_r4.json`; supersedes R3 todo 30) |
| **types** | `CDSFilterStream`, `CDSSafeStream`, `CDSStreamStorage` |
| **prior** | [round3_task_30_report.md](./round3_task_30_report.md) |

## Status

**DONE** — `ReadBytes` moved to class `IDSStream` with `IDSStream *` this; decompile uses filter plate field names; `save_program`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| ECX plate cursor/cap/inner | `CDSFilterStream_ReadBytes@0x00430420` | Disasm: `[ESI+0xc]`/`[+0x10]` cursor u64; `[ESI+0x1c]`/`[+0x20]` cap; `[ESI+0x24]` inner; delegate `inner->vtbl[0x10]` |
| `this` is filter `+0x0c` | `0x00430455` | Throw helper: `LEA EAX,[ESI-0xc]` — recover outer filter for `CDSStreamException` |
| R3 blocker closed | R3 report | R3 renamed `dwIdsStream_state` @ outer `+0x10`; left `ReadBytes` on outer `CDSFilterStream *` |
| Mem stream same class | `CDSEasyMemStream_ReadBytes@0x004307f0` | Already `IDSStream::ReadBytes`; shares plate struct at different offset semantics |

### Decompile after R4 (filter)

```c
void __thiscall IDSStream::ReadBytes(IDSStream *this, void *buf, uint count)
{
  // bounded: nSizeCapHi >= 0; compare dwCursorLo/Hi + count vs dwSizeCapLo / nSizeCapHi
  // (**(code **)(*(int *)this->pInnerStream + 0x10))(buf, count);
  // advance dwCursorLo / dwCursorHi
}
```

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x00430420` | `IDSStream::ReadBytes`; `this` = `IDSStream *` |
| `resize_struct` | `IDSStream` | 32 → **40** bytes |
| `modify_struct_field` / `add_struct_field` | `IDSStream` | `dwCursorLo`/`dwCursorHi`, `dwWindowBaseLo`/`Hi`, `dwSizeCapLo`, `nSizeCapHi`, `pInnerStream` @ plate `+0x0c..+0x24` |
| `force_decompile` | `0x00430420` | Clean filter field refs (no `this[1]` / `pVftable_IDSStream` misuse) |
| `set_decompiler_comment` | `0x00430420` | Plate vs outer offset map |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSFilterStream.md](./CDSFilterStream.md) — cleared ReadBytes `this` UNK; R4 follow-up.
- [CDSEasyMemStream.md](./CDSEasyMemStream.md) — shared `IDSStream` plate note (40 B, dual semantics).

## Remaining UNK

- Other `CDSFilterStream` `IDSStream` slots (`WriteBytes`, `Seek`, `Tell`, …) still on outer `CDSFilterStream *` unless individually retyped.
- `IDSStream` Ghidra names at `+0x0c..+0x24` are filter-biased; mem stream `ReadBytes` uses `dwSizeCapLo` as backing pointer cast and `dwCursorLo` as logical size — document-only, not separate structs.
- `CDSFilterStream` filter-only `dwIdsStream_state` @ outer `+0x10` remains write-only snapshot (R3).
