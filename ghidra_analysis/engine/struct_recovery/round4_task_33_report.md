# Round 4 — Task 33 report

## Task

| Field | Value |
|-------|--------|
| **id** | 33 |
| **title** | Xref CDSFlxFile bodySeekBiasLo non-zero writers |
| **source** | handoff (`agent_todos_50_r4.json`; supersedes R3 todo 33) |
| **supersedes_todo_id** | 33 |
| **addresses** | `0x00432a50` (CreateObject), `0x00432ac0` (BindStream), `0x00432c60` (DecodeFrame) |
| **acceptance** | Program-wide proof whether outer `bodySeekBiasLo` @ `+0x2c` is ever stored non-zero; document Seek operand face aliases |

**Types:** `CDSFlxFile`

## Status

**DONE** — No non-zero writer to outer `bodySeekBiasLo`; sole store is factory zero. `DecodeFrame` / `CloseStream` operands at meta `+0x2c` / `+0x30` are `dwStreamTellLo` / `dwStreamTellHi`, not `bodySeekBiasLo`.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Sole writer outer `+0x2c` | `CDSFlxFile_CreateObject@0x00432a50` | `MOV dword ptr [EAX+0x2c], ECX` @ `0x00432a6d` with `ECX=0` after `XOR ECX,ECX` |
| BindStream skips `+0x2c` | `CDSFlxFile_BindStream@0x00432ac0` | `ESI` = IDSResource `+0x24` face; stores `[ESI±…]` through `+0x28` only — no `[ESI+0x2c]` |
| FLX cluster store scan | `.text` `0x432000`–`0x433fff` | `search_byte_patterns` `89 48 2c` → only `0x00432a6d`; no `89 46/4e/7e 2c` or `c7 40 2c` in cluster |
| Program `MOV [reg+0x2c]` stores | whole module | `89 48 2c` hits include `0x00432a6d` plus unrelated types (`CDSImage`, `CWindow`, …); none in `0x432xxx` besides factory |
| Seek addend (not bias) | `DecodeFrame@0x00432c60` | `ADD EDX, dword ptr [ESI+0x2c]` @ `0x00432c89`; `ESI` = meta `+0x04` face → operand is outer `+0x30` (`dwStreamTellLo`) |
| CloseStream Seek64 | `CDSFlxFile_CloseStream@0x00432b60` | `MOV EDX, [ESI+0x2c]` / push `[ESI+0x30]` @ `0x00432ba2` — streamTell u64 on meta face |
| Struct unchanged | `get_struct_layout CDSFlxFile` | 80 B; `bodySeekBiasLo` @ offset `0x2c` (44) |

### Face alias map (why R3 “bias” was ambiguous)

| `this` face | Insn operand | Absolute outer field |
|-------------|--------------|----------------------|
| Outer base | `[EAX+0x2c]` | `bodySeekBiasLo` — **write-only zero** |
| Meta `+0x04` | `[ESI+0x2c]` | `dwStreamTellLo` @ outer `+0x30` |
| Meta `+0x04` | `[ESI+0x30]` | `dwStreamTellHi` @ outer `+0x34` |

## Ghidra deltas

1. `set_decompiler_comment@0x00432a6d` — sole `bodySeekBiasLo` writer, always 0
2. `set_decompiler_comment@0x00432c89` — meta `+0x2c` = `dwStreamTellLo`, not `bodySeekBiasLo`
3. `set_decompiler_comment@0x00432ba2` — CloseStream Seek64 uses streamTell pair
4. `get_struct_layout CDSFlxFile`
5. `save_program bulanci.exe`

## Struct doc updates

- [CDSFlxFile.md](./CDSFlxFile.md) — layout row `+0x2c`; Follow-up R4 task 33; removed UNK on non-zero bias
- [flx_file_format.md](../../formats/flx_file_format.md) — outer table rows `+0x2c` / `+0x30`

## Remaining UNK

- None for this task. `bodySeekBiasLo` may be a dead reserved field (always 0 in shipped binary); repurposing would need new evidence outside `.text`.
