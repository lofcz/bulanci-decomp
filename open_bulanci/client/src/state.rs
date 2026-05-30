//! Shared client-wide state types + scheduler-slot ID constants.
//!
//! The slot IDs are the canonical name-spacing for every periodic activity
//! in the menu and gameplay loops. See `bulanci_core::scheduler` and
//! `ghidra_analysis/engine/tick_system.md` for the catch-up semantics.
//!
//! The menu's former per-effect slots (cursor ring, idle-twitch frame,
//! ruch scanlines, poem scroll) are gone — those effects now live in the
//! data-driven Luau scene (`paks/base_ui`) and run on its own `b.every`
//! timers. Only the lifecycle slots Rust still owns remain here.

// ============================================================================
// Scheduler slot IDs
// ============================================================================

// Day/night ambient music — poll wall-clock hour every 5s
pub const SLOT_DAY_NIGHT: u32 = 0x30;

// Audio fades — retail menu music ramps by 1 percent every 120ms.
pub const SLOT_AUDIO_FADE: u32 = 0x31;

// Game simulation — fixed 17ms (~58.8Hz) deterministic step, catch-up
pub const SLOT_GAME_TICK: u32 = 0x40;
pub const GAME_TICK_MS: u64 = 17;

// ============================================================================
// Application phase.
//
// `AppPhase` is the overall application phase (intro splash, menu loop,
// connecting, live match). The menu's internal sub-screen state
// (StartGame1 / History / ExitConfirm) used to live here as a `SubScreen`
// enum; it now lives entirely in the Luau menu scene's `b.state` flags.
// ============================================================================

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AppPhase {
    Intro,
    Menu,
    Connecting,
    Playing,
}
