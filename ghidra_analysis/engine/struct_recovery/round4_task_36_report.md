# Round 4 — agent todo 36 report (R4 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 36 |
| **title** | Plate CDSQueueStream vs CDSJpegImage classId 0x4b registry sites |
| **source** | `todos_gather_r4_3.json` / `agent_todos_50_r4.json` (backlog; supersedes R3 todo 36) |
| **types** | CDSJpegImage, CDSQueueStream |
| **addresses** | `0x0043c160`, `0x00432070`, `0x0047daa0`, `0x0047cbf0`, `0x00432030` |
| **acceptance** | Plate/decompiler comments disambiguate registry class id **0x4b** (queue stream) vs JPEG registry **0x15** vs wrapper quality dword **0x4b** @ `+0x60`; `save_program` |

## Status

**DONE**

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Queue-stream factory ≠ JPEG | `CDSQueueStream_CreateObject` @ `0x0043c160` | `PUSH 0x24`; plate + EOL: not `CDSJpegImage` |
| JPEG wrapper factory | `CDSJpegImage_CreateObject` @ `0x00432070` | `PUSH 0x64` → `JMP CDSJpegImage_InitVtables` |
| Registry class id **0x4b** (75) | `CDSQueueStream_StaticClassRegister` @ `0x0047daa0` | `PUSH 0x43c160`, `PUSH 0x4b7bfc`, `PUSH 0x4b`; `CALL HandleClassRegister` |
| Registry class id **0x15** (21) | `CDSJpegImage_StaticClassRegister` @ `0x0047cbf0` | `PUSH 0x432070`, `PUSH 0x4b7bfc`, `PUSH 0x15`; meta `ECX=0x4b7d80` |
| Quality read ≠ registry id | `CDSJpegImage_Save` @ `0x00432030` | IDSChained MI: `[ECX+0xc]` → object `+0x60` → `CompressFromImage` / `jpeg_set_quality`; decompiler comment names **0x15** vs **0x4b** split |
| Factory xrefs | `get_xrefs_to` | `0x0043c160` ← only `CDSQueueStream_StaticClassRegister`; `0x00432070` ← `CDSJpegImage_StaticClassRegister` (+ data ref `0x0047f488` vtable/init table) |

### Three meanings of **0x4b**

| Consumer | Site | Role |
|----------|------|------|
| Engine registry | `CDSQueueStream_StaticClassRegister` @ `0x0047daa0` | Class id **75** for `CDSQueueStream` |
| Wrapper tail | `CDSJpegImage_InitVtables` @ `0x00431d48` | Default IJG quality **75** stored @ object `+0x60` |
| Not used | `CDSJpegImage_StaticClassRegister` | JPEG class id is **`0x15`**, not `0x4b` |

## Ghidra deltas

- `set_plate_comment` @ `0x0047daa0`, `0x0047cbf0`, `0x0043c160`, `0x00432070` (R4-tagged disambiguation of **0x4b** vs **0x15** vs heap sizes)
- `set_decompiler_comment` @ `0x00432030` (`CDSJpegImage_Save` MI quality vs registry ids)
- `get_xrefs_to` @ `0x0043c160`, `0x00432070`, `0x0047daa0`, `0x0047cbf0`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSJpegImage.md](./CDSJpegImage.md) — R4 Ghidra apply note; cross-ref this report

## Remaining UNK

- None for this task scope (R3 closed factory identity and `+0x60` write/read audit).
