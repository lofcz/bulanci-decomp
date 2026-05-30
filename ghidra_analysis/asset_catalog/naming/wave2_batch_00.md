# Asset naming — Wave 2 batch 0/15

**Scope:** Poem indices 5–14 (`unnamed_batches.json` batch 0) — CPoem resources `0x00010005`..`0x0001000e`.

**Verification:** Overlay `.txt` title/opening lines; Ghidra decompile `CPoemScroller::Constructor` @ `004262c0` (resource-pool class filter `0x7fb` / 2043 @ `00426453`); `PickNextPoem` @ `00425df0`. No immediate `PUSH` of these IDs in `bulanci.exe` (runtime pool enumeration only). Secondary `PUSH 0x7fb` @ `0047b35a` is outside any function (data/ref only).

| Idx | ID | Hex | Proposed name | Category | Key xrefs | Ghidra labels |
|-----|-----|-----|---------------|----------|-----------|---------------|
| 5 | 65541 | 0x00010005 | `poem_zeme_duni` | ui/credits | `CPoemScroller::Constructor` pool scan @ `00426453`; xrefs from `CreateObject` @ `004265b0`, `CMenu_ctor_with_ui` @ `00426b78` | `res_pool_scan_CPoem_class_0x7fb` @ `00426453`; plate comment on ctor |
| 6 | 65542 | 0x00010006 | `poem_bojiste` | ui/credits | same class-enumeration | same label (shared scan) |
| 7 | 65543 | 0x00010007 | `poem_vybijena` | ui/credits | same | same |
| 8 | 65544 | 0x00010008 | `poem_setkani` | ui/credits | same | same |
| 9 | 65545 | 0x00010009 | `poem_pohrebni_pisen` | ui/credits | same | same |
| 10 | 65546 | 0x0001000a | `poem_dobry_lov` | ui/credits | same | same |
| 11 | 65547 | 0x0001000b | `poem_zbrane` | ui/credits | same | same |
| 12 | 65548 | 0x0001000c | `poem_od_rana` | ui/credits | same | same |
| 13 | 65549 | 0x0001000d | `poem_rozbres_sili` | ui/credits | same | same |
| 14 | 65550 | 0x0001000e | `poem_s_chladnou_vuli` | ui/credits | same | same |

**Poem titles (from overlay `.txt`):**

| Hex | Czech title / opener |
|-----|----------------------|
| 0x10005 | Země duní, (opening line — no `[CENTER]` title) |
| 0x10006 | Bojiště |
| 0x10007 | Vybíjená |
| 0x10008 | Setkání |
| 0x10009 | Bulánčí pohřební píseň |
| 0x1000a | Dobrý lov |
| 0x1000b | Zbraně |
| 0x1000c | Od rána máš divný pocit, (no title line) |
| 0x1000d | Rozbřesk sílí, |
| 0x1000e | S chladnou vůlí, |

**Registry:** added `0x00010005`..`0x0001000e` under folder `ui/credits/poems` (slugs synced to catalog). **Catalog:** `name`, `category`, `notes`, `xrefs.ghidraText` updated for ids 65541–65550. **Ghidra:** PRE_COMMENT @ `00426453` lists all ten IDs; `save_program` on `bulanci.exe`.
