//! `CHistoryDlg` sub-screen — paged read-only poem display with
//! left/right keyboard navigation.

use macroquad::prelude::*;

use crate::app::ClientApp;
use crate::poem::Align;
use crate::state::SubScreen;

impl ClientApp {
    pub fn update_history_input(&mut self) {
        if is_key_pressed(KeyCode::Escape) {
            self.sub_screen = SubScreen::None;
            self.pressed_btn = None;
        }
        if (is_key_pressed(KeyCode::Right) || is_key_pressed(KeyCode::Down))
            && !self.poems.is_empty()
        {
            self.history_page = (self.history_page + 1) % self.poems.len();
        }
        if (is_key_pressed(KeyCode::Left) || is_key_pressed(KeyCode::Up))
            && !self.poems.is_empty()
        {
            self.history_page = (self.history_page + self.poems.len() - 1) % self.poems.len();
        }
        if is_key_pressed(KeyCode::Home) {
            self.history_page = 0;
        }
        if is_key_pressed(KeyCode::End) && !self.poems.is_empty() {
            self.history_page = self.poems.len() - 1;
        }
    }

    pub fn draw_history(&mut self) {
        // ---- bg_history.jpg (resource 0x1013e) at right portion. ----
        let bg_history = self.get_texture("images/bg_history.jpg");
        let x = 800.0 - bg_history.width();
        let y = 600.0 - bg_history.height();
        draw_texture(&bg_history, x, y, WHITE);

        // ---- CHistoryDlg window. ----
        const BX: f32 = 250.0;
        const BY: f32 = 60.0;
        const BW: f32 = 510.0;
        const BH: f32 = 500.0;

        // Title.
        self.draw_t("HISTORIE", BX + 20.0, BY + 30.0, 22, Color::new(0.95, 0.85, 0.4, 1.0));
        draw_line(
            BX + 20.0, BY + 40.0,
            BX + BW - 20.0, BY + 40.0,
            1.5,
            Color::new(0.95, 0.85, 0.4, 1.0),
        );

        // Render current poem (clipped to dialog inner rect).
        if !self.poems.is_empty() {
            let poem = &self.poems[self.history_page];
            let font_size: u16 = 16;
            let line_h = 22.0;
            let mut y = BY + 70.0;
            let inner_w = BW - 40.0;
            for line in poem {
                let tw = self.measure_t(&line.text, font_size);
                let x = match line.align {
                    Align::Left   => BX + 20.0,
                    Align::Center => BX + 20.0 + (inner_w - tw) * 0.5,
                    Align::Right  => BX + BW - 20.0 - tw,
                };
                if y < BY + BH - 40.0 {
                    self.draw_t(&line.text, x, y, font_size, Color::new(0.95, 0.95, 0.95, 1.0));
                }
                y += line_h;
            }
        }

        // Navigation prompt.
        let prompt = format!(
            "Strana {} / {}    < — →    Esc pro návrat",
            self.history_page + 1,
            self.poems.len().max(1),
        );
        self.draw_t(&prompt, BX + 20.0, BY + BH - 18.0, 14, Color::new(0.7, 0.7, 0.7, 1.0));
    }
}
