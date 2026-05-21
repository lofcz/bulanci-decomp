use super::*;
use std::thread::sleep;
use std::time::Duration;

#[test]
fn clock_starts_at_zero_and_advances() {
    let mut c = EngineClock::new();
    c.tick();
    let first = c.elapsed_ms;
    sleep(Duration::from_millis(15));
    c.tick();
    let second = c.elapsed_ms;
    assert!(second > first, "clock must advance across two ticks: {first} -> {second}");
    assert!(second >= 10, "after a 15ms sleep elapsed_ms should be >= 10ms, got {second}");
}

#[test]
fn synthetic_clock_supports_manual_seeking() {
    let mut c = EngineClock::synthetic();
    c.set_elapsed_ms(12_345);
    assert_eq!(c.elapsed_ms, 12_345);
    c.set_elapsed_ms(67_890);
    assert_eq!(c.elapsed_ms, 67_890);
}
