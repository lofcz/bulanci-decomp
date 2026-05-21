//! Live match — `SLOT_GAME_TICK` handler + the gameplay-phase render.
//!
//! Sim ticks are driven by the scheduler at 17ms cadence with drift-free
//! catch-up: under a frame stall the simulation will fire many times in
//! one frame so the predicted world stays clock-locked to the wall clock.

use macroquad::prelude::*;

use bulanci_core::ClientMessage;

use crate::app::ClientApp;

/// Convert a facing dir byte to a (dx, dy) pixel offset used for drawing
/// the muzzle indicator. 0 = UP, 1 = RIGHT, 2 = DOWN, 3 = LEFT.
pub fn facing_offset(facing_dir: u8) -> (f32, f32) {
    match facing_dir {
        0 => (0.0, -25.0),
        1 => (25.0, 0.0),
        2 => (0.0, 25.0),
        3 => (-25.0, 0.0),
        _ => (0.0, 0.0),
    }
}

impl ClientApp {
    /// SLOT_GAME_TICK handler — fires drift-free at strict 17ms cadence.
    /// One call advances the predicted simulation by exactly one tick and
    /// forwards the freshly-sampled input to the authoritative server.
    /// During a catch-up burst (e.g. after a long render stall) this can
    /// be invoked many times within a single frame, keeping the predicted
    /// world clock-synced to wall-clock time.
    pub fn on_game_tick(&mut self) {
        self.client_tick += 1;
        let input = self.pending_input.clone();

        self.input_history.insert(self.client_tick, input.clone());

        let mut inputs_to_tick = vec![None; 32];
        if let Some(id) = self.assigned_id {
            inputs_to_tick[id as usize] = Some(input.clone());
        }
        self.predicted_sim.tick(&inputs_to_tick);

        if let Some(socket) = &self.socket {
            let msg = ClientMessage::Input { tick: self.client_tick, inputs: input };
            if let Ok(bytes) = postcard::to_allocvec(&msg) {
                let _ = socket.send_to(&bytes, self.server_addr);
            }
        }

        // `shoot` is a press-edge — clear it for the next tick (a held
        // key only fires on its initial press frame, mirroring the
        // original game's CBulanci::OnShoot one-shot semantics).
        self.pending_input.shoot = false;

        // Discard input history older than ~5 seconds of sim time.
        self.input_history.retain(|&tick, _| tick >= self.client_tick.saturating_sub(300));
    }

    pub fn draw_gameplay(&mut self) {
        // ---- Map obstacles. ----
        for obs in &self.predicted_sim.state.obstacles {
            let x = obs.x1 as f32;
            let y = obs.y1 as f32;
            let w = (obs.x2 - obs.x1) as f32;
            let h = (obs.y2 - obs.y1) as f32;
            draw_rectangle(x, y, w, h, DARKGRAY);
            draw_rectangle_lines(x, y, w, h, 2.0, GRAY);
        }

        // ---- Other players. ----
        for other in self.players_cache.values() {
            if !other.is_alive {
                continue;
            }
            draw_circle(other.pos.x as f32, other.pos.y as f32, 20.0, RED);
            let (fx, fy) = facing_offset(other.facing_dir);
            draw_line(
                other.pos.x as f32, other.pos.y as f32,
                other.pos.x as f32 + fx, other.pos.y as f32 + fy,
                3.0, YELLOW,
            );
        }

        // ---- Local player (predicted). ----
        if let Some(id) = self.assigned_id {
            if let Some(me) = self.predicted_sim.state.players.iter().find(|p| p.id == id) {
                if me.is_alive {
                    draw_circle(me.pos.x as f32, me.pos.y as f32, 20.0, GREEN);
                    let (fx, fy) = facing_offset(me.facing_dir);
                    draw_line(
                        me.pos.x as f32, me.pos.y as f32,
                        me.pos.x as f32 + fx, me.pos.y as f32 + fy,
                        3.0, YELLOW,
                    );
                } else {
                    self.draw_t("Jste mrtvý! Čeká se na respawn…", 230.0, 290.0, 22, RED);
                }
            }
        }

        // ---- Bullets. ----
        for b in &self.predicted_sim.state.bullets {
            if b.is_active {
                draw_circle(b.pos.x as f32, b.pos.y as f32, 4.0, ORANGE);
            }
        }

        // ---- HUD. ----
        self.draw_t(&format!("Tick: {}", self.client_tick), 15.0, 30.0, 18, WHITE);
        self.draw_t("Skóre:", 15.0, 58.0, 18, Color::new(1.0, 0.85, 0.3, 1.0));
        let mut y = 78.0;
        if let Some(id) = self.assigned_id {
            if let Some(me) = self.predicted_sim.state.players.iter().find(|p| p.id == id) {
                self.draw_t(&format!("Vy (#{}): {}", id, me.score), 15.0, y, 16, GREEN);
                y += 18.0;
            }
        }
        for other in self.players_cache.values() {
            self.draw_t(&format!("#{}: {}", other.id, other.score), 15.0, y, 16, RED);
            y += 18.0;
        }
    }
}
