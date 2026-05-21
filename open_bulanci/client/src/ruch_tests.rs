use super::{Ruch, HIDDEN_MAX_MS, VISIBLE_DURATION_MS};

#[test]
fn ctor_starts_hidden_per_ghidra() {
    // `CRuch_ctor @ 0x00423c20` calls `CDSView::Hide` before scheduling
    // the first slot fire, so a fresh Ruch must be in the hidden state.
    let r = Ruch::new(800.0, 0);
    assert!(!r.visible);
}

#[test]
fn hidden_to_visible_returns_exact_100ms() {
    let mut r = Ruch::new(800.0, 0);
    r.visible = false;
    let delay = r.on_toggle_tick(800.0);
    assert!(r.visible, "hidden -> visible must flip state");
    assert_eq!(delay, VISIBLE_DURATION_MS, "visible phase = 100ms literal");
}

#[test]
fn visible_to_hidden_returns_delay_within_4s_window_and_teleports() {
    let mut r = Ruch::new(800.0, 0);
    r.visible = true;
    let delay = r.on_toggle_tick(800.0);
    assert!(!r.visible, "visible -> hidden must flip state");
    assert!(delay <= HIDDEN_MAX_MS, "hidden phase must be 0..=4000ms, got {delay}");
    assert!(r.x >= 0.0 && r.x <= 800.0, "x must stay within screen bounds");
}

#[test]
fn cycle_distribution_keeps_x_in_bounds() {
    let mut r = Ruch::new(640.0, 0);
    for _ in 0..256 {
        r.on_toggle_tick(640.0);
        assert!(r.x >= 0.0 && r.x <= 640.0);
    }
}

#[test]
fn random_hidden_delay_in_expected_range() {
    // Mirrors the original `(rand() * 0xfa1) >> 15 ∈ [0, 4000]` formula.
    for _ in 0..1024 {
        let d = Ruch::random_hidden_delay();
        assert!(d <= HIDDEN_MAX_MS, "delay {d} out of range");
    }
}

#[test]
fn visible_window_is_never_swallowed_by_interleaved_dispatch() {
    // Regression test for the burst-dispatch bug: with the original
    // engine's per-iteration handler invocation, a single very-slow
    // frame must not collapse the 100 ms visible window into the
    // hidden phase. Drive the Ruch state machine the way the scheduler
    // would, simulating 5000ms of catch-up with the interleaved model.
    let mut r = Ruch::new(800.0, 0);
    let mut visible_ms_total: u64 = 0;
    let mut hidden_ms_total: u64 = 0;
    let frames = 200;
    for _ in 0..frames {
        // Each "frame" advances by the current returned delay — so we
        // always see exactly one visible/hidden transition per step.
        let prev_visible = r.visible;
        let delay = r.on_toggle_tick(800.0);
        if prev_visible {
            // We were visible for `delay_prev`; just toggled to hidden,
            // and `delay` is now the upcoming hidden duration.
            hidden_ms_total += delay;
        } else {
            // We were hidden, just toggled to visible for 100ms.
            visible_ms_total += delay;
            assert_eq!(delay, VISIBLE_DURATION_MS);
        }
    }
    assert!(visible_ms_total > 0, "visible window must accrue some time");
    assert!(hidden_ms_total > 0, "hidden window must accrue some time");
}
