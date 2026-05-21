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
/// `(35, 37 + 84*row)` for a 65-pixel-tall row; we extend the box across
/// the dial AND its label so the user can click on either half.
pub fn menu_btn_rect(btn: MenuBtn) -> Rect {
    let y = match btn {
        MenuBtn::Start => 37.0,
        MenuBtn::History => 121.0,
        MenuBtn::Quit => 205.0,
    };
    Rect::new(35.0, y, 210.0, 65.0)
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
        self.pressed_btn = Some(btn);
        match btn {
            MenuBtn::Start => {
                // cmd 0xc9 — opens CStartGame1, plays slot 0x1b ("Start hry").
                self.play_sfx("audio/sfx_start.wav");
                self.sub_screen = SubScreen::StartGame1;
                self.sg1_active_item = 0;
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
        let (mx, my) = mouse_position();
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
    }
}

#[cfg(test)]
#[path = "menu_tests.rs"]
mod tests;
