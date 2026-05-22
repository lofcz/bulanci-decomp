use super::*;

#[test]
fn hit_test_maps_row_centers_to_their_item_index() {
    assert_eq!(sg1_hit_test(370.0, 115.0, 0, 3, 3, 0), Some(0)); // Center of item 0 (105..125)
    assert_eq!(sg1_hit_test(370.0, 132.0, 0, 3, 3, 0), Some(1)); // Center of item 1 (122..142)
    assert_eq!(sg1_hit_test(370.0, 353.0, 0, 3, 3, 0), Some(8)); // Center of item 8 (343..363)
}

#[test]
fn hit_test_picks_up_the_dale_button() {
    // Button rect = (270..352, 516..541).
    assert_eq!(sg1_hit_test(310.0, 525.0, 0, 3, 3, 0), Some(9));
}

#[test]
fn hit_test_misses_outside_form_columns() {
    // Way left of column (x<280).
    assert_eq!(sg1_hit_test(50.0, 112.0, 0, 3, 3, 0), None);
    // Far right past the columns.
    assert_eq!(sg1_hit_test(700.0, 112.0, 0, 3, 3, 0), None);
    // Between rows (gap between item 5 and 6: 265+15=280 to 308).
    assert_eq!(sg1_hit_test(370.0, 290.0, 0, 3, 3, 0), None);
}
