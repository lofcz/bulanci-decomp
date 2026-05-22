//! `CExitDlg` sub-screen — Y/N confirmation modal for app exit.
//!
//! Per §5.3 of main_menu.md the original places the dialog at
//! `(221, 24, 762, 569)` inside the menu chrome; we clamp to the 800×600
//! viewport.

use macroquad::prelude::*;

use crate::app::ClientApp;
use crate::audio::AudioFade;
use crate::state::{SLOT_AUDIO_FADE, SubScreen};

const RETAIL_EXIT_FALLBACK_DELAY_MS: u64 = 2_083;

impl ClientApp {
    pub fn update_exit_confirm_input(&mut self) {
        if self.exit_at_ms.is_some() {
            return;
        }
        if is_key_pressed(KeyCode::Enter) || is_key_pressed(KeyCode::Y) {
            self.begin_retail_exit();
        }
        if is_key_pressed(KeyCode::Escape) || is_key_pressed(KeyCode::N) {
            self.sub_screen = SubScreen::None;
            self.pressed_btn = None;
        }
    }

    pub(crate) fn begin_retail_exit(&mut self) {
        if self.exit_at_ms.is_some() {
            return;
        }
        // Retail X/close-event path plays AudioBank slot 0x1a ("Konec hry")
        // and lets the CDSAudioPlayer completion event unwind the CMenu modal.
        let delay_ms = self
            .play_sfx_with_duration("audio/sfx_force_exit.wav")
            .unwrap_or(RETAIL_EXIT_FALLBACK_DELAY_MS);
        self.exit_at_ms = Some(self.clock.elapsed_ms.saturating_add(delay_ms));
        if let Some(audio) = &self.audio {
            audio.start_bg_fade(AudioFade::linear_percent(70, 1));
            self.scheduler.unpause(SLOT_AUDIO_FADE, self.clock.elapsed_ms);
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
