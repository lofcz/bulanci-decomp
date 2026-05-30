# Round 5 — worker 29/50 report

## Task

| Field | Value |
|-------|-------|
| **worker** | 29 / 50 |
| **mode** | WRITE |
| **title** | CDSMpx / CDSMpxStream persist band verification and Ghidra persist |
| **types** | `CDSMpx`, `CDSMpxStream`, `MpxFormatTail_persist`, `CDSMpxPersistFacet`, `mad_stream` |
| **addresses** | `0x00432eb0`, `0x00433180`, `0x00446b00`, `0x00446a00` |
| **prior** | R3 task 38, R4 task 38, [pass_r4_CDSMpxStream_report.md](./pass_r4_CDSMpxStream_report.md) |

## Status

**DONE** — persist band `P+0x08..+0x30` re-verified against Save/Load/CreateFromHandle disasm; on-disk write order pinned (4 B `dwPayloadBytes` then 12 B `mpxFormatTail`); overlay table linked in `CDSMpx.md`; Ghidra field renames + comments/plates; `save_program bulanci.exe`.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Persist facet `ECX = P+0x38` | `SaveMpxFile@0x00432eb0` | `ESI = ECX`; `LEA EBX,[ESI-8]` → `P+0x30`; `LEA ECX,[ESI-0x30]` → `P+0x08` |
| On-disk write order | `SaveMpxFile@0x00432eb0` | `PUSH 4` + `LEA [ESI-8]` then `PUSH 0xc` + `LEA [ESI-0x30]` (size before format tail) |
| Seek + copy | `SaveMpxFile@0x00432eb0` | `[ESI-0x18]` stream, `[ESI-0x10/0xc]` start lo/hi, copy count from `[EBX]` (`dwPayloadBytes`) |
| Load mirrors Save | `LoadMpxFile@0x00433180` | `LEA [EDI-8]` read 4 B; `LEA [EDI-0x30]` read 12 B; `Tell64` → `[EDI-0x10/0xc]`; store stream `[EDI-0x18]` |
| CreateFromHandle band | `CreateFromHandle@0x00446b00` | `ctx+0x1c/0x24/0x28/0x2c` = `P+0x20..+0x30`; `OperatorNew(0x98c8)`; `AttachBitstream(..., ctx+0x2c)` |
| Ghidra size | `get_struct_layout CDSMpxStream` | 39112 (0x98c8); head: `mpxFormatTail@8`, `pPayloadStream@32`, `payloadStart*@40/44`, `dwPayloadBytes@48`, `stashFacet@60`, `frame@68` |
| `mad_stream` embed (decode view) | `get_struct_layout CDSMpx` | `stream@+4` (64 B); same `0x98c8` object |

### Persist band vs `mad_stream` (physical bytes)

| `P+off` | Persist field | Live decode (`mad_stream@P+0x04`) |
|---------|---------------|-------------------------------------|
| `+0x08..+0x13` | `mpxFormatTail` (12 B file) | `bufend` + `skiplen` + `sync` |
| `+0x14..+0x1f` | not in 12-byte file tail | `freerate` + `this_frame` + `next_frame` |
| `+0x20` | `pPayloadStream` | `main_bit` dword 0 |
| `+0x28` | `payloadStartLo` | `anc_bit` [0..3] |
| `+0x2c` | `payloadStartHi` | `anc_bit` [4..7] |
| `+0x30` | `dwPayloadBytes` | `aux` |

## Ghidra deltas

- `modify_struct_field` `CDSMpxStream`: `pad_mad_overlap_14`, `payloadStartLo`, `payloadStartHi`, `pad_before_persist_chained` (MCP success; layout export may retain `pPad_*` / `dwPayloadStart*`)
- `set_decompiler_comment` @ `0x00432ec0`, `0x00432ecf`, `0x00432edd`, `0x0043318f`
- `set_plate_comment` @ `0x00432eb0`, `0x00433180`, `0x00446b00`
- `get_struct_layout` `CDSMpxStream`, `CDSMpx`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSMpxStream.md](./CDSMpxStream.md) — on-disk order; R5 Ghidra apply block
- [CDSMpx.md](./CDSMpx.md) — persist band overlay table (stream-relative aliases)

## Remaining UNK

- Ghidra `get_struct_layout` auto-prefix on renamed head fields (`pPad_mad_overlap_14`, `dwPayloadStartLo`, …).
- Registry `0x48` shell vs decode `0x98c8` — same heap block or not in live game.
- Non-null writers / concrete type of `stashFacet.pSubObjStash` on MPx path.
- Sub-byte WAVEFORMAT ↔ dword alias inside `mpxFormatTail` during decode overlay.
