# bad_exception (`std::bad_exception`)

## Status

**SKIP** — MSVC CRT / STL EH helper (`Runtime.STL`). No dedicated `bad_exception` ctor in the binary; instances are built on the stack via `std::exception::exception` inside EH machinery, or only referenced through RTTI/vtable. Per `AGENT_PROTOCOL.md`, do not apply a Ghidra struct.

Same **12-byte** layout as base `exception` (vtable swap only).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Same footprint as `exception` | `0x0044b047` | `std::bad_exception_ScalarDeletingDtor` casts to `exception *`, calls `exception::~exception`, then `_free(this)` |
| No separate instance fields | — | No writes to `this+N` for N>0 beyond vtable in `bad_exception` code |
| EH stack construction | `0x0044b782` region | `FindHandler` path: `std::exception::exception((exception*)&stack…, &s_bad_exception_00489640)` then `__CxxThrowException_8(..., &DAT_004ac864)` |
| No engine heap sizing | — | No `OperatorNew` xref to `bad_exception`; vtable only DATA xref @ `0x00489638` |

## Layout (CRT reference — not applied in Ghidra)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void **` | `vftable` | `std::bad_exception_ScalarDeletingDtor@0x0044b047` `*(void***)this = g_pBadException_vftable_CDSObject` |
| 0x04 | 4 | `char *` | `_Message` | Inherited; EH path uses `std::exception::exception@0x0044beb0` on stack object before throw |
| 0x08 | 4 | `int` | `_DoFree` | Inherited; same dtor path as `exception` via base `~exception@0x0044bf6e` |

## RTTI / vtable

| Item | Address | Evidence |
|------|---------|----------|
| `std::bad_exception` vtable | `0x00489638` | `master_vtable_catalog.csv`: `[0]=scalar deleting dtor@0x0044b047`, `[1]=what@0x0044bf84` (shared with `std::exception`) |
| RTTI descriptor | `std::bad_exception::RTTI_Type_Descriptor` | Compared in `FindHandler` @ `0x0044b782` via `type_info::operator==` when exception spec mismatch |
| Literal message | `s_bad_exception_00489640` | Passed to `std::exception::exception` in EH rewrite path |

## Ghidra apply

```
get_struct_layout bad_exception  → Size: 1 (placeholder; SKIP)
```

No `create_struct` — CRT stub policy.

### Slice 44 re-verification (2026-05-30)

| MCP check | Result |
|-----------|--------|
| `get_struct_layout bad_exception` | Size 1 (placeholder unchanged) |
| `decompile` @ `0x0044b047` | Scalar dtor → `exception::~exception((exception*)this)` |
| `decompile` @ `0x0044b874` (`FindHandler`) | Stack `local_30[3]` (12 B); `std::exception::exception` then `g_pBadException_vftable` + `__CxxThrowException_8` |
| `create_struct` | **Not applied** (CRT SKIP) |

## UNK

- *(follow-up round 2 closed)* **No dedicated ctor:** Only `std::bad_exception_ScalarDeletingDtor@0x0044b047` is exported; construction is `std::exception::exception((exception *)&stack…, &s_bad_exception_00489640)` inside `FindHandler@0x0044b874` when `IsInExceptionSpec` fails and RTTI matches `std::bad_exception::RTTI_Type_Descriptor` (not `FindHandlerForForeignException`).
- *(follow-up round 2 closed)* **Throw frequency:** `g_pBadException_vftable` @ `0x00489638` has exactly two DATA xrefs — `std::bad_exception_ScalarDeletingDtor` and `FindHandler` (vtable install before throw). No other throw/allocation sites; path is rare EH “unexpected exception” rewrite, not game logic.
