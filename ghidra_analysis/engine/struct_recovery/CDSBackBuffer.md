# CDSBackBuffer

## Status

**VERIFIED** — 80 B embed @ `CDSApp+0x7c`; `CDSImage_BackBufferEmbed` @ `+0x04` (76 B). Tail `CDSImage` MI vtables (`+0x4c..+0x5f` on 96 B heap `CDSImage`) **absent** on embed (slice 24 + task 35).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Embedded in `CDSApp` at **+0x7c** | `0x0042b170` (`CDSApp_ctor`) | Writes `g_pCDSBackBuffer_vftable_*` at `this+0x7c` / `this+0x80` |
| Embedded span **0x50 (80)** | `0x0042b170` | Ctor next clears `this+0xcc` (DD client rect); `0x7c + 0x50 = 0xcc` |
| Not heap-allocated | — | Only embedded in `CDSApp`/`CBulanci`; `DtorScalar` supports `delete` but shipping path uses embedded subobject |
| Max field **+0x4c** (back-buffer base) | `0x00429930` | `CDSBackBuffer_Flip`: surface at `param_1+0x4c` (= embed `pDirectDrawSurface` @ image `+0x48`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `pointer` | `vftable_IDSReferenced` | `CDSApp_ctor` @ `0x0042b170`; `CDSBackBuffer_dtor` @ `0x0042ad40` |
| 0x04 | 0x4c | `CDSImage_BackBufferEmbed` | `embeddedImage` | `CDSImage_dtor(&embeddedImage)` @ `0x0042ad40`; `CDSBackBuffer_ClearPreFlipFields(&embeddedImage)` @ `0x00436750` |
| 0x08 | — | *(MI: `embeddedImage.pVf_primary`)* | — | `g_pCDSBackBuffer_vftable_IDSEventHandler` @ `CDSApp_ctor` `pPad_30+0xc` / dtor patch @ `0x0042ad40` |
| 0x20 | 4 | `void *` | `embeddedImage.pM_pixels` | `CDSBackBuffer_FreeImageMember` tests `*(param_1+0x20)` @ `0x00429960` |
| 0x48 | 4 | `int` | `embeddedImage.nField_44` | `CDSApp_ctor` `*(CDSApp+0xc4)=8` → image `+0x44` |
| 0x4c | 4 | `void *` | `embeddedImage.pDirectDrawSurface` | `CDSBackBuffer_Flip` @ `0x00429930` (`param_1+0x4c`); `CDSBackBuffer_FreeImageMember` @ `0x00429960` |

## Tail MI vtables (verified absent)

Heap `CDSImage` (96 B) stores five tail slots at **`+0x4c..+0x5f`**: `vf_IDSChained`, `refcount`, `vf_streamHost`, `vf_event`, `m_chain` (`CDSImage_InitDefaults` @ `0x00425580`; `CDSImage_ReleaseRefcount` @ `0x004322f0`).

The back-buffer embed cannot host them:

| Check | Result |
|-------|--------|
| Size budget | `CDSBackBuffer` 80 B = 4 B parent vftable + **76 B** `CDSImage_BackBufferEmbed` — no bytes after embed `pDirectDrawSurface` @ **+0x48** (embed ends at `CDSBackBuffer+0x50`) |
| Ctor | `CDSApp_ctor` @ `0x0042b170` patches only `g_pCDSBackBuffer_vftable_IDSReferenced` / `IDSEventHandler`; **does not** call `CDSImage_InitDefaults` |
| Dtor / free | `CDSImage_dtor` @ `0x004254f0` (via `CDSBackBuffer_dtor`) frees buffers/slots only; **no** `ReleaseRefcount` |
| `+0x4c` consumer | `CDSBackBuffer_Flip` / `FreeImageMember` use **`param_1+0x4c`** as **IDirectDrawSurface** COM pointer, not `vf_IDSChained` |
| Refcount thunks | `CDSImage_ReleaseRefcount` xrefs only on heap BMP/JPEG stream-host vtables (`0x004871e8` …), not `g_pCDSBackBuffer_vftable_*` |

**Naming note:** Back-buffer **`CDSBackBuffer+0x4c`** equals **`embeddedImage+0x48`** (`pDirectDrawSurface`). That collides with the *name* `CDSImage+0x4c` (`vf_IDSChained`) on heap images — different layout contract.

## Leaf functions (slice 24)

| Address | Symbol | Role |
|---------|--------|------|
| `0x00429930` | `CDSBackBuffer_Flip` | `ClearPreFlipFields` + IDirectDrawSurface flip (`embeddedImage.pDirectDrawSurface`) |
| `0x00429960` | `CDSBackBuffer_FreeImageMember` | Flip-if-pixels; release DDraw surface; `CDSImage__FreeBuffers` |
| `0x00436750` | `CDSBackBuffer_ClearPreFlipFields` | Zero `pM_pixels` / `pM_auxBuffer` before flip |
| `0x0042ad40` | `CDSBackBuffer_dtor` | Free image + restore vtables |
| `0x0042adb0` | `CDSBackBuffer_DtorScalar` | Scalar deleting wrapper |
| `0x0042b170` | `CDSApp_ctor` | Install vtables @ app `+0x7c/+0x80`; `nField_44=8` on embed |
| `0x0042a330` | `CBulanci_RebuildBackBufferSurface` | Rebind surface into embed |

## Ghidra apply

```
get_struct_layout CDSBackBuffer → Size: 80
get_struct_layout CDSImage_BackBufferEmbed → Size: 76
Prototypes: CDSBackBuffer_Flip/FreeImageMember (CDSBackBuffer*); ClearPreFlipFields (CDSImage_BackBufferEmbed*)
Renamed: CDSBackBuffer_dtor @ 0x0042ad40
save_program bulanci.exe (slice 24)
```

## RTTI / vtable

| Vtable | Address | `this` adjust | Role |
|--------|---------|---------------|------|
| `g_pCDSBackBuffer_vftable_IDSEventHandler` | `0x00486f9c` | `+0x7c` in `CDSApp` | 4 slots — **embed** `pVf_primary` / IDSEventHandler MI |
| `g_pCDSBackBuffer_vftable_IDSReferenced` | `0x00486fb0` | `+0x80` in `CDSApp` | 3 slots — **parent** `vftable_IDSReferenced` |

Not present on embed: `g_pCDSImage_vftable_IDSChained`, stream-host, event, or chain slots from `CDSImage_InitDefaults`.

## Call graph (consumers)

- `CDSApp_RenderFrame` → `CDSBackBuffer_Flip((int)(param_1+0x1f))` = `CDSApp+0x7c` @ `0x0042bae0` region
- `CBulanci_RebuildBackBufferSurface(CDSBackBuffer*)` @ `0x0042a330` (stores surface at `pPad_30+4` / image pixels path)
- `FUN_00429a40` → `CDSBackBuffer_FreeImageMember(this+0x7c)` on display teardown

## UNK

- `embeddedImage.nField_40` / slot vector semantics — see [CDSImage.md](./CDSImage.md) § UNK.
- `pDirectDrawSurface` at embed `+0x48` vs standalone `CDSImage::nDefaultFormatTag` (`8`) — documented in [round3_task_35_report.md](./round3_task_35_report.md).

## Follow-up

- Round 3 task 35 (CDSImage tags + MI): [round3_task_35_report.md](./round3_task_35_report.md)
- Batch 24: [batch_24_followup_summary.md](./batch_24_followup_summary.md)
