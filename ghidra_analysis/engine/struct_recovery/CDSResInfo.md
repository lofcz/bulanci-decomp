# CDSResInfo

## Status

**VERIFIED** — minimum instance size `0x18` (24 bytes); dual-vtable resource key object embedded in `.eap` stream entries and extended by `CDSStrmResInfo` (`0x28`), `CDSResourceSign` (`0x28`), etc.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Material offsets through `+0x17` | `CDSResInfo_ReleaseEmbeddedResource@0x00434270` | `*(param_1+0x14)` pointer release when non-null |
| Dual vtables | `CDSResInfo_dtor@0x00433a10` | `*param_1 = 0x4873cc`; `param_1[1] = 0x4873b0` before `ReleaseEmbeddedResource` |
| Key dwords at object `+0x08` / `+0x0C` | `CDSResInfo_Load@0x00434290` / `Save@0x004342c0` | Stream I/O at `this+4` and `this+8` when `this` is the **IDSChained** face (`+4` from object base) → `dwResourceId` / `dwClassId` |
| Derived alloc superset | `CDSStrmResInfo_factory@0x00433d80` | `OperatorNew(0x28)` extends same prefix; see [CDSStrmResInfo.md](./CDSStrmResInfo.md) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CDSResInfo_dtor@0x00433a10` → `0x4873cc` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | `CDSResInfo_dtor@0x00433a10` → `0x4873b0` |
| `0x08` | 4 | `uint` | `dwResourceId` | `CDSResInfo_Load` / `Save` (via chained `this`); `CDSStrmResInfo_CompareKey@0x004342f0` sorts on `entry+8` |
| `0x0C` | 4 | `uint` | `dwClassId` | `CDSResInfo_Load` / `Save` second dword; pairs with `.eap` `ResourceHeader.class_id` |
| `0x10` | 4 | — | *(implicit pad)* | No base-type consumer; `CDSStrmResInfo` reuses as `loaderAux` |
| `0x14` | 4 | `void *` | `pEmbeddedResource` | `CDSResInfo_ReleaseEmbeddedResource@0x00434270`; **not** the `CDSStrmResInfo.streamExtent` QWORD |

## Ghidra apply

```
get_struct_layout("CDSResInfo") → Size: 24 bytes
  pVftable_IDSReferenced, pVftable_IDSChained,
  dwResourceId, dwClassId, pEmbeddedResource @ +0x14
  (4-byte hole @ +0x10 implicit)
```

Slice 36 (2026-05-30): deleted 1-byte placeholder, `create_struct`, renamed `FUN_00433aa0` → `CDSResInfo_ReleaseViaChainedFace`, prototypes on Load/Save/ReleaseEmbeddedResource, `save_program bulanci.exe`.

## UNK

- No standalone `OperatorNew(0x18)` factory for plain `CDSResInfo` (instances are stack keys, embedded nodes, or subclass allocs).
- `CDSResInfo_ReleaseViaChainedFace@0x00433aa0` — chained-face teardown twin of dtor path; callers not fully catalogued.

## Cross-refs

- Stream pack: [CDSStrmResInfo.md](./CDSStrmResInfo.md), [CDSStreamStorage.md](./CDSStreamStorage.md)
- Class registry: `g_pCDSResInfo` / `DAT_004b7e08` (`CDSFlxFile::GetClassRegistry`)
