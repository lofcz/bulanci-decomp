//! The **gameplay patch** vocabulary — sim-op edits to a live match's
//! [`GameplayDoc`](super::doc::GameplayDoc).
//!
//! Same `{ surface, rev, ops }` envelope as scene/level patches (so the one
//! router routes them), but the ops mutate gamemode configuration — tunables,
//! the spawn table, the gamemode selection — rather than a control tree or
//! level geometry. Critically, applying these in a *live networked match* is
//! gated: see [`HmrMode`](super::surface::HmrMode) and
//! [`GameplaySurface`](super::surface::GameplaySurface), which only queue ops
//! to a tick boundary so peers stay lockstep-deterministic.

use serde::Deserialize;

use crate::engine::SimTunables;
use crate::level::LevelSpawn;

/// Editor→engine envelope for a *gameplay* surface.
#[derive(Deserialize, Debug, Clone)]
pub struct GameplayPatch {
    #[serde(alias = "scene")]
    pub surface: String,
    #[serde(default)]
    pub rev: u64,
    pub ops: Vec<GameplayOp>,
}

impl GameplayPatch {
    pub fn from_json(bytes: &[u8]) -> anyhow::Result<Self> {
        Ok(serde_json::from_slice(bytes)?)
    }
}

/// In-place mutations to a [`GameplayDoc`](super::doc::GameplayDoc).
#[derive(Deserialize, Debug, Clone, PartialEq, Eq)]
#[serde(tag = "op", rename_all = "snake_case")]
pub enum GameplayOp {
    /// Set one named sim tunable (`player_speed`, `bullet_speed`, `player_w`,
    /// `player_h`, `arena_w`, `arena_h`). Unknown keys are ignored.
    SetTunable { key: String, value: i32 },
    /// Replace the whole tunables block at once.
    SetTunables { tunables: SimTunables },
    /// Insert a spawn, or replace the one already carrying `spawn.id`.
    AddSpawn { spawn: LevelSpawn },
    /// Remove a spawn by id.
    RemoveSpawn { id: String },
    /// Select the deterministic gamemode rules script.
    SetGamemode { gamemode: String },
}

/// Apply one tunable by name to a [`SimTunables`]. Returns `false` for an
/// unknown key (so callers can warn without it being fatal).
pub fn set_tunable_field(t: &mut SimTunables, key: &str, value: i32) -> bool {
    match key {
        "player_speed" => t.player_speed = value,
        "player_w" => t.player_w = value,
        "player_h" => t.player_h = value,
        "bullet_speed" => t.bullet_speed = value,
        "arena_w" => t.arena_w = value,
        "arena_h" => t.arena_h = value,
        _ => return false,
    }
    true
}
