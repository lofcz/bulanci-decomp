# Round 9 Other FUN — Task A (001) Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **round** | 9 |
| **kind** | `other_fun` |
| **namespace (manifest)** | `CBulanci` |
| **band** | other (app shell / CBulanci) |
| **seed_address** | `0x0043c9b0` |
| **ghidra_name (before)** | `FUN_0043c9b0` |
| **prior_hint** | R5 w14: `CBulanci+0x200` audio teardown; caller `CDSApp_OnDestroy` — **superseded** (wrong sub-object; see proof) |

## Status

**DONE** — Renamed to `CDSDirectSound_StopAllAndReleaseCom`; retyped `__thiscall` / `CDSDirectSound *`; `save_program bulanci.exe` applied.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0043c9b0` | `CDSDirectSound_StopAllAndReleaseCom` | Stop all `CDSAudioPlayer` instances, then COM `Release` (vtable+8) on `m_pPrimaryBuffer` (+0x3c) and `m_pDirectSound` (+0x38) when `m_pDirectSound` is non-null | Disasm, decompile, xref, struct doc |

### Caller proof (`CDSApp_OnDestroy`)

| Site | Evidence |
|------|----------|
| `0x00429bbe` | `LEA ECX,[ESI+0x200]` → `CALL 0x0043c9b0` — `ESI` = app/`CBulanci*`; **+0x200** = `CDSApp::directSound` embed ([CBulanci.md](../struct_recovery/CBulanci.md), [CDSDirectSound.md](../struct_recovery/CDSDirectSound.md)) |
| Xref | Sole **CALL** from `CDSApp_OnDestroy@0x00429bb0` |

Post-rename decompile at call site:

```c
CDSDirectSound::CDSDirectSound_StopAllAndReleaseCom((CDSDirectSound *)(param_1 + 0x200));
```

### `this` / field proof (not CGaming)

| Check | Result |
|-------|--------|
| Prolog | `MOV ESI,ECX` — member `this` in ECX |
| Fields touched | `[this+0x38]`, `[this+0x3c]` only (after global stop) |
| Struct match | `CDSDirectSound`: `m_pDirectSound@+0x38`, `m_pPrimaryBuffer@+0x3c` — same offsets as `CDSDirectSound_InitPrimary@0x0043cbc0` (`CoCreateInstance` / primary buffer create) |
| Callee | `CDSAudioPlayer_StopAll@0x0043a590` — drains active player list before COM release |

### Disasm (Ghidra)

```
0043c9b0: PUSH ESI
0043c9b1: MOV ESI,ECX
0043c9b3: CALL CDSAudioPlayer_StopAll
0043c9b8: CMP dword ptr [ESI+0x38],0x0
0043c9bc: JZ 0x0043c9e6
0043c9be: MOV EAX,[ESI+0x3c]
0043c9c1: TEST EAX,EAX
0043c9c3: JZ 0x0043c9d4
0043c9c5: MOV ECX,[EAX]
0043c9c7: MOV EDX,[ECX+0x8]
0043c9ca: PUSH EAX
0043c9cb: CALL EDX          ; Release primary buffer
0043c9cd: MOV [ESI+0x3c],0
0043c9d4: MOV EAX,[ESI+0x38]
0043c9d7: MOV ECX,[EAX]
0043c9d9: MOV EDX,[ECX+0x8]
0043c9dc: PUSH EAX
0043c9dd: CALL EDX          ; Release IDirectSound
0043c9df: MOV [ESI+0x38],0
0043c9e6: POP ESI
0043c9e7: RET
```

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (post) | `void __thiscall CDSDirectSound_StopAllAndReleaseCom(CDSDirectSound *this)`; body `0x38` B |
| `config/bulanci/mapping.csv` | `CBulanci::FUN_0043c9b0`; size `0x38`; stale `__fastcall` / `uchar` return |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0043c9b0` | `void CDSDirectSound_StopAllAndReleaseCom(void)` **`__thiscall`** |
| `set_function_this_type` | `0x0043c9b0` | `CDSDirectSound *` — moved into class namespace `CDSDirectSound` |
| `rename_function_by_address` | `0x0043c9b0` | `CDSDirectSound_StopAllAndReleaseCom` |
| `set_decompiler_comment` | `0x0043c9b0`, `0x00429bbe` | WM_DESTROY / embed +0x200 notes |
| `force_decompile` | `0x0043c9b0`, `0x00429bb0` | Typed `pM_pDirectSound` / `pM_pPrimaryBuffer` |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static caller disasm, field offsets, and `CDSDirectSound` layout proof sufficient.

## Remaining UNK

- **`mapping.csv` / `CBulanci.h` / `CBulanci.cpp`**: still list `FUN_0043c9b0` under `CBulanci`; export sync out of scope for this task.
- **Worker thread / `~CDSDirectSound`**: dtor @ `0x0043c8b0` does not call this helper; shutdown ordering vs `m_hWorkerEvent` remains documented separately in [CDSDirectSound.md](../struct_recovery/CDSDirectSound.md).
- Prior decompiler note “CGaming sub-object” at +0x200 was **incorrect** — corrected by embed offset + field names.
