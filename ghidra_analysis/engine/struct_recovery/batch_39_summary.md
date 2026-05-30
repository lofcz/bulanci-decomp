# Struct recovery batch 39/50

**Index:** 39 (`batches_50.json`)  
**Types:** `CDSSafeStream`, `CDSSafeStreamInfo`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CDSSafeStream` | PARTIAL | `0x48` | Thread-safe stream wrapper; MI vtables `+0x00..+0x14`, chain `+0x18`, lock `+0x2c`, name `+0x44` |
| `CDSSafeStreamInfo` | VERIFIED | `0x18` | Per-thread node on parent chain; `threadId` `+0x10`, `m_streamSlice` `+0x14` |

## Key evidence

- **Alloc `0x48`:** `OperatorNewWithBadAlloc(0x48)` in `CBulanci::FUN_00434160` (`0x004341b2`) and `FUN_00434760` (`0x0043483a`) → `CDSSafeStream_ctor` (`0x00433ab0`).
- **Alloc `0x18`:** `OperatorNewWithBadAlloc(0x18)` in `FUN_00446ea0` / `FUN_00446d90` for chain nodes.
- **External handle:** Callers keep `CDSSafeStream_ctor` return `+ 4` (stream interface at `+0x04`).
- **Thread binding:** `FUN_00446d90` walks `CDSSafeStream+0x18`, matches `CDSSafeStreamInfo+0x10` to `GetCurrentThreadId()`, returns slice at `+0x14`.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CDSSafeStream.md`
- `ghidra_analysis/engine/struct_recovery/CDSSafeStreamInfo.md`
- Ghidra: layouts already at 72 / 24 bytes; `get_struct_layout` confirmed; `save_program bulanci.exe` at batch end.

## Follow-ups

- Recover `CDSChain` / `CDSFilterStream` / `CDSQueueStream` bases (batches 30–36) to name MI parents and `nM_chain_count` semantics.
- Map `dwM_streamFlags` (`0x20` vs `1`) with a consumer beyond ctor/`FUN_00446ea0`.
