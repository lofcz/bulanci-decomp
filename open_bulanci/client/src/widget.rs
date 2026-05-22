//! Reusable UI widgets for the Bulanci client menu system.
//!
//! Ported and reverse-engineered from the original Bulanci `CRadio` and `CButton` UI components.

use macroquad::prelude::*;
use crate::app::ClientApp;
use crate::state::{MenuBtn, SubScreen};
use crate::render::{TEXT_COLOR_PRIMARY, TEXT_COLOR_STATIC};

/// Character spacing for radio button labels.
const RADIO_CHAR_SPACING: i32 = 2;

/// Color for disabled UI elements, matching the original game's `0x303030`.
const DISABLED_COLOR: Color = Color::new(0.35, 0.35, 0.35, 1.0);

/// Forced tint used by unfocused `CRadio` groups. Retail computes this in
/// `_Globals::FUN_00402f40(flags & 6)`: inactive, non-disabled widgets get
/// `0x858585`, while the focused-widget sentinel (`0x909090`) leaves the
/// normal bright style block intact.
const UNFOCUSED_GROUP_COLOR: Color = Color::new(
    0x85 as f32 / 255.0,
    0x85 as f32 / 255.0,
    0x85 as f32 / 255.0,
    1.0,
);

/// A reusable radio row widget that handles rendering checkbox-style list items
/// in their authentic original Bulanci menu styles.
///
/// Handles 5 distinct rendering states based on selection, hover, and press states:
/// 1. Unselected, Unhovered: Gray text (`TEXT_COLOR_STATIC`), `radio_unselected.png` checkbox.
/// 2. Unselected, Hovered: Bright text (`TEXT_COLOR_PRIMARY`), `radio_unselected_hover.png` checkbox.
/// 3. Selected, Unhovered: Bright text (`TEXT_COLOR_PRIMARY`), `radio_selected.png` checkbox.
/// 4. Selected, Hovered: Bright text (`TEXT_COLOR_PRIMARY`), `radio_selected_hover.png` checkbox.
/// 5. Pressed (active): Bright text (`TEXT_COLOR_PRIMARY`), `radio_active.png` checkbox.
///
/// It also handles disabled states by dimming both the text and the checkbox using `DISABLED_COLOR` (0x303030).
pub struct RadioRowWidget;

impl RadioRowWidget {
    /// Renders a single radio row option at the specified Y-coordinate.
    pub fn draw(
        app: &mut ClientApp,
        label: &str,
        y: f32,
        checked: bool,
        disabled: bool,
        group_focused: bool,
        hovered: bool,
        pressed: bool,
    ) {
        // 1. Determine text color and checkbox texture path based on state
        let (mut text_color, tex_path) = if disabled {
            // Disabled uses highly dimmed gray text
            let disabled_tex = if checked {
                "images/radio_selected.png"
            } else {
                "images/radio_unselected.png"
            };
            (DISABLED_COLOR, disabled_tex)
        } else if pressed {
            // Active click/press state
            (TEXT_COLOR_PRIMARY, "images/radio_active.png")
        } else if checked {
            // Selected states
            if hovered {
                (TEXT_COLOR_PRIMARY, "images/radio_selected_hover.png")
            } else {
                (TEXT_COLOR_PRIMARY, "images/radio_selected.png")
            }
        } else {
            // Unselected states
            if hovered {
                (TEXT_COLOR_PRIMARY, "images/radio_unselected_hover.png")
            } else {
                // Dimmer gray color for unselected unhovered elements
                (TEXT_COLOR_STATIC, "images/radio_unselected.png")
            }
        };

        if !disabled && !group_focused {
            text_color = UNFOCUSED_GROUP_COLOR;
        }

        // 2. Load the checkbox texture
        let tex = app.get_texture(tex_path);

        // 3. Draw the checkbox
        // Top-left is at x = 280.0, y = y
        let tint = if disabled {
            DISABLED_COLOR
        } else if group_focused {
            WHITE
        } else {
            UNFOCUSED_GROUP_COLOR
        };
        draw_texture(&tex, 280.0, y, tint);

        // 4. Draw the label text
        // Baseline is at x = 306.0, y = y + 13.0 with character spacing of 2
        app.draw_t_sp(label, 306.0, y + 13.0, 15, RADIO_CHAR_SPACING, text_color);
    }
}

/// A reusable menu button widget that handles rendering start, history, and exit buttons
/// with smooth, progressive glow-up and dim-down animations using custom FLX sprite sheets.
pub struct MenuButtonWidget;

impl MenuButtonWidget {
    /// Renders a single menu button, managing both its dial sphere animation and its text icon.
    pub fn draw(app: &mut ClientApp, btn: MenuBtn) {
        let idx = btn.idx();
        let y_dial = 37.0 + idx as f32 * 84.0;
        let y_label = 55.0 + idx as f32 * 84.0;

        // 1. Render the dial using the appropriate animation track and frame index
        let track_idx = app.menu_btn_tracks[idx];
        let frame_val = app.menu_btn_frames[idx];

        let track_ref = if track_idx == 0 {
            &app.menu_btn_off
        } else {
            &app.menu_btn_on
        };

        // There are exactly 8 frames of animation in the transition (0..=7)
        let frame_idx = (frame_val.round() as usize).min(track_ref.frame_count() - 1);
        let frame = &track_ref.frames[frame_idx];
        let texture = track_ref.texture();

        let rect = frame.rect;
        let fw = rect.w as f32;
        let fh = rect.h as f32;

        draw_texture_ex(
            texture,
            35.0,
            y_dial,
            WHITE,
            DrawTextureParams {
                source: Some(Rect::new(rect.x as f32, rect.y as f32, fw, fh)),
                dest_size: Some(vec2(fw, fh)),
                ..Default::default()
            },
        );

        // 2. Render the label icon (binary swap based on whether we are hovered/active)
        let is_active_for_subscreen = matches!(
            (btn, app.sub_screen),
            (MenuBtn::Start, SubScreen::StartGame1)
                | (MenuBtn::History, SubScreen::History)
                | (MenuBtn::Quit, SubScreen::ExitConfirm)
        );
        let is_hovered = app.hovered_btn == Some(btn);
        let highlight = is_active_for_subscreen || is_hovered;

        let label_path = match (btn, highlight) {
            (MenuBtn::Start,   true)  => "images/icon_start_hi.png",
            (MenuBtn::Start,   false) => "images/icon_start.png",
            (MenuBtn::History, true)  => "images/icon_history_hi.png",
            (MenuBtn::History, false) => "images/icon_history.png",
            (MenuBtn::Quit,    true)  => "images/icon_quit_hi.png",
            (MenuBtn::Quit,    false) => "images/icon_quit.png",
        };
        let label_tex = app.get_texture(label_path);
        draw_texture(&label_tex, 100.0, y_label, WHITE);
    }
}
