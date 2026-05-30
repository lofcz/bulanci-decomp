# CDSApp

## Status

**PARTIAL** — application shell **`0x284` (644 B)**; proven fields from `CDSApp_ctor` @ `0x0042b170`, `app_shell.md`, and render/input paths. **`CBulanci`** embeds **`CDSApp app`** @ `+0x00` (agent todo 2, 2026-05-30).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Prefix ends @ `+0x284` | `CBulanci.md` | `pGameEmbed` @ `+0x284`; `0x284` = CDSApp logical span in shipping class |
| `CDSBackBuffer` @ `+0x7c` | `0x0042b170` | `CDSApp_ctor`: `g_pCDSBackBuffer_vftable_*` at `this+0x7c` / `+0x80` |
| Embed span `0x50` | `0x0042b170` | Next init at `+0xcc` (physical client rect) |
| `CDSDirectSound` @ `+0x200` | `0x0042b170` | `CDSDirectSound_ctor(this+0x200)` |
| `drawable` @ `+0x274` | `CDSApp_RenderFrame` / ctor | `this[0x274]` gate; ctor sets via `pPad_preGame[0x1e8]` |
| `windowed` @ `+0xe4` | `0x0042b170` | Registry `Windowed` → `this+0xe4` (`pPad_preGame[0x58]` in CBulanci decompile) |

## Layout table (CDSApp @ `+0x00`–`+0x283`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CDSApp_ctor@0x0042b170` → `g_pCDSApp_vftable` |
| `+0x04` | 4 | `pointer` | `vftable_sub04` | ctor |
| `+0x10` | 4 | `pointer` | `vftable_sub10` | ctor |
| `+0x18` | 4 | `pointer` | `vftable_sub18` | ctor |
| `+0x20` | 16 | `int[4]` | `logicalRect` | `CDSApp_OnCreate` / `CDSView_SetRect` family |
| `+0x44` | 2 | `ushort` | `wFlags1` | `CDSView_DoModal`, `app_shell.md` |
| `+0x46` | 2 | `ushort` | `wFlags2` | input eligibility bits |
| `+0x4a` | 2 | `ushort` | `wExitCode` | modal loop |
| `+0x4c` | 4 | `pointer` | `pParentModal` | modal nesting |
| `+0x54` | 4 | `pointer` | `pChildChain` | `CDSChained` children |
| `+0x68` | 4 | `pointer` | `pClassName` | `CDsStringAssignFromLiteral` @ ctor |
| `+0x6c` | 4 | `pointer` | `pRegistryPath` | ctor |
| `+0x7c` | 80 | `CDSBackBuffer` | `backBuffer` | `CDSApp_ctor`; flip @ `CDSBackBuffer_Flip(this+0x7c)` |
| `+0xc4` | 4 | `int` | *(in embed)* | `defaultBpp = 8` → `backBuffer.embeddedImage+0x44` |
| `+0xcc` | 16 | `int[4]` | `physicalRect` | mouse rescale when windowed |
| `+0xdc` | 4 | `int` | `nDisplayModeIdx` | ctor `= 7` |
| `+0xe0` | 4 | `int` | `nMouseX` | `CDSApp_DispatchInputEvent` |
| `+0xe4` | 1 | `byte` | `bWindowed` | registry `Windowed` @ ctor |
| `+0xe8` | 4 | `pointer` | `pCurrentView` | `CDSApp_RenderFrame` child swap |
| `+0xec` | 4 | `pointer` | `pPendingView` | render path |
| `+0xf0` | 16 | `byte[16]` | `keyDownBitmap` | `CDSApp_DispatchInputEvent` modal-focus per-VK down bits |
| `+0x100` | 256 | `byte[256]` | `keyLatchByVk` | `CDSApp_ctor` `_memset(...,0,0x100)`; `CGaming_SyncKeyLatchAfterModal@0x00429f70` diffs bits 1/2 vs gaming copy; `CGaming_RunPreMatchModal` snapshots 0x40 dwords |
| `+0x110` | — | *(in latch)* | `keyLatchByVk[VK_SHIFT]` | `CWindow_OnKeyDownTabEscEnter` bit0 → tab focus backward |
| `+0x111` | — | *(in latch)* | `keyLatchByVk[VK_CONTROL]` | bits 0–1 = engine input-block latch (Enter / dialog paths) |
| `+0x200` | 84 | `CDSDirectSound` | `directSound` | `CDSDirectSound_ctor(this+0x200)` |
| `+0x254` | 4 | `pointer` | `pDirtyRectArray` | `CDSApp_RenderFrame` |
| `+0x25c` | 4 | `int` | `nDirtyRectCount` | render dirty list |
| `+0x274` | 1 | `byte` | `bDrawable` | ctor `= 1` |
| `+0x275` | 1 | `byte` | `bDirtyDuringFrame` | frame repaint |

## Key methods

| Symbol | Address |
|--------|---------|
| `CDSApp_ctor` | `0x0042b170` |
| `CDSApp_OnCreate` | `0x0042a210` |
| `CDSApp_Run` | `0x00429d60` |
| `CDSApp_PumpTick` | `0x0042be60` |
| `CDSApp_FrameBody` | `0x0042bda0` |
| `CDSApp_RenderFrame` | (see `app_shell.md`) |
| `CDSApp_WndProc` | `0x00429c00` |

## Ghidra apply

Slice **01** (2026-05-30):

- Deleted 1-byte placeholder; `create_struct CDSApp` → **`644` bytes** (`get_struct_layout`).
- `set_function_prototype` `CDSApp_ctor@0x0042b170` as `void __thiscall CDSApp_ctor(CDSApp *this, …)`.
- **`CBulanci`** re-parented: `CDSApp app` @ 0 in `CBulanci` (1228 B total); decompiler may use `field_0x*` for some base offsets until inheritance flattening.

**Agent todo 2 r2 (2026-05-30):** Recreated **`CDSApp`** (644 B); named MI/view band **`+0x08..+0x67`**: `miPad_*` between vtables; `nHitBounds_*` @ `+0x30`; `dwViewField_40`; `wAnchorBits`; chain tail `pVftable_chain_IDSChained` + `dwChainField_5c/60/64`. Recreated **`CBulanci`** (`app` @ 0, `pMainMenu` @ 640, `game` @ 644). Prototypes @ `0x0042b170`, `0x004026f0`, `0x0042c480`, `0x004028a0`. `save_program`.

**Agent todo 2 r3 (2026-05-30):** `pPad_100` → **`keyLatchByVk` `byte[256]`** @ `+0x100..+0x1ff`; `pKeyDownBitmap` → **`keyDownBitmap`**. Comments @ `CDSApp_ctor+0x100`, `CGaming_SyncKeyLatchAfterModal@0x00429f70`, tab/enter consumers. `rename_function` `FUN_00429f70` → `CGaming_SyncKeyLatchAfterModal`. `save_program`.

**R3 worker todo 2 (2026-05-30):** Re-verified band via MCP (`LEA [ESI+0x100]` memset, `g_pApp+0x110/0x111` consumers, `CGaming_RunPreMatchModal` snapshot). `set_function_this_type` `CDSApp_ctor` / `CDSApp_DispatchInputEvent` / `CDSApp_OnCreate`. Decompiler comments @ `0x00403dc8`, `0x00403e02`, `0x0041c4a0`, `0x00429f70`. `save_program`.

## UNK

- `+0x08..+0x1f`: MI padding between secondary vtables.
- `+0x40..+0x43`: `dwViewField_40` (no sole writer).
- `+0x70..+0x7b`: `miPad_before_backBuffer` (12 B before `CDSBackBuffer` @ `+0x7c`).
- ~~`+0x100..+0x1ff` opaque~~ — **done** (agent todo 2 r3): `keyLatchByVk[256]`; VK `0x10`/`0x11` slots repurposed for tab-focus / input-block flags.
- `+0x228..+0x253`: between DirectSound tail and dirty-rect array.
- Reconcile **`CDSDirectSound`** Ghidra size (84 B) vs historical **`0x54`** bound.

## Follow-up

- ~~Reparent **`CBulanci`** to extend **`CDSApp`**~~ — **done** (agent todo 2): `CDSApp app` @ 0.
- `CDSApp_InitDirectDraw` / `CDSApp_SetWindowed` field consumers in `+0x100..+0x1ff`.
