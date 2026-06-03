# Round 6 logic — Task 19 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 19 |
| **title** | Logic sim_429_436: 0x0042d440–0x0042e140 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — Ghidra MCP decompiled 12/22 functions live before `Connection closed`; remaining 10 verified from exported decompile (`bulanci.ghidra.exe.c`, same database) plus prior struct/docs passes (R3–R5). No Frida: CDSString layout, stream wire format, and path helpers are fully visible in static decompile.

## 3. Functions

### CDSString handle layout (shared by slice)

Refcounted wide-string handles: bare `wchar_t*` with 12-byte header at `ptr-0xc`:

| Offset from header | Field |
|--------------------|-------|
| `+0x00` | length (wchar count, no NUL) |
| `+0x04` | capacity |
| `+0x08` | refcount (1 = unique, >1 = COW share) |
| `+0x0c` | `wchar_t buf[capacity]` |

Documented in decompiler comment @ `CDsStringAssignFromHandle@0x0042d490` (prior pass).

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0042d440` | `CDsString_WriteWStringToStream` | **Stream write:** read length from `*(handle-0xc)` (0 if null handle); `IDSStream` vfn `+0x14` writes 4-byte length, then `length×2` wchar payload if non-zero | MCP decompile; symmetric to Read @ `0x0042e140`; callers `CScoreItem::Serialize@0x00408f90`, `CDSResourceSign_WriteToStream`, `CPoem_GetText` export path (`round3_task_18`, `round3_task_17`). Decompiler shows `unaff_EDI` instead of length — register-recovery artifact; logic matches Read mirror |
| `0x0042d490` | `CDsStringAssignFromHandle` | **COW assign:** if unique buffer large enough → in-place `memmove` + `CDsStringSetLengthAndTerminate`; else bump src refcount, release old, adopt src handle | MCP decompile + layout comment; central engine string op |
| `0x0042d510` | `CDsStringAssignFromLiteral` | **Literal assign:** scan NUL for length; unique in-place copy or `CDsStringAllocAndCopyWide`; NULL literal → `PTR_DAT_004afce0` empty string | MCP decompile; **wrong `this`:** typed `CBulanci*` → `(this->app).vftable_primary` instead of `*(wchar_t**)this` |
| `0x0042d5a0` | `CDsStringInsertWide` | **Splice edit:** at index `param_2`, delete `param_3` wchars (negative = to end), insert `param_5` from `param_4`; COW reallocate or in-place `memmove`/`wcsncpy` | MCP decompile; `CEdit_OnKeyDown.md` Back/Delete: `insertLen=1, src=NULL` removes one code unit |
| `0x0042d710` | `CDsStringVsprintfToStackBuffer` | `__vswprintf` into 16 KiW stack buffer (`local_8004[16384]`), then `CDsStringAssignFromLiteral` into dest slot | MCP decompile |
| `0x0042d770` | `CDsStringFormatV` | Vararg wrapper: `local_4 = &stack0x0000000c` → `CDsStringVsprintfToStackBuffer` | MCP decompile + comment; max ~16K wchars before stack overflow |
| `0x0042d7a0` | `CStartGame2_CompareLevelName` | **Misnamed:** `CBulanci_CompareWideString(this, *param_1)` — lexicographic wide strcmp on two **string handle slots**, not level-name-specific logic | MCP decompile; `round4_task_18`: `CScoreItem_MatchesKillsDeathsAndName@0x00409080` calls with `LEA ECX,[EAX+0x10]` (`m_name`); `round5_worker_12`: `CLevelNameList_FindIndex` linear scan comparator |
| `0x0042d7b0` | `CDsString_FindLastWideChar` | Reverse scan from `param_2` (or last index if -1) for `param_1` wchar; returns index or -1 | MCP decompile; used by path splitters @ `0x0042dc70`, `0x0042dec0`; **wrong `this`** (same CBulanci artifact) |
| `0x0042d7f0` | `CDsString_SubstringWide` | Copy `[param_2 .. param_2+param_3)` wchars into new handle via `CDsStringAllocAndCopyWide`; `param_3<0` → to end; empty → `*param_1=0` | MCP decompile; **wrong `this`** |
| `0x0042d850` | `CDsString_EnsureUnique` | If refcount≠1: alloc copy at full capacity, copy length, release old, adopt unique buffer | MCP decompile; `__fastcall` on `int*` handle slot |
| `0x0042d8a0` | `CDsStringAssignFromLong` | `CDsStringFormatV(this, g_pCDSApp_vftable+1)` where vtable tail is `L"%ld"` @ `0x00487094` (`app_shell.md`) | Export @ `104455`; score row numeric labels @ `0x004096d0` cluster |
| `0x0042d8c0` | `CBulanci_CreateCDSFileStream` | `OperatorNew(0x20)` → install `CDSFileStream` vtables @ `0x0047f780` (+ MI @ `+4/+c/+14`); copy path from **`this` string slot**; `CDSFileStream_Open(stream, NULL, openFlags)` | Export @ `104464`; R5 worker 17 disasm vtable stores; callers `CGaming_LoadLevelAssetAndMusic`, `CGaming_LoadBackgroundMusic` (`round5_worker_17`, task 08). **Wrong `this`:** ECX is path handle address, not `CBulanci*` |
| `0x0042d970` | `FUN_0042d970` | **`GetTempPathW` → CDS string:** `GetTempPathW(0x4000, stack)` → `CDsStringAssignFromLiteral`; append `\` if missing; `CBulanci_NormalizePathForExistenceCheck` or throw Win32 error 10 | Export @ `104516`; callers `CBulanci_dtor` tmp cleanup (`FUN_0042d970()` then `FormatV(L"%s*.tmp")` @ `60987`) and level-load temp `.tmp` path (`87565`). **Wrong signature:** `__cdecl CBulanci*` but body uses ECX-as-string-slot pattern like AssignFromLiteral |
| `0x0042dab0` | `FUN_0042dab0` | **MBCS → wide assign:** `lstrlenA`; `CDsString_EnsureCapacityWide`; `MultiByteToWideChar(codepage,…)` with `ERROR_INSUFFICIENT_BUFFER` regrow; NULL → clear handle | Export @ `104593`; caller `FUN_0042e2f0` registry gzip narrow-string read (task 20). **Wrong `this`** |
| `0x0042db60` | `FUN_0042db60` | **Wide → MBCS stack buffer:** `CDsString_EnsureCapacityAnsi` cap **0x4000**; `WideCharToMultiByte`; writes into `*(void**)this` narrow buffer | Export @ `104634`; callers `FUN_0042e400` SaveConfig trailing string, `FUN_0042e4e0` dual-string log write. **Manifest name `CDsString_AssignFromWideCapped4000` is wrong** — function converts wide→ACP bytes, not “assign from wide capped” |
| `0x0042dc30` | `CBulanci_WideStringToLowerInPlace` | If handle non-null: `CDsString_EnsureUnique` then `__wcslwr` on buffer | Export @ `104676`; caller path-normalization @ `60145` |
| `0x0042dc50` | `FUN_0042dc50` | **Clear string in place:** `CDsString_EnsureUnique`; `CDsStringSetLengthAndTerminate(..., 0)` | Export @ `104688`; callers `CDsString_ReadWStringFromStream` (zero-length wire), `CDsString_SplitPathBackslashDot`, `FUN_0042dec0` |
| `0x0042dc70` | `CDsString_SplitPathBackslashDot` | Split on last `\` and last `.`: up to four output slots (`param_2` dir, `param_3` stem, `param_4` ext); uses `FindLastWideChar`, `SubstringWide`, `AssignFromHandle`, `FUN_0042dc50` for empty outs | Export @ `104703`; SEH frame; **wrong `this`** on string-input parameter |
| `0x0042dec0` | `FUN_0042dec0` | **Dir + basename split:** last `\` via `FindLastWideChar`; `param_1`←dir prefix, `param_2`←remainder; no-backslash → clear dir, copy whole path to basename | Export @ `104816`; feeds `FUN_0042dfc0`, `CBulanci_GetPathBasename` |
| `0x0042dfc0` | `FUN_0042dfc0` | **Path join:** `FUN_0042dec0` on `this`; optional literal overrides on dir/base temps; ensure dir ends with `\`; `CDsStringFormatV(this, L"%s%s" @ 0x0048709c)` | Export @ `104871`; xref `0x48709c`; sole direct caller `CBulanci_FormatPathPair@0x0042e230` (task 20) |
| `0x0042e0e0` | `CBulanci_GetPathBasename` | `*param_1=0`; `FUN_0042dec0(this, param_1, NULL)` — basename half only | Export @ `104921`; caller `CBulanci_GetPathSuffixCompareIndex` path @ `111051` |
| `0x0042e140` | `CDsString_ReadWStringFromStream` | **Stream read:** vfn `+0x10` read 4-byte length; if 0 → `FUN_0042dc50`; else alloc or reuse unique buffer, read `length×2`, assign | Export @ `104940`; `round3_task_17`: `CPoem_Deserialize` MI thunk `ADD ECX,4` → JMP here stores @ poem `+0x18`; `CScoreItem::Deserialize` @ `+0xc` |

### Slice themes

1. **CDSString COW core (`0x0042d490`–`0x0042d850`)** — assign, literal, insert, format, uniquify; underpins all UI/config text.
2. **Length-prefixed stream wire (`0x0042d440`, `0x0042e140`)** — 4-byte wchar count + payload; used by score rows, poems, resource signatures, registry gzip records.
3. **Path / temp-file helpers (`0x0042d970`–`0x0042e0e0`)** — temp-dir acquisition, `\`/`.` splitting, `%s%s` join; pairs with task-20 `CBulanci_FormatPathPair`.
4. **Codepage bridges (`0x0042dab0`, `0x0042db60`)** — registry and log paths store narrow length-prefixed strings; load uses `MultiByteToWideChar`, save uses `WideCharToMultiByte` (4000-byte cap).

### Manifest corrections

| Task JSON `function_names` | Actual @ address | Notes |
|-----------------------------|------------------|-------|
| `CDsString_AssignFromWideCapped4000` @ `0x0042db60` | `FUN_0042db60` (wide→ACP convert) | Name describes a different helper; cap 0x4000 is on **ansi** output buffer |
| `CStartGame2_CompareLevelName` @ `0x0042d7a0` | Generic `CDsString` handle strcmp | Historical name from level-list call sites; not `CStartGame2`-specific |

## 4. Ghidra deltas

**None applied** — MCP `Not connected` after initial decompile pass; no `save_program`.

**Queued (evidence-backed, not executed):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `set_function_this_type` | `CDsStringAssignFromLiteral`, `CDsString_FindLastWideChar`, `CDsString_SubstringWide`, `CDsString_SplitPathBackslashDot`, `FUN_0042dab0` | Disasm: ECX = address of `wchar_t*` handle slot, not `CBulanci*` |
| `set_function_this_type` | `CBulanci_CreateCDSFileStream` | Body: `CDsStringAssignFromHandle(&stack, (undefined4*)this)` — `this` is path handle |
| `set_function_this_type` | `FUN_0042d970` | Same string-slot pattern; `GetTempPathW` fills caller-provided handle |
| `set_function_this_type` | `CStartGame2_CompareLevelName` | `CBulanci_CompareWideString` on two handle slots — use `void*` or dedicated typedef |
| `rename_function_by_address` | `FUN_0042d970` → `CBulanci_AssignTempPathWithTrailingBackslash` | `GetTempPathW` + trailing `\` + existence check; dtor + level-load xrefs |
| `rename_function_by_address` | `FUN_0042dab0` → `CDsString_AssignFromMultiByte` | `MultiByteToWideChar` into handle |
| `rename_function_by_address` | `FUN_0042db60` → `CDsString_WideToMultiByteBuffer` | `WideCharToMultiByte` with 0x4000 cap; pair with `FUN_004045f0` (0x80 cap) |
| `rename_function_by_address` | `FUN_0042dc50` → `CDsString_ClearInPlace` | EnsureUnique + zero length |
| `rename_function_by_address` | `FUN_0042dec0` → `CBulanci_SplitPathDirectoryAndBase` | Last-backslash split |
| `rename_function_by_address` | `FUN_0042dfc0` → `CBulanci_JoinPathDirectoryAndBase` | dec0 + optional literals + `L"%s%s"` |
| `rename_function_by_address` | `CStartGame2_CompareLevelName` → `CDsString_CompareHandles` | Generic wide-string compare on `this` vs `*param_1` |
| `set_decompiler_comment` | `CDsString_WriteWStringToStream`, `CDsString_ReadWStringFromStream` | Note `unaff_EDI` / `iStack_4` register-recovery bugs |
| `save_program` | `bulanci.exe` | Once after above |

Prior passes already applied layout comment @ `0x0042d490` and `CBulanci_CreateCDSFileStream` vtable comment @ `0x0042d903` (R5 worker 17).

## 5. Frida

**none** — Static decompile + stream layout docs (`round3_task_17/18`, `CEdit_OnKeyDown.md`) sufficient. Runtime confirmation of COW refcounting would require hooking `CDsStringReleaseHeader@0x0042d2d0` (out of slice) — not needed for logic recovery.

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| Live `get_xrefs_to` for `FUN_0042d970` / `FUN_0042dab0` | MCP disconnect; xrefs from export + task 08/20 docs only |
| `CDsString_WriteWStringToStream` / `ReadWStringFromStream` fresh disasm | Decompiler register bugs; wire format proven by symmetric Serialize/Deserialize pairs |
| Exact `CBulanci_CreateCDSFileStream` open-mode `param_1` values at gaming call sites | Callers named; per-call flags not disassembled this pass |
| Whether `CDsString_SplitPathBackslashDot` `param_2` byte flag is ever non-zero | Signature includes leading `uchar param_2`; export shows `SplitPathBackslashDot(this,0,0,param_1)` at `59377` — other call sites not scanned |

## Cross-links

- [`CEdit_OnKeyDown.md`](../CEdit_OnKeyDown.md) — `CDsStringInsertWide` delete-one-wchar edit path
- [`round3_task_17_report.md`](../struct_recovery/round3_task_17_report.md) — `ReadWStringFromStream` MI adjust @ poem `+0x18`
- [`round3_task_18_report.md`](../struct_recovery/round3_task_18_report.md) — `WriteWStringToStream` @ `CScoreItem+0xc`
- [`round4_task_18_report.md`](../struct_recovery/round4_task_18_report.md) — `CompareLevelName` rename hygiene
- [`round5_worker_17_report.md`](../struct_recovery/round5_worker_17_report.md) — `CreateCDSFileStream` vtable proof
- [`app_shell.md`](../app_shell.md) — `L"%ld"` @ `0x00487094` used by `CDsStringAssignFromLong`
- [`round6_logic_task_20_report.md`](./round6_logic_task_20_report.md) — downstream path join + registry string helpers using `FUN_0042dab0` / `FUN_0042db60`
