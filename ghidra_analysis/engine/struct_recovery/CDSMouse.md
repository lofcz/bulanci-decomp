# CDSMouse

## Status

**VERIFIED** — `sizeof == 0x0C`; abstract mouse interface (dual vtable + refcount only). **Registration-only at runtime:** `CDSMouse_Factory` is linked into `g_apClassByIdTable[0x1d]` at static init but has **no code xrefs**; base vtables `0x00483548` / `0x0048355c` are written **only** in that factory. Concrete mice (`CDSImageMouse`, `CGunMouse`) allocate larger objects with separate vtable clusters and never call `CDSMouse_Factory`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSMouse) == 0x0C` | `CDSMouse_Factory` @ `0x0042aa30` | `PUSH 0xc` → `OperatorNewWithBadAlloc` (`0x00447c42`); writes vtables at `+0` / `+4`, `refCount=1` at `+8` |
| Class id `0x1d` (29) | `CDSMouse_StaticClassRegister` @ `0x0047c600` | `HandleClassRegister(meta 0x4b3b54, classId 0x1d, parent 0x4b7bfc, factory 0x0042aa30)` |
| `IDSEventHandler` vtable | `0x00483548` | `CDSMouse_Factory@0x0042aa4b`: `MOV [EAX+0x4], 0x483548` |
| `IDSChained` vtable | `0x0048355c` | `CDSMouse_Factory@0x0042aa45`: `MOV [EAX], 0x48355c` |
| Class registry blob | `CDSMouse_GetClassRegistry` @ `0x00423b10` | Returns `&g_CDSMouse_classMeta` (`0x004b3b54`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `vf_IDSChained` | `CDSMouse_Factory@0x0042aa45` → `0x48355c` |
| 0x04 | 4 | `void *` | `vf_IDSEventHandler` | `CDSMouse_Factory@0x0042aa4b` → `0x483548` |
| 0x08 | 4 | `uint` | `refCount` | `CDSMouse_Factory@0x0042aa3e` → `[EAX+0x8] = 1` |

## Vtable map (interface only)

| Vtable @ | Interface | Slots | Notable entries |
|----------|-----------|-------|-----------------|
| `0x00483548` | `IDSEventHandler` | 4 | `[0]` `CDSFileStream_GetClassTable@0x00401600`, `[1]` `CDSChain_AdjustThisOffset@0x0042ac90`, `[2]` `IDSStream_ReleaseRefcount_thunk@0x00409260`, `[3]` `CDSMouse_ScalarDeletingDtor_thunk@0x00423b20` |
| `0x0048355c` | `IDSChained` | 7 | `[0]` `CDSMouse_GetClassRegistry@0x00423b10`, `[1]` `CDSException_DtorScalar@0x00434ae0`, `[2]` `CDSFileStream_GetName@0x00401660`, `[3..6]` shared no-op `CDSApp_PreCreateHook@0x00467430` |

## Runtime / heap usage (round 3 task 14)

| Path | Used at runtime? | Evidence |
|------|------------------|----------|
| `CDSMouse_Factory` → `OperatorNew(0x0C)` | **No** (retail binary) | Sole xref DATA @ `CDSMouse_StaticClassRegister`; factory imm32 appears once in `.text` |
| `InitializeByClassId(0x1d)` | **No** (shipped data) | Would call factory via `g_apClassByIdTable[0x1d]`; overlay resource class ids are 65536+; `PUSH 0x1d` for class id only in static register |
| `CDSImageMouse` / `CGunMouse` | **Yes** | Direct `OperatorNew` + class ctors; vtables `0x486f68`/`0x486f7c` and `0x4837ac`/`0x483794` respectively — **not** `0x483548`/`0x48355c` |

Static init sequence @ `0x0047c600`: class register → `CDSMouse_StaticInterfaceRegister` @ `0x0047c630` (`HandleInterfaceRegister` on meta `0x4b3b68` with parent `g_CDSMouse_classMeta`). Same pattern as other abstract CDS types (cf. `CDSStaticTexts` in `static_texts.md`).

## Related types

| Type | Size | Factory / ctor | Class id | Notes |
|------|------|----------------|----------|-------|
| **`CDSMouse`** | `0x0C` | `CDSMouse_Factory` `0x0042aa30` | `0x1d` | Abstract registry shell only |
| **`CDSImageMouse`** | `0x88` | `CDSImageMouse_CreateObject` `0x0042af00` | `0x35` | Menu backing-image cursor pattern |
| **`CGunMouse`** | `0x218` | `CGunMouse_CreateObject` `0x00426500` | `0x7d1` | Live menu sniper cursor: `CBulanci_InitResourceBank` → `CGunMouse_ctor` |

See `CDSImageMouse.md`, `CGunMouse.md`, `round3_task_12_report.md`.

## Ghidra apply

```
get_struct_layout("CDSMouse") → Size: 12 bytes
  pVf_IDSChained (+0), pVf_IDSEventHandler (+4), dwRefCount (+8)
```

Disassembly @ `CDSMouse_Factory@0x0042aa30`: `PUSH 0xc` → `OperatorNewWithBadAlloc`; `[EAX+0]=0x48355c`, `[EAX+4]=0x483548`, `[EAX+8]=1`.

Applied slice 33: `create_struct` (12 B); prototype on `CDSMouse_Factory`; plate comment documents registration-only factory.

## UNK

- **`CDSMouse_ScalarDeletingDtor_thunk@0x00423b20`** — calls `CDSException_DtorScalar(this-4)` (`AdjustThisOffset(-4)`); no proven consumer on bare 12-byte instances (vtable slot also referenced from data @ `0x00486cbc`).
- Custom/mod resource streams carrying deserialize class id **`0x1d`** (engine path exists via `InitializeByClassId`; retail overlay does not use it).

## Notes

- Documented in `ghidra_analysis/engine/player_controls.md` as base mouse alongside `CDSImageMouse`.
- Only **two** game methods in `mapping.csv` (`0x00423b10`–`0x00423b28`); all instance layout evidence comes from `CDSMouse_Factory`.

## Follow-up

- Round 3 task 14: heap vs registration — **resolved** (abstract registration only in retail binary).
