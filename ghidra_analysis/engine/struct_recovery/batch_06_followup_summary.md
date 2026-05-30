# Struct recovery batch 06/50 — follow-up round 2

**Prior:** `batch_06_summary.md`  
**Status:** **FOLLOWUP_DONE** (3/3 follow-up items addressed; `CDeath2` remains PARTIAL)

## Actions taken

### 1. `CAnim` / `ODSImage` → `CDeath2` layout alignment

- Confirmed `CDeath2_SubobjectCtor@0x0041a8c0` takes `CAnim*` and calls `CAnim_SubobjectCtor` first.
- Renamed `CDeath2` Ghidra fields to match recovered `CAnim` vtable/origin names: `vftable_IDSEventHandler`, `vftable_IDSReferenced`, `vftable_IDSUpdated`, `vftable_IDSAnim`, `vftable_anim_sub`, `gaming_host` (`pGaming_host` in layout).
- Documented **overlay** at `+0x68/+0x6c`: `CDeath2` uses `dwM_netStateWord0/1` (uint) where base `CAnim` has byte `view_state_*` — decompiler now shows `CDeath2_UpdateStateFromParams` with typed fields.
- `ODSImage` (`0x10` mixin) not embedded at a separate offset on `CDeath2`; image drawables live inside the shared `CAnim`/view header (`ODSImage.md` applies to `+0x88` cluster as vtables, not a nested `ODSImage` struct field).

### 2. `CDeath` (`0x108`) vs `CDeath2` (`0xfc`)

- Re-decompiled `CBulanek_OnDeath@0x0041fa21`: corpse `OperatorNew(0x108)` + `CDeath_ctor`; tournament tombstone `OperatorNew(0xfc)` + `CDeath2_ctor`.
- Added `ghidra_analysis/engine/struct_recovery/CDeath.md` with tail diff table (`+0xf4` mode flags, `+0xfc` tourney byte, `+0x100/+0x104` extra dwords).

### 3. `CDirectKeyb_GetKeyEdge` typing

- `set_function_prototype` + rename at `0x004121a0` / `0x00412160`.
- **`CDirectKeyb_PollKeyboard`**: decompiler now uses `CDirectKeyb*` (`pM_keyState`, `pM_prevKeyState`, `m_pKeyboard`).
- **`CDirectKeyb_GetKeyEdge`**: assembly confirms `this+4` / `this+0x104` indexing; Ghidra MCP **cannot** retype `__thiscall` ECX `this` (still shows `CGame*` in decompile). Plate comment documents intended `CDirectKeyb*`.

## Ghidra deltas

| Target | Change |
|--------|--------|
| `CDeath2` struct | Field renames (CAnim-aligned vtables, `dwM_netStateWord0/1`, placement tail names) |
| `0x00412160` | Prototype `CDirectKeyb_PollKeyboard(CDirectKeyb*)` — decompile fixed |
| `0x004121a0` | Prototype + plate comment (`CDirectKeyb*` intent) |
| `bulanci.exe` | `save_program` |

## Struct status after follow-up

| Struct | Was | Now | Notes |
|--------|-----|-----|-------|
| `CDeath2` | PARTIAL | **PARTIAL** | CAnim header names aligned; pads `pPad_08/30/70/90` remain |
| `CDirectKeyb` | VERIFIED | **VERIFIED** | Poll decompile clean; GetKeyEdge ECX limitation documented |
| `CDeath` | — | **PARTIAL** | Ghidra `CDeath` 264 B; ctor/subobject/update prototypes; `pHost` @ `+0xf0` (agent todo #6) |

## Remaining UNK

- `CDeath2` interior pads and `pM_pPalette` / `pM_pParent` semantics.
- ~~`CDeath` `field_100` / `field_104` consumers~~ → **done** (agent todo #6 r2): `nM_placementOffsetX/Y` @ `+0x100/+0x104`, `CBulanek_ResolveAndBindAnimTrack`.
- `CDirectKeyb_GetKeyEdge` decompiler `this` type (manual retype in Ghidra UI if desired).
- Full `CAnim` `+0x28..+0x67` (see `CAnim.md`).
