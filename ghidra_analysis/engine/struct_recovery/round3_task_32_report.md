# Round 3 — Task 32 report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 32 |
| **title** | Ghidra comments for nSeqTotalDurationMs / TM seq[0x10] |
| **types** | CDSFlxFile, CDSAnim, CBulPicture |
| **acceptance** | Document FLX `inMemSize` → `nSeqTotalDurationMs` / track `seq[0x10]`; Ghidra comments at BindStream store and TM_AdvanceFrame Bresenham |

## 2. Status

**DONE** — `CDSFlxFile+0x14` `nSeqTotalDurationMs` named in Ghidra; BindStream store @ `0x00432b18` and `TM_AdvanceFrame@0x004399b0` annotated; struct docs updated.

## 3. Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| FLX header `inMemSize` (file `+0x1c`) → outer `+0x14` | `CDSFlxFile_BindStream@0x00432ac0` | After `Read(0x24)`, `MOV [ESI+0x10],EDX` @ `0x00432b18` on IDSResource `+0x24` face (`ESI`) = outer `nSeqTotalDurationMs` @ `+0x14`; stack slot holds header dword from file offset `0x1c` |
| FLX header `flags` (file `+0x20`) → outer `+0x18` | same | `MOV [ESI+0x1c],ECX` @ `0x00432b4d` = `nSeqFrameCountMinusOne` (`frameCount - 1`) |
| Track `seq` pointer is CDSFlxFile meta face | `TM_AdvanceFrame@0x004399b0` | `MOV ECX,[EDI+4]` — track entry `+4` is sequence object; ClassID **52** resources use `resource+4` meta face (`CDSFlxFile.md`) |
| Bresenham uses `seq[0x10]` / `seq[0x14]` | `TM_AdvanceFrame@0x004399b0` | `IMUL EAX,[ECX+0x10]` / `DIV [ECX+0x14]` @ `0x004399d5`–`0x004399e9`; wrap compare `CMP EAX,[ECX+0x14]` @ `0x00439a21` |
| CBulPicture does **not** set timing | `CBulPicture_Create@0x0040eb30` | No store to FLX meta timing band; track timing comes from pool `BindStream` on load (`CBulPicture.md` worker 32) |
| Pack constant | master-pack BitmapSprite headers | `inMemSize == 0x470` (1136) on shipped sprites — coincides with `sizeof(CBulPicture)` alloc hint but is the on-disk FLX field consumed as total clip ms |

### Offset alias map (track-manager “seq” face)

| Track seq offset | CDSFlxFile outer | On-disk FLX | Role |
|-----------------:|------------------:|------------:|------|
| `seq[0x10]` | `+0x14` `nSeqTotalDurationMs` | file `+0x1c` `inMemSize` | Total clip duration **ms** (Bresenham numerator) |
| `seq[0x14]` | `+0x18` `nSeqFrameCountMinusOne` + 1 | file `+0x20` `flags` + 1 | Frame count (Bresenham denominator / wrap) |

**Do not confuse** with `CBulPicture.wViewFlags` @ `+0x14` (ushort view flags) — unrelated to anim timing.

## 4. Ghidra deltas

- `get_struct_layout CDSFlxFile` — 80 B; `nSeqTotalDurationMs` @ offset `0x14`, `nSeqFrameCountMinusOne` @ `0x18`
- `set_decompiler_comment@0x00432ac0` — BindStream header scatter + `inMemSize` → `nSeqTotalDurationMs` / `seq[0x10]`
- `set_disassembly_comment@0x00432b18` — store site for `inMemSize` → `nSeqTotalDurationMs`
- `set_decompiler_comment@0x004399b0` — Bresenham formula on `seq[0x10]` / `seq[0x14]`
- Plate comment on `CDSFlxFile_BindStream` — full file→outer dword map (`flx_file_format.md`)
- `save_program bulanci.exe`

**Note:** Ghidra MCP `modify_struct_field` has no field-comment parameter; dual semantics (FLX alloc hint vs anim total ms) are captured via field name `nSeqTotalDurationMs` plus BindStream / TM comments above.

## 5. Struct doc updates

- `CDSFlxFile.md` — layout row `+0x14`, Ghidra apply block, agent todo 33 header table
- `CBulPicture.md` — sequence timing paragraph (worker 32): Create does not write `seq[0x10]`
- `anim_runtime.md` — `seq[0x10]` source marked resolved (todo 32)
- `status.md` — todo 32 marked done

## 6. Remaining UNK

- Whether `0x470` is always intentional total clip ms vs accidental reuse of `sizeof(CBulPicture)` (pack shows constant on master-pack sprites; Bresenham then yields ~`1136/N` ms per frame).
- Non–ClassID-52 `CDSAnimSequence` types may populate `seq[0x10]`/`seq[0x14]` differently (this task scoped FLX / BitmapSprite path).
