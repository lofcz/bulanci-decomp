# Asset naming — Agent 3/10 (levels/steel_works)

**Scope:** All `levels/steel_works` registry assets + script **65857** resources (`0x10054`–`0x10059`, `0x1005a`, `0x100f4`–`0x100f5`, `0x10141`, `0x10147`, `0x10150`).

**Verification:** PNG dimensions, atlas frame inspection, `res_0000065857_2026_Script.script.asm` xrefs. No commit.

## Script 65857 layer order (OnInit)

| Offset | Op | Resource | Placement |
|--------|-----|----------|-----------|
| `0x00e1` | LoadPreface | 65624 | preface scroll |
| `0x00e6` | SetMusic | 65863 + bank 65538 | BGM + SFX bank |
| `0x00f5` | InsertView | 65623 | back @ (0,0) |
| `0x0105` | CreateAnim | 65780 | (701,180) delay=3 |
| `0x011d` | InsertView+axis | 65622 | parallax @ (129,99) z=-110 |
| `0x0139` | InsertView | 65621 | mid @ (0,0) |
| `0x0149` | InsertView | 65626 | hook @ (736,323) |
| `0x0159` | InsertView | 65620 | boxes @ (551,425) |
| `0x0169` | InsertView | 65625 | lamp @ (713,0) |
| `0x0179` | BindToSlot+anim | 65781 | slot 6 @ (538,-347) |

GetInfo: Czech **"Noční směna"** / English **"Steel works"**; GUID `C48AEB13-1BE6-41e1-A50E-7BBA39E43188`.

## Renames (visual ≠ slug)

| Hex | ID | Old slug | New slug | Visual |
|-----|-----|----------|----------|--------|
| `0x10054` | 65620 | `level_steel_works_decor_br` | `level_steel_works_prop_boxes` | SleepTeam cardboard crates (202×90) |
| `0x10056` | 65622 | `level_steel_works_bg_parallax` | `level_steel_works_parallax_pod` | Tilted furnace/capsule (272×270) |
| `0x10059` | 65625 | `level_steel_works_decor_tr` | `level_steel_works_hanging_lamp` | Chain-hung dome lamp (87×133) |
| `0x1005a` | 65626 | `level_steel_works_smokestack` | `level_steel_works_crane_hook` | Diagonal rod + ring, not a stack (64×192) |
| `0x100f4` | 65780 | `level_steel_works_intro_sign` | `level_steel_works_crucible_glow` | 24f molten-pool glow (63×46), not signage |
| `0x100f5` | 65781 | `level_steel_works_furnace_plume` | `level_steel_works_furnace_mechanism` | 121f loader/tripod anim + motion path, not smoke |

## Confirmed (no rename)

| Hex | ID | Slug | Visual / role |
|-----|-----|------|----------------|
| `0x10055` | 65621 | `level_steel_works_bg_mid` | Lattice truss mid-layer (334×408) |
| `0x10057` | 65623 | `level_steel_works_bg_back` | Full foundry backdrop (800×515) |
| `0x10058` | 65624 | `level_steel_works_preface` | LoadPreface scroll; execution-warning pictogram on black (800×600) |
| `0x10141` | 65857 | `level_steel_works` | Level script |
| `0x10147` | 65863 | `music_level_steel_works` | Level BGM |
| `0x10150` | 65872 | `level_steel_works_sfx_bank` | 3× industrial SFX (hydraulic / explosion / spark) |

## Files changed

- `ghidra_analysis/asset_catalog/registry.json`
- `ghidra_analysis/asset_catalog/catalog.json`
- `ghidra_analysis/asset_catalog/naming/agent_03.md` (this report)
