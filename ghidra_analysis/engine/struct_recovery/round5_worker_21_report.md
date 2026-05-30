# Round 5 worker 21 — CBulanek tail UNK fields

## Task

| Field | Value |
|-------|-------|
| **worker** | 21 / 50 |
| **mode** | WRITE |
| **struct** | `CBulanek` |
| **scope** | UNK tail fields only — ctor write + consumer read proof; `set_function_this_type` on `__thiscall` helpers |

## Status

**DONE** — nine former `dwField_*` / `nField_*` / `bField_*` tail slots renamed in Ghidra and documented with instruction-level writer/reader tables. `CBulanek_Draw` and facing-axis helpers retyped to `__thiscall` with `CBulanek *` / `CGaming *` `this`.

## Evidence (func@addr)

| Offset | Ghidra name | Ctor write | Non-ctor writers | Consumers (read) |
|--------|-------------|------------|------------------|------------------|
| `+0xA4` | `nFacingAxisExtent` | `CBulanekCtor@0x0041e776` `= 0` | `CBulanek_AdaptDisplaySize@0x00417615`/`0x0041762a` | `CBulanek_OffsetCollisionRectByFacing@0x00417465`; `CBulanek_SnapPositionToFacingAxis@0x0041792a` |
| `+0x128` | `nScriptKillScore` | `CBulanekCtor@0x0041e7e2` `= 0` | `CBulanek_OnEvent@0x00420deb` `++` (event `0xD8`) | `CGaming_OnScriptKillScore@0x00418193` max scan; score block copy @ `0x00418184` |
| `+0x13C` | `nDeathCount` | `CBulanekCtor@0x0041e7e8` `= 0` | `CBulanek_OnDeath@0x0041f9b8` `++` | `CGaming_TickRoundStateAndScoring@0x0041f386`/`0x0041f413` (mode 2 min-deaths leader) |
| `+0x140` | `nDrawAlphaPercent` | `CBulanekCtor@0x0041e70f` `= 100` | `CGaming_RespawnPlayer@0x0041f8bd` `= 0`; `CBulanek_OnDeath@0x0041f94c` `= 100` | `CBulanek_Draw@0x00417694` → `(nDrawAlphaPercent * 0xff / 100) * 0x10101` tint |
| `+0x144` | `bPendingKillerSlot` | `CBulanekCtor@0x0041e76f` `= 0xff` | `CBulanek_OnDeath@0x0041f99f` `= killerSlot`; restore `0xff` @ `0x0041f9ea` | `CBulanek_OnEvent@0x00420ea2` / delayed-quip path @ `0x00421079` |
| `+0x188` | `dwPickupFacingLatch` | `CBulanekCtor@0x0041e6d3` `= 0` | `CBulanek_OnEvent@0x00421079` (event `0xF2`) | `CBulanek_OnEvent@0x00421060`/`0x00421133` cmp; `0xF3` clears @ `0x0042114d` |
| `+0x190` | `nLivesSnapshot` | `CBulanekCtor@0x0041e788` `= param_7` | — | `CBulanek_OnDeath@0x0041f926` → `nLivesRemaining` |
| `+0x198` | `bDrawAsMuted` | `CBulanekCtor@0x0041e660` `= 0` | `CBulanek_OnEvent@0x00420f8e`/`0x00421027` (`0xEE`) | `CBulanek_Draw@0x0041764a`; `CGaming_RetestTraceAreasForEntity@0x0041b5d2` |
| `+0x19A` | `bScriptDeathGuard` | `CBulanekCtor@0x0041e666` `= 0` | `CBulanek_OnEvent@0x00420f70` (`0xEE` sub 1) | `CBulanek_OnEvent@0x00420d80` blocks `0xD7` while set; `0xEF` sub 1 clears @ `0x00421009` |

### Semantic notes

- **`nScriptKillScore`** — script kill counter (event `0xD8`); mode **0** uses **maximum** across slots for leading-player banner (`CGaming_OnScriptKillScore`).
- **`nDeathCount`** — per-player death tally; mode **2** (last-man) uses **minimum** `nDeathCount` for survivor HUD (`CGaming_TickRoundStateAndScoring`).
- **`nDrawAlphaPercent`** — draw tint strength (100 = normal); coop respawn slots `0x20..0x23` force `0` for fade-in.
- **`bPendingKillerSlot`** — holds killer slot between death and delayed hit-quip completion (`0xFF` = idle).
- **`dwPickupFacingLatch`** — non-zero while pickup mask event `0xF2` is being applied (paired with scheduler slot 1).

## Ghidra deltas

- `modify_struct_field` — nine renames on `CBulanek` (table above).
- `set_function_prototype` + `set_function_this_type` — `CBulanek_Draw@0x00417640`, `CBulanek_SnapPositionToFacingAxis@0x00417910`, `CGaming_OnScriptKillScore@0x004180e0` (`CGaming *` + `CBulanek * scorer`).
- `set_decompiler_comment@0x0041e776` — `nFacingAxisExtent` init.
- `save_program bulanci.exe`.

## Struct doc updates

- [CBulanek.md](./CBulanek.md) — layout rows + Ghidra apply note; UNK list trimmed (prefix gaps / track subgraph unchanged).

## Remaining UNK (this worker)

- CGameView prefix gaps `+0x08..+0x73` (see `CGameView.md`).
- `videoTrackManager` interior `+0x9C..+0xEF` (CDSObject track batch).
- `pPad_postWalkEmbed@+0x15C..+0x167` — ctor-only zeros.
- `view_flags@+0x44` vs embedded TM `frameDelayOverrideMs` relationship.
