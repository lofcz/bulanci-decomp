//! Main left-side menu — three dial buttons (Start / Historie / Konec) and
//! the always-live keyboard shortcuts. Mirrors `CMenu::Cmd_Dispatch` (§2.5
//! of main_menu.md) and `CMenu::OnKeyDown` (§2.6).
//!
//! The sub-screens are NOT modal — clicking another main-menu button from
//! inside any sub-screen swaps to that button's sub-screen.

use macroquad::prelude::*;

use crate::app::ClientApp;
use crate::state::{MenuBtn, SubScreen};

/// Per-button clickable rectangle. Dial-sprite footprint is
/// `(35, 37 + 84*row)` for a 65-pixel-tall row. The label area (to the right of
/// the dial) is not hoverable or clickable in the original game and does not
/// contribute to the glow.
pub fn menu_btn_rect(btn: MenuBtn) -> Rect {
    let y = match btn {
        MenuBtn::Start => 37.0,
        MenuBtn::History => 121.0,
        MenuBtn::Quit => 205.0,
    };
    Rect::new(35.0, y, 64.0, 65.0)
}

/// Returns the main-menu button (if any) whose footprint contains `(x, y)`.
pub fn hit_test_menu(x: f32, y: f32) -> Option<MenuBtn> {
    [MenuBtn::Start, MenuBtn::History, MenuBtn::Quit]
        .into_iter()
        .find(|&btn| menu_btn_rect(btn).contains(vec2(x, y)))
}

impl ClientApp {
    /// Dispatch a main-menu click (CMenu::Cmd_Dispatch §2.5). Plays the
    /// per-button voice cue and swaps the active sub-screen.
    pub fn click_menu_btn(&mut self, btn: MenuBtn) {
        let is_already_active = matches!(
            (btn, self.sub_screen),
            (MenuBtn::Start, SubScreen::StartGame1)
                | (MenuBtn::History, SubScreen::History)
                | (MenuBtn::Quit, SubScreen::ExitConfirm)
        );
        if is_already_active {
            return;
        }

        self.pressed_btn = Some(btn);
        match btn {
            MenuBtn::Start => {
                // cmd 0xc9 — opens CStartGame1, plays slot 0x1b ("Start hry").
                self.play_sfx("audio/sfx_start.wav");
                self.sub_screen = SubScreen::StartGame1;
                self.sg1_active_item = 0;
                self.sg1_focused_group = 0;
            }
            MenuBtn::History => {
                // cmd 0xca — opens CHistoryDlg, plays slot 0x18 ("Historie").
                self.play_sfx("audio/sfx_history.wav");
                self.sub_screen = SubScreen::History;
            }
            MenuBtn::Quit => {
                // cmd 0xcb — opens CExitDlg, plays slot 0x19 ("Konec").
                self.play_sfx("audio/sfx_quit.wav");
                self.sub_screen = SubScreen::ExitConfirm;
            }
        }
    }

    /// Per-frame input handler for the menu phase. Fanout order: mouse
    /// hover/click on dial buttons → always-live keyboard shortcuts →
    /// per-sub-screen input.
    pub fn update_menu_input(&mut self) {
        let (mx, my) = crate::window_mode::logical_mouse_position();
        self.hovered_btn = hit_test_menu(mx, my);

        let click = is_mouse_button_pressed(MouseButton::Left);
        if click {
            if let Some(btn) = self.hovered_btn {
                self.click_menu_btn(btn);
                return;
            }
        }

        // ---- Keyboard shortcuts (CMenu::OnKeyDown §2.6). ----
        if is_key_pressed(KeyCode::S) {
            self.click_menu_btn(MenuBtn::Start);
            return;
        }
        if is_key_pressed(KeyCode::H) {
            self.click_menu_btn(MenuBtn::History);
            return;
        }
        if is_key_pressed(KeyCode::K) || is_key_pressed(KeyCode::X) {
            self.click_menu_btn(MenuBtn::Quit);
            return;
        }

        // ---- Per-sub-screen input. ----
        match self.sub_screen {
            SubScreen::None => {}
            SubScreen::StartGame1 => self.update_startgame1_input(mx, my, click),
            SubScreen::History => self.update_history_input(),
            SubScreen::ExitConfirm => self.update_exit_confirm_input(),
        }

        // ---- Menu-button brightness state machine (animation + SFX) ----
        //
        // Retail behaviour, from
        // `ghidra_analysis/gameplay/main_menu_hover_audio.md` and the
        // `scripts/frida/menu_hover_audio_trace.js` recordings:
        //
        //  * Each brightness track (glow-up = track 0, dim-down = track 1)
        //    plays for exactly 8 frames × 71 ms = 568 ms and is
        //    **uninterruptible** — `CSwitch_OnMouseEnter`/`OnMouseLeave`
        //    still call `PlayHoverTrack`/`PlayIdleTrack`, but those bail
        //    at the `Scheduler_GetEventSlot(CSwitch+0x80, 0)[+8] & 1`
        //    gate because the slot is disarmed while the animation is
        //    dispatching.
        //  * When the active track's frame index wraps from 7 → 0,
        //    `FUN_00439b90` invokes `TM_PauseAndStampClock` which re-
        //    arms the slot, and `CSwitch_OnAnimEnd` fires immediately
        //    with `frameIdx == -1`. OnAnimEnd consults the LIVE
        //    mouse-over / pressed state to pick the next direction:
        //       - track 0 ended + (!mouseOver && state==0) → PlayIdleTrack
        //       - track 1 ended + (mouseOver || state==1)  → PlayHoverTrack
        //  * `TriggerBankSample(0)` (the hover voice cue) is reachable
        //    **only** from inside `PlayHoverTrack`, so the SFX fires
        //    exactly when (and only when) the engine commits to a
        //    glow-up transition while the slot is armed. Mid-dim-down
        //    re-hovers therefore produce a *delayed* SFX: the dim-down
        //    keeps running to its end, OnAnimEnd then re-fires
        //    `PlayHoverTrack` which plays the cue and starts the glow-up.
        //
        // Our state mirrors the retail shape:
        //   * `menu_btn_hover_armed[idx]` ⇔ `slot[+8] & 1` (true = paused
        //     / gate open, false = animation dispatching / gate closed)
        //   * `menu_btn_tracks[idx]`      ⇔ `TM.currentTrackIdx`
        //   * `menu_btn_frames[idx]`      ⇔ `TM.frameIdx` mapped to `[0.0, 7.0]`
        //
        // The advancing branch advances `frame` and, on reaching `7.0`,
        // simultaneously arms the gate and applies the OnAnimEnd
        // transition. This collapses retail's
        // `FUN_00439b90 → TM_PauseAndStampClock → OnAnimEnd` chain
        // into one synchronous step; the only externally observable
        // difference is the ~1 frame OnAnimEnd lag retail incurs, which
        // is well under the 71 ms per-frame tick.
        let dt_ms = self.clock.elapsed_ms.saturating_sub(self.last_frame_ms);
        let dt = dt_ms as f32 / 1000.0;
        for btn in [MenuBtn::Start, MenuBtn::History, MenuBtn::Quit] {
            let idx = btn.idx();
            let is_active_for_subscreen = matches!(
                (btn, self.sub_screen),
                (MenuBtn::Start, SubScreen::StartGame1)
                    | (MenuBtn::History, SubScreen::History)
                    | (MenuBtn::Quit, SubScreen::ExitConfirm)
            );
            let is_hovered = self.hovered_btn == Some(btn);
            let want_glow = is_active_for_subscreen || is_hovered;

            if self.menu_btn_hover_armed[idx] {
                // Gate open: animation parked at the end of whichever
                // track last completed (or at the boot-seeded
                // track=1/frame=7 "fully dim" rest). The opposite-
                // direction transition mirrors retail's
                // `PlayHoverTrack`/`PlayIdleTrack` succeeding through
                // the armed gate from `OnMouseEnter`/`OnMouseLeave`.
                match (self.menu_btn_tracks[idx], want_glow) {
                    (1, true) => {
                        // Dim at rest + want bright → glow-up + SFX
                        // (unless this button is the currently selected
                        // sub-screen, mirroring `OnMouseEnter`'s
                        // `if (state_0xc4 == 0)` short-circuit).
                        self.menu_btn_tracks[idx] = 0;
                        self.menu_btn_frames[idx] = 0.0;
                        self.menu_btn_hover_armed[idx] = false;
                        if !is_active_for_subscreen {
                            self.play_hover_sfx("audio/sfx_hover.wav");
                        }
                    }
                    (0, false) => {
                        // Bright at rest + want dim → dim-down (no SFX).
                        self.menu_btn_tracks[idx] = 1;
                        self.menu_btn_frames[idx] = 0.0;
                        self.menu_btn_hover_armed[idx] = false;
                    }
                    _ => {
                        // Stable rest: either dim with no hover, or
                        // bright with hover/selected.
                    }
                }
            } else {
                // Gate closed: animation dispatching. Mouse changes are
                // observed but produce no transition until completion.
                self.menu_btn_frames[idx] =
                    (self.menu_btn_frames[idx] + dt / 0.071).min(7.0);
                if self.menu_btn_frames[idx] >= 7.0 {
                    // Track wrap — retail's `FUN_00439b90` re-arms here
                    // and `OnAnimEnd` picks the next direction from the
                    // live `want_glow`.
                    self.menu_btn_hover_armed[idx] = true;
                    match (self.menu_btn_tracks[idx], want_glow) {
                        (0, false) => {
                            // Glow-up ended + mouse off → dim-down (no SFX).
                            self.menu_btn_tracks[idx] = 1;
                            self.menu_btn_frames[idx] = 0.0;
                            self.menu_btn_hover_armed[idx] = false;
                        }
                        (1, true) => {
                            // Dim-down ended + still hovering → glow-up + SFX.
                            // This is the retail "delayed hover SFX" — the
                            // cue waits for the in-flight dim-down to
                            // finish, then fires only if the cursor is
                            // still over the button at that instant.
                            self.menu_btn_tracks[idx] = 0;
                            self.menu_btn_frames[idx] = 0.0;
                            self.menu_btn_hover_armed[idx] = false;
                            if !is_active_for_subscreen {
                                self.play_hover_sfx("audio/sfx_hover.wav");
                            }
                        }
                        _ => {
                            // Glow-up ended + still hovering: stay bright.
                            // Dim-down ended + mouse off: stay dim.
                        }
                    }
                }
            }
        }
    }
}

#[cfg(test)]
#[path = "menu_tests.rs"]
mod tests;
