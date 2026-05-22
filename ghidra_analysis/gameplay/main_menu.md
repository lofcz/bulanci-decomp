# Main menu

Reverse-engineered from `bulanci.exe`. Covers the boot → splash → main
menu → game-setup chain that lives in classes `CBulanci` (the
state-machine driver), `CMenu` (the visible main menu), `CStartGame1`
(game-mode select), `CStartGame2` (lobby / team setup), plus the
satellite dialogs (`CAdvertising`, `CHistoryDlg`, `CExitDlg`,
`CTcpIpConfig`, `CSessionList`, `CMsgDialog`).

Everything below is grounded in a specific `bulanci.exe` address so a
faithful re-implementation can be built side-by-side with the
disassembly.

## 1. Boot to menu — state machine

The whole menu is event-driven through `CBulanci`'s embedded
`CDSEventHandler` at `this+0x10`. Events are scheduled with
`Scheduler_PostMessage` (`FUN_0042f590`/`FUN_0042f390`) and dispatched
through `CBulanci`'s vtable-22 `OnEvent` slot, which is
`CBulanci::OnEvent @ 0x00402490`.

```
WinMain (0x00402680)
  -> CDSApp_AppMain (0x0042aa60)
      -> CBulanci_CreateObject (0x00402a90, malloc 0x4cc + ctor)
      -> CBulanci_ctor (0x004026f0)
          - CDSApp_ctor("Bulanci", L"Software\\SleepTeam\\Bulanci")
          - sets primary vftable 0x0047f834 (CBulanci primary)
          - rect 0,0,800,600
      -> SetCmdLine (vtbl[31] — purecall stub, never reached)
      -> CBulanci_OnCreate (vtbl[28] = 0x00402b20)
          a. FUN_0042e8b0/FUN_0042e730/FUN_004477df — CRT init helpers
          b. CDSApp::CDSApp_OnCreate (0x0042a210):
               1. (this->vtbl[30])() = CBulanci::InitResourceBank
                  (0x00402180) — opens the EXE's overlay resource pool,
                  stores at this+0x70, creates ODSImage at this+0x__
                  via ODSImage::FUN_00426060 (size 0x218).
               2. RegisterClassExW + CreateWindowExW (Win32 window)
               3. CBulanci::CDSApp_InitDirectDraw — DirectDrawCreate + cooperate
               4. CDSDirectSound_InitPrimary at this+0x80
               5. CDSApp_InitClock (g_dwStartMs = timeGetTime())
               6. CDSView_SetModalEligible(1), CDSView_SetActive(1)
          c. CBulanci::FUN_00402990 — **BitmapSpecial cache table**
             (NOT an audio bank as previously documented). Builds an
             8-byte slot-table object whose slot array (sized
             `N * sizeof(void*)`) is populated from the resource-ID
             table at `0x004ae008` (0x6e = 110 IDs). Each ID is looked
             up via the resource pool's `Lookup` virtual, downcast to
             interfaces 0xf and 0xe (refcount adds), and cached. The
             table is stored at `this+0x4b0`. All 110 IDs back
             ClassID-28 `BitmapSpecial` resources — these are the
             pre-cached UI bitmaps (button states, day/night
             backgrounds, `CRuch` gradients, dialog frames, etc.) the
             menu and its sub-screens pull from synchronously by index.
          d. **AudioBankIndex pool** (the real audio bank). Loads
             resource `0x10004` (`CDSAudioBankIndex`, class 67) and
             stores its handle at `this+0x4c0`. Its on-wire format is
             `u32 bankResourceID + u32 reserved + u32 sampleCount +
             u32[] byteLengths`; resolved here, that's bank `0x10152`
             (`CDSAudioBank`, class 43) carrying 41 mono 16-bit PCM
             samples at 22050 Hz. Slot indices 0-40 are looked up by
             `TriggerBankSample` via `*(slot_table + 0x18 + idx*4)` to
             build short-lived `CDSAudioPlayer` instances. The five
             menu UI cues live in slot 0x18..0x1c — see §10. The
             previous "Loads resource 0x10004 (icon? cursor?)" note
             was wrong.
          e. FUN_0042a330(this, 800, 600, 6, 1) — adapt display mode
          f. Scheduler_PostMessage(this+0x10, 0x100, 0xf7,
                                   delay_ms=0, immediate=0)
             — schedules event 0xf7 to fire on the next message-pump
             tick. This kicks off the menu state machine.
      -> CDSApp_Run (vtbl[29] = 0x00429d60)
          = CDSView_DoModal(this, NULL) — generic modal pump.
```

`CDSView_DoModal` then loops calling the per-frame
`CDSApp_PumpTick`/`CDSApp_FrameBody` (documented in `../engine/app_shell.md`).
Inside that loop, every pending Scheduler event is delivered to
`CBulanci::OnEvent (0x00402490)`.

`CBulanci::OnEvent (param_1 = cmdId, param_2 = optional payload)` is
the **top-level menu state machine**. Three event IDs are handled
directly; everything else falls through to `CDSApp::OnEvent_Default
(0x0042c7d0)`.

| Event | Action |
|-------|--------|
| `0xf7` | **Show splash.** Builds a `CAdvertising` (`FUN_0040e5f0`) on the stack, calls `FUN_0040fe30(splash, 0x1013a)` to drop the IWANNAPLAY.COM logo bitmap centred on a white CBlackView, runs `CDSView_DoModal(splash, this)`, then schedules `0xcc` in 256 ms. |
| `0xcc` | **Show main menu.** `this+0x4ac = 1` (menu-active flag). Allocates a `0xe4`-byte `CMenu`, calls **`CMenu_ctor @ 0x004265e0`** (`this, displayDirty=param_2, lastSplashFlag=this[0x4c8]`), stores in `this+0x280`, then `CDSView_DoModal(menu, this)`. On return: clear modal-active flag and interpret the modal exit code (table below). |
| `0xcd` | **Exit confirmation re-arm.** Calls `FUN_00401be0` — checks "was the exit-confirm dialog accepted?". If accepted: just return (caller continues to shut down). If not: schedule another `0xcc` so the user lands back in the main menu. |

Exit codes from `CDSView_DoModal(menu, ...)` in `OnEvent::0xcc`:

| Code | Meaning |
|------|---------|
| `-0x7ffc` | "User pressed Quit / X". Fall through to `_Globals::FUN_0042c3c0(this, 0x8004)` which posts `WM_QUIT`-equivalent and shuts the app down. |
| `-0x7f38` | "Background re-show". Schedule event `0xcd` (re-confirm) in 256 ms. |
| `-0x7f35` | Same shutdown path as `-0x7ffc`. |
| anything else | Menu was simply dismissed — return to message pump, no follow-up. |

Globals on `CBulanci`:

| Offset | Field |
|--------|-------|
| `+0x10` | `CDSEventHandler` subobject (where Scheduler events land) |
| `+0x70` | Resource-pool pointer (set in `InitResourceBank`). All resource lookups in the menu go through `(*g_pApp[0x70])->vfn[4](resId, 0)`. |
| `+0x80` | Embedded `CDSDirectSound` (per `../engine/app_shell.md`) |
| `+0x130` | App icon / cursor resource (set in `OnCreate`) |
| `+0x280` | Current menu instance (`CMenu*`, valid only between `0xcc` push and modal return) |
| `+0x284` | Embedded `CDSObject` track-manager (returned by `CStartGame1::FUN_00401280 → g_pApp+0x284`) |
| `+0x2b5` | Set up in `InitResourceBank` via `FUN_0040a0b0` — secondary resource pool? |
| `+0x2e6` | Returned by `CMenu::FUN_00401050` — small per-app substruct |
| `+0x306` | Selected history page index — read by `CHistoryDlg::FUN_00422f70` after the user picks an item |
| `+0x4ac` | `menu-modal-active` flag (set during the `0xcc` handler) |
| `+0x4c8` | `lastSplashShown` — passed to `CMenu_ctor` so the menu knows whether to do its first-time intro animation |

## 2. `CMenu` — the main menu screen

* **RTTI:** `.?AVCMenu@@` @ `0x004af894`
* **ClassID** (in CDS engine class registry): `2004` (`0x7d4`)
* **Singleton storage:** `g_pCMenuRegistry @ 0x004b3984` (filled by
  `HandleClassRegister(_, 2004, ?, &CMenu_CreateObject)` from the
  static-init record at `0x0047c14c`).
* **Factory:** `CMenu_CreateObject @ 0x004251d0` — `malloc(0xe4)` then
  empty `CMenu_ctor @ 0x00424520` (no UI yet — just clears state).
* **UI builder:** `CMenu_ctor_WithUI @ 0x004265e0(this,
  displayDirtyFlag, lastSplashFlag)` — this is the one
  `CBulanci::OnEvent::0xcc` calls. Everything below lives here.

### 2.1 Object layout (`sizeof = 0xe4`)

| Offset | Field | Notes |
|--------|-------|-------|
| `+0x00` | `vftable` primary (`0x004836b4`) | CDSChained-shape, 24 slots; slot 14 = Tick, 15-18 purecall, 19/22/23/24 = CDSView input hooks |
| `+0x04` | `vftable` IDSChained subobject (`0x00483694`) |
| `+0x10` | `vftable` IDSEventHandler subobject (`0x0048367c`) |
| `+0x14` | flags1 (set to `0x77f` here) |
| `+0x18` | `vftable` IDSReferenced subobject (`0x00483668`) |
| `+0x20..0x2c` | logical rect | initialised to `(0, 0, 800, 600)` |
| `+0x46` | flags2 (`0x21`) |
| `+0x68` | `vftable` IDSUpdated subobject (`0x00483650`) + embedded `CDSUpdatedItem` |
| `+0x80` | `int` — currently-pushed sub-screen id (-1 = none, 0 = StartGame1, 1 = HistoryDlg, 2 = ExitDlg) |
| `+0x84` | `void*` — currently-pushed sub-screen instance |
| `+0x88` | `void*` — current child background bitmap (replaced on screen-switch by `CMenu::FUN_00425870`) |
| `+0x8c` | `void*` — title/logo bitmap (resource `0x1013b`, placed at `(0,0)`, unpacked: `res_0000065851_21_BitmapJPEG.jpg`) |
| `+0x90` | `CDSBitmap*` — Start icon "normal", at `(100, 55)`, resource `[0x004af90c] = 0x00010093`, unpacked: `res_0000065683_28_BitmapSpecial.png` |
| `+0x94` | `CDSBitmap*` — Start icon "highlight", same position, resource `[0x004af910] = 0x00010094`, hidden by default (`FUN_0042d040`), unpacked: `res_0000065684_28_BitmapSpecial.png` |
| `+0x98` | `CDSBitmap*` — History icon "normal", at `(100, 139)`, resource `[0x004af914] = 0x00010043`, unpacked: `res_0000065603_28_BitmapSpecial.png` |
| `+0x9c` | `CDSBitmap*` — History icon "highlight", at `(100, 139)`, resource `[0x004af918] = 0x00010044`, hidden by default, unpacked: `res_0000065604_28_BitmapSpecial.png` |
| `+0xa0` | `CDSBitmap*` — Quit icon "normal", at `(100, 223)`, resource `[0x004af91c] = 0x0001004b`, unpacked: `res_0000065611_28_BitmapSpecial.png` |
| `+0xa4` | `CDSBitmap*` — Quit icon "highlight", at `(100, 223)`, resource `[0x004af920] = 0x0001004c`, hidden by default, unpacked: `res_0000065612_28_BitmapSpecial.png` |
| `+0xb0` | `CSwitch*` — **Start button** at `(35, 37)`, cmd `0xc9` |
| `+0xb4` | `CSwitch*` — **History button** at `(35, 121)`, cmd `0xca` |
| `+0xb8` | `CSwitch*` — **Quit button** at `(35, 205)`, cmd `0xcb` |
| `+0xc0` | `CDSAudioPlayer*` — active looping menu background music player, loaded from ambient resources `0x10149` / `0x1014a` by `CMenu_LoadBackgroundMusic @ 0x004252a0` |
| `+0xc4` | `CDSAudioPlayer*` — active deferred-exit voice player created by `CMenu_DispatchHotkey @ 0x00425340`; posts event id `1` to `this+0x10` when the sample completes |
| `+0xc6` | n/a |
| `+0xc8` | `CDSBitmap*` — decorative bitmap at `(610, 0)`, resource `0x10139` (right-side hero art, unpacked: `res_0000065849_21_BitmapJPEG.jpg`) |
| `+0xcc` | `ushort` — stashed modal exit code for deferred audio-cued exits (`0x80cb` for X / WM_CLOSE, `0x80c8` for F12); consumed by `CMenu::OnEvent(1)` |
| `+0xd0..0xdc` | 4× `CRuch*` — running-Bulanci background actors (one per slot, sizeof = 0x80) |
| `+0xe0` | `bool` — `lastSplashFlag` passed in by `CBulanci::OnEvent` (used to skip the first-time anim) |
| `+0xe1` | `bool` — last day/night toggle state, latched by `FUN_00425400` |
| `+0xe2` | `bool` — background music enabled/fade direction flag used by `CMenu_EnableBackgroundState` and `CMenu_OnMusicFadeTick`; `0` means fade down toward 70% then stop, non-zero means fade up toward 100% |

There is also a sibling `CPoemScroller` child (sizeof `0x128`, ctor
`FUN_004262c0`) that scrolls the credits in the bottom-left and a
sibling `CStaticText` at `(230, 575)` rendering text from static-text
pool index 93 (`g_apCDSStaticTextsSingleton[2] + 0x174` — looks like
the build/version line).

### 2.2 Resource map

| Symbol / addr | Value | Usage | Unpacked File (under `unpacked/overlay/`) |
|---------------|-------|-------|-------------------------------------------|
| literal `0x1013b` | menu title/background bitmap | composited at `(0,0)` | `res_0000065851_21_BitmapJPEG.jpg` |
| literal `0x10139` | right-side hero / "Bulanci" wordmark | composited at `(610,0)` (hero_bmp) | `res_0000065849_21_BitmapJPEG.jpg` |
| literal `0x1013a` | splash screen logo | IWANNAPLAY.COM splash logo | `res_0000065850_21_BitmapJPEG.jpg` |
| literal `0x1013c` | "Start Game" background | displayed when playing start menu | `res_0000065852_21_BitmapJPEG.jpg` |
| literal `0x1013d` | "Quit confirmation" background | displayed on exit dialog | `res_0000065853_21_BitmapJPEG.jpg` |
| literal `0x1013e` | "History" background | displayed in history dialog | `res_0000065854_21_BitmapJPEG.jpg` |
| literal `0x10149` | day-time background ambient audio (`Mp3`) | loaded and started by `CMenu_LoadBackgroundMusic` in `CMenu::SetDayNightBg(0x004252f0)` when hour is 6..21 | `res_0000065865_48_Mp3.mp3` |
| literal `0x1014a` | night-time background ambient audio (`Mp3`) | loaded and started when hour is 22..5 (night) | `res_0000065866_48_Mp3.mp3` |
| literal `0x100d4` | menu-button frame animation "off" | passed to `CSwitch_ctor` (`FUN_00424bc0`) as track 0 | `res_0000065748_52_BitmapSprite.bin` |
| literal `0x100d5` | menu-button frame animation "on" | passed to `CSwitch_ctor` as track 1 | `res_0000065749_52_BitmapSprite.bin` |
| literal `0x100b0` | font for bottom version line | wrapper sets fg via static-text alignment | *(engine-static font ID)* |
| `DAT_004af90c` | `0x00010093` | Start icon normal | `res_0000065683_28_BitmapSpecial.png` |
| `DAT_004af910` | `0x00010094` | Start icon highlight | `res_0000065684_28_BitmapSpecial.png` |
| `DAT_004af914` | `0x00010043` | History icon normal | `res_0000065603_28_BitmapSpecial.png` |
| `DAT_004af918` | `0x00010044` | History icon highlight | `res_0000065604_28_BitmapSpecial.png` |
| `DAT_004af91c` | `0x0001004b` | Quit icon normal | `res_0000065611_28_BitmapSpecial.png` |
| `DAT_004af920` | `0x0001004c` | Quit icon highlight | `res_0000065612_28_BitmapSpecial.png` |

The four "highlight" icons are created up-front, parented to the menu
via `_Globals::AddChild = FUN_0042d0b0`, then immediately hidden via
`FUN_0042d040`. On hover/press the `CSwitch` flips the visibility
through the linked `CDSBitmap` pair.

### 2.3 Construction algorithm

Recovered from `CMenu_ctor_WithUI @ 0x004265e0`:

```cpp
void CMenu::ctor_with_ui(uint displayDirty, uint8_t lastSplashFlag)
{
    CDSChained::ctor(this);                       // 0x004032d0
    CDSUpdatedItem::ctor(this + 0x68);            // 0x0042f060
    set_all_vftables(CMenu::vftable);
    this->cur_screen_id   = -1;
    this->cur_screen      = nullptr;
    this->bg              = nullptr;
    this->rect            = {0, 0, 800, 600};
    this->flags1         |= 0x77f;
    this->flags2         |= 0x21;
    this->lastSplashFlag  = lastSplashFlag;

    auto* pool = g_pApp->res_pool;                // CBulanci+0x70

    // 1. Title bitmap (resource 0x1013b) at (0,0).
    auto* title_img = pool->Lookup(0x1013b, 0);
    this->title_bmp = CDSBitmap::ctor(alloc(0x78),
                                      0, 0,
                                      cast<IDSImage>(title_img));
    AddChild(this, this->title_bmp, 0);

    // 2-4. Three menu buttons (CSwitch, sizeof 0xc8 = 200).
    this->btn_start   = CSwitch::ctor(alloc(0xc8), 0x23, 0x25, 1, 0xc9);
    AddChild(this, this->btn_start, 0);
    this->btn_history = CSwitch::ctor(alloc(0xc8), 0x23, 0x79, 1, 0xca);
    AddChild(this, this->btn_history, 0);
    this->btn_quit    = CSwitch::ctor(alloc(0xc8), 0x23, 0xcd, 1, 0xcb);
    AddChild(this, this->btn_quit, 0);

    // 5-10. Six button-icon bitmaps. Each is a fresh CDSBitmap parented
    // to the menu; the highlight variants are hidden initially.
    static constexpr uint32_t kIconIds[6] = {
        0x00010093, 0x00010094,    // start  normal / hi
        0x00010043, 0x00010044,    // history normal / hi
        0x0001004b, 0x0001004c,    // quit    normal / hi
    };
    static constexpr int       kIconY[6]   = { 55, 55, 139, 139, 223, 223 };
    void**                     icon_slots  = &this->icon_start_normal;
    for (int i = 0; i < 6; i++) {
        auto* img = pool->Lookup(kIconIds[i], 0);
        auto* bm  = CDSBitmap::ctor(alloc(0x78),
                                    100, kIconY[i],
                                    cast<IDSImage>(img));
        icon_slots[i] = bm;
        AddChild(this, bm, 0);
    }
    Hide(this->icon_start_highlight);
    Hide(this->icon_history_highlight);
    Hide(this->icon_quit_highlight);

    // 11. Right-side hero art (resource 0x10139) at (610, 0).
    auto* hero_img = pool->Lookup(0x10139, 0);
    this->hero_bmp = CDSBitmap::ctor(alloc(0x78),
                                     610, 0,
                                     cast<IDSImage>(hero_img));
    AddChild(this, this->hero_bmp, 0);

    // 12. Scrolling credits widget.
    auto* poem = CPoemScroller::ctor(alloc(0x128));
    AddChild(this, poem, 0);

    // 13. Bottom version line.
    auto* version = CStaticText::ctor(alloc(0x98),
                                       /*x*/0xe6, /*y*/0x23f,
                                       /*flag*/0, /*align*/1,
                                       /*font*/0x100b0,
                                       g_static_texts[93]);
    AddChild(this, version, 0);

    // 14-17. Four background "running Bulanci" actors.
    for (int i = 0; i < 4; i++) {
        this->ruch[i] = CRuch::ctor(alloc(0x80));
        AddChild(this, this->ruch[i], 0);
    }

    // Init runtime state.
    CMenu::EnableAllRuch(this, /*enable=*/false);  // FUN_00423f70
    this->day_night_latch = false;
    Scheduler_RegisterEventSlot(this + 0x68,
                                /*slot*/0,
                                /*delay_ms*/120,
                                /*event*/7);       // FUN_0042f210
    CMenu::PollDayNight(this, /*force=*/true);     // FUN_00425400
    if (initial_focus_idx < 3)
        Button::Click(this->btns[initial_focus_idx]);  // FUN_00424d30
}
```

### 2.4 The three menu buttons (`CSwitch` widget)

Each menu button is a `CSwitch` (animated toggle), allocated `0xc8`
bytes, built by `CSwitch::ctor (FUN_00424bc0)`:

```cpp
CSwitch* CSwitch::ctor(CSwitch* self, int x, int y, uint8_t type,
                       uint16_t cmd)
{
    ODSImage::ctor(self);                          // 0x004228f0
    set_all_vftables(CSwitch::vftable);            // 6 slots at 0,4,0x10,0x18,0x68,0x6c
    CDSObject::ConstructTrackManager(self + 0x7c); // 0x00439c70
    self->rect.x = x; self->rect.y = y;

    // Two sprite tracks: 0x100d4 (off) and 0x100d5 (on).
    for (int i = 0; i < 2; i++) {
        auto* img = g_pApp->res_pool->Lookup({0x100d4, 0x100d5}[i], 0);
        AddTrackSource(self + 0x7c, &img->seq);    // 0x00439e20
        Release(img);
    }
    SetCurrentTrack(self + 0x7c, 0, /*reset*/1);   // 0x00439e80
    TM_SetCurrentSequence(self + 0x68, self->seq); // 0x0043a0d0
    self->flags1 |= 0x238;
    self->type   = type;     // 1 = momentary push button
    self->state  = 0;
    self->cmd    = cmd;
    return self;
}
```

When the user clicks (mouse) or activates (keyboard) a button, the
generic `_Globals::Button::Click @ 0x00424d30` runs:

```cpp
void Button::Click(CSwitch* btn)
{
    if (btn->state != 0) return;
    if ((btn->flags1 & 5) != 1) return;           // ignore if disabled

    // Edge case for "radio" type buttons (type==1, in single-cmd mode).
    if ((btn->aux1 == 1 && btn->aux0 == 0) || btn->aux0 == 1)
        Button::DoToggle(btn);                    // 0x00423f20

    btn->state = 1;
    auto* parent_evt = btn->parent + 0x10;        // IDSEventHandler subobject
    Scheduler_PostMessage(parent_evt,
                          /*msg*/0x400, /*delay_ms*/200,
                          /*payload*/btn, /*immediate*/0);  // visual ACK
    Scheduler_PostMessage(parent_evt,
                          /*msg*/0x100, /*delay_ms*/btn->cmd,
                          /*payload*/0, /*immediate*/0);    // dispatch cmd
}
```

The second `Scheduler_PostMessage` is the one that drives the
state machine — `msg=0x100` lands in `CMenu::OnEvent =
FUN_00424f50`, which forwards to `CMenu::Cmd_Dispatch
(FUN_00425970)` with `cmd = btn->cmd`.

### 2.5 Command dispatcher (`CMenu::FUN_00425970`)

`CMenu::Cmd_Dispatch(this, uint16_t cmd)` is the heart of the menu.
Each branch performs the same three-step sequence: shut down the
current sub-screen (`FUN_00423a00`), swap the background image
(`FUN_00425870`), then build + attach the new sub-screen as a child
view (`AddChild`). The sub-screens are NOT modal — they are just
swapped child views, kept under the same `CMenu::DoModal` umbrella.

| cmd | What happens |
|-----|--------------|
| `0xc9` | **Start Game** → close prev sub-screen → load bg `0x1013c` → alloc `CStartGame1 (0xa8)` via `CStartGame1::ctor @ 0x0040f400` → attach → input-handler slot 0x1b → `state=0` → if `this+0xe0` was set, post deferred 0x100/0xcf so the user lands directly on the right page. |
| `0xca` | **History** → load bg `0x1013e` → alloc `CHistoryDlg (0x9c)` via `CHistoryDlg::ctor @ 0x004231d0` → `FUN_00422f70(dlg, g_pApp[0x306])` to scroll to the last-read page → attach → input slot 0x18 → `state=1`. |
| `0xcb` | **Quit confirm** → load bg `0x1013d` → alloc `CExitDlg (0x7c)` via `CExitDlg::ctor @ 0x00411b50` → attach → input slot 0x19 → `state=2`. |
| `0xcf` | **Back** (from a sub-screen): `FUN_00423a00(this, 0)` — close current sub-screen, restore main-menu buttons. Also calls `FUN_00401af0(g_pApp)` (small CBulanci helper). |
| `0x8004` | **App-exit hotkey / native close event** → `CMenu_DispatchHotkey(this, 0x80cb, 0x1a)`. This bypasses `CExitDlg`, plays the "Konec hry" voice cue, hides the three main buttons, and exits the menu modal only after the SFX completion callback fires. |
| `0x80ce` | **F12** → `CMenu_DispatchHotkey(this, 0x80c8, 0x1c)`. Same deferred audio-cued mechanism, but with alternate exit cue slot `0x1c`. |
| any other | Falls through to `CDSApp::OnEvent_Default (0x0042c7d0)`. |

`FUN_00423a00(this, suspend)` is the **sub-screen teardown**:

```cpp
void CMenu::CloseCurrentSubScreen(bool wasUserInitiated)
{
    if (wasUserInitiated && 0 <= state && state < 6) {
        // Restore button highlight (show the normal icon, hide the hi-version).
        Show(this->icons[state*2 + 0]);
        Hide(this->icons[state*2 + 1]);
    }
    if (this->cur_screen) {
        if (state == 0)              // StartGame1 — remember the menu cursor
            this->cur_screen->vfn[4](g_pApp + 0x2e6);
        else if (state == 1)         // HistoryDlg — remember last page
            g_pApp[0x306] = this->cur_screen[0x20];

        // Detach + Release the sub-screen.
        FUN_0042d160(this, this->cur_screen);     // RemoveChild
        Release(this->cur_screen);
    }
    this->cur_screen = nullptr;
    this->state      = -1;
}
```

### 2.6 Keyboard shortcuts (`CMenu::OnKeyDown = FUN_00424fa0`)

Slot 22 of the primary vtable. Receives `(scanCode, isDown)` from the
generic `CDSView::OnChar` pipeline. The reset-key handler
`CDSView::FUN_0042c0c0` runs first; if it eats the event we return 1.

| `scanCode` | Action |
|------|--------|
| `0x48` (`'H'`) | `Button::Click(btn_history)` — "Historie". Fires on both keyDown and keyUp; `Button::Click` is idempotent (early-out if `state != 0`), so the duplicate edge is harmless. |
| `0x4b` (`'K'`) | `Button::Click(btn_quit)` — Czech "Konec" = quit. Same dual-edge behaviour; opens the `CExitDlg` confirmation. |
| `0x53` (`'S'`) | `Button::Click(btn_start)` — "Start hry". |
| `0x58` (`'X'`) | If keyDown only: `Scheduler_PostMessage(this+0x10, 0x100, 0x8004, 0, 0)`. Returns `param_2` (= 1 on keyDown, 0 on keyUp). **Does NOT go through `Button::Click`** — see §2.6.1 below. |
| default | return 0 (event not handled) |

#### 2.6.1 Why `X` looks dead in-game (verified bug)

S/H/K and X follow **structurally different** exit paths:

```
S/H/K ─► Button::Click(btn) ─► Scheduler_PostMessage(parent+0x10,
            0x100, cmd=0xc9|0xca|0xcb, ...)
        ─► CMenu::OnEvent(0x100) ─► Cmd_Dispatch(cmd)
        ─► builds sub-screen (CStartGame1 / CHistoryDlg / CExitDlg)
            as a child view → user sees the dialog *immediately*,
            synchronously, on the next frame.

X     ─► Scheduler_PostMessage(this+0x10, 0x100, 0x8004, ...)
        ─► Cmd_Dispatch(0x8004)
        ─► CMenu::DispatchHotkey(this, 0x80cb, audioSlot=0x1a)
            (a) stashes 0x80cb at this+0xcc
            (b) plays audio bank slot 0x1a via a fresh CDSAudioPlayer
                (TriggerBankSample → FUN_0043a760), with the player's
                completion-event target set to this+0x10
            (c) hides the three main-menu buttons (Start/Hist/Quit)
        ─► …time passes while the sound plays…
        ─► audio completes → DirectSound posts event id 1 to this+0x10
        ─► CMenu::OnEvent(1) → _Globals::FUN_0042c3c0(this, 0x80cb)
            which sets this+0x4a = 0x80cb iff this+0x44 & 0x10 set.
        ─► CDSView::DoModal sees this+0x4a != 0, exits returning
            sign-extended (int16)0x80cb = -0x7f35.
        ─► CBulanci::OnEvent::0xcc treats -0x7f35 as "shutdown app".
```

The X chain therefore depends on **three async preconditions** that
S/H/K simply don't have:

1. **Audio bank slot 0x1a must be populated.** `FUN_0043a760` reads
   `slot[1]` (sample length) and `slot[2..3]` (sample-rate / channels)
   without a null check; an empty slot pointer is a hard NPE, but a
   slot whose backing sample is missing or zero-length plays nothing
   and may never produce a completion edge.
2. **DirectSound must be initialised** (`g_pDirectSoundSingleton+0x38
   != 0`). If it isn't (e.g. headless mode, audio disabled), the
   block that creates the DSound buffer is skipped → the player never
   schedules a completion event.
3. **`this+0x44 & 0x10` must be set** when the completion event fires.
   `FUN_0042c3c0` no-ops silently otherwise — the stashed `0x80cb` at
   `+0xcc` is just thrown away and the modal stays alive forever with
   the buttons still hidden by step (c) above.

If any of (1)/(2)/(3) fails, the **only visible effect of pressing X
is that the three buttons disappear** (step (c) runs unconditionally),
and the menu sits there inert. K, H, S all bypass this because they
build their dialog synchronously and have no audio dependency.

The simplest faithful re-implementation is to treat X like a direct
shutdown trigger (no audio cue), or — to match the K confirmation
flow — to map X to `Button::Click(btn_quit)` exactly like K. The
audio-cued path is original-binary behaviour but is the source of the
"X does nothing" report.

### 2.7 Background day/night switch

`CMenu::PollDayNight (FUN_00425400)` is called once at construction
(forced) and presumably re-invoked on each scheduler tick (event 7,
arming at 120 ms in the ctor — needs confirmation from
`CMenu::vfn[14]=Tick`).

The "is it night" predicate reads `FUN_0042e8b0` (looks like a
`SYSTEMTIME`-style snapshot) and tests `(snap[0] >> 25) - 6 > 15`,
which carves out the hours 6-21 as "day". On state change it calls
`CMenu::SetDayNightBg (FUN_004252f0)`:

```cpp
void CMenu::SetDayNightBg(bool isNight)
{
    this->day_night_latch = isNight;
    if (isNight) {
        CMenu_LoadBackgroundMusic(this, 0x1014a);   // load and loop night ambient track (Mp3)
        Show(this->hero_bmp);
    } else {
        CMenu_LoadBackgroundMusic(this, 0x10149);   // load and loop day ambient track (Mp3)
        Hide(this->hero_bmp);
    }
}
```

`CMenu_LoadBackgroundMusic(this, resId) @ 0x004252a0` wraps
`CDSAudioPlayer_CreateFromResource(category=2, resId, 0, 0, loop=1) @
0x00422550` to instantiate the ambient MP3 loop. It releases any
previous background player at `this+0xc0`, stores the new
`CDSAudioPlayer*` there, sets its volume percent to `0x46` (70/100) via
`CDSAudioPlayer_SetVolumePercent @ 0x0043a0d0`, then calls
`CMenu_EnableBackgroundState(this, 1) @ 0x00424010`.

`CMenu_EnableBackgroundState(this, enabled)` updates the fade-direction
flag at `this+0xe2` and drives the scheduler slot embedded in
`this+0x68`. When enabling, it starts or resumes the player at `+0xc0`;
when disabling, it leaves the fade tick to ramp down before stopping.

`CMenu_OnMusicFadeTick @ 0x00424080` is the full fade mechanism. It runs
from scheduler slot `0` registered on `this+0x68` at **120 ms** cadence:

```c
if (this->bg_music_enabled_e2 == 0) {
    percent = max(player->volume_percent_54 - 1, 70);
    CDSAudioPlayer_SetVolumePercent(player, percent);
    if (percent == 70) {
        Scheduler_ArmSlot(this+0x68, 0);
        CDSAudioPlayer_Stop(player, 1);
    }
} else {
    percent = min(player->volume_percent_54 + 1, 100);
    CDSAudioPlayer_SetVolumePercent(player, percent);
    if (percent == 100) {
        Scheduler_ArmSlot(this+0x68, 0);
        CMenu_EnableAllRuch(this, 1);
    }
}
```

The curve is therefore **linear in integer volume percent**, stepping
by `1` every `120 ms`. The audible gain is not linear because
`CDSAudioPlayer_ApplyEffectiveVolume @ 0x0043a060` later maps percent
to DirectSound attenuation:

```c
attenuationDb100 = ((busDb100 + 10000) * percent) / 100 - 10000;
```

With the menu bus at `0 dB`, `100%` is `0 dB` (`linearGain=1.0`),
`90%` is `-1000` hundredths dB (`~0.3162`), and `70%` is `-3000`
hundredths dB (`~0.03162`). This was verified dynamically with
`scripts/frida/menu_audio_mixer_trace.js`.

### 2.8 The 4 `CRuch` running-Bulanci actors

`CRuch::ctor (FUN_00423c20)` builds a 0x80-byte CDSChained subclass:

* Picks a random X via `rand() * (appRight - appLeft) >> 15`,
  collapsing the actor's bounding box to a 1-pixel-wide vertical
  column spanning the full window height.
* Starts hidden (`FUN_0042d040`).
* `Scheduler_RegisterEventSlot(self + 0x68, slot=0,
   delay=rand()*0xfa1 >> 15, event=6)` — random first appearance
  inside `[0, ~8s]`, so the 4 actors emerge staggered.

The actual sprite swap / motion path lives on `CRuch`'s tick handler
(`FUN_00423ea0` enables/disables them as a group; per-actor motion is
delegated to the engine's `CDSAnim` runtime, see `../engine/anim_runtime.md`).

## 3. `CStartGame1` — game-type / connection select

Pushed as the menu's child view when the user clicks Start.

* **Class size:** `0xa8`
* **Ctor:** `CStartGame1::ctor @ 0x0040f400`. Calls `CWindow::ctor
  (0x00405560, x=250, y=60, w=760, h=560)`, sets vtables, then
  delegates to **`CStartGame1::BuildUi @ 0x0040c650`**.
* **Cmd handler:** `CStartGame1::OnEvent @ 0x0040ae90`
  — only handles `0xcf` (forwards "back" up to the parent menu via
  `Scheduler_PostMessage(parent_evt, 0x100, 0xcf, 0, 0)`); everything
  else falls through to `CDSApp::OnEvent_Default`.
* **Singleton getter:** `CStartGame1::Singleton @ 0x0040f4b0` returns
  `&DAT_004b390c` (the registered class registry slot).

### 3.1 Layout

| Y | Widget | Args | Pool index | Meaning |
|---|--------|------|------------|---------|
| 20 | `CStaticText` | (20, 20), color `0x100ae`, align 2 | 34 (`+0x88`) | "Typ hry:" header |
| 45 | `CRadio` (sizeof `0xa8`) at (30, 45) | 2 options at pool 35, 36 | 35-36 | Game type: Single / Multi |
| 105 | `CStaticText` | (20, 105) | 37 (`+0x94`) | "Připojení:" header |
| 130 | `CRadio` at (30, 130) | 4 options at pool 107, 39, 40, 41 | 107, 39-41 | Connection: TCP/IP, IPX?, Modem?, Lobby? |
| 223 | `CStaticText` | (20, 223) | 42 (`+0xa8`) | "Počet kol:" header |
| 248 | `CRadio` at (30, 248) | 3 options at pool 38, 39, 40 | 38-40 | Round count (?) — reuses 39/40 from above |
| 325 | `CStaticText` | (20, 325) | 43 (`+0xac`) | "Server:" header |
| 350 | `CRadio` at (30, 350) | 2 options at pool 44, 45 | 44-45 | Host / Join |
| 456 | `CButton` | (20, 456) cmd `0xcf` | 48 (`+0xc0`) | "Zpět" — back to main menu |

Object fields:

| Offset | Field |
|--------|-------|
| `+0x22` (`param_1[0x22]`) | Radio 1 — game type |
| `+0x23` | Radio 2 — connection |
| `+0x24` | Radio 3 — round count |
| `+0x25` | Radio 4 — host/join |
| `+0x26` | StaticText 4 — host header |
| `+0x27` | StaticText 2 — connection header |
| `+0x28` | StaticText 3 — rounds header |
| `+0x29` | "ready" flag |
| `+0x4c` | Parent pointer (CMenu*) — used by `OnEvent::0xcf` |

There is **no explicit "Continue / Start" button** in CStartGame1. The
advance into the lobby happens automatically once the user has filled
in all four radios. The radio change-callbacks (`FUN_0040a860`,
`FUN_0040a8c0`, `FUN_0040a970`, `FUN_0040a9d0`) update enable/disable
state of dependent rows and, on the final selection, post the next
cmd:

* `FUN_0040a860(this)` — checks the game-type and host/join radios.
  When both are valid and the player is a host (`host_join_radio == 1`
  or `game_type_radio == 1`), it shows the player-name & round
  rows (`*(int**)(this+0x94)`, `*(int**)(this+0x98)`); otherwise hides
  them. (This is what makes the "round count" and "server"
  sub-questions appear conditionally.)
* `FUN_0040a970(this, isJoinMode)` toggles visibility of the
  TCP-address & lobby-account inputs (only relevant when joining).
* `FUN_0040ae30(this, key, button, arg)` is the per-radio change
  dispatcher (`key=0xce` = "WM_COMMAND-ish notification") — it routes
  the change to the right callback based on which radio fired.

When the user picks "Connect via TCP/IP" and "Join", the CTcpIpConfig
sub-dialog opens (see §6.2 below).

### 3.2 Hand-off to CGaming

The actual game launch fires from inside the network helper
`CMenu::OpenNetworkSession (FUN_00414dd0 = the
`CGame_OpenNetworkSession_AndSendJoin_t00` symbol in `mapping.csv`).
Pipeline:

1. `GetComputerNameW` → lowercase player handle.
2. Allocates `CDSDirectPlay (0x8c)` at `this+0x1dc`.
3. **Host path** (`this+0x62 == 0`):
   * If TCP (`this+0x65 == 0`): `CDSDirectPlay_ConnectTCP(this+0x1dc,
     &g_TcpipGuid_00486d30, &g_BulanciAppGuid_00481a50, ipString)`.
   * If Lobby (`this+0x65 == 1`):
     `CDSDirectPlay_ConnectLobby(this+0x1dc,
     &g_LobbyGuid_00486d40, &g_BulanciAppGuid_00481a50)`.
   * Else: `CDSSimpleException(4, 6)` (unsupported).
   * `CDSDirectPlay_HostSession(this+0x1dc, computerName)`.
   * Mark `this+0x30 = 2` ("hosting"), populate slot bookkeeping for
     `this+0x37` slots.
4. **Join path** (`this+0x62 != 0`): same DirectPlay setup, but inside
   the connect call it pushes a *CTcpIpConfig* dialog first (modal,
   `CMenu::FUN_00413030(this, dlg, suspend=0)`) so the user can
   pick the host. Then `CDSDirectPlay_JoinSession(this+0x1dc, &guid)`
   and `FUN_00414170` enumerates `CSessionList` to pick which game
   to join. Sends msg type `0x00` (`CGame_NetSendJoin`) carrying the
   joining player's profile.
5. Allocates a 0x2000-byte `CDSEasyMemStream` at `this+0x1b0` (msg
   scratch buffer).
6. `this+0x30 = 3` ("ready").
7. Calls **`FUN_00414790(this)`** — `CMenu::ShowLobby`:

```cpp
void CMenu::ShowLobby(this) {
    this->Cleanup();                            // FUN_00412780
    CStartGame2 lobby_view;                     // stack-allocated 0xdc
    CStartGame2::ctor(&lobby_view, this);       // FUN_004104f0
    Scheduler_PushHook(this+0x1c);              // FUN_0042f620 — save modal context
    this->NetSendKickAll();                     // FUN_00414640
    CMenu::DoModalChild(this, &lobby_view, 1);  // FUN_00413030 — enters CGaming if user clicks Start
    Scheduler_PopHook(this+0x1c);               // FUN_0042f5f0
    CStartGame2::dtor(&lobby_view);             // FUN_0040bb90
}
```

`FUN_00413030(parent, child, suspend)` is the **CMenu-side modal
helper** (the in-class equivalent of `CDSView_DoModal`):

```cpp
short CMenu::DoModalChild(CMenu* this, CDSView* child, char suspendParent)
{
    if (suspendParent && this->modal_parent_evt) {
        Scheduler_StopSlot(this+1, 0, -1);          // 0x0042f330
    }
    this->modal_child = child;
    short rc = CDSView_DoModal(child, this->modal_parent_evt);
    this->modal_child = nullptr;

    if (suspendParent && (parent_slot.flags & 1) == 0)
        Scheduler_KillSlot(this+1, 0);              // 0x0042f300

    switch (rc) {
        case -0x7ffd:  CDSSimpleException(4, 6);   // throws
        case -0x7ffc:  CDSSimpleException(0x14, 0); return -0x7ffc;
        case -0x7f37:  CMenu::ThrowModalDone(this); // FUN_004127d0 — propagates
        default:       return rc;
    }
}
```

`CMenu::ThrowModalDone (FUN_004127d0)` rethrows the stored exit code
(`this+0x1f0`) via `__CxxThrowException_8(&code, &type_info_at_0x004a598c)`,
which bubbles up through the CMenu modal stack to whoever called
`DoModal` (ultimately `CBulanci::OnEvent`).

## 4. `CStartGame2` — lobby / team setup

* **Class size:** `0xdc`
* **Ctor:** `CStartGame2::ctor @ 0x004104f0(this, parentState)` where
  `parentState` is the `CMenu` instance (its `+0x1dc` carries the
  `CDSDirectPlay*`, `+0xbc..` carries the level list, etc.).
* **Singleton getter:** `CStartGame2::Singleton @ 0x004245b0` returns
  `&DAT_004b3984` (yes — shared with `CMenu`; the class registry
  slot is reused because CStartGame2 isn't its own resource type, it's
  just a CDSView).

### 4.1 Layout (CWindow 760x560 at (230, 50))

The layout is data-driven on `parentState->slot_count` (`+0xd8`),
`parentState->team_capacity` (`+0x36`/`+0x37`) and `parentState->is_admin`
(`+0x36`). For each of the (up to) `slot_count` rows, the ctor emits a
template of:

1. **Slot header** (cm hidden if "occupied by another team") — sizeof
   `0x70`, ctor `FUN_0040ffd0(this+slot*0x82..)`.
2. **`CGameView`** (sizeof `0x98`) at `(rowX-3, 0x35)` — the avatar
   stage (`CGameView::ctor @ 0x004191a0`).
3. **`CBulAnim`** (sizeof `0xd4`) at `(rowX+10, 0x44)` — the
   animated bulánek for that slot. Construction matches the template
   documented in `../engine/anim_runtime.md` (4 facing-anim variants bound via
   `TM_BindSequence`, random starting frame).
4. **`CEdit`** (sizeof `0xb8`) at `(rowX-12, 0x8a)` w=0x96, h=0x28 —
   player-name editor.
5. *(admin row only)* **`CButton`** (sizeof `0x98`) at `(rowX-3, 0xaa)`
   w=14, cmd `&DAT_004aeaa8[slot]` — kick button. Label from static
   pool `g_apCDSStaticTextsSingleton[2]+0xc4` = index 49.

After the slot loop:

* If user is admin **and** there are >1 slots, a "Help" cycle button at
  `(31, 208)` w=14, cmd `0x14`, label pool index 54 (`+0xd8`).
* Else a static `"# slots"` label at `(20, 210)` color `0x100ae` align
  2, label pool index 50 (`+0xc8`).
* A "Start Server"/"Kick" button at `(400, 210)` w=14, cmd `0x8002`,
  label pool index 87 (`+0x15c`).
* A static "Connecting…" label at `(20, 244)` color `0x100ae`, label
  pool index 52 (`+0xd0`).
* A **`CLevelList`** (sizeof `0xe4`) at the default origin via
  `CLevelList::ctor @ 0x0040b0c0`. Populated by iterating
  `parentState->levels[0..parentState->level_count]`
  (`parentState+0xbc..+0xc4`) and calling `CLevelList::AddItem
  (FUN_0040d360)`.
* If the local user is NOT admin, the level list is hidden
  (`FUN_0042d080`) — only the admin picks the level.
* `FUN_004102d0(this, parentState, 0xf0, 0xf4)` — places "Map preview"
  & "Map info" widgets at `(0xf0, 0xf4) = (240, 244)`.
* **"Back" button** at `(0x28, 0x1d2) = (40, 466)`, w=-1 (auto-size),
  type 1, cmd `0x8002`, label pool index 54 (`+0xd8` = "Zpět"/Back).
  Stored at `this+0xb8`.
* If admin:
  * Disables the back button (admins must Start, not back out without
    everyone).
  * Adds a "Press Start" hint static at `(40, 495)`, color `0x100af`
    align 2, label pool index 106 (`+0x1a8`).
  * `Scheduler_RegisterEventSlot(this+0x70, 0, 500ms, 7)` — periodic
    poll for the "all clients ready" signal.
* **"Start" button** at `(0x8c, 0x1d2) = (140, 466)`, w=14, type 0, cmd
  `0x8003`, label pool index 30 (`+0x78` = "Start").

### 4.2 Network plumbing (for context)

Documented in `../netcode/net_protocol.md`; relevant subset:

| Event in lobby | Net msg | Sender helper |
|----------------|---------|----------------|
| User edits name | `0x02` (`Rename`) | `CGame_NetSendRename_t02 @ 0x004139b0` |
| Admin picks avatar | `0x03` (`SetAvatar`) | `CGame_NetSendSetAvatar_t03 @ 0x00412c70` |
| Admin changes slot count | `0x04` (`SlotCount`) | `CGame_NetSendSlotCount_t04 @ 0x00412cd0` |
| Admin picks level | `0x05` (`SetLevel`) | `CGame_NetSendSetLevel_t05 @ 0x00414340` |
| Slot occupancy change | `0x08`/`0x09` (`SlotChange`) | `CGame_NetSendSlotChange_t08_t09 @ 0x00413f30` |
| Admin presses Start | `0x64` (`AdminByte`) | `CGame_NetSendAdminByte_t64 @ 0x00412d40` |
| Initial join | `0x00` | inlined in `OpenNetworkSession` |

### 4.3 Hand-off into the game

Recovered from `CGame::CGame_NetSendSlotChange_t08_t09 (FUN_00413f30)`
which is the unified "all ready, go!" code path:

```cpp
void CGame::StartGame(this) {
    // 1. Build the live CGaming on the stack.
    CGaming gaming;
    CGaming::ctor(&gaming, this);                // FUN_0041ff90

    // 2. Slot snapshot + adapter array (the local_98 in disasm).
    CGame::BuildSlotSnapshot(this, snapshot);    // FUN_00410e20

    // 3. Build the input adapter (CDirectKeyb, sizeof 0x20c).
    auto* kbd = CDirectKeyb::ctor(alloc(0x20c)); // FUN_004121d0
    this->keyboard_adapter = kbd;

    AddChild(this->view_root, snapshot, 0);
    CDSApp::FrameBody(this->view_root);          // pump one frame to settle the new layout

    // 4. Final pre-game handshake — send msg type 0x09 to peers.
    NetSend(this->dp, msg_0x09);

    // 5. Loading screen, then game itself, both via CMenu::DoModalChild.
    Scheduler_PushHook(this+0x1c, ...);
    CMenu::DoModalChild(this, snapshot, /*suspend*/1);   // shows CLoadingLevel
    Scheduler_PopHook(this+0x1c, ...);

    Scheduler_PushHook(this+0x1c, ...);
    CMenu::DoModalChild(this, &gaming, /*suspend*/1);    // the actual match
    Scheduler_PopHook(this+0x1c, ...);

    // 6. Teardown.
    Release(this->keyboard_adapter);
    CGame::ResetForLobby(this);                  // FUN_00413510
    eh_dtor_iterator(slot_views, 4, 2, FUN_004344c0);
    CDSView::Destruct(snapshot);
    CGaming::dtor(&gaming);                      // FUN_0041b850
}
```

Once the match ends, control returns to `CMenu::ShowLobby`
(§3.2 step 7), which returns to `CMenu::OpenNetworkSession`, which
returns to `CMenu::Cmd_Dispatch::case 0xc9`, which returns to
`CDSView_DoModal`, which keeps the main-menu modal pump running.

## 5. Sub-dialogs

### 5.1 `CAdvertising` — startup splash

* **Size:** small (window-only)
* **Ctor:** `CAdvertising::ctor @ 0x0040e5f0(this)` — `CWindow(0, 0, 800,
  600, 0)`, sets `CAdvertising::vftable`.
* Adds a `CBlackView` child sized 800×600 with color `0x00ffffff`
  (white) — the splash backdrop.
* Sets `flags1 |= 0x111` and `byte+0x23 = 0` (auto-dismiss timer).
* The real payload is loaded by `FUN_0040fe30(splash, 0x1013a)`:
  pulls resource `0x1013a` (the IWANNAPLAY.COM logo image), creates a
  centred `CDSBitmap` at `((800-w)/2, (600-h)/2)`, and finally calls
  `FUN_0040aca0(splash, ...)` which schedules the auto-dismiss event
  (any keypress also dismisses via the splash's `OnKeyDown`).

### 5.2 `CHistoryDlg` — history/story viewer

* **Size:** `0x9c`
* **Ctor:** `CHistoryDlg::ctor @ 0x004231d0(this)`. Window at
  `(0xe7=231, 0x24=36, 0x2fa=762, 0x239=569)`.
* Iterates the resource pool for items with class id `0x802` (= 2050,
  `Poem`/`HistoryPage`) and adds them to the page list at
  `this+0x1c`.
* Allocates four `CIcon` navigation buttons (sizeof `0x78` each) at:
  - `(0x27=39, 0x1e0=480)` — resource id `0xe5`, sprite at `0x00483390`
  - `(0x38=56, 0x1e0=480)` — resource id `0xe6`, sprite at `0x00483384`
  - `(0x48=72, 0x1e0=480)` — resource id `0xe7`, sprite at `0x00483378`
  - `(0x5b=91, 0x1e0=480)` — resource id `0xe8`, sprite at `0x0048336c`
  These look like Prev / Next / First / Last page glyphs (4 hex glyphs
  in a row).
* `FUN_00422f70(dlg, lastPage)` scrolls to the page index stored at
  `g_pApp+0x306` so the user resumes where they left off.

### 5.3 `CExitDlg` — quit confirmation

* **Size:** `0x7c`
* **Ctor:** `CExitDlg::ctor @ 0x00411b50(this)`. Window at
  `(0xdd=221, 0x18=24, 0x2fa=762, 0x239=569)`.
* Two **`CAnim`** characters (sizeof `0xf0` — the sprite from the
  master pack) at `(0xc2=194, 0x16=22)` and `(0x139=313, 0x11a=282)`
  — the two Bulanci on the "Really exit?" splash.
* Static text at `(0x28=40, 0x48=72)`, font `0x100af`, align 2, text
  from pool index 78 — the actual "Really exit?" question.
* Single button at `(0x31=49, 0x1ec=492)`, w=-1, type 1, cmd
  `0x8004`, label pool index 83 (`+0x14c`, "Konec aplikace" = quit
  app). Pressing it posts `0x8004` to the parent CMenu, which the
  cmd dispatcher (§2.5) routes through `FUN_00425340 → 0x80cb → 0xcb`
  to actually quit.

### 5.4 `CTcpIpConfig` — TCP/IP server selection (join mode only)

* **Ctor:** `CTcpIpConfig::ctor @ 0x0040c060(this)`.
* Window at `(0, 0, 0x138=312, 0xa0=160)`; the ctor then **shifts** it
  to `(300, 0)` (so it overlaps the right half of the screen) and
  re-evaluates the size by translating the window-base offsets:
  ```
  rect.h -= rect.y; rect.w = rect.w - rect.x + 300; rect.x = 300; rect.y = 0;
  ```
* Children:
  - Static text at `(20, 20)` w=292, h=60, font `0x100ae`, align 1,
    color 5 — "Adresa serveru:" header (pool 31, `+0x7c`).
  - `CEdit` at `(20, 70)` w=292, h=60, maxlen 18, align 2, color
    `0x100af` — the IP/hostname field.
  - "Cancel" button at `(73, 115)`, w=14, type 1, cmd `0x8002`, label
    pool index 29 (`+0x74`, "Zpět").
  - "OK" button at `(163, 115)`, w=14, type 0, cmd `0x8003`, label
    pool index 30 (`+0x78`, "Start"/OK).
* `flags2 |= 8` (= "auto-modal close on `0x8003`").

### 5.5 `CSessionList` — DirectPlay session browser (join mode only)

Pulled in via `CMenu::FUN_00414170` after a successful
`CDSDirectPlay_JoinSession` call:

```cpp
void CMenu::PickSession(this, GUID* outGuid) {
    CSessionList list;
    CSessionList::ctor(&list);                    // 0x0040c2d0
    Scheduler_StopSlot(this+1, 1, -1);            // 0x0042f330
    CMenu::DoModalChild(this, &list, /*suspend*/0); // 0x00413030
    Scheduler_KillSlot(this+1, 1);                // 0x0042f300

    auto* picked = CSessionList::GetPick(&list);  // 0x0040c550
    *outGuid = *(GUID*)((char*)picked + 0x14);    // copy 16 bytes
    CSessionList::dtor(&list);                    // 0x00401910
}
```

The list is auto-populated by DirectPlay enumeration (running on slot
1 of the scheduler — that's why it gets paused around the modal call).

### 5.6 `CMsgDialog` — generic "Connecting..." progress dialog

Used by `FUN_004137b0` and by the "fatal error" path inside
`CMenu::OpenNetworkSession`. Builds via `CMsgDialog::ctor (FUN_0040be80)`
with `(text, cancelEnabled)`; the text comes from the static pool.

## 6. Resource pool plumbing

Every menu component pulls its art / animations through
`g_pApp->res_pool` (`CBulanci+0x70`). The pool is set up at boot in
`CBulanci::InitResourceBank @ 0x00402180` by opening the EXE itself as
a `CDSFileStream` and wrapping the trailing overlay/master-pack via
`ODSImage::FUN_00426060`.

The two important resource ID prefixes used by the menu:

| ID range | Class | What |
|----------|-------|------|
| `0x0001xxxx` | `BitmapSprite` (52) | Button icons, hero art, splash logo (see `../formats/sprite_container.md`) |
| `0x0010xxxx` | engine-static IDs | UI fonts, alignment hints, dropped through the pool's `Lookup` to return a wrapped `IDSImage` with the matching face. `0x100ae`/`0x100af`/`0x100b0` are colour-coded font palettes; `0x10004` is the cursor; `0x100d4`/`0x100d5` are the button-press animations; `0x10139..0x1013e` are the menu backgrounds/bitmaps, while `0x10149` and `0x1014a` are the ambient audio loops (`Mp3`). |

The bank's vtable:

| Slot | Purpose (inferred) |
|------|---------------------|
| `+0x10` (vfn 4) | `Lookup(resId, flags)` — resolves a resource handle |
| `+0x18` (vfn 6) | `GetCount()` — total resource count |
| `+0x1c` (vfn 7) | `GetByIndex(i)` — yields a `ResourceRecord` with `(classId @ +0xc, resId @ +0x8)` |

`CPoemScroller` and `CHistoryDlg` both walk vfn 7 to harvest every
item with their target class id (`0x7fb`/`0x802` respectively) — so
adding more poems/history pages to the master pack would
automatically populate these dialogs.

## 7. Implementation cheat-sheet

For a faithful re-implementation, the smallest reproducible "main
menu" requires:

1. **Boot driver** that:
   * Creates an 800×600 Win32 window.
   * Implements a frame-pumping event scheduler.
   * Owns a resource pool able to look up bitmap-sprite handles by ID
     (the unpacker emits PNG + JSON sidecars per
     `../formats/sprite_container.md`).

2. **Main menu screen**:
   * Logical rect 0..800 × 0..600.
   * Background bitmap `0x1013b` at (0, 0); right-side hero `0x10139`
     at (610, 0); bottom static "version" line at (230, 575).
   * 3 buttons stacked at x=35: y={37, 121, 205}, cmds {0xc9, 0xca,
     0xcb}, animation set {0x100d4 → 0x100d5}, paired icons at x=100
     y={55, 139, 223}.
   * 4 background `CRuch` actors spawning at random X within the
     window over the first ~8 s.
   * `CPoemScroller` cycling all `Poem` (class 0x7fb) resources in
     the lower-left.
   * Keyboard hotkeys: S=Start, H=History, K=Quit, X=close-app.

3. **Sub-screens** swapped in/out of the menu's child list (NOT modal
   pushes — the menu stays visible during them):
   * `CStartGame1` for choosing game-mode/connection/host vs. join.
   * `CHistoryDlg` for browsing the campaign story / about pages.
   * `CExitDlg` for the quit confirmation.

4. **Quit path**: `CExitDlg` first opens synchronously from the Quit/K
   button (`cmd 0xcb`, SFX slot `0x19`). The final app-exit command
   (`0x8004`, also used by the X hotkey / native close) routes through
   `CMenu_DispatchHotkey(this, 0x80cb, audioSlot=0x1a)`: stash
   `0x80cb` at `CMenu+0xcc`, play "Konec hry", hide main buttons, then
   let the audio completion event id `1` call `CMenu::OnEvent` and end
   the modal with `-0x7f35`. `CBulanci::OnEvent::0xcc` treats that as
   shutdown.

5. **Game launch path**: `CStartGame1` → `CMenu::OpenNetworkSession` →
   `CDSDirectPlay` setup → `CStartGame2` (lobby) → on Start click
   (`cmd 0x8003`) → `CGame::StartGame` → `CGaming` runs the match.
   Once the match returns, control unwinds back to the main menu
   without redrawing it from scratch (the `CMenu` instance persists
   in `CBulanci+0x280`).

## 8. Function map (Ghidra symbols)

Renamed in this pass for downstream readability:

### `CBulanci`

| Address | Name |
|---------|------|
| `0x00402490` | `CBulanci::OnEvent_MenuStateMachine` |
| `0x00402b20` | `CBulanci::CBulanci_OnCreate` |
| `0x00402180` | `CBulanci::InitResourceBank` |

### `CMenu`

| Address | Name |
|---------|------|
| `0x004251d0` | `CMenu::CreateObject` |
| `0x00424520` | `CMenu::ctor` |
| `0x004265e0` | `CMenu::ctor_with_ui` |
| `0x00424e70` | `CMenu::dtor` |
| `0x004245b0` | `CMenu::GetSingleton` |
| `0x00424fa0` | `CMenu::OnKeyDown` |
| `0x00424f50` | `CMenu::OnEvent` |
| `0x00425970` | `CMenu::Cmd_Dispatch` |
| `0x00423a00` | `CMenu::CloseCurrentSubScreen` |
| `0x00425870` | `CMenu::LoadBackgroundResource` |
| `0x00423f70` | `CMenu::EnableAllRuch` |
| `0x00424010` | `CMenu::EnableBackgroundState` |
| `0x00424080` | `CMenu::OnMusicFadeTick` |
| `0x00425400` | `CMenu::PollDayNight` |
| `0x004252f0` | `CMenu::SetDayNightBg` |
| `0x004252a0` | `CMenu::LoadBackgroundMusic` |
| `0x00424bc0` | `CSwitch::ctor` |
| `0x00424d30` | `_Globals::Button_Click` |
| `0x00425340` | `CMenu::DispatchHotkey` |
| `0x00423fa0` | `CMenu::SetButtonsHidden` |
| `0x00413030` | `CMenu::DoModalChild` |
| `0x004127d0` | `CMenu::ThrowModalDone` |
| `0x00412780` | `CMenu::CleanupSlots` |
| `0x00414640` | `CMenu::NetSendKickAll` |
| `0x00414790` | `CMenu::ShowLobby` |
| `0x00414170` | `CMenu::PickSession` |

### `CStartGame1`

| Address | Name |
|---------|------|
| `0x0040f400` | `CStartGame1::ctor` |
| `0x0040c650` | `CStartGame1::BuildUi` |
| `0x0040ae90` | `CStartGame1::OnEvent` |
| `0x0040ae30` | `CStartGame1::OnRadioChange` |
| `0x0040a860` | `CStartGame1::ApplyHostJoinVisibility` |
| `0x0040a970` | `CStartGame1::ApplyJoinModeVisibility` |
| `0x0040f4b0` | `CStartGame1::GetSingleton` |
| `0x0040c060` | `CTcpIpConfig::ctor` |

### `CStartGame2` / lobby

| Address | Name |
|---------|------|
| `0x004104f0` | `CStartGame2::ctor` |
| `0x00414dd0` | `CMenu::OpenNetworkSession` |
| `0x00413f30` | `CGame::StartGame` |
| `0x0041ff90` | `CGaming::ctor` |

### Dialogs

| Address | Name |
|---------|------|
| `0x0040e5f0` | `CAdvertising::ctor` |
| `0x0040fe30` | `CAdvertising::LoadSplashImage` |
| `0x004231d0` | `CHistoryDlg::ctor` |
| `0x00411b50` | `CExitDlg::ctor` |
| `0x004137b0` | `CMenu::ShowConnectingDialog` |

### Audio runtime helpers

| Address | Name |
|---------|------|
| `0x0043a060` | `CDSAudioPlayer::ApplyEffectiveVolume` |
| `0x0043a0d0` | `CDSAudioPlayer::SetVolumePercent` |
| `0x0043a9d0` | `CDSAudioPlayer::Play` |
| `0x0043a4a0` | `CDSAudioPlayer::Stop` |

## 9. Widget runtime details

### 9.1 `CSwitch` vftables (entry-by-entry)

`CSwitch::ctor @ 0x00424bc0` patches six vtables, with the same shape
that `CDSAnim` uses (see `../engine/anim_runtime.md`) **minus** the `+0x78`
anim-sub face. Slot meanings are:

#### Primary face — `+0x00` vftable `@ 0x004834d4` (28 slots)

| slot | addr        | role                                                 |
|-----:|-------------|------------------------------------------------------|
|    0 | `0x004239e0` | `GetTypeInfo()` → `&DAT_004b3970`                   |
|    1 | `0x00423b30` | `~CSwitch(deleteFlag)` — runs `FUN_00423950`, `free(this)` if flag&1 |
|    2 | `0x004245c0` | `CDSObject::dtor_outer` — calls `vfn[1](deleteFlag=1)` |
|    3 | `0x0042c2e0` | `CDSView::ChildrenWalk@+0xc` — sum `vfn[3]` over child chain at `this+0x54` |
|    4 | `0x0042c320` | `CDSView::ChildrenWalk@+0x10`                       |
|    5 | `0x0042c370` | `CDSView::ChildrenWalk@+0x14`                       |
|    6 | `0x004033a0` | `CDSApp::ReturnZero()`                               |
|    7 | `0x004028a0` | `CDSView::HitTest(point)` — `(this+0x30..+0x3c) ⊇ pt` |
|    8 | `0x0042cae0` | `CDSView::AdaptDisplaySize(view)` — viewport recalc on resize |
|    9 | `0x0042ca30` | `CDSView::Draw(clipRect, force)`                     |
|   10 | `0x0042c580` | `CDSView::ComputeAnchoredRect(out, parentSize)` (anchors via `this+0x48`) |
|   11 | `0x0042c480` | `CDSView::SetRect(rect)` — updates `+0x20..+0x2c`, cascades to children |
|   12 | `0x0042c430` | `CDSView::GetParentBounds(out_origin, out_size)`     |
|   13 | `0x0042c3e0` | `CDSView::IsModalDoneRecursive`                      |
|   14 | `0x0040b840` | **`Tick()`** — `TM_TickBlit(this+0x68)` (animation frame blit) |
|   15 | `0x00438340` | `_purecall`                                          |
|   16 | `0x00438340` | `_purecall`                                          |
|   17 | `0x00424cf0` | **`OnMouseEnter()`** — if `state==0`, `FUN_00423f20` (highlight) |
|   18 | `0x00424d10` | **`OnMouseLeave()`** — if `state==0`, `FUN_00423ef0` (dim) |
|   19 | `0x004239f0` | **`OnFocusGain(_, gainedFocus)`** — `gainedFocus=1` → `FUN_0042c860` (claim modal focus) |
|   20 | `0x00425260` | **`OnMouseClick(_, downFlag)`** — `downFlag&1` + hit-test → `Button_Click` |
|   21 | `0x00416770` | (empty stub)                                         |
|   22 | `0x0042c0c0` | `CDSView::OnKeyDown` (forwards to child at `+0x50`)  |
|   23 | `0x0042c0e0` | `CDSView::OnKeyUp`                                   |
|   24 | `0x0042c100` | `CDSView::OnChar`                                    |
|   25 | `0x0042c770` | `CDSView::Broadcast(msg, p1, p2)` — fans a custom msg through children |
|   26 | `0x0042c7d0` | `CDSApp::OnEvent_Default`                            |
|   27 | `0x00424dc0` | **`OnCustomMsg(msgId, sender)`** — `msgId==200` from sibling of same type → clear `state[0xc4]` + `FUN_00423ef0` |

#### `IDSChained` face — `+0x04` vftable `@ 0x004834b8` (6 slots)

| slot | addr        | role                                                 |
|-----:|-------------|------------------------------------------------------|
|    0 | `0x004011f0` | `GetTypeInfo()` → `&DAT_004b7cd8` (CDSChain RTTI)   |
|    1 | `0x0042ac90` | `IdentityCast` — returns `this-4`                    |
|    2 | `0x0041aa40` | `CDSObject::dtor_outer` thunk via `this-4`           |
|    3 | `0x00423b00` | `~CSwitch` thunk — `dtor(this-4, flag)`              |
|    4 | `0x00438340` | `_purecall`                                          |
|    5 | `0x00438340` | `_purecall`                                          |

#### `IDSEventHandler` face — `+0x10` vftable `@ 0x004834a0` (5 slots)

| slot | addr        | role                                                 |
|-----:|-------------|------------------------------------------------------|
|    0 | `0x00402880` | `GetTypeInfo` (base)                                 |
|    1 | `0x004049c0` | `CDSChain` thunk — `this-0xc → CDSChain::IdentityCast` |
|    2 | `0x004034a0` | `CDSView::dtor_outer_thunk` — calls `CDSObject::dtor_outer(this-0x10)` |
|    3 | `0x00423ae0` | `~CSwitch` thunk — `dtor(this-0x10, flag)`           |
|    4 | `0x0042c040` | **`CDSView::DispatchEvent(evt)`** — switches on `evt->msgId`: `0x100`→primary[26] (`OnCmd`), `0x200`→primary[27] (`OnCustom`), `0x400`→primary[25] (`OnBroadcast`). The *only* path scheduled messages take into the widget. |

#### `IDSReferenced` face — `+0x18` vftable `@ 0x0048348c` (4 slots)

| slot | addr        | role                                                 |
|-----:|-------------|------------------------------------------------------|
|    0 | `0x00402890` | `GetTypeInfo` (base)                                 |
|    1 | `0x004049d0` | Ref-count thunk                                       |
|    2 | `0x00423db0` | `CDSView::dtor_outer_thunk` — `this-0x18`            |
|    3 | `0x00423af0` | `~CSwitch` thunk — `dtor(this-0x18, flag)`           |

#### `IDSUpdated` face — `+0x68` vftable `@ 0x00483478` (4 slots)

| slot | addr        | role                                                 |
|-----:|-------------|------------------------------------------------------|
|    0 | `0x0040b820` | `CBulAnim::GetTypeInfo_Sub68` → `&DAT_004b82fc`     |
|    1 | `0x00404de0` | `CScroller`-shared `CDSChain::IdentityCast(this-0x64)` |
|    2 | `0x004229a0` | `CScroller`-shared `CDSObject::dtor_outer(this-0x68)`|
|    3 | `0x00423ad0` | `~CSwitch` thunk — `dtor(this-0x68, flag)`           |

#### `IDSAnim` face — `+0x6c` vftable `@ 0x00483460` (5 slots)

Same shape as `CDSAnim::IDSAnim` (slots 0..3 are the shared engine
defaults). Only slot 4 is overridden:

| slot | addr        | role                                                 |
|-----:|-------------|------------------------------------------------------|
|    0 | `0x00438f20` | `DispatchUserEvents` (shared with `CDSAnim`)        |
|    1 | `0x00438f60` | `BindUserData` (shared)                              |
|    2 | `0x004391d0` | `OnSequenceBound` (shared)                           |
|    3 | `0x00438f80` | `OnFinished` (no-op, shared)                         |
|    4 | `0x00424e10` | **`CSwitch::OnAnimEnd(_, frameIdx)`** — fires when the press/release animation finishes (`frameIdx == -1`). If `state==1` *and* (mouse-over or `+0xa8 != 0`) → snap visual back to the **pressed** track (`FUN_00423f20`). Otherwise → snap back to the **idle** track (`FUN_00423ef0`). Equivalent to "after the transition animation, snap visual to whatever the live input state says". |

#### Track semantics

The ctor loads two animation resources into the track manager at
`this+0x7c`:

```
track 0 ←  resource 0x100d4   — idle / button-not-pressed visual
track 1 ←  resource 0x100d5   — pressed / hover visual
SetCurrentTrack(0, /*reset*/1) starts on the idle anim.
```

So `FUN_00423f20` (`SetCurrentTrack(0)` + play + side-effect) is
"highlight on" and `FUN_00423ef0` (`SetCurrentTrack(1)` + play) is
"highlight off". Click flow:

```
mouse-down + hit ─▶ primary[20] OnMouseClick ─▶ Button_Click
Button_Click ─┬─▶ state[0xc4] = 1
              ├─▶ Scheduler_PostMessage(parent, msg=0x400, payload=btn, delay=200ms)
              │         the parent's primary[25] (OnBroadcast) fans this as msg 0x200
              │         to siblings → each sibling's primary[27] runs OnCustomMsg
              │         (clears their state[0xc4] = "radio" coordination)
              └─▶ Scheduler_PostMessage(parent, msg=0x100, payload=0, delay=cmd_id)
                        parent CMenu's primary[26] runs CMenu::Cmd_Dispatch(cmd_id)
```

The 200 ms (`0xc8`) "visual ACK" message piggybacks the button-id as
the value used by sibling buttons' `OnCustomMsg` (slot 27) to detect
"another radio-button in my group was pressed". The exclusivity check
is `siblings[+0x78] == this[+0x78]` (same `type` byte) **and**
`sender != this`.

### 9.2 `CRuch` per-instance lifecycle

`CRuch::ctor @ 0x00423c20` builds a **vertical-scanline visual effect**
— not a sprite. Five vftables `{+0x00, +0x04, +0x10, +0x18, +0x68}`,
size 0x80.

Rect initialised to `(rx, 0, rx+1, appH)` where `rx = rand()*appW>>15`
— a **1-pixel-wide column** spanning the full screen height.
The construction hides the column and arms a scheduler event:

```
Scheduler_RegisterEventSlot(this+0x68, slot=0,
                            delay = rand()*0xfa1 >> 15,   /* [0..4000] ms */
                            event=6);
```

#### IDSUpdated face (`+0x68` vftable `@ 0x0048357c`, 5 slots)

| slot | addr        | role                                                 |
|-----:|-------------|------------------------------------------------------|
|    0 | `0x004034d0` | `CScroller::GetTypeInfo` → `&DAT_004b7c40`          |
|    1 | `0x00404de0` | `CDSChain::IdentityCast(this-0x64)`                  |
|    2 | `0x004229a0` | `CDSObject::dtor_outer(this-0x68)`                   |
|    3 | `0x00423d90` | `~CRuch` thunk                                       |
|    4 | `0x00423de0` | **`CRuch::OnScheduledTick(this+0x68_face)`** — see below |

#### Scheduler tick (`FUN_00423de0`, fires on event 6)

```c
if (this[+0x44] & 1) {        /* currently visible */
    Scheduler_SetEventDelayMs(this+0x68, slot=0,
                              delay = rand()*0xfa1 >> 15);  /* up to ~4 s */
    Hide(this);                /* clear bit 0 of +0x44 */
    appW = g_pApp[0x28] - g_pApp[0x20];
    new_x = (rand() * appW) >> 15;
    MoveRect(this, new_x, 0);  /* primary vfn[8] keeps w×h, shifts to (x, 0) */
} else {                       /* currently hidden */
    Scheduler_SetEventDelayMs(this+0x68, slot=0, 100);  /* 100 ms */
    Show(this);                /* set bit 0 of +0x44 */
}
```

So each `CRuch` cycles **visible-for-100ms → hidden-for-rand[0..4s] →
teleport-to-new-x → visible-for-100ms → …** — a brief vertical
flash at a random X every few seconds, four instances running in
parallel.

#### Render — primary vftable slot 14 (`FUN_00423e60`)

`CRuch`'s primary slot 14 *replaces* the engine's `Tick` with a direct
line-draw call:

```c
CPoemScroller::DrawVerticalLine(g_pApp+0x80,
    x   = this[+0x30],         /* rect.left  */
    y   = this[+0x34],         /* rect.top   */
    h   = this[+0x3c],         /* rect.bottom */
    color1 = 0x3f3f3f,         /* gradient endpoint A (dark gray) */
    color2 = 0xffffff);        /* gradient endpoint B (white)     */
```

`CPoemScroller::DrawVerticalLine @ 0x00436420` (its real role —
"vertical column rasteriser") clips to the engine's drawing context
rect (`this+0x24..+0x30`) and dispatches per pixel-format
(`FUN_00445740/00445900/00445a70`) with the two endpoint colors.

So `CRuch` is the **scanline / phosphor-glow** effect in the menu
background, not a moving "Bulanek" sprite as the Czech name suggests
("ruch" can mean motion *or* noise/static).

`CMenu::EnableAllRuch @ 0x00423f70` toggles all four instances' event
slots when the menu is shown/hidden.

### 9.3 `CPoemScroller` text shaping and scroll speed

`CPoemScroller::ctor @ 0x004262c0` builds the post-menu lyrics scroller
at rect `(10, 384, 215, 550)` (`0xa, 0x180, 0xd7, 0x226`). It scans the
resource pool for **all class `0x7fb` (Poem)** resources and appends
them to its chain at `this+0x80`. After ctor:

```
Scheduler_RegisterEventSlot(this+0x68, slot=0, delay=0x78, event=6);
Scheduler_SetEventLastFireMs(this+0x68, slot=0, g_dwElapsedMs + 0xbb8);
   /* first scroll-step in 3 seconds (0xbb8 ms),
      then a step every 0x78 = 120 ms */
```

→ steady **1-pixel-per-120 ms** upward scroll (~8.3 px/s) starting
3 seconds after the menu opens.

#### `FUN_00425df0 — PickNextPoem(this)`

```c
poem_count = this[0x88];                          /* size of the +0x80 chain */
idx        = (rand() * poem_count) >> 15;
this[0x90] = idx;
this[0xac] = this[0x2c] - this[0x24];             /* scroll Y = view height (start below) */

poem = this[0x80][idx];                           /* selected Poem resource */

/* Pass 1: measure */
FUN_004375e0(poem->wstr, &measureRect, NULL, &font_info_at_+0x94);
this[0xb0] = -measureRect.bottom;                 /* end-scroll position (one full text above top) */

/* Allocate render surface (CDSBmpImage 0x60) sized to the text */
surface = CDSBmpImage::Create(&measureRect, g_pApp[0x80].pixelFormat);
Release(this[0xb4]); this[0xb4] = surface;
FillSurface(surface, color=0xffffff);             /* white background */

/* Pass 2: rasterise the wstr glyphs onto surface */
FUN_004375e0(poem->wstr, &measureRect, surface, &font_info_at_+0x94);

stride = surface[+4];

/* Allocate two 0x28-row gradient masks for soft fade-in/fade-out */
this[0x118] = malloc(stride * 0x28);              /* upper-edge alpha ramp */
this[0x11c] = malloc(stride * 0x28);              /* lower-edge inverse ramp */
for (row = 0x28 - 1; row >= 0; row--) {
    memset(this[0x118] + row*stride, 6*(row+1), stride);   /*   6,12,…,0xf0 */
    memset(this[0x11c] + row*stride, 0x100 - 6*(row+1), stride);
}
```

#### `FUN_00426030 — OnScrollTick(this+0x68_face)`  (fires every 120 ms)

```c
this[+0x44]--;                          /* note: relative to face, == this[+0xac] absolute */
                                        /*   == scrollY, decreases (text moves up) */
if (this[+0xac] == this[+0xb0])         /* reached end of clip → wrap */
    PickNextPoem(this);

InvalidateView(this);                   /* primary[9] = repaint */
```

#### Per-frame render (`FUN_004240f0`, primary slot 14)

Splits the visible rect into three bands and blits per band, applying
the gradient masks where the text edges cross the top / bottom 40-pixel
fade bands. Effect: scrolling text with **40-pixel soft fade-in at the
top** and **40-pixel soft fade-out at the bottom**, like an old-school
credits roll.

#### Line-break / alignment encoding

The shaper `_Globals::FUN_004375e0(wstr, &bbox, surface, &font_info)`
treats the wide string as a tiny markup language. There is **no
automatic word wrap** — lines break exactly on `'\n'`:

| character | role                                                        |
|-----------|-------------------------------------------------------------|
| `0`       | end-of-string (terminates outer loop)                       |
| `\n` (0x0a)| line break (start a new line)                              |
| `\t` (0x09)| advance X by 4 spaces' worth                               |
| `0x01`    | **per-line center-align** marker (sets `line.align = 1`)    |
| `0x02`    | **per-line right-align** marker (sets `line.align = 2`)     |
| else      | glyph; advance X by `glyph_width + font_info[+4]` (`char_spacing`) |

`font_info[+0]` is a flags dword: bit `0x10` "compute bbox", bits 2..3
v-align (`0` top / `4` center / `8` bottom), bits 0..1 h-align of the
whole block within the surface.

A typical Czech poem in the resource pool looks like:

```
\1Title line\n          ← centered
Normal verse line\n
\2by Author\n           ← right-aligned
\0
```

So the *line-break* algorithm is the trivial `'\n'`-splitter, the
*scroll-speed* algorithm is `1 px / 120 ms` with a 3-second startup
delay, and the *fade* algorithm is two pre-baked 40-row alpha ramps
blitted on top of the moving text.

### 9.4 The "is daytime" predicate

`CMenu::PollDayNight @ 0x00425400` calls `BuildLocalDateTime @
0x0042e8b0`, which packs `GetLocalTime()` into a 2-dword buffer via
two helpers (`FUN_0042e850` / `FUN_0042e880`). The first dword is
built by `FUN_0042e6c0(out, hour, minute, second, ms)`:

```
*out = (((hour & 0x7f) << 7 | (minute & 0x7f)) << 7 | (second & 0x7f)) << 11
     | (millisecond & 0x7ff);
```

Expanding:

```
bits  0..10  millisecond  (11 bits, 0..2047)
bits 11..17  second       ( 7 bits, 0..127)
bits 18..24  minute       ( 7 bits, 0..127)
bits 25..31  hour         ( 7 bits, 0..127)  ← read by FUN_00425400
```

Confirmed: **`dword >> 25` is `SYSTEMTIME.wHour` (local time)**.

The predicate is `is_night = (uint32)((hour - 6) > 15)` — which
unsigned-wraps to **night iff `hour ∈ {0,1,2,3,4,5,22,23}`**, i.e.
**day = 06:00–21:59 local time**. The background ambient audio loop
resources are `0x10149` (day) / `0x1014a` (night), swapped by
`CMenu::SetDayNightBg`. The base background image `0x1013b` is unchanged.

### 9.5 Right-Side Hero Art Transparency & "Fake Transparency" Rendering

In the main menu UI, when `this->hero_bmp` (resource `0x10139`, `res_0000065849_21_BitmapJPEG.jpg`) is shown on the right side of the screen at position `(610, 0)`, it overlaps with the right edge of the main menu's rounded frame border (which is drawn as part of the background bitmap `0x1013b` at `(0, 0)`).

A curious visual artifact occurs where the rounded frame's border turns flat gray where it intersects with the hero art image. Through deep reverse engineering of the image loading, construction, and blitting pipeline, we can fully account for this:

1. **Asset Nature (Opaque JPEG):** The hero art is stored and loaded as a standard JPEG file. Because the JPEG format does not support alpha transparency or masks, the image is physically rectangular and includes a solid flat-gray background surrounding the character sprites.
2. **Object Initialization:** When the JPEG is loaded via `CDSJpegImage::DecompressToImage @ 0x00431b70`, it initializes the underlying `CDSImage` (pixel format 5, 24bpp BGR) via `_Globals::FUN_00436f40`. This default initialization sets:
   - `colorkey = 0xFFFFFFFF` (no chroma/color keying)
   - `alpha = 0xFF` (100% fully opaque)
   - `pMaskBuffer = NULL` (no transparency mask)
3. **Blitter Selection:** When `CDSView_RenderChildren` processes the menu view's child nodes, it schedules the background drawing of `this->hero_bmp` by calling `_Globals::TM_TickBlit @ 0x00439080` (polymorphically bound to slot 14 in `CDSBitmap`'s vtable). This delegates to the master blit dispatcher `CPoemScroller::BlitDispatch @ 0x004368d0`.
4. **Decision Tree:** Since `colorkey == 0xFFFFFFFF`, `alpha == 0xFF`, and `pMaskBuffer == NULL`, `BlitDispatch` routes the drawing to the completely opaque blitting table `BlitTable_Opaque @ 0x004b08c8`. For `src=5, dst=5` (24bpp source to 24bpp destination backbuffer), it dispatches to the specialized opaque kernel `_Globals::FUN_0043df90`.
5. **The Artifact Mechanism:** Because the image is drawn completely opaquely with a flat-gray background, it directly overwrites the pixel data of the background frame border that lies underneath its bounding rectangle. The "opacity" is actually a clever **fake transparency** design shortcut typical of software-rendered games of the era (early 2000s):
   - The designers painted the JPEG's background with the exact same RGB color as the flat areas of the main menu background.
   - When rendered, the flat areas blend seamlessly.
   - However, where the JPEG's rectangular boundary intersects non-flat elements like the rounded frame borders, it simply erases them and paints them flat gray.

### 9.5 Where the level list `+0xbc` comes from

The "lobby level list" lives at **`CGame+0xbc`** (array of name
pointers) with count at `CGame+0xc4`. `CGame` is **not** an alias for
`CStartGame1` (which is only 0xa8 bytes and has no `+0xbc` field).
`CGame` is the embedded "match state" sub-object inside `CBulanci` at
`g_pApp+0x284` (size ≈ `0x248` bytes, reaching to `CBulanci+0x4cc`).

`CStartGame2::ctor` (the lobby) reads it via the `parent` pointer it
saves at `this+0xa8`:

```c
for (i = 0; i < parent[0xc4]; ++i) {
    name = parent[0xbc] + i*4;     /* int pointer at +0xbc holds name list base */
    CLevelList::AddItem(this[0xb4], FUN_00413560(parent, name));
}
```

Population happens **inside `CMenu::OpenNetworkSession @ 0x00414dd0`**
on the *host* path, *before* it calls `CMenu::ShowLobby`:

```c
*(int*)(cgame + 0xc4) = 0;                       /* clear level count */
n = *(int*)(cgame + 0x6e);                       /* source-array count    */
src = *(int*)(cgame + 0x66);                     /* source-array base ptr */
for (i = 0; i < n; ++i) {
    name = ((Resource*)src[i])->name;             /* +0x10 = name field */
    FUN_00414920(cgame + 0xbc, /*flags*/ 1, ...);
}
```

`FUN_00414920` is the chain-append helper: it copies the wstring (via
`FUN_0042d490`) into the chained array at `+0xbc`, growing
`+0xc4` ("count") by 1.

The source array (`+0x66`/`+0x6e`) is **not** populated by a "scan on
entering `CStartGame1`" as previously hypothesised — `CStartGame1::ctor`
is pure UI construction. Instead, `+0x66/+0x6e` is set up earlier in
the app boot (during `CBulanci::InitResourceBank @ 0x00402180` and the
engine's class-registry callbacks `FUN_0042f4b0`, which register every
resource of the appropriate class into per-kind tables). No direct
`MOV [ebx+0x66], …` store is emitted — the writer uses a different
base register pointing into the field (so the offset surfaces as e.g.
`[edi+0x66]` from inside a class-registry helper rather than as a
named field of `CGame`). The end result is a pre-scanned array of
level-class `Resource*` available for `OpenNetworkSession` to consume.

The *join* path of `OpenNetworkSession` *also* reads `+0x66`/`+0x6e`
(to encode the local player roster into the JOIN packet), but the
level list itself arrives over the wire as DirectPlay message type
`0x05` (`SetLevel`), which the receiver applies to `+0xbc`.

## 10. Menu audio cues (resolved)

The CMenu hotkey / dispatch path plays a voice cue from the global
`CDSAudioBankIndex` at `g_pApp+0x4c0` (resource `0x10004` →
partner bank `0x10152`, 41 samples, mono 16-bit PCM @ 22050 Hz).

Both resources are already pulled out by the unpacker
(`tools/bulanci_unpack/bulanci_unpack.py`) and live under
`unpacked/`:

| Role | Resource ID | Unpacked file |
|------|-------------|---------------|
| `CDSAudioBankIndex` (slot table) | `0x10004` = `65540` | [`unpacked/res_0000065540_67_AudioBankIndex.bank.json`](../../unpacked/res_0000065540_67_AudioBankIndex.bank.json) |
| `CDSAudioBank` (PCM blob, 22050 Hz mono 16-bit) | `0x10152` = `65874` | [`unpacked/res_0000065874_43_AudioBank.wav`](../../unpacked/res_0000065874_43_AudioBank.wav) |

`bulanci_unpack.py` wraps the bank's raw PCM in a RIFF/WAVE
container (44-byte header), so the PCM range for slot `N` inside
the `.wav` file is `[44 + samples[N].offsetInBank,
44 + samples[N].offsetInBank + samples[N].byteLen)`.

Five slots are used by the main menu and were resolved by tracing
`Cmd_Dispatch`'s `PUSH <slot>` instructions back to their
`TriggerBankSample(0x1, 0, slot, ...)` call site:

| Slot | Sample # | Offset in PCM | byteLen | ms | Used by | Trigger | Plays |
|------|----------|---------------|---------|----|---------|---------|-------|
| `0x18` | 24 | `0x07b4d8` (505048) | 75586 | 1713 | `Cmd_Dispatch` case `0xca` (PUSH @ `0x00425b03`) | History button click (or `H` key) | "Historie" cue |
| `0x19` | 25 | `0x0dac9a` (580634) | 72558 | 1645 | `Cmd_Dispatch` case `0xcb` (PUSH @ `0x00425a3d`) | Quit button click (or `K` key) | "Konec" (quit-confirm) cue |
| `0x1a` | 26 | `0x09f608` (653192) | 91844 | 2082 | `DispatchHotkey` for cmd `0x8004` (PUSH @ `0x00425c76`) | `X` keyboard hotkey | "Konec hry" (force-exit) cue — stashes modal exit `0x80cb` |
| `0x1b` | 27 | `0x0b5dcc` (745036) | 85752 | 1944 | `Cmd_Dispatch` case `0xc9` (PUSH @ `0x00425baf`) | Start button click (or `S` key) | "Start hry" cue |
| `0x1c` | 28 | `0x0caca4` (830788) | 73214 | 1660 | `DispatchHotkey` for cmd `0x80ce` (PUSH @ `0x00425c6d`) | `F12` keyboard hotkey | (alternate exit cue) — stashes modal exit `0x80c8` |

(Indexed via `bank.json["samples"][24..28]`. ms = `byteLen /
(22050 * 2)` rounded.)

Two semantics:

1. **S/H/K (slots 0x1b / 0x18 / 0x19)**: `TriggerBankSample(1, 0, slot,
   0, 0, 1)` is called with `param_5 = 0` — the audio plays purely
   as a confirmation SFX while the sub-screen is built synchronously.
   No completion callback.
2. **X / F12 (slots 0x1a / 0x1c)**: `TriggerBankSample(1, 0, slot, 0,
   this+0x10, 1)` is called via `DispatchHotkey` with
   `param_5 = this+0x10`. The CDSAudioPlayer posts event id `1` to
   `CMenu::OnEvent` when the sample ends; that handler reads the
   stashed modal-exit code from `this+0xcc` and trips the modal
   unwind via `_Globals::FUN_0042c3c0`. See §2.6.1 for the full
   chain and failure modes.

For a faithful re-implementation, slots 0x18..0x1c are the PCM
ranges listed above inside
`unpacked/res_0000065874_43_AudioBank.wav`; wire them 1:1 to the
event triggers and the menu behaves identically.

To audition a slot, use any tool that can carve a byte-range out
of the WAV's data chunk — e.g.::

    # Python: extract slot 0x18 ("Historie") into its own playable WAV
    import json, struct, pathlib
    bank = pathlib.Path("unpacked/res_0000065874_43_AudioBank.wav").read_bytes()
    idx  = json.loads(pathlib.Path(
        "unpacked/res_0000065540_67_AudioBankIndex.bank.json").read_text())
    s    = idx["samples"][0x18]
    pcm  = bank[44 + s["offsetInBank"] : 44 + s["offsetInBank"] + s["byteLen"]]
    # Re-wrap pcm in a fresh 22050 Hz mono 16-bit RIFF/WAVE header for playback.

### 10.1 First-boot "auto-click" mechanism (the startup bark)

When `bulanci.exe` is launched, players hear an audio effect (a short bark sound) play exactly once before the main menu is fully interactive. This startup sound has been tracked down via decompilation to **`sample_27.wav`** (Audio Slot `0x1b`), which is the Czech voice cue for **"Start hry"** (Start Game).

The complete, deterministic execution flow leading to this sound being played on startup is as follows:

1. **Initialization**:
   When the `CBulanci` application is constructed (`CBulanci_ctor` at `0x004026f0`), the flag `lastSplashShown` (at `this[0x4c8]`) is initialized to `0`.

2. **Splash Screen Display**:
   In `CBulanci_OnCreate (0x00402b20)`, the application posts event `0xf7` to show the `CAdvertising` splash screen (with the `IWANNAPLAY.COM` logo).

3. **Menu Load Transition**:
   Upon splash screen dismissal (either via timer tick or click), event `0xcc` is processed by `CBulanci::OnEvent_MenuStateMachine (0x00402490)` to load the main menu. It constructs the menu by calling:
   ```cpp
   this_00 = CMenu_ctor_with_ui(pCVar4, param_2, this[0x4c8]);
   ```
   Since `this[0x4c8]` was initialized to `0`, `CMenu_ctor_with_ui` receives `param_3 = 0`.

4. **Programmatic "Auto-Click"**:
   At the very end of `CMenu_ctor_with_ui` (at `0x00426c85` / `0x00426c8e`), the menu checks the `param_3` (stashed initial-focus index / `lastSplashFlag`):
   ```assembly
   00426c85  MOV  EAX, dword ptr [ESP + 0x38]   ; parameter 3 (lastSplashFlag)
   00426c89  CMP  EAX, 0x2
   00426c8c  JA   0x00426c9a
   00426c8e  MOV  ECX, dword ptr [ESI + EAX*0x4 + 0xb0]
   00426c95  CALL 0x00424d30  ; Button_Click
   ```
   Because `param_3` is `0` (which is <= 2), it retrieves the Start button pointer at `this + 0xb0` (`this->btn_start`) and passes it to `_Globals::Button_Click` (at `0x00424d30`).

5. **Delayed Command Dispatch**:
   `Button_Click` schedules and posts a delayed message (`0x100`) to the parent menu's event handler with the button's command ID, which for the Start button is `0xc9`.

6. **Triggering Start Game Screen & SFX**:
   When the event pump delivers message `0x100` / command `0xc9`, `CMenu::CMenu_CmdDispatch(this, 0xc9)` is executed. It performs the transition to the first-stage game lobby screen:
   - It closes the current sub-screen (`CMenu_CloseCurrentSubScreen`).
   - It loads the background bitmap resource `0x1013c` (the red Start Game background).
   - It constructs and attaches the `CStartGame1` sub-screen.
   - Crucially, it plays the audio confirmation cue for the Start button:
     ```cpp
     TriggerBankSample(1, 0, 0x1b, 0, 0, 1);
     ```
     Audio slot `0x1b` corresponds directly to `sample_27.wav` in the main audio bank `res_0000065874_43_AudioBank.wav` (Resource `0x10152`). This plays the sound.

7. **Retention of Sub-Screen**:
   At the end of `CMenu_CmdDispatch` for case `0xc9`, it checks if `this[0xe0]` (the stashed `param_3`) is non-zero. Since `this[0xe0]` is `0` (from first boot), the application does *not* post a back message and happily remains on the newly opened `CStartGame1` screen with the voice/bark cue playing!

## 11. Remaining open questions

* Exact location of the writer that populates `CGame+0x66 / +0x6e`
  (the level-resource source array). Static search finds no
  `MOV [reg+0x66], …` — likely via a class-registry callback whose
  base register isn't named `CGame*`. A dynamic / breakpoint pass
  would pin it in seconds.
* Whether resource `0x100d4` is the idle-track or the hover-track
  visually (we have the resource IDs and the track-index mapping; the
  pixel content classification only requires unpacking the FLX).
* Confirmation of `CGame` total size (the embedded sub-object inside
  `CBulanci+0x284`). Field reach goes up to at least `+0x1dc` (the
  `CDSDirectPlay*`), so size ≥ `0x1e0`. The remaining tail
  (`0x4cc - 0x284 - 0x1e0 = 0x68`) is presumably reception buffers.
