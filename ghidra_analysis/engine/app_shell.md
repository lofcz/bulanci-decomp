# `CDSApp` — application shell, WinMain, message loop

`CDSApp` is the engine's top-level "application" class. It owns
`WinMain`, the Win32 window, the WndProc, the modal-pump generator,
and the per-frame engine tick. The shipping derived class is
`CBulanci` (the Sleep Team game app — same short-name as the player
class; that ambiguity is in the original code base).

Everything below is anchored to addresses in `bulanci.exe`.

## CRT to first engine entry

The PE entry point goes through the standard MSVC8 CRT bootstrap, then
hands control to the engine's hand-rolled `WinMain` shape:

```
_mainCRTStartup     0x00448e40  ; entry point
  ___tmainCRTStartup 0x00448c60 ; CRT init
    WinMain          0x00402680 ; (hInst, hPrev, ansiCmdLine) -> int
      FUN_0042aa60   0x0042aa60 ; engine "run" with wide cmdline
```

`WinMain` (`FUN_00402680`) does only two things:

* converts the ANSI cmdline to UTF-16 via the ATL thread-ACP thunk
  (`PTR__AtlGetThreadACPThunk_004afbfc`),
* calls `FUN_0042aa60(hInstance, wideCmdline, &g_AppDescriptor)`
  where `g_AppDescriptor` is the static class-registration record at
  `0x004b3300`.

`FUN_0042aa60` is the actual engine top-level:

```c
int CDSApp_AppMain(HINSTANCE hInst, LPCWSTR cmdline, ClassReg* desc) {
    CoInitialize(NULL);
    (*(FactoryFn*)(desc + 0xc))();           // -> CBulanci_CreateObject (0x00402a90)
                                             //    sets g_pApp = newly-built CDSApp instance
    FUN_0042d490(&hLog, g_pApp + 0x1a);      // open a CDSChained logger (uses g_pApp.name field offset)
    g_pHInstance = hInst;
    (*g_pApp->vtbl[31])(cmdline);            // SetCmdLine  (purecall - derived must override)
    (*g_pApp->vtbl[28])();                   // CDSApp_OnCreate
    (*g_pApp->vtbl[29])();                   // CDSApp_Run
    (*g_pApp->vtbl[2])();                    // CDSApp_dtor
    CoUninitialize();
    return 0;
}
```

The "CDSApp + CBulanci-app" object is constructed by `operator new`
(0x4cc bytes) + `CBulanci::FUN_004026f0` (the derived ctor), which in
turn chains to the base ctor `FUN_0042b170(this, L"Bulanci",
L"Software\\SleepTeam\\Bulanci")`.

## App descriptor `g_AppDescriptor @ 0x004b3300`

`g_AppDescriptor` is a CDS class-registration record. It is filled at
load time by the C++ static initializer `FUN_0047aff0`:

```c
HandleClassRegister(&g_AppDescriptor,
                    /*classId*/ 2000,
                    /*table*/   0x004b3b20,     // returned by vtbl[0] (0x0042b130)
                    /*factory*/ 0x00402a90);    // CBulanci::CreateObject
atexit(&DAT_0047de40);
```

Layout (matches every other CDS class registration used by the resource
container — same `HandleClassRegister` is the universal hook):

| offset | size | meaning |
|--------|------|---------|
| `+0x00` | dword | classId = `2000` |
| `+0x04` | dword | class lookup table pointer = `0x004b3b20` |
| `+0x08` | dword | (reserved) |
| `+0x0c` | dword | factory function pointer = `0x00402a90` |

`(*desc->factory)()` calls `CBulanci::CreateObject`:

```c
void* CBulanci::CreateObject(void) {       // 0x00402a90
    void* p = operator_new(0x4cc);          // 0x00447c42
    return CBulanci::FUN_004026f0(p);       // derived ctor
}
```

The constructor stores the resulting `this` pointer into both
`DAT_004b3b88` (active app) and `DAT_004b3b90` (head of input-recipient
chain) — see "Globals" below.

## Primary vftable `CDSApp::vftable @ 0x0048700c`

The class has multi-inheritance (4 vftable slots inside the object at
offsets `+0x00`, `+0x04`, `+0x10`, `+0x18`; the corresponding 4 vftables
in `.rdata` are at `0x0048700c`, `0x00486fec`, `0x00486fd4`,
`0x00486fc0`). Only the **primary** table is mapped here; the rest are
narrow MI thunks and not used by the message-loop machinery.

RTTI complete-object-locator sits at `0x00487008` (=
`vftable - 4`) and points at `0x004a3d04`.

Vtable length: 34 entries. Confirmed by the trailing `L"%ld\0%s"`
literal which starts immediately at `0x00487094`.

| slot | offset | target (Ghidra label) | semantics |
|-----:|-------:|--------|--------------|
| 0  | `+0x00` | `CDSApp_GetClassTable` (`0x0042b130`) | `return &g_AppClassTable;` |
| 1  | `+0x04` | `CDSApp_DtorScalar` (`0x0042b980`) | scalar-deleting dtor (calls slot 2 then conditional `_free(this)`) |
| 2  | `+0x08` | `CDSApp_dtor` (`0x0042b560`) | destructor body: `DestroyWindow(g_pHwnd); FUN_0042add0; FUN_0042f530 (drain DS); base::~()` |
| 3  | `+0x0c` | `0x0042c2e0` | base: returns `sum(child->vtbl[3]())` over `+0x54` child list — "GetWeight"-style accumulator |
| 4  | `+0x10` | `0x0042c320` | base: walks children, calling `child->vtbl[4](cursor); cursor += child->vtbl[3]()` — paired with slot 3; doubles as the "release prior child" hook used by `CDSApp_RenderFrame` |
| 5  | `+0x14` | `0x0042c370` | base: same shape as slot 4 but invokes `child->vtbl[5]` — mirror operation; doubles as the "activate now-current child" hook used by `CDSApp_RenderFrame` |
| 6  | `+0x18` | `0x004033a0` | `return 0;` — engine-default predicate |
| 7  | `+0x1c` | `CDSView_HitTest` (`0x004028a0`) | `return inside(this->bounds[+0x30..+0x3c], pt);` |
| 8  | `+0x20` | `CDSApp_AdaptDisplaySize` (`0x0042cae0`) | resize-on-enter-modal: queries slot 12 for parent dims, slot 11 to apply |
| 9  | `+0x24` | `CDSView_Invalidate` (`0x0042bd70`) | mark dirty if drawable, default rect = `this+0x20..+0x2c` |
| 10 | `+0x28` | `CDSView_ComputeAnchoredRect` (`0x0042c580`) | given the parent's resize delta `param_2[2]`, fold this view's anchor bits `(this[0x48] & 0x0F)` into a fresh rect; "WinForms anchor" reflow |
| 11 | `+0x2c` | `CDSView_SetRect` (`0x0042c480`) | assigns `this->bounds = param_1[4]` and broadcasts slot 10 / slot 8 to children |
| 12 | `+0x30` | `CDSView_GetParentBounds` (`0x0042c430`) | outputs `(0,0)` + `parent->bounds.size` (or `INT_MAX` if no parent) |
| 13 | `+0x34` | `CDSView_IsModalDoneRecursive` (`0x0042c3e0`) | walks children's slot 13 — any child returning 0 means "still modal" |
| 14 | `+0x38` | `0x0042ccf0` | per-dirty-rect render callback invoked by `CDSApp_RenderFrame` |
| 15..18 | `+0x3c..+0x48` | `purecall` (`0x00438340`) | unused-in-base; reserved for derived overrides |
| 19 | `+0x4c` | `CDSView_OnFocus` (`0x0042cf50`) | per-event hook fired by mouse-down (kind 0x10) in `CDSApp_DispatchInputEvent`; base impl is a one-line thunk to `CDSView_AcquireKeyboardFocus` (click-to-focus) |
| 20 | `+0x50` | `0x00416770` | overridden by `CBulanci` (game audio hook?) |
| 21 | `+0x54` | `0x00416770` | overridden by `CBulanci` (same body) |
| 22 | `+0x58` | `CDSView_OnKeyDown` (`0x0042a000`) | give focus chance to consume; magic ALT+`X` posts close-event (`0x8004`) via `FUN_0042c3c0` |
| 23 | `+0x5c` | `CDSView_OnKeyUp` (`0x0042c0e0`) | bubble up the `this[0x50]` chain if parent `flags2 & 2` set |
| 24 | `+0x60` | `CDSView_OnChar` (`0x0042c100`) | bubble up the `this[0x50]` chain if parent `flags2 & 4` set |
| 25 | `+0x64` | `0x0042c770` | broadcast `{type=0x200, code, x, y}` event to each child's input chain head `+0x10` |
| 26 | `+0x68` | `0x0042c7d0` | top-bit-gated `if (code & 0x8000) FUN_0042c3c0(this, code)` — synthetic-close-event router |
| 27 | `+0x6c` | `CDSView_EmptyHook27` (`0x00438f80`) | empty stub |
| 28 | `+0x70` | `CDSApp_OnCreate` (`0x0042a210`) | **OnCreate** — RegisterClassExW + CreateWindowExW + DirectSound boot + clock init |
| 29 | `+0x74` | `CDSApp_Run` (`0x00429d60`) | **Run** — modal pump entry (`CDSView_DoModal(this, NULL)`) |
| 30 | `+0x78` | `CDSApp_PreCreateHook` (`0x00467430`) | empty hook (one-byte `RET`) |
| 31 | `+0x7c` | `purecall` (`0x00438340`) | **SetCmdLine** — purecall (derived class must override) |
| 32 | `+0x80` | `CDSApp_WndProcDispatch` (`0x0042a660`) | **WndProc dispatcher** — drives the WM_* switch (see below) |
| 33 | `+0x84` | `CDSApp_OnActivateApp` (`0x00429d00`) | invoked from `WM_ACTIVATEAPP`; flips drawable flag, posts `0x400/8` (gain) or `0x400/9` (lose) to modal focus |

In the shipping `CBulanci`-app vtable, slots 0..14 are reused as-is and
slots 20..27 get game-specific overrides; the entry-point quartet (28,
29, 31, 32) is the always-same engine plumbing documented here.

## `OnCreate` — `FUN_0042a210` (vtbl[28])

```c
void CDSApp_OnCreate(this) {
    (*this->vtbl[30])();                       // CDSApp_PreCreateHook (empty in base)
    RegisterClassExW(&wc {
        .lpfnWndProc   = CDSApp_WndProc,
        .hInstance     = g_pHInstance,
        .hbrBackground = GetStockObject(BLACK_BRUSH),
        .lpszClassName = this->classname        // L"Bulanci" or fallback PTR_DAT_004afca0
    });
    g_pHwnd = CreateWindowExW(0, classname, classname,
                              WS_POPUP,
                              0, 0,
                              this->rect.right  - this->rect.left,    // 800
                              this->rect.bottom - this->rect.top,     // 600
                              NULL, NULL, g_pHInstance, NULL);
    SetFocus(g_pHwnd);
    CBulanci::FUN_00429990(this, ...);          // game-side post-create (windowed flag)
    CDSDirectSound_InitPrimary(this + 0x80,     // 2ch / 22050 Hz / 16 bpp primary buffer
                               g_pHwnd, 2, 22050, 16);
    CDSApp_InitClock();                          // g_dwStartMs = timeGetTime()
    CDSView_SetModalEligible(this, 1);           // push into modal stack
    CDSView_SetActive(this, 1);                  // mark active
}
```

`FUN_0042bed0` and `FUN_0042c290` are recursive helpers — they also
flip the same bits on every child view threaded through `this+0x54`
(walked via `+0x4c` next-pointers).

## `Run` — `FUN_00429d60` (vtbl[29])

The actual top-level "run the game" method is a 3-line forwarder:

```c
int CDSApp_Run(this) {
    int exitCode = CDSView_DoModal(this, NULL);  // modal pump
    CDSView_SetActive(this, 0);                  // mark inactive
    CDSView_SetModalEligible(this, 0);           // pop from modal stack
    return exitCode;
}
```

The whole pump body lives in **`FUN_0042d1a0`** which doubles as the
generic "open a modal sub-frame and pump until it exits" primitive
called from every CDS dialog. When invoked with `parent = NULL` it
just means "this is the root frame".

```c
short CDSView_DoModal(this, parent) {
    if (g_pApp == NULL)        return 0x8003;     // not initialised
    if (parent && (parent->flags1 & 0x80) == 0) FUN_0042d0b0(parent, this, 0);

    int* savedFocus = this->parent ? this->parent->focusChain : NULL;
    int  wasActive  = this->flags1 & 1;

    g_pModalFocus = this;                          // become the keyboard-focus target
    CDSView_AcquireKeyboardFocus(this);
    this->flags1   |= 0x10;                        // visible
    if (!wasActive) FUN_0042c990(this);
    (*this->vtbl[8])(0);                           // CDSApp_AdaptDisplaySize

    do {
        this->exitCode = 0;
        do {
            CDSApp_PumpTick(g_pApp);
        } while (this->exitCode == 0);
    } while ((this->flags2 & 0x40) &&              // "stay modal" eligibility
             (*this->vtbl[13])(this->exitCode) == 0);

    (*this->vtbl[9])(0, 0);                        // CDSView_Invalidate (final repaint)
    if (!wasActive) FUN_0042d040(this);
    g_pModalFocus = savedFocus;
    this->flags1 &= ~0x10;
    if (savedFocus) CDSView_AcquireKeyboardFocus(savedFocus);
    return this->exitCode;
}
```

Key state:

* `this+0x44` / `this+0x46` — two `uint16` flag words (see "Globals" /
  "Object fields" sections below).
* `this+0x4a` — `uint16` exit code that ends the inner loop. Written by
  `PostQuitMessage` translation, by dialog "OK/Cancel" exits, and by
  the synthetic `WM_CLOSE`-to-focus path.
* `this+0x4c` — pointer to the parent modal frame for nesting.

## Pump tick — `FUN_0042be60`

This is the per-iteration body of the inner modal loop:

```c
void CDSApp_PumpTick(CDSApp* g_pApp) {
    while (CDSApp_PollEventQueue() == 0) {       // engine event queue empty?
        CDSApp_FrameBody(g_pApp);                //   run a full frame
    }
    CDSApp_DispatchOneEvent();                   // drain one engine event
}
```

* `CDSApp_PollEventQueue` — peeks `g_pEventQueue` at `0x004b7bf0`;
  returns 1 if there's still work pending after walking the queue's
  stale entries with `FUN_0042ee20` / `FUN_0042ed70`, else 0.
* `CDSApp_DispatchOneEvent` — pulls one event from `g_pEventQueue`,
  then dispatches via `(*evt->vtbl[4])(evt)` (slot offset `+0x10`).
* `CDSApp_FrameBody` — the frame body (next section).

The shape — "run frames while no events pending, otherwise eat one
event" — means events take strict priority over rendering. That is how
dialog "OK" presses, network packets and similar can short-circuit the
render path.

## Frame body — `FUN_0042bda0`

```c
void CDSApp_FrameBody(CDSApp* g_pApp) {
    CDSApp_UpdateClock();                          // g_dwElapsedMs = timeGetTime() - g_dwStartMs
    CDSApp_PulseTasks();                           // pulse g_pTaskList

    if (CDSApp_PollEventQueue() == 0) {            // still no engine events?
        MSG msg;
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            while (msg.message != WM_QUIT) {       // 0x12
                TranslateMessage(&msg);
                DispatchMessageW(&msg);            // -> CDSApp_WndProc -> CDSApp_WndProcDispatch
                if (!PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                    CDSApp_RenderFrame(g_pApp);    // render before yielding
                    return;
                }
            }
            // WM_QUIT seen - synthesize "close" event to the active modal frame
            void* focus = g_pModalFocus ? ((char*)g_pModalFocus + 0x10) : NULL;
            FUN_0042f590(focus, 0x100, 0x8004, 0, 0);
        }
    }
    CDSApp_RenderFrame(g_pApp);
}
```

`g_pModalFocus + 0x10` is the modal frame's CDSChain head — the synthetic
event with `(type=0x100, code=0x8004, ...)` is the same shape the
WndProc emits for `WM_CLOSE` (see below), so the frame's "close
intent" path is funnelled through a single handler regardless of
whether the user pressed `[X]` or the OS sent `WM_QUIT`.

`CDSApp_RenderFrame` is the **modal-frame render**:

* bails if `this[0x274]` (the "drawable" bool — set to `1` by the
  constructor) is `0`,
* otherwise iterates `this->dirtyRects[]` (count `this+0x25c`, base
  `this+0x254`, 16 bytes per rect: `int32 left, top, right, bottom`),
  calling `(*this->vtbl[14])` per non-degenerate rect,
* swaps in the pending child view (`this+0xec -> this+0xe8`) with
  ref-count adjustments,
* calls `FUN_00429930(this + 0x7c)` to page-flip the embedded
  `CDSBackBuffer` subobject.

`this+0x7c` (and `+0x80`) hold the **CDSBackBuffer** vtables —
`CDSBackBuffer::vftable @ 0x00486fb0` / `0x00486f9c` — installed by the
constructor. That is the only place `CDSBackBuffer` is embedded; every
other piece of the renderer talks to the back buffer through this
sub-object.

## WndProc

`CDSApp_WndProc @ 0x00429c00` is the procedure registered with Win32:

```c
LRESULT CDSApp_WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_pApp == NULL) return 1;
    int handled = (*g_pApp->vtbl[32])(msg, wp, lp);     // CDSApp_WndProcDispatch
    return handled ? 0 : DefWindowProcW(hwnd, msg, wp, lp);
}
```

The dispatcher behind `vtbl[32]` (`CDSApp_WndProcDispatch`) is the only
thing that knows about WM_* codes. It funnels keyboard messages into
`CDSApp_KeybQueue(eventType, vk, alt)` and mouse messages into
`CDSApp_MouseQueue(buttonMask, transitionFlags, lParam)`:

| WM_*           | hex   | handler |
|----------------|-------|---------|
| WM_DESTROY     | 0x02  | `CDSApp_OnDestroy(this); g_pHwnd = NULL; PostQuitMessage(0)` |
| WM_PAINT       | 0x0F  | if `this[0x274]` set: `(*this->vtbl[9])(this+0x20, 1)` (i.e. `CDSView_Invalidate`) |
| WM_CLOSE       | 0x10  | synthetic `(0x100, 0x8004, 0, 0)` to `g_pModalFocus+0x10` |
| WM_ACTIVATEAPP | 0x1C  | `CDSApp_OnActivateApp(this, wParam != 0)` via `vtbl[33]` |
| WM_SETCURSOR   | 0x20  | `SetCursor(NULL); return 1` (engine draws its own cursor) |
| WM_KEYDOWN     | 0x100 | `CDSApp_KeybQueue(1, vk, 0)` |
| WM_KEYUP       | 0x101 | `CDSApp_KeybQueue(2, vk, 0)` |
| WM_CHAR        | 0x102 | `CDSApp_KeybQueue(4, ch, 0)` |
| WM_SYSKEYDOWN  | 0x104 | `CDSApp_KeybQueue(1, vk, 1)` |
| WM_SYSKEYUP    | 0x105 | if vk == VK_RETURN: **ALT+ENTER toggles fullscreen** via `CDSApp_SetWindowed(this, !this->windowed)`; else `CDSApp_KeybQueue(2, vk, 1)` |
| WM_MOUSEMOVE   | 0x200 | `CDSApp_MouseQueue(0x08, 0,           lParam)` |
| WM_LBUTTONDOWN | 0x201 | `CDSApp_MouseQueue(0x10, mk\|1, lParam)`    — kind 0x10 = DOWN, btn 1 = L |
| WM_LBUTTONUP   | 0x202 | `CDSApp_MouseQueue(0x20, mk\|1, lParam)`    — kind 0x20 = UP |
| WM_LBUTTONDBLCLK | 0x203 | `CDSApp_MouseQueue(0x40, mk\|1, lParam)`  — kind 0x40 = DBLCLK |
| WM_RBUTTONDOWN | 0x204 | `CDSApp_MouseQueue(0x10, mk\|2, lParam)`    — btn 2 = R |
| WM_RBUTTONUP   | 0x205 | `CDSApp_MouseQueue(0x20, mk\|2, lParam)` |
| WM_RBUTTONDBLCLK | 0x206 | `CDSApp_MouseQueue(0x40, mk\|2, lParam)` |
| anything else  |       | return 1 → fall through to `DefWindowProcW` |

The `(0x10, 0x20, 0x40)` event-type bits (DOWN / UP / DBLCLK) and the
`(0x08)` move-bit are exactly what `CDSApp_DispatchInputEvent`
(handles each event after it pops back off the queue) tests for;
see below. `WM_MBUTTON*` is absent — the WndProc does not publish
middle-button events at all.

## Input dispatch — `CDSApp_DispatchInputEvent`

`CDSApp_MouseQueue` and `CDSApp_KeybQueue` only stuff events into a CDS
chained event list (`FUN_0042f3e0` at `this+0x10`). When the pump
dequeues one of those events its callback eventually reaches
**`CDSApp_DispatchInputEvent @ 0x00429db0`** which routes a normalised
input event to its owner widget:

```c
void CDSApp_DispatchInputEvent(this, Event* ev) {
    ushort kind = ev->flags;                                    // ev+4
    if ((kind & 0x78) == 0) {                                   // keyboard branch
        // 0x01=down 0x02=up 0x04=char  (set by KeybQueue: type<<0)
        if ((kind & 7) && g_pModalFocus &&
            (g_pModalFocus->flags2 & kind)) {
            if (kind == 1) { /* set per-VK bit in this[ev->vk + 0xf0]; (*g_pModalFocus->vtbl[22])(vk, alt); */ }    // CDSView_OnKeyDown
            else if (kind == 2) { /* clear bit; (*g_pModalFocus->vtbl[23])(vk, alt); */ }                            // CDSView_OnKeyUp
            else if (kind == 4) { /* (*g_pModalFocus->vtbl[24])(); */ }                                              // CDSView_OnChar
            return;
        }
        CDSView::FUN_0042c040((CDSView*)this, ev);              // fall-through to view base
    } else {                                                    // mouse branch
        this->mouseX = ev->x;        // this+0xe0
        this->mouseY = ev->y;        // this+0xe4
        if (kind & 0x08) {                                       // MOVE
            if (g_pModalFocus) FUN_0042c880(g_pModalFocus);
            if (g_pInputChainHead && (g_pInputChainHead->flags2 & 0x08))
                (*g_pInputChainHead->vtbl[16])(ev+0xc);          // OnMouseMove
        }
        if (g_pInputChainHead) {
            for (v = g_pInputChainHead; v && (kind & 0x10) && !(v->flags2 & 0x10); v = v->next) {}
            if (v) (*v->vtbl[19])(ev+0xc, ev->buttonFlags);      // OnMouseDown   (CDSView_OnFocus is the base impl)
            for (v = g_pInputChainHead; v && (kind & 0x20) && !(v->flags2 & 0x20); v = v->next) {}
            if (v) (*v->vtbl[20])(ev+0xc, ev->buttonFlags);      // OnMouseUp
            for (v = g_pInputChainHead; v && (kind & 0x40) && !(v->flags2 & 0x40); v = v->next) {}
            if (v) (*v->vtbl[21])(ev+0xc, ev->buttonFlags);      // OnMouseDblClk
        }
    }
}
```

So:

* `g_pModalFocus` is the **keyboard focus** target (set by the modal
  pump on entry, restored on exit).
* `g_pInputChainHead` is the **mouse-recipient chain head** — walked
  via `next = next->chainLink` until one whose `flags2` has the right
  bit set is found.
* Each view advertises which event kinds it accepts via the
  `flags2 = +0x46` byte (bits 0x08 / 0x10 / 0x20 / 0x40 = mouse move /
  down / up / dblclk; bits 0x01 / 0x02 / 0x04 = key down / up / char).
  Buttons are distinguished by the per-event `buttonFlags` byte
  (bit 1 = L, bit 2 = R; the WndProc never sets a "middle"
  button bit).

Mouse coordinates are stretched into device space first inside
`CDSApp_MouseQueue` — when `this->windowed (+0xe4)` is true, the
(LPARAM x,y) pair is rescaled from the physical client size
(`+0xcc..+0xd8`) into the logical app rect (`+0x20..+0x2c`).

## Object fields (CDSApp portion, 0x4cc total for the shipping
derived class)

Only the fields the message-loop / WndProc / render path actually
touches are pinned; the rest of the layout is in flight.

| offset | size | what |
|-------:|-----:|------|
| `+0x00` | 4 | primary vptr (`CDSApp::vftable`) |
| `+0x04` | 4 | secondary vptr 1 (`0x00486fec`) |
| `+0x10` | 4 | secondary vptr 2 (`0x00486fd4`) |
| `+0x18` | 4 | secondary vptr 3 (`0x00486fc0`) |
| `+0x20`..`+0x2c` | 16 | logical app rect: `int32 left, top, right, bottom` (default 0,0,800,600) |
| `+0x44` | 2 | `flags1` ushort: 0x08 mouse-move-default, 0x10 visible, 0x40 in-modal-stack (set by `FUN_0042bed0`), 0x80 active (set by `FUN_0042c290`) |
| `+0x46` | 2 | `flags2` ushort: 0x01/0x02/0x04 = key-down/-up/-char eligibility, 0x08/0x10/0x20/0x40 = mouse-move / mouse-down / mouse-up / mouse-dblclk eligibility (init = 0x43 = key-down + key-up + mouse-down), additional 0x10 set by derived ctor |
| `+0x4a` | 2 | modal-loop exit code |
| `+0x4c` | 4 | parent modal frame |
| `+0x54` | 4 | child-views list head (CDSChain) |
| `+0x68` | 4 | window title `LPCWSTR` (= `L"Bulanci"`) |
| `+0x6c` | 4 | registry path `LPCWSTR` (= `L"Software\\SleepTeam\\Bulanci"`) |
| `+0x7c` | 4 | `CDSBackBuffer::vftable` (subobject) |
| `+0x80` | 4 | `CDSBackBuffer::vftable` (subobject, second vptr) |
| `+0xc4` | 4 | default bpp (= 8) |
| `+0xcc`..`+0xd8` | 16 | physical DirectDraw rect (for mouse rescale) |
| `+0xdc` | 4 | (init = 7) |
| `+0xe0`..`+0xe4` | 8 | last mouse (x, y) in logical coords |
| `+0xe4` | 1 | `windowed` bool — read from registry `Windowed` value under `HKLM\Software\SleepTeam\Bulanci` |
| `+0xe8` | 4 | current child view |
| `+0xec` | 4 | pending child view (swap-in target) |
| `+0xf0`..`+0xff` | 16 | per-VK key-down bitmap (`this[vk + 0xf0] |= bit`) |
| `+0x254` | 4 | dirty-rect array base |
| `+0x25c` | 4 | dirty-rect count |
| `+0x274` | 1 | `drawable` bool (init = 1) |
| `+0x275` | 1 | `dirtyDuringFrame` bool |

## Globals

| address | Ghidra label | type | what |
|---------|--------------|------|------|
| `0x004b3300` | `g_AppDescriptor`   | `ClassReg`     | `HandleClassRegister(_, 2000, &g_AppClassTable, &CBulanci_CreateObject)` (filled by `CBulanci_RegisterAppDescriptor`, the C++ static init) |
| `0x004b3b18` | `g_pHInstance`      | `HINSTANCE *`  | written by `CDSApp_AppMain` |
| `0x004b3b1c` | `g_pHwnd`           | `HWND *`       | written by `CDSApp_OnCreate`, cleared on `WM_DESTROY` |
| `0x004b3b20` | `g_AppClassTable`   | (anchor)       | "class lookup table" anchor returned by vtbl[0] (`CDSApp_GetClassTable`) |
| `0x004b3b88` | `g_pApp`            | `void *`       | active `CDSApp`/`CBulanci` instance |
| `0x004b3b8c` | `g_pModalFocus`     | `void *`       | current modal frame / keyboard-focus target — set on entry to `CDSView_DoModal` |
| `0x004b3b90` | `g_pInputChainHead` | `void *`       | head of mouse-recipient chain; defaults to `g_pApp` |
| `0x004b3b9c..a8` | — | `int[4]`            | rectangle being drawn this frame (clobbered each call to `CDSApp_RenderFrame`) |
| `0x004b3bd8` | `g_dwElapsedMs`     | `DWORD`        | `timeGetTime() - g_dwStartMs`, refreshed at top of every frame by `CDSApp_UpdateClock` |
| `0x004b3bdc` | `g_dwStartMs`       | `DWORD`        | `timeGetTime()` captured by `CDSApp_InitClock` at end of `CDSApp_OnCreate` |
| `0x004b7be8` | `g_pTaskList`       | `void *`       | task / animation list pulsed by `CDSApp_PulseTasks` once per frame |
| `0x004b7bf0` | `g_pEventQueue`     | `void *`       | engine event queue probed by `CDSApp_PollEventQueue` / drained by `CDSApp_DispatchOneEvent` |
| `0x0048700c` | `g_pCDSApp_vftable` | `void *[34]`   | primary CDSApp vftable (slot map above) |

## What's still open

* Slot 3 / 4 / 5 semantics — base implementations are pure
  "walk child list" templates, so the per-class meaning is whatever
  the derived class plugs in. The decompiler-visible pattern is
  "slot 3 returns a `size_t`, slot 4 / 5 act on a cursor advanced by
  that size", which is consistent with `(GetSize, Save, Load)` or
  `(GetWeight, Distribute, Collect)` shapes — but a confirming
  derived-class override (e.g. on `CMenu` or `CStartGame2`) hasn't
  been read yet.
* Slot 14 (`0x0042ccf0`) — the per-dirty-rect render callback. 320 B,
  the largest virtual on the table; only spot-checked so far. Pinning
  it would close the render-loop story.
* Slot 20 / 21 — both `0x00416770` (one body, registered twice in this
  base table). Reads like an audio-related game override that
  shouldn't be in `CDSApp::vftable`; very likely a propagation
  artefact from `CBulanci`'s namespace claim.
* `CBulanci`-specific overrides for the base slots in the **derived**
  `CBulanci::vftable` (referenced by the four `CDSApp::vftable` writes
  in `CBulanci_ctor`'s prologue — Ghidra collapses them under the
  same display symbol, so I haven't enumerated the per-subobject
  override deltas yet).
* Layout of `g_AppDescriptor` past `+0x0c` (`HandleClassRegister`'s
  signature is generic — it's the same one used by the
  `ClassID 52 / 2000 / ...` resource factories, but the in-memory
  record shape after `+0x0c` is not in this artefact yet).
* `KeybQueue` (`CDSApp_KeybQueue`) and `MouseQueue` (`CDSApp_MouseQueue`)
  both push into a CDS chain at `this+0x10`; the chain's pop side is
  implicit in `CDSApp_PollEventQueue` / `CDSApp_DispatchOneEvent`
  but the exact wakeup contract has not been fully traced.
