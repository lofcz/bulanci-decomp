use super::*;

#[test]
fn hit_test_maps_row_centers_to_their_item_index() {
    assert_eq!(ClientApp::sg1_hit_test(370.0, 135.0), Some(0));
    assert_eq!(ClientApp::sg1_hit_test(370.0, 160.0), Some(1));
    assert_eq!(ClientApp::sg1_hit_test(370.0, 425.0), Some(8));
}

#[test]
fn hit_test_picks_up_the_dale_button() {
    // Button rect = (350..470, 480..515).
    assert_eq!(ClientApp::sg1_hit_test(400.0, 495.0), Some(9));
}

#[test]
fn hit_test_misses_outside_form_columns() {
    // Way left of column (x<275).
    assert_eq!(ClientApp::sg1_hit_test(50.0, 135.0), None);
    // Far right past the IP entry area.
    assert_eq!(ClientApp::sg1_hit_test(700.0, 135.0), None);
    // Between rows (gap between item 5 and 6).
    assert_eq!(ClientApp::sg1_hit_test(370.0, 340.0), None);
}
