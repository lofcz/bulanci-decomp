# Asset naming — Agent 9/30 (indices 90–99)

**Sort key:** `catalog.json` → `entries` sorted by `id` ascending (347 entries).

**Verification:** overlay script xref + PNG/manifest dimensions. No `bulanci.exe` immediate operands for these IDs (overlay-pack only).

| Idx | ID | Hex | Proposed name | Category | Key xrefs | Ghidra labels | Open questions |
|-----|-----|-----|---------------|----------|-----------|---------------|----------------|
| 90 | 65626 | 0x1005a | `level_steel_works_smokestack` | levels/steel_works | script 65857 `@0x0149` InsertView (736,323) | PRE_COMMENT on `CLevelScriptOpExt_CreateImage` | Exact industrial part (pipe vs. stack) |
| 91 | 65627 | 0x1005b | `level_malaria_bush` | levels/malaria | script 65858 `@0x00e0` InsertView (432,61) | same | — |
| 92 | 65628 | 0x1005c | `level_malaria_bg` | levels/malaria | script 65858 `@0x0232` InsertView (0,0) | same | — |
| 93 | 65629 | 0x1005d | `level_8bit_wire_pyramid` | levels/8bit | script 65859 `@0x0195` SetOrderAxis z=-67 | same | — |
| 94 | 65630 | 0x1005e | `level_8bit_wire_glyph_sm` | levels/8bit | script 65859 `@0x01ab` z=-30 | same | Semantic role of small wire glyph |
| 95 | 65631 | 0x1005f | `level_8bit_wire_prism` | levels/8bit | script 65859 `@0x0169` z=-25 | same | — |
| 96 | 65632 | 0x10060 | `level_8bit_spawn_pool_3` | levels/8bit | script 65859 CollInsert + SpawnOpponentEx | PRE_COMMENT on `CollInsert` / `SpawnOpponentEx` | Maps to which opponent archetype |
| 97 | 65633 | 0x10061 | `level_8bit_spawn_pool_0` | levels/8bit | CollInsert [0] | same | same |
| 98 | 65634 | 0x10062 | `level_8bit_spawn_pool_1` | levels/8bit | CollInsert [1] | same | same |
| 99 | 65635 | 0x10063 | `level_8bit_spawn_pool_2` | levels/8bit | CollInsert [2] | same | same |

**CollInsert order (global 12):** 65633 → 65634 → 65635 → 65632 (indices 0–3).

**Ghidra:** `save_program` after catalog patch; resource IDs are not embedded in `.text` (script VM only).
