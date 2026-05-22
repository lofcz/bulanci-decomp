use super::{hit_test_menu, menu_btn_rect};
use crate::state::MenuBtn;

#[test]
fn button_rects_are_stacked_with_no_overlap() {
    let r_start = menu_btn_rect(MenuBtn::Start);
    let r_hist  = menu_btn_rect(MenuBtn::History);
    let r_quit  = menu_btn_rect(MenuBtn::Quit);
    // Vertical stack: Start → History → Quit at strictly increasing Y.
    assert!(r_start.y < r_hist.y);
    assert!(r_hist.y < r_quit.y);
    // No vertical overlap between adjacent buttons.
    assert!(r_start.y + r_start.h <= r_hist.y, "Start row must not overlap History row");
    assert!(r_hist.y + r_hist.h  <= r_quit.y, "History row must not overlap Quit row");
}

#[test]
fn hit_test_returns_correct_button_for_each_dial_center() {
    // Dial sprite is anchored at (35, 37 + 84*row), 64 wide. The center
    // of each dial should map back to its own button.
    assert_eq!(hit_test_menu(35.0 + 32.0, 37.0 + 32.0),  Some(MenuBtn::Start));
    assert_eq!(hit_test_menu(35.0 + 32.0, 121.0 + 32.0), Some(MenuBtn::History));
    assert_eq!(hit_test_menu(35.0 + 32.0, 205.0 + 32.0), Some(MenuBtn::Quit));
}

#[test]
fn hit_test_misses_outside_button_strip() {
    // Far right (sub-screen area) — never hits a button.
    assert_eq!(hit_test_menu(500.0, 100.0), None);
    // Inside the label area (e.g., x=150) — should not hit a button in the original game.
    assert_eq!(hit_test_menu(150.0, 69.0), None);
    assert_eq!(hit_test_menu(150.0, 153.0), None);
    assert_eq!(hit_test_menu(150.0, 237.0), None);
    // Above the first button.
    assert_eq!(hit_test_menu(60.0, 10.0), None);
    // Below the last button.
    assert_eq!(hit_test_menu(60.0, 500.0), None);
    // In the vertical gap between buttons (Start ends at y=102, History
    // starts at y=121, so y=110 is the gap).
    assert_eq!(hit_test_menu(60.0, 110.0), None);
}
