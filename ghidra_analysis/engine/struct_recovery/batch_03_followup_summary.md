# Struct recovery batch 03 — follow-up round 2

**Prior:** [`batch_03_summary.md`](batch_03_summary.md)  
**Structs:** `CAnim`, `CBitmap`  
**Status:** **FOLLOWUP_DONE** (Ghidra mutated; docs updated)

## Follow-up items addressed

| Prior follow-up | Result |
|-----------------|--------|
| Replace 1-byte `CAnim` / `CBitmap` placeholders | Done — deleted placeholders + `*_recovered`; recreated canonical **`CAnim`** (240 B) and **`CBitmap`** (152 B) |
| Field-resolve `header_unk_28` (`+0x28..+0x67`) | **Done (agent todo 5)** — Ghidra names on `CAnim`/`CGameView`/`CBitmap`: `bounds`, `view_flags`, `pChain_pad_48`, `child_chain`, `pHeader_tail_58`; `CAnim` adds `pLinked_bulanek` @ `+0x64`; `GetSpatialBucketKey@0x00416f50` proves `+0x2c`/`+0x6c` reads |
| Track-manager tail (`+0xac..+0xef` on `CAnim`) | **Done (agent todo 5 r2)** — `CDSVideoPlayer track_manager` @ `+0xa8`; nested `CDSTrackVector trackVector` @ host `+0xc4` (`track_manager+0x1c`); `ConstructTrackManager` / TM helper prototypes |
| Fold `CBitmap_recovered` into `CGameView` docs | Done in [`CBitmap.md`](CBitmap.md) — `CreateImage` / `CGameView_ctor` alias note |

## Ghidra MCP actions

1. `delete_data_type` — `CAnim`, `CBitmap`, `CAnim_recovered`, `CBitmap_recovered`
2. `create_struct` — `CAnim` (37 fields, 240 B), `CBitmap` (30 fields, 152 B; `ODSImage` @ `+0x88`)
3. `set_function_prototype` — `CAnim_ctor@0x00419940`, `CGameView_ctor@0x004191a0` (as `CBitmap *`), `CAnim_RenderAnimFrame@0x004164f0`
4. `get_struct_layout` — verified sizes 240 / 152
5. `save_program bulanci.exe` — once at end

## Ghidra deltas

- **`CAnim`**: 1 B placeholder → 240 B struct with named vtables, origins, CDSView header fields, anim inner / track-manager tail blob
- **`CBitmap`**: 1 B placeholder → 152 B struct; tail typed as **`ODSImage`**
- Decompiler on `CAnim_ctor` now shows `this->nOrigin_x`, `this->vftable_IDSAnim`, etc. (ECX `this` still `void*` per MCP limitation)

## Evidence added (layout)

| Offset | Field | Evidence |
|--------|-------|----------|
| `+0x30..+0x3f` | `bounds` (4×`int`) | `CDSView_SetRect@0x0042c480` |
| `+0x44` | `view_flags` | `CDSView_SetRect@0x0042c480`; `TM_SetTrack@0x004391e0` via `inner+8` → outer `+0x44` |
| `+0x54` | `child_chain` | `CDSView_SetRect@0x0042c480` |
| `+0x88` | `ODSImage` (`CBitmap` only) | `CGameView_ctor@0x004191a0`, `ODSImage.md` |

## Remaining UNK

- `CAnim` / `CBitmap`: pad semantics for `pPad_28`, `dwField_40`, `pChain_pad_48`, `pHeader_tail_58` (named; no ctor write)
- `CAnim`: `+0x90..+0x97`; `track_manager` inner fields `+0xac..+0xef`
- `CBitmap`: `+0x14` uint16 widget flags (CBulAnim path only, not re-verified on `CBitmap`)
- Dedicated `CGameView.md` not created (alias documented in `CBitmap.md`)

## Deliverables updated

- [`CAnim.md`](CAnim.md)
- [`CBitmap.md`](CBitmap.md)
