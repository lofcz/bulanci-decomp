# Round 3 — Task 42 report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 42 |
| **title** | Fix CBulanekCtor scheduler + videoTrackManager decompiler |
| **priority** | critical (blocker) |
| **acceptance** | `CBulanek::CBulanekCtor(CBulanek *this, …)` decompile uses `this->scheduler` @ `+0x88` and `this->videoTrackManager` @ `+0xA8`; not `CGameView::ods_image` / opaque `param_1+0x88` |

## 2. Status

**DONE** — `set_function_this_type(CBulanek *)` @ `0x0041e4b0` confirmed; decompiler shows named embeds and ApplyAction playback path.

## 3. Evidence

### 3.1 Struct layout (`get_struct_layout CBulanek`)

| Offset | Size | Field | Type |
|--------|------|-------|------|
| `+0x88` (136) | 24 | `scheduler` | `CDSUpdatedItem` |
| `+0xA0` (160) | 4 | `vftable_event` | `pointer` |
| `+0xA8` (168) | 72 | `videoTrackManager` | `CDSVideoPlayer` |

Total size **412** (`0x19c`).

### 3.2 `CBulanekCtor` @ `0x0041e4b0`

Prototype (post-fix):

```c
CBulanek * __thiscall
CBulanek::CBulanekCtor(CBulanek *this, CGame *pGame, uchar slotKind, …);
```

Key decompiler lines:

| Site | Decompiler | Asm intent |
|------|------------|------------|
| `+0x88` | `CDSUpdatedItem::CDSUpdatedItem_ctor(&this->scheduler)` | `LEA`/`PUSH` with `EDI = this+0x88` |
| `+0xA0` | `this->vftable_event = g_pCBulanek_vftable_event` | vtable patch on scheduler facet |
| `+0xA8` | `CDSObject::ConstructTrackManager(&this->videoTrackManager, …)` | embed `CDSVideoPlayer` 72 B |
| walk load | `_Globals::AddTrackSource(&this->videoTrackManager, …)` | four facing track sources |
| tail | `_Globals::BeginCurrentTrackPlayback(&this->videoTrackManager)` | initial playback |
| tail | `Scheduler_RegisterEventSlot(&this->scheduler, …, eventKind=7)` | player action scheduler |

Plate comment documents: `CDSUpdatedItem_ctor(this+0x88) ConstructTrackManager(this+0xa8)`.

### 3.3 `CBulanek_ApplyAction` @ `0x00420910`

Action `0` with `param_2 == 0`:

```c
_Globals::BeginCurrentTrackPlayback(&this->videoTrackManager);
```

Decompiler comment ties this to ctor `ConstructTrackManager` embed @ `+0xA8`.

## 4. Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `CBulanek *` @ `0x0041e4b0` | Confirmed in class `CBulanek`; auto-storage `this` |
| (prior) `set_function_prototype` | same | `CBulanek::CBulanekCtor` __thiscall |
| (prior) struct fields | `CBulanek` | `scheduler` / `videoTrackManager` types @ `+0x88` / `+0xA8` |
| plate / decompiler comments | ctor + ApplyAction | offset anchors for agents |
| `save_program` | `bulanci.exe` | persisted |

## 5. Struct doc

- `ghidra_analysis/engine/struct_recovery/CBulanek.md` — player extension table, ctor graph, agent notes.

## 6. Remaining UNK

- Prefix `+0x48..+0x87` still lists some `pChain_pad_*` / `pHeader_tail_*` names in Ghidra layout export (todo 41 follow-up); does not affect scheduler/track-manager decompile.
- `gaming_host` typing on `CGameView_InitGamingFields` call inside ctor (shared with task 4/16 pattern).

## 7. Blockers cleared

Prior blocker: ctor typed as `CGameView *` or flat `param_1` made `+0x88` decode as `ods_image` / anonymous `param_1+0x88`. **`set_function_this_type(CBulanek *)`** is the required fix; struct field names alone were insufficient.
