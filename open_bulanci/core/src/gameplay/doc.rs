//! The **gameplay document** — the hot-patchable configuration half of a live
//! match, distinct from the per-tick world state.
//!
//! The deterministic [`EngineSimulation`](crate::engine::EngineSimulation)
//! owns the *world* (players, bullets, collisions) and the *primitives* that
//! evolve it. A [`GameplayDoc`] owns the *knobs*: the
//! [`SimTunables`](crate::engine::SimTunables) the sim reads, the spawn table a
//! gamemode draws from, and the name of the gamemode rules script. Splitting
//! configuration out of state is what makes gameplay HMR-able without touching
//! the lockstep-critical world: an editor edits the doc; the sim keeps running.

use serde::{Deserialize, Serialize};

use crate::engine::SimTunables;
use crate::level::{LevelDoc, LevelSpawn};

/// Live-tunable gameplay configuration. Seeded from a [`LevelDoc`] (spawns) and
/// gamemode defaults (tunables), then patched in place via
/// [`GameplayOp`](super::patch::GameplayOp).
#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, Eq, Default)]
pub struct GameplayDoc {
    /// Routing key / display name for HMR addressing.
    #[serde(default)]
    pub name: String,
    /// The deterministic gamemode rules script (Luau) this match runs, e.g.
    /// `"deathmatch"`. The Rust sim supplies primitives; the gamemode supplies
    /// rules (scoring, win condition, spawn selection) and must itself be
    /// lockstep-deterministic. Empty = the built-in default behavior.
    #[serde(default)]
    pub gamemode: String,
    /// Parameters the sim reads each tick (speeds, sizes, arena bounds).
    #[serde(default)]
    pub tunables: SimTunables,
    /// Spawn table the gamemode picks from (seeded from the level).
    #[serde(default)]
    pub spawns: Vec<LevelSpawn>,
}

impl GameplayDoc {
    pub fn from_json(bytes: &[u8]) -> anyhow::Result<Self> {
        Ok(serde_json::from_slice(bytes)?)
    }

    /// Seed a gameplay doc from a level: the level supplies the spawn table,
    /// the gamemode supplies default tunables. The two halves of the content
    /// model (level = static geometry, gameplay = live rules/knobs) compose
    /// here.
    pub fn from_level(level: &LevelDoc, gamemode: &str) -> Self {
        GameplayDoc {
            name: level.name.clone(),
            gamemode: gamemode.to_string(),
            tunables: SimTunables::default(),
            spawns: level.spawns.clone(),
        }
    }
}
