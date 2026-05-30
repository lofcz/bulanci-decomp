# Round 3 — Task 30 report

## Task

| Field | Value |
|-------|--------|
| **id** | 30 |
| **title** | Name CDSFilterStream post-BindSource dword; dedup CDSSimpleException vs CDSException embed |
| **types** | `CDSFilterStream`, `CDSException`, `CDSSimpleException`, `CDSStreamStorage` |
| **addresses** | `0x00430ca0`, `0x00430420`, `0x00434760`, `0x00434c20`, `0x00401790` |
| **acceptance** | Update `CDSFilterStream.md` / `CDSSimpleException.md` / `CDSException.md`; Ghidra mutation if evidence supports |

**Handoff (R2 todo 30):** `CDSFilterStream::field_10` — ctor `0x20` only in follow-up scope; **BindSource** copies inner `IDSStream+4`.

## Status

**DONE**

## Evidence

### CDSFilterStream `+0x10` → `dwIdsStream_state`

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Ctor default closed sentinel | `CDSFilterStream_Ctor@0x00430dd0` | `this->dwIdsStream_state = 0x20` before `BindSource` |
| Post-bind snapshot from inner stream | `CDSFilterStream_BindSource@0x00430ca0` | `this->dwIdsStream_state = *(uint *)((int)this->pInnerStream + 4)` |
| Assembly store | `CDSFilterStream_BindSource@0x00430ce5` | `MOV EDI,[ECX+0x4]` (`ECX` = `pInnerStream`); `MOV [ESI+0x10],EDI` |
| Alloc / wrap path | `CDSStreamStorage_CreateFilterSafeStream@0x00434760` | `OperatorNewWithBadAlloc(0x38)` → `CDSFilterStream_Ctor`; `CDSSafeStream_ctor` takes `&filter->pVftable_IDSStream` (`filter+0x0c`) |
| Read path uses IDSStream plate | `CDSFilterStream_ReadBytes@0x00430420` | Cursor/cap at `IDSStream+0x0c/+0x10/+0x1c/+0x20` (outer `+0x18..+0x2c`); does not read filter `+0x10` |

Semantics: same **IDSStream lifecycle dword** at interface `+4` documented in `stream_hierarchy.md` §2.2 (`0x20` = closed/default; inner stream may carry `7` = open after backing init on mem streams).

### CDSSimpleException vs `CDSException` dedup

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Leaf alloc `0x3C` | `CDSSimpleException_Throw@0x00434c20` | `OperatorNewWithBadAlloc(0x3c)`; no tail past base |
| Shared prefix init | `CDSException_InitFields@0x00434a40` | Writes `pVftable`, codes, `pInlineMessage` span through `+0x3b` |
| Subclass vtable patch | `CDSSimpleException_Throw@0x00434c20` | After init: `local_4->pVftable = 0x48754c` |
| Ghidra embed only | `get_struct_layout("CDSSimpleException")` | Single component **`CDSException base` @ `+0`**, 60 B — no parallel flat field list |
| Canonical flat names | `get_struct_layout("CDSException")` | `pVftable`, `bDeleteOnRelease`, `pPad_05`, `pMessageCache`, `dwCodePrimary`, `dwStaticTextIndex`, `pInlineMessage` |

**Verdict:** `CDSSimpleException` is a **vtable-specialized leaf** of the `CDSException` prefix; Ghidra uses embedded `CDSException base` (same pattern as `CDSApiException` task 27). No `delete_data_type` on a duplicate flat struct required.

## Ghidra deltas

- Renamed `CDSFilterStream.dwField_10` → **`dwIdsStream_state`** @ outer `+0x10`.
- Decompiler comments @ `CDSFilterStream_BindSource` (store), `CDSFilterStream_Ctor` (default `0x20`), `CDSStreamStorage_CreateFilterSafeStream`, `CDSSimpleException_Throw` (embed / alloc).
- Verified `get_struct_layout`: `CDSFilterStream` 56 B; `CDSSimpleException` 60 B embed; `CDSException` 60 B flat canonical.
- `save_program bulanci.exe` (once).

## Struct doc updates

- [CDSFilterStream.md](./CDSFilterStream.md) — `dwIdsStream_state` row + agent todo 30 note.
- [CDSSimpleException.md](./CDSSimpleException.md) — Ghidra embed layout; dedup note.
- [CDSException.md](./CDSException.md) — cross-ref `CDSSimpleException` embed @ `+0`.

## Remaining UNK

- `CDSFilterStream_ReadBytes` still types outer `this` instead of `IDSStream*` plate at `+0x0c` (decompiler hygiene).
- `dwIdsStream_state` on filter is **write-only** in filter methods (snapshot only); consumers use inner stream or sibling stream types.
- `CDSSimpleException` `__swprintf` bound (format-string table dependency) — unchanged from batch 36.
