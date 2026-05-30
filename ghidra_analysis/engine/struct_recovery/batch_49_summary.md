# Struct recovery batch 49/50

**Index:** 49 (`batches_50.json`)  
**Structs:** `TranslatorGuardRN` only  
**Program:** `bulanci.exe` (saved once at end)  
**Slice 49 RE agent (2026-05-30):** Re-verified layout; `save_program`; no struct mutation (already applied).

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `TranslatorGuardRN` | VERIFIED | 40 (0x28) | `create_struct` applied; `get_struct_layout` confirms |

## Evidence summary

- **Allocator / users:** MSVC `_CallSETranslator` builds a stack `TranslatorGuardRN` and registers it on `FS:[0]`; `TranslatorGuardHandler` is the only other consumer (CRT EH translation).
- **Proof path:** Disassembly of `_CallSETranslator@0x004478fd` (field init + FS link) and `TranslatorGuardHandler@0x004479d2` (cookie check, `___InternalCxxFrameHandler` args, unwind flag, ESP/EBP restore).
- **Deliverable:** [TranslatorGuardRN.md](./TranslatorGuardRN.md)

## Notes

- CRT internal type (VS2005 Release per Ghidra library ID); not SKIP — full layout recovered from binary evidence per batch scope.
- Decompiler may still show `param_2 + 0xNN` in `TranslatorGuardHandler` until re-analysis; struct type is 40 bytes in the datatype manager.
- Batch 50 (index 49 in 0-based terms) is the final manifest entry; this batch completes the CRT/EH tail of `struct_recovery_manifest.json`.
