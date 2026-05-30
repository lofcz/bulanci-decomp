# Round 5 — Worker 25 Report

## Task

| Field | Value |
|-------|-------|
| **worker** | 25 / 50 |
| **round** | 5 |
| **title** | `CGaming.md` layout gaps |
| **mode** | WRITE |
| **structs** | `CGaming`, `CGame`, `CLevelScript`, `CPauseDlg`, `CExplosion` |

## Status

**DONE** — Ghidra gap **`+0x24C..+0x33F`** filled from `CGaming_ctor` disassembly and consumer xrefs; corrected **`wModalExitCode`** offset; renamed **`+0x31c`** entity-view root.

## Evidence

### Size and owner model

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `sizeof == 0x36C` | `CGame_StartGame` region | Stack `0x36C` coordinator (unchanged) |
| Owner is **`CGame *`**, not embed | `CGaming_ctor@0x004200cf` | `MOV [ESI+0x84],EDI` (`ownerGame` param); level script reads `[ESI+0x84]+0xCC` @ `0x00420278` |
| Prior **`CGame @+4`** wrong | `CGaming_ctor@0x0041ffd4` | `+0x04` stores **`0x00482748`** (IDSChained vtable), not `CGame_ctor` |

### Entity slots and vectors

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `apEntitySlots` @ `+0xC8`, 512 B | `CGaming_ctor@0x004200a7` | `PUSH 0x200` / `LEA EAX,[ESI+0xC8]` / `memset` |
| Slot read | `GetObjectAtSlotUnchecked@0x00482062` | `MOV EAX,[ECX+slot*4+0xC8]` |
| `bulletSlotVec` @ `+0x2C8` | `CGaming_ctor@0x0042001e` | `MOV [ESI+0x2C8],0` … `MOV [ESI+0x2D4],8` |
| Bullet cleanup | `CGaming_CleanupInactiveBullets@0x0041a2a0` | Uses `+0x2C8` / `+0x2D0` |

### Entity view root (`+0x31c`)

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Store HUD panel | `CGaming_ctor@0x00420263` | `MOV [ESI+0x31c],EAX` after `OperatorNew(0x68)` + `CDSChained_InitWithRect` |
| Blast collect walk | `CExplosion_CollectEntitiesInBlastRect@0x004183d3` | `MOV ECX,[ECX+0x31c]`; `ADD ECX,0x54`; `CDSChained_GetFirstChildView` |
| Clear all entities | `CGaming_ClearAllEntities@0x00419d20` | `GetFirstChildView(*(this+0x31c)+0x54)` loop |

### Modal / round-end tail

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| **`wModalExitCode` @ `+0x364`** (not `+0x360`) | `CGaming_OnCmd@0x0041d602` | `CMP word ptr [ESI+0x364],0xFFFF`; `MOV [ESI+0x364],AX` |
| Init sentinel | `CGaming_ctor@0x004200ee` | `MOV word [ESI+0x364],0xFFFF` |
| `nRoundEndWait` @ `+0x338` | `CGaming_IsRoundEndTransitionComplete@0x004168e0` | `CMP dword [ECX+0x338],0` |
| `dwWeaponSpawnerMode` @ `+0x368` | `CGaming_ctor@0x00420117` | `MOV dword [ESI+0x368],2` (admin path `=6` @ `0x00420132`) |

### Depth list + script + audio

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `pDepthInsertHead/Tail` | `CGaming_InsertEntityByDepth@0x004184fe` | `CMP/MOV [EBX+0x348]`; tail @ `+0x34c` @ `0x00418529` |
| `pLevelScript` | `CGaming_ctor@0x004202c9` | `MOV [ESI+0x344],EDI`; `CallExport` on resume/dtor |
| `dwMusicResourceId` | `CGaming_SetMusicAndSoundBank@0x0041ba67` | `MOV [ESI+0x350],EAX` |
| Intro / BGM players | `CGaming_ctor@0x00420174`, dtor `@0x0041b9..` | `[ESI+0x358]`, `[ESI+0x354]`, `[ESI+0x35c]` release |

### Player HUD views

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `apPlayerHudViews[4]` @ `+0x148` | `CGaming_CountActiveEntities@0x00416943` | `LEA EDX,[ECX+0x148]`; 4-iteration loop |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `add_struct_field` | `pOwnerGame` `CGame *` @ **132** | Owner pointer |
| `modify_struct_field` | `apEntitySlots` → `void *[128]` @ **200** | 512 B slot table |
| `add_struct_field` | `bulletSlotVec` `CDSPtrSlotVec` @ **712** | Active bullet vector |
| `add_struct_field` | `pEntityViewRoot` @ **796** | Former `field_0x31c` |
| `add_struct_field` | `pLevelScript`, depth heads, music/audio fields | **836..856** |
| `modify_struct_field` | `wModalExitCode` @ **868**; `pad_0x360` @ **864** | Fixed modal offset vs stale `+0x360` |
| Removed (implicit) | `CGame game` @ +4 | Superseded by MI + pointer model |
| `set_decompiler_comment` | `0x00420263` | Entity-view root note |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CGaming.md](./CGaming.md) — full linear layout tables (MI shell, slots, tail); corrected owner and modal offsets.

## Remaining UNK

- Semantic names for **`vecSlotVec_2d8` / `2e8` / `2f8`** (ctor/dtor only).
- **`pPad_33c`**, **`pad_0x360`**, **`+0x30C`** — zero-init, no reads.
- Decompiler still aliases some stores through **`(game).chain.*`** until `pOwnerGame` typing propagates to helpers.
