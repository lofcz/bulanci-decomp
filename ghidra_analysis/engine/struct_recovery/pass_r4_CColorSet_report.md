# Pass R4 — CColorSet report

## Task

| Field | Value |
|-------|-------|
| **title** | Map `CColorSet` (size, fields, palette LUT builders, `CGame_BuildTeamPaletteLut` xrefs) |
| **structs** | `CColorSet` |
| **deliverables** | [CColorSet.md](./CColorSet.md), Ghidra `save_program` |

## Status

**DONE** — size `0x70`, layout refreshed from Ghidra; palette path documented; `CGame_BuildTeamPaletteLut` callers listed; lobby co-use with `CStartGame2_ctor` proven.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x70` | `0x0040e9f0` | `CColorSet_Allocate`: `OperatorNewWithBadAlloc(0x70)` |
| Lobby heap size | `0x00410657` | `CStartGame2_ctor`: `OperatorNewWithBadAlloc(0x70)` before `CColorSet_ctor_slotPillar` |
| Four vtables | `0x0040ffd0` | `0x480914`, `0x4808f8`, `0x4808e0`, `0x4808cc` |
| Pillar bitmap res | `0x0041005e` region | ctor loads `0x10013`; stores @ `+0x6c` |
| Tint at render | `0x0040aed0` | `WidgetStateFlags_ToTintColor(bViewStateFlags & 2)`; `(char*)(pPillarBitmap+0x18) = tint` |
| View state @ +0x44 | `0x0040aed0` | Same `ESI+0x44` byte pattern as `CScrollBar_Render@0x00403537` |
| Color grid hit test | `0x0040f580` | 0x14×0xf cell geometry → `SetColorIndex` |
| Event `0xd1` | `0x0040cc60` | `FUN_0042ecc0(parent+0x10, 0x400, 0xd1, CONCAT11(bColorIndex,bTeamTint), 0)` |
| Remote color sync `0xde` | `0x0040d6f2` | `CGameGetPlayerColorByte` → `SetColorIndex` on `this+0x98[slot]` |
| **No** LUT call from `CColorSet` | — | Zero xrefs from `0x0040aed0`..`0x0040f580` to `0x00413c50` |
| `CGame_BuildTeamPaletteLut` | `0x00413c50` | `CopyDefaultPaletteSeed` + `BuildPaletteLut` |
| Lobby LUT consumer | `0x00410808` | `CStartGame2_ctor` slot loop: pillar ctor then `BuildTeamPaletteLut` for `CBulAnim` |
| `BuildPaletteLut` core | `0x00413220` | Memcpy `DAT_004aeb20` (256×4); remap from team color dword |
| `CopyDefaultPaletteSeed` | `0x00413200` | `*dst = DAT_004aef24[index*4]` |

### `CGame_BuildTeamPaletteLut` — all xrefs to `0x00413c50`

| From | Function |
|------|----------|
| `0x00410808` | `CStartGame2_ctor` |
| `0x00411393` | `CScore_ctor` |
| `0x0040aa7d` | `CBulPicture_ApplyTeamPalette` |
| `0x0040aff8` | `CBulAnim_ApplyTeamPalette` |
| `0x0041c760` | `CBulanek_ResolveAndBindAnimTrack` |
| `0x0041c8fb` | `CBulanek_BindDeathTombstoneAnim` |
| `0x0041e9db` | `CBulanekCtor` |
| `0x004173c7` | `CBulanek_OnShow` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `add_struct_field` | `CColorSet` @ `+0x44` | `bViewStateFlags` (`byte`) |
| `set_decompiler_comment` | `0x00413c50` | Team LUT wrapper + lobby relation |
| `set_decompiler_comment` | `0x0040aed0` | Tint / `bViewStateFlags` / bitmap `+0x18` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CColorSet.md](./CColorSet.md) — full layout from `get_struct_layout`, palette/LUT section, `BuildTeamPaletteLut` xref table, lobby flow.

## Remaining UNK

- Bytes `+0x40..+0x43` between bounds tail and `bViewStateFlags`
- `+0x48..+0x67` padding — no pillar-ctor writes
- Exact mapping `bTeamTint` (ctor slot index) vs `CGameGetPlayerColorByte` (network `0xde` path)
