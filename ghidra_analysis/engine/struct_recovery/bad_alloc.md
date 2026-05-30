# bad_alloc (`std::bad_alloc`)

## Status

**SKIP** — MSVC CRT exception type; no game-owned heap layout beyond `std::exception` (see `exception.md`, batch 44). Singleton at `DAT_004b85b0` (12 bytes). Do not `create_struct` in Ghidra.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Object size = 12 (same as `exception`) | `0x00447c42` | `OperatorNewWithBadAlloc`: stack throw object `undefined **local_10[3]` (3×4) before `__CxxThrowException_8` |
| Static instance size = 12 | `0x004b85b0` | `bad_alloc::bad_alloc((bad_alloc *)&DAT_004b85b0)`; dtor `exception::~exception((exception *)&DAT_004b85b0)` |
| No derived fields past `exception` | `0x00447be4` | Ctor only calls `exception::exception(...)` then overwrites vptr |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | vftable | `bad_alloc@0x00447be4` writes `g_pVtbl_StdBadAlloc_IDSChained` after base ctor |
| 0x04 | 4 | `char *` | `_Message` | Inherited; `bad_alloc@0x00447be4` → `exception::exception@0x0044befe` with `&PTR_s_bad_allocation_004b0e4c`; ctor always stores `*(this+8)=0` (third arg unused) |
| 0x08 | 4 | `int` | `_DoFree` | Inherited; `exception::exception@0x0044befe` (see `exception.md`) |

Vtable: `std::bad_alloc` @ `0x004895c0` (5 slots; slots 3–4 shared empty stubs per export comments).

## Ghidra apply

Not applied (SKIP). Verified 2026-05-30 (agent slice 45):

```
get_struct_layout bad_alloc  → Size: 1 (placeholder; /std/bad_alloc)
```

No `create_struct` per CRT stub policy. Functions already named: `std::bad_alloc::bad_alloc@0x00447be4`, `std_bad_alloc_ScalarDeletingDtor@0x00447c08`, `OperatorNewWithBadAlloc@0x00447c42`.

**Rename (slice 45):** `FUN_0047ee91` → `std_bad_alloc_atexit_teardown` — `_atexit` callback from `OperatorNewWithBadAlloc`; restores vptr on `DAT_004b85b0` then `exception::~exception`.

## UNK

- Whether Ghidra should model explicit inheritance vs alias to `exception` (cosmetic).

## Cross-refs (game-relevant)

| Site | Role |
|------|------|
| `OperatorNewWithBadAlloc@0x00447c42` | Engine `operator new` wrapper; throws `bad_alloc` on OOM (hundreds of alloc call sites) |
| `bad_alloc@0x00447be4` | Ctor (VS2005/2008 CRT) |
| `std_bad_alloc_ScalarDeletingDtor@0x00447c08` | Scalar deleting dtor |
| `std_bad_alloc_what@0x0044bf84` | `what()` → `"bad allocation"` |
| `std_bad_alloc_atexit_teardown@0x0047ee91` | Process-exit teardown for static singleton `DAT_004b85b0` |

Throw helper: `__CxxThrowException_8(local_10, &DAT_004ac57c)` @ `0x00447c96`. Stack throw object: `undefined **local_10[3]` (12 bytes) @ `OperatorNewWithBadAlloc@0x00447c42`.
