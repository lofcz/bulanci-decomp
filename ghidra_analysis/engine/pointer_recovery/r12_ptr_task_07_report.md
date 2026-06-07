# Round 12 — Pointer Task 07 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **title** | Fix pointer arithmetic / calling convention in `CWeapon_OnSchedulerEvent` |
| **seed_address** | `0x0041b180` |
| **callee** | `CBulanek_StepMovementAndCollision` @ `0x0041af70` |

## Status

**DONE** — Disasm proves MI adjustor and stack cleanup. Prototype, parameter type/name, and callee call fixed in live Ghidra. Program saved.

## Disassembly proof (`0x0041b180`)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x0041b180` | `ADD ECX,0xFFFFFF60` | MI adjustor: `ECX -= 0xA0` → recover `CBulanek*` from facet @ `CBulanek+0xA0` |
| `0x0041b186` | `CALL 0x0041af70` | Delegate to `CBulanek_StepMovementAndCollision` |
| `0x0041b18b` | `RET 0xC` | Callee pops **12 bytes** = **3** stack parameters |

### Calling convention (proved, not guessed)

| Candidate | Expected `RET` | Verdict |
|-----------|----------------|---------|
| **`__thiscall`** (`this` in **ECX**, 3 stack args) | `RET 0xC` | **MATCH** |
| `__stdcall` (`this` on stack + 3 args = 4 DWORDs) | `RET 0x10` | **REJECTED** |

Machine ABI: **`uchar __thiscall CWeapon_OnSchedulerEvent(IDSEventHandler *this, uint, uint, uint)`**.

Ghidra display ABI: **`uchar __fastcall`** — required for stable MI-thunk decompilation. Setting `__thiscall` + `IDSEventHandler*` or `CBulanek*` via API produces bogus struct offsets (`&this[-1].videoTrackManager…`; see R10 task 02). `__fastcall` keeps ECX as first parameter without injecting a duplicate auto-`this`.

## Vtable / object facet proof

| Item | Value | Proof |
|------|-------|-------|
| Vtable | `g_pCBulanek_vftable_event` @ `0x00481ddc` | `read_memory@0x00481ddc` → `80 b1 41 00` = `0x0041b180` |
| Facet offset | `CBulanek+0xA0` | Ctor `MOV [ESI+0xa0],0x481ddc`; [CBulanek.md](../struct_recovery/CBulanek.md) |
| **Not** `CWeapon*` | Separate weapon vtable @ `0x00481eec` slot 0 = `0x00418f60` | `master_vtable_catalog.csv` |

## Before / after decompilation

### Before (stale)

```c
uchar __fastcall CWeapon_OnSchedulerEvent(int this_cbulanek_plus_A0, ...)
{
  uVar1 = CBulanek_StepMovementAndCollision((void *)(this_cbulanek_plus_A0 + -0xa0));
}
```

Problems: `int` facet pointer, explicit `(void *)` cast, byte-offset arithmetic on integer.

### After (live Ghidra post-fix)

```c
uchar __fastcall
CBulanek::CWeapon_OnSchedulerEvent
          (IDSEventHandler *this_ids_event_facet, uint param_2, uint param_3, uint param_4)
{
  uVar1 = CBulanek_StepMovementAndCollision(this_ids_event_facet + -0x28);
  return uVar1;
}
```

Improvements:

- Parameter typed **`IDSEventHandler *`** and renamed **`this_ids_event_facet`** (MI facet at `CBulanek+0xA0`).
- Callee argument is typed pointer arithmetic: **`this_ids_event_facet + -0x28`** = subtract **40 × sizeof(IDSEventHandler)** = **−0xA0 bytes** → **`CBulanek*`** (matches disasm `ADD ECX,-0xA0` and IDA `sub_41B180(this-40)`).
- No `(void *)` cast on the call.

### Callee (`0x0041af70`)

| Action | Result |
|--------|--------|
| `set_function_prototype` | `uchar __thiscall CBulanek_StepMovementAndCollision(CBulanek *this)` — accepted |
| Decompiler `this` in body | Still `void *` — **Ghidra API limitation** (`ECX:4 (auto)` cannot be retyped via MCP) |
| Direct caller `0x0041bf00` | Passes `CBulanek *param_1` correctly |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `create_struct` | `IDSEventHandler` | 4 B: `{ void * pVftable; }` |
| `set_function_prototype` | `0x0041b180` | `uchar __fastcall CWeapon_OnSchedulerEvent(IDSEventHandler *this_ids_event_facet, uint, uint, uint)` |
| `set_parameter_type` | `0x0041b180` / `this_cbulanek_plus_A0` | → `IDSEventHandler *` |
| `rename_variable` | `0x0041b180` | `this_cbulanek_plus_A0` → `this_ids_event_facet` |
| `set_function_prototype` | `0x0041af70` | `uchar __thiscall CBulanek_StepMovementAndCollision(CBulanek *this)` |
| `set_decompiler_comment` | `0x0041b180` | Machine `__thiscall` + Ghidra `__fastcall` note |
| `force_decompile` | `0x0041b180`, `0x0041af70` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Pointer arithmetic equivalence

```
disasm:  ECX -= 0xA0                    (byte offset)
IDA:     this - 40                      (40 dwords × 4 = 0xA0 bytes)
Ghidra:  this_ids_event_facet + -0x28  (-0x28 dwords × 4 = -0xA0 bytes)
```

All three recover **`CBulanek*`** from the **`IDSEventHandler`** facet pointer at **`CBulanek+0xA0`**.

## Remaining limitations

- Seed Ghidra CC label remains **`__fastcall`** despite machine **`__thiscall`** (documented in plate comment).
- `CBulanek_StepMovementAndCollision` function body still shows `void *this` until manual UI retype of ECX auto-parameter.
- Stack params `param_2..4` unused in thunk; event semantics UNK (no code xrefs besides vtable DATA).
