# Round 4 — Task 17 Report

## Task

| Field | Value |
|-------|--------|
| **id** | 17 |
| **title** | Plate CPoem IDSStream MI thunks; xref GetText |
| **priority** | medium |
| **source** | handoff (R4) |
| **supersedes** | R3 todo 17 (`pWstrHandle` @ `+0x18` vs `+0x04`) |
| **types** | `CPoem`, `CDSFileStream`, `IDSStream`, `CPoemScroller` |
| **addresses** | `0x00409100`, `0x00409870`, `0x004094a0`, `0x00425df0` |

## Status

**DONE**

## Problem (R4 handoff)

R3 proved poem UTF-16 lives at **`pWstrHandle` @ `+0x18`** and documented decompiler comments on Deserialize/GetText. R4 required **plate comments** on IDSStream MI thunks, **prototype** cleanup, and a **GetText xref** survey (vtable-only vs live call sites).

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Deserialize MI adjust | `CPoem_Deserialize@0x00409100` | Asm: `ADD ECX,0x4` → `JMP CDsString_ReadWStringFromStream@0x0042e140` |
| GetText MI adjust | `CPoem_GetText@0x00409870` | Asm: `ADD ECX,0x4`; `CDsStringAssignFromHandle` + `CDsString_WriteHandleToStream` |
| GetText vtable only | `get_xrefs_to@0x00409870` | Single DATA ref `0x004804ac` (IDSStream vtable slot+5) |
| GetText indirect consumer | `CDSCollection_SerializeElement@0x0042ff20` | `CheckedVirtualBaseCast(...,6)` then `(**)(*piVar2 + 0x14)(this)` |
| Save calls SerializeElement | `CDSCollection_Save@0x00431210` | Loop `CALL CDSCollection_SerializeElement` @ `0x00431249` |
| Deserialize indirect consumer | `CDSCollection_DeserializeElement@0x0042fd40` | `(**)(*piVar1 + 0x10)(param_1)` after `InitializeByClassId` |
| Load calls DeserializeElement | `CDSCollection_Load@0x00431360` | Per-element factory + Deserialize vcall |
| Menu reads `+0x18` only | `CPoemScroller_PickNextPoem@0x00425df0` | `*(LPCWSTR *)(poem + 0x18)` → `TextShaper_LayOutAndRender` |
| Destructor releases `+0x18` | `CPoem::Destructor@0x004094a0` | `[ESI+0x18]` → `CDsStringReleaseHeader(ptr-0xc)` @ `0x004094ca` |
| Ghidra struct | `get_struct_layout CPoem` | **28 B**; `pVftable_IDSStream` @ `+0x14`, `pWstrHandle` @ `+0x18` |
| MI deleting dtors | `0x00409430` / `0x00409440` / `0x00409460` | `SUB ECX,0xc` / `0x14` / `0x4` → `ScalarDeletingDestructor` |

### IDSStream vtable `0x480498` (CPoem `+0x14`)

| Vtable slot | Address | Symbol |
|-------------|---------|--------|
| +3 (`0x0c`) | `0x00409440` | `DeletingDestructorThunk_12` |
| +4 (`0x10`) | `0x00409100` | `Deserialize` |
| +5 (`0x14`) | `0x00409870` | `GetText` |

## Ghidra deltas

1. **`set_plate_comment`** — `Deserialize`, `GetText`, `DeletingDestructorThunk_4/10/12` (MI adjustor + consumer notes).
2. **`set_function_prototype`** — `uchar __thiscall CPoem_Deserialize(CPoem *, int *)`; `uchar __thiscall CPoem_GetText(CPoem *, void *)` (per `mapping.csv`).
3. **`set_decompiler_comment`** — `CDSCollection_SerializeElement@0x0042ff20`, `CDSCollection_DeserializeElement@0x0042fd40` (vcall → GetText/Deserialize).
4. **`set_disassembly_comment`** @ `0x004094ca` — `pWstrHandle` release.
5. **`save_program bulanci.exe`**

## Struct doc updates

- [CPoem.md](./CPoem.md) — IDSStream vtable slot table; GetText xref chain; MI dtor plates.

## Remaining UNK

- `set_function_this_type` with `IDSStream *` on Deserialize/GetText (decompiler still shows `&pVftable_IDSFacet04` at `this+4` artifact).
- IDSStream vtable slots 0–2 on `0x480498` (shared `CDSFileStream` stubs).
- Whether any non-collection overlay path invokes GetText at runtime (pack save/load confirmed; main-menu scroller uses deserialized `+0x18` only).
