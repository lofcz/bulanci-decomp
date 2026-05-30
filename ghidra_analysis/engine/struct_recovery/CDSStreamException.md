# CDSStreamException

## Status

**VERIFIED** (size 0x50; base + stream-specific string handles and errno fields)

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Instance size **0x50** (80) | `RaiseStreamException` @ `0x00430270` | `OperatorNewWithBadAlloc(0x50)` + `CDSStreamException_ctor` |
| Same size | `ThrowStreamErrorNoReturn` @ `0x004302e0` | `OperatorNewWithBadAlloc(0x50)` + `CDSStreamException_ctor_win32` |
| Same size | `CDSStreamException_AllocateDefault` @ `0x00430a90` | Factory path `OperatorNewWithBadAlloc(0x50)`; sole DATA xref @ `0x0047ca10` |
| Vtable **0x004870cc** | `CDSStreamException_ctor` @ `0x004300f0` | `*(undefined4 *)this = 0x4870cc` after `CDSException_InitFields(..., 5, 7, 1)` |
| RTTI string | — | `.?AVCDSStreamException@@` @ `0x004afd6c` (documented in `formats/stream_hierarchy.md`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------:|-----:|------|------|----------------------|
| 0x00 | 4 | `void *` | vftable | `CDSStreamException_ctor@0x004300f0` → `0x4870cc` |
| 0x04 | 1 | `byte` | deleteOnRelease | `CDSException_InitFields@0x00434a40` (via ctor) |
| 0x05 | 3 | `byte[3]` | pad_05 | Padding after +4 byte |
| 0x08 | 4 | `void *` | messageCache | `CDSException_InitFields@0x00434a40`; base `GetMessageW` path |
| 0x0C | 4 | `uint` | codePrimary | `CDSException_InitFields` with **5** in ctor (`stream` category) |
| 0x10 | 4 | `uint` | staticTextIndex | `CDSException_InitFields` with **7** in ctor (static text slot) |
| 0x14 | 0x28 | `wchar_t[20]` | inlineMessage | Base buffer; ends at 0x3C by analogy with `CDSSimpleException` alloc geometry |
| 0x3C | 4 | `void *` | formatMsg | Ctor zeroes `this+0x3c`; `CDSStreamException_FormatMessage@0x00430360` `CDsStringFormatV(this+0x3c, …)`; dtor `param_1[0xf]` release |
| 0x40 | 4 | `void *` | win32Msg | `FormatMessage@0x00430360` assigns Win32 text; dtor `param_1[0x10]` |
| 0x44 | 4 | `void *` | streamName | Ctor `CDsStringAssignFromHandle(this+0x44, …)` from stream `GetName` vslot `+0x34`; dtor `param_1[0x11]` |
| 0x48 | 4 | `uint` | streamErrno | `CDSStreamException_ctor@0x004300f0` `dwStreamErrno=param_1`; `CDSStreamException_AllocateDefault@0x00430a90` zeroes `dwStreamErrno` |
| 0x4C | 4 | `uint` | win32Error | Ctor reads `GetLastError()` into `+0x4c`; win32 ctor stores explicit `param_3`; `FormatMessage` tests `+0x4c` |

## Ghidra apply

```
get_struct_layout("CDSStreamException") → Size: 80 bytes
  CDSException base @ +0,
  pFormatMsg, pWin32Msg, pStreamName, dwStreamErrno, dwWin32Error
```

Applied via `create_struct` (2026-05-30 batch 36). Ghidra names: `pVftable`, `bDeleteOnRelease`, `pPad_05`, `pMessageCache`, `dwCodePrimary`, `dwStaticTextIndex`, `pInlineMessage`, `pFormatMsg`, `pWin32Msg`, `pStreamName`, `dwStreamErrno`, `dwWin32Error`.

**Slice 39 (2026-05-30):** `get_struct_layout` → 80 bytes (unchanged). Renamed `FUN_00430a90` → `CDSStreamException_AllocateDefault`; prototypes on ctor / ctor_win32 / dtor / factory — dtor decompile now uses `this->pFormatMsg` / `pWin32Msg` / `pStreamName` (not `param_1[0xf]`).

**Agent todo 39 (2026-05-30):** Rebuilt with embedded `CDSException base` @ `+0`; `CDSStreamException_ctor` calls `CDSException_InitFields(&this->base,…)` and sets `(this->base).pVftable = 0x4870cc`.

## UNK

- **errno → English string table** mapping lives in `formats/stream_hierarchy.md` (engine errno 1–8); not struct fields.

## Follow-up resolved (batch 36 round 2)

- **`+0x3C` vs `inlineMessage`:** `wchar_t[20]` at `+0x14` occupies `0x14..0x3B` (`0x28` bytes); `pFormatMsg` at `+0x3C` is adjacent, not overlapping. Ctor/dtor use `this->pFormatMsg` / `param_1[0xf]` (`0x3C`), `pWin32Msg` / `[0x10]`, `pStreamName` / `[0x11]` with `CDsStringReleaseHeader(ptr-0xc)` (`CDSStreamException_ctor@0x004300f0`, `CDSStreamException_dtor@0x00430000`).
- **`CDSStreamException_ctor_win32` @ `0x004301b0`:** Same field order and vtable `0x4870cc`; sets `dwWin32Error = param_3` instead of `GetLastError()`; otherwise matches default ctor.
- **Vtable @ `0x004870cc`:** `[0]` `GetClassRegistry@0x0042ffe0`; `[1]` scalar deleting dtor thunk `@0x00430b20`; `[2]` `ReleaseViaFlag@0x0042fff0`; `[3]` `CDSStreamException_FormatMessage@0x00430360` (`CDsStringFormatV` on `pFormatMsg`, optional `CDsStringFromWin32ErrorCode` on `dwWin32Error`); `[4]` `CDSObject_GetThis@0x00434b10`.

## Notes

- Primary construction path: `_Globals::RaiseStreamException` / `ThrowStreamErrorNoReturn` from `IDSStream` I/O failures.
- Dtor `CDSStreamException_dtor@0x00430000` releases three string headers then resets vtable to `g_pCDSObject_vftable_IDSReferenced`.
- Vtable @ `0x004870cc`: slots include `GetClassRegistry@0x0042ffe0`, scalar dtor thunk `@0x00430b20`, shared `ReleaseViaFlag@0x0042fff0`, `FormatMessage@0x00430360` (`vftable_methods.csv`).
