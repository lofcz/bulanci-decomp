# Round 5 — worker 39 report (`CColorSet` 0x70 pillar UNKs)

## 1. Task

| Field | Value |
|-------|--------|
| **worker** | 39 / 50 |
| **title** | Close `CColorSet` `0x70` pillar layout UNKs (R4 `pass_r4_CColorSet_report` follow-up) |
| **types** | `CColorSet`, `CWindow` |
| **mode** | WRITE — Ghidra MCP evidence-only |
| **prior** | [pass_r4_CColorSet_report.md](./pass_r4_CColorSet_report.md), [CColorSet.md](./CColorSet.md) (pre-R5) |

**R4 UNK seeds:** `+0x40..+0x43` gap; `+0x48..+0x67` “padding”; `+0x1c`/`+0x22`; `bTeamTint` vs `CGameGetPlayerColorByte`.

## 2. Status

**DONE** — `CColorSet` Ghidra layout aligned to **`CWindow` (112 B)**; chain band `+0x40..+0x67` named; tail overlay `+0x68..+0x6c` documented; `bTeamTint` = lobby slot index (distinct from palette byte).

## 3. Evidence

### Size / shell equivalence

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x70` | `0x0040e9f0`, `0x00410657` | `OperatorNewWithBadAlloc(0x70)` |
| Same field offsets as `CWindow` | `get_struct_layout` | `CColorSet` and `CWindow` both 112 B; offsets `+0x20..+0x64` match after R5 renames |
| Render disasm | `0x0040aed0` | `MOVZX` @ `[ESI+0x44]`; blit rect `[ESI+0x30..0x3c]`; `CMP` @ `[ESI+0x68]`; bitmap `[ESI+0x6c]` |

### Former UNK bands (closed)

| Offset | Resolution | Evidence |
|--------|------------|----------|
| `+0x40..+0x43` | `dwChainHead_40` | Missing Ghidra field inserted @ 64; matches `CWindow` / `CDSChained_ResetChainCounters@0x0042beb0` |
| `+0x48..+0x67` | `wChainCounter_48`, `wChainCounter_4a`, `dwChainField_50`, chain vtables, `dwField_5c/60`, `pOverlapEntity` | Same names/offsets as [CWindow.md](./CWindow.md); not written in `CColorSet_ctor_slotPillar` |
| `+0x1c` | `dwField_1c` | `CDSChained_ctor` zero (shared) |
| `+0x16` | `wPad_16` | Padding after `wViewFlags` (shared) |
| `+0x68..+0x6c` | Tail overlay | ctor: `bColorIndex=0xff`, `bTeamTint=teamTint`, `pPillarBitmap` load `0x10013`; overlays `CWindow` `bModalFlag` / `pDefaultFocusChild` |

### Misnamed R4 fields (corrected)

| Old name | Correct name | Offset |
|----------|--------------|--------|
| `nOrigin_x/y` | `nBbox_left/top` | `+0x20` / `+0x24` |
| `nInner_w/h` | `nBbox_right/bottom` | `+0x28` / `+0x2c` |
| `nBounds_*` | `nScreenBbox_*` | `+0x30..+0x3c` |
| `bViewStateFlags` (byte) | `wViewStateFlags` (ushort; low byte used) | `+0x44` |

### `bTeamTint` semantics

| Path | Address | Finding |
|------|---------|---------|
| Lobby ctor loop | `0x00410657` | `teamTint` counter `0,1,2…` passed to `CColorSet_ctor_slotPillar(..., teamTint)` → `bTeamTint` |
| Event broadcast | `0x0040cc60` | `FUN_0042ecc0(..., 0xd1, CONCAT11(bColorIndex, bTeamTint), 0)` |
| Network color sync | `0x0040d6f2` | `CGameGetPlayerColorByte(pParentState, slot)` → `SetColorIndex` — **palette index only** |
| Anim listener | `0x0040aff8` region | `CBulAnim_OnTeamPaletteEvt`: low byte `bTeamId`, high byte color from `0xd1` word |

`CGameGetPlayerColorByte@0x0041320c` returns `*(uchar *)(player*0x23+0xdc+base)` — game-state color byte, **not** the ctor slot index in `bTeamTint`.

## 4. Ghidra deltas

- `add_struct_field` — `dwChainHead_40` @ `+0x40`; chain fields @ `+0x58..+0x64`; `wChainCounter_4a` @ `+0x4a`
- `modify_struct_field` — bbox / screen / chain / parent / widget renames (see §3 table)
- `set_function_this_type` — `CColorSet *` @ `CColorSet_ctor_slotPillar@0x0040ffd0`, `CColorSet_Render@0x0040aed0`, `CColorSet_OnMouseDown@0x0040f580`
- `set_decompiler_comment` — ctor @ `0x0040ffd0`; `SetColorIndex` @ `0x0040cc60`
- `force_decompile` — ctor + render (typed `this->nScreenBbox_*`, `pPillarBitmap`, etc.)
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CColorSet.md](./CColorSet.md) — full `CWindow`-aligned layout; tail overlay table; `bTeamTint` vs `CGameGetPlayerColorByte`; UNK trimmed

## 6. Remaining UNK

- Shared `CWindow` chain dword semantics (`dwField_08`, `dwField_30..3c`, `dwField_5c/60`) beyond ctor zero/reset — not pillar-specific.
- Whether `nScreenBbox_*` diverge from `nBbox_*` after ctor (no consumer in pillar methods).
