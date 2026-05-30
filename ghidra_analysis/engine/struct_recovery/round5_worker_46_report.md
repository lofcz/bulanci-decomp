# Round 5 — Worker 46 Report (T–Z UNK sweep)

## Task

| Field | Value |
|-------|-------|
| **worker** | 46 / 50 |
| **round** | 5 |
| **mode** | WRITE |
| **title** | struct_recovery T–Z UNK |
| **scope** | `TranslatorGuardRN`, `type_info`, `_LocaleUpdate`, `_s_CatchableType` (all `struct_recovery/*.md` basenames in `[T-Z]` and leading `_`) |

## Status

**DONE** — Four CRT/EH structs in the T–Z band; closed or narrowed every in-scope UNK with Ghidra decompile/xref evidence. Ghidra PRE comments + `save_program`. Remaining items are out-of-scope (`_s_ThrowInfo`) or toolchain (`/Demangler/_s_CatchableType`).

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| CSET_SPECIAL reuses `pNext` slot | `_CallSETranslator@0x004478fd` | Decompile: `ExceptionRecord == 0x123` → `Frame->field0_0x0 = 0x4479a6` (`ExceptionContinuation`); else builds 40-byte stack guard |
| No game xrefs to translator guard | `_CallSETranslator`, `TranslatorGuardHandler` | `get_function_xrefs`: callers `0x0044b7d5`, `0x00447a53` only; data `0x00447923`, `0x004a4f34` |
| `TranslatorGuardRN` size 40 | Ghidra struct | `get_struct_layout TranslatorGuardRN` → 40 bytes, 10 fields |
| `_LocaleUpdate` teardown inlined | `getSystemCP@0x0044c8ab`, `__woutput_l@0x0044f03a` | `AND [ptd+0x70], ~2` when `updated`; ctor sets bit @ `_LocaleUpdate@0x004487c8` |
| `_LocaleUpdate` fields 0..0xC | `_LocaleUpdate@0x004487c8` | Writes at `+0`, `+4`, `+8`, `this[0xc]`; no access `>= 0x10` |
| `type_info` name @ +9 | `type_info::operator==@0x00447377` | `_strcmp` on `this+9` / `param_1+9` |
| Catchable `dwProperties` bits | `___TypeMatch@0x0044b09d`, `___BuildCatchObjectHelper@0x0044b506` | Masks `0x1`, `0x2`, `0x4`, `0x8` on catchable; `0x80000000` on throw object |
| No catchable tail past 0x18 | EH cluster | No `[reg+0x1c]` on catchable in `___BuildCatchObject*`; struct ends `copyFunction` @ +0x18, size 28 |
| `_s_CatchableType` Ghidra layout | Ghidra | `get_struct_layout _s_CatchableType` → 28 bytes, 7 fields |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x004478fd` | CSET_SPECIAL / `pNext` dual-use note |
| `set_decompiler_comment` | `0x0044b09d` | `___TypeMatch` property-bit summary |
| `set_decompiler_comment` | `0x00447377` | `type_info` name compare @ `+9` |
| `set_function_prototype` | `0x0044b069` | **blocked** — demangler stub type resolution |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [TranslatorGuardRN.md](./TranslatorGuardRN.md) — CSET_SPECIAL + xref closure; `ENABLE_EHTRACE` only remaining UNK
- [_LocaleUpdate.md](./_LocaleUpdate.md) — UNK cleared (inlined teardown, no padding)
- [_s_CatchableType.md](./_s_CatchableType.md) — property flag table; tail padding closed; demangler blocker retained
- [type_info.md](./type_info.md) — name offset `+9` proven; game isolation confirmed

## Remaining UNK

| Struct | Item | Notes |
|--------|------|-------|
| `TranslatorGuardRN` | `ENABLE_EHTRACE` tail | Not in VS2005 build (size `0x28`) |
| `type_info` | `+0x04..+0x08` | Opaque between vftable and name @ `+9` |
| `_s_CatchableType` | Demangler duplicate type | `/Demangler/_s_CatchableType` size 1 vs `/_s_CatchableType` 28 |
| (out of band) | `_s_ThrowInfo` | Worker 46 T–Z scope only; defer separate batch |
