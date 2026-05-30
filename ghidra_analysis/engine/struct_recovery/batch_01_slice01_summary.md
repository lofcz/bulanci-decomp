# Struct recovery slice 01/50

**Program:** `bulanci.exe`  
**Structs:** `CDSApp`, `CGameView` (`batches_50.json` index 1)  
**Saved:** `save_program bulanci.exe`

## Results

| Struct | Status | Runtime size proof | Ghidra struct size (post-apply) |
|--------|--------|-------------------|--------------------------------|
| `CDSApp` | PARTIAL | **`0x284`** prefix in `CBulanci` (before `pGameEmbed`) | **644 B** — shell fields + `CDSBackBuffer` @ `+0x7c`, `CDSDirectSound` @ `+0x200` |
| `CGameView` | PARTIAL | **`0x98`** / **`0x88`** shell | **152 B** — mirrors `CBitmap`; `ODSImage` @ `+0x88` |

## Ghidra actions

- `delete_data_type` — 1-byte `CDSApp`, `CGameView`
- `create_struct CGameView` — 30 fields, 152 B
- `create_struct CDSApp` — 44 fields, 644 B
- `set_function_prototype` — `CDSApp_ctor@0x0042b170`, `CGameView_ctor@0x004191a0` (`CBitmap *` / `void * this`)
- `rename_function_by_address` — `CGameView_InitGamingFields@0x00416590`
- `save_program bulanci.exe`

## Files changed

- `struct_recovery/CDSApp.md` (new)
- `struct_recovery/CGameView.md` (Ghidra apply section)
- `gameplay_struct_backlog.md` (slice 01 row)
- `struct_recovery/batch_01_slice01_summary.md` (this file)

## Blockers

- **`CGameView *`** not resolved in prototype API (ctor still `void * this`).
- ~~**`CBulanci`** flat layout not reparented~~ — **done** (agent todo 2): `CDSApp app` @ 0.
- **`CDSApp+0x100..+0x1ff`** opaque (`pPad_100` in Ghidra).
- **`CDSDirectSound`** Ghidra size 84 B vs historical 0x54 documentation drift.
