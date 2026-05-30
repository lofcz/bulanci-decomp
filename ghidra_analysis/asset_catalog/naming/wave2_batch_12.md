# Naming wave 2 — batch 12/15

**Scope:** overlay indices 309, 310–312, 319–324 (`unnamed_batches.json`).

**Verification:** Ghidra MCP `get_bulk_xrefs` (overlay IDs → no `.text` immediates), `search_instructions` for concrete hits; overlay script disassembly for LoadPreface / InsertView / GetInfo strings.

| Idx | ID | Hex | Proposed name | Class | Category / registry | Key xrefs | Ghidra (exe) | Open questions |
|-----|-----|-----|---------------|-------|---------------------|-----------|--------------|----------------|
| 309 | 65845 | 0x10135 | `history_part_3` | HistoryScript | `scripts.65845` | `CHistoryDlg_ctor` class enum 0x802 @`0042329e`; HCreateMovie 65613/65590 in script | synthetic CMP only | Page 3 prose spans 1993–1998; clips pair 77828–77831 |
| 310 | 65846 | 0x10136 | `level_vybijena_jpeg_bg` | BitmapJPEG | `assets.0x00010136` → `levels/vybijena` | script 65855 `@0x00b4` InsertView full-screen | none (overlay VM) | 800×600 JPEG (~322 KiB); arena backdrop |
| 311 | 65847 | 0x10137 | `level_vybijena_preface` | BitmapJPEG | `assets.0x00010137` | script 65855 `@0x00a0` LoadPreface | none | Preface scroll before duel layout |
| 312 | 65848 | 0x10138 | `level_bedtime_story_preface` | BitmapJPEG | `assets.0x00010138` → `levels/bedtime_story` | script 65856 `@0x00d5` LoadPreface | none | — |
| 319 | 65855 | 0x1013f | `level_vybijena` | Script | `scripts.65855` | GetInfo `"Vybíjená"` / `"Dj Slaughter"`; CLevelScript enum 0x7ea @`00409fb7` | none | Also branded Dj Slaughter in EN GetInfo branch |
| 320 | 65856 | 0x10140 | `level_bedtime_story` | Script | `scripts.65856` | GetInfo `"Na dobrou noc"`; audio bank 65537 | class enum @`00409fb7` | — |
| 321 | 65857 | 0x10141 | `level_steel_works` | Script | `scripts.65857` | GetInfo `"Noční směna"`; bank 65538 | class enum @`00409fb7` | — |
| 322 | 65858 | 0x10142 | `level_malaria` | Script | `scripts.65858` | GetInfo `"Malárie"` | class enum @`00409fb7` | — |
| 323 | 65859 | 0x10143 | `level_8bit_captivity` | Script | `scripts.65859` | GetInfo `"V zajetí 8-bitu"`; bank 65536 | class enum @`00409fb7` | — |
| 324 | 65860 | 0x10144 | `level_exitus` | Script | `scripts.65860` | GetInfo `"Exitus"`; **CMP 0x10144** @`0041c24b` in `CGaming_OnResumeOrStartGame` | 1 concrete insn | 7 s engine timers on resume (script_lifecycle.md) |

## Ghidra MCP summary

| ID hex | `get_bulk_xrefs` | `search_instructions` (operand) |
|--------|------------------|-----------------------------------|
| 0x10135–0x10138 | `[]` | — (overlay-only) |
| 0x1013f–0x10143 | `[]` | `0x7ea` → `FUN_00409f60` @`00409fb7` (shared class-enumeration for all six level scripts) |
| 0x10144 | `[]` | `0x10144` → `CGaming_OnResumeOrStartGame` @`0041c24b` |

HistoryScript page resources (0x10133–0x10135) share the same synthetic xref: `CHistoryDlg_ctor` CMP `[obj+0xc], 0x802` @`0042329e`.

## Catalog / registry

- `catalog.json`: `name`, `category`, `notes` on all 10 IDs (20 rows: `entries` + `byClass`).
- `registry.json`: renamed `scripts.65855` / `65860` slugs from `engine_script_*`; added `assets` for `0x00010136`–`0x00010138`; expanded `scripts.65845` notes.

**Not in scope:** `catalog.md` regen, Ghidra `save_program`, commit.
