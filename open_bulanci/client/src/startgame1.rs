//! `CStartGame1` sub-screen — host/join + player-count radios + "Dále"
//! continue button. Per §3.1 of main_menu.md the form has 9 selectable
//! rows + a continue button.

use macroquad::prelude::*;

use crate::app::ClientApp;
use crate::render::{TEXT_COLOR_PRIMARY, TEXT_COLOR_STATIC};
use crate::state::SubScreen;

/// Per `CStartGame1::BuildUi @ 0x0040c650`, every header static text is
/// built via `CStaticText::BuildAtAuto(this, x, y, smartptr, 2, 0x100ae)`.
/// `param_4 = 2` lands at `this+0x70`, which is `font_info[+4]` —
/// i.e. `char_spacing = 2` between every pair of glyphs. The radios
/// underneath use the same value (`CRadio::BuildAt` sets `this+0x84 = 2`),
/// so the headers and the rows beneath them share the same letter rhythm.
const STATIC_TEXT_CHAR_SPACING: i32 = 2;

/// Same source — `CRadio::BuildAt @ 0x00408540` stores `2` in the
/// `font_info[+4]` slot of every radio group.
const RADIO_CHAR_SPACING: i32 = 2;

/// Per `CButton::BuildAt @ 0x004087f0`, every CButton's font-info block
/// has `+0x7c = 1`. The dialog buttons (incl. "Dále") rendered through
/// this widget all get the same 1-px inter-glyph gap.
const BUTTON_CHAR_SPACING: i32 = 1;

/// Y-coordinates of the 9 radio rows (item index 0..=8).
/// Item 9 is the "Dále" button drawn separately at (350, 480, 120, 35).
const ITEM_YS: [f32; 9] = [135.0, 160.0, 230.0, 255.0, 280.0, 305.0, 375.0, 400.0, 425.0];

const ROW_X_MIN:  f32 = 275.0;
const ROW_X_MAX:  f32 = 460.0;
const BTN_RECT:   Rect = Rect { x: 350.0, y: 480.0, w: 120.0, h: 35.0 };

impl ClientApp {
    /// Map mouse coords to the radio-row / button index they hover. Item 9
    /// is the "Dále" continue button.
    fn sg1_hit_test(mx: f32, my: f32) -> Option<usize> {
        for (i, &y) in ITEM_YS.iter().enumerate() {
            if (ROW_X_MIN..=ROW_X_MAX).contains(&mx)
                && (y - 15.0..=y + 10.0).contains(&my)
            {
                return Some(i);
            }
        }
        if BTN_RECT.contains(vec2(mx, my)) {
            return Some(9);
        }
        None
    }

    pub fn update_startgame1_input(&mut self, mx: f32, my: f32, click: bool) {
        // Mouse hover over checkbox rows or the Dále button.
        let hovered = Self::sg1_hit_test(mx, my);
        if let Some(i) = hovered {
            self.sg1_active_item = i;
        }

        // Keyboard navigation.
        if is_key_pressed(KeyCode::Down) {
            self.sg1_active_item = (self.sg1_active_item + 1) % 10;
        }
        if is_key_pressed(KeyCode::Up) {
            self.sg1_active_item = (self.sg1_active_item + 9) % 10;
        }

        if is_key_pressed(KeyCode::Escape) {
            self.sub_screen = SubScreen::None;
            self.pressed_btn = None;
            return;
        }

        let activate = is_key_pressed(KeyCode::Enter) || (click && hovered.is_some());
        if activate {
            self.activate_sg1_item(self.sg1_active_item);
        }

        // IP input (only meaningful while Join is the active server mode).
        if self.sg1_total_players > 1 && self.sg1_server_mode == 1 {
            while let Some(c) = get_char_pressed() {
                if (c.is_ascii_digit() || c == '.' || c == ':')
                    && self.ip_input_buffer.len() < 21
                {
                    self.ip_input_buffer.push(c);
                }
            }
            if is_key_pressed(KeyCode::Backspace) {
                self.ip_input_buffer.pop();
            }
        }
    }

    /// Activate-on-Enter / activate-on-click handler. Each item flips a
    /// radio or kicks off the match.
    fn activate_sg1_item(&mut self, idx: usize) {
        match idx {
            0 => self.sg1_server_mode = 0,
            1 if self.sg1_total_players > 1 => self.sg1_server_mode = 1,
            2 => {
                self.sg1_total_players = 1;
                self.sg1_local_players = 1;
                self.sg1_server_mode = 0;
            }
            3 => {
                self.sg1_total_players = 2;
                self.sg1_local_players = self.sg1_local_players.min(2);
            }
            4 => {
                self.sg1_total_players = 3;
                self.sg1_local_players = self.sg1_local_players.min(3);
            }
            5 => self.sg1_total_players = 4,
            6 => self.sg1_local_players = 1,
            7 if self.sg1_total_players >= 2 => self.sg1_local_players = 2,
            8 if self.sg1_total_players >= 3 => self.sg1_local_players = 3,
            9 => self.kick_off_match(),
            _ => {}
        }
    }

    /// Translate the StartGame1 form state into a server-address dial and
    /// fire `try_connect`. For now: training / host both connect to the
    /// local listen-server; join dials the configured IP. The lobby
    /// (CStartGame2) is not yet wired up, so we go straight to the live
    /// simulation.
    fn kick_off_match(&mut self) {
        let addr_str = if self.sg1_total_players > 1 && self.sg1_server_mode == 1 {
            if self.ip_input_buffer.contains(':') {
                self.ip_input_buffer.clone()
            } else {
                format!("{}:34568", self.ip_input_buffer)
            }
        } else {
            "127.0.0.1:34568".to_string()
        };
        if let Ok(addr) = addr_str.parse() {
            self.server_addr = addr;
            let _ = self.try_connect();
        }
    }

    pub fn draw_startgame1(&mut self) {
        // ---- bg_start.jpg (resource 0x1013c) overlays the right-bottom. ----
        let bg_start = self.get_texture("images/bg_start.jpg");
        let x = 800.0 - bg_start.width();
        let y = 600.0 - bg_start.height();
        draw_texture(&bg_start, x, y, WHITE);

        // Headers are `CStaticText` widgets rendered with the large
        // CDSFont (`0x100ae`) at `char_spacing = 2` — see
        // `CStartGame1::BuildUi @ 0x0040c650`:
        //
        //   uVar10 = 0x100ae;   // font_id
        //   uVar8  = 2;         // → font_info[+4] = char_spacing
        //   uVar7  = 0;         // smartptr stash
        //   CStaticText_BuildAtAuto(pCVar1, 0x14, 0x14,
        //                           uVar7, uVar8, uVar10);
        //
        // The colour is `0x909090`: `CStaticText::Render @ 0x004054d0`
        // unconditionally writes `0x909090` into `font_info[+0x10]`
        // when `+0x94 == 0`, and `TextShaper_LayOutAndRender` drives
        // `CDSFont::DrawChar` in with-outline mode for it. That
        // produces the dimmer grey the original game uses for every
        // group caption ("Hru:", "Počet hráčů celkem:", …), distinct
        // from the brighter `0xCECECE` the radio rows below use.
        let header_color = TEXT_COLOR_STATIC;
        self.draw_t_sp("Hru:",                280.0, 110.0, 16, STATIC_TEXT_CHAR_SPACING, header_color);
        self.draw_t_sp("Počet hráčů celkem:", 280.0, 205.0, 16, STATIC_TEXT_CHAR_SPACING, header_color);
        self.draw_t_sp("Na tomto počítači:",  280.0, 350.0, 16, STATIC_TEXT_CHAR_SPACING, header_color);

        // Per §3.1 the StartGame1 form has 9 selectable rows + a continue button.
        let items: [(f32, &str, bool, bool); 9] = [
            (135.0, "Vytvořit",         self.sg1_server_mode == 0, false),
            (160.0, "Připojit se",      self.sg1_server_mode == 1, self.sg1_total_players == 1),
            (230.0, "1 hráč (trénink)", self.sg1_total_players == 1, false),
            (255.0, "2 hráči",          self.sg1_total_players == 2, false),
            (280.0, "3 hráči",          self.sg1_total_players == 3, false),
            (305.0, "4 hráči",          self.sg1_total_players == 4, false),
            (375.0, "1 hráč",           self.sg1_local_players == 1, false),
            (400.0, "2 hráči",          self.sg1_local_players == 2, self.sg1_total_players < 2),
            (425.0, "3 hráči",          self.sg1_local_players == 3, self.sg1_total_players < 3),
        ];

        for &(y, text, checked, disabled) in items.iter() {
            // `CRadio_Render` paints disabled rows via the dim-grey
            // sentinel `0x303030`, then re-tints back to opaque at the
            // end of the loop (`+0x90 = 0xffffffff`). We reuse the
            // primary `0xCECECE` colour for the enabled state and dim
            // it down for greyed-out rows.
            let color = if disabled {
                Color::new(0.45, 0.45, 0.45, 1.0)
            } else {
                TEXT_COLOR_PRIMARY
            };
            // Square box outline.
            draw_rectangle_lines(280.0, y - 12.0, 14.0, 14.0, 1.5, color);
            // Steel-grate filling (4 vertical bars) when checked.
            if checked {
                for off in [3.0_f32, 6.0, 9.0, 11.0] {
                    draw_line(280.0 + off, y - 10.0, 280.0 + off, y, 1.5, color);
                }
            }
            // Radio labels use the medium CDSFont (`0x100af`, picked
            // by the size-15 hint) with `char_spacing = 2` so the
            // letters breathe the same way the original game's radio
            // rows do — see `RADIO_CHAR_SPACING` above.
            self.draw_t_sp(text, 305.0, y + 1.0, 15, RADIO_CHAR_SPACING, color);
        }

        // ---- "Dále" continue button. ----
        let btn_focused = self.sg1_active_item == 9;
        let btn_bg = if btn_focused {
            Color::new(0.25, 0.12, 0.12, 1.0)
        } else {
            Color::new(0.12, 0.12, 0.12, 1.0)
        };
        draw_rectangle(BTN_RECT.x, BTN_RECT.y, BTN_RECT.w, BTN_RECT.h, btn_bg);
        draw_rectangle_lines(BTN_RECT.x, BTN_RECT.y, BTN_RECT.w, BTN_RECT.h, 1.5, Color::new(0.9, 0.9, 0.9, 1.0));
        // `CButton` ("Dále") uses the large CDSFont (`0x100ae`,
        // matched here by size 18) with `BUTTON_CHAR_SPACING = 1`.
        let dw = self.measure_t_sp("Dále", 18, BUTTON_CHAR_SPACING);
        self.draw_t_sp(
            "Dále",
            BTN_RECT.x + (BTN_RECT.w - dw) * 0.5,
            BTN_RECT.y + 25.0,
            18,
            BUTTON_CHAR_SPACING,
            TEXT_COLOR_PRIMARY,
        );

        // ---- Optional IP input box (only when Join is active). ----
        if self.sg1_total_players > 1 && self.sg1_server_mode == 1 {
            self.draw_t("IP hostitele:", 445.0, 350.0, 14, Color::new(1.0, 0.85, 0.2, 1.0));
            draw_rectangle(445.0, 360.0, 180.0, 25.0, Color::new(0.10, 0.10, 0.10, 0.85));
            draw_rectangle_lines(445.0, 360.0, 180.0, 25.0, 1.0, WHITE);
            self.draw_t(&self.ip_input_buffer, 452.0, 378.0, 14, Color::new(0.6, 1.0, 0.6, 1.0));
        }

        // Keyboard focus indicator — a thin underline beneath the currently
        // selected row.
        if self.sg1_active_item < ITEM_YS.len() {
            let y = ITEM_YS[self.sg1_active_item];
            draw_line(
                275.0, y + 4.0,
                455.0, y + 4.0,
                1.0,
                Color::new(0.95, 0.85, 0.3, 0.55),
            );
        } else if self.sg1_active_item == 9 {
            draw_rectangle_lines(
                BTN_RECT.x - 2.0, BTN_RECT.y - 2.0,
                BTN_RECT.w + 4.0, BTN_RECT.h + 4.0,
                1.0,
                Color::new(0.95, 0.85, 0.3, 0.6),
            );
        }
    }
}

#[cfg(test)]
#[path = "startgame1_tests.rs"]
mod tests;
