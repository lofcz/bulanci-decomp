# Round 4 — Task 42 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 42 |
| **round** | 4 (R3 closure verify — MCP `set_function_this_type` + decompile) |
| **title** | Verify R3 closure: Fix CBulanekCtor scheduler + videoTrackManager decompiler |
| **one_liner** | `CBulanekCtor` must decompile with `this->scheduler` @ `+0x88` and `this->videoTrackManager` @ `+0xA8`, not `CGameView::ods_image` / opaque `param_1+0x88`. |
| **prior** | [round3_task_42_report.md](./round3_task_42_report.md) |
| **structs** | CBulanek, CDSUpdatedItem, CDSVideoPlayer, CGame |

## Status

**DONE** — R4 re-verified disasm + decompiler; `set_function_this_type(CBulanek *)` confirmed @ `0x0041e4b0`; ctor and `CBulanek_ApplyAction` show named embeds.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `scheduler` @ `CBulanek+0x88` (136) | struct | `get_struct_layout CBulanek` → `CDSUpdatedItem scheduler` offset **136**, size 24 |
| `videoTrackManager` @ `+0xA8` (168) | struct | `CDSVideoPlayer videoTrackManager` offset **168**, size 72; total **412** B |
| ctor `this` type | `0x0041e4b0` | `CBulanek * __thiscall CBulanek::CBulanekCtor(CBulanek *this, …)` |
| `CDSUpdatedItem_ctor(&scheduler)` | `0x0041e4b0` | Decompile: `CDSUpdatedItem::CDSUpdatedItem_ctor(&this->scheduler)`; asm `LEA EDI,[ESI+0x88]` @ `0x0041e515` |
| `ConstructTrackManager` embed | `0x0041e4b0` | Decompile: `CDSVideoPlayer::ConstructTrackManager(&this->videoTrackManager,…)`; asm `LEA ECX,[ESI+0xa8]` @ `0x0041e530` |
| Walk tracks on mgr | `0x0041e4b0` | `_Globals::AddTrackSource(&this->videoTrackManager,…)` loop; `BeginCurrentTrackPlayback(this_00)` with `this_00 = &videoTrackManager` |
| Scheduler register | `0x0041e4b0` | `Scheduler_RegisterEventSlot(this_01,…,7)` with `this_01 = &this->scheduler`; asm `LEA EDI,[ESI+0x88]` @ `0x0041ea02` |
| ApplyAction playback | `0x00420910` | `param_2==0` → `BeginCurrentTrackPlayback(&this->videoTrackManager)`; asm `LEA ECX,[ESI+0xa8]` @ `0x0042096f` |
| Plate comment | `0x0041e4b0` | `CDSUpdatedItem_ctor(this+0x88) ConstructTrackManager(this+0xa8)` |

### Decompiler acceptance (verified)

- No `ods_image` / `param_1+0x88` on ctor path when `this` is `CBulanek *`.
- Comments explicitly distinguish scheduler vs `CGameView.ods_image`.

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x0041e4b0` | Confirmed in class `CBulanek`; `this` = `CBulanek *` |
| `decompile_function` | `0x0041e4b0`, `0x00420910` | Named `scheduler` / `videoTrackManager` accesses |
| `get_struct_layout` | `CBulanek` | 412 B; `scheduler` @ 136, `videoTrackManager` @ 168 |
| `search_instructions` | `CBulanekCtor` | `LEA EDI,[ESI+0x88]` ×2; `LEA ECX/EBP,[ESI+0xa8]` ×3 |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CBulanek.md](./CBulanek.md) — R4 todo 42 verify log.

## Remaining UNK

- Prefix band `+0x48..+0x87` still exports `pChain_pad_*` / `pHeader_tail_*` in Ghidra layout (todo **41**); does not affect scheduler/track-manager decompile.
- `CGameView_InitGamingFields((CGameView *)this)` cast on shared prefix (todo **4** pattern).
- `pWalkTrackSources` vs Ghidra name `pWalkTrackSources` (array @ +0x14C) — cosmetic naming vs doc `apWalkTrackSources`.
