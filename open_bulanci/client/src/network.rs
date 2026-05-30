//! Networking — UDP gateway connect, packet drain, and server-state
//! reconciliation.
//!
//! The reconciliation step replays our locally-recorded inputs from
//! `last_server_tick+1..=client_tick` against the authoritative server
//! state so the predicted simulation only diverges by inputs the server
//! hasn't yet seen.

use std::net::UdpSocket;
use anyhow::Result;

use bulanci_core::{ClientMessage, ServerMessage};

use crate::app::ClientApp;
use crate::state::AppPhase;

impl ClientApp {
    pub fn try_connect(&mut self) -> Result<()> {
        let socket = UdpSocket::bind("0.0.0.0:0")?;
        socket.set_nonblocking(true)?;
        let msg = ClientMessage::Connect { name: self.player_name.clone() };
        let bytes = postcard::to_allocvec(&msg)?;
        socket.send_to(&bytes, self.server_addr)?;
        self.socket = Some(socket);
        self.phase = AppPhase::Connecting;
        Ok(())
    }

    pub fn poll_network(&mut self) {
        let socket = match &self.socket {
            Some(s) => match s.try_clone() {
                Ok(c) => c,
                Err(_) => return,
            },
            None => return,
        };

        let mut buf = [0u8; 4096];
        while let Ok((size, _)) = socket.recv_from(&mut buf) {
            let pkt = &buf[..size];
            let Ok(server_msg) = postcard::from_bytes::<ServerMessage>(pkt) else { continue };
            match server_msg {
                ServerMessage::Welcome { assigned_id, initial_state } => {
                    self.assigned_id = Some(assigned_id);
                    self.authoritative_state = initial_state.clone();
                    self.predicted_sim.state = initial_state;
                    self.client_tick = self.authoritative_state.tick;
                    self.last_server_tick = self.authoritative_state.tick;
                    self.phase = AppPhase::Playing;
                    // The match is live — retract the connecting overlay.
                    self.surfaces.pop_overlay_named("connecting");
                }
                ServerMessage::StateUpdate { state } => {
                    if self.phase == AppPhase::Playing && state.tick > self.last_server_tick {
                        self.last_server_tick = state.tick;
                        self.authoritative_state = state;
                        self.reconcile_state();
                    }
                }
                ServerMessage::PlayerJoined { .. } => {}
                ServerMessage::PlayerLeft { id } => {
                    self.players_cache.remove(&id);
                }
            }
        }
    }

    /// Roll our predicted simulation back to the authoritative server state,
    /// then replay the local input history forward to `client_tick`. This
    /// is the standard client-side prediction + server-reconciliation step.
    fn reconcile_state(&mut self) {
        let Some(assigned_id) = self.assigned_id else { return };
        self.predicted_sim.state = self.authoritative_state.clone();
        let start_tick = self.last_server_tick + 1;
        for tick in start_tick..=self.client_tick {
            let local_input = self.input_history.get(&tick).cloned().unwrap_or_default();
            let mut inputs = vec![None; 32];
            inputs[assigned_id as usize] = Some(local_input);
            self.predicted_sim.tick(&inputs);
        }
        for p in &self.authoritative_state.players {
            if p.id != assigned_id {
                self.players_cache.insert(p.id, p.clone());
            }
        }
    }
}
