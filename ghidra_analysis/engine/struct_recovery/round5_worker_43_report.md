# Round 5 — Worker 43 Report (E–G UNK structs)

## Scope

| Field | Value |
|-------|-------|
| **Worker** | 43 / 50 |
| **Mode** | WRITE |
| **Slice** | `struct_recovery` struct names **E–G** with open `## UNK` |
| **Manifest** | 4 deliverables (+ `FOLLOWUP_ROUND_PROTOCOL.md` excluded — not a struct) |

| Struct / doc | Status | R5 outcome |
|--------------|--------|------------|
| `EHExceptionRecord` | SKIP (CRT) | UNK → permanent CRT documentation |
| `EHRegistrationNode` | SKIP (CRT) | UNK → overload / non-field items documented |
| `exception` | SKIP (CRT) | UNK already closed; re-verified |
| `FUN_0040a650` / `CDSChain_LoadConfigFromRegistry` | VERIFIED | **Closed** IDSChained vtable slot `+0x10` |

## Status

**DONE** — one game-relevant UNK closed (`CDSChain_Append`); three CRT SKIP structs have no remaining actionable UNK.

## Evidence — `CDSChain_Append` @ registry tail

| Claim | Address | Evidence |
|-------|---------|----------|
| Vtable base | `0x0047f6b8` | `read_memory` → 6 slots; `[4]` = `0x0042fb70` |
| Slot index | `0x0040a5d1` | `MOV EAX,[EAX+0x10]` after `[EBX+0x68]` |
| `this` (MI) | `0x0040a5d4` | `LEA ECX,[EBX+0x68]` → `CDSChain_full::pVftable_IDSChained` |
| Stream arg | `0x0040a5da` | `PUSH [EBP+0xc]` — gzip read face (`CDSGZipStream` local `+0xc`) |
| Target | `0x0040a5df` | `CALL EAX` → `CDSChain_Append` |

Catalog: `master_vtable_catalog.csv` — `CDSChain` / `IDSChained` / `[4]=CDSChain_Append`.

## Ghidra deltas (R5)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0042fb70` | `void __thiscall CDSChain_Append(CDSChain_full *this, void *pConfigStream)` |
| `set_function_this_type` | `0x0042fb70` | `CDSChain_full *` (class-scoped) |
| `set_decompiler_comment` | `0x0040a5ce`, `0x0042fb70` | vtable slot + stream arg notes |
| `force_decompile` | `0x0040a440` | PRE comment at dispatch site |
| `save_program` | `bulanci.exe` | success |

**Blocker (cosmetic):** indirect call at `0x0040a5df` still decompiles as `(*pcVar1)()` without visible arg until vtable slot is typed as a function pointer.

## Doc updates

- [EHExceptionRecord.md](./EHExceptionRecord.md) — CRT permanent UNK notes
- [EHRegistrationNode.md](./EHRegistrationNode.md) — `CSET_SPECIAL` / EBP / no game xrefs
- [exception.md](./exception.md) — no open UNK
- [FUN_0040a650.md](./FUN_0040a650.md) — slot closed
- [CDSChain_LoadConfigFromRegistry.md](./CDSChain_LoadConfigFromRegistry.md) — slot + `pGzipForEh` note

## Remaining UNK (slice E–G)

None actionable. CRT types retain documented non-game fields only.
