//! The declarative **level document** — sim-adjacent content, decoupled from
//! rendering.
//!
//! Where a [`SceneDoc`](crate::scene::SceneDoc) is a UI control tree fed to the
//! renderer, a [`LevelDoc`] is the data the *deterministic gameplay simulation*
//! consumes: static collision obstacles, spawn points, and gamemode tunables.
//! It uses the same integer pixel space as [`MapObstacle`](crate::MapObstacle)
//! / [`Position`](crate::Position) so a level can seed an
//! [`EngineSimulation`](crate::engine::EngineSimulation) directly, and every
//! tunable is integer-valued to stay lockstep-deterministic (no float drift
//! across peers).

use std::collections::BTreeMap;

use serde::{Deserialize, Serialize};

/// A static collision rectangle in engine pixels (integer, like the sim's
/// [`MapObstacle`](crate::MapObstacle)). `id` is the stable HMR address — the
/// level analogue of [`ControlNode.id`](crate::scene::ControlNode) — so a patch
/// can move or remove it regardless of position in the list.
#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, Eq, Default)]
pub struct LevelObstacle {
    #[serde(default)]
    pub id: String,
    pub x1: i32,
    pub y1: i32,
    pub x2: i32,
    pub y2: i32,
}

/// A player/entity spawn point with a team tag, in the same integer pixel
/// space as the sim's [`Position`](crate::Position).
#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, Eq, Default)]
pub struct LevelSpawn {
    #[serde(default)]
    pub id: String,
    pub x: i32,
    pub y: i32,
    #[serde(default)]
    pub team: u8,
}

/// Declarative level content: collision obstacles, spawn points, and numeric
/// gamemode tunables. Authored as `levels/<name>.level.json` and addressed by
/// that literal path (the same path-hash mounting `scenes/*.scene.json` uses).
///
/// A level owns its own patch vocabulary ([`LevelOp`](super::patch::LevelOp))
/// rather than scene [`PatchOp`](crate::scene::PatchOp)s, because it mutates sim
/// data, not a layout tree — but it still rides the shared
/// [`HmrSurface`](crate::scene::HmrSurface) router.
#[derive(Serialize, Deserialize, Clone, Debug, PartialEq, Eq, Default)]
pub struct LevelDoc {
    /// Routing key / display name. Authoritative for HMR addressing; defaults
    /// from the requested name on load when left blank.
    #[serde(default)]
    pub name: String,
    #[serde(default)]
    pub obstacles: Vec<LevelObstacle>,
    #[serde(default)]
    pub spawns: Vec<LevelSpawn>,
    /// Gamemode tunables (gravity, time limit, score cap, …) addressed by key.
    /// Integer-valued so live edits stay lockstep-deterministic across peers.
    #[serde(default)]
    pub params: BTreeMap<String, i64>,
}

impl LevelDoc {
    /// Parse a `*.level.json` byte payload.
    pub fn from_json(bytes: &[u8]) -> anyhow::Result<Self> {
        Ok(serde_json::from_slice(bytes)?)
    }
}
