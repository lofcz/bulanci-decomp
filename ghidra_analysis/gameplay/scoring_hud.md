# Scoring / HUD Counters Subsystem Analysis

This document provides a highly comprehensive architectural and implementation analysis of the **Scoring & HUD Counters** subsystem in *Bulanci* (compiled using MSVC C++). 

---

## 1. Subsystem Overview

The Scoring and HUD subsystem is responsible for:
1. **Game Counters / HUD Elements:** Managing and rendering player HUD indicators (numerical scores, ammo, reload states, and round-level events).
2. **Scoring Data structures:** Representing individual player scores (`CScoreItem`), level scores (`CLevelScore`), and session-level totals (`CScore`).
3. **Inter-round Poem Display:** Animating and scrolling thematic Czech poems between game rounds using a custom alpha-ramping text blitter (`CPoem` and `CPoemScroller`).

---

## 2. Component Layout & Memory Structures

The subsystem leverages standard MSVC multiple inheritance and virtual method tables. Class instances are derived from base types like `CDSObject`, `CDSChained`, and `CDSView`.

### 2.1 CScoreItem (Size: ~24 bytes)
Represents a singular score entry (e.g., player score on a map).
* **Inheritance:** `CScoreItem -> CDSChained -> CDSObject`
* **Vftables:** Multiple inheritance results in primary and secondary vftables.
* **Fields:**
  * `[0x00]` Primary `vftable` (for `CDSObject` interface)
  * `[0x04]` Secondary `vftable` (for `CDSChained` interface)
  * `[0x0c]` `m_name` (`CDSString` - ref-counted UTF-16 characters)
  * `[0x10]` `m_kills` (`int`)
  * `[0x14]` `m_deaths` (`int`)

### 2.2 CLevelScore
Tracks score properties associated with a specific level or map.
* **Inheritance:** `CLevelScore -> CDSChained -> CDSObject`
* **Fields:**
  * `[0x00]` Primary `vftable`
  * `[0x04]` Secondary `vftable`
  * `[0x0c]` `m_levelName` (`CDSString`)
  * `[0x10]` `m_pScoreChain` (`CDSChain*` or pointers to nested score records)

### 2.3 CScore
Manages session-wide scores and key handling on the inter-round screen.
* **Inheritance:** `CScore -> CWindow` (or related MFC-like UI base class)
* **Fields:** Includes pointers to collections of `CScoreItem` records and UI state variables.

---

## 3. HUD Counter Variants

Bulanci implements specialized HUD rendering classes.

### 3.1 CGameCounter
The general coordinator for gameplay HUD states.
* **Inheritance:** `CGameCounter -> CDSChained -> CDSObject`
* **Layout:**
  * `[0x1a * 4 = 0x68]` Array of 2 `CDSString` objects (likely representing asset paths or fonts).
  * `[0x70] - [0x7c]` Four pointers to sub-counters (e.g., `CNumCounter` or `CShotCounter` for each player slot).
* **Behavior:**
  * **Event/Message Dispatcher (`0x0040b140`):** Listens for event `0xe2` (custom game signal). If the event sub-parameter is `4`, it issues a global event (`0x8002`) to handle resetting or round transitions.
  * **Sequence Triggers:** For sub-parameter `< 3`, it updates the texture sequence via `_Globals::TM_SetCurrentSequence` and triggers sound ID `0x12` (18). Otherwise, it triggers sound ID `0x11` (17) and updates sequence indexes.

### 3.2 CNumCounter (Numerical HUD Score)
Represents a 3-digit animated numerical dial (used for player score, ammo value, or timer).
* **Inheritance:** `CNumCounter -> CDSChained` with embedded `CDSUpdatedItem`.
* **Layout:**
  * `[0x68]` `CDSUpdatedItem` sub-object.
  * `[0x80]` Pointer to texture/font sheet containing digit glyphs.
  * `[0x84]` Pointer to glyph width/layout descriptor.
  * `[0x88]` Direction boolean (`0` = decrement, `1` = increment).
  * `[0x90] - [0x98]` Current scrolling positions / offsets for 3 individual digits.
  * `[0x9c] - [0xa4]` Target values for 3 individual digits.
* **Scroll & Digit Step Logic (`0x00427020`):**
  * When digit stepping, it compares the current value offset to the target value.
  * If descending (`m_direction == 0`), it decrements. If it goes below 0, it wraps around to `max_digits - 1`.
  * If ascending (`m_direction == 1`), it increments. If it exceeds the maximum, it resets to 0 and flags a carry.
* **Tick Update (`0x004270a0`):**
  * Periodically steps Digit 0, Digit 1, and Digit 2 sequentially. If any digit has shifted, it calls the redraw/invalidate method `CDSView::Invalidate` at virtual offset `0x24` to update the screen.
* **Render Logic (`0x00427740`):**
  * Iterates 3 times (once per digit).
  * In each loop iteration, it calls `BlitDispatch` to render the appropriate sub-rectangle of the digit sheet onto the screen backbuffer. It handles vertical clipping if a digit is mid-scroll, achieving a smooth rolling cylinder effect.

### 3.3 CShotCounter (HUD Ammo Counter)
A highly sophisticated graphical bullet-slot renderer.
* **Inheritance:** `CShotCounter -> CDSView -> CDSObject`
* **Layout:**
  * `[0x30], [0x34]` X and Y coordinates.
  * `[0x68]` Pointer to Main HUD Background Frame Sprite.
  * `[0x6c]` Pointer to Reloading/Filled Progress Sprite.
  * `[0x70]` Pointer to Discrete Bullet Sprite.
  * `[0x74]` Pointer to Empty Slot Sprite.
  * `[0x78]` `m_currentAmmo` (`byte`)
  * `[0x79]` `m_maxAmmo` (`byte`)
  * `[0x7a]` `m_bInterpolate` / `m_bAnimated` (`char` - reloading flag)
* **Rendering Pipeline (`0x00426e10`):**
  1. **Background Blit:** Blits `m_pMainSprite` at the counter's coordinates `(X, Y)`.
  2. **Reload Bar Interpolation:** If `m_bInterpolate != 0` (reload in progress), it calculates progress via:
     $$\text{height\_offset} = \frac{\text{m\_currentAmmo} \times \text{sprite\_height}}{\text{m\_maxAmmo} - 1}$$
     It then clips and blits the filled bar sprite (`m_pFilledSprite`) to reflect reloading progress.
  3. **Discrete Bullet Rendering:** If not in interpolation mode, it loops from `0` to `m_currentAmmo` and draws discrete bullet sprites side-by-side. Horizontal placements are calculated using a rounding helper (`0x00448b50`) to space bullets perfectly.

---

## 4. Inter-round Poems (`CPoem` & `CPoemScroller`)

Between combat rounds, Bulanci features stylized scrolling Czech poetry with smooth fading margins.

### 4.1 CPoem (Size: 28 bytes)
A wrapper for a text resource stored in the game's master pack under Class ID `2043` (`0x7fb`).
* **Fields:**
  * `[0x18]` `m_pszText` (`wchar_t*` pointer to UTF-16LE Czech string buffer).
* **Deserialization (`0x00409100`):**
  * Reads length and characters into the `CDSString` wrapper at offset `0x4`.

### 4.2 CPoemScroller
The manager that loads, renders, and scrolls these poems.
* **Constructor (`0x004262c0`):**
  * Registers a timer event in the global Scheduler running at an interval of **120 ms** (`0x78`).
  * Scans all game pack resources matching Class ID `0x7fb` (Poems), adding them to a list.
  * Queues the initial scroll event 3 seconds into the future (`g_dwElapsedMs + 3000`).
* **Pick Next Poem (`0x00425df0`):**
  1. Selects a random poem from the registered list:
     $$\text{idx} = \frac{\text{rand()} \times \text{poem\_count}}{32768}$$
  2. Measures the layout dimensions of the text using `_Globals::TextShaper_LayOutAndRender`.
  3. Allocates a temporary bitmap surface (`CDSBmpImage`) matching the exact bounding box.
  4. Rasterizes the poem text onto the bitmap.
  5. **Pre-bakes Alpha Ramps:** Allocates two `0x28`-row (40 pixels) buffers for edge fading.
     * **Upper alpha ramp:** Linearly scales opacity from `6` (at row 0) to `240` (at row 39).
     * **Lower alpha ramp:** Linearly fades opacity from `250` down to `16`.
* **Scroll Updates (`0x00426030`):**
  * On every 120 ms tick, it shifts the vertical coordinate (`m_scrollY`). If the poem has scrolled entirely off the top, it automatically randomizes and loads the next poem.
* **Screen Rendering (`0x004240f0`):**
  * Blits the main text surface at `m_scrollY`.
  * Overlays the pre-baked upper and lower alpha ramps at the margins, creating a beautiful cinematic fade-in and fade-out effect.

---

## 5. Reverse-Engineering Reference: MSVC Virtual Method Thunks

Because of complex multiple inheritance hierarchies, MSVC generates non-trivial virtual destructor thunks. Understanding these is essential for accurate decompilation.

### 5.1 Destructor Thunk Offsets
When a deleting destructor is invoked through a secondary base pointer (offset relative to `this`), the compiler generates thunk functions to re-align the pointer before executing the destructor.

#### CScore (Primary: `0x0040f070`)
* **Thunk `0x0040bde0`:** Realigns `this = this - 0x18` and jumps to `CScore::ScalarDeletingDestructor`.
* **Thunk `0x0040bdf0`:** Realigns `this = this - 4` and jumps to `CScore::ScalarDeletingDestructor`.
* **Thunk `0x0040be00`:** Realigns `this = this - 0x10` and jumps to `CScore::ScalarDeletingDestructor`.

#### CScoreItem (Primary: `0x00409a90`)
* **Thunk `0x004093b0`:** Realigns `this = this - 4` and jumps to `CScoreItem::ScalarDeletingDestructor`.

#### CGameCounter (Primary: `0x0040ef40`)
* **Thunk `0x0040bcb0`:** Realigns `this = this - 0x10` and jumps to `CGameCounter::ScalarDeletingDestructor`.
* **Thunk `0x0040bcc0`:** Realigns `this = this - 0x18` and jumps to `CGameCounter::ScalarDeletingDestructor`.
* **Thunk `0x0040bcd0`:** Realigns `this = this - 4` and jumps to `CGameCounter::ScalarDeletingDestructor`.

#### CNumCounter (Primary: `0x00427af0`)
* **Thunk `0x004273f0`:** Realigns `this = this - 0x10` and jumps to `CNumCounter::ScalarDeletingDestructor`.
* **Thunk `0x00427400`:** Realigns `this = this - 0x18` and jumps to `CNumCounter::ScalarDeletingDestructor`.
* **Thunk `0x00427410`:** Realigns `this = this - 0x20` and jumps to `CNumCounter::ScalarDeletingDestructor`.
* **Thunk `0x00427420`:** Realigns `this = this - 4` and jumps to `CNumCounter::ScalarDeletingDestructor`.

#### CPoem (Primary: `0x00409af0`)
* **Thunk `0x00409430`:** Realigns `this = this - 4` and jumps to `CPoem::ScalarDeletingDestructor`.
* **Thunk `0x00409440`:** Realigns `this = this - 12` and jumps to `CPoem::ScalarDeletingDestructor`.
* **Thunk `0x00409460`:** Realigns `this = this - 20` and jumps to `CPoem::ScalarDeletingDestructor`.

#### CPoemScroller (Primary: `0x004264e0`)
* **Thunk `0x004032a0`:** Realigns `this = this - 0x18` and jumps to `CPoemScroller::ScalarDeletingDestructor`.
* **Thunk `0x00425db0`:** Realigns `this = this - 0x10` and jumps to `CPoemScroller::ScalarDeletingDestructor`.
* **Thunk `0x00425dc0`:** Realigns `this = this - 0x1c` and jumps to `CPoemScroller::ScalarDeletingDestructor`.
* **Thunk `0x00425dd0`:** Realigns `this = this - 0x24` and jumps to `CPoemScroller::ScalarDeletingDestructor`.
* **Thunk `0x00425de0`:** Realigns `this = this - 4` and jumps to `CPoemScroller::ScalarDeletingDestructor`.
