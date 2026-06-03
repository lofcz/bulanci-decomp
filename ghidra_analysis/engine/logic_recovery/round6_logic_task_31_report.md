# Round 6 logic — Task 31 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 31 |
| **title** | Logic sim_429_436: 0x004331e0–0x004339e0 (22 funcs) |
| **range** | `sim_429_436` (`0x00429`–`0x00436`) |
| **seed_address** | *(none — slice task)* |

## 2. Status

**PARTIAL** — All 22 functions analyzed from frozen decompile (`bulanci.ghidra.exe.c`), `config/bulanci/mapping.csv`, `vftable_methods.csv`, and prior struct docs (`CDSStrmResInfo.md`, `CDSStreamStorage.md`, `formats/stream_hierarchy.md`). **Ghidra MCP disconnected** (`Not connected`) before live `batch_decompile`, `get_xrefs_to`, renames, `set_function_this_type`, and `save_program`. No Frida (Win32/stream behavior fully provable static).

## 3. Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004331e0` | `CBulanci_CloseFindHandle` | If `*(CBulanci+0x254) != INVALID_HANDLE_VALUE`, `FindClose` on that handle. | Export @ `110896`; field reuse: find handle stored in `CDSApp` dirty-rect pointer slot during enumeration (`OpenFindFirst` writes there). |
| `0x00433200` | `FUN_00433200` | **In-place AABB intersect:** if `param_1` rect valid (`left<right`, `top<bottom`), clip `this` rect to intersection with `param_1`; else if only `param_1` valid, copy `param_1` into `this`. | Export @ `110907` (logic + UNCERTAIN comment); **not** “normalize only” — `combat_projectiles.md` label imprecise. Callers: `FUN_0041af70` @ `85567`, `CShot::TraceCollision` @ `88189`, `CDSApp_AddDirtyRectCoalesced` @ `102006`/`102050`/`102056`, `CGaming_CheckTraceAreasForEntity` @ `84307`, blast collect @ `83624`. |
| `0x00433280` | `rect_Intersect` | **Second intersect helper** (Ghidra namespace `CPoemScroller` — **wrong `this`**): max left/top, min right/bottom vs `param_1`; used with `(CPoemScroller *)&stackRect`. | Export @ `110960`; xrefs: `CDSView` render clip (`app_shell.md` @ `80479`), `SpatialQuery` entity bounds (`82440`), dirty-rect coalesce pairs with `FUN_00433200` @ `102000`. Distinct algorithm from `FUN_00433200`. |
| `0x004332d0` | `CDSRect_ClampToBounds` | Clamps width/height of rect at `ECX` using min/max point pairs (`param_1`, `param_2`); uses `CBulanci.app` physical rect fields. | Export @ `110994`; callers `CDSView_ComputeAnchoredRect` paths @ `103077`, `103421`. |
| `0x00433320` | `CBulanci_OpenFindFirst` | SEH-wrapped: `CloseFindHandle(this)`; `FindFirstFileW(pattern, &WIN32_FIND_DATA at app+0x04)`; store handle in `(this->app).pDirtyRectArray`; flag `pPad_258[0]`; on success assign basename via `CBulanci_GetPathBasename` + `CDsStringAssignFromHandle`. | Export @ `111026`; imports `FindFirstFileW` / `FindClose` (`_externs.h`). |
| `0x004333e0` | `CloseFileHandle` | `__fastcall` on stream subobject: if `*(streamThis+0x1c) != -1`, `CloseHandle` and set `-1`. | Export @ `111074`; called from `CloseStream` / dtor path (`59564`). |
| `0x00433400` | `ReadBytes` | **IDSStream slot 4** (`0x0047f72c`): `ReadFile(this->pFile, …)`; errno `1` / EOF `0x26` on failure. | Export @ `111091`; `stream_hierarchy.md` §2.2; vtable row slot 4. |
| `0x00433470` | `WriteBytes` | **IDSStream slot 5:** `WriteFile(this->pFile, …)`; errno `2` on failure/short write. | Export @ `111119`; vtable slot 5. |
| `0x004334c0` | `LockRegion` | **IDSStream slot 11:** `LockFile(this->pFile, …)`; errno `5`. | Export @ `111142`; vtable slot 11. |
| `0x00433510` | `UnlockRegion` | **IDSStream slot 12:** `UnlockFile(…)`; errno `6`. | Export @ `111162`; vtable slot 12. |
| `0x00433560` | `SeekPosition` | **IDSStream slot 10:** map origin `0/1/2` → `SetFilePointer`; errno `3` + Win32 code. | Export @ `111182`; vtable slot 10. |
| `0x004335e0` | `SetStreamSize` | **IDSStream slot 9:** virtual `Tell` → seek to size → `SetEndOfFile` → restore tell (low 32 bits only — >4 GiB bug noted in export). | Export @ `111216`; vtable slot 9. |
| `0x00433660` | `GetSize` | **IDSStream slot 7:** `GetFileSize` + `__allmul` 64-bit compose; errno `4`. | Export @ `111243`; vtable slot 7. |
| `0x004336c0` | `TellPosition` | **IDSStream slot 8:** `SetFilePointer(..., FILE_CURRENT)`. | Export @ `111272`; vtable slot 8. |
| `0x00433720` | `FlushStream` | **IDSStream slot 6:** `FlushFileBuffers(this->pFile)`; errno `7`. | Export @ `111300`; vtable slot 6. |
| `0x00433750` | `CDSFileStream_Open` | `CreateFileW` with access/share/creation flags from `param_2` bitmask; `CDsStringAssignFromHandle` path; stores handle @ `this+0x10` / `+0x1c`; SEH. | Export @ `111314`; caller `CBulanci_CreateCDSFileStream` @ `104506` (`round5_worker_17_report.md`). |
| `0x004338d0` | `CDSFileStream_CreateInstance` | Factory: `OperatorNew(0x20)` → `CDSFileStream_Ctor`; **IDSChained slot 4** @ `0x0047f710`. | Export @ `111386`; `vftable_methods.csv` `CDSFileStream,0047f710,,4,004338d0`. |
| `0x00433940` | `CDSStrmResInfo_Serialize` | `CDSResInfo_Load` then 8-byte `streamExtent` + 4-byte `dwStreamFlags` via stream vtable read (`+0x10`). | Export @ `111414`; `CDSStrmResInfo.md` VERIFIED. |
| `0x00433980` | `CDSStrmResInfo_Deserialize` | `CDSResInfo_Save` then 8+4 byte write (`+0x14` tail). | Export @ `111427`; `CDSStrmResInfo.md`. |
| `0x004339c0` | `CDSStreamStorage_GetStreamCount` | **IDSStorage vtable +6:** returns `(this->collection).pM_items` @ `+0x24` — **not** entry count. | Export @ `111443`; `round3_task_46_report.md`; suspect caller `IDSAnim_SelectRandomTrack` @ `117067`. |
| `0x004339d0` | `CDSStreamStorage_GetStreamEntry` | **IDSStorage +7 dead stub:** indexes `m_pVtable_IDSChained` @ `+0x20` (off-by-4 vs live `m_items`). Vtable-only. | Export @ `111456`; `CDSStreamStorage.md` stub audit. |
| `0x004339e0` | `FUN_004339e0` | SEH unwind: `LeaveCriticalSection((LPCRITICAL_SECTION)*param_1)`. Seven `Unwind@00479fxx/0047a8xx` call sites in export. | Export @ `111468`; `round5_worker_08_report.md`. |

### Slice grouping

1. **`0x004331e0`–`0x00433320`** — Win32 directory search helpers + shared **RECT** utilities (intersect ×2, clamp).
2. **`0x004333e0`–`0x00433750`** — `CDSFileStream` **IDSStream** Win32 file backend (`CreateFile` / `ReadFile` / `WriteFile` / lock / seek / size).
3. **`0x004338d0`** — heap factory for `CDSFileStream` (0x20 bytes).
4. **`0x00433940`–`0x004339e0`** — pack resource stream metadata I/O + `CDSStreamStorage` IDSStorage interface stubs + EH unlock.

### `FUN_00433200` vs `rect_Intersect` (manifest note)

Both participate in collision and dirty-rect paths; they are **different functions** at different addresses. Prior R5 text “RECT intersect helper” for `FUN_00433200` matches export; `combat_projectiles.md` “normalize” should read **sub-step intersect** before `SpatialQuery`.

## 4. Ghidra deltas

**None applied** — MCP `Not connected` after analysis pass.

**Queued (evidence-backed, not executed):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `FUN_00433200` → `CDSRect_IntersectInPlace` | Export body + caller set (movement, shot trace, dirty rect, trace areas, explosions) |
| `rename_function_by_address` | `FUN_004339e0` → `Eh_LeaveCriticalSection` or `CDSStreamStorage_UnwindLeaveCriticalSection` | Sole insn `LeaveCriticalSection(*param_1)`; unwind-only xrefs |
| `set_function_this_type` | `FUN_00433200` → `tagRECT *` (or `CDSRect *`) | `ECX` is dest rect, not a C++ object |
| `set_function_this_type` | `rect_Intersect@0x00433280` → `tagRECT *` | All call sites pass stack `RECT*` with bogus `CPoemScroller` cast |
| `set_decompiler_comment` | `GetStreamCount` / `GetStreamEntry` | Re-apply R3/R4 stub audit if comments missing in DB |
| `save_program` | `bulanci.exe` | Once after above |

Symbols already correct in export for `CDSFileStream_*`, `CDSStrmResInfo_*`, `CBulanci_OpenFindFirst`, `CBulanci_CloseFindHandle`.

## 5. Frida

**none** — Win32 API mapping and vtable slots verified from disasm/decompile export and `stream_hierarchy.md`.

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| `CBulanci_OpenFindFirst` field aliases | Reuses `CDSApp.pDirtyRectArray` / `pPad_258` for find handle + success flag — layout doc for enumeration state not in slice |
| `FUN_004339e0` parent functions | Unwind sites @ `177984`…`179373` not mapped to throwing ctor names this pass (no live xrefs MCP) |
| `rect_Intersect` vs `CDSRect_IntersectInPlace` unification | Two algorithms; whether one superseded the other — **not** merged without disasm diff |
| `IDSAnim_SelectRandomTrack` + `GetStreamCount` | Suspect fake `CDSStreamStorage*` @ `CBulAnim+0x1c` (`round3_task_46`) — out of slice except stub @ `0x004339c0` |
| Live Ghidra DB vs export | `report.json` still lists `FUN_00433750` / `FUN_00433320` in places — re-verify after MCP restore |

## Cross-links

- [`formats/stream_hierarchy.md`](../../formats/stream_hierarchy.md) — IDSStream slot map for `CDSFileStream`
- [`CDSStrmResInfo.md`](../struct_recovery/CDSStrmResInfo.md) — serialize tail @ `+0x14`..`+0x1f`
- [`CDSStreamStorage.md`](../struct_recovery/CDSStreamStorage.md) — IDSStorage stubs +6/+7
- [`round5_worker_17_report.md`](../struct_recovery/round5_worker_17_report.md) — `CBulanci_CreateCDSFileStream` → `CDSFileStream_Open`
- [`round3_task_46_report.md`](../struct_recovery/round3_task_46_report.md) — `GetStreamCount` / `GetStreamEntry` audit
- [`combat_projectiles.md`](../../gameplay/combat_projectiles.md) — `CShot::TraceCollision` sub-steps
- [`app_shell.md`](../app_shell.md) — dirty-rect coalesce + render clip
