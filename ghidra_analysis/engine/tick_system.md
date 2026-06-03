# `CDSApp` Tick Subsystem & Fixed-Timestep Catch-up Scheduler

Bulánci features a robust cooperative real-time timing and scheduling architecture built into its application shell (`CDSApp`). The engine decouples or coordinates three distinct aspects of its execution:
1. **The Real-Time Clock**: Built on the Windows Multimedia Timer (`timeGetTime`).
2. **The Cooperative Render Loop**: An uncapped or VSync-bound message pump that updates the window/backbuffer during idle periods.
3. **The Fixed-Timestep Event Scheduler**: A scheduler that dispatches periodic logic events, physics updates, and animations using monotonic time differentials, supporting drift-free catch-up during framerate drops.

---

## The Monotonic Clock (`CDSApp_UpdateClock @ 0x0042e790`)

The game maintains a global monotonic clock `g_dwElapsedMs` representing the number of milliseconds elapsed since the application started. It is updated exactly once per frame tick:

```cpp
void CDSApp_UpdateClock(void) {
    g_dwElapsedMs = timeGetTime() - g_dwStartMs;
}
```

* `timeGetTime()` provides a high-resolution (typically 1 ms accuracy on modern Windows, or configured via `timeBeginPeriod` on older systems) millisecond counter.
* This elapsed time is used globally across all subsystems to measure time deltas, trigger animations, and drive physics.

---

## The Modal Message Pump (`CDSView_DoModal @ 0x0042d1a0`)

Bulánci is designed around a hierarchical, modal view-state architecture where almost every active screen—from the main menu (`CMenu`) to the gameplay coordinator (`CGaming`) and individual config/message dialogs—is a `CDSView` (or derived class).

When a new modal view is pushed onto the screen stack, the engine pauses the caller and executes a recursive modal loop:

```cpp
int CDSView_DoModal(CDSView* this, CDSView* parent) {
    this->m_parent = parent;
    this->m_isActive = true;
    this->m_exitCode = 0;
    
    // ... register and setup view ...

    do {
        CDSApp_PumpTick(g_pApp);
    } while (this->m_exitCode == 0);

    // ... clean up and restore parent ...
    return this->m_exitCode;
}
```

This cooperative nested-loop approach ensures that regardless of which screen is active, the top-level application loop remains pumping messages and ticking the game.

---

## The Per-Frame Engine Tick (`CDSApp_PumpTick @ 0x0042be60`)

The main tick coordinator is `CDSApp_PumpTick`. It cooperative-yields to Windows message dispatching. If no Windows message is pending, it executes the frame tick body:

```cpp
void CDSApp_PumpTick(CDSApp* this) {
    MSG msg;
    BOOL hasMessage = PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);
    if (hasMessage) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    } else {
        CDSApp_FrameBody(this);
    }
}
```

### Frame Tick Body (`CDSApp_FrameBody @ 0x0042bda0`)

When the OS has no pending input or window messages, `CDSApp_FrameBody` performs the core frame logic, updating clock, executing tasks, and rendering:

```cpp
void CDSApp_FrameBody(CDSApp* this) {
    // 1. Update the global monotonic clock
    CDSApp_UpdateClock();

    // 2. Pulse all registered tasks/schedulers
    CDSApp_PulseTasks(this);

    // 3. Render the current active view (or backbuffer flip)
    CDSApp_RenderFrame(this);
}
```

---

## The Fixed-Timestep Scheduler (`Scheduler_DispatchDueEvents @ 0x0042eb30`)

The engine provides a highly flexible, drift-free scheduler mechanism used by various game entities to execute logic and simulation steps at precise intervals.

### Task List Registration (`g_pTaskList @ 0x004b3b8c`)
Any class implementing scheduled behavior can register a `CDSUpdatedItem` (constructed via `CDSUpdatedItem_ctor @ 0x0042f060`) into the global active task list `g_pTaskList`.
* `CDSApp_PulseTasks` iterates through this list every frame and invokes the scheduler dispatcher on each active task's scheduler instance:

```cpp
void CDSApp_PulseTasks(CDSApp* this) {
    CDSUpdatedItem* item = g_pTaskList;
    while (item != NULL) {
        if (item->m_isLive) {
            Scheduler_DispatchDueEvents(item->m_scheduler);
        }
        item = item->m_pNext;
    }
}
```

### Event Slot Structure (`sizeof = 16 bytes`)

Each scheduler maintains a dynamic array of event slots. A slot registers a recurring or single-shot callback with a target delay:

| Offset | Type | Field | Description |
|--------|------|-------|-------------|
| `+0x00` | `dword` | `flags` | Bit 2 (`0x04`) indicates a recurring/periodic event |
| `+0x04` | `dword` | `lastFireMs` | Absolute timestamp of the last executed tick |
| `+0x08` | `dword` | `delayMs` | Programmed interval between ticks (e.g. 10ms, 120ms, 8000ms) |
| `+0x0c` | `dword` | `id` | Identifier of this event slot |

### Dispatch and Drift-Free Rescheduling

When `Scheduler_DispatchDueEvents` evaluates the slots, it performs a strict check against the wall-clock time:

$$\text{g\_dwElapsedMs} \ge \text{slot.lastFireMs} + \text{slot.delayMs}$$

If this condition is met, the scheduler executes the registered callback (a virtual function of the owning object, dispatching to `OnSchedulerTimer`).

#### The Accumulation Catch-Up Logic
To prevent timing drift, the scheduler does **not** reset `lastFireMs` to the current `g_dwElapsedMs`. Instead, it advances the threshold mathematically by the exact scheduled delay:

$$\text{slot.lastFireMs} \leftarrow \text{slot.lastFireMs} + \text{slot.delayMs}$$

```cpp
void Scheduler_DispatchDueEvents(Scheduler* scheduler) {
    for (int i = 0; i < scheduler->m_slotCount; i++) {
        EventSlot* slot = &scheduler->m_slots[i];
        if (!slot->m_active) continue;

        // Check if event has matured
        if (g_dwElapsedMs >= slot->m_lastFireMs + slot->m_delayMs) {
            // Invoke callback
            scheduler->m_owner->OnSchedulerTimer(slot->m_id);

            if (slot->m_flags & SCHEDULER_FLAG_RECURRING) {
                // Drift-free rescheduling: advance timestamp relative to previous target
                slot->m_lastFireMs += slot->m_delayMs;
            } else {
                // Clear single-shot slot
                slot->m_active = false;
            }
        }
    }
}
```

**Why this prevents drift:**
If an event is scheduled every `100 ms` and fires slightly late at `105 ms` due to a long frame rendering:
* If it reset `lastFireMs = g_dwElapsedMs` (to `105 ms`), the next event would fire at `205 ms` (cumulative drift of `+5 ms`).
* Instead, it sets `lastFireMs = lastFireMs + 100` (to `200 ms`). The next frame tick at `201 ms` will see the event is due and fire it immediately, successfully self-correcting the timing back to its nominal timeline.

---

## Framerate Independence vs. Slowdown Behavior

### 1. The Rendering Loop: Uncapped or VSync-Bound
The game does not place artificial sleep throttling in `CDSApp_FrameBody`. It renders as fast as the system allows.
* However, when page flipping is initialized on the DirectDraw screen surfaces (`IDirectDrawSurface7::Flip`), it is called with `DDFLIP_WAIT` flag.
* This locks the render pipeline to the monitor's physical refresh rate (typically **60Hz**, **75Hz**, or **144Hz**). If VSync is disabled in driver overrides, the frame rate is uncapped and can run in the hundreds of frames per second.

### 2. Event Schedulers: High-Rate Temporal Schedules
Critical micro-events (like the main menu cursor `CGunMouse` dot positioning tracking) require execution steps at a rate completely independent of rendering speed.
* `CGunMouse` updates its history trajectory ring buffer at a strict `10ms` schedule (`100Hz`).
* If the rendering framerate drops to 30 FPS (one frame every `33.3ms`), the scheduler dispatch loop `Scheduler_DispatchDueEvents` will see that multiple `10ms` intervals have matured in a single frame.
* The scheduler loop will process and execute those multiple events in rapid succession within the same frame tick, ensuring that physics buffers, trails, and animations catch up instantly to match real elapsed time.

### 3. Gameplay Logic: Classic Frame-Linked Updates
While schedulers handle periodic UI events, ambient animations, and timer rules:
* The core gameplay movement, bullet collision detection, and player physics inside `CGaming::Render` are driven by `CGaming_TickRoundStateAndScoring @ 0x0041f350`.
* This function is executed **exactly once per render frame**, rather than through a fixed-timestep physical loop.
* **Slowdown Behavior:** Consequently, if the rendering rate drops (e.g. from 60 FPS down to 15 FPS due to heavy rendering), the player entity coordinates are evaluated less frequently without delta-time scaling ($\Delta t$). The entire game speed literally slows down (classic early-2000s behavior where game speed is proportional to rendering execution speed).
* **Speedup Behavior:** If the game runs on a high-refresh-rate monitor (e.g., 144Hz) with VSync enabled, or uncapped, the game physics runs faster. The developers compensated for this in the original release by keeping VSync strictly enabled via DirectDraw settings.

---

## Mapped Timing & Scheduler Functions

| Function Address | Original Symbol | Meaningful Renamed Symbol | Description |
|------------------|-----------------|---------------------------|-------------|
| `0x0042be60` | `FUN_0042be60` | `CDSApp_PumpTick` | Cooperative modal windows message pump |
| `0x0042bda0` | `FUN_0042bda0` | `CDSApp_FrameBody` | Core per-frame tick (Clock, Schedulers, Render) |
| `0x0042e790` | `FUN_0042e790` | `CDSApp_UpdateClock` | Captures monotonic millisecond elapsed time since boot |
| `0x0042ebf0` | `FUN_0042ebf0` | `CDSApp_PulseTasks` | Loops through `g_pTaskList` and triggers scheduler updates |
| `0x0042eb30` | `FUN_0042eb30` | `Scheduler_DispatchDueEvents` | Scans registered timer events and runs catch-up dispatch |
| `0x0042f210` | `FUN_0042f210` | `Scheduler_RegisterEventSlot` | Sets up a recurring or single-shot timer callback slot |
| `0x0042f2d0` | `FUN_0042f2d0` | `Scheduler_SetEventDelayMs` | Updates or schedules the delay interval of an event slot |
| `0x0042f060` | `FUN_0042f060` | `CDSUpdatedItem_ctor` | Constructs a scheduler registration record |
| `0x0041f350` | `FUN_0041f350` | `CGaming_TickRoundStateAndScoring` | Gameplay logic tick (movement, scoring, round updates) |
| `0x0041f050` | `FUN_0041f050` | `CGaming_OnSchedulerTimer` | Game round/pickup event handler callback |
| `0x00423de0` | `FUN_00423de0` | `CRuch_OnScheduledTick` | Scanline background actor timing callback |
| `0x00424080` | `FUN_00424080` | `CMenu_OnMusicFadeTick` | Music transition fade timer callback |
