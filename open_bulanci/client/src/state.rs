//! Shared client-wide state types + scheduler-slot ID constants.
//!
//! The slot IDs are the canonical name-spacing for every periodic activity
//! in the menu and gameplay loops. See `bulanci_core::scheduler` and
//! `ghidra_analysis/engine/tick_system.md` for the catch-up semantics.

// ============================================================================
// Scheduler slot IDs
// ============================================================================

// CGunMouse — driven by two scheduler slots. See `cursor.rs` and
// §4/§6 of `red_menu_cursor.md` for the byte-level semantics.
//
// SLOT_CURSOR_TICK runs the 30-slot mouse ring buffer push + pop
// (§4.A and §4.B). The original gate is per-Draw with
// `lastPushTick + 10 <= elapsed`, but on the era-typical hardware
// that the Frida trace captured (~200 fps, median Erase gap 5 ms)
// the gate's gap-grow regime meant the push effectively fired once
// per frame at the Draw cadence, yielding a 30 × 5 = 150 ms
// follow-lag — repeatedly measured in the trace as 134–140 ms from
// the first mouse motion to the first target update.
//
// At 60 fps the same per-Draw gate would push only every 16.67 ms,
// inflating the lag to ~500 ms — which feels nothing like the
// original and motivated the "boxy follow / huge delay" feedback.
// To keep the trace-measured 150 ms perception independent of the
// frame rate, we drive push/pop from a fixed 5 ms scheduler slot
// with catch-up (matching the trace's effective cadence). The
// scheduler is what the original engine itself would have used if
// the gameplay loop ran below 100 Hz — it is the natural Rust
// expression of "do this once every 5 ms of engine time".
pub const SLOT_CURSOR_TICK: u32 = 0x01; // 5ms (queue push+pop)

// SLOT_CURSOR_DOT_FRAME advances the dot's sprite-strip frame index
// inside the active idle-twitch track *and* applies that frame's
// `(Δx, Δy)` to the dot offset accumulator (§6.B — one logical
// `DecodeFrame → NotifyMove` of the active BitmapSprite). The
// original `TM_AdvanceFrame` cadence is per-sprite
// `sequenceDefaultCadenceMs` (clip duration ÷ frame count); the
// Frida capture (`tools/frida/cursor_trace.js`) measured 84–85 ms
// per tick across all three master-pack tracks (65801 / 65802 /
// 65803), so we hard-wire 85 ms here. Track switching happens
// implicitly at end-of-loop via `GunMouse::on_sprite_frame_tick`
// (each track closes cleanly to (0, 0), so we can re-roll without
// teleporting the dot — see motion_paths.rs).
pub const SLOT_CURSOR_DOT_FRAME: u32 = 0x02; // 85ms

pub const CURSOR_TICK_MS: u64 = 5;

// CRuch instances (background phosphor scanlines)
pub const SLOT_RUCH_0: u32 = 0x10;
pub const SLOT_RUCH_1: u32 = 0x11;
pub const SLOT_RUCH_2: u32 = 0x12;
pub const SLOT_RUCH_3: u32 = 0x13;
pub const RUCH_SLOTS: [u32; 4] = [SLOT_RUCH_0, SLOT_RUCH_1, SLOT_RUCH_2, SLOT_RUCH_3];

// CPoemScroller — strict 1 pixel / 120ms (see main_menu.md §9.3)
pub const SLOT_POEM_SCROLL: u32 = 0x20;

// Day/night ambient music — poll wall-clock hour every 5s
pub const SLOT_DAY_NIGHT: u32 = 0x30;

// Audio fades — retail menu music ramps by 1 percent every 120ms.
pub const SLOT_AUDIO_FADE: u32 = 0x31;

// Game simulation — fixed 17ms (~58.8Hz) deterministic step, catch-up
pub const SLOT_GAME_TICK: u32 = 0x40;
pub const GAME_TICK_MS: u64 = 17;

// ============================================================================
// State machine mirroring CBulanci/CMenu structure from main_menu.md.
//
// The original separates two orthogonal concepts:
//   * AppPhase  — overall application phase (menu loop vs. live match).
//   * SubScreen — CMenu's currently-attached child view. The menu itself
//                 stays visible (left-side buttons, poem scroller, BG) the
//                 whole time. Sub-screens are NOT modal; clicking any of
//                 the main-menu buttons swaps to a different sub-screen
//                 via CMenu::Cmd_Dispatch (see §2.5 of main_menu.md).
// ============================================================================

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AppPhase {
    Intro,
    Menu,
    Connecting,
    Playing,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SubScreen {
    None,
    StartGame1,
    History,
    ExitConfirm,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum MenuBtn {
    Start,
    History,
    Quit,
}

impl MenuBtn {
    pub fn idx(self) -> usize {
        match self {
            MenuBtn::Start => 0,
            MenuBtn::History => 1,
            MenuBtn::Quit => 2,
        }
    }
}
