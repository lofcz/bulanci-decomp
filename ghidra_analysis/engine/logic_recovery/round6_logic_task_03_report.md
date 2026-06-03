# Round 6 — Logic Task 03 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **title** | Logic cluster: CWeapon_OnSchedulerEvent + scheduler/gaming neighbors |
| **range** | `cluster_41ab` (`0x0041a`–`0x0041b`) |
| **seed_address** | `0x0041b180` |

## Status

**PARTIAL** — Seed `CWeapon_OnSchedulerEvent@0x0041b180` control flow, vtable placement, MI adjustor, and callee chain are **proven** (Ghidra MCP decompile/disasm + `bulanci.ghidra.exe.c` + `master_vtable_catalog.csv`). Neighbor wav-stream thunks and class-registry factories are documented from prior R4/R5 work and export comments. **Ghidra MCP disconnected** before `set_decompiler_comment` / `save_program`; recommended mutations listed below.

## Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| **`0x0041b180`** | **`CWeapon_OnSchedulerEvent`** | **IDSEventHandler MI thunk** on **`CBulanek+0xA0`** — adjusts `ECX -= 0xA0` to recover `CBulanek*`, then runs weapon-aim movement step | **Disasm:** `ADD ECX,0xffffff60; CALL 0x0041af70; RET 0xC`. **Decompile:** `FUN_0041af70((CBulanek *)(param_1 - 0xa0))`. **Xref:** DATA only — vtable `0x00481ddc` slot **0** (`read_memory`: `80 b1 41 00 …`) |
| `0x0041af70` | `FUN_0041af70` | Shared **`CBulanek` movement/collision step** (weapon `dwParamA/B`, `pTrackHolder` anchor → collision clamp → vtable `+0x20`/`+0x24`) | **Callee of seed** @ `0x0041b186`. Full flow in [round6_logic_task_02_report.md](./round6_logic_task_02_report.md). **Xrefs (3):** `CWeapon_OnSchedulerEvent`, `CBulanek_ApplyPickupEffect@0x0041eb28`, `CWeapon::Update@0x0041bf61` |
| `0x0041bf00` | `CWeapon::Update` | Syncs cached weapon track params on owner; calls movement step when `dwParamA/B` or `pTrackHolder` change and `bField_168 == 0` | **Export @86516:** compares `pWeapon->dwParamA/B` vs `pWalkTrackSources[2/3]` and embed `pTrackHolder`; on change → `FUN_0041af70(this)`; else `vftable_primary+0x24(0,0)` |
| `0x0041a610` | `CDSChain_AdjustThisOffset_ThisMinus30` | `CDSWavStream` / `IDSChained5` MI adjustor `ECX-0x30` → `CDSChain_AdjustThisOffset@0x0042ac90` | R5 w19 + [CDSWavStream.md](../struct_recovery/CDSWavStream.md); vtable `0x482354` slot 1 |
| `0x0041a620` | `CDSWavStream_ScalarDeletingDtor_thunk_Sub34` | MI adjustor `ECX-0x34` → `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | R5 w19; vtable `0x482354` slot 3 |
| `0x0041a630` | `CDSWavStream_ScalarDeletingDtor_thunk_Sub18` | MI adjustor `ECX-0x18` → stream scalar dtor path | R3 todo 50; vtable `0x482388` (`IDSEventHandler`) slot 3 |
| `0x0041a640` | `CDSWavStream_dtor` | Non-deleting stream dtor — `IDSChainedTail_ClearSubObjStash` @ `+0x34`, then `CDSObject` base | R4 task 50; export @84881; only from `CDSWavStream_ScalarDeletingDtor@0x0041bc00` |
| `0x0041a6a0` | `CreateObject` | Class registry factory **0x7ec** — `OperatorNew(0x19c)` → `CGameView::FUN_00418d70` (CBulanek shell ctor without full player init) | Export @84917 + static-init comment @180364 |
| `0x0041a710` | `CreateObject` | Class registry factory **0x7eb** — `ODSImage::ODSImage_FactoryCtor` | Export comment @180386 |
| `0x0041a780` | `CreateObject` | Class registry factory **0x800** — `CBitmap` factory | Export comment @180508 |
| `0x0041a7f0` | `CreateObject` | Class registry factory **0x7ef** — `CAnim::CAnim_SubobjectCtor` | Export comment @180397 |
| `0x0041a860` | `CDeath_SubobjectCtor` | `CAnim` prefix vtable install; zeros corpse placement tail `+0x100/+0x104` | [CDeath.md](../struct_recovery/CDeath.md); export @85047 |

### Seed — vtable and scheduler context

| Item | Value | Proof |
|------|-------|-------|
| Object facet | `CBulanek+0xA0` (`vftable_event`) | `CBulanek.md`; ctor `MOV [ESI+0xa0],0x481ddc` @ `0x0041e55c` |
| Vtable | `g_pCBulanek_vftable_event` @ **`0x00481ddc`** | `master_vtable_catalog.csv`: **IDSEventHandler**, 5 slots |
| Slot 0 | **`0x0041b180`** (`CWeapon_OnSchedulerEvent`) | `read_memory@0x00481ddc` → `0x0041b180` |
| Sibling slots | `0x004178d0`, `CDSView_OnMouseStub`, `0x004178f0`, stub | `vftable_methods.csv` rows 355–358 |
| Callee | `FUN_0041af70` @ `0x0041b186` | Sole logic; always returns via movement step (`AL=1`) |

**Naming note:** Despite the symbol `CWeapon_OnSchedulerEvent`, the entry point is **not** on `CWeapon*` — it is **`CBulanek`'s event-handler facet** at `+0xA0`. Weapon-specific scheduler polling lives in `CBulanek_SchedulerTick@0x0041aed0`, which walks `pWeapon->trackManager.scheduler` and calls `FUN_0041ad80` (see export @85486). The seed thunk is the **event vtable hook** that applies weapon aim deltas through the shared movement step when the handler fires.

### Seed control flow

```
IDSEventHandler dispatch (ECX = CBulanek + 0xA0)
  CWeapon_OnSchedulerEvent@0x0041b180
    ECX -= 0xA0                         ; recover CBulanek*
    CALL FUN_0041af70@0x0041af70       ; weapon-aim movement / collision step
    RET 0xC                             ; 3 stack parameters (handler signature)
```

Parallel path when weapon track params change outside scheduler tick:

```
CWeapon::Update@0x0041bf00 (ECX = CBulanek* per export)
  if pWeapon && (dwParamA/B or pTrackHolder changed)
    cache params in pWalkTrackSources / embed
    if bField_168 == 0 → FUN_0041af70(CBulanek*)
    else → vftable_primary+0x24(0,0)
```

## Ghidra deltas

**None applied** (MCP `Not connected` after initial read batch).

**Recommended when MCP is back:**

| Action | Target | Rationale |
|--------|--------|-----------|
| `set_decompiler_comment` | `0x0041b180` | MI adjustor + vtable `0x481ddc` slot 0 + delegate to `@0x41af70` |
| `set_function_prototype` | `0x0041b180` | `uchar __fastcall CWeapon_OnSchedulerEvent(void *this_at_CBulanek_plus_A0, uint, uint, uint)` — **do not** type ECX as `CWeapon*` (disasm proves `+0xA0` facet) |
| `set_decompiler_comment` | `0x0041bf00` | Weapon param sync → conditional `FUN_0041af70`; xref from seed task |
| `save_program` | `bulanci.exe` | Once per agent |

**Skip (prior rounds):** `0x0041a610`–`0x0041a640` wav MI/dtor — R4 task 50 + R5 worker 19 already typed/plated. Neighbor renames for `FUN_0041af70` deferred to task 02.

## Frida

**none** — MI adjustor (`SUB ECX,0xA0`), vtable bytes, and callee chain are closed statically.

## Remaining UNK

- Exact **runtime dispatcher** that invokes `0x00481ddc` slot 0 (indirect only; no code xref to `0x0041b180` besides vtable data).
- **`CWeapon::Update` `this` typing** — export shows `CBulanek *param_1` while symbol is under `CWeapon`; disasm @ `0x0041bf00` not re-fetched live this session (MCP down). Field accesses match `CBulanek*` (`pWeapon@+0xF8`, embed band `+0x104`..).
- Primary vtable slots **`+0x20` / `+0x24`** on `CBulanek` — indirect only inside `FUN_0041af70`.
- Class-registry **`CreateObject`** factories: class IDs proven from static-init comments; per-factory heap sizes / ctor bodies not re-decompiled live for `0x0041a710` / `0x0041a780`.

## Struct doc updates

**none** — no new proven layout beyond existing [CBulanek.md](../struct_recovery/CBulanek.md), [CWeapon.md](../struct_recovery/CWeapon.md), [CDSWavStream.md](../struct_recovery/CDSWavStream.md), [CDeath.md](../struct_recovery/CDeath.md).
