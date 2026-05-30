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

- Full `type_info` layout in VS2005 (name pointer / decorated name handle) — only dtor/equals surfaced in this binary’s import slice.
- Relationship to game `CDS*` types (none; RTTI comparisons hit `0x0049xxxx` / `0x004axxxx` data, not engine classes).
