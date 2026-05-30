# Round 3 — Task 33 report

## Task

| Field | Value |
|-------|-------|
| **id** | 33 |
| **title** | Verify CDSFlxFile Ghidra fields vs flx_file_format.md |
| **source** | `agent_todos_50_r3.json` (handoff; supersedes stale `round3_tasks.json` CBulPicture entry) |
| **addresses** | `0x00432a50` (factory), `0x00432ac0` (`BindStream`), `0x00432c60` (`DecodeFrame`) |
| **acceptance** | Ghidra `CDSFlxFile` layout matches file↔object header table; no `hdr_*` / `field_*` leftovers; `flx_file_format.md` + `CDSFlxFile.md` aligned |

## Status

**DONE** — Ghidra struct (80 B) matches `CDSFlxFile.md` and the proven `BindStream` dword map; `flx_file_format.md` stale outer-field section replaced; plate comment @ `BindStream`.

## Ghidra vs docs (verification)

`get_struct_layout CDSFlxFile` → **Size 80 (0x50)**, 20 named fields. No `hdr_dword*` / `field_*` placeholders remain.

| Outer `+` | Ghidra name | `CDSFlxFile.md` | `flx_file_format.md` file↔outer |
|---:|---|---|---|
| `0x08` | `nChannels` | ✓ | file `+0x18` |
| `0x0c` | `nBitmapHeight` | ✓ | file `+0x14` |
| `0x10` | `nBitmapWidth` | ✓ | file `+0x10` |
| `0x14` | `nSeqTotalDurationMs` | ✓ | file `+0x1c` (was `nInMemSizeHint` in doc — renamed) |
| `0x18` | `nSeqFrameCountMinusOne` | ✓ | file `+0x20` (was `nAnimFrameCountMinusOne` — renamed) |
| `0x2c` | `bodySeekBiasLo` | ✓ | (not in 36 B read; factory `0`) |
| `0x30`/`0x34` | `dwStreamTellLo`/`Hi` | `streamTell` u64 | `Tell()` after header |
| `0x38` | `pSourceStream` | ✓ | stream param |
| `0x3c` | `pDecodeBuffer` | ✓ | lazy scratch |
| `0x40` | `bodyStartCursor` | ✓ | file `+0x04` |
| `0x44` | `bodyEndCursor` | ✓ | `len - 0x24` |
| `0x48` | `dwTotalSize` | ✓ | file `+0x00` |
| `0x4c` | `dwEncodedSize2` | ✓ | file `+0x08` |

**Disasm proof:** `CDSFlxFile_BindStream@0x00432ac0` — single `Read(0x24)`, stores via `ESI` = `+0x24` face (`[ESI±N]` → outer `N±0x24`). Matches unpacker fields `[0..8]` in `bulanci_unpack.py` (`totalSize` … `flags`).

**Naming note:** Ghidra splits `streamTell` as `dwStreamTellLo`/`dwStreamTellHi` @ `+0x30`/`+0x34` (MSVC u64). File-level aliases `inMemSize` / `flags` kept in unpacker; outer/canonical names are `nSeqTotalDurationMs` / `nSeqFrameCountMinusOne`.

## Ghidra deltas

- `set_plate_comment@0x00432ac0` — file dword → outer field map (see plate text).
- `save_program bulanci.exe`
- No struct field renames required (batch 30 / todo 32–33 already applied canonical names).

## Doc updates

- `ghidra_analysis/formats/flx_file_format.md` — file↔outer table uses canonical outer names; **§ Outer field layout** rewritten (removed erroneous “body size @ +0x40” / “TBD @ +0x44”).
- `ghidra_analysis/engine/struct_recovery/CDSFlxFile.md` — already current (agent todo 33 table §); no layout change.
- **Replaced** prior `round3_task_33_report.md` (incorrect CBulPicture content from obsolete `round3_tasks.json`).

## Remaining UNK

- Whether `bodySeekBiasLo` is ever non-zero outside `CreateObject` (no writer in `0x432xxx` cluster).
- `bulanci.ghidra.exe.c` export still shows pre-recovery `dwHdr_dword*` names in decompile of `BindStream` — stale C export, not live Ghidra struct.
