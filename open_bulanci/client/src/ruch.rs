//! `CRuch` — background phosphor-scanline effect (4 instances).
//!
//! This is a 1-pixel-wide vertical "scanline glitch" that flashes briefly
//! at a random horizontal position on the menu background. The original
//! `CMenu` owns exactly four `CRuch` instances (see
//! `CMenu_EnableAllRuch @ 0x00423f70` which iterates a 4-slot table at
//! `this+0xd0`) and ticks each one independently via the scheduler.
//!
//! ## Verified state machine (per-instance)
//!
//! Decompiled from Ghidra at the addresses listed below:
//!
//! * `CRuch_ctor @ 0x00423c20`
//!   1. Picks initial `x = rand() * screen_w / 32768`, sets the view
//!      rect to `(x, 0, x+1, screen_h)` where
//!      `screen_w = g_pApp[0x28] - g_pApp[0x20]` and
//!      `screen_h = g_pApp[0x2c] - g_pApp[0x24]` — i.e. the rect spans
//!      the entire screen vertically from the very first to the very
//!      last scanline. **Confirmed via Ghidra MCP @ 0x00423c20.**
//!   2. `CDSView::Hide` — starts **hidden**.
//!   3. Registers scheduler event slot 0 with delay
//!      `(rand() * 0xfa1) >> 15 ∈ [0, 4000]` ms.
//!
//! * `CRuch_OnScheduledTick @ 0x00423de0`
//!   - If currently **visible**: reprograms its slot to
//!     `(rand() * 0xfa1) >> 15 ∈ [0, 4000]` ms, calls
//!     `CDSView::Hide`, then `CDSView::SetPosition(new_x, 0)`
//!     where `new_x = rand() * screen_w / 32768`.
//!   - If currently **hidden**: reprograms its slot to **100 ms**,
//!     calls `CDSView::Show`. (Position is NOT re-rolled here — it was
//!     teleported when it went hidden, and the next visible flash will
//!     use that already-rolled X.)
//!
//! * `CRuch_Render @ 0x00423e60`
//!   - Calls `CDSImage::DrawVerticalLine(this, this[+0x30] /*x*/,
//!     this[+0x34] /*y_top*/, this[+0x3c] /*y_bottom*/,
//!     color1=0x3f3f3f, color2=0xffffff)`. The view rect is
//!     `(left=+0x30, top=+0x34, right=+0x38, bottom=+0x3c)` — note
//!     `bottom` lives at +0x3c, **not** +0x38.
//!   - The `color2=0xffffff` sentinel selects the "solid color"
//!     branch of every per-pixel-format dispatcher used by the
//!     backbuffer. **Verified via Ghidra MCP** on all three format
//!     branches:
//!       * `FUN_00445740` (16-bit RGB565): `*pixel = color1`
//!         encoded to RGB565, no LUT, no blending.
//!       * `FUN_00445900` (24-bit RGB888): `*pixel = color1` as a
//!         raw `u16 + u8` low/hi store, no LUT.
//!       * `FUN_00445a70` (32-bit RGBA): `*pixel = color1` as a
//!         raw `u32` store, no LUT.
//!     So this is a SOLID `0x3f3f3f = rgb(63, 63, 63)` (~24.7 % gray)
//!     column at full opacity — **not** a gradient, **not**
//!     alpha-blended, and **not** tinted by `color2`. We render the
//!     exact same 8-bit triple via macroquad's straight-RGB pipeline.
//!
//! * `CRuch_EnableDisable @ 0x00423ea0`
//!   - On disable: hide view, pause slot 0.
//!   - On enable: unpause slot 0.
//!
//! ## Properties confirmed
//!
//! * **No min/max distance constraint between lines.** Each `CRuch`
//!   independently rolls its own random X with no awareness of the
//!   others — two columns can overlap or sit adjacent.
//! * **Multiple lines can be visible simultaneously.** Each of the 4
//!   instances has its own scheduler slot; nothing serializes them.
//!   In practice each is visible 100 ms out of ~2050 ms on average
//!   (≈4.9% duty cycle), so seeing two at once is statistically rare
//!   but not prevented.
//! * **Position teleports on the visible→hidden transition.** Not on
//!   the hidden→visible transition. So the new X is "primed" during
//!   the hidden interval and the next flash appears at that position.
//! * **Initial delay equals the hidden-phase delay.** Both use the
//!   same `(rand() * 0xfa1) >> 15` formula → [0, 4000] ms.

use macroquad::prelude::*;

/// Visible-phase duration, in milliseconds. Matches the literal `100`
/// passed to `Scheduler_SetEventDelayMs` in `CRuch_OnScheduledTick`.
pub const VISIBLE_DURATION_MS: u64 = 100;

/// Upper bound (inclusive) of the random hidden-phase duration, in
/// milliseconds. The original computes `(rand() * 0xfa1) >> 15` where
/// `0xfa1 = 4001` and `rand()` ranges over `[0, 32767]`, giving an
/// integer result in `[0, 4000]`.
pub const HIDDEN_MAX_MS: u64 = 4000;

/// 0x3f gray channel — matches the literal `color1=0x3f3f3f` passed to
/// `CDSImage::DrawVerticalLine` by `CRuch_Render`. R = G = B = 0x3f
/// gives a pure neutral gray at exactly 63/255 ≈ 24.7 % intensity, no
/// red/green/blue bias. Re-verified via Ghidra MCP on all three
/// per-pixel-format branches (16/24/32-bit) — none of them apply any
/// LUT or alpha blend when `color2 == 0xffffff`, so this is the exact
/// on-screen color of every ruch scanline in the original game.
const COLOR_CHANNEL: f32 = 0x3f as f32 / 255.0;

pub struct Ruch {
    /// Current X column in screen pixels. The original stores
    /// `rect.left` and `rect.right = rect.left + 1`; we keep just the
    /// left edge and rebuild the 1-pixel-wide rect at draw time.
    pub x: f32,
    /// `CDSView` visibility flag (bit 0 of `*((int*)this + 0x44)` in
    /// the original). Starts `false` — the ctor calls `CDSView::Hide`.
    pub visible: bool,
    /// Scheduler slot id (so the caller can route `set_delay_ms` back
    /// to the right slot).
    pub slot_id: u32,
}

impl Ruch {
    /// Construct a new Ruch, mirroring `CRuch_ctor @ 0x00423c20`:
    /// picks a random initial X, starts hidden, slot owner registers
    /// the scheduler entry with a random initial delay separately.
    pub fn new(screen_w: f32, slot_id: u32) -> Self {
        Ruch {
            x: rand::gen_range(0.0, screen_w),
            visible: false,
            slot_id,
        }
    }

    /// Return the random initial slot delay (ms). Same formula and
    /// range as the hidden-phase delay; computed here so the call site
    /// in `app.rs` doesn't have to duplicate the magic number.
    pub fn random_hidden_delay() -> u64 {
        rand::gen_range(0u64, HIDDEN_MAX_MS + 1)
    }

    /// Scheduler-driven toggle. Returns the next slot delay (ms) for
    /// the caller to write back via `Scheduler::set_delay_ms`. Direct
    /// port of `CRuch_OnScheduledTick @ 0x00423de0`.
    ///
    /// Note that the position re-roll happens on the **visible→hidden**
    /// edge (not on the hidden→visible edge) — matching the original's
    /// call order: `SetEventDelayMs` → `CDSView::Hide` →
    /// `CDSView::SetPosition`.
    pub fn on_toggle_tick(&mut self, screen_w: f32) -> u64 {
        if self.visible {
            self.visible = false;
            self.x = rand::gen_range(0.0, screen_w);
            Self::random_hidden_delay()
        } else {
            self.visible = true;
            VISIBLE_DURATION_MS
        }
    }

    /// Draw the column if visible. Solid `0x3f3f3f` at full opacity,
    /// 1 pixel wide, from y=0 to `screen_h` — see module-level docs
    /// for the `CRuch_Render` derivation.
    pub fn draw(&self, screen_h: f32) {
        if !self.visible {
            return;
        }
        // Pixel-snap so the column lands on an integer column boundary
        // and doesn't smear across two pixels at fractional X.
        let x = self.x.round();
        draw_rectangle(
            x,
            0.0,
            1.0,
            screen_h,
            Color::new(COLOR_CHANNEL, COLOR_CHANNEL, COLOR_CHANNEL, 1.0),
        );
    }
}

#[cfg(test)]
#[path = "ruch_tests.rs"]
mod tests;
