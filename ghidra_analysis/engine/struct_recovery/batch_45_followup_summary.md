# Struct recovery batch 45 — follow-up round 2

**Status:** `HANDOFF_CLEAN`  
**Prior:** [batch_45_summary.md](./batch_45_summary.md)  
**Deliverables reviewed:** [bad_alloc.md](./bad_alloc.md), [_LocaleUpdate.md](./_LocaleUpdate.md)

## Prior batch follow-ups

None listed in `batch_45_summary.md` (no `## Follow-ups` section; no explicit follow-up / UNK / cleanup queue).

## Actions taken

| Action | Result |
|--------|--------|
| Read `batch_45_summary.md` + struct deliverables | Two CRT types: `bad_alloc` and `_LocaleUpdate`; both **SKIP** (layout documented, Ghidra not applied) |
| Cross-check docs vs batch evidence | `bad_alloc` = 12-byte `exception` derivative; `_LocaleUpdate` = 13-byte stack guard (fields 0/4/8/0xC) |
| `get_struct_layout bad_alloc` (bulanci.exe) | Size 1 placeholder (`/std/bad_alloc`); unchanged from batch 45 |
| `get_struct_layout _LocaleUpdate` (bulanci.exe) | Size 1 placeholder (`/Demangler/_LocaleUpdate`); unchanged from batch 45 |
| Ghidra mutations | **None** (CRT SKIP policy; no open follow-ups) |
| `save_program bulanci.exe` | **Not called** (no program changes) |

## Ghidra deltas

None.

## Remaining UNK (documented only — not batch queue)

From [bad_alloc.md](./bad_alloc.md) § UNK:

- Ghidra inheritance vs alias to `exception` (cosmetic typing).
- `FUN_0047ee91` atexit tail for singleton `DAT_004b85b0` (CRT lifecycle only).

From [_LocaleUpdate.md](./_LocaleUpdate.md) § UNK:

- Explicit `~_LocaleUpdate` symbol (teardown inlined at call sites, e.g. `getSystemCP@0x0044c8ab`).
- Padding after offset `0x0C` (no caller observed requiring >13 bytes).

## Handoff

Batch 45 / manifest index 45 (`bad_alloc`, `_LocaleUpdate`) is complete for struct recovery. Optional future work (out of scope): apply 13-byte `_LocaleUpdate` or 12-byte `bad_alloc`/`exception` chain in Ghidra if CRT namespace separation is added — see batch summary “Notes for batch 46+”. No follow-up round 3 items unless doc/Ghidra drift is found.
