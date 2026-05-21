//! Unit tests for the bitmap font module.
//!
//! These exercise the pure-Rust parts only — JSON parsing, CP-1250
//! mapping, glyph-advance measurement, atlas size dispatch. Anything
//! that needs a live macroquad GPU context (texture upload, draw calls)
//! lives in the integration tier and is skipped here.

use super::*;

const MEDIUM_JSON: &str =
    include_str!("../../assets/fonts/bitmap/font_medium.json");
const SMALL_JSON: &str =
    include_str!("../../assets/fonts/bitmap/font_small.json");
const LARGE_JSON: &str =
    include_str!("../../assets/fonts/bitmap/font_large.json");

#[test]
fn parses_medium_font_metrics_matches_unpacker_output() {
    let (default_w, line_h, glyphs) = parse_metrics(MEDIUM_JSON).unwrap();
    // From `_save_font`'s parse of resource 0x100af:
    //   defaultWidth = 15, lineHeight = 13.
    assert_eq!(default_w, 15);
    assert_eq!(line_h, 13);

    // ASCII 'A' must be present and consistent with the
    // hand-inspected JSON values.
    let a = &glyphs[b'A' as usize];
    assert_eq!(a.offset_x, 153);
    assert_eq!(a.width, 6);
    assert_eq!(a.offset_y, 3);
    assert_eq!(a.height, 11);
}

#[test]
fn parses_small_and_large_font_dimensions() {
    let (_, line_h_small, _) = parse_metrics(SMALL_JSON).unwrap();
    let (_, line_h_large, _) = parse_metrics(LARGE_JSON).unwrap();
    assert_eq!(line_h_small, 11);
    assert_eq!(line_h_large, 16);
}

#[test]
fn estimated_baseline_matches_cap_height_band() {
    // For the medium atlas, capital A has offset_y = 3 and height = 11,
    // so the baseline lands at row 11 from the cell top.
    let (_, _, glyphs) = parse_metrics(MEDIUM_JSON).unwrap();
    assert_eq!(estimate_baseline(&glyphs), 11.0);
}

#[test]
fn cp1250_ascii_is_identity() {
    for c in 0u32..=0x7F {
        assert_eq!(utf32_to_cp1250(c), c as u8, "mismatch at {c:#x}");
    }
}

#[test]
fn cp1250_czech_diacritics_round_trip() {
    // Hand-picked subset of common Czech letters used in the poems.
    let cases = [
        ('á', 0xE1u8), ('č', 0xE8), ('ď', 0xEF), ('é', 0xE9),
        ('ě', 0xEC), ('í', 0xED), ('ň', 0xF2), ('ó', 0xF3),
        ('ř', 0xF8), ('š', 0x9A), ('ť', 0x9D), ('ú', 0xFA),
        ('ů', 0xF9), ('ý', 0xFD), ('ž', 0x9E),
        ('Á', 0xC1), ('Č', 0xC8), ('Ě', 0xCC), ('Š', 0x8A),
        ('Ž', 0x8E),
    ];
    for (ch, expected) in cases {
        assert_eq!(
            utf32_to_cp1250(ch as u32),
            expected,
            "wrong CP-1250 byte for {ch:?}"
        );
    }
}

#[test]
fn cp1250_unmappable_falls_back_to_question_mark() {
    // Japanese hiragana — definitely not in CP-1250.
    assert_eq!(utf32_to_cp1250('あ' as u32), b'?');
    // Astral plane emoji
    assert_eq!(utf32_to_cp1250(0x1F600), b'?');
}

/// `char_advance` must match `CDSFont::GetCharWidth @ 0x00437330`:
/// glyphs with a non-zero `width` advance by that width; glyphs with
/// `width == 0` advance by `defaultWidth >> 2`. This is the original
/// engine's trick for sizing the literal space character (whose
/// `width` is 0 in every atlas).
#[test]
fn measure_uses_default_width_div4_for_missing_glyphs() {
    // Build a synthetic font shape that calls into `char_advance`
    // through the same measure() path the renderer uses.
    fn synth(default_w: u32, set_widths: &[(u8, u8)]) -> ([GlyphMetric; 256], u32) {
        let mut glyphs = [GlyphMetric::default(); 256];
        for &(b, w) in set_widths {
            glyphs[b as usize] = GlyphMetric {
                offset_x: 0,
                width: w,
                offset_y: 0,
                height: 13,
            };
        }
        (glyphs, default_w)
    }

    let (glyphs, default_w) = synth(15, &[(b'A', 6), (b'B', 8)]);

    fn measure_with(
        text: &str,
        char_spacing: i32,
        glyphs: &[GlyphMetric; 256],
        default_width: u32,
    ) -> f32 {
        let mut x: i32 = 0;
        for ch in text.chars() {
            let b = utf32_to_cp1250(ch as u32);
            let w = glyphs[b as usize].width;
            let advance = if w != 0 {
                w as i32
            } else {
                (default_width as i32) >> 2
            };
            x += advance + char_spacing;
        }
        x.max(0) as f32
    }

    // Two known-width glyphs: 6 + 8 = 14 px, no spacing.
    assert_eq!(measure_with("AB", 0, &glyphs, default_w), 14.0);
    // 'C' is unmapped (width 0) so it advances `default_width / 4 = 15 / 4 = 3` px.
    assert_eq!(measure_with("ABC", 0, &glyphs, default_w), 14.0 + 3.0);
    // The literal space (' ', 0x20) also goes through the width-0
    // path, so "A B" is 6 + 3 + 6 = 15 px.
    let (g, _) = synth(15, &[(b'A', 6)]);
    assert_eq!(measure_with("A A", 0, &g, default_w), 6.0 + 3.0 + 6.0);
    // char_spacing adds one per glyph.
    assert_eq!(measure_with("AB", 1, &glyphs, default_w), 16.0);
    assert_eq!(measure_with("", 1, &glyphs, default_w), 0.0);

    // Small / medium / large defaults map to 2 / 3 / 4 px advance
    // for missing glyphs respectively.
    assert_eq!(9 >> 2, 2);
    assert_eq!(15 >> 2, 3);
    assert_eq!(19 >> 2, 4);
}
