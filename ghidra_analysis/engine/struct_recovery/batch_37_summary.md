# Struct recovery batch 37/50

**Index:** 37 (`batches_50.json`)  
**Structs:** `CDSResourceException`, `CDSResourceSign`  
**Program:** `bulanci.exe` (saved once at end)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSResourceSign` | VERIFIED | `0x28` | Applied — 10 fields, 40 bytes |
| `CDSResourceException` | PARTIAL | `0x44` | Applied — 9 fields, 68 bytes; `+0x14..+0x3B` left as `undefined1[40]` |

## Evidence highlights

- **CDSResourceSign** — Editor colophon record (ClassID 94). Factory `OperatorNew(0x28)` at `0x00434930`; ctor at `0x00434540` sets four vptrs and date/strings; `WriteToStream` / `ReadFromStream` at `0x00434310` / `0x004343a0` pin `+0x14` date, `+0x18`/`+0x1c` wide strings, `+0x20` flag byte. Cross-ref: `ghidra_analysis/formats/sign_record.md`.
- **CDSResourceException** — `CDSException` subclass for bad resource open. `OperatorNew(0x44)` at `0x00439394` and throw helper `0x004346f0`; `CDSException_InitFields(this, 6, 8, 1)`; derived `resourceId` at `+0x3C`, formatted message buffer at `+0x40` (`What` / dtor string release).

## Deliverables

- `CDSResourceException.md`
- `CDSResourceSign.md`
- `batch_37_summary.md` (this file)

## Follow-ups

- Resolve `CDSException` base layout (`+0x14..+0x3B`) in batch 29 or a dedicated exception pass.
- Reconcile `CDSResourceSign_dtor` `param_1[8]` string release vs. `flagByte` at `+0x20`.
