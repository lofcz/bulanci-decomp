# Round 6 logic — Task 20 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 20 |
| **title** | Logic sim_429_436: 0x0042e230–0x0042ea20 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — Ghidra MCP decompiled 11/22 functions live (`batch_decompile`); MCP disconnected before second batch, `get_xrefs_to`, `disassemble_function`, and `save_program`. Remaining 11 functions verified from exported decompile (`bulanci.ghidra.exe.c`, same database) plus prior struct/docs passes. No Frida: behavior is stream layout, calendar math, or static-init registry glue with disasm-level proof already in repo.

## 3. Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0042e230` | `CBulanci_FormatPathPair` | SEH-wrapped path join: `CDsStringAssignFromLiteral` on two wide literal args (empty → `PTR_DAT_004afce0`); `FUN_0042dfc0` concat; release temporaries | MCP decompile; xref `CBulanci_DirEnumCtx_FormatCurrentPath@0x004014b0` (`CBulanci.md`) |
| `0x0042e2f0` | `FUN_0042e2f0` | **Registry gzip string read:** `IDSStream` vfn `+0x10` reads 4-byte length; if 0 → `FUN_0042dc50` empty assign; else malloc narrow buffer, read, NUL, `MultiByteToWideChar` via `FUN_0042dab0`, `CDsStringAssignFromLiteral` into **`this` string slot** | MCP decompile; callers `CDSChain_LoadConfigFromRegistry@0x0040a547` / `0x0040a5c9` on `pKeyBindings+0x0f+i*6` and `strConfigTail` (`CDSChain_LoadConfigFromRegistry.md`); export @ `69020` |
| `0x0042e400` | `FUN_0042e400` | **Registry gzip string write:** write 4-byte `CDSString` byte length (`*(handle-0xc)`); if non-empty, optional `CDsString_AssignFromWideCapped4000` + stream vfn `+0x14` narrow payload | MCP decompile; caller `SaveConfigToRegistry@0x00409cd0` (`CDSChain.md`) |
| `0x0042e4e0` | `FUN_0042e4e0` | **Dual wide-string log write:** two `CDsString_AssignFromWideCapped*` (4000 / 80) + two stream `Write` calls; `__thiscall` on first-string object, second string via stack | MCP decompile; sole caller `FUN_0042a9c0` logging path (decompiler comment) |
| `0x0042e630` | `IsLeapYear` | Gregorian leap test: divisible by 4, except centuries unless divisible by 400 | MCP decompile; used by `CDate_SetDate` |
| `0x0042e680` | `CDSResourceSign_ReadDateField` | `IDSStream` read 4 bytes into `this+0x14` (`publishDate`) | MCP decompile; `CDSResourceSign.md` |
| `0x0042e6a0` | `CDSResourceSign_WriteDateField` | `IDSStream` write 4 bytes from `this+0x14` | MCP decompile; `CDSResourceSign.md` |
| `0x0042e6c0` | `PackTimeToDword` | Pack local time: bits 0–10 ms, 11–17 sec, 18–24 min, 25–31 hour; throws `CDSSimpleException(7,9)` on range error | MCP decompile + plate comment; `CMenu::PollDayNight` uses `dword>>25` as hour (`main_menu.md` §9.4) |
| `0x0042e730` | `CBulanci_PackedTimeToMs` | Decode packed time dword → ms since midnight | MCP decompile; `rng.md` |
| `0x0042e770` | `CDSApp_InitClock` | `g_dwStartMs = timeGetTime(); g_dwElapsedMs = 0` | MCP decompile |
| `0x0042e790` | `CDSApp_UpdateClock` | `g_dwElapsedMs = timeGetTime() - g_dwStartMs` | MCP decompile |
| `0x0042e7b0` | `CDate_SetDate` | Pack calendar date into `*param_1`: year in high bits, month (1–12), day; month length table `DAT_004afce4`; Feb 29 via `IsLeapYear` | `bulanci.ghidra.exe.c`; ctor `CDSResourceSign` default `CDate_SetDate(0,1,1)` |
| `0x0042e850` | `CBulanci_PackTimeDwordFromSystemTime` | `PackTimeToDword` from `SYSTEMTIME` fields `wHour/wMinute/wSecond/wMilliseconds` | Export; called from `BuildLocalDateTime` |
| `0x0042e880` | `CBulanci_SetDateFromShortFields` | `CDate_SetDate(wYear, wMonth, wDay)` from `SYSTEMTIME` ushort array | Export |
| `0x0042e8b0` | `BuildLocalDateTime` | `GetLocalTime` → pack 8-byte buffer: dwords `[0]` time via `PackTimeDwordFromSystemTime`, `[1]` date via `SetDateFromShortFields` on `param_1+4` | Export + `main_menu.md` / `rng.md` (corrects stale “CRT init helper” label in older menu notes) |
| `0x0042e8f0` | `ClassRegEntry_PrependListHead` | Intrusive prepend: `*(void**)this = g_pClassRegHead; g_pClassRegHead = this; *(this+4) = parentMeta` | R5 worker 47 rename; export @ `105370`; static init `0x0047c857` (`static_texts.md`) |
| `0x0042e910` | `HandleClassRegister` | Prepend `ClassRegEntry` on `g_pClassRegHead`; first call `memset(g_apClassByIdTable,0,0x4000)`; fields `+4 parent`, `+8 classId`, `+c factory`, `+10 implements` | Export @ `105385`; `class_registry.py` / `dsm_file_format.md` |
| `0x0042e960` | `FUN_0042e960` | **Singly-linked list membership:** walk `node = *(node+4)` until `node==target` or null; return 1/0 | Export @ `105405`; callers `CLevelScriptOpExt_SeekAnim`, `IsViewKind` with sentinel `&DAT_004b3768` |
| `0x0042e980` | `HandleInterfaceRegister` | Link `InterfaceEntry` on class meta: splice `param_1+0x10` list, store adjustor `param_2` / parent meta `param_3` | Export @ `105426`; static init pairs in `CDSWavStream.md` / `static_texts.md` |
| `0x0042e9a0` | `HandleVirtualBaseCast` | Walk class-meta chain; scan interface list at `meta+0x10` for matching type id; invoke adjustor thunk | Export @ `105439` |
| `0x0042e9f0` | `CheckedVirtualBaseCast` | `HandleVirtualBaseCast`; on failure `CDSSimpleException_Throw(3,5)` | Export @ `105463`; widespread resource-face casts (`CDSBitmap.md`, etc.) |
| `0x0042ea20` | `InitializeClassIdLookup` | If `entry->classId < 0x1000`: `g_apClassByIdTable[classId] = entry` | Export @ `105477`; `dsm_file_format.md` |

### Slice themes

1. **Config / path strings (`0x0042e230`–`0x0042e4e0`)** — gzip registry load/save helpers for `CDSChain_full` key-binding labels and tail string; path pair helper for directory enumeration.
2. **Time / date (`0x0042e630`–`0x0042e8b0`)** — engine calendar encoding, `timeGetTime` frame clock, menu day/night local-hour predicate input.
3. **CDS RTTI registry (`0x0042e8f0`–`0x0042ea20`)** — static-init registration, interface MI graph, fast class-id table, safe casts for deserialization.

### `FUN_0042e2f0` / `FUN_0042e400` calling convention note

Call sites pass **ECX = address of a `CDSString` field** inside `CDSChain_full` / config store (`pKeyBindings+0x0f`, `strConfigTail@+0x45`), not a `CBulanci*`. Ghidra labels both as `CBulanci::__thiscall` because the slot offset was applied to the wrong type.

## 4. Ghidra deltas

**None applied** — MCP `Not connected` after first `batch_decompile`; no `save_program`.

**Queued (evidence-backed, not executed):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `FUN_0042e960` → `ClassRegEntry_ListContains` | List walk @ `105405`; script consumers named in decompiler comment |
| `rename_function_by_address` | `FUN_0042e2f0` → `CDsString_ReadNarrowLengthPrefixedFromStream` | LoadConfig byte order + stream vfn `+0x10` |
| `rename_function_by_address` | `FUN_0042e400` → `CDsString_WriteNarrowLengthPrefixedToStream` | SaveConfig mirror |
| `set_function_this_type` | `FUN_0042e2f0`, `FUN_0042e400` | `CDSString *` (or `wchar_t **` handle) — disasm ECX at `0x0040a547` / `0x00409cd0` |
| `set_function_prototype` + `force_decompile` | same | Prove field access after this fix |

`ClassRegEntry_PrependListHead`, `HandleClassRegister`, `HandleInterfaceRegister`, `HandleVirtualBaseCast`, `CheckedVirtualBaseCast`, `InitializeClassIdLookup`, and date/clock symbols already renamed in prior rounds.

## 5. Frida

**none** — Static decompile + registry layout docs + export file sufficient.

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| `FUN_0042e4e0` formal parameters | Decompiler loses second wide-string argument (`unaff_retaddr`); caller `FUN_0042a9c0` not disassembled this pass |
| `PackTimeToDword` decompiler guard | MCP shows bogus `(uint*)0x17 < param_1` — real checks are hour/min/sec/ms ranges (see plate comment + `PackTimeDwordFromSystemTime` caller) |
| `FUN_0042e960` export name | Behavior proven; rename queued only |
| Live MCP xrefs | `get_xrefs_to` not run (disconnect) — xrefs taken from export + struct_recovery docs |

## Cross-links

- [`CDSChain_LoadConfigFromRegistry.md`](../struct_recovery/CDSChain_LoadConfigFromRegistry.md) — `FUN_0042e2f0` in gzip record order
- [`CDSChain.md`](../struct_recovery/CDSChain.md) — save/load field map
- [`CDSResourceSign.md`](../struct_recovery/CDSResourceSign.md) — `publishDate` @ `+0x14`
- [`dsm_file_format.md`](../../formats/dsm_file_format.md) — class registry addresses
- [`static_texts.md`](../../formats/static_texts.md) — `ClassRegEntry` layout, static init disasm
- [`main_menu.md`](../../main_menu.md) — `BuildLocalDateTime` / day-night hour predicate
- [`rng.md`](../../rng.md) — `_srand` seed via `BuildLocalDateTime` + `PackedTimeToMs`
