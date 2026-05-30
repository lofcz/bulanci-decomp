# exception (`std::exception`)

## Status

**SKIP** — MSVC CRT / STL library type (`Runtime.STL`). No game/engine `OperatorNew` allocation of `exception` objects; all use is via linked VS2005 Release library ctors/dtors and EH glue. Per `AGENT_PROTOCOL.md`, do not apply a Ghidra struct (placeholder remains size 1).

Reference layout below is **CRT-evidence only** (for re-typing if this batch is reopened).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Object spans `this+0` … `this+0xB` (12 bytes) | `0x0044beb0` | `std::exception::exception(char const* const&)` writes vftable @+0, message ptr @+4, flag @+8 |
| `+8` is last instance field | `0x0044bf6e` | `exception::~exception` tests `*(int*)(this+8)` before `_free(*(void**)(this+4))` |
| Heap delete uses whole object | `0x0044bf91` | `std__exception_ScalarDeletingDtor` → `~exception` then `_free(this)` when `param_1 & 1` |
| No engine heap sizing | — | No `OperatorNewWithBadAlloc(0xc)` (or other `0xc` game alloc) xref to `std::exception` ctors; game throws via `CDS*Exception` hierarchy |

## Layout (CRT reference — not applied in Ghidra)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void **` | `vftable` | `std::exception::exception@0x0044beb0` `*(void***)this = g_pException_vftable_CDSObject` |
| 0x04 | 4 | `char *` | `_Message` | ctor @`0x0044beb0` / @`0x0044befe` / copy @`0x0044bf16`; `std_bad_alloc_what@0x0044bf84` reads `*(char**)(param_1+4)` |
| 0x08 | 4 | `int` | `_DoFree` | ctor @`0x0044beb0` sets `1` (heap copy); @`0x0044befe` sets `0` (literal/static); copy @`0x0044bf16` copies flag; dtor @`0x0044bf6e` gates `_free` |

## RTTI / vtable

| Item | Address | Evidence |
|------|---------|----------|
| `std::exception` vtable | `0x0048965c` | `master_vtable_catalog.csv`: slots `[0]=scalar deleting dtor@0x0044bf91`, `[1]=what@0x0044bf84` |
| Throw helper RTTI | `0x004ac57c` | `OperatorNewWithBadAlloc@0x00447be4` → `__CxxThrowException_8(local_10, &DAT_004ac57c)` after copying into stack `exception` |

## Ghidra apply

```
get_struct_layout exception  → Size: 1 (placeholder; SKIP)
```

No `create_struct` — CRT stub policy.

### Slice 44 re-verification (2026-05-30)

| MCP check | Result |
|-----------|--------|
| `get_struct_layout exception` | Size 1 (placeholder unchanged) |
| `decompile` @ `0x0044beb0` | `*(void***)this`, `*(char**)(this+4)`, `*(int*)(this+8)=1` |
| `decompile` @ `0x0044bf6e` | `~exception` frees `+4` when `+8 != 0` |
| `create_struct` | **Not applied** (CRT SKIP) |

## UNK

- *(R5 worker 43 — no open UNK; CRT SKIP)* All items below closed in prior batches; re-verified 2026-05-30.
- **Naming:** Ghidra uses both `exception` and `std::exception` for the same CRT type — demangler inconsistency only.
- **Game embed:** none. Ctors/dtors only on EH / `bad_alloc` throw glue (`FindHandler@0x0044b874`, `OperatorNewWithBadAlloc@0x00447c42`, static `bad_alloc` `@atexit`).
