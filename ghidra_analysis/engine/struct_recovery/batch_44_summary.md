# Struct recovery batch 44/50

**Structs:** `exception`, `bad_exception`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile (`get_struct_layout`, ctors/dtors, EH `FindHandler` path) + `master_vtable_catalog.csv`

## Results

| Struct | Status | CRT ref size | Ghidra size | Applied |
|--------|--------|--------------|-------------|---------|
| `exception` | **SKIP** | `0xC` (12) | 1 (placeholder) | No |
| `bad_exception` | **SKIP** | `0xC` (inherits) | 1 (placeholder) | No |

## Rationale (CRT SKIP)

- Both types live in `Runtime.STL` (`0x0044b000`–`0x0044c000` per `docs/MODULES.md`).
- All ctors/dtors are **Library: Visual Studio 2005 Release**; no game `OperatorNewWithBadAlloc` sizing for `0xc` tied to these types.
- Game/engine errors use `CDSException` / `CDS*Exception` (separate batches); `std::exception` appears on stack/static only (`OperatorNewWithBadAlloc` bad_alloc path, EH `FindHandler` rewrite).

## Key evidence anchors

- `std::exception::exception(char const* const&)` @ `0x0044beb0` — vftable `+0`, heap message `+4`, `_DoFree=1` @ `+8`.
- `std::exception::exception(char const* const&, int)` @ `0x0044befe` — literal message `+4`, `_DoFree=0` @ `+8`.
- `exception::~exception` @ `0x0044bf6e` — conditional `_free` of `+4` when `+8 != 0`.
- `std::bad_exception_ScalarDeletingDtor` @ `0x0044b047` — swaps vtable `0x00489638`, delegates to base dtor.
- `std_bad_alloc_what` @ `0x0044bf84` — reads `what()` string at `this+4` (shared vtable slot for `std::exception` / `bad_exception`).

## Ghidra actions

- [x] `get_struct_layout exception` → size 1 (placeholder)
- [x] `get_struct_layout bad_exception` → size 1 (placeholder)
- [ ] `create_struct` — **skipped** (CRT policy)
- [x] `save_program bulanci.exe` — slice 44 (2026-05-30); no struct mutations, program saved after MCP verify

## Agent slice 44 (2026-05-30)

Parallel RE agent **44/50** re-ran MCP evidence only (`exception`, `bad_exception`). Confirmed 12-byte CRT reference layout; Ghidra placeholders remain size 1. No `create_struct` / renames per `AGENT_PROTOCOL.md` CRT SKIP.

## Deliverables

- `ghidra_analysis/engine/struct_recovery/exception.md`
- `ghidra_analysis/engine/struct_recovery/bad_exception.md`

## Follow-ups

- Batch 45 (`bad_alloc`, `_LocaleUpdate`) — `bad_alloc` shares this `exception` layout; same SKIP/CRT rules likely apply.
- Game-facing exception layout: continue `CDSException` chain (batch 8 manifest) rather than STL placeholders.
