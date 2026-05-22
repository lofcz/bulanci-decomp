//! `CStartGame1` sub-screen — host/join + player-count radios + "Dále"
//! continue button. Per §3.1 of main_menu.md the form has 9 selectable
//! rows + a continue button.

use macroquad::prelude::*;

use crate::app::ClientApp;
use crate::render::{TEXT_COLOR_PRIMARY, TEXT_COLOR_STATIC};
use crate::state::SubScreen;
use crate::widget::RadioRowWidget;

/// Per `CStartGame1::BuildUi @ 0x0040c650`, every header static text is
/// built via `CStaticText::BuildAtAuto(this, x, y, smartptr, 2, 0x100ae)`.
/// `param_4 = 2` lands at `this+0x70`, which is `font_info[+4]` —
/// i.e. `char_spacing = 2` between every pair of glyphs. The radios
/// underneath use the same value (`CRadio::BuildAt` sets `this+0x84 = 2`),
/// so the headers and the rows beneath them share the same letter rhythm.
const STATIC_TEXT_CHAR_SPACING: i32 = 2;

/// Per `CButton::BuildAt @ 0x004087f0`, every CButton's font-info block
/// has `+0x7c = 1`. The dialog buttons (incl. "Dále") rendered through
/// this widget all get the same 1-px inter-glyph gap.
const BUTTON_CHAR_SPACING: i32 = 1;

/// The original game coordinates for the right pane window
const WINDOW_X: f32 = 250.0;
const WINDOW_Y: f32 = 60.0;

/// Mathematically precise, content-driven layout parameters reverse-engineered from the original retail binary.
const CONTENT_ROOT_Y: f32 = 20.0;
const HEADER_TO_OPTION_SPACING: f32 = 25.0;
const OPTION_TO_OPTION_SPACING: f32 = 17.0;
const GROUP_TO_HEADER_SPACING: f32 = 43.0;

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum Sg1Item {
    ServerMode(usize), // 0 = Vytvořit, 1 = Připojit se
    TotalPlayers(usize), // 0..=3 (1..=4 players)
    LocalPlayers(usize), // 0..=2 (1..=3 players)
    ConnectionType(usize), // 0 = TCP/IP, 1 = IPX
    ContinueButton,
}

#[derive(Clone, Debug)]
pub struct SelectableItem {
    pub kind: Sg1Item,
    pub label: &'static str,
    pub y: f32, // Relative Y to WINDOW_Y (top edge of checkbox/row)
    pub checked: bool,
    pub disabled: bool,
}

fn sg1_group_for_item(kind: Sg1Item) -> Option<usize> {
    match kind {
        Sg1Item::ServerMode(_) => Some(0),
        Sg1Item::TotalPlayers(_) => Some(1),
        Sg1Item::LocalPlayers(_) => Some(2),
        Sg1Item::ConnectionType(_) => Some(3),
        Sg1Item::ContinueButton => None,
    }
}

fn sg1_step_in_focused_group(
    items: &[SelectableItem],
    active_item: usize,
    focused_group: usize,
    direction: isize,
) -> usize {
    let group_indices: Vec<usize> = items
        .iter()
        .enumerate()
        .filter_map(|(i, item)| {
            (!item.disabled && sg1_group_for_item(item.kind) == Some(focused_group)).then_some(i)
        })
        .collect();

    if group_indices.is_empty() {
        return active_item;
    }

    let current_pos = group_indices
        .iter()
        .position(|&i| i == active_item)
        .unwrap_or(0);
    let len = group_indices.len() as isize;
    let next_pos = (current_pos as isize + direction).rem_euclid(len) as usize;
    group_indices[next_pos]
}

/// Dynamically calculates the positions and active states of all STARTGAME1 menu groups
/// based on the original game's vertical chain collapsing layout (`CDSChained`).
/// Returns a tuple of (selectable_items, headers).
pub fn get_sg1_layout(
    server_mode: usize,
    total_players: usize,
    local_players: usize,
    connection_type: usize,
) -> (Vec<SelectableItem>, Vec<(&'static str, f32)>) {
    let is_host = server_mode == 0;
    
    // Group visibility rules matching the original game's C++ code:
    // 1. Group 1 (Hru: Vytvořit/Připojit se) is always visible.
    // 2. Group 2 (Počet hráčů celkem: 1..4 players) is visible only in host (Vytvořit) mode.
    let group2_visible = is_host;
    
    // 3. Group 3 (Na tomto počítači: 1..3 players) is visible unless hosting a 1-player training game.
    let group3_visible = !(is_host && total_players == 1);
    
    // 4. Group 4 (Způsob připojení: TCP-IP/IPX) is visible in join (Připojit se) mode,
    //    OR when hosting (total_players > 1) and some players are remote (local_players < total_players).
    let group4_visible = !is_host || (total_players > 1 && local_players < total_players);

    let mut items = Vec::new();
    let mut headers = Vec::new();
    let mut current_y = CONTENT_ROOT_Y;

    // Group 1 (Hru:)
    headers.push(("Hru:", current_y));
    current_y += HEADER_TO_OPTION_SPACING;

    items.push(SelectableItem {
        kind: Sg1Item::ServerMode(0),
        label: "Vytvořit",
        y: current_y,
        checked: server_mode == 0,
        disabled: false,
    });
    current_y += OPTION_TO_OPTION_SPACING;

    items.push(SelectableItem {
        kind: Sg1Item::ServerMode(1),
        label: "Připojit se",
        y: current_y,
        checked: server_mode == 1,
        disabled: false,
    });

    // Group 2 (Počet hráčů celkem:)
    if group2_visible {
        current_y += GROUP_TO_HEADER_SPACING;
        headers.push(("Počet hráčů celkem:", current_y));
        current_y += HEADER_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::TotalPlayers(0),
            label: "1 hráč (trénink)",
            y: current_y,
            checked: total_players == 1,
            disabled: false,
        });
        current_y += OPTION_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::TotalPlayers(1),
            label: "2 hráči",
            y: current_y,
            checked: total_players == 2,
            disabled: false,
        });
        current_y += OPTION_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::TotalPlayers(2),
            label: "3 hráči",
            y: current_y,
            checked: total_players == 3,
            disabled: false,
        });
        current_y += OPTION_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::TotalPlayers(3),
            label: "4 hráči",
            y: current_y,
            checked: total_players == 4,
            disabled: false,
        });
    }

    // Group 3 (Na tomto počítači:)
    if group3_visible {
        current_y += GROUP_TO_HEADER_SPACING;
        headers.push(("Na tomto počítači:", current_y));
        current_y += HEADER_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::LocalPlayers(0),
            label: "1 hráč",
            y: current_y,
            checked: local_players == 1,
            disabled: false,
        });
        current_y += OPTION_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::LocalPlayers(1),
            label: "2 hráči",
            y: current_y,
            checked: local_players == 2,
            disabled: is_host && total_players < 2,
        });
        current_y += OPTION_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::LocalPlayers(2),
            label: "3 hráči",
            y: current_y,
            checked: local_players == 3,
            disabled: is_host && total_players < 3,
        });
    }

    // Group 4 (Způsob připojení:)
    if group4_visible {
        current_y += GROUP_TO_HEADER_SPACING;
        headers.push(("Způsob připojení:", current_y));
        current_y += HEADER_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::ConnectionType(0),
            label: "TCP/IP protokol",
            y: current_y,
            checked: connection_type == 0,
            disabled: false,
        });
        current_y += OPTION_TO_OPTION_SPACING;

        items.push(SelectableItem {
            kind: Sg1Item::ConnectionType(1),
            label: "IPX protokol",
            y: current_y,
            checked: connection_type == 1,
            disabled: false,
        });
    }

    // Continue button is always placed at the bottom, at relative position 456.0
    items.push(SelectableItem {
        kind: Sg1Item::ContinueButton,
        label: "Dále",
        y: 456.0,
        checked: false,
        disabled: false,
    });

    (items, headers)
}

/// Hit test function to see which menu row or continue button is hovered.
pub fn sg1_hit_test(
    mx: f32,
    my: f32,
    server_mode: usize,
    total_players: usize,
    local_players: usize,
    connection_type: usize,
) -> Option<usize> {
    let (items, _) = get_sg1_layout(server_mode, total_players, local_players, connection_type);
    for (i, item) in items.iter().enumerate() {
        if item.kind == Sg1Item::ContinueButton {
            let btn_abs_x = WINDOW_X + 20.0;
            let btn_abs_y = WINDOW_Y + item.y;
            let btn_rect = Rect::new(btn_abs_x, btn_abs_y, 82.0, 25.0);
            if btn_rect.contains(vec2(mx, my)) {
                return Some(i);
            }
        } else {
            // Hit box of checkbox + label: starts at X = 280.0 (where checkbox is drawn)
            // and extends to the right (about 185px width). Y span is row_y to row_y + 20.0.
            let row_abs_x = WINDOW_X + 30.0;
            let row_abs_y = WINDOW_Y + item.y;
            if (row_abs_x..=row_abs_x + 185.0).contains(&mx)
                && (row_abs_y..=row_abs_y + 20.0).contains(&my)
            {
                return Some(i);
            }
        }
    }
    None
}

impl ClientApp {
    /// Ensures that focus remains on a selectable (enabled) visible item.
    fn ensure_valid_focus(&mut self) {
        let (items, _) = get_sg1_layout(
            self.sg1_server_mode,
            self.sg1_total_players,
            self.sg1_local_players,
            self.sg1_connection_type,
        );

        if self.sg1_active_item >= items.len() || items[self.sg1_active_item].disabled {
            // Find first non-disabled item
            for i in 0..items.len() {
                if !items[i].disabled {
                    self.sg1_active_item = i;
                    break;
                }
            }
        }

        if !items
            .iter()
            .any(|item| sg1_group_for_item(item.kind) == Some(self.sg1_focused_group))
        {
            if let Some(group) = items.iter().find_map(|item| sg1_group_for_item(item.kind)) {
                self.sg1_focused_group = group;
            }
        }
    }

    pub fn update_startgame1_input(&mut self, mx: f32, my: f32, click: bool) {
        let (visible_items, _) = get_sg1_layout(
            self.sg1_server_mode,
            self.sg1_total_players,
            self.sg1_local_players,
            self.sg1_connection_type,
        );

        // Mouse movement updates the hovered cell only for the focused radio group.
        // Retail still hit-tests other radios, but their CRadio lacks the focus bit
        // (`this+0x44 & 2`), so hover/pressed row states are not visually active.
        let hovered = sg1_hit_test(
            mx,
            my,
            self.sg1_server_mode,
            self.sg1_total_players,
            self.sg1_local_players,
            self.sg1_connection_type,
        );
        if is_mouse_button_pressed(MouseButton::Left) {
            if let Some(i) = hovered {
                if !visible_items[i].disabled {
                    if let Some(group) = sg1_group_for_item(visible_items[i].kind) {
                        self.sg1_focused_group = group;
                        self.sg1_active_item = i;
                        self.play_sfx("audio/sfx_radio_click.wav");
                    } else {
                        self.sg1_active_item = i;
                    }
                }
            }
        }

        // Keyboard navigation.
        if is_key_pressed(KeyCode::Down) {
            self.sg1_active_item = sg1_step_in_focused_group(
                &visible_items,
                self.sg1_active_item,
                self.sg1_focused_group,
                1,
            );
        }
        if is_key_pressed(KeyCode::Up) {
            self.sg1_active_item = sg1_step_in_focused_group(
                &visible_items,
                self.sg1_active_item,
                self.sg1_focused_group,
                -1,
            );
        }

        if is_key_pressed(KeyCode::Escape) {
            self.sub_screen = SubScreen::None;
            self.pressed_btn = None;
            return;
        }

        if click {
            if let Some(i) = hovered {
                if !visible_items[i].disabled {
                    let kind = visible_items[i].kind;
                    if let Some(group) = sg1_group_for_item(kind) {
                        self.sg1_focused_group = group;
                    }
                    self.sg1_active_item = i;
                    self.activate_sg1_item_kind(kind);
                }
            }
        } else if is_key_pressed(KeyCode::Enter) {
            if self.sg1_active_item < visible_items.len()
                && !visible_items[self.sg1_active_item].disabled
            {
                let kind = visible_items[self.sg1_active_item].kind;
                if let Some(group) = sg1_group_for_item(kind) {
                    self.sg1_focused_group = group;
                }
                self.activate_sg1_item_kind(kind);
            }
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

        self.ensure_valid_focus();
    }

    /// Handles activation of a specific Sg1Item kind.
    fn activate_sg1_item_kind(&mut self, kind: Sg1Item) {
        match kind {
            Sg1Item::ServerMode(0) => {
                self.sg1_server_mode = 0;
                self.sg1_local_players = self.sg1_local_players.min(self.sg1_total_players);
            }
            Sg1Item::ServerMode(1) => {
                self.sg1_server_mode = 1;
            }
            Sg1Item::ServerMode(_) => {}
            Sg1Item::TotalPlayers(idx) => {
                match idx {
                    0 => {
                        self.sg1_total_players = 1;
                        self.sg1_local_players = 1;
                        self.sg1_server_mode = 0;
                    }
                    1 => {
                        self.sg1_total_players = 2;
                        self.sg1_local_players = self.sg1_local_players.min(2);
                    }
                    2 => {
                        self.sg1_total_players = 3;
                        self.sg1_local_players = self.sg1_local_players.min(3);
                    }
                    3 => {
                        self.sg1_total_players = 4;
                    }
                    _ => {}
                }
            }
            Sg1Item::LocalPlayers(idx) => {
                self.sg1_local_players = idx + 1;
            }
            Sg1Item::ConnectionType(idx) => {
                self.sg1_connection_type = idx;
            }
            Sg1Item::ContinueButton => {
                self.kick_off_match();
            }
        }
    }

    /// Translate the StartGame1 form state into a server-address dial and
    /// fire `try_connect`.
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

        // Header drawing. Position calculated dynamically matching the active collapsing groups:
        let (visible_items, headers) = get_sg1_layout(
            self.sg1_server_mode,
            self.sg1_total_players,
            self.sg1_local_players,
            self.sg1_connection_type,
        );
        let (mx, my) = crate::window_mode::logical_mouse_position();
        let hovered_item = sg1_hit_test(
            mx,
            my,
            self.sg1_server_mode,
            self.sg1_total_players,
            self.sg1_local_players,
            self.sg1_connection_type,
        );

        let header_color = TEXT_COLOR_STATIC;

        // Draw headers at absolute X = WINDOW_X + 20 = 270.0
        for (text, rel_y) in headers {
            let abs_y = WINDOW_Y + rel_y + 13.0; // Offset for text baseline alignment
            self.draw_t_sp(text, WINDOW_X + 20.0, abs_y, 16, STATIC_TEXT_CHAR_SPACING, header_color);
        }

        let is_left_button_down = is_mouse_button_down(MouseButton::Left);
        for (i, item) in visible_items.iter().enumerate() {
            if item.kind == Sg1Item::ContinueButton {
                // ---- "Dále" continue button. ----
                let is_hovered = hovered_item == Some(i);
                let is_pressed = is_hovered && is_left_button_down;

                let tex_name = if is_pressed {
                    "images/btn_dialog_pressed.png"
                } else if is_hovered {
                    "images/btn_dialog_hover.png"
                } else {
                    "images/btn_dialog_normal.png"
                };

                let btn_tex = self.get_texture(tex_name);
                let btn_abs_x = WINDOW_X + 20.0;
                let btn_abs_y = WINDOW_Y + item.y;

                draw_texture(&btn_tex, btn_abs_x, btn_abs_y, WHITE);

                let dw = self.measure_t_sp("Dále", 15, BUTTON_CHAR_SPACING);
                self.draw_t_sp(
                    "Dále",
                    btn_abs_x + (82.0 - dw) * 0.5,
                    btn_abs_y + 17.0,
                    15,
                    BUTTON_CHAR_SPACING,
                    TEXT_COLOR_PRIMARY,
                );
            } else {
                let group_focused = sg1_group_for_item(item.kind) == Some(self.sg1_focused_group);
                let hovered = group_focused && hovered_item == Some(i);
                let pressed = hovered && is_left_button_down;
                
                let abs_y = WINDOW_Y + item.y;
                RadioRowWidget::draw(
                    self,
                    item.label,
                    abs_y,
                    item.checked,
                    item.disabled,
                    group_focused,
                    hovered,
                    pressed,
                );
            }
        }

        // ---- Optional IP input box (only when Join is active). ----
        if self.sg1_total_players > 1 && self.sg1_server_mode == 1 {
            self.draw_t("IP hostitele:", 445.0, 350.0, 14, Color::new(1.0, 0.85, 0.2, 1.0));
            draw_rectangle(445.0, 360.0, 180.0, 25.0, Color::new(0.10, 0.10, 0.10, 0.85));
            draw_rectangle_lines(445.0, 360.0, 180.0, 25.0, 1.0, WHITE);
            self.draw_t(&self.ip_input_buffer, 452.0, 378.0, 14, Color::new(0.6, 1.0, 0.6, 1.0));
        }
    }
}

#[cfg(test)]
#[path = "startgame1_tests.rs"]
mod tests;
