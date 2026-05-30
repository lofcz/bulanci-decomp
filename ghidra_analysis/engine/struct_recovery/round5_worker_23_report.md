# Round 5 — worker 23/50 — CGameView UNK fields

## Task

| Field | Value |
|-------|-------|
| **worker** | 23 / 50 |
| **mode** | WRITE |
| **struct** | `CGameView` |
| **scope** | UNK offsets from [CGameView.md](./CGameView.md) pre-R5 list |
| **supersedes** | Misnamed `dwView_flags`, `pChain_pad_48`, `pHeader_tail_58`, `child_chain` |

## Status

**DONE**

## Disassembly evidence

| Offset | Name (after) | Instruction | Function @ address |
|--------|--------------|-------------|------------------|
| `+0x08` | `dwField_08` | `MOV dword ptr [ESI+0x8],EDI` (EDI=0) | `CDSChained_ctor` `0x00403309` |
| `+0x0C` | `dwField_0c` | `MOV dword ptr [ESI+0xc],EDI` | `CDSChained_ctor` `0x0040330c` |
| `+0x40` | `dwChainRoot` | `MOV dword ptr [ECX+0x40],EAX` (EAX=0) | `CDSChained_ResetChainCounters` `0x0042bec4` |
| `+0x44` | `wChainInit44` | `MOV word ptr [ECX+0x44],0x1` | `ResetChainCounters` `0x0042beb2` |
| `+0x46` | `wChainFlag46` | `MOV word ptr [ECX+0x46],AX` (AX=0) | `ResetChainCounters` `0x0042beb8` |
| `+0x48` | `wChainFlag48` | `MOV word ptr [ECX+0x48],AX` | `ResetChainCounters` `0x0042bebc` |
| `+0x4A` | `wChainFlag4a` | `MOV word ptr [ECX+0x4a],AX` | `ResetChainCounters` `0x0042bec0` |
| `+0x4C` | `pChainParent` | `MOV dword ptr [ECX+0x4c],EAX` | `ResetChainCounters` `0x0042beca` |
| `+0x50` | `dwChainField_50` | `MOV dword ptr [ECX+0x50],EAX` | `ResetChainCounters` `0x0042bec7` |
| `+0x54` | `pVftable_embeddedChain_IDSReferenced` | `MOV dword ptr [ESI+0x54],0x47f6d4` | `CDSChained_ctor` `0x00403352` |
| `+0x58` | `pVftable_embeddedChain_IDSChained` | `MOV dword ptr [ESI+0x58],0x47f6b8` | `CDSChained_ctor` `0x00403359` |
| `+0x5C` | `dwEmbeddedChainField_5c` | `MOV dword ptr [ESI+0x5c],EDI` | `CDSChained_ctor` `0x00403360` |
| `+0x60` | `dwEmbeddedChainField_60` | `MOV dword ptr [ESI+0x60],EDI` | `CDSChained_ctor` `0x00403363` |
| `+0x64` | `pOverlapEntity` | `MOV dword ptr [ECX+0x64],EDX` | `CBulanek_UpdateStateFromParams` `0x004178f7` |
| `+0x68..+0x6B` | gaming bytes / `bNetStateByte3` | `MOV dword ptr [ECX+0x68],EAX` | `UpdateStateFromParams` `0x004178fd` (packed dword) |
| `+0x44` (collision) | `wChainInit44` LSB | `TEST byte ptr [ECX+0x44],0x1` | `GetWorldCollisionRect` `0x00417210` |
| `+0x71` | `pPad_71` | — | No `+0x71` gameplay operand in program scan (R5) |

### Corrected `TM_SetFrameDelayOverrideMs` / `+0x44` confusion

| Address | Disasm | Interpretation |
|---------|--------|----------------|
| `0x00439724` | `MOV dword ptr [ECX+0x44],EAX` | Writes **caller’s** `+0x44` |
| `0x0041e629` (CBulanekCtor) | `LEA …, [EBX+0xa8]` then `CALL 0x00439720` | **ECX = `videoTrackManager`**, not view header → **`CDSObject.nFrameDelayOverrideMs`** |

Prior doc label **`dwView_flags`** on the **view** shell was incorrect; collision uses **`wChainInit44`** byte test, not frame-delay ms.

### `GetFirstChildView` / embedded chain

`CDSChained_GetFirstChildView@0x0042f7c0`: **`MOV EAX,[ECX+0x8]`** with **ECX = `this+0x54`** (embedded chain object), then **`MOV EAX,[EAX+0xc]`** — not a bare pointer field named `child_chain`.

## Ghidra deltas

- `modify_struct_field` — `dwField_08/0c`, `dwChainRoot`, `wChainInit44`, `wChainFlag46/48/4a`, `pChainParent`, `dwChainField_50`, embedded chain facet, `pOverlapEntity`, `bNetStateByte3`
- `add_struct_field` — chain-band ushorts/pointers where split from blob fields
- `set_decompiler_comment` @ `0x00403309`, `0x0042beb0`, `0x00439720`, `0x004178f0`
- `get_struct_layout CGameView` → **152 B** (unchanged size)
- `save_program bulanci.exe`

## Struct doc updates

- [CGameView.md](./CGameView.md) — full layout `+0x08..+0x71`, collision/TM note, R5 Ghidra apply

## Remaining UNK

- `dwField_08`, `dwField_0c`, `dwField_1c` — ctor-zero only.
- `dwEmbeddedChainField_5c/60` — ctor-zero only on shell.
- `pPad_71` — padding (no `.text` consumer).
- Per-flag semantics for `wChainFlag46/48/4a` beyond init.
- Retype `TM_SetFrameDelayOverrideMs` callee `this` to `CDSVideoPlayer *` / `CDSObject *`.
