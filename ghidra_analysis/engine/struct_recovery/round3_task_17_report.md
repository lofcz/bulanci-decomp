# Round 3 — Task 17 Report

## 1. Task

| Field | Value |
|-------|--------|
| **id** | 17 |
| **title** | Prove CPoem wstrHandle vs pCachedText after Deserialize |
| **one_liner** | After pack load, is the poem UTF-16 at primary `+0x04` (`pCachedText` / decompiler `this+4`) or at `+0x18` (`wstrHandle`)? |
| **acceptance** | Update CPoem.md; Ghidra struct field + MI comments if evidence supports |
| **source** | handoff (`agent_todos_50_r3.json`) |
| **structs** | `CPoem`, `CPoemScroller`, `CDSFileStream` |

## 2. Status

**DONE** — one CDS wide-string handle lives at primary **`+0x18`** (`pWstrHandle` in Ghidra). Primary **`+0x04`** is the second MI vtable (`pVftable_IDSFacet04`); Deserialize never overwrites it. There is **no** separate `pCachedText` buffer.

## 3. Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Factory installs vtable @ `+0x04`, zeroes string @ `+0x18` | `CPoem_factory@0x00409ab0` | Decompile: `pVftable_IDSFacet04 = …0x4804cc`; `pWstrHandle = NULL` |
| Deserialize is IDSStream MI thunk | `CPoem__Deserialize@0x00409100` | Asm: `ADD ECX,0x4` then `JMP CDsString_ReadWStringFromStream@0x0042e140`; entry ECX = stream subobject @ primary `+0x14` → after add, **`+0x18`** |
| ReadWString stores into `this` handle slot | `CDsString_ReadWStringFromStream@0x0042e140` | `CDsStringAssignFromHandle(this, …)` writes `*(void**)this`; MI-adjusted `this` == `&poem->pWstrHandle` |
| Decompiler `this+4` is MI artifact | `CPoem__Deserialize@0x00409100` | Typed as primary `CPoem*` shows `&pVftable_IDSFacet04`; runtime target is **`+0x18`**, not `+0x04` |
| GetText exports same handle | `CPoem__GetText@0x00409870` | Asm: `ADD ECX,0x4` on stream ECX; `CDsStringAssignFromHandle` then `CDsString_WriteHandleToStream` |
| Consumer reads poem text @ `+0x18` | `CPoemScroller_PickNextPoem@0x00425df0` | `MOV EAX,[ESI+0x18]` / decompile: `*(LPCWSTR *)(poem + 0x18)` → `TextShaper_LayOutAndRender` (measure + rasterize) |
| Destructor releases @ `+0x18` only | `CPoem::Destructor@0x004094a0` | Asm @ `0x004094ca`: `MOV EAX,[ESI+0x18]`; `CDsStringReleaseHeader(EAX-0xc)` |
| No string xref at primary `+0x04` | `CPoem` cluster | Factory sole writer of `+0x04` (vtable ptr); no `CDsString*` consumer at `+0x04` |

### MI adjustor chain (Deserialize)

```text
vtable call on IDSStream face (+0x14)
  ECX = poem + 0x14
CPoem__Deserialize@0x00409100:
  ADD ECX, 4          ; ECX = poem + 0x18  (wstrHandle slot)
  JMP ReadWStringFromStream
CDsString_ReadWStringFromStream:
  CDsStringAssignFromHandle(this=+0x18, stream)
```

Prior naming confusion: decompiler displays `&this->pVftable_IDSFacet04` because primary-base `this+4` coincides with the **offset within the stream subobject**, not because the poem string is stored in the facet vtable slot.

### wstrHandle vs pCachedText

Both names refer to the same CDS refcounted wide-string handle (`wchar_t*` with 12-byte header at `ptr-0xc`). After Deserialize there is **one** materialized copy at `+0x18`; PickNextPoem reads it directly (no lazy cache at `+0x04`).

## 4. Ghidra deltas

- Struct `CPoem` (28 B): `pVftable_IDSFacet04` @ `+0x04`, `pWstrHandle wchar_t*` @ `+0x18` (`get_struct_layout CPoem`).
- Decompiler comment @ `CPoem__Deserialize@0x00409100` (IDSStream MI → `+0x18`).
- Decompiler comment @ `CPoem__GetText@0x00409870` (export `+0x18`).
- Decompiler comment @ `CPoemScroller_PickNextPoem@0x00425df0` (`poem+0x18` LPCWSTR).
- Disassembly comment @ `CPoem::Destructor@0x004094ca` (`[ESI+0x18]` release).
- `save_program bulanci.exe`.

## 5. Struct doc updates

- [CPoem.md](./CPoem.md) — layout `wstrHandle` @ `+0x18`; R3 todo 17 verified section; corrected destructor address `0x004094a0`.

## 6. Remaining UNK

- MI deleting-destructor thunks at `+0x04`, `+0x0c`, `+0x10`, `+0x14` bases (defer to `IDSStream` / `CDSFileStream` family pattern).
- Whether `GetText` is invoked on shipped overlay paths (Serialize round-trip exists; menu uses pool-loaded instances only).
