# CDSChain_LoadConfigFromRegistry @ 0x0040a440

## Status

**VERIFIED** — full pipeline, on-disk record order, stack locals, SEH catch/finally, and `CDSChain_full` field targets proven from disassembly + decompile. Mirror save path: `CBulanci::SaveConfigToRegistry` @ `0x00409cd0`.

## Entry

| Item | Value |
|------|-------|
| Symbol | `CDSChain_LoadConfigFromRegistry` |
| Address | `0x0040a440` |
| Size | `0x1ef` |
| Convention | `__fastcall` |
| Parameter | `CDSChain_full *this` (ECX) |
| Sole caller | `CDSChain_ctor` @ `0x0040a680` (`CGame_ctor` → `&game.chain`) |

Registry: `HKEY_LOCAL_MACHINE\Software\SleepTeam\Bulanci`, value **`Config`** (`REG_BINARY`), via `RegQueryBinaryStream` @ `0x00438160`. Subkey literal: `PTR_g_wszRegSubKeySleepTeamBulanci_004ae000`.

---

## Stack frame (parent @ 0x0040a440)

Frame: `LEA EBP,[ESP-0x5c]` / `SUB ESP,0x5c` then `SUB ESP,0x30` for saves.

| EBP offset | Size | Type (C++) | Evidence |
|------------|------|------------|----------|
| `+0x00` | `0x48` | `CDSGZipStream gzip` | `CDSGZipStream_Ctor` `LEA ECX,[EBP]` @ `0x0040a4d8`; success dtor `LEA ECX,[EBP]` @ `0x0040a5e1` |
| `+0x0c` | — | `CDSGZipStream *` read face | all `ReadBytes` use `LEA ECX,[EBP+0xc]` @ `0x0040a4e6`… |
| `+0x48` | 4 | `CDSGZipStream *pGzipForEh` | catch @ `0x0040a62f`: `MOV ECX,[EBP+0x48]` — slot **immediately after** 72-byte object (`sizeof(CDSGZipStream)==0x48`) |
| `+0x4c` | 4 | `CDSChain_full *this` | `MOV [EBP+0x4c],EBX` @ `0x0040a472` after `MOV EBX,ECX` |
| `+0x50` | 8+ | `CDSRegKey regKey` | `LEA ECX,[EBP+0x50]` @ `0x0040a49d` |
| `+0x54` | 4 | `wchar_t *` / string stack | `MOV [EBP+0x54],ESP` @ `0x0040a480` |
| `-0x3c` | `0x2c` | `CDSEasyMemStream mem` (`0x2c` used; type 44 B) | `LEA ECX,[EBP-0x3c]` @ `0x0040a4af` |
| `-0x30` | 4 | stream ptr temp | `RegQueryBinaryStream` arg @ `0x0040a4c1` |
| `-0x4` | 1 | SEH try-state | `0→1→2→3→4→0xFF` |
| `-0xc` | 4 | saved `ExceptionList` | `LEA EAX,[EBP-0xc]` @ `0x0040a464` |

`get_struct_layout CDSGZipStream` → **72 bytes (`0x48`)**, confirming `EBP+0x48` is the EH spill **pointer slot** after the embedded object body.

---

## Catch @ 0x0040a62f — type at `[EBP+0x48]`

**Answer:** `CDSGZipStream *` (`pGzipForEh`), addressing the stack-local **`CDSGZipStream`** whose storage is **`[EBP+0 .. EBP+0x47]`**.

Disassembly:

```
0040a62f  MOV ECX, dword ptr [EBP+0x48]   ; pGzipForEh
0040a632  MOV EDX, dword ptr [ECX]        ; IDSReferenced vtable @ object+0 (0x4805e8)
0040a634  MOV EAX, dword ptr [EDX+0x8]    ; slot 2 → FUN_00401660
0040a637  CALL EAX                        ; __thiscall(ECX = pGzipForEh)
```

| Claim | Evidence |
|-------|----------|
| `pGzipForEh` points at stack gzip | `sizeof(CDSGZipStream)==0x48`; object at `[EBP+0]`; spill at `[EBP+0x48]`; unwind state 2 uses `LEA ECX,[EBP]` → same object @ `0x00475320` |
| Virtual target is **not** full `CDSGZipStream_dtor` | Success path calls `CDSGZipStream_dtor` @ `0x004098a0` directly; catch uses vtable **`+0x8`** |
| Vtable `0x4805e8` slot 2 | `master_vtable_catalog.md`: `FUN_00401660` (shared `IDSReferenced` / `IDSStream_ReleaseRefcount` helper) |
| Then factory defaults | `MOV ESI,[EBP+0x4c]`; `CALL CBulanci_InstallFactoryDefaults@0x0040a290`; `[ESI+0x78]=1` |
| Resume finally | `MOV EAX,0x0040a64d` → `CDSChain_LoadConfigFromRegistry_finally` |

**C++ (modern, no SEH):** `CDSGZipStream` RAII destructor on scope exit; on error call `CBulanci_InstallFactoryDefaults(this)` and set `bField_78 = 1` — no manual `[EBP+0x48]` spill.

---

## Gzip payload byte order (decompressed)

Sequential `CDSGZipStream::ReadBytes` @ `0x00435220`. All multi-byte integers **little-endian**.

| # | Size | Destination (`CDSChain_full`) | Evidence |
|---|------|----------------------------------|----------|
| 1 | 1 | `bConfigSignature` @ `+0x04` | @ `0x0040a4e2`; must equal **`0xB4`** else `CDSSimpleException_Throw(9,0xB)` @ `0x0040a4fb` |
| 2 | 1 | loop counter `local_5` | @ `0x0040a503` |
| 3a | 6 × N | heap nodes → `pRegistryProfileList` @ `+0x79` | while `local_5 != 0`: dec, `OperatorNew(6)`, read 6, `CIntListInsertSortedOrAppend(&+0x79, node, 0, 1)` @ `0x0040a51f`–`0x0040a53e` |
| 4 | 6 × 6 | `keyBindings` @ `+0x0D` | `for (i=0; i<6; i++)` @ `0x0040a547`: read 1+1 bytes @ `+0x0d+i*6`, `FUN_0042e2f0` @ `+0x0f+i*6` (CDSString) |
| 5 | 4 | `dwProfileUserDword` @ `+0x31` | @ `0x0040a57f` |
| 6 | 14 | `optionBlock` @ `+0x56` | `ReadBytes(..., 0x0e)` @ `0x0040a58a` |
| 7 | 4 | `nPanBindingIndex` @ `+0x49` | @ `0x0040a598` |
| 8 | 4 | `nReservedAudioDword1` @ `+0x4d` | @ `0x0040a5a6` |
| 9 | 4 | `nReservedAudioDword2` @ `+0x51` | @ `0x0040a5b4` |
| 10 | — | `strConfigTail` @ `+0x45` | `FUN_0042e2f0` @ `0x0040a5c9` (length-prefixed string — see `FUN_0042e2f0` @ `0x0042e2f0`) |

After reads:

| Step | Evidence |
|------|----------|
| `bField_78 = 1` | @ `0x0040a5db` |
| `(*(code**)(*(void**)(this+0x68) + 0x10))(this+0x68, gzipReadFace)` | `MOV EAX,[EBX+0x68]`; load `[EAX+0x10]`; `LEA ECX,[EBX+0x68]`; `PUSH [EBP+0xc]`; `CALL EAX` @ `0x0040a5ce`–`0x0040a5df` — **IDSChained** vtable slot **4** (`+0x10`), arg = gzip read `this` |
| `bConfigSignature = 0xB4` | @ `0x0040a5e4` (rewrite even if read differed before throw path) |
| `bRegistryCommitFlag = 1` | @ `0x0040a5e8` (`+0x55`) |

**Key-binding tuple (6 bytes):**

| Offset in tuple | Content |
|-----------------|--------|
| `+0` | DIK / scan byte 1 |
| `+1` | DIK / scan byte 2 |
| `+2..+5` | `CDSString` handle via `FUN_0042e2f0` (stream length-prefixed narrow → UTF-16 string) |

**Profile record (6 bytes):** opaque; inserted into sorted int-list at `+0x79` (same layout as factory defaults in `CBulanci_InstallFactoryDefaults` @ `0x0040a290`).

---

## Pipeline (ordered)

```
1. CDsStringAssignFromLiteral → stack path for reg open
2. CDSRegKey_ctor(&regKey, HKLM, 0, sam=0xF003F)     @ 0x00409510
3. CDSEasyMemStream_ctor(&mem, 0x1000, 0x800)         @ 0x00409170
4. RegQueryBinaryStream(&regKey, L"Config", &mem)     @ 0x00438160
5. CDSGZipStream_Ctor(&gzip, &mem.stream, write=0)    @ 0x00435960
6. [read table above]
7. IDSChained vfn dispatch (slot +0x10)
8. CDSGZipStream_dtor(&gzip)                          @ 0x004098a0
9. CDSEasyMemStream_dtor(&mem)                      @ 0x00409270
10. CDSRegKey_CloseKey if handle non-null            @ 0x00437b00
11. CDsStringReleaseHeader(path string) if set
12. finally: CDSAudio_SetPanPreview(nPanBindingIndex) @ 0x0040a650
```

SEH: handler `LAB_00475330` / FuncInfo `@ 0x004a69dc`; catch `@ 0x0040a62f`; unwind dtors for regKey/mem/gzip per `@ 0x00475310`.

---

## Reference C++ (reimplementation)

See `src/bulanci/CDSChain.cpp` — `CDSChain_LoadConfigFromRegistry`. Uses `CDSChain_full_layout` offsets matching `CDSChain.md`.

**Finally (always):** `CDSAudio_SetPanPreview(this->nPanBindingIndex);` — not optional; see [`FUN_0040a650.md`](FUN_0040a650.md).

## UNK

- Exact symbolic name of IDSChained vtable slot `+0x10` (registry load callback taking `CDSGZipStream*`).
- Whether MSVC writes `pGzipForEh` explicitly in asm or only via EH metadata (runtime value equals `&gzip` at `[EBP+0]` when catch runs after gzip ctor).
