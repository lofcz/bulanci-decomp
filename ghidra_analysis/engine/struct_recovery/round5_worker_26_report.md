# Round 5 — Worker 26 report (CWeapon `trackManager` interior UNKs)

## Task

| Field | Value |
|-------|-------|
| **worker** | 26 / 50 |
| **round** | 5 (WRITE) |
| **focus** | `CWeapon.trackManager` interior `+0x08..+0x4F` — close R3/R4 UNKs with asm + Ghidra apply |
| **structs** | `CWeapon`, `CDSVideoPlayer`, `CDSTrackVector`, `CBulanek` |
| **prior** | [round4_task_49_report.md](./round4_task_49_report.md), [round3_task_49_report.md](./round3_task_49_report.md) |

## Status

**DONE** — asm-closed ctor epilogue + `SetCurrentTrack` arg; `pTrackHolder` typed and linked to `trackManager.pRenderTarget`; `FUN_0041bf80` → `CWeapon_SetTrackHolder`; `CDSVideoPlayer.trackVector` → `CDSTrackVector`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `SetCurrentTrack` track index = `weaponKind` | `0x0041dd2a` | `MOVZX EDX, byte ptr [ESP+0x38]` then `PUSH EDX` before `CALL 0x00439eb0`; **not** `(uint)pOwner & 0xff` (decompiler artifact) |
| `pOwner` assigned from ctor param | `0x0041dd4e` | `MOV EAX, [ESP+0x30]` / `MOV [ESI+0x54], EAX` after `BeginCurrentTrackPlayback` |
| `pTrackHolder` ← `pRenderTarget` | `0x0041dd3c` | `MOV EAX, [ESI+0x38]` / `PUSH EAX` / `CALL CWeapon_SetTrackHolder` (`ESI+0x38` = `trackManager.pRenderTarget`) |
| `pTrackHolder` used as `CBulanek*` in `Fire` | `0x0042130a`..`0x00421593` | `MOV ECX, [ESI+0x50]` then `+0x20/+0x24/+0x70/+0xd4/+0x16b` — standard `CBulanek` field offsets |
| `pOwner` vs `pTrackHolder` split | `0x00421319` | Mine spawn: `CMina_Ctor(..., [ESI+0x50])`; `AddEntity([ESI+0x54], ...)` |
| Pistol fire uses `TM_Play` track 0 | `0x0041fd22` | `LEA ECX, [ESI+0x8]` → `CDSVideoPlayer::TM_Play(&trackManager, 0)` |
| Holder swap helper | `0x0041bf80` | Release prior `+0x50`, `AddRef` new holder, `CIntListInsertSortedOrAppend(holder+0x38, this+4, …)` — registers weapon scheduler facet |
| `CDSVideoPlayer.trackVector` type | struct | `modify_struct_field` → `CDSTrackVector` @ offset 28 (0x1c); size 72 unchanged |

### Weapon runtime use of `trackManager` interior (CWeapon absolute)

| CWeapon abs. | TM field | Weapon role |
|--------------|----------|-------------|
| `+0x08` | `pVftable_IDSChained` | `ConstructTrackManager` → `0x487788` |
| `+0x0C` | `scheduler` | `CBulanek` polls `pWeapon->trackManager.scheduler` for FLX `0x0C` events |
| `+0x24` | `trackVector` | Ctor loop: 4× `AddTrackSource` from `gAWeaponDefsTable[kind]` stride `0x1C` |
| `+0x34` | `nCurrentTrackIdx` | `SetCurrentTrack(&trackManager, weaponKind, 0)` at ctor |
| `+0x38` | `pRenderTarget` | FLX decode consumer from active track; copied to `pTrackHolder` via `CWeapon_SetTrackHolder` |
| `+0x39` / `+0x3A` | `bPlayFlags` / `bPaused` | `TM_Play(&trackManager, 0)` on pistol (`CWeapon_FirePistol`) |
| `+0x40` | `dwCurrentFrameIdx` | Advanced by `BeginCurrentTrackPlayback` / `TM_AdvanceFrame` |
| `+0x44` / `+0x48` | cookies / delay | Default ctor init (`0`, `0xffffffff`, `-1` ms) — no weapon-specific writers found |

**Semantics:** `pTrackHolder` mirrors `trackManager.pRenderTarget`, which for weapons resolves to the **`CBulanek` host** used as FLX consumer (same pointer `Fire` uses for ammo/aim/spawn math). `pOwner` remains the entity passed into `AddEntity` / world wiring.

## Ghidra deltas (R5 worker 26)

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CDSVideoPlayer.trackVector` | `CDSTrackVector` @ +0x1c |
| `modify_struct_field` | `CWeapon.pTrackHolder` | `CBulanek *` @ +0x50 |
| `rename_function_by_address` | `0x0041bf80` | `CWeapon_SetTrackHolder` (class `CWeapon`) |
| `set_function_prototype` | `0x0041bf80` | `void __thiscall CWeapon_SetTrackHolder(CWeapon *, CBulanek *)` |
| `set_function_this_type` | `0x0041bf80` | `CWeapon *` |
| `set_function_prototype` | `0x00439eb0` | `void __thiscall SetCurrentTrack(CDSVideoPlayer *, int, char)` |
| `set_decompiler_comment` | `0x0041dd2a`, `0x0041dd42`, `0x0041dd4e` | Asm-backed ctor notes |
| `force_decompile` | ctor / `CWeapon_Fire` | `pTrackHolder` typed; `Fire` uses `CBulanek` fields on `pTrackHolder` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CWeapon.md](./CWeapon.md) — interior runtime table; `pTrackHolder` semantics; R5 worker 26 Ghidra log; trimmed UNK list.
- [CDSVideoPlayer.md](./CDSVideoPlayer.md) — note `trackVector` `CDSTrackVector` embed confirmed in Ghidra layout.

## Remaining UNK

- `SetCurrentTrack` decompile at `CWeapon_ctor` call site may still show `(uint)pOwner & 0xff` — **asm uses `weaponKind`**; cosmetic until call-site typing improves.
- Ctor loads **4** tracks (`0..3`) but passes **`weaponKind`** (`0..5`) as initial track index — no bounds check in `SetCurrentTrack@0x00439eb0`; verify retail only equips kinds `0..3` or table order guarantees safety.
- `CWeapon_SetTrackHolder` listener key `this+4` is `pVftable_secondary` address — intentional object identity for sorted listener list; not scheduler base.
