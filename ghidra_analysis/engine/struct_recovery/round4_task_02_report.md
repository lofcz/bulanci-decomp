# Round 4 — Task 02 Report (`CDSApp` keyLatch names + `OnCreate` `CBulanci*`)

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **title** | CDSApp keyLatch display names and OnCreate CBulanci* param |
| **one_liner** | Finish R3 latch band: Ghidra field display names for `keyLatchByVk` / `keyDownBitmap`; type `CDSApp_OnCreate` as `__thiscall` with `CBulanci *this` (factory object / `g_pApp`). |
| **acceptance** | `get_struct_layout` or decompile shows latch field names; `CDSApp_OnCreate` `__thiscall` + `CBulanci *`; `set_function_this_type` on ctor/dispatch/OnCreate; `save_program` |
| **source** | `todos_gather_r4_0.json` (fallback; `agent_todos_50_r4.json` not present in repo) |
| **supersedes** | R3 todo 2 — [round3_task_02_report.md](./round3_task_02_report.md) |
| **structs** | `CDSApp`, `CBulanci`, `CWindow` |

## Status

**DONE** (OnCreate typing); **PARTIAL** (struct export still lists `pKeyLatchByVk` / `pKeyDownBitmap`)

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Latch band @ `CDSApp+0x100` | `0x0042b315` | `CDSApp_ctor`: `LEA EDX,[ESI+0x100]` → `_memset(..., 0, 0x100)` |
| VK_SHIFT tab latch | `0x00403dc8` | `CWindow_OnKeyDownTabEscEnter`: `g_pApp+0x110` bit0 |
| VK_CONTROL input block | `0x00403e02` | same fn: `g_pApp+0x111` bits 0–1 before Enter |
| Modal down bitmap @ `+0xf0` | `0x00429db0` | `CDSApp_DispatchInputEvent`: `this->pKeyDownBitmap[vk]` |
| OnCreate vtable slot | `0x0042a210` | `CDSApp_AppMain` → `g_pApp` vtbl[28]; object from `CBulanci_CreateObject` (`0x4cc`) |
| Sync after modal | `0x00429f70` | `CGaming_SyncKeyLatchAfterModal`: `LEA [ESI+0x100]` vs gaming shadow |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CDSApp.pKeyLatchByVk` → `keyLatchByVk` | MCP success |
| `modify_struct_field` | `CDSApp.pKeyDownBitmap` → `keyDownBitmap` | MCP success |
| `get_struct_layout CDSApp` | — | Export still **`pKeyLatchByVk` / `pKeyDownBitmap`** @ 240/256 (array-decay naming; same as R3) |
| `set_function_this_type` | `0x0042a210` | `CBulanci *` — fn moved to **`CBulanci::CDSApp_OnCreate`** |
| `set_function_prototype` | `0x0042a210` | `void __thiscall CDSApp_OnCreate(CBulanci *this)` |
| `set_function_this_type` | `0x0042b170`, `0x00429db0` | Confirmed **`CDSApp *`** (`CDSApp_ctor`, `CDSApp_DispatchInputEvent`) |
| `force_decompile` | OnCreate / ctor / dispatch | OnCreate: `__thiscall CBulanci::CDSApp_OnCreate(CBulanci *this)`; ctor still indexes `pKeyLatchByVk` in decompile |
| `set_decompiler_comment` | `0x0042a210` | R4 note: `CBulanci*` = `g_pApp` after factory |
| `recreate_struct CBulanci` | embed `CDSApp app` @ 0 | Size 1228 preserved; layout export still omits `app` prefix (Ghidra UI quirk) |
| **`save_program bulanci.exe`** | — | saved |

### Decompile snapshot (post-R4)

- **OnCreate:** `void __thiscall CBulanci::CDSApp_OnCreate(CBulanci *this)` — uses `this->field_0x68` / `field_0x200` (flat offsets ≡ `CDSApp` prefix until `CBulanci.app` embed surfaces in decompiler).
- **Ctor:** `this->pKeyLatchByVk` in `_memset` (comment documents `keyLatchByVk` semantics).
- **Dispatch:** `this->pKeyDownBitmap[vk]` modal-focus path.

## Struct doc updates

- [CDSApp.md](./CDSApp.md) — R4 apply log.
- [CBulanci.md](./CBulanci.md) — no file edit (embed documented in prior todos).

## Remaining UNK

- Ghidra **`get_struct_layout`** / decompiler **identifier** for `pKeyLatchByVk` / `pKeyDownBitmap` (rename applied; export not refreshed).
- `CDSApp_OnCreate` body still **`field_0xNN`** until `CBulanci` shows nested `(this->app).pClassName` in decompile.
- Writers that set `keyLatchByVk[VK_CONTROL]` bits (unchanged from R3).
