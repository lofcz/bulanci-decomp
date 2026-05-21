//! `CExitDlg` sub-screen — Y/N confirmation modal for app exit.
//!
//! Per §5.3 of main_menu.md the original places the dialog at
//! `(221, 24, 762, 569)` inside the menu chrome; we clamp to the 800×600
//! viewport.

use macroquad::prelude::*;

use crate::app::ClientApp;
use crate::state::SubScreen;

impl ClientApp {
    pub fn update_exit_confirm_input(&mut self) {
        if is_key_pressed(KeyCode::Enter) || is_key_pressed(KeyCode::Y) {
            std::process::exit(0);
        }
        if is_key_pressed(KeyCode::Escape) || is_key_pressed(KeyCode::N) {
            self.sub_screen = SubScreen::None;
            self.pressed_btn = None;
        }
    }

    pub fn draw_exit_confirm(&mut self) {
        // bg_quit.jpg is the exit-dialog background (CExitDlg, 0x1013d).
        let bg_quit = self.get_texture("images/bg_quit.jpg");
        draw_texture(&bg_quit, 203.0, 169.0, WHITE);
        draw_rectangle_lines(203.0, 169.0, 394.0, 261.0, 1.5, Color::new(0.9, 0.9, 0.9, 0.95));

        let title = "Opravdu chcete skončit?";
        let tw = self.measure_t(title, 20);
        self.draw_t(title, 203.0 + (394.0 - tw) * 0.5, 240.0, 20, Color::new(1.0, 0.85, 0.4, 1.0));

        self.draw_t("[Enter / Y]  Ano", 240.0, 310.0, 16, Color::new(0.6, 1.0, 0.6, 1.0));
        self.draw_t("[Esc / N]    Ne",  440.0, 310.0, 16, Color::new(1.0, 0.7, 0.7, 1.0));
    }
}
