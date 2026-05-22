//! `CGunMouse` — the menu's red software cursor (lines + reticle ring + dot).
//!
//! Field layout mirrors the original `CGunMouse` C++ class as decoded from
//! Ghidra (`CGunMouse_ctor @ 0x00426060`, `CGunMouse_Draw @ 0x00424610`,
//! `CGunMouse_Activate @ 0x00423b50`):
//!
//! ```text
//!   +0x190 / +0x194  m_mouseX / m_mouseY      — snapshot of the live cursor
//!   +0x198 / +0x19c  m_targetX / m_targetY    — "dot anchor" in screen space
//!   +0x1a0 / +0x1a4  m_dotOffsetX / m_dotOffsetY — Activate seeds them to (2, 3)
//!   +0x1a8 / +0x1ac  m_dotBaseOffsetX / Y     — accumulator promoted into the
//!                                                snapshot by `Erase`
//!   +0x208…          coord ring buffer (cap = 30, push every 10 ms)
//! ```
//!
//! The dot is drawn at `(m_targetX + m_dotOffsetX, m_targetY + m_dotOffsetY)`
//! minus half the sprite size (see `CGunMouse_Draw` decomp). With the seed
//! `(2, 3)` constant, that simplifies to "dot sits at `m_target` ± a few
//! pixels".
//!
//! `m_target` is the queue's oldest sample. Verified live with
//! `tools/frida/cursor_trace.js`: every `Draw` advances `[+0x214]`
//! (queue head) by 1 and copies the just-popped entry into
//! `[+0x198]/[+0x19c]`. There is NO rolling-average or interpolation
//! step inside `Draw`.
//!
//! ## Queue cadence: 5 ms scheduler slot, not per-Draw
//!
//! In the original the push + pop sits inside `CGunMouse::Draw`,
//! gated by `lastPushTick + 10 <= g_dwElapsedMs`. That gate looks
//! like a 100 Hz cap but **only acts as one when fps is well above
//! 100**; below that, the gap between `g_dwElapsedMs` and
//! `lastPushTick` widens unboundedly, the gate fires every Draw, and
//! the push rate collapses to the frame rate. So:
//!   * fps ≫ 100 (≈200 fps, the trace's regime): push ≈ 5 ms,
//!     lag = 30 × 5 = 150 ms (the trace consistently measures
//!     134–140 ms from first mouse motion to first target update).
//!   * fps = 60: push every 16.67 ms, lag = 500 ms — visually a
//!     half-second of "boxy" replay that feels nothing like the
//!     original.
//!
//! We want the cursor to *feel* like the trace (150 ms-ish lag,
//! immediate visible catch-up via the sprite wobble) regardless of
//! render fps. The cleanest way is to drive the queue from a fixed
//! 5 ms scheduler slot ([`crate::state::SLOT_CURSOR_TICK`]) with the
//! engine's catch-up dispatcher. Push/pop then runs at exactly the
//! cadence the original was *built for* (the era's typical 200 fps)
//! regardless of our actual frame rate. See
//! [`GunMouse::on_cursor_tick`].
//!
//! ## Idle drift component
//!
//! The "catch-up immediately" sensation the user reported earlier
//! does NOT come from blending the queue — it comes from `dot_offset`,
//! which the active BitmapSprite (65801 / 65802 / 65803) keeps
//! nudging along a closed-loop motion path via `OnMouseMove`
//! (opcode 0x0A in `DecodeFrame` → `NotifyMove`).
//!
//! ## Idle motion path playback
//!
//! `CGunMouse_ctor @ 0x00426060` registers three `BitmapSprite`s
//! (`DAT_004af900 = {65803, 65802, 65801}`) with its inner track
//! manager via `AddTrackSource`, then calls `SetCurrentTrack(0, 1)`.
//! While a track is active, the engine fires one
//! `DecodeFrame → NotifyMove` per ~85 ms tick, applying the next
//! `(Δx, Δy)` from the sprite's opcode-0x0A stream to `dot_base_*`
//! **and** advances the frame index on the matching sprite strip.
//! Both come from the same `BitmapSprite` resource, so the visual
//! frame the player sees and the offset the dot drifts to are
//! always in lock-step. We load those resources at boot via
//! [`crate::cursor_atlas::CursorAtlasSet`]; tracks close to (0, 0)
//! by construction so re-rolling at end-of-loop is seamless.
//!
//! See [`GunMouse::on_cursor_tick`], [`GunMouse::on_sprite_frame_tick`]
//! and [`GunMouse::on_mouse_move`] for the per-call semantics.

use macroquad::prelude::*;
use std::collections::VecDeque;

use crate::atlas::Atlas;
use crate::cursor_atlas::{frame_motion, CursorAtlasSet};
use crate::msvcrt_rand::MsvcrtRand;

pub const COORD_QUEUE_CAP: usize = 30;
pub const RETICLE_W: i32 = 75;
pub const RETICLE_H: i32 = 75;
pub const RETICLE_CENTER: i32 = RETICLE_W / 2;
pub const DOT_BASE_SEED_X: f32 = 2.0;
pub const DOT_BASE_SEED_Y: f32 = 3.0;
pub const SCREEN_W: i32 = 800;
pub const SCREEN_H: i32 = 600;
pub const LINE_THICKNESS_PX: f32 = 1.0;
pub const LINE_ALPHA: f32 = 0.5;

pub struct GunMouse {
    pub mouse_x: f32,
    pub mouse_y: f32,

    prev_frame_mouse_x: f32,
    prev_frame_mouse_y: f32,

    pub target_x: f32,
    pub target_y: f32,

    pub dot_offset_x: f32,
    pub dot_offset_y: f32,

    pub dot_base_x: f32,
    pub dot_base_y: f32,

    dot_needs_update: bool,
    coord_queue: VecDeque<(f32, f32)>,
    pub dot_ready: bool,
    pub drift_track: u8,
    pub sprite_frame: u32,

    /// Track-selection RNG. The original `CGunMouse_OnAnimTick @
    /// 0x00423bd0` calls `MSVCRT::rand()` directly; we keep a
    /// dedicated [`MsvcrtRand`] instance per `GunMouse` so the
    /// cursor's random schedule is reproducible and isolated from
    /// any other RNG consumer (gameplay sim, AI, etc.). Default
    /// seed = 1, matching the C runtime's unseeded state.
    pub track_rng: MsvcrtRand,
}

impl GunMouse {
    pub fn new() -> Self {
        let mut g = GunMouse {
            mouse_x: 400.0,
            mouse_y: 300.0,
            prev_frame_mouse_x: 400.0,
            prev_frame_mouse_y: 300.0,
            target_x: 400.0,
            target_y: 300.0,
            dot_offset_x: 0.0,
            dot_offset_y: 0.0,
            dot_base_x: 0.0,
            dot_base_y: 0.0,
            dot_needs_update: false,
            coord_queue: VecDeque::with_capacity(COORD_QUEUE_CAP),
            dot_ready: false,
            drift_track: 0,
            sprite_frame: 0,
            track_rng: MsvcrtRand::default(),
        };
        g.activate();
        g
    }

    pub fn activate(&mut self) {
        self.dot_base_x = DOT_BASE_SEED_X;
        self.dot_base_y = DOT_BASE_SEED_Y;
        self.dot_offset_x = DOT_BASE_SEED_X;
        self.dot_offset_y = DOT_BASE_SEED_Y;
        self.dot_needs_update = false;
        self.coord_queue.clear();
        self.dot_ready = false;
    }

    fn erase(&mut self) {
        self.dot_offset_x = self.dot_base_x;
        self.dot_offset_y = self.dot_base_y;
        self.dot_needs_update = false;
    }

    #[allow(dead_code)]
    pub fn on_mouse_move(&mut self, dx: f32, dy: f32) {
        if !self.dot_needs_update {
            self.dot_base_x = self.dot_offset_x + dx;
            self.dot_base_y = self.dot_offset_y + dy;
            self.dot_needs_update = true;
        } else {
            self.dot_base_x += dx;
            self.dot_base_y += dy;
        }
    }

    /// Per-render-frame entry point. Mirrors the *visual* half of
    /// `CGunMouse::Draw @ 0x00424610` — promote the dot-offset
    /// accumulator from the previous frame (`Erase`'s job in the
    /// original) and snapshot the live OS mouse so the next batch
    /// of `SLOT_CURSOR_TICK` fires see the freshest position.
    ///
    /// The queue push/pop is intentionally **not** done here. It is
    /// driven by [`Self::on_cursor_tick`] from the engine scheduler
    /// at a fixed 5 ms cadence — see the module-level docs for why.
    pub fn update_frame(&mut self) {
        self.erase();

        let (mx, my) = crate::window_mode::logical_mouse_position();
        self.mouse_x = mx;
        self.mouse_y = my;
        self.prev_frame_mouse_x = mx;
        self.prev_frame_mouse_y = my;
    }

    /// One logical "queue beat" — fires from `SLOT_CURSOR_TICK`
    /// (5 ms cadence with catch-up). Mirrors the inline block in
    /// `CGunMouse::Draw @ 0x00424610`:
    ///
    /// ```c
    /// CoordRingPush(&ring, &m_mouseX);    // FUN_00424370
    /// if (ring.size == 30) {
    ///     CoordRingPop(&ring, &m_targetX);    // FUN_004243b0
    ///     m_dotReady = 1;
    /// }
    /// ```
    ///
    /// Key fidelity points:
    /// * The push silently fails when the ring is full — its
    ///   capacity-check returns 0 in `FUN_00424370`. We model that
    ///   by skipping the `push_back` when the ring is saturated.
    ///   In steady state size therefore oscillates between 29 and 30.
    /// * Pop only fires when the ring is *exactly* full (`== 30`),
    ///   not "any time there's data". This is what creates the
    ///   30-tick warm-up at activation time (~150 ms here) before
    ///   the dot becomes visible — exactly what the Frida trace
    ///   shows from the cold-start of the menu.
    /// * No `lastPushTick + 10 <= elapsed` gate: the 5 ms scheduler
    ///   cadence *is* the gate. The original's gate at the era's
    ///   ~200 fps already yielded a 5 ms effective push interval,
    ///   so this is the trace-empirical cadence promoted to a
    ///   first-class scheduler beat.
    pub fn on_cursor_tick(&mut self) {
        if self.coord_queue.len() < COORD_QUEUE_CAP {
            self.coord_queue.push_back((self.mouse_x, self.mouse_y));
        }
        if self.coord_queue.len() == COORD_QUEUE_CAP {
            if let Some((tx, ty)) = self.coord_queue.pop_front() {
                self.target_x = tx;
                self.target_y = ty;
                self.dot_ready = true;
            }
        }
    }

    /// Index into the [`CursorAtlasSet::tracks`] array, clamped to a
    /// safe range. Kept as a free function so callers can pass any
    /// borrow flavour of the atlas set without lifetime gymnastics.
    pub fn active_track_idx(&self, atlas: &CursorAtlasSet) -> usize {
        (self.drift_track as usize) % atlas.len()
    }

    /// Pick a fresh idle-twitch track and rewind to its first frame.
    ///
    /// Byte-faithful port of `CGunMouse_OnAnimTick @ 0x00423bd0`'s
    /// re-roll branch (`param_2 == -1`):
    ///
    /// ```c
    /// iVar1 = MSVCRT::rand();
    /// SetCurrentTrack(this+0x1a4,
    ///     (iVar1*0x30 + ((iVar1*0x30 >> 0x1f) & 0x7fff)) >> 0x13,
    ///     1);
    /// ```
    ///
    /// The `(rand() * 48) >> 19` formula is a zero-bias uniform
    /// 3-way roll (10923 / 10923 / 10922 outcomes over rand()'s
    /// `[0, 32767]` range) — verified empirically against the
    /// Frida trace in `tools/frida/analyze_track_distribution.py`
    /// (chi² = 2.36, df = 2, p ≈ 0.31; observed counts are
    /// consistent with the uniform null at any reasonable α).
    ///
    /// We re-roll on track completion only (the `param_2 == -1`
    /// branch). Every track is a closed loop, so switching at
    /// end-of-loop yields a seamless transition; switching
    /// mid-track would teleport the dot.
    ///
    /// `track_count` is the number of registered tracks (always 3
    /// in the master pack, but plumbed through so the test suite
    /// can use a stub atlas).
    pub fn on_drift_track_tick(&mut self, track_count: usize) {
        let idx = self.track_rng.roll_three() as usize;
        let cap = track_count.max(1);
        // Should always be 0/1/2 by construction; clamp defensively.
        self.drift_track = (idx % cap) as u8;
        self.sprite_frame = 0;
    }

    /// Per-frame tick of the active idle-twitch track.
    ///
    /// One call corresponds to one `DecodeFrame → NotifyMove` in the
    /// engine: apply the active track's next `(Δx, Δy)` to `dot_base_*`
    /// (via the same `OnMouseMove` accumulator the original observer
    /// uses) **and** advance the sprite frame index. When the index
    /// wraps past the track's length the loop has fully closed
    /// (cumulative offset back at zero by construction), so we re-roll
    /// the active track.
    ///
    /// Cadence: ~85 ms per tick, computed from the BitmapSprite's
    /// `sequenceDefaultCadenceMs` (clip duration ÷ frame count). All
    /// three master-pack tracks land in 84–85 ms per the Frida trace,
    /// so the SLOT_CURSOR_DOT_FRAME slot is hard-wired to 85 ms.
    ///
    /// Takes a slice of [`Atlas`]es — frame index and motion delta
    /// come from the same `frames[i]` entry, so the visual and the
    /// positional drift can never desync. The headless test suite
    /// builds the slice via [`crate::atlas::parse_atlas_bytes`]
    /// (which leaves each atlas's `texture` as `None`) so no GL
    /// context is required.
    pub fn on_sprite_frame_tick(&mut self, tracks: &[Atlas]) {
        if tracks.is_empty() {
            return;
        }
        let idx = (self.drift_track as usize) % tracks.len();
        let track = &tracks[idx];
        let i = (self.sprite_frame as usize) % track.frame_count();
        let (dx, dy) = frame_motion(&track.frames[i]);
        self.on_mouse_move(dx as f32, dy as f32);

        let next = self.sprite_frame.wrapping_add(1);
        if (next as usize) >= track.frame_count() {
            self.on_drift_track_tick(tracks.len());
        } else {
            self.sprite_frame = next;
        }
    }

    /// Render the full cursor: crosshair arms, reticle ring, and the
    /// motion-track dot. The dot is always drawn from the *active*
    /// track's atlas — strip frame and (Δx, Δy) come from the same
    /// `BitmapSprite` resource, in lock-step.
    pub fn draw(&self, reticle: &Texture2D, atlas: &CursorAtlasSet, dot_alpha: f32) {
        let mx = self.mouse_x.round() as i32;
        let my = self.mouse_y.round() as i32;
        let ret_left = mx - RETICLE_CENTER;
        let ret_top = my - RETICLE_CENTER;
        let ret_right = ret_left + RETICLE_W;
        let ret_bottom = ret_top + RETICLE_H;

        if self.dot_ready {
            let idx = self.active_track_idx(atlas);
            let track = atlas.atlas(idx);
            let texture = atlas.texture(idx);
            // Clamp the sprite frame index to the track length —
            // re-rolling mid-tick could otherwise leave `sprite_frame`
            // pointing one past the new track's last frame for the
            // single render between the wrap and the next dispatch.
            let frame_idx = (self.sprite_frame as usize) % track.frame_count();
            let frame = &track.frames[frame_idx];

            let rect = frame.rect;
            let fw = rect.w as f32;
            let fh = rect.h as f32;
            let ox = frame.origin.0 as f32;
            let oy = frame.origin.1 as f32;

            let cx = self.target_x + self.dot_offset_x;
            let cy = self.target_y + self.dot_offset_y;
            let dot_left = (cx - ox).round();
            let dot_top = (cy - oy).round();
            draw_texture_ex(
                texture,
                dot_left,
                dot_top,
                Color::new(1.0, 1.0, 1.0, dot_alpha),
                DrawTextureParams {
                    source: Some(Rect::new(rect.x as f32, rect.y as f32, fw, fh)),
                    dest_size: Some(vec2(fw, fh)),
                    ..Default::default()
                },
            );
        }

        let line = Color::new(1.0, 0.0, 0.0, LINE_ALPHA);
        if ret_left > 0 {
            draw_rectangle(
                0.0,
                my as f32,
                ret_left as f32,
                LINE_THICKNESS_PX,
                line,
            );
        }
        if ret_right < SCREEN_W {
            draw_rectangle(
                ret_right as f32,
                my as f32,
                (SCREEN_W - ret_right) as f32,
                LINE_THICKNESS_PX,
                line,
            );
        }
        if ret_top > 0 {
            draw_rectangle(
                mx as f32,
                0.0,
                LINE_THICKNESS_PX,
                ret_top as f32,
                line,
            );
        }
        if ret_bottom < SCREEN_H {
            draw_rectangle(
                mx as f32,
                ret_bottom as f32,
                LINE_THICKNESS_PX,
                (SCREEN_H - ret_bottom) as f32,
                line,
            );
        }

        draw_texture(
            reticle,
            ret_left as f32,
            ret_top as f32,
            Color::new(1.0, 1.0, 1.0, LINE_ALPHA),
        );
    }
}

impl Default for GunMouse {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
#[path = "cursor_tests.rs"]
mod tests;
