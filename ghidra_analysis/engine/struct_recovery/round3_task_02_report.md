# Round 3 — Task 02 Report (`CDSApp` +0x100..+0x1ff)

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **title** | Name CDSApp interior fields +0x100..+0x1ff |
| **one_liner** | Prove and apply `keyLatchByVk[256]` @ `CDSApp+0x100` (per-VK latch; bits 1/2 = edge state) plus documented VK_SHIFT/VK_CONTROL repurposing; wire Ghidra/decompiler on ctor, modal sync, and input consumers. |
| **acceptance** | `CDSApp.md` layout for `+0x100..+0x1ff`; Ghidra `byte[256]` @ offset 256 (`0x100`); `set_function_this_type` on `__thiscall` app methods; comments on proof sites; `save_program` if mutated |
| **source** | `agent_todos_50_r3.json` (supersedes stale `round3_tasks.json` id 2 = CDSAudioBank) |
| **structs** | `CDSApp`, `CBulanci` |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `keyLatchByVk` span `0x100` bytes @ `+0x100` | `0x0042b315` | `CDSApp_ctor`: `LEA EDX,[ESI+0x100]` → `_memset(..., 0, 0x100)` |
| Ends before `CDSDirectSound` @ `+0x200` | `0x0042b170` | ctor: `CDSDirectSound_ctor(this+0x200)`; `get_struct_layout` → `directSound` @ **512** (`0x200`) |
| Per-VK latch bits 1/2 replayed after modal | `0x00429f70` | `CGaming_SyncKeyLatchAfterModal`: 256-iter loop; `CDSApp_KeybQueue` on bit1/bit2 mismatch vs gaming shadow |
| Pre-match snapshot (first `0x40` dwords) | `0x0041c4a0` | `CGaming_RunPreMatchModal`: `puVar5 = (int)g_pApp + 0x100`; `memcpy` 0x40 dwords to stack buffer |
| Modal sync caller | `0x0041c4d3` | `CGaming_RunPreMatchModal` tail: `CGaming_SyncKeyLatchAfterModal(g_pApp, &local_138…)` |
| `keyLatchByVk[VK_SHIFT]` tab direction | `0x00403dc8` | `CWindow_OnKeyDownTabEscEnter`: `g_pApp+0x110` bit0 → `CWindow_FocusSibling(~bit0)` |
| `keyLatchByVk[VK_CONTROL]` input block | `0x00403e02`, `0x00403fac`, `0x00407924`, `0x00405145` | `g_pApp+0x111` bits 0–1 tested before Enter / button / edit / scroller page keys |
| Separate modal **down** bitmap @ `+0xf0` | `0x00429db0` | `CDSApp_DispatchInputEvent`: indexed `this+0xf0` per VK on key-down/up to modal focus (`app_shell.md`) |
| Embedded in `CBulanci` | `0x004026f0` | `CBulanci_ctor` → `CDSApp_ctor(&this->app)`; latch at `app+0x100` ≡ `CBulanci+0x100` |

### Band layout (`+0x100..+0x1ff`)

| Offset | Size | Type | Name | Role |
|--------|------|------|------|------|
| `+0x100` | 256 | `byte[256]` | `keyLatchByVk` | Per-VK latch; **bit0** and **bit1** = edge/hold state (`SyncKeyLatchAfterModal`) |
| `+0x110` | 1 | `byte` | `keyLatchByVk[0x10]` | VK_SHIFT — bit0 = tab-focus backward flag |
| `+0x111` | 1 | `byte` | `keyLatchByVk[0x11]` | VK_CONTROL — bits 0–1 = engine input-block during modals/dialogs |

Most indices `+0x112..+0x1ff` share the latch semantics (256-byte memset + 256-iter diff); no additional named subfields in this band.

## Ghidra deltas

- `get_struct_layout CDSApp`: **`keyLatchByVk` `byte[256]` @ 256** (`0x100`); **`keyDownBitmap` `byte[16]` @ 240** (`0xf0`); `directSound` @ 512 (`0x200`).
- `set_function_this_type`: `CDSApp_ctor@0x0042b170`, `CDSApp_DispatchInputEvent@0x00429db0`, `CDSApp_OnCreate@0x0042a210` → **`CDSApp *`** (ctor decompile now uses struct members vs `field_0xNN`).
- `set_decompiler_comment` @ `0x00403dc8`, `0x00403e02`, `0x0041c4a0`, `0x00429f70` (latch semantics).
- `modify_struct_field` rename attempts (`pKeyLatchByVk`→`keyLatchByVk`, `pKeyDownBitmap`→`keyDownBitmap`): tool reports success; **`get_struct_layout` export still lists `p*` prefixes** (Ghidra array decay naming — cosmetic; offset/type correct).
- **`save_program bulanci.exe`**.

## Struct doc updates

- [`CDSApp.md`](./CDSApp.md) — band already documented (agent todo 2 r3); verified against MCP decompile/disasm this pass.
- [`CBulanci.md`](./CBulanci.md) — no change (embed offset unchanged).

## Remaining UNK

- Writers that **set** `keyLatchByVk[VK_CONTROL]` bits on `g_pApp` (consumers proven; explicit stores may be via latch sync / bit ops not matching `g_pApp+0x111` disasm pattern).
- Per-VK meaning for indices other than `0x10`/`0x11` beyond bit1/bit2 edge replay (treat as generic latch bytes).
- Ghidra field **display names** `pKeyLatchByVk` / `pKeyDownBitmap` until struct editor refresh or manual rename.
- `CDSApp_OnCreate` still typed with `CBulanci *` parameter in places (ECX from vtable call path).
