# Round 3 — task 00 report (`CDSJpegImage`)

## Task

| Field | Value |
|-------|--------|
| **id** | 0 |
| **title** | CDSJpegImage |
| **one_liner** | `CDSJpegImage`: registry factory `0x0043c160` not disassembled; dual meaning of `+0x60` (`0x4b` class id vs JPEG quality 75 in Save) |
| **acceptance** | Prove or document UNK in CDSJpegImage.md with func@addr evidence |

## Status

**DONE**

## Evidence

| Claim | func@addr | Proof |
|-------|-----------|-------|
| `0x0043c160` is **not** a JPEG wrapper factory | `CDSQueueStream_CreateObject@0x0043c160` | `PUSH 0x24`; `OperatorNewWithBadAlloc`; vtables `0x486dec` / `0x486dac` / `0x486d94`; `puVar1[2]=0x20` — matches `CDSQueueStream` ctor pattern |
| JPEG wrapper factory | `CDSJpegImage_CreateObject@0x00432070` | `PUSH 0x64` (100); `CDSJpegImage_InitVtables` |
| Class id **`0x4b`** → queue factory | static init bytes @ `0x0047daa0` | `PUSH 0x0043c160`; `PUSH 0x4b`; `CALL HandleClassRegister` |
| Class id **`0x15`** → JPEG factory | static init bytes @ `0x0047cbf0` | `PUSH 0x00432070`; `PUSH 0x15`; `CALL HandleClassRegister` |
| Init stores **75** at `+0x60` | `CDSJpegImage_InitVtables@0x00431d48` | `MOV dword ptr [EAX+0x60], 0x4b` |
| Save reads quality from IDSImage face | `CDSJpegImage_Save@0x00432030` | `LEA EAX,[ECX-0x54]` null test; `MOV ECX,[ECX+0xc]` → third arg to `CompressFromImage` (`ECX` = IDSImage @ object `+0x54` ⇒ object `+0x60`) |
| Quality drives libjpeg | `CompressFromImage@0x00431e50` | `jpeg_set_quality(local_170, param_3, 1)`; format still from `*(param_2+0xc)` |
| `DecompressToImage` `[EBP+0x60]` | `CDSJpegImage::DecompressToImage@0x00431c18` | Stack frame inside `jpeg_decompress_struct` — **not** object field `+0x60` |

## Ghidra deltas

- `create_function` @ `0x0043c160` (was undefined)
- `rename_function_by_address` → **`CDSQueueStream_CreateObject`**
- Plate comment @ `0x0043c160` (registry class `0x4b` vs JPEG factory `0x00432070`)
- `save_program bulanci.exe` (once)

## Struct doc updates

- `CDSJpegImage.md` — factory table, `+0x60` as `defaultJpegQuality`, cleared resolved UNK; remaining UNK listed

## Remaining UNK

- Post-init writers to `defaultJpegQuality` (none seen in JPEG-specific code this task).
- Full `m_image` interior field naming (`CDSImage.md` scope).
