# Struct recovery batch 49 — follow-up round 2

**Status:** `HANDOFF_CLEAN`  
**Prior:** [batch_49_summary.md](./batch_49_summary.md)  
**Deliverable reviewed:** [TranslatorGuardRN.md](./TranslatorGuardRN.md)

## Prior batch follow-ups

None listed in `batch_49_summary.md` (no explicit follow-up / UNK / cleanup queue).

## Actions taken

| Action | Result |
|--------|--------|
| Read `batch_49_summary.md` + `TranslatorGuardRN.md` | Single struct `TranslatorGuardRN`; status **VERIFIED**, 40 bytes |
| Cross-check doc vs prior evidence | Layout table matches `_CallSETranslator@0x004478fd` and `TranslatorGuardHandler@0x004479d2` |
| `get_struct_layout TranslatorGuardRN` (bulanci.exe) | Size 40; 10 fields at offsets 0–36; matches deliverable Ghidra apply table |
| Ghidra mutations | **None** (no open follow-ups; struct already applied) |
| `save_program bulanci.exe` | **Not called** (no program changes) |

## Ghidra deltas

None.

## Remaining UNK (documented only — not batch queue)

From [TranslatorGuardRN.md](./TranslatorGuardRN.md) § UNK — informational, no further binary proof in scope:

- **+0x00 dual use** on `CSET_SPECIAL` (`pExcept == 0x123`): `param_2` is `void **` for `ExceptionContinuation`, not a normal `pNext` link.
- **`ENABLE_EHTRACE` `trace_level`**: not present; size ends at 0x28 (VS2005 CRT build).
- **Game xrefs**: none (CRT-only).

## Handoff

Batch 49 / manifest index 49 (`TranslatorGuardRN`) is complete for struct recovery. Final manifest entry; no follow-up round 3 items unless a later audit finds doc/Ghidra drift.
