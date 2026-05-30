# Round 4 — agent todo 01 report (R4 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **title** | CDSObject image MI +0x54/+0x5c and CGame cmdline dtor @0x4028d0 |
| **source** | handoff (`agent_todos_50_r4.json`; supersedes R3 todo 1) |
| **types** | CDSObject, CDSImage, CGame, CBulanci |
| **addresses** | `0x00425620`, `0x0042563a`, `0x00425460`, `0x004028d0`, `0x00401992` |
| **acceptance** | Ghidra names `CDSObject+0x54..+0x5c` MI tail; cmdline/audio teardown helper uses `(this->game).pCmdLine` not dword indices; `set_function_this_type` on `__thiscall` members; `save_program` |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `CDSObject+0x54` IDSChained MI vtable | `0x0042568a` | `CDSObject_CtorWithImage`: `this->pImage_vf_IDSChained = CDSImage::g_pCDSImage_vftable_IDSChained` |
| `CDSObject+0x58` stream-host MI | `0x00425693` | Same ctor: `this->pImage_vf_streamHost = CDSImage::vftable` |
| `CDSObject+0x5c` embed tail dword | `0x00425666` | `this->pImage_tail_5c = 0` (standalone `CDSImage` uses `m_chain` here) |
| Ghidra struct 96 B tail | MCP | `get_struct_layout CDSObject` → `pImage_vf_IDSChained` @ 84, `pImage_vf_streamHost` @ 88, `pImage_tail_5c` @ 92 |
| `CDSImage_ctor` on embed | `0x00425460` | `CDSImage::CDSImage_ctor(CDSImage *this, …)`; `nDefaultBppTag` @ image `+0x44` |
| Cmdline release on destroy | `0x00402913` | Disasm `MOV EAX,[ESI+0x4c4]`; post-retype decompile `(this->game).pCmdLine` + `CDsStringReleaseHeader` |
| Audio bank + `CGame` dtor | `0x00402943`–`0x00402967` | Decompile `(this->game).pReleaseOnDestroy`, `CDSPtrSlotVec_Resize(&(this->game).pAudioBankArray,0)`, `CGame_dtor(&this->game)` |
| `SetCmdLine` targets embed | `0x00401992` | Disasm `LEA ESI,[ECX+0x4c4]`; decompile `&(this->game).pCmdLine` + `CDsStringAssignFromLiteral` |
| Scalar dtor caller | `0x00402b03` | `CBulanci_DtorScalar` → `CALL CBulanci_DestroyEmbedFields` with `ECX=this` |

## Ghidra deltas

- Verified `CDSObject` fields `pImage_vf_IDSChained` / `pImage_vf_streamHost` / `pImage_tail_5c` @ `+0x54..+0x5c` (already present; `CDSObject_CtorWithImage` decompile confirmed).
- `set_function_prototype` `CBulanci_DestroyEmbedFields@0x004028d0` → `void __thiscall …(CBulanci *this)` (was `__fastcall` / `FUN_004028d0`).
- `set_function_this_type` `CBulanci *` @ `0x004028d0`, `0x00401992` (`CBulanci_SetCmdLine`).
- `rename_function_by_address` `FUN_004028d0` → **`CBulanci_DestroyEmbedFields`**.
- `set_decompiler_comment` @ `0x00402913`, `0x0042568a`.
- `force_decompile` @ `0x004028d0`, `0x00425620`.
- `save_program bulanci.exe`.

## Struct doc updates

- [CDSObject.md](./CDSObject.md) — R4 MI tail verify + UNK cleared for `+0x54..+0x5c` names.
- [CGame.md](./CGame.md) — `pCmdLine` consumers via typed destroy/setter paths.
- [CBulanci.md](./CBulanci.md) — `CBulanci_DestroyEmbedFields` / `SetCmdLine` Ghidra apply note.

## Remaining UNK

- `CBulanci_DestroyEmbedFields` still restores raw `field_0x4` / `field_0x10` / `field_0x18` vtable slots on `CDSApp` prefix (MI facet restore — separate `CDSApp` task).
- `CDSApp::FUN_0042b3d0` tail call at end of destroy helper — not retyped this round.
- Full MI thunk graph for `pImage_vf_streamHost` Load/Save adjustors — `CDSImage.md` / `bmp_decoder.md`.
