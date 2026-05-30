# Struct recovery batch 37 — follow-up round 2

**Status:** `FOLLOWUP_DONE`  
**Prior:** [batch_37_summary.md](./batch_37_summary.md)  
**Deliverables reviewed:** [CDSResourceException.md](./CDSResourceException.md), [CDSResourceSign.md](./CDSResourceSign.md)  
**Program:** `bulanci.exe`

## Prior batch follow-ups

| Item | Resolution |
|------|------------|
| Resolve `CDSException` base `+0x14..+0x3B` on `CDSResourceException` | **Done** — cross-batch evidence: `CDSException_GetMessageW@0x00434b80` uses `this+0x14`; `CDSSimpleException` / batch 36 define `wchar_t[20] pInlineMessage` through `0x3C`; subclass tail `dwResourceId` / `pszFormatted` at `0x3C` / `0x40` unchanged |
| Reconcile `CDSResourceSign_dtor` `param_1[8]` vs `flagByte` at `+0x20` | **Done (doc)** — same offset; dtor `CDsStringReleaseHeader` is a false third-string from untyped `undefined4*` indexing; stream/ctor plate + shipping `flagByte == 0` → release never runs |

Note: batch 37 summary cited "batch 29" for `CDSException`; batch 29 recovered stream types, not exceptions. Base layout comes from **batch 36** (`CDSSimpleException`, `CDSStreamException`, `CDSException_InitFields`).

## Actions taken

| Action | Result |
|--------|--------|
| Decompile `CDSException_InitFields`, `CDSException_GetMessageW`, resource exception/sign ctors/dtors, stream I/O | Confirmed shared prefix and sign colophon layout |
| `modify_struct_field` on `CDSResourceException` | Replaced `pUnkBaseTail` → `pInlineMessage` (`wchar_t[20]`); renamed prefix fields to match batch 36 |
| `get_struct_layout CDSResourceException` | 68 bytes, 9 named fields — matches `0x44` alloc proof |
| `get_struct_layout CDSResourceSign` | Unchanged (40 bytes); already correct |
| Updated struct markdown deliverables | `CDSResourceException` → **VERIFIED**; `CDSResourceSign` follow-up note on `+0x20` |
| `save_program bulanci.exe` | Once |

## Ghidra deltas

- **CDSResourceException:** `pUnkBaseTail` / `undefined1[40]` @ `+0x14` → `pInlineMessage` `wchar_t[20]`; field renames (`pVftable`, `bDeleteOnRelease`, `pMessageCache`, `dwCodePrimary`, `dwStaticTextIndex`).
- **CDSResourceSign:** no type-manager change.

## Remaining UNK

| Struct | Item |
|--------|------|
| `CDSResourceException` | `CDSResourceException_What` format template id; secondary vtable slot semantics |
| `CDSResourceSign` | `+0x18` malloc-only zero-init; MI sub-object nested structs |

## Handoff

Batch 37 follow-up queue is empty. Optional: apply `CDSResourceException` typed `this` on `FUN_004346f0` / factory paths (currently mis-typed as `CDSStreamStorage` in decompiler).
