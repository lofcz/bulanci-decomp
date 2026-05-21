use super::*;

#[test]
fn recurring_slot_fires_exactly_once_at_first_due() {
    let mut sched = Scheduler::new();
    sched.register(1, 10, /*recurring*/ true, /*now_ms*/ 0);

    // Just before due.
    assert!(sched.dispatch_due_events(9).is_empty());
    // Exactly at due.
    let f = sched.dispatch_due_events(10);
    assert_eq!(f, vec![1]);
    // Same tick — nothing more.
    assert!(sched.dispatch_due_events(10).is_empty());
}

#[test]
fn single_shot_slot_fires_once_then_deactivates() {
    let mut sched = Scheduler::new();
    sched.register(7, 50, /*recurring*/ false, 0);
    let f = sched.dispatch_due_events(100);
    assert_eq!(f, vec![7]);
    // Subsequent dispatches must not refire.
    assert!(sched.dispatch_due_events(1_000).is_empty());
    assert!(!sched.is_active(7));
}

#[test]
fn catch_up_fires_multiple_times_in_one_dispatch_when_behind() {
    // The headline test for tick_system.md §"Framerate Independence vs.
    // Slowdown Behavior". A 10ms-period slot whose owner stuttered for
    // 53ms must fire **five times** (catching up the missed 10ms
    // intervals) in one dispatch.
    let mut sched = Scheduler::new();
    sched.register(42, 10, true, 0);
    let fired = sched.dispatch_due_events(53);
    assert_eq!(fired, vec![42, 42, 42, 42, 42], "expected 5 catch-up fires for 53ms drift @ 10ms cadence");

    // After catch-up the slot's last_fire should be at 50 (not 53!) —
    // proving drift-free accumulation. Next fire is due at 60.
    assert_eq!(sched.last_fire_ms(42), 50);
    assert!(sched.dispatch_due_events(59).is_empty());
    assert_eq!(sched.dispatch_due_events(60), vec![42]);
}

#[test]
fn drift_free_property_across_uneven_dispatch_calls() {
    // The slot should fire exactly N times after N*delay_ms wall-clock
    // regardless of how the calls are sliced — this is the key
    // anti-drift invariant of accumulative scheduling.
    let mut sched = Scheduler::new();
    sched.register(5, 100, true, 0);
    let mut fire_count = 0;
    for ms in [30, 110, 305, 499, 600, 999, 1_000] {
        fire_count += sched.dispatch_due_events(ms).len();
    }
    // 1000ms / 100ms = 10 fires total.
    assert_eq!(fire_count, 10);
    assert_eq!(sched.last_fire_ms(5), 1_000);
}

#[test]
fn set_delay_ms_changes_period_without_resetting_anchor() {
    let mut sched = Scheduler::new();
    sched.register(9, 100, true, 0);
    // Fire once at 100.
    assert_eq!(sched.dispatch_due_events(100), vec![9]);
    // Re-program to 50ms cadence. Next fire should be at 150
    // (last_fire 100 + 50), not at 100 + 100.
    sched.set_delay_ms(9, 50);
    assert!(sched.dispatch_due_events(149).is_empty());
    assert_eq!(sched.dispatch_due_events(150), vec![9]);
}

#[test]
fn set_last_fire_ms_supports_poem_scroller_3s_delay() {
    // CPoemScroller's first scroll fires 3000ms after registration —
    // implemented by `Scheduler_SetEventLastFireMs(now + 3000)` so
    // the slot doesn't fire until the synthetic future timestamp.
    let mut sched = Scheduler::new();
    sched.register(11, 120, true, 0);
    sched.set_last_fire_ms(11, 3_000); // park anchor 3s into the future
    // No fire at t=1_000 (would be due at 3120).
    assert!(sched.dispatch_due_events(1_000).is_empty());
    // No fire at t=3_119.
    assert!(sched.dispatch_due_events(3_119).is_empty());
    // First fire at t=3_120.
    assert_eq!(sched.dispatch_due_events(3_120), vec![11]);
}

#[test]
fn pause_and_unpause_stops_catch_up_burst() {
    let mut sched = Scheduler::new();
    sched.register(3, 10, true, 0);
    sched.pause(3);
    // Wall-clock jumps 1s while paused. No fires.
    assert!(sched.dispatch_due_events(1_000).is_empty());
    // Unpause re-anchors to current `now_ms` (avoids a 100-fire flood).
    sched.unpause(3, 1_000);
    assert!(sched.dispatch_due_events(1_009).is_empty());
    assert_eq!(sched.dispatch_due_events(1_010), vec![3]);
}

#[test]
fn re_registering_same_id_replaces_in_place() {
    let mut sched = Scheduler::new();
    sched.register(1, 10, true, 0);
    sched.register(1, 5, true, 100); // re-register slot 1
    assert_eq!(sched.slot_count(), 1, "no second slot must be appended");
    assert_eq!(sched.last_fire_ms(1), 100, "re-register must reset the anchor");
    // Slot 1 should now have delay=5 anchored at 100 — first fire @ 105.
    assert!(sched.dispatch_due_events(104).is_empty());
    assert_eq!(sched.dispatch_due_events(105), vec![1]);
}

#[test]
fn catch_up_burst_is_bounded_against_long_pause() {
    // Simulate the "OS sleep" scenario: a 10ms slot frozen at t=0,
    // then time jumps to t=60s. Naive scheduling would fire 6000 times.
    let mut sched = Scheduler::new();
    sched.register(99, 10, true, 0);
    let fired = sched.dispatch_due_events(60_000);
    assert!(fired.len() <= MAX_CATCH_UP_PER_DISPATCH as usize,
            "catch-up burst must be bounded; fired {} times", fired.len());
    // The slot must remain active and ready for the next frame.
    assert!(sched.is_active(99));
    // After the safety bail, last_fire_ms snaps to now_ms.
    assert_eq!(sched.last_fire_ms(99), 60_000);
}

#[test]
fn next_due_event_returns_one_slot_per_call() {
    let mut sched = Scheduler::new();
    sched.register(1, 10, true, 0);
    // Three intervals have matured (10/20/30) by t=35.
    assert_eq!(sched.next_due_event(35), Some(1));
    assert_eq!(sched.last_fire_ms(1), 10, "must advance by delay, not snap to now");
    assert_eq!(sched.next_due_event(35), Some(1));
    assert_eq!(sched.last_fire_ms(1), 20);
    assert_eq!(sched.next_due_event(35), Some(1));
    assert_eq!(sched.last_fire_ms(1), 30);
    // Now caught up — next would fire at 40.
    assert_eq!(sched.next_due_event(35), None);
}

#[test]
fn next_due_event_lets_handler_reprogram_delay_mid_burst() {
    // This is the headline regression test for the Ruch behavior: the
    // handler runs *between* catch-up iterations, so a `set_delay_ms`
    // from inside the handler must take effect for the next iteration.
    //
    // Scenario mirrors `CRuch_OnScheduledTick @ 0x00423de0`:
    //   - Slot starts at delay=100ms (visible phase).
    //   - First fire: handler flips to "hidden" and reprograms delay=2000.
    //   - The huge frame stutter (now_ms=500) must NOT swallow the
    //     transition by firing four extra times with the stale 100ms cadence.
    let mut sched = Scheduler::new();
    sched.register(99, 100, true, 0);

    let mut fires = 0u32;
    while let Some(id) = sched.next_due_event(500) {
        assert_eq!(id, 99);
        fires += 1;
        // Handler emulates Ruch's visible→hidden re-program.
        sched.set_delay_ms(99, 2_000);
        if fires > 4 {
            panic!("scheduler did not honor the handler's new delay");
        }
    }
    assert_eq!(fires, 1, "exactly one fire — the new 2000ms delay pushes the next fire to t=2100");
}

#[test]
fn dispatch_due_events_remains_burst_compatible() {
    // The non-interleaving convenience method keeps its existing semantics
    // so callers like the dedicated server (whose handlers never re-program
    // their own slot) can rely on the bursted Vec output.
    let mut sched = Scheduler::new();
    sched.register(42, 10, true, 0);
    let fired = sched.dispatch_due_events(53);
    assert_eq!(fired, vec![42, 42, 42, 42, 42]);
}

#[test]
fn snap_all_to_clears_pending_catch_up_burst() {
    let mut sched = Scheduler::new();
    sched.register(1, 10, true, 0);
    sched.register(2, 25, true, 0);
    sched.snap_all_to(10_000);
    assert!(sched.dispatch_due_events(10_009).is_empty());
    assert_eq!(sched.dispatch_due_events(10_010), vec![1]);
    assert!(sched.dispatch_due_events(10_010).is_empty());
}

#[test]
fn multiple_slots_dispatch_independently() {
    let mut sched = Scheduler::new();
    sched.register(1, 10, true, 0);   // every 10ms
    sched.register(2, 25, true, 0);   // every 25ms
    sched.register(3, 100, false, 0); // single-shot @ 100
    let fired = sched.dispatch_due_events(100);
    // 10ms slot: fires at 10/20/30/40/50/60/70/80/90/100 = 10 fires
    // 25ms slot: fires at 25/50/75/100 = 4 fires
    // single-shot: fires once
    let count_1 = fired.iter().filter(|&&x| x == 1).count();
    let count_2 = fired.iter().filter(|&&x| x == 2).count();
    let count_3 = fired.iter().filter(|&&x| x == 3).count();
    assert_eq!(count_1, 10);
    assert_eq!(count_2, 4);
    assert_eq!(count_3, 1);
}
