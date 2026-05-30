# Net opcode gameplay impact (master table)

Cross-linked to Ghidra handlers in `bulanci.exe` and wire layouts in [`net_protocol.md`](net_protocol.md).
Dispatcher: **`CGame_ProcessNetMessage`** @ `0x00415290` (plate `NET_DISPATCH` + `AUTHORITY` summary).

**Gates (vanilla):**

| Gate | Meaning |
|------|---------|
| `CGame+0x30 == 6` | In-match; required for `0x0A`–`0x15`, `0x18`–`0x19` recv |
| `CGame+0x36 == 0` | Host/admin |
| `CBulanek_IsHumanPlayer` @ `0x00416720` | Local human may send slot-owned match msgs |
| `CGame+0x209` | Demo recording; journal writes **`0x0C`–`0x19`** only |
| `0x16` / `0x17` recv | **Not** gated on `+0x30`; re-enter send with `isReplay=1` |

---

## Lobby / setup (`0x00`–`0x09`, `0x64`)

| Op | Send @ | Recv @ | GAMEPLAY_IMPACT | Must-match bytes |
|----|--------|--------|-----------------|------------------|
| **0x00** | `0x00414dd0` inline | `0x00415390` case | Host roster ingest; join accepted/rejected; host replies **0x01**. State: host slots from `N×PlayerRec[36]`. | `[0x00, dpInit, field_0x37, u32 count@+3, pad×35, count×36 name bytes]` |
| **0x01** | case 0 reply @ `0x00415290` | `0x00415741` case | Client lobby populate; `+0x30`: 1→2; `hostSlot[4]`, `dpid[4]`, `randSeed@+0x1B`. | Fixed header `0x23` + `M×36` |
| **0x02** | `0x004139b0` | `0x00415980` case | Rename in UI (event **0xDD** send / **0xE1** path). `+0xDE+slot*0x23`. | `[0x02, slot, UTF-16LE name+NUL]` |
| **0x03** | `0x00412c70` | `0x00415960` case | Avatar portrait. `+0xDC+slot*0x23`. | 3 bytes |
| **0x04** | `0x00412cd0` | `0x00415940` case | Gamemode / score limit. `+0x19C`, `+0x19E+idx*4`. | 6 bytes LE u32 @+2 |
| **0x05** | `0x00414340` | `0x00415A20` case | Level title `+0xCC`. | **38** bytes fixed |
| **0x06** | `0x00413b90` | `0x00415A50` case | Chat line **0xE1**. | `[0x06, slot, UTF-16LE text+NUL]` |
| **0x07** | `0x00412da0` | `0x00415AA0` case | Loading countdown **0xE2**; digit 4 → **0x8002**. `+0x1E8`. | 2 bytes |
| **0x08** | `0x00413ce0` inline | `0x00415B80` case | Level-loaded per slot (`+0x16F+slot*0xD`). StartGame: `(localSlot, 2)`. | 3 bytes |
| **0x09** | `0x00413ce0` inline | `0x00415BC0` case | Slot-change banner **0xE3**. StartGame: `(1, 0)`. | 6 bytes LE u32 @+2 |
| **0x64** | `0x00412d40` | `0x00415DE0` case | Admin / weapon-spawner `+0x19D` → `CGaming+0x368`. | 2 bytes |

---

## Match (`0x0A`–`0x19`)

| Op | Send @ | Recv @ | GAMEPLAY_IMPACT | Must-match bytes |
|----|--------|--------|-----------------|------------------|
| **0x0A** | `0x004124a0` (`NetSendKick` / pause) | `0x00415C20` case | Pause/status per slot **0xE9**; `+0x174+slot*0xD`. Not lobby kick. | 3 bytes |
| **0x0B** | `0x00413180` | `0x00415C80` case | Round-end **0x80CC** / **0x80CD**. | `[0x0B, isWin]` |
| **0x0C** | `0x00412a40` | `0x0041f210` | **Weapon/ammo sync** (not HP). Widget + ammo bar. | `[0x0C, slot, weaponKind]` — **3 B** |
| **0x0D** | `0x00412b10` | `0x004209f0` | Remote pos + anim. | 7 bytes; s16 x/y |
| **0x0E** | `0x00412b60` | `0x00420a70` | Primary action from `player+0x64`. | 2 bytes (slot) |
| **0x0F** | `0x00412b90` | `0x00417e80` | Shot spawn deterministic. | 8 bytes |
| **0x10** | `0x00412bf0` | `0x00420510` | Hit → respawn at impact + facing. | `[0x10, hit_slot, face, s16 x, s16 y]` — **7 B** |
| **0x11** | `0x00412c40` | `0x004180c0` | Death / reload timer. | 2 bytes |
| **0x12** | `0x00412a00` | `0x00415D00` case | Round timer HUD; `+0x20E`. Host send. | `[0x12, s16 secs]` |
| **0x13** | `0x00412950` | `0x0041d2c0` | Special pickup @ world slot **100**. Host spawn. | 5 bytes (x,y) |
| **0x14** | `0x00412990` | `0x0041f030` | Pick kind-1 special; destroy slot 100. | `[0x14, picker_slot]` |
| **0x15** | `0x00412df0` | `0x004185a0` | Script **OnNetCustom** #9. | `[0x15, u16 L, payload[L]]` |
| **0x16** | `0x00414550` | `0x00415151` case | Team score **0xEE**; recv **ungated**, `isReplay=1`. | 3 bytes |
| **0x17** | `0x004138b0` | `0x00415156` case | Opposing **0xEF**; recv **ungated**, `isReplay=1`. | 3 bytes |
| **0x18** | `0x00412ac0` | `0x0041d280` | World pickup spawn slots **101–107**. Host. | `[0x18, kind, world_slot, s16 x, s16 y]` |
| **0x19** | `0x00412a80` | `0x0041f010` | Pick world obj (kind≠1); destroy icon. Wire: **world_slot, kind, player_slot**. | `[0x19, world_slot@+1, kind@+2, player_slot@+3]` — **4 B** |

---

## EOL corrections (2026-05-30 pass)

Prior draft used wrong field names for:

| Op | Wrong | Correct |
|----|-------|---------|
| **0x0C** | attacker / victim | **slot**, **weaponKind** |
| **0x10** | shooter / victim | **hit_slot**, **face**, x, y |
| **0x19** | player, kind, world_slot order | **world_slot@+1**, **kind@+2**, **player_slot@+3** |

---

## Renames (this pass)

| Address | Name |
|---------|------|
| `0x00414640` | `CMenu_NetSendLobbySyncAll` (was `CMenu_NetSendKickAll`) |
| `0x004124a0` | Kept `CGame_NetSendKick_t0a`; plate documents pause alias |

---

## Subagent sources

| ID | Contribution |
|----|----------------|
| `76e02c13` | Synthesis + GAMEPLAY_IMPACT table |
| `f07ca6f5` | Recv matrix; 0x0C/0x10 fixes |
| `8b10fb85` | Opcodes 0x00–0x0F |
| `8f1cb484` | Opcodes 0x10–0x19 |
| `2828e8cc` | Lobby recv PRE addresses |
| `6ba1c833` | Moderation UX; rename checklist |
| `36f15d08` | Authority matrix → NET_DISPATCH plate |
| `33badbe5` | Prior MCP changelog |
| [`mcp_apply_log.md`](mcp_apply_log.md) | Tool apply history |
