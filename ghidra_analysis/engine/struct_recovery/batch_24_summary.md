# Struct recovery batch 24/50 (agent slice 24)

**Index:** 24 (`batches_50.json`)  
**Structs:** `CDSAudioVideoPlayer`, `CDSBackBuffer`  
**Program:** `bulanci.exe` (saved)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSAudioVideoPlayer` | **VERIFIED** | **0x50 (80)** | 80 B struct; nested `CDSVideoPlayer` @ `+0x08`; leaf fns renamed |
| `CDSBackBuffer` | **VERIFIED** | **0x50 (80)** @ `CDSApp+0x7c` | 80 B + `CDSImage_BackBufferEmbed` 76 B; flip/dtor prototypes applied |

## Ghidra actions (slice 24)

- `set_function_prototype`: `CDSBackBuffer_Flip`, `CDSBackBuffer_FreeImageMember`, `CDSBackBuffer_ClearPreFlipFields`
- `rename_function_by_address`: `CDSAudioVideoPlayer_Constructor`, `_SetupTrack`, `_Play`, `CDSBackBuffer_dtor`
- `get_struct_layout`: confirmed 80 B layouts (no struct recreate needed)
- `save_program`

## Blockers

- ~~Duplicate Ghidra type `/CDSAudioVideoPlayer/CDSAudioVideoPlayer` (size 1)~~ **closed** (agent todo 23): nested 1 B type deleted; ctor/dtor/scalar-dtor use `CDSAudioVideoPlayer *`; embed `videoTrackManager` is `byte[72]` pending `CDSVideoPlayer` 72 B restore (todo 44).

## Deliverables

- `CDSAudioVideoPlayer.md`, `CDSBackBuffer.md` (VERIFIED + function tables)
- This summary
