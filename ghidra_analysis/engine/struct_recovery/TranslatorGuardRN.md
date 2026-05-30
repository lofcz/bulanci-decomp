# TranslatorGuardRN

## Status

**VERIFIED** (CRT / backlog **SKIP**) — MSVC CRT SEH translator guard registration node (VS2005–2010). Sized and laid out from `_CallSETranslator` stack instance and `TranslatorGuardHandler` only; no game-code allocation.

**Slice 49** (`batches_50.json` index 49, 2026-05-30): Re-verified `get_struct_layout` → 40 bytes / 10 fields; Ghidra `param_2` on `TranslatorGuardHandler@0x004479d2` is `TranslatorGuardRN *`. Stack `local_28` typing as `TranslatorGuardRN` in `_CallSETranslator` was attempted and reverted (decompiler mis-assigns fields).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Total size 0x28 (40 bytes) | `_CallSETranslator` @ `0x004478fd` | Stack `TGRN` at `[EBP-0x28]` … `DidUnwind` at `[EBP-0x4]`; last field span ends at offset 0x27 → 40 bytes |
| `DidUnwind` at +0x24 | `TranslatorGuardHandler` @ `0x004479d2` | `MOV dword ptr [EAX+0x24], 0x1` (unwind path); `CMP dword ptr [EAX+0x24], 0` |
| `EBP` at +0x20 | `TranslatorGuardHandler` @ `0x004479d2` | `MOV EBP, dword ptr [EBX+0x20]` before `JMP EAX` |
| `ESP` at +0x1c | `TranslatorGuardHandler` @ `0x004479d2` | `MOV ESP, dword ptr [EBX+0x1c]` |
| Cookie at +0x08 | `TranslatorGuardHandler` @ `0x004479d2` | `MOV ECX, [EAX+0x8]` then `XOR ECX, EAX` → `__security_check_cookie` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `EHRegistrationNode *` | `pNext` | `_CallSETranslator@0x004478fd` — `MOV [EBP-0x28], EAX` from `FS:[0]`; link head `FS:[0] := &TGRN` |
| 0x04 | 4 | `void *` | `pFrameHandler` | `_CallSETranslator@0x004478fd` — `MOV [EBP-0x24], 0x4479d2` (`TranslatorGuardHandler`) |
| 0x08 | 4 | `uint` | `RandomCookie` | `_CallSETranslator@0x004478fd` — `cookie ^ &TGRN` into `[EBP-0x20]`; `TranslatorGuardHandler@0x004479d2` — check `[pRN+8] ^ pRN` |
| 0x0c | 4 | `_s_FuncInfo *` | `pFuncInfo` | `_CallSETranslator@0x004478fd` — param copy to `[EBP-0x1c]`; `TranslatorGuardHandler@0x004479d2` — push `[EAX+0xc]` |
| 0x10 | 4 | `EHRegistrationNode *` | `pRN` | `_CallSETranslator@0x004478fd` — `[EBP-0x18]`; `TranslatorGuardHandler@0x004479d2` — push `[EAX+0x10]` |
| 0x14 | 4 | `int` | `CatchDepth` | `_CallSETranslator@0x004478fd` — `[EBP-0x14]`; `TranslatorGuardHandler@0x004479d2` — push `[EAX+0x14]` |
| 0x18 | 4 | `EHRegistrationNode *` | `pMarkerRN` | `_CallSETranslator@0x004478fd` — `[EBP-0x10]`; `TranslatorGuardHandler@0x004479d2` — push `[EAX+0x18]` |
| 0x1c | 4 | `void *` | `ESP` | `_CallSETranslator@0x004478fd` — `MOV [EBP-0xc], ESP`; `TranslatorGuardHandler@0x004479d2` — restore |
| 0x20 | 4 | `void *` | `EBP` | `_CallSETranslator@0x004478fd` — `MOV [EBP-0x8], EBP`; `TranslatorGuardHandler@0x004479d2` — restore |
| 0x24 | 4 | `int` | `DidUnwind` | `_CallSETranslator@0x004478fd` — zero init `[EBP-0x4]`; `TranslatorGuardHandler@0x004479d2` — set/test +0x24 |

## Ghidra apply

```
create_struct TranslatorGuardRN (10 fields, 40 bytes)
get_struct_layout TranslatorGuardRN → Size: 40 bytes
save_program bulanci.exe
```

Applied layout (Ghidra may auto-rename some members):

| Offset | Size | Type | Name |
|--------|------|------|------|
| 0 | 4 | `EHRegistrationNode *` | `pNext` |
| 4 | 4 | `void *` | `pFrameHandler` |
| 8 | 4 | `uint` | `dwRandomCookie` |
| 12 | 4 | `_s_FuncInfo *` | `pFuncInfo` |
| 16 | 4 | `EHRegistrationNode *` | `pRN` |
| 20 | 4 | `int` | `nCatchDepth` |
| 24 | 4 | `EHRegistrationNode *` | `pMarkerRN` |
| 28 | 4 | `void *` | `pESP` |
| 32 | 4 | `void *` | `pEBP` |
| 36 | 4 | `int` | `nDidUnwind` |

## UNK

- **Offset 0 dual use**: On `_CallSETranslator` with `pExcept == 0x123` (`CSET_SPECIAL`), `param_2` is a `void **` and `*(void**)param_2 = 0x4479a6` (`ExceptionContinuation`); not a valid `pNext` in that mode (`_CallSETranslator@0x004478fd`).
- **`ENABLE_EHTRACE` trailing `trace_level`**: Present in newer MSVC sources; not observed in this VS2005 binary (size stops at 0x28).
- **Game xrefs**: None — CRT-only (`TranslatorGuardHandler` data ref from `_CallSETranslator`; handler @ `0x4479d2`).

## Consumers (leaf CRT only)

| Function | Address | Role |
|----------|---------|------|
| `_CallSETranslator` | `0x004478fd` | Builds stack guard at `[EBP-0x28]`; links `FS:[0]`; stores `TranslatorGuardHandler` @ `+0x04` |
| `TranslatorGuardHandler` | `0x004479d2` | SEH filter: cookie @ `+0x08`, `___InternalCxxFrameHandler` args from `+0x0c..+0x18`, unwind `nDidUnwind` @ `+0x24`, ESP/EBP restore |

No game-code xrefs.

## References

- `TranslatorGuardHandler` — `0x4479d2` (mapping.csv)
- `_CallSETranslator` — `0x4478fd`
- MSVC `trnsctrl.cpp` (`TranslatorGuardRN` / `_TranslatorGuardHandler`) — corroborates field order, not used as primary evidence
