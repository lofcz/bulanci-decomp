# Round 3 — task 36 report

## Task

| Field | Value |
|-------|-------|
| **id** | 36 |
| **title** | Disasm CDSJpegImage registry factory @ 0x0043c160 |
| **one_liner** | Resolve mistaken identity of `0x0043c160` vs JPEG heap factory; confirm `+0x60` on 100 B wrapper vs embedded `0x60` image allocs |
| **acceptance** | Update CDSJpegImage.md; Ghidra mutation if evidence supports |

## Status

**DONE** — `0x0043c160` is **`CDSQueueStream_CreateObject`** (`OperatorNew(0x24)`); JPEG wrapper factory is **`CDSJpegImage_CreateObject@0x00432070`** (`PUSH 0x64`). Wrapper tail `+0x60` holds write-once **`0x4b`** (default IJG quality); `Save` reads it via IDSChained MI (`MOV ECX,[ECX+0xc]`). Not the engine class id **`0x15`** for JPEG or registry id **`0x4b`** for queue streams.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| `0x0043c160` ≠ JPEG | `CDSQueueStream_CreateObject` @ `0x0043c160` | `PUSH 0x24` → `OperatorNewWithBadAlloc`; vtables `0x486dec` / `0x486dac` / `0x486d94`; `dwStreamState=0x20`, `nRefcount=1` |
| Queue stream registered with class **`0x4b`** | `CDSQueueStream_StaticClassRegister` @ `0x0047daa0` | `PUSH 0x43c160`, `PUSH 0x4b7bfc`, `PUSH 0x4b`, `MOV ECX,0x4b847c`, `CALL HandleClassRegister` |
| JPEG wrapper factory | `CDSJpegImage_CreateObject` @ `0x00432070` | `PUSH 0x64` → `OperatorNewWithBadAlloc(100)` → `JMP CDSJpegImage_InitVtables` |
| JPEG registered with class **`0x15`** | `CDSJpegImage_StaticClassRegister` @ `0x0047cbf0` | `PUSH 0x432070`, `PUSH 0x4b7bfc`, `PUSH 0x15`, `MOV ECX,0x4b7d80`, `CALL HandleClassRegister` |
| Sole store to wrapper `+0x60` | `CDSJpegImage_InitVtables` @ `0x00431d48` | `MOV dword ptr [EAX+0x60],0x4b`; byte pattern `c7 40 60 4b 00 00 00` → one hit |
| `Save` quality via MI | `CDSJpegImage_Save` @ `0x00432030` | `LEA EAX,[ECX-0x54]`; normal path `MOV ECX,[ECX+0xc]` → `PUSH ECX` → `CompressFromImage` (quality arg); `ECX` on IDSChained face (`+0x54`) → object `+0x60` |
| `Load` does not touch `+0x60` | `CDSJpegImage_Load` @ `0x00431cc0` | `LEA EAX,[ECX-0x50]` → embedded `CDSImage` @ `+0x04`; calls `CDSJpegImage__DecompressToImage` only |
| Embedded decode uses **`0x60`** image | (xref) `CDSDsmFile::InitializeChildObject`, `CDSFlxFile::CreateBoundClone` | `OperatorNew(0x60)` + `CDSObject_CtorWithImage` — standalone bitmap, not 100 B wrapper |
| Ghidra struct size | `get_struct_layout` `CDSJpegImage` | 100 B: `pVf_IDSReferenced` @ 0 + `m_image` `CDSImage` @ +4; tail dword aliases `m_image.pM_chain` @ object `+0x60` |

### Factory disasm (summary)

```
0043c160  PUSH 0x24          ; CDSQueueStream (36 B) — NOT JPEG
00432070  PUSH 0x64          ; CDSJpegImage wrapper (100 B)
0043207e  MOV ECX,EAX
00432080  JMP  0x00431cf0     ; CDSJpegImage_InitVtables
00431d48  MOV [EAX+0x60],0x4b ; default quality on wrapper tail only
```

## Ghidra deltas

- Functions/comments already present from prior batches: `CDSQueueStream_CreateObject`, `CDSJpegImage_CreateObject`, plate/EOL comments @ factories and `StaticClassRegister` sites, decompiler comment on `CDSJpegImage_Save` (MI quality read).
- Verified disasm/comments via MCP (no rename/layout changes required this pass).
- `save_program bulanci.exe` — saved successfully.

## Struct doc updates

- [CDSJpegImage.md](./CDSJpegImage.md) — factory table and `+0x60` rows unchanged (already match this disasm); cross-ref this report in **Agent todo 36 r3**.

## Remaining UNK

- None for this task scope (pack overwrite of `+0x60` closed in r2 audit).
