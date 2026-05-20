# Startup splash screen (Advertising)

Reverse-engineered from `bulanci.exe`. Covers the startup splash screen subsystem managed by the `CAdvertising` class, showing the `IWANNAPLAY.COM` logo centred on a white background, which is dismissed automatically after a timeout, or immediately upon user keyboard/mouse input.

Everything below is grounded in specific `bulanci.exe` addresses to enable faithful re-implementation alongside the disassembly.

---

## 1. Overview and Lifecycle

The startup splash screen is driven by the top-level menu state machine inside **`CBulanci::CBulanci_OnEvent_MenuStateMachine`** at `0x00402490` when processing event `0xf7` (Show splash).

```
CBulanci::OnEvent(0xf7)
  -> CAdvertising::CAdvertising_ctor (0x0040e5f0)
      - CWindow::ctor(0, 0, 800, 600)
      - Creates a white CBlackView backdrop
  -> CAdvertising::LoadSplashImage (0x0040fe30, ID=0x1013a, delay=4000)
      - Looks up "iwannaplay_com" bitmap resource
      - Centers the bitmap on the 800x600 canvas
      - Instantiates and attaches a child CDSBitmap view
      - CAdvertising::ArmDismissTimer (0x0040aca0, delay=4000 ms)
          - Scheduler_RegisterEventSlot on the CDSUpdatedItem base
  -> CDSView_DoModal (modal message pump)
      - Shows splash, ticks/renders child views (white backdrop + centered logo)
      - Dismissed on:
         * Timer expiration -> OnTimerTick (0x0040b4b0) -> Dismiss (0x0040acc0)
         * Any keypress     -> OnKeyDown (0x0040b500)   -> Dismiss (0x0040acc0)
         * Mouse click      -> OnLButtonDown (0x0040b4d0) -> Dismiss (0x0040acc0)
  -> Destruction (0x00401a10)
      - Destroys CAdvertising object & child views
  -> Scheduler_PostMessage(0xcc, delay=256 ms)
      - Enters main menu state
```

---

## 2. Object Layout and Multiple Inheritance

`CAdvertising` is a composite object inheriting from both `CWindow` (and by extension `CDSView`) and `CDSUpdatedItem` (for timer registration). It utilizes multiple inheritance, which MSVC compiles into a multi-vtable structure.

### Base Class Offsets & VTables

| Offset | Base Class / Interface | VTable Address | Purpose |
|--------|------------------------|----------------|---------|
| `+0x00` | `CAdvertising` (Primary) | `0x0048181c` | Primary object interface / `CDSView` |
| `+0x04` | Base interface `+0x4` | `0x004817fc` | Interface adjustment |
| `+0x10` | Base interface `+0x10` | `0x004817e4` | Interface adjustment |
| `+0x18` | Base interface `+0x18` | `0x004817d0` | Interface adjustment |
| `+0x70` | `CDSUpdatedItem` | `0x004817b8` | Timer / Scheduler Client |

### Key Virtual Function Overrides (Primary VTable `0x0048181c`)

* **`[0]` (`0x0040e710`):** `GetClassId()` — Returns a pointer to a static 32-byte zero block (`&DAT_004b36cc`) acting as the RTTI class identifier.
* **`[1]` (`0x0040fb70`):** `deleting_destructor(byte flags)` — Scalar deleting destructor. Frees memory when `flags & 1` is true.
* **`[19]` (`0x0040b4d0`):** `OnLButtonDown(int x, int y)` — Left mouse button down handler.
* **`[22]` (`0x0040b500`):** `OnKeyDown(char key, char state)` — Keyboard down handler.

---

## 3. Subsystem Functions

### 3.1 `CAdvertising::CAdvertising_ctor` (`0x0040e5f0`)

Performs object and view setup.
1. Calls the parent constructor `CWindow::CWindow(0, 0, 800, 600, 0)`.
2. Initializes the `CDSUpdatedItem` base at `this + 0x70` via `CDSUpdatedItem::ctor`.
3. Sets up the vtables for multiple inheritance (offsets `0x0`, `0x4`, `0x10`, `0x18`, `0x70`).
4. Allocates a white transition backdrop:
   * Calls `operator new(0x6c)` to create a `CDSChained` (subclassed as `CBlackView`).
   * Calls `CBlackView::ctor(0, 0, 800, 600)`.
   * Sets the vtables for `CBlackView` and sets its background color to RGB White (`*(view + 0x68) = 0xffffff`).
5. Adds the backdrop view to the window via `CDSView::AddChild(this, backdrop, 0)`.
6. Sets window flags: `*(this + 0x14) |= 0x111` (modal/active settings).
7. Initializes the dismiss-blocking flag: `m_bBlockDismiss` (`this + 0x8c`) = `false`.

---

### 3.2 `CAdvertising::LoadSplashImage` (`0x0040fe30`)

Loads the splash logo, centers it on the canvas, and schedules the auto-dismiss timer.
```cpp
void CAdvertising::LoadSplashImage(uint resourceId, uint delayMs)
```
1. Retrieves the bitmap resource via the application's global resource bank:
   `resource = g_pApp->m_pResourcePool->Lookup(resourceId)` (defaults to `0x1013a` — the `iwannaplay_com.bmp` logo).
2. Performs type checking on the loaded resource using RTTI (`_Globals::CheckedVirtualBaseCast`).
3. Extracts dimensions from the bitmap resource:
   * `width = resource[1]`
   * `height = resource[2]`
4. Centers the bitmap on the 800x600 splash window:
   * `X = (800 - width) / 2`
   * `Y = (600 - height) / 2`
5. Instantiates the bitmap widget:
   * Calls `operator new(0x78)` to allocate `CDSBitmap`.
   * Constructs `CDSBitmap(X, Y, resource)` centered on screen.
6. Adds the `CDSBitmap` widget as a child view: `CDSView::AddChild(this, bitmap_view, 0)`.
7. Arms the timer: `ArmDismissTimer(delayMs)` (where `delayMs = 4000`, matching 4 seconds).
8. Releases the temporary resource reference.

---

### 3.3 `CAdvertising::ArmDismissTimer` (`0x0040aca0`)

Registers the auto-dismiss trigger with the application scheduler.
```cpp
void CAdvertising::ArmDismissTimer(uint delayMs)
```
* Calls `Scheduler_RegisterEventSlot(this + 0x70, 0, delayMs, 6)`.
* Registers the event using the `CDSUpdatedItem` interface at `this + 0x70`. Once `delayMs` expires, the scheduler triggers `OnTimerTick`.

---

### 3.4 `CAdvertising::OnTimerTick` (`0x0040b4b0`)

Called by the scheduler when the auto-dismiss timer expires.
```cpp
void CAdvertising::OnTimerTick(void* param_1)
```
1. Retrieves the parent `CAdvertising` object by subtracting the base offset:
   `CAdvertising* pThis = (CAdvertising*)((char*)this - 0x70)`.
2. Triggers dismissal: `pThis->Dismiss()`.
3. Cleans up the scheduler event slot via `Scheduler_UnregisterSlot(this, 0)`.

---

### 3.5 `CAdvertising::Dismiss` (`0x0040acc0`)

Instructs the modal loop to exit.
```cpp
void CAdvertising::Dismiss()
```
* Posts the modal dismissal message:
  `Scheduler_PostMessage(this + 0x10, 0x100, 0x8003, 0, 0)`
* Sending command `0x8003` to the window event queue breaks the `CDSView_DoModal` message pump, allowing the boot sequence to proceed to the main menu (event `0xcc`).

---

### 3.6 `CAdvertising::OnLButtonDown` (`0x0040b4d0`)

Handles mouse left clicks to skip the splash screen.
```cpp
void CAdvertising::OnLButtonDown(int x, int y)
```
1. Delegates event to the base class mouse handler: `CDSView::OnLButtonDown(x, y)`.
2. Checks dismissal safety:
   `if (this->m_bBlockDismiss == false) { Dismiss(); }`
   *(Since `m_bBlockDismiss` is initialized to `false`, clicking anywhere on the splash immediately skips it).*

---

### 3.7 `CAdvertising::OnKeyDown` (`0x0040b500`)

Handles keyboard keystrokes to skip the splash screen.
```cpp
uint CAdvertising::OnKeyDown(char key, char state)
```
1. Delegates event to the base window keyboard handler: `CWindow::OnKeyDown(key, state)`.
2. If handled by the base class (returns non-zero), return handled.
3. Checks dismissal safety:
   `if (this->m_bBlockDismiss == false) { Dismiss(); }`
   *(Since `m_bBlockDismiss` is `false`, any unhandled keystroke immediately skips the splash).*
