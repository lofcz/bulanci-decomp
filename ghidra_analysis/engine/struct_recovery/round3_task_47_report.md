# Round 3 — task 47 report

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 47 |
| **title** | Verify CDSStrmResInfo pad_tail @ +0x20 AppendOrReuseStream read |
| **one_liner** | Verify CDSStrmResInfo `pad_tail` @ `+0x20`: sole consumer is `AppendOrReuseStream` (`MOV EDX,[EDI+0x20]`); `+0x14` is `streamExtent`, not `pEmbeddedResource`. |
| **acceptance** | Update CDSStrmResInfo.md; Ghidra mutation if evidence supports |
| **types** | CDSStrmResInfo, CDSResInfo, CDSStreamStorage |
| **addresses** | `0x00433f70`, `0x00433940`, `0x00433980`, `0x00434270` |

## 2. Status

**DONE** — Tail `+0x20` is a named **`dwFilterSliceAddend`** (`filterSliceAddend` in docs), read only on the append path. `+0x24` remains allocator padding with no consumer. `+0x14` on `CDSStrmResInfo` is **`streamExtent`** (`ulonglong`); `pEmbeddedResource` applies only to base **`CDSResInfo`** (24 B).

## 3. Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Sole `[EDI+0x20]` consumer | `CDSStreamStorage_AppendOrReuseStream@0x00433f70` | Asm `MOV EDX,dword ptr [EDI+0x20]` @ `0x0043405b` (`8b5720`); pushed as 4th arg to `CreateFilterSafeStream@0x00434760` |
| Decompiler names field | same | `entry->dwFilterSliceAddend` passed to `CDSStreamStorage_CreateFilterSafeStream` |
| Filter ctor adds slice extent | `CDSStreamStorage_CreateFilterSafeStream@0x00434760` | `CDSFilterStream_Ctor(..., sliceOffsetLo + sliceExtentLo, sliceOffsetHi + sliceExtentHi + carry, ...)` — `sliceExtentLo` is the `+0x20` dword |
| Append vs reuse branch | `AppendOrReuseStream@0x00433f70` | `CMP dword ptr [EDI+0x14],0` @ `0x00433fad` — zero `streamExtent` → append path that reads `+0x20`; non-zero → `CDSStreamStorage_AddRefHeldObject` |
| Serialize tail only `+0x14`..`+0x1f` | `CDSStrmResInfo_Serialize@0x00433940` | After `CDSResInfo_Load`: 8 B `@streamExtent`, 4 B `@dwStreamFlags`; **no** I/O at `+0x20` |
| Deserialize symmetric | `CDSStrmResInfo_Deserialize@0x00433980` | `CDSResInfo_Save` then 8+4 B write; **no** `+0x20` |
| Base release uses pointer @ `+0x14` | `CDSResInfo_ReleaseEmbeddedResource@0x00434270` | Comment + guard on `pEmbeddedResource`; must not run on stream entries where `+0x14` is extent |
| Ghidra struct 40 B | `get_struct_layout CDSStrmResInfo` | `streamExtent` @ `+0x14` (8), `dwStreamFlags` @ `+0x1c`, `dwFilterSliceAddend` @ `+0x20`, `dwPad_reserved_hi` @ `+0x24` |
| Base `CDSResInfo` | `get_struct_layout CDSResInfo` | `pEmbeddedResource` @ `+0x14` (24 B object only) |

### Interpretation

Historical `byte[8] pPad_tail` @ `+0x20` masked two dwords: the **low** dword is a runtime filter slice addend consumed when splicing a new stream; the **high** dword (`+0x24`) is never read. Factory `OperatorNew(0x28)` allocates both but does not initialize them (unlike `loaderAux` / `streamExtent.lo` at `+0x10`/`+0x14`).

## 4. Ghidra deltas

- Struct already applied: `dwFilterSliceAddend` @ `+0x20`, `dwPad_reserved_hi` @ `+0x24`; no `pad_tail` / `pEmbeddedResource` on `CDSStrmResInfo`.
- PRE comments present @ `0x00433940`, `0x00433980`, `0x0043405b` (inside `AppendOrReuseStream`), `0x00434270`.
- `save_program bulanci.exe` (verification pass).

## 5. Struct doc updates

- [CDSStrmResInfo.md](./CDSStrmResInfo.md) — layout table and round 3 agent todo 47 notes (pre-updated; aligned with this report).

## 6. Remaining UNK

- `dwPad_reserved_hi` @ `+0x24`: no xref (alignment / reserved).
- Whether `dwFilterSliceAddend` is ever non-zero from pack data (not in serialize path; may be stack-key / loader only).
