//! Accumulative Fixed-Timestep Catch-up Scheduler.
//!
//! Direct port of the original engine's `CDSObject` timer machinery
//! documented in `ghidra_analysis/engine/tick_system.md`:
//!
//!   * `Scheduler_RegisterEventSlot @ 0x0042f210`
//!   * `Scheduler_SetEventDelayMs   @ 0x0042f2d0`
//!   * `Scheduler_SetEventLastFireMs @ 0x0042f290`
//!   * `Scheduler_DispatchDueEvents @ 0x0042eb30`
//!
//! Each [`EventSlot`] stores its `last_fire_ms`, a periodic `delay_ms`,
//! and a `recurring` flag. On each [`Scheduler::dispatch_due_events`]
//! invocation the scheduler scans every active slot and, for as long as
//! `now_ms >= last_fire_ms + delay_ms`, it:
//!
//! 1. records the slot's `id` in the fired list,
//! 2. **advances `last_fire_ms` by `delay_ms`** (never snaps to `now_ms`),
//!    which is the key drift-free property described in tick_system.md
//!    §"Accumulation Catch-Up Logic",
//! 3. or deactivates the slot if it was a single-shot.
//!
//! This `while`-loop dispatch means that if the host stutters and a
//! 10ms-period slot is 50ms behind, the scheduler fires it five times
//! within the same dispatch call so the simulation/animation/cursor
//! state catches up to the real wall-clock value *within one frame*.
//! This matches the original game's described behavior — see the
//! "Framerate Independence vs. Slowdown Behavior" section of
//! tick_system.md, in particular:
//!
//! > "If the rendering framerate drops to 30 FPS (one frame every
//! >  33.3ms), the scheduler dispatch loop `Scheduler_DispatchDueEvents`
//! >  will see that multiple 10ms intervals have matured in a single
//! >  frame. The scheduler loop will process and execute those multiple
//! >  events in rapid succession within the same frame tick, ensuring
//! >  that physics buffers, trails, and animations catch up instantly
//! >  to match real elapsed time."
//!
//! A `MAX_CATCH_UP_PER_DISPATCH` guard caps the maximum number of times
//! a single slot can fire within one `dispatch_due_events` call. This
//! prevents the "spiral of death" in catastrophic cases (e.g. resuming
//! from sleep): without the cap, a 17ms gameplay slot frozen for
//! 5 minutes would attempt ~17,600 catch-up steps and lock the app.

#[derive(Debug, Clone)]
pub struct EventSlot {
    pub id: u32,
    pub delay_ms: u64,
    pub last_fire_ms: u64,
    pub recurring: bool,
    pub active: bool,
}

/// Hard limit on the number of times a single recurring slot can fire
/// within one [`Scheduler::dispatch_due_events`] call. Picked large
/// enough that a 60s pause at a 10ms cadence still catches up
/// (6000 / 10 = 600 ticks), but small enough to bound a runaway loop.
pub const MAX_CATCH_UP_PER_DISPATCH: u32 = 1024;

#[derive(Debug, Clone, Default)]
pub struct Scheduler {
    slots: Vec<EventSlot>,
}

impl Scheduler {
    pub fn new() -> Self {
        Self { slots: Vec::new() }
    }

    /// Registers a slot. If a slot with the same `id` already exists it is
    /// **replaced in place** (preserving the slot index ordering) — this
    /// matches the original engine's `Scheduler_RegisterEventSlot` behavior
    /// of writing into a fixed-index slot table.
    pub fn register(&mut self, id: u32, delay_ms: u64, recurring: bool, now_ms: u64) {
        if let Some(slot) = self.slots.iter_mut().find(|s| s.id == id) {
            slot.delay_ms = delay_ms;
            slot.last_fire_ms = now_ms;
            slot.recurring = recurring;
            slot.active = true;
            return;
        }
        self.slots.push(EventSlot {
            id,
            delay_ms,
            last_fire_ms: now_ms,
            recurring,
            active: true,
        });
    }

    /// Equivalent of `Scheduler_KillSlot` — deactivates without removing.
    pub fn deactivate(&mut self, id: u32) {
        if let Some(s) = self.slots.iter_mut().find(|s| s.id == id) {
            s.active = false;
        }
    }

    /// `Scheduler_PauseEventSlot` analogue — same as `deactivate` for us
    /// since we use a single `active` bit.
    pub fn pause(&mut self, id: u32) {
        self.deactivate(id);
    }

    /// Reactivates a previously-deactivated slot. Snaps `last_fire_ms` to
    /// `now_ms` so we don't unleash a catch-up burst from the time it was
    /// paused — matches `Scheduler_UnpauseEventSlot @ 0x0042f320`.
    pub fn unpause(&mut self, id: u32, now_ms: u64) {
        if let Some(s) = self.slots.iter_mut().find(|s| s.id == id) {
            s.active = true;
            s.last_fire_ms = now_ms;
        }
    }

    /// `Scheduler_SetEventDelayMs @ 0x0042f2d0` — changes the recurring
    /// interval without resetting `last_fire_ms` (the slot continues
    /// catching up from where it was).
    pub fn set_delay_ms(&mut self, id: u32, delay_ms: u64) {
        if let Some(s) = self.slots.iter_mut().find(|s| s.id == id) {
            s.delay_ms = delay_ms;
        }
    }

    /// `Scheduler_SetEventLastFireMs @ 0x0042f290` — re-anchors a slot's
    /// last-fire timestamp. Used e.g. by `CPoemScroller::ctor` to delay
    /// the first scroll tick by 3000ms (`g_dwElapsedMs + 0xbb8`).
    pub fn set_last_fire_ms(&mut self, id: u32, ms: u64) {
        if let Some(s) = self.slots.iter_mut().find(|s| s.id == id) {
            s.last_fire_ms = ms;
        }
    }

    /// Returns `true` if the slot with `id` is currently registered + active.
    pub fn is_active(&self, id: u32) -> bool {
        self.slots.iter().any(|s| s.id == id && s.active)
    }

    /// Returns the timestamp of the slot's last fire (or `0` if unknown).
    pub fn last_fire_ms(&self, id: u32) -> u64 {
        self.slots.iter().find(|s| s.id == id).map(|s| s.last_fire_ms).unwrap_or(0)
    }

    /// **Atomic single-step dispatch** — the building block that mirrors
    /// the original engine's per-iteration handler invocation.
    ///
    /// Scans slots in registration order; for the first slot that is
    /// **currently due** at `now_ms` this method:
    ///
    /// 1. advances that slot's `last_fire_ms` by its `delay_ms` (or marks
    ///    it inactive if non-recurring), and
    /// 2. returns its `id`.
    ///
    /// If no slot is due, returns `None`.
    ///
    /// The caller is expected to **run the slot's handler** before the
    /// next invocation. This is the key property that lets a handler call
    /// [`Scheduler::set_delay_ms`] (or [`Scheduler::deactivate`]) and have
    /// the very next `next_due_event` call see the updated delay/state —
    /// matching the original engine's `Scheduler_DispatchDueEvents @
    /// 0x0042eb30` which calls `(*dispatcher)(slot_idx)` **inside** its
    /// catch-up `while` loop.
    ///
    /// `CRuch_OnScheduledTick` relies on this exact behavior: when the
    /// visible→hidden transition fires it re-programs its slot from
    /// 100 ms to a random 0–4000 ms hidden interval; the next
    /// `next_due_event` invocation must observe that new delay, otherwise
    /// a single slow frame could swallow the 100 ms visible window.
    pub fn next_due_event(&mut self, now_ms: u64) -> Option<u32> {
        for slot in self.slots.iter_mut() {
            if !slot.active || slot.delay_ms == 0 {
                continue;
            }
            if now_ms < slot.last_fire_ms + slot.delay_ms {
                continue;
            }
            let id = slot.id;
            if slot.recurring {
                slot.last_fire_ms += slot.delay_ms;
            } else {
                slot.active = false;
            }
            return Some(id);
        }
        None
    }

    /// Snaps every active slot's `last_fire_ms` forward to `now_ms`.
    /// Used as a safety bail when a catastrophically long pause would
    /// otherwise demand thousands of catch-up steps in a single frame.
    pub fn snap_all_to(&mut self, now_ms: u64) {
        for s in self.slots.iter_mut() {
            if s.active {
                s.last_fire_ms = now_ms;
            }
        }
    }

    /// **The drift-free catch-up dispatcher (non-interleaved).**
    ///
    /// Returns the ordered list of slot IDs that fired this dispatch call.
    /// A single recurring slot can appear in the returned list multiple
    /// times (consecutively) if catch-up fired it multiple times.
    ///
    /// This is a convenience for callers whose handlers never mutate slot
    /// timing (e.g. the server's fixed-cadence simulation tick). Callers
    /// whose handlers re-program their own slot from inside the handler
    /// (e.g. the `CRuch` toggle ticker) must use [`Scheduler::next_due_event`]
    /// in a loop instead — see the doc-comment on that method.
    pub fn dispatch_due_events(&mut self, now_ms: u64) -> Vec<u32> {
        let mut fired = Vec::new();
        for slot in self.slots.iter_mut() {
            if !slot.active || slot.delay_ms == 0 {
                continue;
            }
            let mut catch_up_count: u32 = 0;
            while now_ms >= slot.last_fire_ms + slot.delay_ms {
                fired.push(slot.id);
                if slot.recurring {
                    slot.last_fire_ms += slot.delay_ms;
                    catch_up_count += 1;
                    if catch_up_count >= MAX_CATCH_UP_PER_DISPATCH {
                        // Safety bail — snap forward so we don't lock up.
                        slot.last_fire_ms = now_ms;
                        break;
                    }
                } else {
                    slot.active = false;
                    break;
                }
            }
        }
        fired
    }

    pub fn slot_count(&self) -> usize {
        self.slots.iter().filter(|s| s.active).count()
    }
}

#[cfg(test)]
#[path = "scheduler_tests.rs"]
mod tests;

