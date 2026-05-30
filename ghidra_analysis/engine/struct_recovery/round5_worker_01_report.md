# Round 5 — worker 01/50 report

## Task

| Field | Value |
|-------|-------|
| **worker** | 1 / 50 |
| **scope** | Gameplay spine band `0x00401000`–`0x00401F9F` (slice 1 of 50 × `0xFA0` bytes) |
| **targets** | `FUN_*` in `CBulanci` / pack-path / SEH helpers feeding `CGame` boot |
| **program** | `bulanci.exe` via `user-ghidra-mcp` |

## Status

**DONE** — all six `FUN_*` in slice resolved (five renamed with instruction-level proof; one left `FUN_*` pending dialog helper identity).

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Slice inventory | `0x00401000`–`0x00401F9F` | `search_functions_enhanced` name `FUN_00401*` → **6** symbols only |
| DirEnum path join | `0x004014b0` | Asm: `MOV EAX,[ECX]`; `ADD ECX,0x30`; `CALL CBulanci_FormatPathPair@0x0042e230`; `RET 4` |
| DirEnum ctx layout | `0x004014b0` | P-code: `ctx+0` base path, `ctx+0x30` suffix (`cFileName`); out param on stack |
| Callers (game boot) | `0x00402d19`, `0x0040a1be` | `CBulanci_dtor` tmp `DeleteFileW` loop; `CBulanci_BuildLevelResourceTable` `*.eap` scan |
| SEH lock teardown | `0x004011d0` | Asm: `PUSH ECX`; `CALL [DeleteCriticalSection]`; body 8 B |
| Lock offset proof | `0x004740b9` | `Unwind_004740b9`: `FUN_004011d0(*(frame-0x10)+0x48)` ↔ `CDSStreamStorage.lock` |
| `CDSStreamStorage` lock | `CDSStreamStorage.md` | Ctor `InitializeCriticalSection(this+0x48)`; dtor `DeleteCriticalSection` same |
| wstring vector elem dtor | `0x00401340` | Asm: `MOV EAX,[ECX]`; `TEST`; `LEA ECX,[EAX-0xc]`; `JMP CDsStringReleaseHeader@0x0042d2d0` |
| EH vector consumers | `0x00401340` | **141** xrefs — all `Unwind@*` / `_eh_vector_destructor_iterator_` sites |
| Heap member free | `0x00401520` | Asm: load `*ECX`; `PUSH`; `MOV ECX,0x4b7c94`; `CALL Runtime_Free@0x0042f720` |
| Slot vector unwind | `0x00401d70` | Asm: `PUSH 0`; `CALL CDSPtrSlotVec_Resize@0x00406340`; `RET` |
| Bank `+0x18` unwind | `0x00478f5f` | `Unwind_00478f5f`: `FUN_00401d70(*(frame-0x10)+0x18)` = `CDSAudioBank.slotVector` |
| Exception catch filter | `0x00401a70` | `Catch@00401b70` / `Catch@00401c30` call filter; uses `CDSException_GetMessageW`, vtbl `+8`, `CDSView_PostMessage_NullSafe` |
| Dialog callee UNK | `0x0040f2c0` | Still `FUN_0040f2c0` — no proven `CMsgDialog` symbol; filter rename withheld |

## Ghidra deltas

- `rename_function_by_address` `FUN_004014b0` → **`CBulanci_DirEnumCtx_FormatCurrentPath`**
- `rename_function_by_address` `FUN_004011d0` → **`eh_CDSStreamStorage_DeleteCriticalSection`**
- `rename_function_by_address` `FUN_00401340` → **`eh_CDsStringReleaseHeader`**
- `rename_function_by_address` `FUN_00401520` → **`eh_Runtime_FreeMemberPtr`**
- `rename_function_by_address` `FUN_00401d70` → **`eh_CDSPtrSlotVec_ResizeZero`**
- `set_function_prototype` on all five renamed symbols (see Evidence)
- `set_decompiler_comment` @ `0x004014f5`, `0x004011d1`
- `save_program bulanci.exe`

## Struct doc updates

- [CBulanci.md](./CBulanci.md) — `CBulanci_DirEnumCtx_FormatCurrentPath` in Key methods
- [CDSAudioBank.md](./CDSAudioBank.md) — unwind helper symbol for `slotVector` @ `+0x18`
- [CDSStreamStorage.md](./CDSStreamStorage.md) — cross-ref SEH lock helper

## Remaining UNK

- `FUN_00401a70` — MSVC `CDSException` catch **filter**; semantic name blocked until `FUN_0040f2c0` (modal dialog) is proven.
- `CBulanci_DirEnumCtx` — stack context struct not yet a Ghidra type (`void *this` on formatter); ctor/dtor already named @ `0x004013b0` / `0x00401450`.
- Next slice **worker 02**: `0x00401FA0`–`0x00402F3F`.
