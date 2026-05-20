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
* **Artefacts:** `./app_shell.md`

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
  * On-disk source of `seq[0x10]` (sequence-default duration in ms). The sequence pointer added to the track is `resource_handle + 4`, but a CBulPicture's `+0x14` field (the offset that becomes `seq[0x10]`) is uninitialized by `CBulPicture_Create` — it must be written later by the resource-pool wrapper that wraps a BitmapSprite blob into the 2-vftable sequence object returned by `(*g_pApp[0x70])->vfn[4](resId, 0)`.
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
