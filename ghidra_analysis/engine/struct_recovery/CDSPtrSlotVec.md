# CDSPtrSlotVec

## Status

**VERIFIED** — 8-byte `{pSlots, cCapacity}` heap-resize helper object used wherever `CDSPtrSlotVec_Resize` @ `0x00406340` is called with `this` pointing at an embedded field.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 8` | `CDSPtrSlotVec_Resize` @ `0x00406340` | Reads/writes `*(void**)this` and `*(int*)(this+4)`; realloc via `FUN_0042f730(pSlots, newCapacity * 4)` |
| Embedded in `CDSImage` | `CDSImage_dtor` @ `0x004254f0` | `CDSPtrSlotVec_Resize(&this->m_slotVector, 0)` after `nField_40 = 0` |
| Embedded in `CDSAudioBank` | `CDSAudioBank_dtor` @ `0x0042943f` | `ResizeSlots(bank+0x18, 0)` (same helper, was mis-tagged `CDSAudioBank_ResizeSlots`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pSlots` | `CDSPtrSlotVec_Resize@0x00406340` — passed to `FUN_0042f730` |
| `0x04` | 4 | `int` | `cCapacity` | `CDSPtrSlotVec_Resize@0x00406340` — compared to `newCapacity`, updated after grow/shrink |

## Ghidra apply

```
create_struct CDSPtrSlotVec fields=[
  {"name":"pSlots","type":"void *","offset":0},
  {"name":"cCapacity","type":"int","offset":4}
]
rename_function 0x00406340 → CDSPtrSlotVec_Resize
prototype: void __thiscall CDSPtrSlotVec_Resize(CDSPtrSlotVec *this, int newCapacity)
```

**Agent todo 35 r2 (2026-05-30):** `set_function_prototype@0x00406340` — cleared stale `CDSAudioBank * this` in signature string.

**Agent todo 35 r4 (2026-05-30):** `set_function_this_type` → `CDSPtrSlotVec *`; `CDSImage_dtor` callee decompiles as `CDSPtrSlotVec::CDSPtrSlotVec_Resize` (see [round4_task_35_report.md](./round4_task_35_report.md)).

## UNK

- None for layout; generic helper also used from `CGame`, `CHelpDlg`, `CBulanci_BuildBitmapCache`, etc. (see xrefs to `0x00406340`).
