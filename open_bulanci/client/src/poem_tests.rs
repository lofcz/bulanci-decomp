use super::{parse_poem_line, Align};

#[test]
fn default_line_is_left_aligned() {
    let l = parse_poem_line("Hello world");
    assert_eq!(l.text, "Hello world");
    assert_eq!(l.align, Align::Left);
}

#[test]
fn center_prefix_is_stripped_and_recorded() {
    let l = parse_poem_line("[CENTER]Hello");
    assert_eq!(l.text, "Hello");
    assert_eq!(l.align, Align::Center);
}

#[test]
fn right_prefix_is_stripped_and_recorded() {
    let l = parse_poem_line("[RIGHT]world");
    assert_eq!(l.text, "world");
    assert_eq!(l.align, Align::Right);
}

#[test]
fn surrounding_whitespace_is_trimmed() {
    let l = parse_poem_line("   leading and trailing   ");
    assert_eq!(l.text, "leading and trailing");
}

#[test]
fn empty_line_yields_empty_left_align() {
    let l = parse_poem_line("");
    assert_eq!(l.text, "");
    assert_eq!(l.align, Align::Left);
}

#[test]
fn czech_diacritics_round_trip() {
    let l = parse_poem_line("[CENTER]Bulánci žijí na střechách");
    assert_eq!(l.text, "Bulánci žijí na střechách");
    assert_eq!(l.align, Align::Center);
}
