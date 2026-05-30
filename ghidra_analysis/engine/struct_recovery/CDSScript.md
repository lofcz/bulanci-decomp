# CDSScript

## Status

**VERIFIED** (size 0x430 / 1072 bytes). Layout is **PARTIAL**: proven header + dispatch splice + `framePtr`; bytes `+0xe0..+0x42b` are reserved padding (subclass `CLevelScript` continues at `+0x434`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Object ends before `CLevelScript` tail fields | — | `CLevelScript::ctor@0x004185c0` writes `field_0x434` immediately after base init (`CDSScript::ctor` then `*(this+0x434)=1`) |
| Last base-class field `framePtr` at `+0x42c` | `0x00438b30` | `CDSScript_Run`: save/restore `*(this+0x42c)` around interpreter loop |
| Total size **0x430** | — | `0x42c` + 4-byte `framePtr` |
| `CLevelScript` heap size **0x460** (distinct) | `0x00464415` (approx) | `OperatorNewWithBadAlloc(0x460)` in level-script factory — not used for bare `CDSScript` |

## Vtable / multiple-inheritance (task 42)

### Prefix (`CDSScript` @ `+0x00..+0x28`)

Only **two** slots are vtable pointers. Subclass constructors **replace** these after `CDSScript::ctor`; they do **not** turn `+0x08..+0x20` into vptrs.

| Offset | Name | Base value (`CDSScript::ctor@0x00438390`) | Interface (`master_vtable_catalog.csv`) |
|--------|------|-------------------------------------------|----------------------------------------|
| `+0x00` | `pVftable_IDSReferenced` | `0x00487664` | `CDSScript` / `IDSReferenced` |
| `+0x04` | `pVftable_IDSChained` | `0x00487648` | `CDSScript` / `IDSChained` |

`CDSScript_dtor@0x00438400` restores `+0x00` / `+0x04` to the base vtables during destruction.

### Full script instance (subclass heap object)

After subclass ctor, **four** vtables are typical; **five** on `CLevelScript` (`+0x440` timer `CDSUpdatedItem`).

| Offset | `CLevelScript@0x004185c0` | `CHelpScript@0x004215e0` | `CHistoryScript@0x004226c0` |
|--------|---------------------------|--------------------------|----------------------------|
| `+0x00` | `0x00481dcc` `IDSReferenced` | `0x00482a3c` | `0x004830d0` |
| `+0x04` | `0x00481db0` `IDSChained` | `0x00482a20` | `0x004830b4` |
| `+0x430` | `0x00481d9c` `IDSEventHandler` | `0x00482a0c` | `0x004830a0` |
| `+0x438` | `0x00481d84` `IDSChained` | `0x004829f4` | `0x00483088` |
| `+0x440` | `0x00481d6c` `IDSChained` (timer item) | `0` (`pBoundView` data) | `0` (`pBoundView` data) |

Tail layout: `CHelpScript.md`, `CHistoryScript.md`, `CLevelScript` (no dedicated struct doc yet).

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | pointer | pVftable_IDSReferenced | `CDSScript::ctor@0x00438390` → `0x487664`; `CDSScript_dtor@0x00438400` |
| 0x04 | 4 | pointer | pVftable_IDSChained | `CDSScript::ctor@0x00438390` → `0x487648`; `CDSScript_dtor@0x00438400` |
| 0x08 | 4 | uint | dwPc | `CDSScript::ReadU8@0x00438380`, `CDSScript_Run@0x00438b30` |
| 0x0c | 4 | uint | dwReserved_0c | `CDSScript::ctor@0x00438390` (zero); no read in base `CDSScript` methods |
| 0x10 | 4 | pointer | bytecode | `ReadU8@0x00438380` (`*(pc)+bytecode)`; freed `CDSScript_dtor@0x00438400` |
| 0x14 | 4 | uint | dwReserved_14 | `CDSScript::ctor@0x00438390` (zero); no read in base `CDSScript` methods |
| 0x18 | 4 | pointer | scriptData | freed `CDSScript_dtor@0x00438400` (`param_1[6]`) |
| 0x1c | 4 | int | nExports | `CDSScript::CallExport@0x00438c40` |
| 0x20 | 4 | pointer | exports | `CallExport@0x00438c40`; freed `CDSScript_dtor@0x00438400` |
| 0x24 | 1 | byte | bReturnFlag | `CDSScript_Run@0x00438b30` loop; ctor sets `1` |
| 0x28 | 4 | int | nOpcodeCount | `InstallOpcodeTable@0x00438310` accumulates count |
| 0x2c | 180 | pointer[45] | pDispatchTable (base) | `InstallOpcodeTable@0x00438310(this,0,…,0x2d)` in ctor |
| 0x42c | 4 | pointer | framePtr | `CDSScript_Run@0x00438b30` |

## Ghidra apply

**Slice 02 (2026-05-30):** `get_struct_layout CDSScript` → **1072 B**, 15 fields (`pVftable_IDSReferenced`, `pVftable_IDSChained`, `dwPc`, `dwReserved_0c`, `bytecode`, `dwReserved_14`, `scriptData`, `nExports`, `exports`, `bReturnFlag`, `pPad_25`, `nOpcodeCount`, `pDispatchTable[45]`, `pPad_e0_42b`, `framePtr`). `CDSScript::ctor@0x00438390` and `CDSScript::CallExport@0x00438c40` decompile with typed fields; `InstallOpcodeTable` renamed **`CDSScript_InstallOpcodeTable@0x00438310`**.

**Agent todo 04 (2026-05-30):** Removed nested **1 B** `/CDSScript/CDSScript` placeholder (and `CDSScript *` under that category) via `McpInlineDeleteCdsscriptNested.java`; recreated canonical **1072 B** `/CDSScript` when needed. Applied `CDSScript *` prototypes on ctor/dtor/`InstallOpcodeTable`; `save_program bulanci.exe`.

**RE round-2 todo 04 (2026-05-30):** VM header `+0x08..+0x27` verified in Ghidra (`dwPc` … `nOpcodeCount`, `pad_after_bReturnFlag`); `CDSScript_ReadU8@0x00438380` prototype → typed `dwPc`/`bytecode` decompile; decompiler + plate comments on ctor/ReadU8/Run; `save_program`.

| Function | Address | Prototype / decompile |
|----------|---------|------------------------|
| `CDSScript::ctor` | `0x00438390` | `CDSScript * __fastcall CDSScript_ctor(CDSScript *this)` — typed fields |
| `CDSScript_Run` | `0x00438b30` | `CDSScript *this` in prototype; decompiler ECX may stay `void *` (__thiscall API limit) |
| `CDSScript_dtor` | `0x00438400` | `void __fastcall CDSScript_dtor(CDSScript *this)` — `exports`/`scriptData`/`bytecode` by name |
| `CDSScript_InstallOpcodeTable` | `0x00438310` | `CDSScript *this`; `pDispatchTable`/`nOpcodeCount` in decompile |
| `CDSScript::CallExport` | `0x00438c40` | `this->nExports`, `this->exports` |

Round 3 task 42: renamed vtable fields in Ghidra (`vf_primary` / `vf_IDSChained` → names above). Note: Ghidra type at `+0x24..+0x40` may still not match asm field order — see task 42 report.

## Follow-up

- **Round 3 task 42**: closed “five vtable pointers in `+0x00..+0x28`” UNK — only two vptrs in prefix; four/five vtables on full subclass object (see table above).
- **`CDSScript_Run` decompiler:** `__thiscall` ECX `this` may remain `void *` despite `CDSScript *` prototype (document in plate comment if needed).

## UNK

- **+0x0c `dwReserved_0c`, +0x14 `dwReserved_14`**: ctor-zero only; decompile/xref pass found no consumers on `CDSScript*` (note: pack enumerator compares `+0xc` on **resource descriptors**, not script instances).
- **+0x18 `scriptData`**: freed in `CDSScript_dtor@0x00438400`; no typed store besides ctor `=0` in `bulanci.exe` (resource bind may write via raw offsets — not proven).
- **+0xe0..+0x42b (844 bytes)**: no field-level xrefs in base-class methods; `CLevelScript`/`CHistoryScript` extend opcode table into part of this span via second `InstallOpcodeTable` splice at `+0x2c+0x2d*4`.
