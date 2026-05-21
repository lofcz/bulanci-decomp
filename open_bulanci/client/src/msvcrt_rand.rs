//! A byte-faithful reproduction of MSVCRT's classic 32-bit `rand()`.
//!
//! The original `Bulanci.exe` is a VC8 MSVCRT8 binary; every call to
//! `_rand` in the disassembly resolves to the canonical 15-bit
//! Lehmer/LCG variant Microsoft shipped from MSVCRT 1.0 onwards:
//!
//! ```text
//!     seed = seed * 214013 + 2531011
//!     return  (seed >> 16) & 0x7fff       // 15 bits, RAND_MAX = 32767
//! ```
//!
//! Reference: Microsoft's `crt/src/rand.c` (CRT source ships with VS).
//! The seed lives in a per-thread CRT slot and starts at `1` unless
//! `srand` is called. We mirror that default here.
//!
//! Why we bother:
//! `CGunMouse_OnAnimTick @ 0x00423bd0` selects the next idle-twitch
//! track with
//!
//! ```text
//!     idx = (MSVCRT::rand() * 0x30 + ((rand * 0x30) >> 0x1f & 0x7fff)) >> 0x13
//!         = (rand() * 3) / 32768          // sign-correction is a no-op
//! ```
//!
//! which is a true zero-bias uniform 3-way roll over `rand()`'s
//! `[0, 32767]` range (10923 / 10923 / 10922 outcomes). Implementing
//! the exact MSVCRT LCG + the exact formula means: given the same
//! seed the entire track schedule replays bit-for-bit against the
//! original — useful for golden-master cursor tests and replay
//! validation. See [`MsvcrtRand::next_u15`] and [`MsvcrtRand::roll_three`].

/// Linear-congruential generator matching `_rand` in MSVCRT8.
#[derive(Debug, Clone)]
pub struct MsvcrtRand {
    seed: u32,
}

impl Default for MsvcrtRand {
    fn default() -> Self {
        // C runtime initialises `_rand` state to 1 if `srand` was
        // never called. Match that so a freshly-constructed RNG
        // produces the canonical "default" sequence the original
        // game would have seen up to the point of its first `srand`.
        Self::new(1)
    }
}

impl MsvcrtRand {
    pub const fn new(seed: u32) -> Self {
        Self { seed }
    }

    /// Re-seed, as if calling `srand(seed)`. The MSVCRT `srand`
    /// likewise stores the seed verbatim into the per-thread state.
    /// Exposed for future use (e.g. seeded golden-master replay of
    /// the original game's track schedule from a captured trace).
    #[allow(dead_code)]
    pub fn srand(&mut self, seed: u32) {
        self.seed = seed;
    }

    /// One LCG step. Returns the canonical 15-bit `rand()` result
    /// in `[0, 32767]`.
    pub fn next_u15(&mut self) -> u32 {
        // u32 wraparound matches x86's 32-bit `imul`/`add` semantics.
        self.seed = self.seed.wrapping_mul(214013).wrapping_add(2531011);
        (self.seed >> 16) & 0x7fff
    }

    /// The literal `CGunMouse_OnAnimTick` track-selection roll.
    /// Returns `0`, `1`, or `2` with the exact bucket sizes the
    /// engine uses (10923 / 10923 / 10922 outcomes — i.e. uniform
    /// 1/3 within ±10⁻⁴).
    pub fn roll_three(&mut self) -> u32 {
        let r = self.next_u15() as i32;
        // Reproduce the asm verbatim (with the sign-correction
        // branch the decomp emitted — guaranteed a no-op here since
        // `r * 48` can never overflow a signed 32-bit int, but we
        // keep it for parity with the original).
        let scaled = r.wrapping_mul(0x30);
        let corr = ((scaled >> 0x1f) & 0x7fff) as i32;
        (scaled.wrapping_add(corr) >> 0x13) as u32
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// First five draws from a seed-1 MSVCRT rand stream. These are
    /// the canonical literature values you can verify against any
    /// Microsoft CRT source release (or against running the
    /// original `Bulanci.exe` under a tracer).
    #[test]
    fn matches_canonical_msvcrt_sequence_for_seed_one() {
        let mut r = MsvcrtRand::new(1);
        // Verified against the MSVCRT formula by hand and against
        // multiple independent reimplementations (e.g. ReactOS).
        let expected = [41, 18467, 6334, 26500, 19169];
        for (i, e) in expected.iter().enumerate() {
            let v = r.next_u15();
            assert_eq!(v, *e, "draw #{i} mismatch: got {v}, want {e}");
        }
    }

    /// The track-selection formula must produce one of {0, 1, 2}
    /// and the per-bucket counts over the full `[0, 32767]` range
    /// must match `CGunMouse_OnAnimTick`'s observable behaviour.
    #[test]
    fn roll_three_partitions_rand_range_into_exact_buckets() {
        // Brute-force every possible rand() output value. This is
        // a 32768-iteration loop — cheap.
        let mut counts = [0u32; 3];
        for r in 0..=32767i32 {
            let scaled = r.wrapping_mul(0x30);
            let corr = ((scaled >> 0x1f) & 0x7fff) as i32;
            let idx = ((scaled.wrapping_add(corr) >> 0x13) as u32) as usize;
            assert!(idx < 3, "rand={r}: produced out-of-range idx {idx}");
            counts[idx] += 1;
        }
        // Per the formula `(r * 48) >> 19` over r ∈ [0, 32767]:
        //   r in [    0, 10922] -> idx 0  (10923 outcomes)
        //   r in [10923, 21845] -> idx 1  (10923 outcomes)
        //   r in [21846, 32767] -> idx 2  (10922 outcomes)
        assert_eq!(counts, [10923, 10923, 10922]);
        // Sanity: total samples = 32768.
        assert_eq!(counts.iter().sum::<u32>(), 32768);
    }

    /// Long-run distribution test: across many seeds the empirical
    /// per-bucket frequency should be within ~0.5 % of 1/3. This
    /// catches a regression where someone "simplifies" the formula
    /// to `rand() % 3` and accidentally introduces modulo bias on a
    /// larger RAND_MAX.
    #[test]
    fn roll_three_is_approximately_uniform_over_many_draws() {
        let mut r = MsvcrtRand::new(0xc0ffee);
        let mut counts = [0u32; 3];
        for _ in 0..100_000 {
            counts[r.roll_three() as usize] += 1;
        }
        for c in counts {
            let p = c as f32 / 100_000.0;
            assert!(
                (p - 1.0 / 3.0).abs() < 0.005,
                "bucket frequency {p} deviates >0.5% from 1/3 — count = {c}",
            );
        }
    }
}
