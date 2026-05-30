# Round 5 worker 9/50 — CDSAudio / DirectSound / WAV / MPx FUN_* band

## Task

| Field | Value |
|-------|-------|
| Worker | 9 / 50 |
| Mode | WRITE — evidence-only renames / `create_function` |
| Band | `0x00470000`–`0x004B0000` |
| Theme | `CDSAudio*`, `CDSDirectSound`, `CDSWav*`, `CDSMpx*` static init + paired `_atexit` stubs |
| Program | `bulanci.exe` |

## Status

**DONE** — audio class-registration cluster in `0x0047c4b0`–`0x0047da64` fully named; **259** total `FUN_*` in band (mostly non-audio CRT `_atexit` placeholders `RET`-only in `0x0047de40`–`0x0047ee90`). `save_program bulanci.exe`.

## Band inventory

| Region | `FUN_*` count | Notes |
|--------|---------------|-------|
| `0x00470000`–`0x0047c000` | 2 | `FUN_004713d0` (zlib `build_tree` callee), `FUN_00472420` (zlib `send_bits` callee) — **not audio** |
| `0x0047c4b0`–`0x0047da64` | 0 (after) | Audio / AV static `HandleClassRegister` + `HandleInterfaceRegister` + `_atexit` pairs |
| `0x0047de40`–`0x0047ee90` | ~250 | Generic `_atexit` stubs (`PUSH imm; CALL 0x00447e72`); body `RET` only — left unnamed (non-audio) |

## Evidence — audio static registration (func@addr)

| Address | Symbol (after) | Proof |
|---------|----------------|-------|
| `0x0047c4b0` | `CDSAudioBank_StaticClassRegister` | `PUSH 0x00429470`; `PUSH 0x4b7bfc`; `PUSH 0x43` (67); `MOV ECX,0x4b3ae0`; `CALL HandleClassRegister@0x0042e910` |
| `0x0047c4e0` | `CDSAudioBank_TypeinfoAdjust4_StaticRegister` | `PUSH 0x0042fd30` (+4 adjust); `CALL HandleInterfaceRegister@0x0042e980`; `_atexit` → `0x0047e560` |
| `0x0047c510` | `CDSAudioBank_TypeinfoAdjust12_StaticRegister` | `PUSH 0x0043bdd0` (`ADD EAX,0xc`); parent `0x4b3ae0`; `_atexit` → `0x0047e570` |
| `0x0047cec0` | `CDSMpxStream_StaticClassRegister` | `PUSH 0x00433110`; `PUSH 0x4b8510`; `PUSH 0x30`; `MOV ECX,0x4b7e4c`; `CALL HandleClassRegister` |
| `0x0047cef0` | `CDSMpxStream_TypeinfoAdjust4_StaticRegister` | `PUSH 0x00428980` (+4); meta `0x4b7e60`; `_atexit` → `0x0047e910` |
| `0x0047d750` | `CDSVideoPlayer_StaticClassRegister` | (pre-named) `PUSH 0x00439f50`; `PUSH 0x31`; meta `0x4b834c` — AV player factory |
| `0x0047d780` | `CDSVideoPlayer_TypeinfoAdjust4_StaticRegister` | `PUSH 0x0042fd30`; meta `0x4b8360`; `_atexit` → `0x0047ec10` |
| `0x0047d9b0` | `CDSWavStream_TypeinfoAdjust18_StaticRegister` | `PUSH 0x004465d0` (`ADD EAX,0x18` → `IDSEventHandler` @ `+0x18`); meta `0x4b8430`; `_atexit` → `0x0047ed10` |
| `0x0047d9e0` | `CDSWavStream_StaticClassRegister` | `PUSH 0x0043bb00`; `PUSH 0x2b` (43); meta `0x4b843c`; `_atexit` → `0x0047ed20` |
| `0x0047da10` | `CDSWavStream_TypeinfoAdjust4_StaticRegister` | `PUSH 0x00428980`; meta `0x4b8450`; `_atexit` → `0x0047ed30` |
| `0x0047da40` | `CDSWavStream_TypeinfoAdjust34_StaticRegister` | `PUSH 0x0043baa0` (`ADD EAX,0x34` → `IDSChained6` @ `+0x34`); meta `0x4b845c`; `_atexit` → `0x0047ed40` |

### Paired `_atexit` stubs (renamed from `FUN_*`)

| Address | Symbol | Registered from |
|---------|--------|-----------------|
| `0x0047e550` | `CDSAudioBank_StaticClassRegister_atexit` | `0x0047c4c6` |
| `0x0047e560` | `CDSAudioBank_TypeinfoAdjust4_StaticRegister_atexit` | `0x0047c4f9` |
| `0x0047e570` | `CDSAudioBank_TypeinfoAdjust12_StaticRegister_atexit` | `0x0047c529` |
| `0x0047e900` | `CDSMpxStream_StaticClassRegister_atexit` | `0x0047ced6` |
| `0x0047e910` | `CDSMpxStream_TypeinfoAdjust4_StaticRegister_atexit` | `0x0047cf09` |
| `0x0047ec00` | `CDSVideoPlayer_StaticClassRegister_atexit` | `0x0047d766` |
| `0x0047ec10` | `CDSVideoPlayer_TypeinfoAdjust4_StaticRegister_atexit` | `0x0047d799` |
| `0x0047ed10` | `CDSWavStream_TypeinfoAdjust18_StaticRegister_atexit` | `0x0047d9c9` |
| `0x0047ed20` | `CDSWavStream_StaticClassRegister_atexit` | `0x0047d9f6` |
| `0x0047ed30` | `CDSWavStream_TypeinfoAdjust4_StaticRegister_atexit` | `0x0047da29` |
| `0x0047ed40` | `CDSWavStream_TypeinfoAdjust34_StaticRegister_atexit` | `0x0047da59` |

All `_atexit` bodies are **`RET` only** (process-exit placeholders; same pattern as `StaticDtor_CDSMemoryExceptionSingleton_atexit@0x0047ea50` which writes a vtable).

### Out-of-band factory (xref from in-band register)

| Address | Symbol | Proof |
|---------|--------|-------|
| `0x00429470` | `CDSAudioBank_Factory` | Sole DATA push from `CDSAudioBank_StaticClassRegister`; ctor stamps `0x486eec`/`0x486ed8`/`0x486ec0`/`0x486ea4` (bank facet vtables) |

## Ghidra deltas

- `create_function` — 11 static-register entry points (`CDSAudioBank`/`CDSMpxStream`/`CDSWavStream`/`CDSVideoPlayer` MI + class registers)
- `rename_function_by_address` — 11 `_atexit` stubs (table above)
- `set_plate_comment` — `CDSAudioBank_StaticClassRegister`, `CDSMpxStream_StaticClassRegister`, `CDSWavStream_StaticClassRegister`
- `create_function` @ `0x00429470` → `CDSAudioBank_Factory` (referenced from in-band register; factory below band)
- **Correction:** `0x0047cf50` briefly mis-labeled `CDSMpxMemQueue_*`; disasm proof (`PUSH 0x00433880`, vtables `0x0047f780` = `CDSFileStream`) → renamed `CDSFileStream_StaticClassRegister` (non-audio; left for worker covering file streams)
- `save_program bulanci.exe`

## Struct doc updates

- [CDSWavStream.md](./CDSWavStream.md) — static init / MI register addresses
- [CDSAudioBank.md](./CDSAudioBank.md) — class-67 register + factory name
- [CDSMpxStream.md](./CDSMpxStream.md) — registry shell static init

## Remaining UNK

- **CDSDirectSound** / **CDSAudioPlayer** — no `HandleClassRegister` in this band (embedded in `CDSApp` / heap `CreateObject` below `0x00470000`).
- **CDSMpx** full decoder (`CreateFromHandle@0x00446b00`, `0x98c8`) — runtime factory, not static-init band.
- **~240** generic `_atexit` stubs `0x0047de40`–`0x0047ee90` — one xref each from unrelated class static inits; not audio-themed.
- **zlib** helpers `FUN_004713d0` / `FUN_00472420` in `0x004712xx`–`0x004724xx` — callers `build_tree` / `send_bits` (deflate), not WAV/MPx decode.

## References

- [class_registry.py](../class_registry.py) — `HandleClassRegister` scan recipe
- [CDSAudioVideoPlayer.md](./CDSAudioVideoPlayer.md) — `CDSVideoPlayer_StaticClassRegister@0x0047d750`
- [master_vtable_catalog.csv](../master_vtable_catalog.csv) — `CDSFileStream` @ `0x0047f780`
