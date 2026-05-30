# Round 5 — worker 31 report (`CDSImage` MI / slot UNKs)

## 1. Task

| Field | Value |
|-------|--------|
| **worker** | 31 / 50 |
| **scope** | Close `CDSImage.md` **MI facet** and **`+0x38..+0x40` slot** UNKs (evidence-only) |
| **types** | `CDSImage`, `CDSPtrSlotVec`, `ODSImage`, `CDSBmpImage` |
| **mode** | WRITE — Ghidra MCP |
| **related manifest** | R5 todo **31** (FLX `BroadcastFrameTimeHint` / `ODSImage__SetImage` subscribers); R5 todo **35** (scalar-deleting dtor graph — deferred) |

**Seed addresses:** `0x004254f0`, `0x004360d0`, `0x004360f0`, `0x00436d50`, `0x00436ef0`, `0x00439100`, `0x00483728`, `0x00483740`, `0x0048375c`, `0x00487208`

**Inputs:** [CDSImage.md](./CDSImage.md), [CDSPtrSlotVec.md](./CDSPtrSlotVec.md), [pass_r4_CDSImage_report.md](./pass_r4_CDSImage_report.md), [bmp_decoder.md](../../formats/bmp_decoder.md), `master_vtable_catalog.csv`

## 2. Status

**DONE** (slot band + MI slot maps + plane helpers) — **`PARTIAL`** on full scalar-deleting dtor **call graph** (catalogued slot indices; adjustor wiring left for R5 task 35).

## 3. Evidence

### `m_slotCount` @ `+0x40` (was `nField_40`)

| Claim | Address | Evidence |
|-------|---------|----------|
| CIntList count field | `CIntList_BinarySearch@0x00401d…` | When `param_3 == -1`, uses `*(int *)((int)this + 8)` as bound |
| Image list head | `ODSImage__SetImage@0x00439100` | `CIntList_BinarySearch(&pDrawable->m_slotVector, …, pDrawable->m_slotCount)`; `CIntListInsertSortedOrAppend(&pObject->m_slotVector, &this->pVf_odsimage, …, 1)` |
| Remove decrements count | `CDynPtrArray_RemoveRange@0x004032…` | `*(int *)((int)this + 8) -= param_2` with `this = &m_slotVector` |
| FLX fan-out | `BroadcastFrameTimeHint@0x00436ef0` | `for (i = *(int *)(this+0x40); i; …)` walks `*(void **)(this+0x38)` |
| Init / teardown | `CDSImage_ctor@0x00425460`, `CDSImage_InitDefaults@0x00425580`, `CDSImage_dtor@0x004254f0` | `m_slotCount = 0`; dtor clears count then `CDSPtrSlotVec_Resize(&m_slotVector, 0)` |
| **Not** slot count on embed | `CMenu_LoadBackgroundMusic` | `CDSBackBuffer.embeddedImage+0x40` stores `CDSAudioPlayer *` — different 76 B layout |

### Subscriber registration model

| Subscriber key | Stored value | Consumer vfn |
|----------------|--------------|--------------|
| `ODSImage+4` (`pVf_odsimage`) | Inserted into drawable `m_slotVector` | `BroadcastFrameTimeHint` → `CALL [obj+0x10](CDSImage*, u16)` — e.g. `CGunMouse_OnAnimTick`, `CWeapon_Fire`, `CBitmap` event row (`0x00483794`, `0x00481f00`, … per `anim_runtime.md`) |

### MI vtable slots (standalone `CDSImage`)

| Image off | Vtable | Slot | Function | Role |
|-----------|--------|------|----------|------|
| `+0x54` | `0x00483740` | 4 | `CDSImage_Load@0x00437160` | Stream-host MI (`ECX = image+0x54`) |
| `+0x54` | `0x00483740` | 5 | `CDSImage_Save@0x00436c60` | Same adjustor family |
| `+0x54` | `0x00483740` | 3 | `0x00437520` | Scalar-deleting dtor thunk (−0x54) |
| `+0x4c` | `0x00483728` | 3 | `0x00437530` | Scalar-deleting dtor thunk (−0x4c) |
| `+0x58` | `0x0048375c` | 3 | `0x00437540` | Scalar-deleting dtor thunk (−0x58) |
| `+0x54` BMP | `0x00487208` | 4–5 | `CDSBmpImage_LoadDibStream` / `SaveDibStream` | Wrapper replaces generic Load/Save |

### Plane helpers (`__thiscall` proof)

| Function | Address | Typed access after fix |
|----------|---------|------------------------|
| `GetPaletteBuffer` | `0x004360d0` | `this->pM_pixels` gate + `nM_paletteEntries` bounds |
| `GetColorPlane` | `0x004360f0` | `pM_pixels + nM_paletteEntries*4` when `1..0x100` palette |
| `CDSImage__FreeBuffers` | `0x00436d50` | Member of `CDSImage` namespace |

## 4. Ghidra deltas

- `modify_struct_field` — `CDSImage.nField_40` → `m_slotCount`
- `set_function_prototype` + `set_function_this_type` — `GetPaletteBuffer@0x004360d0`, `GetColorPlane@0x004360f0`, `CDSImage__FreeBuffers@0x00436d50` → `CDSImage *`
- `set_decompiler_comment` — `BroadcastFrameTimeHint@0x00436ef0`, `ODSImage__SetImage@0x00439100`, `CDSImage_dtor@0x004254f0`
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CDSImage.md](./CDSImage.md) — `m_slotCount`, subscriber + MI vtable sections; UNK trimmed

## 6. Remaining UNK

- Scalar-deleting dtor thunk **edges** (which thunk dispatches to which primary dtor) — R5 task **35** / [bmp_decoder.md](../../formats/bmp_decoder.md)
- Non-zero `CDSPtrSlotVec_Resize` growth on heap `CDSImage` outside dtor (subscribers grow via `CIntList_EnsureCapacity`, not image `Resize` xref)
- `CDSObject` embed `+0x50` refcount vs `vf_IDSChained` overlap
