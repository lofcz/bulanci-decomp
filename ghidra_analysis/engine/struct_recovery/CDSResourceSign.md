# CDSResourceSign

## Status

**VERIFIED** — `sizeof == 0x28`; ctor, dtor, stream I/O, and factory `malloc(0x28)` agree on all material offsets.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSResourceSign) == 0x28` | `0x00434930` | `_Globals::InitializeAndAllocate` → `OperatorNewWithBadAlloc(0x28)` then `CDSResourceSign_ctor` |
| ClassID 94 factory | `0x0047d1d0` | `RegisterCDSResourceSignAsClass94` registers factory `0x00434930` |
| vtables (primary / +4 / +8 / +16) | `0x00434540` | ctor writes `0x487510`, `0x4874f4`, `0x4874dc`, `0x4874c8` at `+0`, `+4`, `+8`, `+0x10` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `pointer` | `vfptr_primary` | `CDSResourceSign_ctor@0x00434540` |
| 0x04 | 4 | `pointer` | `vfptr_sub4` | `CDSResourceSign_ctor@0x00434540` |
| 0x08 | 4 | `pointer` | `vfptr_sub8` | `CDSResourceSign_ctor@0x00434540` |
| 0x0C | 4 | `uint` | `chainField` | `CDSResourceSign_ctor@0x00434540` (`param_1[3] = 0`) |
| 0x10 | 4 | `pointer` | `vfptr_sub16` | `CDSResourceSign_ctor@0x00434540` |
| 0x14 | 4 | `uint` | `publishDate` | `CDSResourceSign_ctor@0x00434540` (`param_1[5]=1` then `CDate_SetDate(0,1,1)`); `CDSResourceSign_ReadDateField@0x0042e680` / `WriteDateField@0x0042e6a0` on `this+0x14`; `CDSResourceSign_Release@0x00434610` refcount on `+0x14` |
| 0x18 | 4 | `pointer` | `wszContent` | `CDSResourceSign_WriteToStream@0x00434310` / `ReadFromStream@0x004343a0` → `CDsString_*` at `this+0x18` |
| 0x1C | 4 | `pointer` | `wszCopyright` | `CDSResourceSign_WriteToStream@0x00434310` / `ReadFromStream@0x004343a0` at `this+0x1c`; dtor releases `param_1[7]` |
| 0x20 | 1 | `byte` | `flagByte` | `CDSResourceSign_WriteToStream@0x00434310` writes 1 byte at `this+0x20`; `ReadFromStream@0x004343a0` conditional 1-byte read; ctor `param_1[8]=0` zeros the dword at `+0x20` |

## Ghidra apply

```
Structure: CDSResourceSign
Size: 40 bytes

     0 | vfptr_primary
     4 | vfptr_sub4
     8 | vfptr_sub8
    12 | dwChainField
    16 | vfptr_sub16
    20 | dwPublishDate
    24 | wszContent
    28 | wszCopyright
    32 | bFlagByte
    33 | undefined1[7]
```

Applied via `create_struct` after deleting the prior 1-byte placeholder.

Slice **37** (2026-05-30): `get_struct_layout` re-verified 40 bytes; decompile `CDSResourceSign_WriteToStream@0x434310` / `ReadFromStream@0x4343a0` use `dwPublishDate`, `wszContent`, `wszCopyright`, `bFlagByte`; factory `RegisterCDSResourceSignAsClass94@0x0047d1d0` → `CDSResourceSign_ctor@0x00434930` (`OperatorNewWithBadAlloc(0x28)`).

## UNK

- `+0x18` not explicitly zeroed in ctor (only `param_1[7]`/`[8]`); relies on `malloc` zero-init — not separately proven.
- RTTI / secondary interface sub-object semantics (`IDSChained`, `IDSReferenced`) not expanded into nested structs.

## Notes (follow-up round 2)

- **`param_1[8]` vs `flagByte`:** `CDSResourceSign_dtor@0x00434670` calls `CDsStringReleaseHeader` on `param_1[8]` (offset `+0x20`) while stream I/O and the ctor plate treat `+0x20` as a single **`u8 flagByte`**. Same physical offset: the dtor body is typed as `undefined4 *` and reuses dword index `8` for the trailing slot. Shipping `.eap` records always store `flagByte == 0x00` (`sign_record.md`), so `param_1[8] == 0` and the release branch is **dead** in practice — not a third `wstring`. Only `param_1[7]` (`wszCopyright`) and `param_1[6]`-indexed content (`wszContent` at `+0x18`) are string-managed; catalog text: "frees the two `std::wstring` payloads".
