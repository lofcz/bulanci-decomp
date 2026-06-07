# Round 10 deep — Task 05 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **title** | R6 rerun: sim slice 0x0042d440–0x0042e140 |
| **kind** | logic_rerun |
| **prior** | R6 task 19 (`round6_logic_task_19_report.md`) |
| **seed** | `0x0042d440` |
| **acceptance** | Deep xref on view/input helpers; correct wrong calling conventions in Ghidra |

Slice is the **CDSString COW + stream wire + path/temp + codepage bridge** band (22 functions). Not CDSView input dispatch — those live upstream (`0x0042c880`–`0x0042d160` per R6 task 18).

## Status

**PARTIAL** — Live Ghidra MCP connected; IDA signatures correlated; R6 renames confirmed applied; `CDSString` struct created; several prototypes and plate comments updated; `save_program` executed. **`set_function_this_type` HTTP endpoint returns 404** on this plugin build; Ghidra API also blocks retyping `__thiscall` ECX `this` — decompiler still shows `CBulanci*` / `CStartGame2*` on handle-slot functions despite correct disasm.

## Functions / Struct

### CDSString handle layout (proven R3–R6, unchanged)

Refcounted wide-string handles: bare `wchar_t*` with 12-byte header at `ptr-0x0c`:

| Offset from header | Field |
|--------------------|-------|
| `+0x00` | length (wchar count, no NUL) |
| `+0x04` | capacity |
| `+0x08` | refcount (1 = unique, >1 = COW) |
| `+0x0c` | `wchar_t buf[capacity]` |

IDA types handle slots as `void **` / `wchar_t **` (`__thiscall` ECX = address of slot).

### Function table (22 @ slice)

| Address | Ghidra name | IDA signature (ground truth) | Role | Evidence |
|---------|-------------|------------------------------|------|----------|
| `0x0042d440` | `CDsString_WriteWStringToStream` | `int __thiscall sub_42D440(_DWORD *this, int stream)` | 4-byte wchar count + `length×2` payload via stream vfn `+0x14` | Live decompile; symmetric to Read; callers `CScoreItem` serialize @ `0x00408f90`, `CDSResourceSign_WriteToStream` |
| `0x0042d490` | `CDsStringAssignFromHandle` | `void **__thiscall sub_42D490(void **this, const void **a2)` | COW assign from another handle | Prior layout comment; central string op |
| `0x0042d510` | `CDsStringAssignFromLiteral` | `void **__thiscall sub_42D510(void **this, WCHAR *Src)` | Literal assign / empty fallback | Live decompile; plate comment added; **decomp still `CBulanci*`** |
| `0x0042d5a0` | `CDsStringInsertWide` | `void **__thiscall sub_42D5A0(...)` | Splice insert/delete | `CEdit_OnKeyDown.md` |
| `0x0042d710` | `CDsStringVsprintfToStackBuffer` | `void **__thiscall sub_42D710(...)` | `__vswprintf` 16 KiW stack → literal assign | R6 decompile |
| `0x0042d770` | `CDsStringFormatV` | `void **sub_42D770(...)` | Vararg wrapper | R6 decompile |
| `0x0042d7a0` | `CDsString_CompareHandles` | `int __thiscall sub_42D7A0(const unsigned __int16 **this, const unsigned __int16 **a2)` | Thin wrapper → `sub_42D3F0(this, *a2)` | **Disasm proof:** `MOV EAX,[ESP+4]; MOV EDX,[EAX]; MOV [ESP+4],EDX; JMP 0x0042d3f0`. Live xrefs (10): `CLevelNameList_FindIndex`, `CScoreItem_MatchesKillsDeathsAndName`, `CEdit_SubmitText`, `CStartGame2_*` name compares |
| `0x0042d7b0` | `CDsString_FindLastWideChar` | `int __thiscall sub_42D7B0(int *this, __int16, int)` | Reverse wchar scan | Path splitters |
| `0x0042d7f0` | `CDsString_SubstringWide` | `_DWORD *__thiscall sub_42D7F0(...)` | Substring into new handle | Path helpers |
| `0x0042d850` | `CDsString_EnsureUnique` | `void __thiscall sub_42D850(void **this)` | COW uniquify | `__fastcall` on slot in some exports |
| `0x0042d8a0` | `CDsStringAssignFromLong` | `void **__thiscall sub_42D8A0(void **this, int)` | `FormatV` with `L"%ld"` | `app_shell.md` @ `0x00487094` |
| `0x0042d8c0` | `CBulanci_CreateCDSFileStream` | `int __thiscall sub_42D8C0(const void **this, DWORD)` | `OperatorNew(0x20)` + `CDSFileStream` vtables; path from **this** slot | R5 worker 17 vtable proof; callers `CGaming_LoadLevelAssetAndMusic` |
| `0x0042d970` | `CBulanci_AssignTempPathWithTrailingBackslash` | `void **__cdecl sub_42D970(void **a1)` | `GetTempPathW` → literal assign → trailing `\` → existence check | **Live xrefs:** `CBulanci_dtor@0x00402c96`, `CGaming_LoadLevelAssetAndMusic@0x0041d357`. Prototype fixed to `__cdecl` |
| `0x0042dab0` | `CDsString_AssignFromMultiByte` | `void __thiscall sub_42DAB0(void **this, LPCSTR, UINT)` | `MultiByteToWideChar` into handle | Caller `CDsString_ReadNarrowLengthPrefixedFromStream@0x0042e2f0` (live callee xref) |
| `0x0042db60` | `CDsString_WideToMultiByteBuffer` | `void __thiscall sub_42DB60(void **this, LPCWSTR, UINT)` | `WideCharToMultiByte` cap 0x4000 | Callers `CDsString_WriteNarrowLengthPrefixedToStream`, `CDsString_WriteTwoWideStringsToStream` (live) |
| `0x0042dc30` | `CBulanci_WideStringToLowerInPlace` | `void **__thiscall sub_42DC30(void **this)` | `EnsureUnique` + `__wcslwr` | Export |
| `0x0042dc50` | `CDsString_ClearInPlace` | `void **__thiscall sub_42DC50(void **this)` | Zero length in place | Used by Read @ zero-length wire |
| `0x0042dc70` | `CDsString_SplitPathBackslashDot` | `void __thiscall sub_42DC70(int *this, void **×3)` | Split on last `\` and `.` | Xref `CBulanci_GetPathExtension@0x0040138d` |
| `0x0042dec0` | `CBulanci_SplitPathDirectoryAndBase` | `void __thiscall sub_42DEC0(int *this, void **, void **)` | Dir + basename on last `\` | Xrefs `CBulanci_GetPathBasename`, `CBulanci_JoinPathDirectoryAndBase` (live) |
| `0x0042dfc0` | `CBulanci_JoinPathDirectoryAndBase` | `void **__thiscall sub_42DFC0(...)` | dec0 + optional literals + `L"%s%s"` | Caller `CBulanci_FormatPathPair@0x0042e230` |
| `0x0042e0e0` | `CBulanci_GetPathBasename` | `void **__thiscall sub_42E0E0(...)` | Basename-only dec0 wrapper | Export |
| `0x0042e140` | `CDsString_ReadWStringFromStream` | `void **__thiscall sub_42E140(void **this, int stream)` | Read mirror of Write | `CPoem_Deserialize`, `CScoreItem::Deserialize` |

### Live xref highlights (new this pass)

| Target | Callers (sample) |
|--------|------------------|
| `0x0042d7a0` | `CLevelNameList_FindIndex`, `CScoreItem_MatchesKillsDeathsAndName`, `CEdit_SubmitText`, 7× `CStartGame2_*` name/level compares |
| `0x0042d970` | `CBulanci_dtor`, `CGaming_LoadLevelAssetAndMusic` |
| `0x0042dab0` | `CDsString_ReadNarrowLengthPrefixedFromStream` |
| `0x0042db60` | `CDsString_WriteNarrowLengthPrefixedToStream`, `CDsString_WriteTwoWideStringsToStream` |
| `0x0042dec0` | `CBulanci_GetPathBasename`, `CBulanci_JoinPathDirectoryAndBase` |
| `0x0042dc70` | `CBulanci_GetPathExtension` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `create_struct` | `CDSString` `{ wchar_t * handle @0 }` (4 bytes) | **Applied** |
| `set_function_prototype` | `0x0042d440` → `int CDsString_WriteWStringToStream(int *param_1)` `__thiscall` | **Applied** — `this` now `void *` in decomp |
| `set_function_prototype` | `0x0042d510`, `0x0042d7b0`, `0x0042d7f0`, `0x0042d8c0`, `0x0042dc50`, `0x0042dc70`, `0x0042dec0`, `0x0042dfc0` | **Applied** (per IDA) |
| `set_function_prototype` | `0x0042d7a0` → `int CDsString_CompareHandles(void *param_1)` | **Applied** |
| `set_function_prototype` | `0x0042dab0` → `void CDsString_AssignFromMultiByte(char *param_1, UINT param_2)` | **Applied** (`LPCSTR` unresolved in DT mgr) |
| `set_function_prototype` | `0x0042db60` → `void CDsString_WideToMultiByteBuffer(wchar_t *param_1, UINT param_2)` | **Applied** |
| `set_function_prototype` | `0x0042d970` → `void * CBulanci_AssignTempPathWithTrailingBackslash(void)` `__cdecl` | **Applied** — was wrongly `CBulanci *__cdecl` |
| `set_function_this_type` | 11 handle-slot functions → `CDSString *` | **Failed** — HTTP 404 (`/set_function_this_type` not in plugin) |
| `set_parameter_type` | `this` @ `0x0042d510` → `CDSString *` | **Failed** — Ghidra: cannot retype register `this` |
| `set_plate_comment` | `0x0042d510`, `0x0042d970` | **Applied** — documents real `CDSString*` / stack slot semantics |
| `set_decompiler_comment` | `0x0042d440`, `0x0042e140`, `0x0042d510`, `0x0042d7a0`, `0x0042d970` | **Applied** |
| `force_decompile` | slice endpoints | **Applied** |
| `save_program` | `bulanci.exe` | **Applied** |

R6 renames (`CBulanci_AssignTempPathWithTrailingBackslash`, `CDsString_AssignFromMultiByte`, `CDsString_WideToMultiByteBuffer`, `CDsString_ClearInPlace`, path helpers, `CDsString_CompareHandles`) were **already present** from `_apply_r6_ghidra.py` / pass3.

Apply log: `ghidra_analysis/engine/logic_recovery/r10_task05_apply_log.json`

## Decomp corrections (IDA vs Ghidra)

| Address | IDA | Ghidra before R10 | After R10 / remaining |
|---------|-----|-------------------|------------------------|
| `0x0042d440` | `this` = handle slot; length from `*(handle-0xc)` | `unaff_EDI` for length | `this` → `void *`; **length still `unaff_EDI`** (register recovery) |
| `0x0042d510` | `void **this` | `CBulanci*` → `(this->app).vftable_primary` | Prototype set; **decomp still `CBulanci*`** — API cannot retype ECX |
| `0x0042d7a0` | `return sub_42D3F0(this, *a2)` | `CStartGame2_CompareLevelName` misname | Renamed; disasm = tail-call thunk; decomp garbles callee args |
| `0x0042d970` | `__cdecl`, stack `void **a1` | `CBulanci *__cdecl` | `__cdecl void *`; shows `in_stack_00000004` as `CBulanci*` (slot on stack) |
| `0x0042dab0` | `void **this` | `CBulanci*` + wrong field offsets | Params fixed (`char*`, `UINT`); **body still uses `(this->app).vftable_primary`** |
| `0x0042db60` | `void **this` | Wrong name + `this` type | Signature `void * this`; body OK-ish |
| `0x0042e140` | symmetric read | `unaff_EDI` / `iStack_4` bugs | Comment added; register bugs persist |

**Root cause:** MSVC `__thiscall` passes the handle-slot address in ECX. Ghidra inherited `CBulanci*`/`CStartGame2*` from early call-site typing. Fixing requires `CDSString *` on `this`, which the MCP plugin cannot apply (missing endpoint + Ghidra register-`this` limitation).

## Frida

**none** — Static IDA + live Ghidra disasm/xrefs sufficient for slice logic. Runtime COW refcount confirmation would need `CDsStringReleaseHeader@0x0042d2d0` hooks (out of scope).

## Remaining UNK

| Item | Reason |
|------|--------|
| Decompiler `this` typing on handle-slot `__thiscall` | `set_function_this_type` 404; `set_parameter_type` blocked on ECX |
| `CDsString_WriteWStringToStream` / `ReadWStringFromStream` length locals | Register-recovery `unaff_EDI`; wire format proven by symmetric serialize pairs |
| `CDsString_SplitPathBackslashDot` `param_1` byte flag | IDA `sub_42DC70(int *this, void**, void**, void**)` — Ghidra still shows extra `CBulanci *param_1`; non-zero flag call sites not exhaustively scanned |
| `CBulanci_CreateCDSFileStream` per-caller `openFlags` | Callers named; per-invocation DWORD values not disassembled this pass |
| Manual Ghidra UI step | Retype `this` → `CDSString *` in decompiler for 11 functions (documented in plate comments) |

## Cross-links

- [`round6_logic_task_19_report.md`](../logic_recovery/round6_logic_task_19_report.md) — prior PARTIAL baseline
- [`round6_logic_task_20_report.md`](../logic_recovery/round6_logic_task_20_report.md) — downstream registry/path helpers
- [`CEdit_OnKeyDown.md`](../CEdit_OnKeyDown.md) — `CDsStringInsertWide` edit path
- [`app_shell.md`](../app_shell.md) — `L"%ld"` format literal
