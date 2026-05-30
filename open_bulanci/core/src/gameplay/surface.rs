//! [`GameplaySurface`] — a live match as an [`HmrSurface`], with the
//! determinism gating a networked sim demands.
//!
//! It owns the two halves of a match:
//!   * the deterministic [`EngineSimulation`](crate::engine::EngineSimulation)
//!     (the Rust *primitive* core — movement, AABB collision, hitscan,
//!     bullets, advanced by the netcode);
//!   * the [`GameplayDoc`] (the hot-patchable *configuration* — tunables,
//!     spawn table, gamemode selection).
//!
//! ## The determinism rule (critical)
//!
//! In a lockstep match every peer must apply identical state transitions on
//! identical ticks. A raw editor patch that mutated the sim *mid-tick*, or on
//! only one peer, would desync the match instantly. So HMR here is **gated by
//! [`HmrMode`]**:
//!   * [`HmrMode::Dev`] (single-player / editor preview): edits apply
//!     immediately — there are no peers to desync.
//!   * [`HmrMode::LiveLockstep`] (networked match): [`apply_raw_patch`] only
//!     *queues* the ops; they are drained and applied at the next **tick
//!     boundary** inside [`GameplaySurface::tick`], the same boundary every
//!     peer crosses together. The netcode is expected to broadcast the queued
//!     ops as a deterministic event so all peers apply them on the same tick;
//!     until that broadcast exists, live HMR stays a dev/single-player
//!     affordance and `pending_ops` exposes what would need syncing.
//!
//! [`apply_raw_patch`]: HmrSurface::apply_raw_patch

use std::cell::{Ref, RefCell};

use crate::engine::EngineSimulation;
use crate::scene::patch::peek_surface;
use crate::scene::{DrawItem, HmrSurface, PatchOutcome};
use crate::FrameInputs;

use super::doc::GameplayDoc;
use super::draw::world_draw_items;
use super::patch::{set_tunable_field, GameplayOp, GameplayPatch};

/// How live-patches to a gameplay surface are allowed to apply — the
/// determinism gate (see the module docs).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum HmrMode {
    /// Single-player / editor preview: apply edits immediately.
    Dev,
    /// Networked lockstep match: queue edits, apply at a tick boundary, expect
    /// the netcode to broadcast them so peers stay in sync.
    LiveLockstep,
}

/// A hot-patchable live-match surface.
pub struct GameplaySurface {
    name: String,
    sim: RefCell<EngineSimulation>,
    doc: RefCell<GameplayDoc>,
    deps: Vec<String>,
    mode: HmrMode,
    /// Edits accepted but not yet applied (LiveLockstep only); drained at the
    /// next tick boundary.
    pending: RefCell<Vec<GameplayOp>>,
}

impl GameplaySurface {
    /// Build a surface around a doc. `mode` decides the HMR gate; the sim is
    /// seeded with the doc's tunables.
    pub fn new(doc: GameplayDoc, mode: HmrMode, deps: Vec<String>) -> Self {
        let sim = EngineSimulation::with_tunables(doc.tunables);
        Self {
            name: doc.name.clone(),
            sim: RefCell::new(sim),
            doc: RefCell::new(doc),
            deps,
            mode,
            pending: RefCell::new(Vec::new()),
        }
    }

    /// Read-only borrow of the live configuration.
    pub fn doc(&self) -> Ref<'_, GameplayDoc> {
        self.doc.borrow()
    }

    /// Read-only borrow of the deterministic world sim.
    pub fn sim(&self) -> Ref<'_, EngineSimulation> {
        self.sim.borrow()
    }

    /// Mutable access to the sim for the netcode (spawning players, applying
    /// authoritative snapshots). Kept explicit so it's obvious where the
    /// lockstep-critical world is mutated.
    pub fn sim_mut(&self) -> std::cell::RefMut<'_, EngineSimulation> {
        self.sim.borrow_mut()
    }

    pub fn mode(&self) -> HmrMode {
        self.mode
    }

    /// Ops accepted but not yet applied — what the netcode must broadcast for
    /// a deterministic live edit. Empty in [`HmrMode::Dev`].
    pub fn pending_ops(&self) -> Vec<GameplayOp> {
        self.pending.borrow().clone()
    }

    pub fn has_pending(&self) -> bool {
        !self.pending.borrow().is_empty()
    }

    /// Advance the match by one tick. This is the deterministic **tick
    /// boundary**: any queued HMR edits are applied here, *before* the step,
    /// so every peer that crosses this boundary applies the same edits on the
    /// same tick. Returns the sim's expired-timer slot ids (unchanged
    /// contract).
    pub fn tick(&self, inputs: &[Option<FrameInputs>]) -> Vec<i32> {
        self.drain_pending();
        self.sim.borrow_mut().tick(inputs)
    }

    /// Apply queued ops at a tick boundary and re-sync the sim. Public so the
    /// netcode can apply broadcast edits exactly when it decides the boundary
    /// is, independent of stepping.
    pub fn drain_pending(&self) {
        let ops = std::mem::take(&mut *self.pending.borrow_mut());
        if ops.is_empty() {
            return;
        }
        self.apply_ops(&ops);
    }

    /// Apply ops to the doc and push the (possibly changed) tunables into the
    /// sim. Never called mid-tick in LiveLockstep.
    fn apply_ops(&self, ops: &[GameplayOp]) {
        {
            let mut doc = self.doc.borrow_mut();
            for op in ops {
                match op {
                    GameplayOp::SetTunable { key, value } => {
                        if !set_tunable_field(&mut doc.tunables, key, *value) {
                            eprintln!("[gameplay] unknown tunable '{key}'");
                        }
                    }
                    GameplayOp::SetTunables { tunables } => doc.tunables = *tunables,
                    GameplayOp::AddSpawn { spawn } => {
                        match doc
                            .spawns
                            .iter_mut()
                            .find(|s| !s.id.is_empty() && s.id == spawn.id)
                        {
                            Some(existing) => *existing = spawn.clone(),
                            None => doc.spawns.push(spawn.clone()),
                        }
                    }
                    GameplayOp::RemoveSpawn { id } => doc.spawns.retain(|s| s.id != *id),
                    GameplayOp::SetGamemode { gamemode } => doc.gamemode = gamemode.clone(),
                }
            }
        }
        // Tunables flow into the sim at the boundary, never mid-tick.
        let tunables = self.doc.borrow().tunables;
        self.sim.borrow_mut().set_tunables(tunables);
    }

    /// The world as a flat draw list (obstacles → players → bullets), so a
    /// live match composes in the surface stack with a HUD overlay.
    pub fn draw_list(&self) -> Vec<DrawItem> {
        let sim = self.sim.borrow();
        world_draw_items(&sim.state.obstacles, &sim.state.players, &sim.state.bullets, sim.tunables)
    }

    /// Apply a typed gameplay patch, honoring the determinism gate. Returns
    /// `true` when accepted (applied in Dev, queued in LiveLockstep), `false`
    /// when addressed to a different surface.
    pub fn apply(&self, patch: &GameplayPatch) -> bool {
        if patch.surface != self.name {
            return false;
        }
        match self.mode {
            HmrMode::Dev => self.apply_ops(&patch.ops),
            HmrMode::LiveLockstep => self.pending.borrow_mut().extend(patch.ops.iter().cloned()),
        }
        true
    }
}

impl HmrSurface for GameplaySurface {
    fn surface_name(&self) -> &str {
        &self.name
    }

    fn deps(&self) -> &[String] {
        &self.deps
    }

    fn apply_raw_patch(&self, raw: &[u8]) -> PatchOutcome {
        match peek_surface(raw) {
            Some(s) if s == self.name => {}
            Some(_) => return PatchOutcome::NotMine,
            None => return PatchOutcome::Failed,
        }
        match GameplayPatch::from_json(raw) {
            Ok(patch) => {
                if self.apply(&patch) {
                    // In LiveLockstep this is "accepted, queued to a tick
                    // boundary"; in Dev it's "applied now". Either way the
                    // router stops here.
                    PatchOutcome::Applied
                } else {
                    PatchOutcome::NotMine
                }
            }
            Err(e) => {
                eprintln!("[gameplay-patch] parse error: {e}");
                PatchOutcome::Failed
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::engine::SimTunables;
    use crate::level::{LevelDoc, LevelSpawn};
    use crate::scene::NodeKind;

    fn doc() -> GameplayDoc {
        let level = LevelDoc {
            name: "arena".into(),
            spawns: vec![LevelSpawn { id: "p1".into(), x: 100, y: 100, team: 0 }],
            ..Default::default()
        };
        GameplayDoc::from_level(&level, "deathmatch")
    }

    #[test]
    fn dev_mode_applies_tunables_immediately_to_doc_and_sim() {
        let gp = GameplaySurface::new(doc(), HmrMode::Dev, vec!["gameplay/arena".into()]);
        let patch = r#"{ "surface":"arena", "ops":[
            { "op":"set_tunable", "key":"player_speed", "value":9 },
            { "op":"set_tunable", "key":"arena_w", "value":1024 }
        ] }"#;
        assert_eq!(gp.apply_raw_patch(patch.as_bytes()), PatchOutcome::Applied);
        assert!(!gp.has_pending());
        assert_eq!(gp.doc().tunables.player_speed, 9);
        // Tunables flowed into the deterministic sim immediately.
        assert_eq!(gp.sim().tunables.player_speed, 9);
        assert_eq!(gp.sim().tunables.arena_w, 1024);
    }

    #[test]
    fn live_mode_queues_until_tick_boundary() {
        let gp = GameplaySurface::new(doc(), HmrMode::LiveLockstep, vec![]);
        let patch = r#"{ "surface":"arena", "ops":[
            { "op":"set_tunable", "key":"player_speed", "value":9 } ] }"#;
        assert_eq!(gp.apply_raw_patch(patch.as_bytes()), PatchOutcome::Applied);

        // Accepted but NOT yet applied — the sim is untouched mid-match.
        assert!(gp.has_pending());
        assert_eq!(gp.sim().tunables.player_speed, SimTunables::default().player_speed);
        assert_eq!(gp.pending_ops().len(), 1);

        // Crossing a tick boundary applies it deterministically.
        gp.tick(&[]);
        assert!(!gp.has_pending());
        assert_eq!(gp.sim().tunables.player_speed, 9);
        assert_eq!(gp.doc().tunables.player_speed, 9);
    }

    #[test]
    fn set_tunables_replaces_block_and_unknown_key_is_ignored() {
        let gp = GameplaySurface::new(doc(), HmrMode::Dev, vec![]);
        let custom = SimTunables { player_speed: 2, player_w: 30, player_h: 30, bullet_speed: 12, arena_w: 640, arena_h: 480 };
        let json = format!(
            r#"{{ "surface":"arena", "ops":[ {{ "op":"set_tunables", "tunables":{} }} ] }}"#,
            serde_json::to_string(&custom).unwrap()
        );
        assert_eq!(gp.apply_raw_patch(json.as_bytes()), PatchOutcome::Applied);
        assert_eq!(gp.sim().tunables, custom);

        // Unknown tunable key is a no-op (still Applied — the patch parsed).
        let bad_key = r#"{ "surface":"arena", "ops":[ { "op":"set_tunable", "key":"nope", "value":1 } ] }"#;
        assert_eq!(gp.apply_raw_patch(bad_key.as_bytes()), PatchOutcome::Applied);
        assert_eq!(gp.sim().tunables, custom);
    }

    #[test]
    fn spawn_table_edits_apply() {
        let gp = GameplaySurface::new(doc(), HmrMode::Dev, vec![]);
        let patch = r#"{ "surface":"arena", "ops":[
            { "op":"add_spawn", "spawn":{ "id":"p2", "x":700, "y":500, "team":1 } },
            { "op":"set_gamemode", "gamemode":"ctf" }
        ] }"#;
        assert_eq!(gp.apply_raw_patch(patch.as_bytes()), PatchOutcome::Applied);
        assert_eq!(gp.doc().spawns.len(), 2);
        assert_eq!(gp.doc().gamemode, "ctf");
    }

    #[test]
    fn foreign_and_malformed_patches_are_distinguished() {
        let gp = GameplaySurface::new(doc(), HmrMode::Dev, vec![]);
        // A scene patch routed here -> NotMine (peek before parse).
        let scene = r#"{ "surface":"main_menu", "ops":[ { "op":"set_text", "id":"t", "text":"x" } ] }"#;
        assert_eq!(gp.apply_raw_patch(scene.as_bytes()), PatchOutcome::NotMine);
        // Our surface, undecodable op -> Failed.
        let bad = r#"{ "surface":"arena", "ops":[ { "op":"explode" } ] }"#;
        assert_eq!(gp.apply_raw_patch(bad.as_bytes()), PatchOutcome::Failed);
    }

    #[test]
    fn draw_list_emits_world_shapes() {
        let gp = GameplaySurface::new(doc(), HmrMode::Dev, vec![]);
        {
            let mut sim = gp.sim_mut();
            sim.add_obstacle(0, 0, 100, 20);
            sim.add_player(0, 0, 50, 50);
        }
        let items = gp.draw_list();
        // 1 obstacle quad + (1 circle + 1 facing line) for the alive player.
        assert_eq!(items.len(), 3);
        assert!(matches!(items[0].kind, NodeKind::Quad { .. }));
        assert!(matches!(items[1].kind, NodeKind::Circle { .. }));
        assert!(matches!(items[2].kind, NodeKind::Line { .. }));
    }
}
