# Round 10 — Deep Task 09 Report (CWeapon layout + combat path)

## Task

| Field | Value |
|-------|-------|
| **id** | 9 |
| **title** | Struct deep: CWeapon layout + combat path |
| **kind** | struct |
| **struct_name** | `CWeapon` |
| **seed_address** | `0x0041b180` |
| **addresses** | `0x0041b180`, `0x0041a6a0`, `0x0041a710` |

## Status

**DONE** — `sizeof(CWeapon) == 0x70` (112 B) and all tail fields `+0x50`..`+0x6C` re-proven via live Ghidra MCP (ctor/disasm, dtor/decompile, alloc xrefs) with IDA `bulanci.ida.exe.c` correlation. Ghidra struct already matches; ctor prototype fixed to 4-arg form.

## Functions / Struct

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041dbc0` | `CWeapon::CWeapon_ctor` | Full weapon init; writes all tail fields | **Disasm:** zeros `+0x50/+0x5c/+0x60/+0x68/+0x6c`; `MOV [ESI+0x64], AL` from `[ESP+0x34]`; `SetCurrentTrack` uses `[ESP+0x38]` @ `0x0041dd2a`. **IDA:** `sub_41DBC0` stores `a3` @ `+100`, `a4` → `sub_439EB0(this+8,…)`. **Xrefs (2):** `CBulanekCtor@0x0041e8ef`, `CBulanek_ApplyPickupEffect@0x0041eb0f` |
| `0x0041c550` | `CWeapon::CWeapon_dtor` | Releases tail pointers in reverse ctor order | **IDA `sub_41C550`:** `this+20` (+0x50) `pTrackHolder`, `+26` (+0x68) `pHudIconA`, `+27` (+0x6c) `pHudIconB`; then `sub_439D30(this+2)` destroys `trackManager` |
| `0x0041e8ef` / `0x0041eb0f` | alloc sites | Heap `0x70` → ctor → `CBulanek+0xF8` | **Ghidra:** `OperatorNewWithBadAlloc(0x70)`; **IDA:** `operator new(0x70u)` @ lines 95646, 95739 |
| `0x0041bf80` | `CWeapon_SetTrackHolder` | Copies `trackManager.pRenderTarget` → `+0x50` | **Disasm @ `0x0041dd3c`:** `MOV EAX,[ESI+0x38]` / `CALL 0x0041bf80` |
| `0x004212b0` | `CWeapon::Fire` | Combat dispatch; reads `pTrackHolder`, `pOwner`, `dwParamB` | **Decompile:** mine @ `weaponKind==2`, shot @ `4`/`5`; `pTrackHolder` for ammo/aim, `pOwner` for `AddEntity` |
| `0x0041b180` | `CBulanek::CWeapon_OnSchedulerEvent` | **Not** `CWeapon*` — MI adjustor on `CBulanek+0xA0` | **IDA:** `sub_41B180` → `sub_41AF70(this-40)`; **Disasm:** `ADD ECX,0xffffff60` |
| `0x0041a6a0` | MFC `CreateObject` | Class factory `0x7ec`; alloc **`0x19c`** → `CGameView` shell | **Not CWeapon** — unrelated neighbor in task band |
| `0x0041a710` | MFC `CreateObject` | Class factory `0x7eb`; alloc `0x70` → **`sub_418EF0`** partial stub | **Not full CWeapon path** — stub only sets vtables + `ConstructTrackManager`; no HUD icons / track load |

### Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x70` | `CBulanekCtor`, `CBulanek_ApplyPickupEffect` | `OperatorNewWithBadAlloc(0x70)` / IDA `operator new(0x70u)` immediately before `CWeapon_ctor` |
| Last field ends @ `+0x6C` | `CWeapon_ctor` disasm | `MOV [ESI+0x6c], EDI` (zero init); struct size `0x6c+4 = 0x70` |
| Ghidra DT manager | `get_struct_layout` | `CWeapon` size **112** bytes; fields through `pHudIconB` @ **108** |

### Layout (proven offsets)

| Offset | Size | Type | Name | Evidence |
|--------|------|------|------|----------|
| `+0x00` | 4 | `void *` | `pVftable_primary` | ctor → `0x481eec`; dtor restore |
| `+0x04` | 4 | `void *` | `pVftable_secondary` | ctor → `0x481ed4` |
| `+0x08` | `0x48` | `CDSVideoPlayer` | `trackManager` | `ConstructTrackManager(&+0x08)`; dtor `TM_Destructor` |
| `+0x50` | 4 | `CBulanek *` | `pTrackHolder` | ctor zero + `CWeapon_SetTrackHolder`; dtor release `this+20` |
| `+0x54` | 4 | `CBulanek *` | `pOwner` | ctor `MOV [ESI+0x54], EAX` @ `0x0041dd52` from stack `pOwner` |
| `+0x58` | 4 | `CGaming *` | `pWorld` | ctor `MOV [ESI+0x58]` from `pOwner+0x84` (`pGamingHostScratch`) |
| `+0x5C` | 4 | `uint` | `dwParamA` | ctor zero @ `0x0041dc12` |
| `+0x60` | 4 | `uint` | `dwParamB` | ctor zero @ `0x0041dc15`; `Fire` reads as weapon-kind dispatch byte |
| `+0x64` | 1 | `byte` | `bWeaponKind` | ctor `MOV [ESI+0x64], AL` from `[ESP+0x34]` |
| `+0x65` | 3 | padding | — | Ghidra `pPad_0x65`; no accesses in ctor/dtor/xrefs |
| `+0x68` | 4 | `CDSObject *` | `pHudIconA` | ctor/dtor index `+26`; bitmap load + `CheckedVirtualBaseCast` |
| `+0x6C` | 4 | `CDSObject *` | `pHudIconB` | ctor/dtor index `+27`; conditional secondary bitmap |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0041dbc0` | `void CWeapon_ctor(CBulanek *pOwner, uchar weaponKind, uchar initialTrackIdx)` `__thiscall` |
| `set_decompiler_comment` | `0x0041dc2e`, `0x0041dd2a` | Asm-backed weaponKind vs initialTrackIdx stack slots |
| `force_decompile` | `0x0041dbc0` | 4th param visible; `SetCurrentTrack` arg still decompiler-artifact |
| `save_program` | `bulanci.exe` | saved |

**No struct mutation** — `get_struct_layout` already shows 112 B with all proven fields.

## Decomp corrections (IDA vs Ghidra)

| Issue | IDA (`sub_41DBC0`) | Ghidra (before R10) | Resolution |
|-------|-------------------|---------------------|------------|
| Ctor arity | 4 stack params: `pOwner`, `weaponKind`, `initialTrackIdx` | 3 params; `SetCurrentTrack` showed `(uint)pOwner & 0xff` | **Prototype fixed**; asm @ `0x0041dd2a` uses `[ESP+0x38]` = 4th param (`a4` / `initialTrackIdx`), distinct from `weaponKind` @ `[ESP+0x34]` |
| `pOwner` store | `*(_DWORD *)(this + 84) = a2` at end | Bogus `this->pOwner = local_4` (SEH) | **Asm @ `0x0041dd52`** proves stack `pOwner` → `+0x54` |
| `0x0041a710` factory | `operator new(0x70)` → `sub_418EF0` (partial) | Labeled `ODSImage_FactoryCtor` | **Not** gameplay `CWeapon`; only stub vtable + empty `trackManager` |
| `0x0041b180` | `sub_41B180(this-40)` — `CBulanek*` facet | Named `CWeapon_OnSchedulerEvent` | Correct name but **wrong `this` type** — event handler on `CBulanek+0xA0`, not `CWeapon*` |
| CBulanekCtor weapon kind arg | `sub_41DBC0(v20, this, a9, a5)` — `a9` = 9th ctor param | Passed `(uchar)skinPaletteId` | **IDA:** `a9` is separate from palette; Ghidra caller typing still wrong (document only) |

## Frida

Not required — static ctor/dtor/disasm + alloc xrefs sufficient.

## Remaining UNK

- `SetCurrentTrack` decompile at `0x0041dd37` may still show `(uint)pOwner & 0xff` despite 4-arg prototype (decompiler artifact; asm is authoritative).
- `CBulanekCtor` Ghidra call site may still pass wrong 3rd arg name (`skinPaletteId` vs `a9` weapon-kind param) — needs caller prototype fix outside this task band.
- `0x0041a710` / `sub_418EF0` — purpose of partial `CWeapon` vtable on MFC `ODSImage` factory (registry class `0x7eb`); not used for in-game `pWeapon` allocation.
