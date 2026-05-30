# CDSCollection

## Status

**VERIFIED** — size 0x18 (24 bytes); all six fields have ctor/resize/insert/load/save xrefs. Payload dword range **+0x08..+0x17** fully assigned (vector core); base vtables at **+0/+4** match engine `IDSReferenced` / `IDSChained` faces (round 3 task 37).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Object size ≥ 0x18 | — | Last field `m_growthChunk` at +0x14 (4 bytes) → end +0x18 |
| Embedded in `CDSStreamStorage` at +0x1c | `CDSStreamStorage_ctor` @ 0x00401790 | Vtables at parent+0x1c/+0x20; data at parent+0x24..+0x30 |
| `FindKeyIndex` consumer | `CDSStreamStorage_FindKeyIndex` @ 0x00431170 | `ECX=storage+0x1c`: `[EBP+0x8]`/`[EBP+0xc]` = `m_items`/`m_count`; from `CloseStreamByKey@0x00433cb0` (`LEA ECX,[ESI+0x1c]`) and `CDSCollection_InsertKeyed@0x004312dc` |
| `CDSCollection_ctor` base | 0x00431260 | Writes vtables at +0/+4; calls `CDSCollection_Resize` |
| Next sibling after `CDSDirectSound` in `CBulanci` | `CBulanci::CDSApp_ctor` @ 0x0042b116 | `CDSDirectSound_ctor` at +0x200; next init at +0x254 → DS object span 0x54 |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `m_pVtable_IDSReferenced` | `CDSCollection_ctor` @ 0x00431260 → temp `CDSCollection::vftable`, then `g_pCDSObject_vftable_IDSReferenced` |
| 0x04 | 4 | `void *` | `m_pVtable_IDSChained` | `CDSCollection_ctor` @ 0x00431260 → `g_pCDSCollection_vftable_IDSChained` (`0x0047f6e4`) |
| 0x08 | 4 | `void **` | `m_items` | `CDSCollection_Resize` @ 0x00431000; `_Globals::CDSCollection_Insert` @ 0x004310f0; `_Globals::CDSCollection_Remove` @ 0x00431140; `CDSStreamStorage_FindKeyIndex` plate @ 0x00431170 (`coll+0x8`) |
| 0x0C | 4 | `int` | `m_count` | `CDSCollection_Resize` @ 0x00431000; `_Globals::CDSCollection_Insert` @ 0x004310f0; `CDSCollection_Save` @ 0x00431210; `FindKeyIndex` (`coll+0xc`) |
| 0x10 | 4 | `int` | `m_capacity` | `CDSCollection_Resize` @ 0x00431000; `_Globals::CDSCollection_EnsureCapacity` @ 0x00431080 |
| 0x14 | 4 | `int` | `m_growthChunk` | `_Globals::CDSCollection_EnsureCapacity` @ 0x00431080; default `0x20` in `CDSStreamStorage_ctor` @ 0x00401790 (parent+0x30) |

### Payload +0x08..+0x17

Sixteen bytes covering **`m_items`**, **`m_count`**, **`m_capacity`**, **`m_growthChunk`** — no gaps or padding in this band. Consumers use typed `CDSCollection *` (`CDSCollection_Resize`) or raw `this+0x8` / `+0xc` / `+0x10` / `+0x14` (`_Globals::CDSCollection_*`).

## Base vtables (+0 / +4)

| Face | Address | Size | Slots | [0] | [1] | [2] | [3] | [4] | [5] |
|------|---------|------|-------|-----|-----|-----|-----|-----|-----|
| `IDSChained` | `0x0047f6e4` | 0x1c | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` | `CDSCollection_ScalarDeletingDtorThunk` @ 0x00401270 | `CDSCollection_Load` @ 0x00431360 | `CDSCollection_Save` @ 0x00431210 |
| `IDSReferenced` | `0x0047f700` | 0x20 | 3 | `CDSCollection_GetTypeInfo` @ 0x00401230 | `CDSCollection_ScalarDeletingDtor` @ 0x00401240 | `CDSObject_ReleaseViaVtable` @ 0x004245c0 | — | — | — |

**Pattern:** First three `IDSChained` slots match `CDSChain` / `CDSChained` engine stubs (`FUN_004011f0`, `FUN_0042ac90`, `FUN_0041aa40`); `IDSReferenced` face matches the usual 3-slot refcount/typeinfo/release layout. Type-specific work is slots [3..5] on the chained face (load/save/thunk).

**DATA xrefs** (install into `this+4` / embedded `parent+0x20`): `CDSCollection_ctor` @ 0x0043128e / 0x00431288; `CDSStreamStorage_ctor` @ 0x004017e3 / 0x004017dc; `CLevelScriptOpExt_NewCollection` @ 0x00418a66 / 0x00418a60; `CDSUpdatedItem_ctor` @ 0x0042f0c0 / 0x0042f0ba.

Parent-type (`IDSReferenced` / `IDSChained`) struct sizes and RTTI tail at `+0x18` / `+0x0c` are catalogued in `master_vtable_catalog.md`, not re-derived here.

## Ghidra apply

```
get_struct_layout CDSCollection → Size: 24 bytes (6 fields)
```

Applied via `create_struct` (batch 27). Ghidra field names use `pM_` / `nM_` prefixes; layout matches table above.

## UNK

- None at collection payload (+0x08..+0x17).
- `CDSCollection_ScalarDeletingDtor` @ 0x00401240 calls `CDSCollection_ctor()` with no `this` (misnamed teardown wrapper); element release uses `CDSCollection_Resize` with `param_2 != 0` @ 0x00431000.
- `CDSCollection_Save` / `CDSCollection_Load` decompile through `this+4` (chained sub-pointer); some call sites still show stale pointer arithmetic — see `CDSCollection_InsertKeyed` (task 38).

## Follow-up (round 3 task 37)

- Re-verified payload and vtables via live decompile + `read_memory` on `0x0047f6e4` / `0x0047f700`.
- Corrected embedded-parent ctor citation: `CDSStreamStorage_ctor` @ **0x00401790** (was `0x004013b0`).

## Follow-up (round 3 task 44)

- **`FindKeyIndex` path:** `CloseStreamByKey` passes embedded collection (`storage+0x1c`); lookup uses `m_items`/`m_count`; entry load uses parent `+0x24` — see [CDSStreamStorage.md](./CDSStreamStorage.md).
- Decompiler plate @ `0x00431170` documents collection `this` and search modes.

## Leaf functions (slice 27)

| Address | Symbol | Role |
|---------|--------|------|
| 0x00401230 | `CDSCollection_GetTypeInfo` | `IDSReferenced` vtable slot |
| 0x00401240 | `CDSCollection_ScalarDeletingDtor` | Release / teardown |
| 0x00401270 | `CDSCollection_ScalarDeletingDtorThunk` | Chained dtor thunk |
| 0x00431000 | `CDSCollection_Resize` | `m_items` / `m_count` / `m_capacity` |
| 0x00431070 | `CDSCollection_EnsureCapacity` | Growth via `m_growthChunk` |
| 0x004310b0 | `_Globals::CDSCollection_Insert` | Insert at index (`this+8`/`+c`) |
| 0x00431100 | `CDSCollection_Remove` | Remove slot |
| 0x00431210 | `CDSCollection_Save` | Serialize count + elements |
| 0x00431260 | `CDSCollection_ctor` | Vtables + empty resize |
| 0x004312c0 | `CDSCollection_InsertKeyed` | Keyed insert + `FindKeyIndex` |
| 0x00431360 | `CDSCollection_Load` | Deserialize |
| 0x0042fd40 | `CDSCollection_DeserializeElement` | Stream factory path |
| 0x0042ff20 | `CDSCollection_SerializeElement` | Per-element save |

**Consumers (not owned by this type):** `CDSStreamStorage_FindKeyIndex` @ 0x00431170 (`ECX = storage+0x1c`); `CDSStreamStorage_ctor` @ 0x00401790 embeds collection at parent **+0x1c**.

## Follow-up (slice 27 / batch 27)

- Ghidra struct **24 B / 6 fields** confirmed (`get_struct_layout`).
- `CDSCollection_Resize` decompiles with typed `CDSCollection *` (`pM_items`, `nM_count`, `nM_capacity`).
- `CDSCollection_InsertKeyed` — resolved r3 todo 43 via `set_function_this_type` (`CDSCollection *this`; was mis-attributed `CDSUpdatedItem *`).
- `CDSCollection_Save` uses `this->pM_items` / `nM_capacity`; element walk still goes through `pM_pVtable_IDSChained` (+0x4) — chained-subobject quirk unchanged.

## Follow-up (agent todo 30, 2026-05-30)

| Item | Result |
|------|--------|
| `CDSCollection_InsertKeyed` @ `0x004312c0` | Plate comment **Parameters** documents `CDSCollection *this`; asm proof `[ESI+0x8]`/`[ESI+0xc]` = `m_items`/`m_count` @ `0x00431303`..`0x00431321` |
| Root cause | Sole xref `CDSUpdatedItem_ctor@0x0042f107` passes `g_pTaskList` (heap `CDSCollection`); `+0x8`/`+0xc` collide with `CDSUpdatedItem::pSelf` / `pEventSlots` names |
| Ghidra API | `set_function_prototype(CDSCollection *)` applied; MCP warns **ECX auto-parameter cannot be retyped** — decompiler header stays `CDSUpdatedItem::CDSCollection_InsertKeyed(CDSUpdatedItem *this, …)` |
| `CDSStreamStorage_FindKeyIndex` @ `0x00431170` | Prototype retargeted to `CDSCollection *this` (same ECX limitation); plate/decompiler comments unchanged |

**Manual fix (if needed):** In decompiler UI, right-click `this` → Retype Variable → `CDSCollection *`.

## Follow-up (agent todo 43 r2, 2026-05-30)

| Item | Result |
|------|--------|
| MCP retry | `set_function_prototype` @ `0x004312c0` → `CDSCollection *this` (accepted; ECX auto-param warning) |
| Blocked APIs | `set_parameter_type`, `set_local_variable_type`, `set_variables` → *Auto-parameter may not be modified* |
| Script | `run_script_inline` (namespace detach) → OSGi class load failure |
| Live state | `get_function_variables`: `this` still `CDSUpdatedItem *`, storage `ECX:4 (auto)`; decompiler header `CDSUpdatedItem::CDSCollection_InsertKeyed` |
| Caller proof | `CDSUpdatedItem_ctor@0x0042f107` decompiles `CDSCollection_InsertKeyed(g_pTaskList, …)` — first arg is heap collection, not `this` |
| Asm proof | `MOV ESI,ECX`; `[ESI+0x8]`/`[ESI+0xc]` @ `0x00431303`..`0x00431321` = `m_items`/`m_count` |
| Docs | Plate + PRE comment refreshed (todo 43); `save_program` |

## Follow-up (agent todo 43 r3, 2026-05-30)

| Item | Result |
|------|--------|
| `set_function_this_type` @ `0x004312c0` | **`CDSCollection *`** — `get_function_variables` confirms `this` on ECX; decompiler `CDSCollection::CDSCollection_InsertKeyed` |
| Plate @ `0x004312c0` | Parameters/Algorithm/Returns: `CDSCollection *this` (ECX); asm `[ESI+0x8]`/`[ESI+0xc]` = `m_items`/`m_count` @ `0x00431303`..`0x00431321` |
| ECX xref bases | `InsertKeyed@0x004312dc`: `ECX=this` (`MOV ESI,ECX`); `CloseStreamByKey@0x00433d09`: `LEA ECX,[ESI+0x1c]`; PRE @ `0x00433d09` |
| Caller | PRE @ `0x0042f107`: `g_pTaskList` is heap `CDSCollection`, not `CDSUpdatedItem*` |
| Remaining | Callee `FindKeyIndex@0x00431170` still decompiles as `CDSStreamStorage::…` with wrong field names on `this` — plate `CDSCollection*` separately |
| `save_program` | `bulanci.exe` saved |

## Follow-up (R3 task 28, 2026-05-30)

| Item | Result |
|------|--------|
| MCP | `set_function_this_type(CDSCollection *)` @ `0x004312c0` — function moved into class `CDSCollection` |
| Decompile | `CDSCollection::CDSCollection_InsertKeyed(CDSCollection *this, …)`; body uses `this->pM_items`, `this->nM_count` |
| Variables | `get_function_variables`: `this` = `CDSCollection *`, storage `ECX:4 (auto)` |
| Prior blocker | Supersedes ECX auto-param limit documented for todo 43; use `set_function_this_type` not prototype-only |
