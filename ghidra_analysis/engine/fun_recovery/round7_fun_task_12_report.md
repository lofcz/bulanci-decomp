# Round 7 — FUN task 12 report

## Task

| Field | Value |
|-------|-------|
| **id** | 12 |
| **title** | FUN recovery: FUN_004298B0 @ 0x004298b0 (xrefs=1) |
| **band** | sim |
| **seed_address** | `0x004298b0` |

## Status

**DONE** — Renamed with disasm + xref + DDraw vtable proof. Prior R6 slice mislabeled this as “CDSAudioPlayer cluster”; pairs with R7 task 04 `CDSBackBuffer_BindImageFromSurface` (`vtable+0x64` **GetSurfaceDesc**) on the same `embeddedImage.pDirectDrawSurface` @ `+0x4c`.

## Function

| Address | Ghidra name (after) | Role summary | Evidence |
|---------|---------------------|--------------|----------|
| `0x004298b0` | `CDSBackBuffer_RestoreSurface` | `__fastcall` on `CDSBackBuffer*` (`ECX`): **`IDirectDrawSurface::Restore`** via `*(this+0x4c)` vtable **+0x6c**; return **`HRESULT >= 0`** as `bool` (`SETGE AL`). | Disasm `0x004298b0`–`0x004298c0`; decompile; 1 xref; [CDSBackBuffer.md](../struct_recovery/CDSBackBuffer.md); sibling `CDSBackBuffer_Flip` uses **+0x80** Flip |

### Xrefs (incoming)

| From | Context |
|------|---------|
| `0x00429be5` | `FUN_00429bd0` — after `Restore` on `CDSApp+0x78`, calls `CDSBackBuffer_RestoreSurface(CDSApp+0x7c)` |

### Caller chain

`FUN_0042a550` (`CDSApp` HRESULT gate after dirty-rect **BitBlt** `vtable+0x14`) handles `HRESULT == 0x887601C2` (`DDERR_SURFACELOST`) / synthetic msg `-0x7789fe3e` → `FUN_00429bd0` → this function → optional `CDSApp` invalidate `vtable+0x24`.

### Disasm highlights

```
004298b0  MOV EAX,[ECX+0x4c]     ; pDirectDrawSurface
004298b5  MOV EDX,[ECX+0x6c]      ; Restore (vtable+0x6c)
004298b9  CALL EDX
004298bd  SETGE AL                 ; success if HRESULT >= 0
```

COM slot proof: `IDirectDrawSurface` method index 23 **Restore** @ byte offset **0x6c** (after `IUnknown`; **Blt** @ **+0x14**, **Flip** @ **+0x80** on same surface — matches `CDSBackBuffer_Flip`).

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `rename_function_by_address` | `FUN_004298b0` → **`CDSBackBuffer_RestoreSurface`** |
| `set_function_prototype` | `bool CDSBackBuffer_RestoreSurface(void)` **`__fastcall`** |
| `set_decompiler_comment` | Restore + caller `FUN_00429bd0` |
| `force_decompile` | Refreshed body (`in_ECX`, `return -1 < iVar1`) |
| `set_function_this_type` | **Skipped** — Ghidra rejects `this_type` on `__fastcall` (same as `CDSBackBuffer_Flip` / task 04) |
| `save_program` | `bulanci.exe` |

## Frida

**none** — static COM offset + embed layout sufficient.

## Remaining UNK

| Item | Note |
|------|------|
| `FUN_00429bd0` @ `0x00429bd0` | Restores `CDSApp+0x78` then back-buffer — R7 task **05** |
| `FUN_0042a550` | Surface-lost HRESULT gate — R7 task **06** |
| Export tree | `_Globals.h` / `mapping.csv` still stub `FUN_004298b0` — sync in separate export pass |

## Evidence paths

- [ROUND7_FUN_PROTOCOL.md](../ROUND7_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [round6_logic_task_12_report.md](../logic_recovery/round6_logic_task_12_report.md), [round7_fun_task_04_report.md](./round7_fun_task_04_report.md)
- [CDSBackBuffer.md](../struct_recovery/CDSBackBuffer.md), [round5_worker_06_report.md](../struct_recovery/round5_worker_06_report.md)
- `config/bulanci/mapping.csv` (`0x4298b0`, `__fastcall`, size `0x11`)
