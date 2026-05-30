# type_info (`std::type_info`)

## Status

**SKIP** — MSVC CRT / RTTI library type (`Runtime` / VS2005 Release). No game `OperatorNewWithBadAlloc` sizing; used only by EH / RTTI helpers (`_Type_info_dtor`, `operator==`, dynamic cast glue). Per `AGENT_PROTOCOL.md`, Ghidra placeholder (size 1) is left unchanged.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| CRT-only; no engine alloc | — | No `OperatorNew*` xref to `type_info` ctors in `0x0040xxxx` game range |
| Instance field at `+0` only in dtor | `type_info::~type_info` @ `0x0044734d` | `*(void***)this = vftable`; delegates `_Type_info_dtor` |
| Scalar deleting dtor | `type_info::_scalar_deleting_destructor_` @ `0x00447361` | `~type_info` then conditional `_free(this)` |
| `operator==` | `type_info::operator==` @ `0x00447377` | `_strcmp` on name bytes at `this+9` / `param_1+9` (CRT) |
| `~type_info` | `type_info::~type_info` @ `0x0044734d` | Sets vftable; `_Type_info_dtor(this)` |

## Layout table (CRT reference — not applied)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void **` | `vftable` | `~type_info@0x0044734d` |
| `0x04`+ | — | — | (opaque) | `_Type_info_dtor@0x004498d0` — registry-backed; not game-layout recovered |

## Ghidra apply

```
get_struct_layout type_info → Size: 1 (placeholder; SKIP)
```

No `create_struct` — CRT stub policy.

## UNK

- Bytes `+0x04..+0x08` between vftable and name string (not read in this binary’s `type_info` slice).

### R5 worker 46 (closed)

- **Decorated name offset (VERIFIED)**: `type_info::operator==@0x00447377` — `_strcmp((char *)(param_1 + 9), (char *)(this + 9))`; compare runs on **byte offset +9** (after 4-byte vftable + 4-byte opaque prefix).
- **No game `CDS*` relationship (VERIFIED)**: CRT-only; RTTI tables in `0x0049xxxx` / `0x004axxxx` `.rdata`; no `OperatorNew` / gameplay ctor paths.
