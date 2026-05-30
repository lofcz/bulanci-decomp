# Struct recovery — batch 27 follow-up (round 2)

**Index:** 27 (`batches_50.json`)  
**Prior:** [batch_27_summary.md](./batch_27_summary.md)  
**Status:** **HANDOFF_IMPROVED**  
**Ghidra:** `bulanci.exe` saved after `CDSDirectSound` struct edits

## Actions taken

| Struct | Prior | After | Ghidra delta |
|--------|-------|-------|----------------|
| `CDSCollection` | VERIFIED | VERIFIED (unchanged) | No edits — prior UNK list was empty |
| `CDSDirectSound` | PARTIAL | **VERIFIED** | `m_updatedItem` → nested `CDSUpdatedItem`; renamed `+0x38`/`+0x3c` COM fields; added `+0x40..+0x48` PCM format cache |

## Evidence resolved (from batch 27 UNK)

1. **`+0x04..+0x1b` (`CDSUpdatedItem`)** — cross-batch `CDSUpdatedItem` (batch 41/42) now embedded at `+0x04` instead of `byte[24]`.
2. **`m_field_38` / `m_field_3c`** — `CDSDirectSound_InitPrimary` is invoked on `CDSApp+0x200` (the embedded singleton), writing `IDirectSound*` at `+0x38` and primary buffer at `+0x3c`; consumed via `g_pDirectSoundSingleton+0x38` in `CDSAudioPlayer_Init`, `CDSDirectSound_DuplicateSoundBuffer`, etc.
3. **Tail gap `+0x40..+0x4b`** — `InitPrimary` stores `0` at `+0x40`, channel count / bits / sample rate at `+0x44`/`+0x46`/`+0x48` (call site `2`, `0x10`, `0x5622` @ 0x0042afd0).
4. **Mid padding `+0x22..+0x23`, `+0x35..+0x37`, `+0x4d..+0x4f`** — Ghidra auto-padding between proven fields; no standalone consumers.

## Remaining UNK

- `m_reserved_40` (+0x40): init-only zero, no readers.

## Agent todo 25 (2026-05-30)

- **Call sites:** `CDSApp_OnCreate` @ `0x0042a2fa` passes `LEA ECX,[app+0x200]` to `CDSDirectSound_InitPrimary`; `CDSApp_ctor` @ `0x0042b27d` same offset for `CDSDirectSound_ctor`. Ghidra `CDSApp::directSound` embed confirmed @ `+0x200`.
- **InitPrimary @ `0x0043cbc0`:** signature uses `CDSDirectSound *this`; plate + EOL comments. Decompiler body still maps `this` through `CDSApp` fields (class member + ECX retype API limit).

## Agent todo 26 (2026-05-30)

- **Resolved:** `SetEvent` canonical handle at `+0x50` (`m_hWorkerEvent`); `+0x4c` is `m_bShutdown`. `SignalEventIfVoicesActive` @ `0x0043cce1` uses `this+0x4c` in asm (off-by-4 vs ctor/dtor/worker). Ghidra: decompiler + plate comments; struct fields renamed `m_bShutdown` / `m_hWorkerEvent` (`HANDLE`).

## Deliverables updated

- [CDSDirectSound.md](./CDSDirectSound.md)
- [CDSCollection.md](./CDSCollection.md) — unchanged
