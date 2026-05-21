//! Unit tests for `GunMouse`. Each test cites the section of
//! `ghidra_analysis/gameplay/red_menu_cursor.md` it is validating so a
//! future RE pass can find the byte-level source if anything diverges.

use super::{GunMouse, COORD_QUEUE_CAP, DOT_BASE_SEED_X, DOT_BASE_SEED_Y};
use crate::atlas::Atlas;
use crate::cursor_atlas::parse_cursor_set_for_tests;

// ---------------------------------------------------------------------------
// Shared test fixture: the same three motion paths shipped in
// `assets/cursor/cursor.json` and `assets/atlases/`, parsed once per test
// through the real loader. Using the on-disk assets as the source of truth
// means a regression in the asset pipeline (e.g. a hand-edited PNG
// that desyncs from the JSON) lights up here, not silently in the
// running app.
// ---------------------------------------------------------------------------

fn shipped_atlases() -> Vec<Atlas> {
    let composition = std::fs::read("../assets/cursor/cursor.json")
        .expect("run scripts/build_cursor_atlas.py first (missing cursor.json)");
    let composition_parsed: serde_json::Value = serde_json::from_slice(&composition)
        .expect("cursor.json parsing failed");

    let blobs: Vec<(String, Vec<u8>)> = composition_parsed["idleTracks"]
        .as_array()
        .expect("idleTracks missing or not an array")
        .iter()
        .map(|t| {
            let name = t["atlas"].as_str().expect("atlas name is not a string").to_owned();
            let path = format!("../assets/atlases/{}/atlas.json", name);
            let bytes = std::fs::read(&path)
                .unwrap_or_else(|_| panic!("missing atlas json: {path}"));
            (name, bytes)
        })
        .collect();
    let blob_refs: Vec<(&str, &[u8])> =
        blobs.iter().map(|(n, b)| (n.as_str(), b.as_slice())).collect();
    parse_cursor_set_for_tests(&composition, &blob_refs)
        .expect("failed to parse cursor set for tests")
        .into_atlases()
}

// ===========================================================================
// §5.C — Activate seeds the dot to (2, 3) and zeroes everything else.
// ===========================================================================

#[test]
fn activate_seeds_dot_offsets_to_2_3_and_clears_state() {
    let g = GunMouse::new();
    assert_eq!(g.dot_base_x, DOT_BASE_SEED_X);
    assert_eq!(g.dot_base_y, DOT_BASE_SEED_Y);
    assert_eq!(g.dot_offset_x, DOT_BASE_SEED_X);
    assert_eq!(g.dot_offset_y, DOT_BASE_SEED_Y);
    assert!(!g.dot_ready);
    assert_eq!(g.target_x, 400.0);
    assert_eq!(g.target_y, 300.0);
    assert_eq!(g.sprite_frame, 0);
}

// ===========================================================================
// §4 — 30-tick ring buffer + 150 ms (= 30 × SLOT_CURSOR_TICK) lag.
//
// `on_cursor_tick()` is the scheduler-driven push+pop beat (see
// state.rs `SLOT_CURSOR_TICK` for the 5 ms cadence rationale). Each
// call enqueues the current `mouse_x/y` once and pops the oldest
// entry into `target_x/y` once the ring is exactly full. The tests
// don't need a synthetic clock because the scheduler upstream
// guarantees one call per 5 ms of engine time.
// ===========================================================================

#[test]
fn target_lags_30_ticks_behind_cursor_after_queue_saturates() {
    let mut g = GunMouse::new();
    for i in 0..COORD_QUEUE_CAP {
        g.mouse_x = (i * 10) as f32;
        g.mouse_y = 0.0;
        g.on_cursor_tick();
    }
    // 30th tick fills the ring and pops the first entry (0, 0) into
    // target_*.
    assert_eq!(g.target_x, 0.0);
    assert_eq!(g.target_y, 0.0);
    assert!(g.dot_ready);

    g.mouse_x = 300.0;
    g.on_cursor_tick();
    // The next-oldest entry was the second push (mouse_x = 10) —
    // verifies the queue is a strict FIFO, not a moving average.
    assert_eq!(g.target_x, 10.0);
    assert_eq!(g.target_y, 0.0);
}

#[test]
fn dot_is_not_ready_until_queue_saturates() {
    let mut g = GunMouse::new();
    for _ in 0..COORD_QUEUE_CAP - 1 {
        g.on_cursor_tick();
        assert!(!g.dot_ready, "dot went ready too early");
    }
    g.on_cursor_tick();
    assert!(g.dot_ready, "dot did not go ready at the 30th push");
}

#[test]
fn on_cursor_tick_is_exactly_one_push_per_call() {
    // The scheduler at SLOT_CURSOR_TICK invokes `on_cursor_tick` once per
    // logical 5 ms beat (catch-up dispatches make this true even across
    // long stalls). Each call must produce exactly one queue entry —
    // no internal gates, no while-loops.
    let mut g = GunMouse::new();
    g.on_cursor_tick();
    assert_eq!(g.coord_queue_len_for_tests(), 1);
    g.on_cursor_tick();
    assert_eq!(g.coord_queue_len_for_tests(), 2);
    g.on_cursor_tick();
    assert_eq!(g.coord_queue_len_for_tests(), 3);
}

#[test]
fn coord_queue_caps_at_thirty_and_pop_runs_only_when_full() {
    // Pushes silently drop once the ring is at capacity (FUN_00424370
    // returns 0). Pop must only fire when size == 30, otherwise we'd
    // pop spurious stale entries and target_* would track noise.
    let mut g = GunMouse::new();
    for _ in 0..(COORD_QUEUE_CAP + 50) {
        g.on_cursor_tick();
    }
    assert_eq!(g.coord_queue_len_for_tests(), COORD_QUEUE_CAP - 1,
        "steady-state size oscillates between cap and cap-1 (this snapshot \
         is post-pop, so it lands at cap-1)");
    assert!(g.dot_ready, "dot should have gone ready at the 30th tick");
}

// ===========================================================================
// §5.B — OnMouseMove two-branch rule.
// ===========================================================================

#[test]
fn first_mouse_move_in_frame_reseeds_base_from_offset() {
    // dot_needs_update = false at start of frame.
    // First move: dot_base = dot_offset + Δ; dot_needs_update = true.
    let mut g = GunMouse::new();
    g.dot_offset_x = 5.0;
    g.dot_offset_y = 7.0;
    g.dot_base_x = 999.0; // poisoned — must be overwritten on first event
    g.dot_base_y = 999.0;
    g.on_mouse_move(3.0, -2.0);
    assert_eq!(g.dot_base_x, 5.0 + 3.0);
    assert_eq!(g.dot_base_y, 7.0 - 2.0);
}

#[test]
fn subsequent_mouse_moves_in_same_frame_accumulate() {
    let mut g = GunMouse::new();
    g.dot_offset_x = 0.0;
    g.dot_offset_y = 0.0;
    g.dot_base_x = 0.0;
    g.dot_base_y = 0.0;
    g.on_mouse_move(1.0, 1.0); // first event: reseed -> (1, 1)
    g.on_mouse_move(2.0, 3.0); // continued: += (2, 3) -> (3, 4)
    g.on_mouse_move(-1.0, -1.0); // continued: += (-1, -1) -> (2, 3)
    assert_eq!(g.dot_base_x, 2.0);
    assert_eq!(g.dot_base_y, 3.0);
}

// ===========================================================================
// §2.A — Erase promotes base into offset; dot_base is NOT reset here.
// ===========================================================================

#[test]
fn erase_promotes_dot_base_into_dot_offset_without_resetting_base() {
    let mut g = GunMouse::new();
    g.dot_base_x = 17.0;
    g.dot_base_y = -4.0;
    g.dot_offset_x = 0.0;
    g.dot_offset_y = 0.0;
    g.erase_for_tests();
    assert_eq!(g.dot_offset_x, 17.0);
    assert_eq!(g.dot_offset_y, -4.0);
    // dot_base must NOT have changed.
    assert_eq!(g.dot_base_x, 17.0);
    assert_eq!(g.dot_base_y, -4.0);
}

// ===========================================================================
// §6 — Track-roll stays in {0, 1, 2}, sprite frame wraps at the *active*
// track's length and at end-of-loop the dot offset is back at where it
// started (closed-loop invariant — see cursor_atlas.rs).
// ===========================================================================

#[test]
fn drift_track_stays_in_three_track_range() {
    let mut g = GunMouse::new();
    let tracks = shipped_atlases();
    for _ in 0..1024 {
        g.on_drift_track_tick(tracks.len());
        assert!((g.drift_track as usize) < tracks.len());
        assert_eq!(g.sprite_frame, 0, "drift track reroll did not reset frame");
    }
}

/// Byte-faithful reproduction of `CGunMouse_OnAnimTick @ 0x00423bd0`:
/// with the MSVCRT-default seed of 1, the first eight track-roll
/// outcomes must be exactly `[0, 1, 0, 2, 1, 1, 1, 2]` (verified by
/// running the LCG `seed*214013+2531011 ; (s>>16)&0x7fff` and
/// applying the engine's `(rand*48)>>19` selector). This is the
/// kind of golden test that catches anyone "simplifying" the RNG or
/// the formula in a way that silently desyncs from the original.
#[test]
fn drift_track_matches_msvcrt_seed1_sequence() {
    let mut g = GunMouse::new();
    let expected = [0u8, 1, 0, 2, 1, 1, 1, 2];
    for (i, want) in expected.iter().enumerate() {
        g.on_drift_track_tick(3);
        assert_eq!(
            g.drift_track, *want,
            "roll #{i} mismatch: got {} want {}",
            g.drift_track, *want
        );
    }
}

/// Sanity bound on the long-run distribution: across 30 000 rolls
/// every track must be picked within ±1.5 % of 1/3. Catches a
/// regression to `rand() % N` on a non-32767 RAND_MAX (which would
/// re-introduce modulo bias) or a stuck RNG.
#[test]
fn drift_track_long_run_is_near_uniform() {
    let mut g = GunMouse::new();
    let mut counts = [0u32; 3];
    for _ in 0..30_000 {
        g.on_drift_track_tick(3);
        counts[g.drift_track as usize] += 1;
    }
    for (i, c) in counts.iter().enumerate() {
        let p = *c as f32 / 30_000.0;
        assert!(
            (p - 1.0 / 3.0).abs() < 0.015,
            "track {} p={p} too far from 1/3 (count = {c})",
            i,
        );
    }
}

/// Playing one full track must apply every per-frame Δ to `dot_base`
/// and land us back at the original offset (closed-loop invariant).
/// We exercise this for all three master-pack tracks loaded from
/// the shipped manifest — so this test also validates that the
/// on-disk JSON is consistent with the runtime semantics.
#[test]
fn full_track_loop_returns_dot_base_to_starting_offset() {
    let tracks = shipped_atlases();
    for (idx, track) in tracks.iter().enumerate() {
        let mut g = GunMouse::new();
        // Pin the active track deterministically; can't rely on the
        // `on_drift_track_tick` RNG here.
        g.drift_track = idx as u8;
        g.sprite_frame = 0;
        let start_base = (g.dot_base_x, g.dot_base_y);

        // One tick per delta. The last tick of the loop wraps and
        // re-rolls via `on_drift_track_tick` — so we lock the field
        // back to `idx` after each call so the loop continues on
        // the same motion path.
        for _ in 0..track.frame_count() {
            g.on_sprite_frame_tick(&tracks);
            g.drift_track = idx as u8;
        }

        // Promote the accumulator the same way the real per-frame
        // `Erase` would. With a closed loop, dot_base now equals
        // dot_offset (no net drift) and matches the seed.
        g.erase_for_tests();
        let sprite_id = track.meta.get("engineSpriteId").and_then(|v| v.as_i64()).unwrap_or(0);
        assert_eq!(
            (g.dot_base_x, g.dot_base_y),
            start_base,
            "track {} (sprite {}) did not close to (0, 0)",
            idx,
            sprite_id
        );
    }
}

/// Per-tick `(Δx, Δy)` deltas are applied via the same `OnMouseMove`
/// accumulator the engine observer uses. The dot_base accumulator
/// must equal `dot_offset_seed + Σ deltas[0..k]` after k ticks
/// *before* any `Erase` runs.
#[test]
fn sprite_frame_tick_applies_track_deltas_to_dot_base() {
    let tracks = shipped_atlases();
    let mut g = GunMouse::new();
    g.drift_track = 0; // sprite 65803 (shortest loop)
    g.sprite_frame = 0;
    let track = &tracks[0];
    let (mut ex, mut ey) = (DOT_BASE_SEED_X, DOT_BASE_SEED_Y);
    // Walk just shy of a full loop so the re-roll doesn't fire.
    for i in 0..track.frame_count() - 1 {
        let (dx, dy) = crate::cursor_atlas::frame_motion(&track.frames[i]);
        ex += dx as f32;
        ey += dy as f32;
        g.on_sprite_frame_tick(&tracks);
        assert_eq!(
            (g.dot_base_x, g.dot_base_y),
            (ex, ey),
            "dot_base diverged after delta #{i} of track 0",
        );
    }
}

/// The sprite-frame index that drives the on-screen blit must move
/// in lock-step with the deltas, so the visual and the motion never
/// diverge. After k ticks the sprite frame should be k (modulo
/// the track length), regardless of which track is active.
#[test]
fn sprite_frame_index_advances_in_lock_step_with_deltas() {
    let tracks = shipped_atlases();
    for (idx, track) in tracks.iter().enumerate() {
        let mut g = GunMouse::new();
        g.drift_track = idx as u8;
        g.sprite_frame = 0;
        let sprite_id = track.meta.get("engineSpriteId").and_then(|v| v.as_i64()).unwrap_or(0);
        for expected in 1..track.frame_count() {
            g.on_sprite_frame_tick(&tracks);
            assert_eq!(
                g.sprite_frame as usize, expected,
                "track {} (sprite {}): sprite_frame at tick #{} = {}, expected {}",
                idx, sprite_id, expected, g.sprite_frame, expected
            );
            // Defeat the re-roll only on the very last tick of the loop.
            g.drift_track = idx as u8;
        }
    }
}

// ===========================================================================
// Full per-frame loop — `update_frame` runs Erase + coalesced OnMouseMove.
// ===========================================================================

#[test]
fn update_frame_promotes_base_then_re_accumulates_against_stale_offset() {
    // Simulate three frames worth of `update_frame` calls. We can't
    // hit macroquad's `mouse_position` from a unit test, so we drive
    // the per-frame engine math directly through `erase_for_tests` +
    // `on_mouse_move`.
    let mut g = GunMouse::new();
    // Frame 0 — start from the activate seed.
    assert_eq!(g.dot_offset_x, DOT_BASE_SEED_X);
    assert_eq!(g.dot_offset_y, DOT_BASE_SEED_Y);
    assert_eq!(g.dot_base_x, DOT_BASE_SEED_X);
    assert_eq!(g.dot_base_y, DOT_BASE_SEED_Y);

    // Frame 1: erase then move by (+10, 0).
    g.erase_for_tests();
    g.on_mouse_move(10.0, 0.0);
    assert_eq!(g.dot_offset_x, DOT_BASE_SEED_X);
    assert_eq!(g.dot_base_x, DOT_BASE_SEED_X + 10.0);

    // Frame 2: erase promotes base→offset; move by (+5, 0).
    g.erase_for_tests();
    assert_eq!(g.dot_offset_x, DOT_BASE_SEED_X + 10.0);
    g.on_mouse_move(5.0, 0.0);
    assert_eq!(g.dot_base_x, DOT_BASE_SEED_X + 15.0);

    // Frame 3: erase; no motion.
    g.erase_for_tests();
    assert_eq!(g.dot_offset_x, DOT_BASE_SEED_X + 15.0);
    assert_eq!(g.dot_base_x, DOT_BASE_SEED_X + 15.0);
    // dot_needs_update was cleared by erase, so a re-seed on next move
    // would set base = offset + Δ (a no-op if Δ == 0).
}

// Helpers: tests need direct access to internals to exercise the
// state-machine transitions without going through macroquad input.
impl GunMouse {
    pub(super) fn erase_for_tests(&mut self) {
        self.erase();
    }
    pub(super) fn coord_queue_len_for_tests(&self) -> usize {
        self.coord_queue.len()
    }
}
