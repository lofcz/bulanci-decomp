# CDSDirectXException

## Status

`VERIFIED` (size `0x48`; full `CDSException` prefix through `pInlineMessage` + DirectX tail at `+0x3c..+0x47`)

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSDirectXException) == 0x48` | `0x0043b820` | `CDSDirectXException_ThrowFromHresult` → `OperatorNewWithBadAlloc(0x48)` |
| Same size on heap factory | `0x0043b8b0` | `CDSDirectXException_AllocEmpty@0x0043b8b0` → `OperatorNewWithBadAlloc(0x48)` |
| Ghidra struct size | — | `get_struct_layout("CDSDirectXException")` → **72 bytes** (`0x48`) |

## Layout table

Inherits `CDSException` prefix (`CDSException_InitFields@0x00434a40`).

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `pVftable` | `CDSDirectXException_ThrowFromHresult@0x0043b820` → `g_pCDSDirectXException_vftable_IDSChained` |
| `+0x04` | 1 | `byte` | `bDeleteOnRelease` | `CDSException_InitFields@0x00434a40` (`param_4 == 1`) |
| `+0x05` | 3 | `byte[3]` | `pPad_05` | Structural padding after `+0x04` (no ctor writes) |
| `+0x08` | 4 | `void *` | `pMessageCache` | `CDSException_InitFields@0x00434a40` zeroed; `CDSException_GetMessageW@0x00434b80` lazy-fill |
| `+0x0c` | 4 | `uint` | `dwCodePrimary` | `CDSException_InitFields` with `0xf` in throw path @ `0x0043b820` |
| `+0x10` | 4 | `uint` | `dwStaticTextIndex` | `CDSException_InitFields` with `3` @ `0x0043b820` |
| `+0x14` | `0x28` | `wchar_t[20]` | `pInlineMessage` | `CDSException_GetMessageW@0x00434b80` uses `this+0x14` as `swprintf` buffer (shared base; same geometry as `CDSSimpleException`) |
| `+0x3c` | 4 | `void *` | `message_handle` | Throw ctor clears; `CDSDirectXException_What@0x0043b7c0` formats via `CDsStringFormatV`; dtor @ `0x0043b760` releases |
| `+0x40` | 4 | `uint` | `dwContext_code` | Throw ctor `param_1`; `CDSDirectXException_What@0x0043b7c0` → `CDSDirectXException_ResolveMessageStringId@0x0043b720` (context 0/1/2) |
| `+0x44` | 4 | `uint` | `dwHresult` | Throw ctor `param_2`; `CDSDirectXException_ResolveMessageStringId` → `CDSDirectXException_LookupStringIdForHresult@0x0043b6f0` |

## Ghidra apply

```
get_struct_layout("CDSDirectXException") → Size: 72 bytes
  CDSException base @ +0; message_handle @ +0x3c; dwContext_code @ +0x40; dwHresult @ +0x44
```

Applied follow-up: replaced `pPad_0x14` with `pInlineMessage`; aligned base field names with `CDSSimpleException` / batch 36. **Agent todo 29 (2026-05-30):** flat prefix deduplicated — embeds `CDSException base` @ `+0` (matches `CDSApiException` / `CDSSimpleException`). See [CDSException.md](./CDSException.md) for the shared `0x3C` prefix.

**Slice 28 (2026-05-30):** Leaf renames — `CDSDirectXException_ResolveMessageStringId@0x0043b720`, `CDSDirectXException_AllocEmpty@0x0043b8b0`; prototypes set on both + `LookupStringIdForHresult`.

## Leaf functions

| Address | Name | Role |
|---------|------|------|
| `0x0043b760` | `CDSDirectXException_dtor` | Releases `message_handle` (`+0x3c`) |
| `0x0043b7c0` | `CDSDirectXException_What` | `ResolveMessageStringId` + `CDsStringFormatV` on `message_handle` |
| `0x0043b820` | `CDSDirectXException_ThrowFromHresult` | Alloc `0x48`, init, `__CxxThrowException` |
| `0x0043b8b0` | `CDSDirectXException_AllocEmpty` | Default shell (no throw) |
| `0x0043b6f0` | `CDSDirectXException_LookupStringIdForHresult` | Linear scan `g_DirectXHresultStringTable` @ `0x4b0288` |
| `0x0043b720` | `CDSDirectXException_ResolveMessageStringId` | Context-gated lookup; else `g_apCDSStaticTextsSingleton[2]+0x50` |

**Agent todo 39 r4 (2026-05-30):** `CDSDirectXException_ThrowFromHresult@0x0043b820` — `this` / `local_10` typed `CDSDirectXException *`; tail fields named in decompile. See [round4_task_39_report.md](./round4_task_39_report.md).

## UNK

- Vtable @ `0x489474`: slot symbols partially `FUN_*` in Ghidra (`master_vtable_catalog.csv` has full list).

## Follow-up resolved (agent todo 29, round 2)

- **`What` vs inline buffer:** `CDSDirectXException_What@0x0043b7c0` formats into **`message_handle` @ `+0x3c`** via `CDsStringFormatV` after `ResolveMessageStringId` — it does **not** call `CDSException_GetMessageW` and does **not** write **`pInlineMessage` @ `+0x14`**. The shared `wchar_t[20]` inline buffer is for the base `GetMessageW` path (`CDSException_GetMessageW@0x00434b80` lazy `swprintf` when the static-text slot is zero). Thrown instances set HRESULT tail at `+0x40` / `+0x44` in `ThrowFromHresult@0x0043b820`.
