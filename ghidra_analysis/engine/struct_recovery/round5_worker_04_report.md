# Round 5 — worker 04 report (level / score / teleport)

## Task

| Field | Value |
|-------|--------|
| **Worker** | 4 / 50 |
| **Mode** | WRITE |
| **Address band** | `FUN_*` ~`0x00460000`–`0x00480000` |
| **Theme** | Level list, level scores, post-match score UI, teleport gates |
| **Rules** | Evidence-only renames; disasm + xref + sole-caller proof |

## Status

**PARTIAL** — The assigned VA band is almost entirely **libjpeg / zlib / libmad** and **`.CRT$XCU` atexit stubs**. No `FUN_*` in `0x00460000`–`0x0047dfff` has a defensible level/score/teleport role. **Three score-adjacent renames** were applied **outside** the band (sole callers in `CLevelScore_*` / `CScore_RenderHighScoreRow`). Struct doc address drift in `CLevelScore.md` was corrected.

## Range inventory (`FUN_*`, still default-named)

Ghidra `search_functions_enhanced` → **~69** `FUN_*` entries with `address ∈ [0x00460000, 0x00480000)` (excluding already-named IJG/zlib symbols such as `jinit_marker_writer`, `encode_one_block`, `III_huffdecode`).

| Sub-range | Count (approx.) | Verdict | Evidence |
|-----------|-----------------|---------|----------|
| `0x00460000`–`0x0046dfff` | ~55 | **SKIP — codecs** | Callee chains only from `encode_one_block`, `jpeg_compress_data`, `III_huffdecode`, `start_pass_dcolor`, `send_bits`, `bi_windup`, etc. Example: `FUN_00468120@0x00468120` — bit-stuff emit; xrefs **only** from `encode_one_block` / `FUN_004681d0`. `FUN_0045a220@0x0045a220` — libmad fixed-point; xrefs **only** from `III_huffdecode`. |
| `0x0046e000`–`0x0046ffff` | (mixed) | **SKIP — zlib 1.1.x** | `FUN_00472420` — `_tr_stored_block` fragment; callers `send_bits@0x004728e1`. Documented in `ghidra_analysis/formats/gzip_stream.md`. |
| `0x00470000`–`0x0047dfff` | ~8 | **SKIP — IJG tails** | `FUN_004713d0` — Huffman length adjust; sole cluster `FUN_004725a0`. No xref from gameplay `.text`. |
| `0x0047de40`–`0x0047ee90` | ~130 | **SKIP — static init** | Each `FUN_*` has **xref_count = 1** (atexit / `HandleClassRegister` tail). Same class as R5 worker 15/17 CRT glue. |

**Level-script heap size `0x460`:** `OperatorNewWithBadAlloc(0x460)` for `CLevelScript` is at **`CreateObject@0x00408ec1`** (`PUSH 0x460` disasm) — **outside** this band. `CDSScript.md` note “`0x00464415`” was a stale Ghidra anchor inside **libjpeg `int_upsample`**, not the factory.

## Evidence — renames applied (out-of-band, score theme)

| Old | New | Address | Proof |
|-----|-----|---------|--------|
| `FUN_004096d0` | `CScore_RenderHighScoreColumn` | `0x004096d0` | Decompile: `CStaticText_BuildAt` + `CDSView__AddChild`; **5×** `CALL 0x004096d0` from `CScore_RenderHighScoreRow@0x00409bc0` (`0x00409c18`, `0x00409c30`, `0x00409c52`, `0x00409c74`, `0x00409c99`). Plate comment already noted scoreboard row helper. |
| `FUN_0042fae0` | `CDSChain_SortChildrenWithComparator` | `0x0042fae0` | Decompile: `_qsort` + `CDSChained_ClearChildren` + `CDSChained_PrependChild` on list head. **Sole caller:** `CLevelScore_AddPlayerScore@0x00409b7f` with `CScoreItem_CompareByNetScore`. |
| `FUN_0042f8b0` | `CDSChain_GetChildAtIndex` | `0x0042f8b0` | Indexed walk on `+0x8`/`+0xc` links. **Sole caller:** `CLevelScore_AddPlayerScore@0x00409b96` (prune when `dwChildCount > 6`). |

## Evidence — primary symbols (in-band callers none)

Gameplay anchors for this theme (already named in Ghidra / `mapping.csv`):

| Symbol | Address | Role |
|--------|---------|------|
| `CGame_GetOrCreateLevelScore` | `0x00414280` | `PUSH 0x28` @ `0x004142b4`; vtables @ `0x004142c8`–`0x004142ec`; append @ `LEA ECX,[ESI+0x95]` `0x00414302` |
| `CGame_FindLevelScoreInChain` | `0x00413940` | Walk `CGame+0x95`; `LEA ECX,[ESI+0x10]` + `CDsString` compare @ `0x00413980` |
| `CLevelScore_AddPlayerScore` | `0x00409b10` | Uses renamed sort/index helpers above |
| `CLevelScore_InitializeDefaultScores` | `0x00409620` | Six× `OperatorNew(0x1c)` |
| `CLevelList_ctor` / `CLevelList_RenderItem` | `0x0040b0c0` / `0x0040d490` | Lobby list UI |
| `CTeleportPoint_Ctor` / `OnEvent` / `TriggerTeleportFX` | `0x0041c9a0` / `0x0041fed0` / `0x0041fca0` | Gate pair / FX (band `0x0041xxxx`) |
| `CreateObject` (`CLevelScript`) | `0x00408ec1` | `OperatorNew(0x460)` — level scripts, not bare `CDSScript` |

## Ghidra deltas

- `rename_function_by_address` → `CScore_RenderHighScoreColumn@0x004096d0`
- `rename_function_by_address` → `CDSChain_SortChildrenWithComparator@0x0042fae0`
- `rename_function_by_address` → `CDSChain_GetChildAtIndex@0x0042f8b0`
- `save_program bulanci.exe`

## Struct doc updates

- `CLevelScore.md` — fixed ctor/allocation evidence addresses (were wrongly cited as `0x004776xx`).
- `CScore.md` — `FUN_004096d0` → `CScore_RenderHighScoreColumn`.
- `CScoreItem.md` — prune helper name `CDSChain_GetChildAtIndex`.
- `CDSChain.md` — sort helper + cross-ref to score prune path.

## Remaining UNK (this worker)

- **In-band:** No gameplay `FUN_*` left to rename without mislabeling jpeg/zlib/libmad/atexit.
- **Out-of-band:** `CLevelScore_AddPlayerScore` decompiler still types rows as `CDSChain *` (true `CScoreItem *`). `FUN_0042f7c0` / `FUN_0042f920` on find-walk path at `CGame_FindLevelScoreInChain` remain `FUN_*` (R5 band 08 / chain worker).
- **Teleport:** `FUN_00416590` (`CAnim` view init) @ `0x00416590` — below `0x00460000`; CTeleportPoint field map in `CTeleportPoint.md`.

## Follow-up

- Delegate **jpeg cluster** `0x00460000`–`0x0046dfff` to a codecs worker (align with `jpeg_decoder.md` / `gzip_stream.md` verified tables).
- Delegate **`0x0047de40+`** atexit `FUN_*` to R5 ctor/static-init worker 17.
