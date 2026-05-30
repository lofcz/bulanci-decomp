# Round 4 — task 32 report (R4 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 32 |
| **title** | Audit FLX inMemSize 0x470 vs sizeof(CBulPicture) and other seq types |
| **source** | handoff (`agent_todos_50_r4.json` / `todos_gather_r4_3.json`) |
| **supersedes_todo_id** | 32 |
| **types** | CDSFlxFile, CDSAnim, CBulPicture, CDSAnimSequence |
| **addresses** | `0x00432ac0`, `0x00432b18`, `0x004399b0`, `0x0040eb30`, `0x004340c0` |
| **acceptance** | Decide whether `inMemSize` is `sizeof(CBulPicture)` reuse vs per-clip ms; document other `CDSAnimSequence` timing sources |

## Status

**DONE** — `inMemSize` is per-sprite total clip duration **ms** (not the CBulPicture alloc constant); sole `.text` Bresenham consumer is `TM_AdvanceFrame`; ClassID **76** `CDSDsmFile` fills the same meta-face offsets via `HandleOpenStream`.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| `CBulPicture_Create` allocates `0x470` only for the view shell | `CBulPicture_Create@0x0040eb30` | `OperatorNewWithBadAlloc(0x470)`; no store to any `seq[0x10]` / FLX timing band |
| Sole writer of FLX `seq[0x10]` (`nSeqTotalDurationMs`) | `CDSFlxFile_BindStream@0x00432ac0` | `MOV [ESI+0x10],EDX` @ `0x00432b18` after `Read(0x24)`; stack slot = file `+0x1c` `inMemSize` |
| Sole Bresenham reader of `[seq+0x10]` / `[seq+0x14]` | `TM_AdvanceFrame@0x004399b0` | Only `IMUL EAX,[ECX+0x10]` / `DIV [ECX+0x14]` in program @ `0x004399d5`–`0x004399e9`; wrap `CMP EAX,[ECX+0x14]` @ `0x00439a21` |
| `inMemSize` ≠ `sizeof(CBulPicture)` for almost all shipped sprites | `unpacked/overlay/_manifest.json` (130× ClassID **52**) | **1/130** have `inMemSize == 1136 (0x470)` (resource `65713`); **70** `< 1136`, **59** `> 1136`; range **210..8591** |
| Values behave as total clip ms | same manifest + Bresenham | `inMemSize / (flags+1)` → **35..100** ms/frame (median **~65**); not correlated with `0x470` alloc |
| Gameplay anim bind uses anim-sequence RTTI | `CBulanci_AllocAnimFromSprite@0x0041cff6` | `CheckedVirtualBaseCast(resource, DAT_004b8370)` → `CAnim::ParameterizedCtor(..., seq, 3)` → `AnimInner_InitParam` → `TM_BindSequence` |
| Movie / DSM path also reaches `TM_AdvanceFrame` | `CDSAudioVideoPlayer_SetupTrack` | `get_xrefs_to(TM_AdvanceFrame)` includes `0x0043bbe3` |
| DSM header fills same meta offsets (different file layout) | `CDSDsmFile::HandleOpenStream@0x00428ad0` | File `+0x10` → `dwDurationMs` @ outer `+0x14`; file `+0x14` → `dwFrameCount` @ `+0x18` (full count, not `flags−1`) |
| Abstract `CDSAnimSequence` layout | Ghidra struct | `dwDurationMs` @ `+0x10`, `dwFrameCount` @ `+0x14` (24 B) — matches track-manager `seq[0x10]` / `seq[0x14]` aliases |

### Pack audit (`inMemSize` vs `0x470`)

| Metric | Value |
|--------|------:|
| Master-pack BitmapSprite count | 130 |
| `inMemSize == 1136 (0x470)` | 1 |
| `inMemSize < 1136` | 70 |
| `inMemSize > 1136` | 59 |
| Most common `inMemSize` | 355 (13 sprites), 1275 (12), 426 (9) |

**Conclusion:** `0x470` is a historical “alloc hint” name on the FLX header field, not the dominant on-disk value. Runtime treats the dword as **total clip milliseconds** for Bresenham scheduling regardless of whether it accidentally equals `sizeof(CBulPicture)` on a single asset.

## Ghidra deltas

- `set_decompiler_comment@0x00432b18` — R4 pack audit (`1/130 == 0x470`; per-sprite ms range)
- `set_decompiler_comment@0x004399b0` — sole Bresenham site; FLX vs DSM meta-face note
- `set_decompiler_comment@0x0040eb30` — `0x470` alloc unrelated to FLX `inMemSize` timing
- `search_instructions` — confirmed single `IMUL [ECX+0x10]` consumer; `CDSFlxFile_BindStream` sole `MOV [ESI+0x10]` store on FLX path
- `get_xrefs_to@0x004399b0` — TM + `CDSAudioVideoPlayer_SetupTrack`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSFlxFile.md](./CDSFlxFile.md) — `nSeqTotalDurationMs` pack statistics; R4 Ghidra note
- [CBulPicture.md](./CBulPicture.md) — timing paragraph + UNK cleared for `0x470` coincidence
- [anim_runtime.md](../anim_runtime.md) — open-question closure; DSM parallel path
- [status.md](../status.md) — R4 todo 32 done line

## Remaining UNK

- **FLX `flags` vs `CDSAnimSequence::dwFrameCount`:** on-disk `flags = frameCount − 1` but TM wrap compares `dwCurrentFrameIdx` to `seq[0x14]` (same offset as `nSeqFrameCountMinusOne` on FLX). DSM stores a **full** `dwFrameCount` at the same offset — mixed semantics under one abstract struct name.
- Whether any non–ClassID-52/76 resource is ever bound as `DAT_004b8370` anim sequence without populating `seq[0x10]` (no second writer found in `.text` besides `CDSFlxFile_BindStream` for FLX).
