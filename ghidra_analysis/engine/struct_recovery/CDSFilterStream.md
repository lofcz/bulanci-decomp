# CDSFilterStream

## Status

**VERIFIED** — heap **`0x38`** (56 B); MI header through `IDSChained` and windowed-read fields proven from `CDSFilterStream_Ctor@0x00430dd0`, `CDSFilterStream_BindSource@0x00430ca0`, `CDSFilterStream_dtor@0x00430b90`, and `CDSFilterStream_ReadBytes@0x00430420`. `CDSSafeStream` wraps the **`IDSStream`** face at **`filter+0x0c`** (`CDSStreamStorage_CreateFilterSafeStream@0x00434760` passes `piVar3 + 3` = `filter+0x0c`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x38` | `CDSStreamStorage_CreateFilterSafeStream@0x00434760` | `OperatorNewWithBadAlloc(0x38)` → `CDSFilterStream_Ctor` |
| Last field `+0x30` | `CDSFilterStream_dtor@0x00430b90` | Releases `param_1[0xc]` (`pInnerStream`); ctor zeros `pInnerStream` before `BindSource` |
| Window/cursor init | `CDSFilterStream_BindSource@0x00430ca0` | Copies `param_2`/`param_3` into `dwCursorLo`/`dwCursorHi` and `dwWindowBaseLo`/`dwWindowBaseHi`; caps at `dwSizeCapLo`/`dwSizeCapHi` |
| `IDSStream` mdisp `+0x0c` | `CDSFilterStream_ReadBytes@0x00430420` | Plate: cursor `this+0xc/+0x10`, cap `this+0x1c`, bound flag `this+0x20`, inner `this+0x24` ⇒ outer `+0x18..+0x30` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `pVftable_IDSReferenced` | `CDSFilterStream_Ctor` `= 0x487150`; dtor restore |
| `+0x04` | 4 | `pointer` | `pVftable_IDSEventHandler` | ctor `= 0x48713c` |
| `+0x08` | 4 | `uint` | `dwStreamState` | ctor `= 1`; `CloseStream` / `CDSSafeStream_Close` write `0x20` at iface `+0x4` → outer `+0x08` |
| `+0x0c` | 4 | `pointer` | `pVftable_IDSStream` | ctor `= 0x4870fc`; passed to `CDSSafeStream_ctor` as `(filter+0xc)` |
| `+0x10` | 4 | `uint` | `dwIdsStream_state` | ctor `= 0x20`; `BindSource` snapshots `*(pInnerStream+4)` (IDSStream lifecycle sentinel; `stream_hierarchy.md` §2.2) |
| `+0x14` | 4 | `pointer` | `pVftable_IDSChained` | ctor `= 0x4870e4` |
| `+0x18` | 4 | `uint` | `dwCursorLo` | `ReadBytes` / `BindSource` window cursor low |
| `+0x1c` | 4 | `uint` | `dwCursorHi` | `ReadBytes` carry / `BindSource` |
| `+0x20` | 4 | `uint` | `dwWindowBaseLo` | `BindSource` copies `param_2` to `dwCursorLo` and here |
| `+0x24` | 4 | `uint` | `dwWindowBaseHi` | `BindSource` `param_3` duplicate at `+0x24` |
| `+0x28` | 4 | `uint` | `dwSizeCapLo` | `ReadBytes` bound check vs IDSStream `this+0x1c` |
| `+0x2c` | 4 | `int` | `dwSizeCapHi` | `ReadBytes` bound flag at IDSStream `this+0x20` (`-1` = unbounded) |
| `+0x30` | 4 | `pointer` | `pInnerStream` | `BindSource` / dtor release; `ReadBytes` delegate target |
| `+0x34` | 4 | `uint` | `dwPadding_34` | Structural tail pad to `0x38` (no ctor/xref writes) |

## Ghidra apply

```
get_struct_layout("CDSFilterStream") → Size: 56 bytes (0x38)
  pVftable_IDSReferenced, pVftable_IDSEventHandler, dwStreamState,
  pVftable_IDSStream, dwIdsStream_state, pVftable_IDSChained,
  dwCursorLo, dwCursorHi, dwWindowBaseLo, dwWindowBaseHi,
  dwSizeCapLo, dwSizeCapHi, pInnerStream, dwPadding_34
```

Applied slice 29 (2026-05-30): struct already present at 0x38; confirmed layout via `get_struct_layout`. Prototypes set on `CDSFilterStream_Ctor`, `CDSFilterStream_BindSource`, `CDSFilterStream_ReadBytes`. Renamed `FUN_00434760` → `CDSStreamStorage_CreateFilterSafeStream`.

**Agent todo 30 (2026-05-30):** Renamed `dwField_10` → `dwIdsStream_state`; decompiler comments @ `BindSource` store (`0x00430ce5`) and ctor init (`0x00430dd8`).

## CDSSafeStream composition (R4 todo 40)

`CDSStreamStorage_CreateFilterSafeStream@0x00434760` builds a **`0x38`** filter plus a **`0x48`** safe stream. Safe stream shares the header through `+0x14` but **replaces** the filter body at `+0x18` with an embedded `CDSChain` (not cursor/window fields). Safe ctor argument is the filter’s **`IDSStream`** subobject (`filter+0x0c`).

## UNK

- `CDSGZipStream` / `CDSEasyMemStream` tails beyond shared filter prefix (separate batches).
- Shared Ghidra `IDSStream` plate struct (40 B) uses filter-oriented names at `+0x0c..+0x24`; `CDSEasyMemStream::ReadBytes` reuses the same offsets with mem-stream semantics (`dwCursor` @ `+0x8`, backing pointer cast from `dwSizeCapLo` @ `+0x1c`) — see `stream_hierarchy.md` §2.3 vs §2.4.

## Follow-up (R4 task 30, 2026-05-30)

- `set_function_this_type` `IDSStream *` @ `CDSFilterStream_ReadBytes@0x00430420` → class `IDSStream`; decompile uses `dwCursorLo`/`dwCursorHi`, `nSizeCapHi`, `dwSizeCapLo`, `pInnerStream` on the `filter+0x0c` plate.
- Extended `IDSStream` Ghidra struct **32 → 40 B** (`pInnerStream` @ `+0x24` on plate). `save_program bulanci.exe`.
- Report: [round4_task_30_report.md](./round4_task_30_report.md).

## References

- `CDSFilterStream_Ctor` — `0x00430dd0`
- `CDSFilterStream_BindSource` — `0x00430ca0`
- `CDSFilterStream_dtor` — `0x00430b90`
- `CDSFilterStream_ReadBytes` — `0x00430420`
- `CDSStreamStorage_CreateFilterSafeStream` — `0x00434760`
- `stream_hierarchy.md` §2.4 — passthrough contract and field semantics
