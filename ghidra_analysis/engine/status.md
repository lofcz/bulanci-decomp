# Engine Subsystems Status

Confidence levels:
* **Empty** — not yet investigated.
* **Sketched** — high-level shape only; details TBD.
* **Partial** — most of the surface mapped; specific corners uncertain.
* **Verified** — Ghidra-cross-checked: layouts, dispatch tables and callsites are tied to specific addresses in `bulanci.exe`.

---

## Low-Level Engine Subsystems & Runtime

These represent core execution engines, lifecycle managers, hardware wrappers, and standard contracts.

### Script VM

* **Classes:** `CDSScript`, `CLevelScript`
* **Bytes:** ~0.6 KB
* **Confidence:** **Verified**
* **What's known:**
  * 103-opcode dispatch table at `this+0x2c` built from `0x004b0118` (base 45) + `0x004af018` (ext 58).
  * Dispatcher `FUN_004384c0`.
  * Interpreter loop `FUN_00438b30`.
  * Public entry `FUN_00438c40`.
  * All 13 export indices mapped to engine callsites with argc and semantic names.
* **Open questions:**
  * Per-opcode operand decoding for ops without a clean editor counterpart.
  * Full stack-effect table.
* **Artefacts:** `./script_lifecycle.md`, `./script_dispatch_table.md`

### App shell

* **Classes:** `CDSApp`
* **Bytes:** 1.6 KB
* **Confidence:** **Verified**
* **What's known:**
  * CRT entry chain `_mainCRTStartup` (`0x00448e40`) → `___tmainCRTStartup` (`0x00448c60`) → `WinMain` (`0x00402680`) → `CDSApp_AppMain` (`0x0042aa60`).
  * `CDSApp_AppMain` calls `(*g_AppDescriptor.factory)()` = `CBulanci_CreateObject` (`0x00402a90`), then dispatches through `g_pApp`'s vftable in fixed order: `SetCmdLine` (vtbl[31], purecall) → `CDSApp_OnCreate` (vtbl[28] = `0x0042a210`) → `CDSApp_Run` (vtbl[29] = `0x00429d60`) → `CDSApp_dtor` (vtbl[2] = `0x0042b560`).
  * **Primary vftable `g_pCDSApp_vftable @ 0x0048700c`** — 34 slots enumerated end-to-end and renamed in Ghidra. Includes the entry-point quartet, the WndProc dispatcher, the activate hook, the modal entry/exit hooks, the dirty-rect callback, view-tree serialization slots 3/4/5 (broadcast pattern), `CDSView_HitTest`, `CDSView_SetRect`/`GetParentBounds`/`ComputeAnchoredRect`, `CDSView_OnFocus`/`OnKeyDown`/`OnKeyUp`/`OnChar`, and the purecall stubs at 15..18 and 31. Full slot table in `app_shell.md`.
  * **WndProc** `CDSApp_WndProc` (`0x00429c00`) is a trivial relay into vtbl[32] (`CDSApp_WndProcDispatch` = `0x0042a660`), the WM_* switch. Full message map documented (DESTROY, PAINT, CLOSE, ACTIVATEAPP, SETCURSOR, KEY{DOWN,UP,CHAR}, SYSKEY{DOWN,UP}, MOUSE{MOVE,L*,R*}). ALT+ENTER toggles fullscreen via `CDSApp_SetWindowed`.
  * **Message pump**: `CDSApp_Run` → `CDSView_DoModal(this, NULL)` — the generic modal pump also reused by every dialog. Inner loop is `while (this->exitCode == 0) CDSApp_PumpTick(g_pApp)`. Tick body is "drain frames while engine event queue is empty, else dispatch one engine event". One frame = `CDSApp_UpdateClock` + `CDSApp_PulseTasks` + Win32 PeekMessageW / TranslateMessage / DispatchMessageW drain, then `CDSApp_RenderFrame` via the embedded `CDSBackBuffer` at `+0x7c/+0x80`.
  * **Tick frequency**: free-running (no `Sleep` / no `WaitMessage`). The pump only stops on the per-frame `PeekMessage`. Master clock is `g_dwElapsedMs = timeGetTime() - g_dwStartMs` (`@ 0x004b3bd8` / `0x004b3bdc`), refreshed at the top of every frame.
  * **Globals labelled & typed in Ghidra**: `g_pHInstance`, `g_pHwnd`, `g_pApp`, `g_pModalFocus`, `g_pInputChainHead`, `g_dwElapsedMs`, `g_dwStartMs`, `g_pTaskList`, `g_pEventQueue`, `g_AppDescriptor`, `g_AppClassTable`, `g_pCDSApp_vftable`.
  * **Object layout** (CDSApp portion): 4-vptr MI at `+0/+4/+0x10/+0x18`, logical app rect at `+0x20..+0x2c` (default 800×600), `flags1` `+0x44` and `flags2` `+0x46`, modal-loop exit code at `+0x4a`, parent-frame ptr at `+0x4c`, child-views list head at `+0x54`, embedded `CDSBackBuffer` at `+0x7c/+0x80`, `windowed` registry cache at `+0xe4`, dirty-rect array `+0x254` / count `+0x25c`, drawable flag `+0x274`. Shipping derived class is `CBulanci` (`0x4cc` bytes total, ctor `CBulanci_ctor`).
  * **App descriptor** at `g_AppDescriptor @ 0x004b3300` is a CDS class-registration record filled at C++ static-init time by `CBulanci_RegisterAppDescriptor` calling `HandleClassRegister(_, 2000, &g_AppClassTable, &CBulanci_CreateObject)`. The `+0xc` slot holds the factory pointer the engine top-level invokes.
  * Functions named in Ghidra (this round): `WinMain`, `CDSApp_AppMain`, `CDSApp_OnCreate`, `CDSApp_Run`, `CDSApp_WndProc`, `CDSApp_WndProcDispatch`, `CDSApp_OnActivateApp`, `CDSApp_PreCreateHook`, `CDSApp_OnDestroy`, `CDSApp_SetWindowed`, `CDSApp_ctor`, `CDSApp_dtor`, `CDSApp_DtorScalar`, `CDSApp_GetClassTable`, `CDSApp_DispatchInputEvent`, `CDSApp_KeybQueue`, `CDSApp_MouseQueue`, `CDSApp_PumpTick`, `CDSApp_FrameBody`, `CDSApp_RenderFrame`, `CDSApp_InitClock`, `CDSApp_UpdateClock`, `CDSApp_PulseTasks`, `CDSApp_PollEventQueue`, `CDSApp_DispatchOneEvent`, `CDSApp_AdaptDisplaySize`, `CDSView_DoModal`, `CDSView_SetModalEligible`, `CDSView_SetActive`, `CDSView_Invalidate`, `CDSView_IsModalDoneRecursive`, `CDSView_AcquireKeyboardFocus`, `CDSView_HitTest`, `CDSView_SetRect`, `CDSView_GetParentBounds`, `CDSView_ComputeAnchoredRect`, `CDSView_OnFocus`, `CDSView_OnKeyDown`, `CDSView_OnKeyUp`, `CDSView_OnChar`, `CDSView_EmptyHook27`, `CBulanci_CreateObject`, `CBulanci_ctor`, `CBulanci_RegisterAppDescriptor`, `CBulanci_GetAppDescriptor`, `CDSDirectSound_InitPrimary`.
* **Open questions:** Semantics of base-class vtable slots 3/4/5 (broadcast templates whose per-class meaning depends on the derived override — needs a concrete override read on e.g. `CMenu`); layout of `g_AppDescriptor` past `+0x0c`; the wakeup contract between `CDSApp_KeybQueue` / `CDSApp_MouseQueue` enqueues at `this+0x10` and the pump's `CDSApp_PollEventQueue` / `CDSApp_DispatchOneEvent` dequeue side. None of these block byte-exact matching of `CDSApp` itself.
* **Artefacts:** `./app_shell.md`, `./master_vtable_catalog.csv`, `./master_vtable_catalog.md`, `./vftable_methods.csv`

### Animation runtime

* **Classes:** `CDSAnim`, `CAnim`, `CBulAnim` (+ pure `IDSAnim` interface @ RTTI `0x004ae8ac`)
* **Bytes:** 0.7 KB (engine classes only — most of the actual playback code lives in a shared 7-vtable shell + ~28 functions in the `0x004386f0..0x00439eb0` cluster that Ghidra splits across `CDSVideoPlayer` / `CDSAudioPlayer` / `CDSObject` namespaces)
* **Confidence:** **Partial**
* **What's known:**
  * **Inheritance.** RTTI confirms the ladder is `IDSInterface → IDSAnim → CDSAnim → CBulAnim`, with `CAnim` as a *sibling* concrete class (separate hierarchy, also implementing `IDSAnim` but at object offset `+0x98` instead of `+0x78`). Sizes: `CDSAnim` = 0xd0, `CBulAnim` = 0xd4 (extra team-index byte at `+0xd0`), `CAnim` = 0xf0.
  * **Seven-vftable layout.** Both `CDSAnim` and `CBulAnim` carry vftables at offsets `0x00 / 0x04 / 0x10 / 0x18 / 0x68 / 0x6c / 0x78` — the IDSImage / IDSChained / IDSEventHandler / IDSReferenced / IDSUpdated / **IDSAnim** / chain-thunk faces. `CAnim`'s 0x20-byte widget chrome shifts the last three slots to `0x88 / 0x8c / 0x98`. All COL records and per-slot dispatch tables are mapped in `anim_runtime.md`.
  * **Track manager** is an embedded `CDSObject` at `outer + 0x88` (init `CDSObject::ConstructTrackManager @ 0x00439c70`, teardown `0x00439d30`). It owns:
    * a tracks array (`+0x1c`, 8-byte entries `{u32 flags, CDSAnimSequence*}`),
    * the current track index (`+0x2c`),
    * current frame index (`+0x38`) that wraps at `seq[0x14]`,
    * a paused flag (`+0x35`), and
    * a `frameDelayOverrideMs` field (`+0x44`, `-1` = use sequence duration).
  * **Frame timing model — unit pinned to milliseconds.** The engine clock is `g_dwElapsedMs = timeGetTime() - g_dwStartMs` updated once per frame in `CDSApp_UpdateClock @ 0x0042e790`. Per-track timers live on the CDSObject scheduler (`Scheduler_DispatchDueEvents @ 0x0042eb30`, arm with `Scheduler_SetEventDelayMs @ 0x0042f2d0`, anchor with `Scheduler_SetEventLastFireMs @ 0x0042f290`): each slot fires when `g_dwElapsedMs >= lastFire + delay`, so **every `delay` value flowing through this system is real wall-clock milliseconds**. On each tick `TM_AdvanceFrame @ 0x004399b0` arms the next slot from the active sequence's declared duration via Bresenham-style integer division `delay = ((f+1)*seq[0x10])/seq[0x14] - (f*seq[0x10])/seq[0x14]` ms, so `seq[0x10]` is total duration in ms and `seq[0x14]` is frame count. A non-negative `+0x44` overrides this with a fixed per-frame delay (also ms).
  * **`frameDelayOverrideMs` (`+0x44`) is set ONLY by the construction-time speed formula** — there is exactly one writer in the whole binary (`TM_SetFrameDelayOverrideMs @ 0x00439720`, called from `CBulanek::FUN_0041e4b0`). Given a speed parameter (40..200, default 100 == no override), it computes `delay_ms = round(47.25 / (speed / 100.0)) = round(4725 / speed)` (constants pinned at `g_kSpeedFormulaNumerator_47p25 @ 0x00482908` and `g_kSpeedFormulaNeutral_100p0 @ 0x00482910`). That gives the sensible-game range: 24 ms / 41.7 fps at speed=200, 47 ms / 21 fps at speed=100 (formula skipped — keeps -1 so sequence default applies), 118 ms / 8.5 fps at speed=40.
  * **FLX opcode 0x0C is a side-channel notification, NOT a timing override.** The opcode-0x0C handler in `CDSFlxFile::DecodeFrame` calls `BroadcastFrameTimeHint @ 0x00436ef0` (previously `_Globals::NotifyFrameTime`), which walks the **per-consumer** subscriber list at `consumer+0x38..consumer+0x40` and dispatches to each subscriber's vftable slot 4. Across the entire binary there is no path from this fan-out that writes the track manager's `frameDelayOverrideMs` — confirmed by an exhaustive instruction sweep for direct stores to `[*+0x44]` and a callgraph walk from `BroadcastFrameTimeHint`. The earlier inference that 0x0C drove playback cadence was wrong; in the 130-sprite master pack the actual values (mostly 0/1) corroborate this — they would be sub-perceptible as ms delays.
  * **Connection to `CBulPicture` / FLX.** Resources expose their animation face via `CheckedVirtualBaseCast(res, DAT_004b8370)` (distinct from the static-bitmap face at `DAT_004b826c`). On every tick the runtime calls `seq->vfn[7]((trackEntry, renderTarget))` — for a `CBulPicture` sequence this is the engine wrapper around `CDSFlxFile::DecodeFrame @ 0x00432c60`. The reverse direction (sequence → runtime) is the 5-slot `IDSAnim` vtable at `obj+0x6c`, with slot 0 (`FUN_00438f20`) being the bulk-event dispatcher that forwards each 16-byte event record to `vbase->vfn[9]` (the script-facing `OnBitmapEvt` path).
  * **Per-tick blit** goes through `CPoemScroller::BlitDispatch` (`TM_TickBlit @ 0x00439080`), called against the engine singleton at `DAT_004b3b88 + 0x80`.
  * **Construction templates** for both `CBulAnim` (`CMenu::FUN_004104f0`) and `CAnim` (`CBulanci::FUN_00411010`) recovered: alloc → base ctor `CDSAnim::FUN_00439560(this, x, y, NULL, 0)` → patch all 7 vftables → 4× `BindSequence(this+0x78, ...)` for the four facing/anim variants packaged in one BitmapSprite → `SetTrack` with a random starting frame and the team-tinted 256-entry palette remap.
* **Open questions:**
  * ~~On-disk source of `seq[0x10]` / `inMemSize` vs `sizeof(CBulPicture)`~~ **done (R3+R4 todo 32):** `CDSFlxFile_BindStream@0x00432ac0` copies file `inMemSize` → `nSeqTotalDurationMs` (per-clip ms; master pack **1/130** == `0x470`). Sole Bresenham reader `TM_AdvanceFrame@0x004399b0`. ClassID **76** `CDSDsmFile` fills same meta offsets; see [round4_task_32_report.md](struct_recovery/round4_task_32_report.md).
  * The real purpose of FLX opcode 0x0C (broadcasts a u16 via `BroadcastFrameTimeHint @ 0x00436ef0` to per-consumer subscribers). Plausible roles: profiling/debug timing hints, script-VM hooks for cadence-aware behavior, or a deprecated codepath.
  * Exact slot order of the 24-entry primary vtable (the IDSImage face).
  * Semantics of `+0x70` and `+0x74` in the IDSAnim subobject (used as a subscriber-identity cookie in `SetCurrentSequence`).
  * Layout of the 16-byte "user event" record that IDSAnim slot 0 forwards to `vbase->vfn[9]`.
* **Artefacts:** `./anim_runtime.md`

### DirectSound wrapper

* **Classes:** `CDSDirectSound`, `CDSAudioPlayer`, `CDSAudioVideoPlayer`
* **Bytes:** 1.2 KB
* **Confidence:** **Empty**
* **What's known:** DSound primary/secondary buffer management.
* **Open questions:** Mixer slot count, audio/video sync model.
* **Artefacts:** —

### Audio bank

* **Classes:** `CDSAudioBank`, `CDSAudioBankSample`, `CDSWav`, `CDSWavStream`
* **Bytes:** 2.0 KB
* **Confidence:** **Empty**
* **What's known:** Streamed sample playback layered on top of the stream hierarchy.
* **Open questions:** Sample-table layout in `.dsm`, RAM-cached vs streamed decision.
* **Artefacts:** —

### Threading

* **Classes:** `CDSWorkingThread`
* **Bytes:** 0.14 KB
* **Confidence:** **Empty**
* **What's known:** 5 funcs / 2 vtables; small but central — wraps `CreateThread` for audio/video streaming.
* **Open questions:** API surface (start/stop/wait), used by which clients.
* **Artefacts:** —

### Input

* **Classes:** `CDirectKeyb`, `CDSMouse`, `CDSImageMouse`, `CGunMouse`
* **Bytes:** 2.8 KB
* **Confidence:** **Empty**
* **What's known:** DirectInput-backed keyboard wrapper + three mouse variants (raw / image-cursor / weapon-aim).
* **Open questions:** Key-binding storage (probably profile-side), how `CGunMouse` couples with `CBulanci` aim.
* **Artefacts:** —

### Exception hierarchy

* **Classes:** `CDSException` + 7 subclasses (`CDSStreamException`, `CDSDirectXException`, `CDSRegKeyException`, `CDSApiException`, `CDSResourceException`, `CDSMemoryException`, `CDSSimpleException`)
* **Bytes:** 1.6 KB
* **Confidence:** **Empty**
* **What's known:** One vtable each; the base owns `What()`.
* **Open questions:** Which throw sites are wired to which subclass — knowing this attaches every `Unwind@xxxx` to its semantic parent.
* **Artefacts:** —

---

## Game UI Widget Contract

### Widget base contract

* **Classes:** (UI widgets: `CButton`, `CRadio`, `CSwitch`, `CScrollBar`, `CScroller`, `CListBox`, `CListBoxItem`, `CListViewer`, `CEdit`, `CNumEdit`, `CWindow`, `CPanel`, `CStaticText`, `CVolume`, `CColorSet`, `CColorSwitch`, `CKeybShow`, `CProgressBar`, `CSwitch`, `CBitmap`)
* **Bytes:** ~30 KB
* **Confidence:** **Empty**
* **What's known:** Every widget has a 4- or 5-entry vtable with what looks like ctor/dtor/Draw/Tick/HandleInput.
* **Open questions:** Exact virtual-slot order on the shared base — unlocks ~270 widget functions for typed matching.
* **Artefacts:** —

---

## Gameplay spine (CBulanci / CBulanek campaign)

* **Classes:** `CBulanci`, `CGame`, `CGaming`, `CGameView`, `CBulanek`, `CWeapon`, (+ leaves `CBulAnim`, `CShot`, `CMina`, `CTeleportPoint`)
* **Confidence:** **Partial** (coordinator round 2026-05-30)
* **What's known:**
  * **`CBulanci`**: `0x4CC` heap app object; **`CDSApp app`** @ `+0` + **`CGame game`** @ `+0x284` (`0x248`, audio tail inside embed) — `CBulanci.md`.
  * **`CGame`**: **584 B (`0x248`)** in Ghidra; used for embed and `CGame *` consumers — `CGame.md`.
  * **`CGaming`**: match modal **876 B (`0x36C`)** on stack; **128 entity slots** @ `+0xC8`; **not** 0x20c (that is `CDirectKeyb`) — `CGaming.md`.
  * **`CGameView`**: shared **152 B (`0x98`)** view header; `CBulanek` reuses through `+0x87` — `CGameView.md` / `CBitmap.md`.
  * **`CBulanek`**: player entity **412 B (`0x19c`)**; ctor `0x0041e4b0`; 6 vtable facets; weapon @ `+0xF8`, game @ `+0xF4`, track mgr @ `+0xA8` — `CBulanek.md`.
  * **`CWeapon`**: **112 B (`0x70`)**; owned by `CBulanek+0xF8`; `Fire` @ vtable `0x00481ed4` slot 4 — `CWeapon.md`.
  * **Input path**: `CGame__SchedulerDispatch` case 7 → `CGame_DispatchPlayerAction` → `CBulanek_ApplyAction` (`player_controls.md`).
* **Open questions:**
  * Apply Ghidra structs for `CGameView`, `CGaming`, full `CBulanek` field names (MCP layout still has gaps; **`videoTrackManager` @ +0xA8** applied — agent todo 42).
  * `CGame` padded bands and level-list element type @ `+0xbc`.
  * `CShot` struct doc (slice 52; agent in flight).
  * ~~Resolve `pGameEmbed` vs standalone `CGame` typing on `CBulanci`~~ **done** (agent todo 12).
* **Artefacts:** `struct_recovery/CBulanek.md`, `CGameView.md`, `CGaming.md`, `CWeapon.md`, `gameplay_struct_backlog.md`, `struct_recovery/batches_50.json`

---

## Agent todo wave round 3 (2026-05-30)

**Coordinator:** round-3 gather G0–G4 → [`todos_gather_r3_0.json`](./todos_gather_r3_0.json) … [`todos_gather_r3_4.json`](./todos_gather_r3_4.json) merged into [`agent_todos_50_r3.json`](./agent_todos_50_r3.json) (checklist: [`agent_todos_50_r3.md`](./agent_todos_50_r3.md)). **Sources:** 29 handoff, 13 blocker, 8 backlog (R2 results + slice blockers; skips R2-done unless new blocker). **Workers:** 50 background agents → [`agent_todos_50_r3_results.jsonl`](./agent_todos_50_r3_results.jsonl); registry [`agent_todos_50_r3_workers.json`](./agent_todos_50_r3_workers.json).

**Round-3 focus:** CBulanek tail rename + ctor decompiler (41–42), `CLevelScore`/`CListBoxItem` Ghidra verify (14), `CDSChain_full` sentinel (27), MCP ECX bundle (28), ~~`CDSAudioVideoPlayer` nested track (25)~~ **done** — `videoTrackManager` → `CDSVideoPlayer` @ +0x08 (R3 todo 25), gameplay dialog/`CWindow` embeds (slices 00–19).

**Ghidra MCP (2026-05-30):** Structure resize and member-function `this` typing — upstream [STRUCT_RESIZE_WORKFLOW](https://github.com/bethington/ghidra-mcp/blob/main/docs/STRUCT_RESIZE_WORKFLOW.md) (`resize_struct`, `recreate_struct`, `set_function_this_type`, …).

| Band | ids | Priority highlights |
|------|-----|---------------------|
| G0 slices 00–09 | 1–10 | `CBulanci` tail xrefs, `CDSApp` +0x100 band, `CAdvertising` `CWindow` embed |
| G1 slices 10–19 | 11–20 | **`CLevelScore` 0x28 apply** (14), `CPauseDlg_Build` ECX (16), `CGame+0x66` writer (13) |
| G2 slices 20–29 | 21–30 | **`CDSChain_full`** (27), **MCP ECX bundle** (28), `videoTrackManager` retype (25) |
| G3 slices 30–39 | 31–40 | FLX naming (31–33), `CDSFont`/`CDSImage` MI (34–35), exception dedup (39) |
| G4 slices 40–49 | 41–50 | **CBulanek** tail cleanup (41), scheduler/video mgr ctor (42), `CWeapon_ctor` (49) |

Round-2 manifest (superseded for new work): [`agent_todos_50_r2.json`](./agent_todos_50_r2.json) | results: [`agent_todos_50_r2_results.jsonl`](./agent_todos_50_r2_results.jsonl) (3/50 recorded at R3 gather time).

---

## Agent todo wave round 2 (2026-05-30)

**Coordinator:** round-2 gather G0–G4 → [`todos_gather_r2_0.json`](./todos_gather_r2_0.json) … [`todos_gather_r2_4.json`](./todos_gather_r2_4.json) merged into [`agent_todos_50_r2.json`](./agent_todos_50_r2.json) (checklist: [`agent_todos_50_r2.md`](./agent_todos_50_r2.md)). **Sources:** 34 handoff, 14 blocker, 2 backlog (follow-ups from round-1 done/partial + slice blockers; excludes CRT SKIP). **Workers:** 50 background agents → [`agent_todos_50_r2_results.jsonl`](./agent_todos_50_r2_results.jsonl); registry [`agent_todos_50_r2_workers.json`](./agent_todos_50_r2_workers.json).

**Round-2 focus:** gameplay spine field passes (`CGame`/`CGaming`/`CBulanek`), Ghidra apply gaps (slice 13 `CLevelScore`, MCP ECX retry todo 28), `CDSScript`/`CDSAudioVideoPlayer` cleanup, `CDSChain_full` sentinel (todo 27).

| Band | ids | Priority highlights |
|------|-----|---------------------|
| G0 slices 00–09 | 1–10 | `CGame` scheduler embed, `CDSApp` MI gaps, `CDSScript` VM header |
| G1 slices 10–19 | 11–20 | `CGaming.game` 0x248 embed, slice 13 `CLevelScore`, `CPauseDlg` ECX |
| G2 slices 20–29 | 21–30 | `CDSChain_full`, MCP retry bundle, `CDSAudioVideoPlayer` nested track |
| G3 slices 30–39 | 31–40 | FLX opcode 0x0C, `CDSImage` MI, exception dedup |
| G4 slices 40–49 | 41–50 | `CBulanek` tail layout, `CWeapon` ctor, `CDSWav` vs DSM |

**R2 worker 50 (todo 50):** **done** — `HandleResourceRead` `this` bases proven by disasm: `CDSWav_HandleResourceRead@0x43b960` ECX=face (`primary+4`); `CDSWav_ReleaseRefcount@0x433040` ECX=primary+0; `CDSDsmFile_HandleResourceRead@0x428ad0` ECX=`CDSDsmFile+0x1c`. Ghidra: `CDSWav_face8slots` struct, prototypes/comments, DSM rename.

**R3 worker 50 (todo 50):** **done** — class-43 heap owner is **`CDSWavStream`**: `CDSWavStream_ScalarDeletingDtor@0x41bc00` → `CDSWavStream_dtor` + stash `@+0x34`; `CDSWav_ScalarDeletingDtor@0x41bbe0` is light MI (`CDSObject` only). Factory `0x4823xx` routes all installed delete thunks to stream dtor; ROM `0x482348` not written by factory. Report: [`struct_recovery/round3_task_50_report.md`](./struct_recovery/round3_task_50_report.md).

Round-1 manifest (superseded for new work): [`agent_todos_50.json`](./agent_todos_50.json) | results: [`agent_todos_50_results.jsonl`](./agent_todos_50_results.jsonl).

**R2 worker 1 (todo 1):** ~~`CGame.scheduler` `CDSUpdatedItem` @ +0x04~~ **done** — was `byte[24] pScheduler`; decompile `CGame_ctor` / `CGame__SchedulerDispatch@0x00416030`.

---

## Agent todo wave (2026-05-30) — round 1

50 parallel workers dispatched from [`agent_todos_50.json`](./agent_todos_50.json) (checklist: [`agent_todos_50.md`](./agent_todos_50.md)). Gather: G0–G4 → 50 handoff todos (0 backlog fill).

1. ~~Recover CGame embed at CBulanci+0x284~~ **done** — `CBulanci.game` `CGame` 584 B (todos 1/12)
2. Reparent CBulanci to extend CDSApp in Ghidra [critical]
3. ~~Register CGameView* and fix CGameView_ctor prototype~~ **done** — `CGameView` 152 B + `CGameView *`; ctor/update/init prototypes (agent todo 3)
4. ~~Delete stale 1B CDSScript type; apply CDSScript* prototypes~~ [high] — **done** (worker 4, 2026-05-30)
5. Name CAnim/CGameView header band +0x28..+0x67 [high]
6. Apply CDeath struct 0x108 and ctor prototypes in Ghidra [high]
7. ~~Type CBulanci+0x280 as CMenu* (main menu hook)~~ **done** — `pMainMenu` `CMenu *` @ +640 (agent todo 7)
8. Split CDSUpdatedItem fields in CAnim track_manager +0xA8 [medium]
9. Name CDSObject image MI slots +0x48..+0x4f [medium]
10. Map CGunMouse trackManager +0x1B0..+0x1F7 and scalar gap [medium]
11. ~~Rebuild global CWindow to 0x70 dialog prefix~~ **done** — `CWindow` 112 B (32 fields, chain band `+0x40..+0x50`); `CSessionList` embeds `win` (agent todo 11)
12. ~~Shrink CGame Ghidra layout to 0x248 bytes~~ **done** — `CGame` 584 B; `CBulanci.game` embed; `CGame_embedded` removed (worker 12)
13. ~~Name CGame level resource table at +0x66~~ **done** — `pLevelResourceTable` @ +0x66, `CGame_FindResourceByName` prototype (worker 13)
14. ~~Fix CPauseDlg_Build decompiler this type~~ [high] **partial** — return `CPauseDlg*`; prototype/plate/call-site comments; `pGame` struct fixed; decompiler ECX `this` still `CBulanci*` / `field_0x70` (MCP `__thiscall` limit; r2 worker 14)
15. ~~Type CMina gaming_host at anim +0x84~~ **done** — `CAnim`/`CBitmap` `gaming_host` → `CGaming *`; `CMina_UpdateTraceAreas@0x00419fd0` (agent todo 15)
16. ~~Apply CLevelScore struct 0x28 in Ghidra~~ [high] **done** — `CLevelScore` 40 B, `scoreChain` `CDSChain` @ +0x14 (agent todo 16)
17. ~~Split CScore base_to_6c into CWindow fields~~ [medium] **done** — `CScore` 116 B, 31 fields, `CExitDlg`-aligned prefix (worker 17)
18. ~~Name CHelpScript / dialog CWindow prefix~~ **done** — `CHelpScript` + `CDSScript script`; `CHistoryDlg`/`CHelpDlg` embedded `CWindow win` @ 0; ctors `0x004215e0`/`0x004231d0`/`0x00421e40` (agent todo 18 r2)
19. ~~Retype CSessionList scheduler methods this~~ **done** (R3 todo 19 — `set_function_this_type` @ 0x0040c570/0x0040c5d0/0x0040c550; scheduler @ 0x004162bd/0x0041630d)
20. ~~Replace CMina CAnim_recovered with CAnim~~ [medium] **DONE** (worker 20 — `CMina.animBase` = `CAnim`, `CAnim_recovered` absent)
21. Embed CDSChain_full in CGame at +0x31 (0xa4) [critical]
22. Rename CDSChained view-tree walk helpers [critical]
23. ~~Delete duplicate 1B CDAudioVideoPlayer type~~ **done** — nested 1 B type removed; `CDSAudioVideoPlayer_Constructor`/`_dtor`/`_ScalarDeletingDtor` use `CDSAudioVideoPlayer *`; `videoTrackManager` `byte[72]` @ +8 (agent todo 23)
24. ~~Field-map CDSObject track_manager inside CDSAnim @ +0x88~~ **DONE** (agent todo 24: `CDSVideoPlayer track_manager` @ `CDSAnim+0x88`, `CDSObject` prefix renamed, `CDSTrackEntry` 8 B)
25. ~~Retype CDSDirectSound_InitPrimary call sites~~ **done** — `CDSDirectSound_InitPrimary(CDSDirectSound *)`; call sites `LEA [app+0x200]` @ 0x0042a2fa / 0x0042b27d; `CDSApp::directSound` @ +0x200 (agent todo 25; InitPrimary body still `CDSApp *` in decompiler)
26. ~~Resolve CDSDirectSound SetEvent offset (+0x4c vs +0x50)~~ **done** — handle @ +0x50; `SignalEventIfVoicesActive` asm off-by-4 @ `0x0043cce1` [high]
27. ~~Name CDSAnim/CDSChained drawable prefix +0x28..+0x67~~ **done** — `CDSAnim`/`CDSBitmap` Ghidra fields `+0x14..+0x67` mirror `CDSChained` shell; `wChainInit44` @ `+0x44` (visibility byte, `TM_SetTrack`) (worker 27) [high]
28. ~~Trace CDSAudioPlayer pEventTarget playback-complete dispatch~~ **done** — `CDSDirectSound_OnPlaybackCompleteMessage@0x0043cdc0` reads +0x18 after WM 0x200/1 from `OnPlaybackTick` [high]
29. ~~Fix CTeleportPoint OnEvent partner_node typing~~ **done** — `pPartner_node` `CTeleportPoint*`; OnEvent partner probe `this[-1].pPartner_node` + `dwView_flags` @ partner `+0x44` (paired alloc)
30. ~~Fix CDSCollection_InsertKeyed decompiler this quirk~~ **partial** (worker 30) — `CDSCollection*` prototype + plate/asm; ECX retype blocked by Ghidra API [medium]
31. ~~CDSFlxFile DecodeFrame callsites~~ [high] — **done** (worker 31): vtable-only static xref `0x004872c0`; live dispatch `TM_AdvanceFrame@0x00439a15` → meta vtable slot 7; Ghidra comments + `CDSFlxFile.md` callsite table
32. ~~FLX inMemSize / seq timing audit~~ **done (R4)** — pack: 1/130 `inMemSize==0x470`; per-sprite total ms; DSM parallel path; Ghidra comments @ `0x00432b18` / `0x004399b0` / `0x0040eb30` ([round4_task_32_report.md](struct_recovery/round4_task_32_report.md)) [medium]
33. ~~FLX BindStream header dwords~~ **done** — file↔outer table @ `CDSFlxFile_BindStream@0x00432ac0`; `flx_file_format.md` + decompiler comment (agent todo 33)
34. CDSFont payload tail fields [high]
35. CDSImage MI Load/Save [high]
36. ~~CDSJpegImage factory +0x60~~ **done** — `CreateObject` @ `0x00432070` (`0x64`); `+0x60` quality `0x4b`; `CDSQueueStream` @ `0x0043c160` disambiguated (worker 36) [medium]
37. ~~CDSMpx libmad interior~~ **done** — recreated `mad_stream`/`mad_frame`/`mad_synth_bulanci`; `CDSMpx` embeds `stream`/`frame`/`synth`; libmad↔Ghidra name map in `CDSMpx.md` (worker 37) [medium]
38. ~~CDSMpxStream persistence overlay~~ **done** — `CDSMpxStream` 39112 B with `mpxFormatTail`/`pPayloadStream`/`dwPayloadBytes` @ +0x08..+0x3c; `CDSMpxPersistFacet` 0x38; `SaveMpxFile`/`LoadMpxFile`/`CreateFromHandle` prototypes (worker 38) [medium]
39. CDSException base struct [high]
40. ~~CDSSafeStream auxHeap + MI vs filter~~ **done** (R3+R4 todo 40) — `pAuxHeap` teardown-only (no alloc in binary); MI header `+0x00..+0x14` shared, body diverges @ `+0x18` (chain vs cursor); dual alloc `CreateFilterSafeStream`; `save_program` OK — [round4_task_40_report.md](struct_recovery/round4_task_40_report.md)
41. ~~CBulanek scheduler @ +0x88~~ **done** — `CBulanek.scheduler` `CDSUpdatedItem` @ +136; ctor/tick xrefs (agent todo 41)
42. CBulanek videoTrackManager @ +0xA8 [critical]
43. ~~CDSUpdatedItem IDSUpdated facet @ +0x00~~ **done** — `pVftable_IDSUpdated` @ embed +0; 29-ctor catalog; `CShot`/`CGame`/`CMenu` embeds typed (agent todo 43)
44. ~~CDSVideoPlayer CDSTrackVector @ +0x1c~~ **done** — R3 embed + helpers; R4 `InsertOrFindTrack` `CDSTrackVector*` (agent todo 44)
45. ~~CDSStreamStorage pack collection embed~~ **done** — `CDSCollection` @ +0x1c, `CDSChain` @ +0x34; `OpenPackStream` `CDSStreamStorage*` path (agent todo 45)
46. CDSStreamStorage IDSStorage stub audit [high]
47. ~~CDSStrmResInfo streamExtent @ +0x14~~ [medium] **done** (agent 47)
48. ~~ODSImage weapon namespace cleanup~~ [critical] — done (todo 48)
49. ~~CWeapon 0x70 Ghidra struct apply~~ **done** — `CWeapon` 112 B Ghidra struct; `trackManager` `CDSVideoPlayer` @ +8, `weaponKind` @ +0x64 (agent todo 49)
50. CDSWav class-43 vs bank overlay [high]
