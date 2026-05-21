//! Monotonic engine clock — the OpenBulanci equivalent of the original
//! `g_dwElapsedMs` global (`@ 0x004b3bd8`).
//!
//! Mirrors `CDSApp_UpdateClock @ 0x0042e790`:
//!
//! ```text
//! g_dwElapsedMs = timeGetTime() - g_dwStartMs
//! ```
//!
//! See `ghidra_analysis/engine/tick_system.md` for the full architecture.
//! The clock is *refreshed exactly once per frame* at the top of the
//! frame body (`CDSApp_FrameBody @ 0x0042bda0`), so every scheduled
//! event slot resolves against the same wall-clock value within a frame.

use std::time::Instant;

#[derive(Debug, Clone)]
pub struct EngineClock {
    start: Instant,
    /// Wall-clock ms since `start`, refreshed exactly once per frame
    /// (the OpenBulanci analogue of `g_dwElapsedMs`).
    pub elapsed_ms: u64,
}

impl EngineClock {
    pub fn new() -> Self {
        Self { start: Instant::now(), elapsed_ms: 0 }
    }

    /// Construct a clock pinned to a synthetic origin — used by unit tests
    /// and deterministic golden-master runs where wall-clock time must be
    /// driven manually via [`Self::set_elapsed_ms`].
    pub fn synthetic() -> Self {
        Self { start: Instant::now(), elapsed_ms: 0 }
    }

    /// Refresh `elapsed_ms` from the OS monotonic timer. Call once per
    /// frame at the top of `CDSApp_FrameBody`-equivalent loop.
    pub fn tick(&mut self) {
        self.elapsed_ms = self.start.elapsed().as_millis() as u64;
    }

    /// Manually set the clock — for deterministic tests / replay seeding.
    pub fn set_elapsed_ms(&mut self, ms: u64) {
        self.elapsed_ms = ms;
    }
}

impl Default for EngineClock {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
#[path = "clock_tests.rs"]
mod tests;
