# Round 3 — task 39 report

## Task

| Field | Value |
|-------|--------|
| **id** | 39 |
| **title** | Dedup exception structs: inherited prefix + tail fields |
| **one_liner** | Replace flat `0x3C` prefix duplicates and `pUnkBaseTail` blobs with embedded `CDSException base` @ `+0`; tails from `+0x3C` on `0x44`/`0x48`/`0x50`/`0x240` subclasses. |
| **acceptance** | `get_struct_layout` on all exception siblings; ctors call `CDSException_InitFields(&this->base,…)`; `save_program` |

**Types:** `CDSException`, `CDSSimpleException`, `CDSMemoryException`, `CDSResourceException`, `CDSStreamException`, `CDSDirectXException` (+ verified `CDSApiException` from task 27)

**Key addresses:** `CDSException_InitFields@0x00434a40`, `CDSSimpleException_Throw@0x00434c20`, `CDSMemoryException_ctor@0x00434a70`, `CDSResourceException_ctor@0x00434400`, `CDSStreamException_ctor@0x004300f0`, `CDSApiException_ThrowFromGetLastError@0x00434d00`

## Status

**DONE** — canonical `CDSException` (60 B) is the sole prefix type; all listed subclasses embed `CDSException base` @ `+0`. `CDSMemoryException` no longer has `pUnkBaseTail`; inherited `pInlineMessage` lives under `base`.

## Evidence

### Ghidra layouts (`get_struct_layout`, 2026-05-30)

| Struct | Size | `base` @ +0 | Tail @ +0x3C+ |
|--------|------|-------------|----------------|
| `CDSException` | 60 | *(root)* | — |
| `CDSSimpleException` | 60 | 60 B embed | — |
| `CDSApiException` | 68 | 60 B embed | `pFormattedMessage`, `dwWin32Error` |
| `CDSResourceException` | 68 | 60 B embed | `dwResourceId`, `pszFormatted` |
| `CDSDirectXException` | 72 | 60 B embed | `pMessage_handle`, `dwContext_code`, `dwHresult` |
| `CDSStreamException` | 80 | 60 B embed | `pFormatMsg` … `dwWin32Error` |
| `CDSMemoryException` | 576 | 60 B embed | `dwFormatArg`, `pFormatted[256]` |

`search_data_types` pattern `CDSException`: only `/CDSException` (60 B) and `/CDSException *` — no unused flat 60 B duplicate to delete.

### Decompiler (embedded base)

| Func @ addr | Finding |
|-------------|---------|
| `CDSException_InitFields@0x00434a40` | `CDSException *this` — sets `pVftable`, codes, `pInlineMessage` geometry |
| `CDSMemoryException_ctor@0x00434a70` | `CDSException_InitFields(&this->base,1,1,0)`; `(base).pVftable = 0x487534` |
| `CDSMemoryException_What@0x004349c0` | `this->dwFormatArg`, `this->pFormatted` (not flat `+0x14` buffer) |
| `CDSStreamException_ctor@0x004300f0` | `InitFields(&this->base,5,7,1)`; tail string handles at `+0x3C` |
| `CDSResourceException_ctor@0x00434400` | `InitFields(&this->base,6,8,1)`; `dwResourceId`, `pszFormatted` |
| `CDSResourceException_ctor_default@0x004344a0` | Same base; clears `pszFormatted` |
| `CDSSimpleException_Throw@0x00434c20` | `OperatorNew(0x3c)`; `InitFields` on allocated object (no tail) |

### Size proof (allocation)

| Claim | Address | Evidence |
|-------|---------|----------|
| Base-only `0x3C` | `CDSSimpleException_Throw@0x00434c20` | `OperatorNewWithBadAlloc(0x3c)` |
| `0x44` + embed | `CDSApiException_Throw@0x00434d00`, `CDSResourceException_Throw@0x004346f0` | `OperatorNewWithBadAlloc(0x44)` |
| `0x48` + embed | `CDSDirectXException_ThrowFromHresult@0x0043b820` | `OperatorNewWithBadAlloc(0x48)` |
| `0x50` + embed | `RaiseStreamException@0x00430270` | `OperatorNewWithBadAlloc(0x50)` |
| `0x240` + embed | `CDSMemoryException_Factory@0x00434aa0` | `OperatorNewWithBadAlloc(0x240)` |

## Ghidra deltas

- Verified all exception structs use **`CDSException base`** @ `+0` (replaced prior `pUnkBaseTail` / flat prefix on `CDSMemoryException`).
- `set_function_this_type` on `CDSMemoryException_ctor`, `CDSStreamException_ctor`, `CDSResourceException_ctor` / `_ctor_default`.
- `CDSResourceException_ctor@0x00434400` moved into class namespace; decompile uses `this->base` / `dwResourceId` / `pszFormatted`.
- `modify_struct_field` `pPszFormatted` → `pszFormatted` on `CDSResourceException`.
- Prototype refresh on throw helpers (`0x00434d00`, `0x004346f0`, `0x0043b820`).
- `save_program bulanci.exe` (×2).

## Struct doc updates

- `CDSException.md`, `CDSSimpleException.md`, `CDSMemoryException.md`, `CDSResourceException.md`, `CDSStreamException.md`, `CDSDirectXException.md` — agent todo 39 notes (embed pattern).
- `CDSMemoryException.md` — layout table collapsed to `base` + tail rows; status no longer cites `pUnkBaseTail`.

## Remaining UNK

- **Throw-site locals:** `CDSApiException_ThrowFromGetLastError` and `CDSDirectXException_ThrowFromHresult` still allocate into `CDSException *` / use `this[1].*` for tail fields — struct manager embed is correct; fix needs `CDSApiException *` / `CDSDirectXException *` stack locals (Ghidra limitation).
- **`CDSResourceException_ThrowFromResourceId@0x004346f0`:** decompiler parent namespace still shows `CDSStreamStorage::` prefix; function renamed, alloc types `CDSResourceException *`.
- Vtable slot catalogs for exception tables (`0x487534`, `0x4874b0`, …) — deferred (task 34).
