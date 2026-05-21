# Gameplay Subsystems Status

Confidence levels:
* **Empty** — not yet investigated.
* **Sketched** — high-level shape only; details TBD.
* **Partial** — most of the surface mapped; specific corners uncertain.
* **Verified** — Ghidra-cross-checked: layouts, dispatch tables and callsites are tied to specific addresses in `bulanci.exe`.

---

## Game Logic & State Machines

These cover game rules, entities, combat mechanics, UI screens, menus, and high-level orchestration.

### Player entity (Bulánci)

* **Classes:** `CBulanci`, `CBulanek`
* **Bytes:** 37.2 KB
* **Confidence:** **Empty**
* **What's known:** `CBulanci` alone is 30.7 KB / 140 funcs — the heaviest game class. `CBulanek` (6.5 KB / 38) is the per-team-slot peer.
* **Open questions:**
  * State machine: spawn → move → shoot → die.
  * Inventory/weapon switching.
  * Network replication.
  * Where script callbacks (`OnTimer`, `OnEnter`/`OnLeave`) feed into the entity.
* **Artefacts:** —

### Combat / projectiles

* **Classes:** `CWeapon`, `CShot`, `CMina`, `CSpells`, `CExplosion`, `CTeleportPoint`
* **Bytes:** 5.1 KB
* **Confidence:** **Verified**
* **What's known:** Primary combat flow, bullet spawning, deployable mine triggers, area-of-effect raycasting, power-up effects, and player teleportation fully reversed and cross-referenced in Ghidra. All key functions renamed.
* **Open questions:** None.
* **Artefacts:** `./combat_projectiles.md`

### Match orchestration

* **Classes:** `CGame`, `CGaming`, `CLevelList`, `CLoadingLevel`, `CLevelScore`
* **Bytes:** 5.8 KB
* **Confidence:** **Verified**
* **What's known:** full match state machine (`CGame::m_state` transitions 4 -> 5 -> 6), the loading progress updates in `CLoadingLevel` (with progress bar at offset `0x70`), level music loader (`CDSWavStream` and `CDSAudioPlayer`), frame tick update orders (Scheduler Events: 0 is Ambient Anims, 1 is Pickup Spawner), and local persistent Top 6 High Score tracking via `CLevelScore` (`CScoreItem` collection).
* **Open questions:** None; match state transitions, timer-loop event-routing, and high-score rankings are fully reverse-engineered and mapped.
* **Artefacts:** `../gameplay/match_orchestration.md`, `../engine/script_lifecycle.md`

### Menu / start state machine

* **Classes:** `CMenu`, `CStartGame1`, `CStartGame2`, `CGunMouse`, plus the satellite dialogs `CAdvertising`, `CHistoryDlg`, `CExitDlg`, `CTcpIpConfig`, `CSessionList`, `CMsgDialog`, `CRuch`, `CPoemScroller`
* **Bytes:** 15.8 KB
* **Confidence:** **Verified** (full screen-graph, layouts, widget vtables, and custom sniping cursor mechanics mapped and synchronized with C++ sources)
* **What's known:**
  * **State machine driver lives in `CBulanci`**, not in `CMenu`. `CBulanci::OnEvent_MenuStateMachine @ 0x00402490` handles three scheduler events:
    * `0xf7` — show `CAdvertising` splash (resource `0x1013a`) modally, then schedule `0xcc` 256 ms later. Bootstrapped by `CBulanci_OnCreate @ 0x00402b20` via `Scheduler_PostMessage(this+0x10, 0x100, 0xf7, 0, 0)`.
    * `0xcc` — alloc `CMenu` (size `0xe4`), call **`CMenu_ctor_with_ui @ 0x004265e0`**, `CDSView_DoModal(menu, this)`. Modal exit codes `-0x7ffc / -0x7f35` quit the app (`FUN_0042c3c0(0x8004)`); `-0x7f38` schedules `0xcd`.
    * `0xcd` — confirms exit via `FUN_00401be0`; if user said "no", reschedules `0xcc` so they land back in the main menu.
  * **`CMenu` is NOT the entire main menu** — it is a `0xe4`-byte `CDSChained` widget that owns:
    * 3 menu buttons (`CSwitch`, sizeof `0xc8`) stacked at x=35, y={37, 121, 205}, cmds `{0xc9 Start, 0xca History, 0xcb Quit}`. Two-frame animations from resources `0x100d4` / `0x100d5`.
    * 6 paired button icons (Start / History / Quit × normal / highlight) at x=100, y={55, 139, 223}, IDs from `g_kMenuIconRes_*` (`0x10093/0x10094 0x10043/0x10044 0x1004b/0x1004c` @ `0x004af90c..0x004af920`).
    * Background bitmap (resource `0x1013b`) at `(0, 0)`; right-side hero art (`0x10139`) at `(610, 0)`; bottom-centre version line from static pool index 93.
    * `CPoemScroller` (sizeof `0x128`) walking every `Poem` (class id `0x7fb`) resource — the scrolling credits.
    * 4× `CRuch` background actors that animate across the screen at random intervals (~8 s window).
    * Day/Night background toggle via `CMenu_PollDayNight @ 0x00425400` → `CMenu_SetDayNightBg @ 0x004252f0` swapping resources `0x10149` (night) / `0x1014a` (day).
    * Keyboard shortcuts (`CMenu_OnKeyDown @ 0x00424fa0`): `S`=Start, `H`=History, `K`=Quit (Czech "Konec"), `X`=app-exit.
  * **`CMenu_CmdDispatch @ 0x00425970`** is the routing centre. Sub-screens are added as **child views (not modal pushes)** so the main menu stays under its own DoModal:
    * `0xc9` → `CStartGame1::ctor @ 0x0040f400` (size `0xa8`).
    * `0xca` → `CHistoryDlg::ctor @ 0x004231d0` (size `0x9c`).
    * `0xcb` → `CExitDlg::ctor @ 0x00411b50` (size `0x7c`).
    * `0xcf` → close current sub-screen (`CMenu_CloseCurrentSubScreen @ 0x00423a00`).
    * `0x8004` / `0x80ce` → keymap helpers via `0x00425340`.
  * **`CGunMouse` Custom Sniping Cursor**: A `0x218` (536 bytes) custom software-rendered mouse cursor that features full-screen horizontal and vertical crosshair red lines, a center reticle sprite, and a lagging inner red dot driven by a 30-element coordinate history queue (300ms ring buffer) and spring physics with drag momentum displacement.
  * **`CStartGame1::BuildUi @ 0x0040c650`** assembles four `CRadio` groups (game-type, connection, rounds, host/join) plus headers and a single "Back" `CButton` (cmd `0xcf`). The screen auto-advances when all radios are set — there is no Continue button. Radio callbacks dispatched by `CStartGame1_OnRadioChange @ 0x0040ae30` handle conditional row visibility.
  * **`CMenu_OpenNetworkSession @ 0x00414dd0`** is the bridge into the actual game. It stands up `CDSDirectPlay` at `CMenu+0x1dc`, either hosts (`HostSession(GetComputerNameW())`) or joins (push `CTcpIpConfig` modal → `JoinSession` → push `CSessionList` modal). Then calls `CMenu_ShowLobby @ 0x00414790` which builds `CStartGame2` on the stack and pushes it via `CMenu_DoModalChild @ 0x00413030`.
  * **`CStartGame2::ctor @ 0x004104f0`** is the lobby — slot-driven layout (`parent->slot_count` × `parent->team_capacity`) with one avatar (`CGameView` + `CBulAnim`), name editor (`CEdit`) and optional kick button per slot. Bottom row: Back (`cmd 0x8002`), Start (`cmd 0x8003`), plus a `CLevelList`. Admin schedules a 500 ms poll (event 7) for "all ready" detection.
  * **`CGame_StartGame @ 0x00413f30`** is the lobby-to-game hand-off. Builds `CGaming` on the stack, allocates `CDirectKeyb` (sizeof `0x20c`), pushes a loading screen modally, then pushes `CGaming` modally. On match exit, tears everything down and returns to the lobby.
  * Net-msg helpers tied to the lobby (`CGame_NetSendRename_t02`, `t03 SetAvatar`, `t04 SlotCount`, `t05 SetLevel`, `t08/t09 SlotChange`, `t64 AdminByte`) are already mapped in `net_protocol.md`.
  * Class registry: `CMenu` registered with engine class id `2004` (`0x7d4`) via static-init at `0x0047c14c` (`HandleClassRegister(g_pCMenuRegistry @ 0x004b3984, 2004, name@0x004b3bac, &CMenu_CreateObject @ 0x004251d0)`). The registry slot is never looked up by literal `2004` anywhere — `CMenu` instances are created directly by `CBulanci::OnEvent_MenuStateMachine::0xcc`.
  * **`CSwitch` vtables fully decoded** — six tables at `{+0x00:0x004834d4, +0x04:0x004834b8, +0x10:0x004834a0, +0x18:0x0048348c, +0x68:0x00483478, +0x6c:0x00483460}`. Primary has 28 slots (CDSView shape + 6 CSwitch overrides); IDSEventHandler slot 4 (`CDSView::DispatchEvent @ 0x0042c040`) is the central routing for scheduler messages `{0x100→OnCmd, 0x200→OnCustom, 0x400→OnBroadcast}`. CSwitch overrides: primary[14]=Tick, primary[17/18/19/20]=Mouse(Enter/Leave/Focus/Click), primary[27]=sibling-button-pressed handler. IDSAnim slot 4 (`0x00424e10`) is the "after animation finish, snap visual to live state" hook. Tracks: `0x100d4`=idle, `0x100d5`=hover/pressed.
  * **`CRuch` is a scanline visual effect**, not a sprite. Primary slot 14 (`0x00423e60`) directly calls `CPoemScroller::DrawVerticalLine @ 0x00436420(g_pApp+0x80, x, y, bottom, color1=0x3f3f3f, color2=0xffffff)` — a 1-pixel-wide full-height column drawn with a two-color gradient. Scheduler event 6 fires every 100 ms while visible / `rand()*0xfa1 >> 15` ms while hidden, alternating visibility and teleporting to a new random X each cycle. Four instances run in parallel.
  * **`CPoemScroller` mapped end-to-end**. Scrolls 1 px upward every `0x78` ms (120 ms = ~8.3 px/s), 3-second initial delay (`Scheduler_SetEventLastFireMs(slot, g_dwElapsedMs + 0xbb8)`). `FUN_00425df0` (`PickNextPoem`) randomly picks one of the class-`0x7fb` Poem resources, calls `FUN_004375e0` to first measure then rasterise the wstring onto a freshly-allocated `CDSBmpImage`, then pre-bakes two 40-row alpha ramps (`6,12,…,0xf0` and inverse) at `this[0x118/0x11c]` for the fade-in/fade-out edges. Line-break = pure `'\n'` split (no auto-wrap); `0x01` / `0x02` are per-line center / right alignment markers; `0x09` advances X by 4 spaces. Render is `FUN_004240f0` (primary slot 14), splitting into top-fade / middle / bottom-fade bands.
  * **Daytime predicate confirmed**. `FUN_0042e6c0` packs `GetLocalTime()` into a dword as `(hour<<25) | (minute<<18) | (second<<11) | millisecond`, so `dword>>25 == SYSTEMTIME.wHour` (local). The check `(hour - 6) > 15` (unsigned) is **night iff `hour ∈ {0..5, 22, 23}`**, i.e. **day = 06:00–21:59**.
  * **Level list origin clarified**. The lobby's level array lives at `CGame+0xbc` / `CGame+0xc4` (count), where `CGame` is the embedded sub-object inside `CBulanci+0x284` (size ≈ `0x248`, reaches `CBulanci+0x4cc`). It is populated by `CMenu::OpenNetworkSession @ 0x00414dd0` (the host path) from a pre-scanned `Resource*` array at `CGame+0x66` / `CGame+0x6e`, via `FUN_00414920` chain-append. `CStartGame1` is purely UI and does **not** scan resources on entry (its 0xa8 byte size doesn't even reach offset `0xbc`). The join path receives the level list over the wire as DirectPlay msg type `0x05` (`SetLevel`).
* **Open questions:**
  * The writer of `CGame+0x66` / `CGame+0x6e` (the source array of level-class resources). Static analysis finds **no** direct `MOV [reg+0x66], …` store — the writer must be using a different base register inside the class-registry mechanism (`FUN_0042f4b0` or one of its callees during `CBulanci::InitResourceBank`).
* **Artefacts:** `./main_menu.md`, `./history_screen.md`, `./red_menu_cursor.md`

### Multiplayer lobby UI

* **Classes:** `CChatList`, `CChatEdit`, `CSessionList`, `CSessionItem`, `CTcpIpConfig`
* **Bytes:** 1.6 KB
* **Confidence:** **Verified**
* **What's known:** Network session browser (`CSessionList` / `CSessionItem`) and TCP/IP host dialog (`CTcpIpConfig`) fully reversed, showing integration with DirectPlay and `DPSESSIONDESC2` structures (GUID extraction and ANSI-to-Unicode conversion). Chat input/output loop (`CChatEdit` / `CChatList`) fully mapped, including the network wire format, custom event broadcasts, and a hidden developer credits easter egg. All 17 constituent functions renamed in Ghidra.
* **Open questions:** None.
* **Artefacts:** `./lobby_ui.md`

### Scoring / HUD counters

* **Classes:** `CScore`, `CScoreItem`, `CGameCounter`, `CNumCounter`, `CShotCounter`, `CPoem`, `CPoemScroller`
* **Bytes:** 4.0 KB
* **Confidence:** **Empty**
* **What's known:** Six counter variants + a scrolling poem widget for between-round screens.
* **Open questions:** Counter render contract; `CPoem*` for which scenes.
* **Artefacts:** —

### Movie / cinema views

* **Classes:** `CMovieView`, `CBlackView`
* **Bytes:** 0.8 KB
* **Confidence:** **Verified**
* **What's known:** Fully decompiled, cross-referenced, and renamed in Ghidra. `CBlackView` represents a solid-color fill view used for fade transitions. `CMovieView` plays cinematic movies by coordinating separate audio (MPX) and video streams, driven synchronously via `CDSAudioVideoPlayer`.
* **Open questions:** —
* **Artefacts:** `./movie_cinema_views.md`

### Save / profile format

* **Classes:** (no obvious class; check `CBulanci` / `CGame` write-paths and `CDSRegKeyException` callers)
* **Bytes:** —
* **Confidence:** **Empty**
* **What's known:** Likely a mix of registry (HKCU) values and a file blob. The presence of `CDSRegKeyException` is a clear hint that profile state lives in the registry.
* **Open questions:** Registry layout (key path, value names), on-disk binary blob format if any.
* **Artefacts:** —

### Advertising

* **Classes:** `CAdvertising`
* **Bytes:** 0.21 KB
* **Confidence:** **Sketched**
* **What's known:**
  * Startup splash widget. `CAdvertising_ctor @ 0x0040e5f0` builds a `CWindow(0, 0, 800, 600, 0)` with a white `CBlackView` backdrop.
  * Driven by `CBulanci_OnEvent_MenuStateMachine::0xf7` — `CAdvertising_LoadSplashImage @ 0x0040fe30(splash, 0x1013a)` centres the IWANNAPLAY.COM logo bitmap on the white view, then `FUN_0040aca0` arms the auto-dismiss timer.
  * Asset lives in the EXE's overlay resource pool under id `0x1013a` (the `iwannaplay_com.bmp`-style logo).
  * Dismissed on timeout or on any keypress (splash's own `OnKeyDown`).
* **Open questions:** Exact auto-dismiss delay (~256 ms based on the subsequent `0xcc` reschedule, but `FUN_0040aca0`'s body hasn't been fully decoded).
* **Artefacts:** `./main_menu.md`

### Help / History scripts

* **Classes:** `CHelpScript`, `CHistoryScript`, `CHelpDlg`, `CHistoryDlg`, `CHelpView`, `CHistoryView`
* **Bytes:** 1.1 KB
* **Confidence:** **Verified**
* **What's known:** The History screen and page navigation are fully mapped out. The screen uses specialized `CHistoryScript` VM opcodes (such as `47` for `CreateImage`, which wraps loaded JPEGs in a `CDSBitmap` view) and coordinates with the ambient scheduler to cross-fade background music when cinematic movies (`CMovieView`) start (`0xf0`) and stop (`0xf1`).
* **Open questions:** None.
* **Artefacts:** `./history_screen.md`, `../engine/script_lifecycle.md`

### Powerups & Dynamic Spawner

* **Classes:** `CGaming`, `CBulanek`, `CWeapon`, `CSpells`
* **Bytes:** —
* **Confidence:** **Verified**
* **What's known:**
  - **The Spawner State Machine (`CGaming_RandomPickupSpawner_Tick` @ `0x0041e350`)**: Operates on host instances. Uses a random selection normalized to map limits `*(int *)(CGaming + 0x368)`. Bound limit is `2` in shotgun-only mode (spawning only Special Pickups) and `6` in all-weapons mode (spawning weapons, mines, and specials).
  - **World Slots & Networking**: Special pickups spawn at reserved slot `100` and sync via DirectPlay msg `0x13`. General pickups (Mines `0x1010C`, Machine Gun `0x100DE`, Rocket Launcher `0x100DB`) are placed in slots `101..107` and sync via msg `0x18`.
  - **Pickup Collection & Refill (`CGame_ApplyPickup` @ `0x0041eba0`)**: Deletes world pickup entities and resolves inventory updates. Sets ammo counts at `this + 0x11c + kind`. Standard weapons/mines ammo caps are read from `g_kMaxAmmoTable` (`0x00481a60`).
  - **Special Mode Guard Flag**: Collecting special pickups in shotgun-only mode sets flag `this[0x16b] = 1`. If the player dies (`CBulanek_OnDeath` @ `0x0041f900`) or is depleted of weapon ammo (`CWeapon::Fire` @ `0x004212b0`), this flag triggers a respawn of the special pickup back onto the map.
  - **Spells Subsystem (`CSpells` @ `0x00426da0` / `0x004278c0`)**: Floating head indicators. State is driven by events `0xee` (on) and `0xef` (off). Tracks active mask at `+0x78` mapping 3 status effects: Bit 0 = Shield / Armor (source rect `25, 0, 36, 14`), Bit 1 = Hourglass (source rect `0, 0, 11, 14`), and Bit 2 = Invisibility Eye (source rect `13, 0, 23, 14`).
* **Open questions:** —
* **Artefacts:** `./powerups.md`, `./map_slots_spawner.md`

---

## Core Engine Subsystems

These cover low-level platform interfaces, the main application shell, timing/tick distribution, and scripting lifecycles.

### Monotonic Timing & Event Scheduler

* **Classes:** `CDSApp`, `CDSUpdatedItem`, `Scheduler`
* **Bytes:** 4.2 KB
* **Confidence:** **Verified**
* **What's known:** The engine runs a cooperative modal-pump loop driven by the Windows Multimedia Timer (`timeGetTime`). It features a fixed-timestep, drift-free scheduler that prevents timing errors across long runs by advancing the execution threshold mathematically rather than matching clock time.
* **Open questions:** None; clock updates, cooperative message dispatch, and task polling are fully decompiled and documented.
* **Artefacts:** `../engine/tick_system.md`