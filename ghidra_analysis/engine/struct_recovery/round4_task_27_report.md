# Round 4 — task 27 report

## Task

| Field | Value |
|-------|-------|
| **id** | 27 |
| **title** | CDSChain_full dtor on 0xa4 object; CGame.chain vs CBulanciConfigStore parity |
| **priority** | critical |
| **manifest** | `todos_gather_r4_2.json` id 27 (R3 handoff blockers) |
| **supersedes** | R3 todo 27 ([round3_task_27_report.md](./round3_task_27_report.md)) |
| **types** | `CDSChain`, `CDSChain_full`, `CGame`, `CBulanci` |
| **addresses** | `0x0042fcd0`, `0x0040a380`, `0x00414d8a`, `0x0040a3d9`, `0x00414aeb` |

## Status

**DONE** — `CDSChain_dtor` is list-head-only (`CDSChain` @ `+0x64`); full `0xa4` teardown is `CBulanci_DestroyConfigStore` on `CGame.chain` (`CDSChain_full` @ `CGame+0x31`); no separate heap config object.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| List-head dtor only | `CDSChain_dtor@0x0042fcd0` | Sets IDSReferenced/IDSChained vtables; `CDSChained_ClearChildren`; no profile/keybinding/registry access |
| Dtor on `+0x64` not base | `CBulanci_DestroyConfigStore@0x0040a380` | Disasm `0x0040a3d9`: `LEA ECX,[ESI+0x64]` → `CALL 0x0042fcd0` after profile/level list clears |
| CGame embed teardown | `CGame_dtor@0x00414ca0` | `0x00414d8a`: `LEA ECX,[ESI+0x31]` → `CALL CBulanci_DestroyConfigStore` |
| Sole full-object ctor | `CGame_ctor@0x00414aeb` | Only xref to `CDSChain_ctor@0x0040a680` |
| Other `CDSChain_dtor` sites | xrefs | `CDSSafeStream_dtor` @ `+0x18`, `CDSStreamStorage_dtor` @ `+0x34`, `CLevelScore_dtor` @ `+0x14`, `CDSView_dtor` @ `+0x54` — all 20-byte embedded heads |
| `dwField_85` ctor-only | `CDSChain_ctor@0x0040a680` | `MOV dword [ESI+0x85], 8` @ `0x0040a6fc`; program search: no read of `[reg+0x85]` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CDSChain_full.dwField_85` | → `dwProfileCapacityMirror` |
| `set_function_prototype` | `CDSChain_dtor@0x0042fcd0` | `void __fastcall CDSChain_dtor(CDSChain *this)` |
| `set_function_prototype` | `CBulanci_DestroyConfigStore@0x0040a380` | `void __fastcall CBulanci_DestroyConfigStore(CDSChain_full *config)` |
| `set_decompiler_comment` | `0x0042fcd0`, `0x0040a3d9`, `0x00414d8a` | list-head vs full teardown documented |
| `force_decompile` | `CGame_dtor`, `CBulanci_DestroyConfigStore` | `DestroyConfigStore` uses `CDSChain_full` field names |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSChain.md](./CDSChain.md) — round 4 task 27 section; `dwProfileCapacityMirror`; dtor/parity UNK resolved

## Remaining UNK

- **`pAuxHeap`:** still teardown-only, never allocated (R3 verdict unchanged).
- **`CBulanciConfigStore` Ghidra struct (133 B):** stale vs `CDSChain_full` (164 B); `SaveConfigToRegistry` / related helpers still take `CBulanciConfigStore *` cast — replace or typedef in a later MCP pass.
- **CIntList @ `+0x79`:** three dwords match `CIntList` data/capacity/count; `nM_growthChunk` not present — not a nested 16-byte `CIntList` field.
- Decompiler `CDSChain::` prefix on some `CDSChained_*` call sites (`__thiscall` ECX limit, R3).
