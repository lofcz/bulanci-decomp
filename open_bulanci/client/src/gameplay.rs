//! Live match — `SLOT_GAME_TICK` handler + the gameplay-phase render.
//!
//! Sim ticks are driven by the scheduler at 17ms cadence with drift-free
//! catch-up: under a frame stall the simulation will fire many times in
//! one frame so the predicted world stays clock-locked to the wall clock.

use raylib::prelude::*;

use bulanci_core::gameplay::draw::{
    bullet_item, obstacle_item, player_items, TEAM0_COLOR, TEAM1_COLOR,
};
use bulanci_core::scene::DrawItem;
use bulanci_core::ClientMessage;

use crate::app::ClientApp;

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

    pub fn draw_gameplay<D: RaylibDraw>(&self, d: &mut D) {
        // The match world (obstacles, players, bullets) is built into the same
        // flat `DrawItem` list scenes produce and blitted through the unified
        // renderer — so gameplay composes in the surface stack with the menu's
        // draw path instead of its own bespoke immediate-mode code. The local
        // predicted player is highlighted green; server-cached peers red.
        let tunables = self.predicted_sim.tunables;
        let mut items: Vec<DrawItem> = Vec::new();

        for obs in &self.predicted_sim.state.obstacles {
            items.push(obstacle_item(obs));
        }
        for other in self.players_cache.values() {
            if other.is_alive {
                items.extend(player_items(other, TEAM1_COLOR, tunables));
            }
        }
        let mut local_is_dead = false;
        if let Some(id) = self.assigned_id {
            if let Some(me) = self.predicted_sim.state.players.iter().find(|p| p.id == id) {
                if me.is_alive {
                    items.extend(player_items(me, TEAM0_COLOR, tunables));
                } else {
                    local_is_dead = true;
                }
            }
        }
        for b in &self.predicted_sim.state.bullets {
            if b.is_active {
                items.push(bullet_item(b));
            }
        }
        crate::scene_runtime::draw_items(self, &items, d);

        // ---- HUD (text overlay; a HUD scene surface is the future home). ----
        if local_is_dead {
            self.draw_t(d, "Jste mrtvý! Čeká se na respawn…", 230.0, 290.0, 22, Color::RED);
        }
        self.draw_t(d, &format!("Tick: {}", self.client_tick), 15.0, 30.0, 18, Color::WHITE);
        self.draw_t(d, "Skóre:", 15.0, 58.0, 18, Color::new(255, 217, 77, 255));
        let mut y = 78.0;
        if let Some(id) = self.assigned_id {
            if let Some(me) = self.predicted_sim.state.players.iter().find(|p| p.id == id) {
                self.draw_t(d, &format!("Vy (#{}): {}", id, me.score), 15.0, y, 16, Color::GREEN);
                y += 18.0;
            }
        }
        for other in self.players_cache.values() {
            self.draw_t(d, &format!("#{}: {}", other.id, other.score), 15.0, y, 16, Color::RED);
            y += 18.0;
        }
    }
}
