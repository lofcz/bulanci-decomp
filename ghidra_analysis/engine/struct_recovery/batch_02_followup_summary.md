# Struct recovery batch 02 follow-up (round 2)

**Prior:** `batch_02_summary.md`, `CDSScript.md`, `CAdvertising.md`  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_PARTIAL** (Ghidra updated; some UNK remain)  
**Saved:** `save_program bulanci.exe` (once)

## Actions taken

### CAdvertising

| Action | Evidence |
|--------|----------|
| Replaced `byte[28] pUpdatedItem` @ `+0x70` with `CDSUpdatedItem updatedItem` (24 B) | `CDSUpdatedItem` VERIFIED @ 24 B (`CDSUpdatedItem.md`); ctor `CDSUpdatedItem_ctor(this+0x70)` @ `0x0040e5f0` |
| Added `byte[4] pPad_preDismiss` @ `+0x88` | `0x70+0x18=0x88`; `m_bBlockDismiss` @ `+0x8c` |
| Renamed `bM_bBlockDismiss` → `m_bBlockDismiss` | Matches layout table naming |
| Updated `CAdvertising.md` | Layout + Ghidra apply + UNK |

### CDSScript

| Action | Evidence |
|--------|----------|
| Renamed `dwPad_0c` → `reserved_0c`, `dwPad_14` → `reserved_14` | Only `CDSScript::ctor@0x00438390` writes; no reads in `ReadU8`/`Run`/`CallExport`/`InstallOpcodeTable`/`dtor` |
| Xref check on `+0x18 scriptData` | `CDSScript_dtor` frees `param_1[6]`; sole typed assignment is ctor `=0` |
| Clarified `+0xc` on pack entries vs `CDSScript` | `CBulanci_EnumerateLevelScripts@0x00409f60`: `*(iVar2+0xc)==0x7ea` on **resource row**, not script object |
| Updated `CDSScript.md` UNK | Documented reserved fields + scriptData gap |

## Ghidra deltas (post-apply)

**CAdvertising** (144 B): `updatedItem@0x70` (`CDSUpdatedItem`), `pPad_preDismiss@0x88`, `m_bBlockDismiss@0x8c`.

**CDSScript** (1072 B): `dwReserved_0c@0x0c`, `dwReserved_14@0x14`; tail `framePtr@0x42c` unchanged.

## Remaining UNK

| Struct | Item | Notes |
|--------|------|-------|
| `CDSScript` | `+0xe0..+0x42b` padding | Subclass opcode extensions / `CLevelScript` tail; not base-class fields |
| `CDSScript` | `scriptData` @ `+0x18` | Dtor-free path only; no proven loader store in typed decompile |
| `CDSScript` | MI vtable slots beyond base ctor | Filled by `CLevelScript` / `CHistoryScript` / `CHelpScript` ctors |
| ~~`CAdvertising`~~ | ~~`+0x08..+0x6f`~~ | **Done** (R3 todo 3): embedded `CWindow win` @ 0 — see `CAdvertising.md` |
| `CAdvertising` | Heap children | White backdrop `0x6c`, splash `CDSBitmap` `0x78` via `CDSView__AddChild` |

## Handoff

Batch 02 size proofs stand. Next manifest index remains **3** (`CAnim`, `CBitmap`). Downstream batches referencing `CDSScript` padding or `CAdvertising.updatedItem` can use the applied Ghidra types.
