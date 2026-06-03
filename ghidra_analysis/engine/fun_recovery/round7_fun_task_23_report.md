# Round 7 FUN — Task 23 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 23 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x0042e4e0` |
| **title** | FUN recovery: FUN_0042E4E0 @ 0x0042e4e0 (xrefs=1) |
| **prior_hint** | R6 task 20: dual wide-string log write; pair with `CDsString_WriteNarrowLengthPrefixedToStream@0x0042e400` |

## Status

**DONE** — Disasm, decompile, and sole xref prove a dual wide-string → ACP → `IDSStream` write helper. Renamed to `CDsString_WriteTwoWideStringsToStream` (mirrors `CDsString_WriteNarrowLengthPrefixedToStream` naming). Prototype fixed to `__thiscall` with `pStream` + `pWide2`; `set_function_this_type` skipped (`void *` rejected).

## Function

| Address | Ghidra name (before → after) | Role | Evidence |
|---------|------------------------------|------|----------|
| `0x0042e4e0` | `FUN_0042e4e0` → **`CDsString_WriteTwoWideStringsToStream`** | **`__thiscall`:** `ECX` = pointer to a **CDSString handle slot** (first wide string); stack = `IDSStream *` + second `wchar_t *`. Converts each wide string to thread ACP bytes (`CDsString_WideToMultiByteBuffer` cap **0x4000**, then `CDsString_AssignFromWideCapped80` cap **0x80**), then invokes stream vtable **`+0x14`** (`Write`) with narrow buffer + byte length (`*(handle-0xc)` for slot, `wcslen*2` for raw pointer). Null handles use empty literal `PTR_DAT_004afce0`. | **Xref:** `UNCONDITIONAL_CALL` from `FUN_0042a9c0@0x0042a9f2`. **Caller disasm:** `PUSH [ESP+0x18]` (3rd arg = `pWide2`), `PUSH ECX` (stream), `LEA ECX,[ESP+0x1c]` (string slot), `CALL 0x0042e4e0`; callee `RET 0x8`. **Callee disasm:** `MOV ESI,ECX`; `MOV EBX,[ESP+0x40a8]` stream; `MOV EDX,[ESP+0x40ac]` second wide; two `CALL [EDX+0x14]` via `EBX`. **Consumer:** `Catch_0042ab28` opens `CDSFileStream`, then `FUN_0042a9c0(stream, …, PTR_DAT_004afca4)` for exception log payload. |

### Call-site proof (`FUN_0042a9c0@0x0042a9e0`)

```
0042a9e0: MOV EAX,[ESP+0x18]     ; pWide2 from caller (Catch_0042ab28 3rd param)
0042a9e4: PUSH EAX
0042a9e5: PUSH ECX               ; stream (this of FUN_0042a9c0)
0042a9e6: LEA ECX,[ESP+0x1c]     ; &local CDSString handle (init 0)
0042a9f2: CALL 0x0042e4e0
```

Thin wrapper releases temporary handle after call (`CDsStringReleaseHeader` if non-null).

### Sibling (`0x0042e400`)

`CDsString_WriteNarrowLengthPrefixedToStream` — single CDSString slot, 4-byte length prefix + narrow body (registry gzip config). Same stream vfn `+0x14`, different framing.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0042e4e0` → `CDsString_WriteTwoWideStringsToStream` | success |
| `set_function_prototype` | `void CDsString_WriteTwoWideStringsToStream(int * pStream, wchar_t * pWide2)` `__thiscall` | success |
| `set_function_this_type` | `void *` | failed — Ghidra requires struct/class for `this` |
| `set_decompiler_comment` | `0x0042e4e0` | R7 proof comment |
| `force_decompile` | `0x0042e4e0` | `pStream` typed; second wide still `unaff_retaddr` in body (alloca frame) |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — Static decompile + xref/disasm closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Second parameter in decompiler body | Prototype lists `pWide2`; body still uses `unaff_retaddr` until stack/allocation model improves |
| `FUN_0042a9c0` @ `0x0042a9c0` | Task 16 — thin SEH wrapper; not renamed this worker |
| `PTR_DAT_004afca4` literal text | Data xref only; not required for rename proof |
| `mapping.csv` / `_Globals.cpp` stub | Still `FUN_0042e4e0`; export regen out of scope |

## Cross-links

- [`round6_logic_task_20_report.md`](../logic_recovery/round6_logic_task_20_report.md) — sim band `0x0042e230`–`0x0042ea20` context
- [`round6_logic_task_19_report.md`](../logic_recovery/round6_logic_task_19_report.md) — `CDsString_WideToMultiByteBuffer` / capped-80 helpers
- [`CDSChain.md`](../struct_recovery/CDSChain.md) — registry string write @ `0x0042e400`
