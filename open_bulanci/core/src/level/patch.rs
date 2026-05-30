//! The **level patch** wire vocabulary — the editor→engine ops that mutate a
//! live [`LevelDoc`](super::doc::LevelDoc) in place.
//!
//! Structurally this mirrors [`ScenePatch`](crate::scene::ScenePatch): the same
//! `{ surface, rev, ops }` envelope (with the legacy `scene` alias), so the
//! client's surface router can hand the very same raw payload to a level
//! surface or a scene manager and let each claim only what it understands. The
//! `ops` differ: a level edits sim data (obstacles / spawns / tunables) rather
//! than a control tree, and every op addresses content by stable `id`/`key` so
//! an edit survives reordering — the same identity contract scene ops hold
//! against [`ControlNode.id`](crate::scene::ControlNode).

use serde::Deserialize;

use super::doc::{LevelObstacle, LevelSpawn};

/// Editor→engine envelope for a *level* surface.
#[derive(Deserialize, Debug, Clone)]
pub struct LevelPatch {
    #[serde(alias = "scene")]
    pub surface: String,
    #[serde(default)]
    pub rev: u64,
    pub ops: Vec<LevelOp>,
}

impl LevelPatch {
    pub fn from_json(bytes: &[u8]) -> anyhow::Result<Self> {
        Ok(serde_json::from_slice(bytes)?)
    }
}

/// In-place mutations to a [`LevelDoc`](super::doc::LevelDoc). Adds are
/// upsert-by-`id` (a fresh `id` inserts; an existing one replaces), so the
/// editor can re-emit a node idempotently.
#[derive(Deserialize, Debug, Clone, PartialEq, Eq)]
#[serde(tag = "op", rename_all = "snake_case")]
pub enum LevelOp {
    /// Insert an obstacle, or replace the one already carrying `obstacle.id`.
    AddObstacle { obstacle: LevelObstacle },
    /// Drop the obstacle with this id.
    RemoveObstacle { id: String },
    /// Move/resize an existing obstacle in place.
    SetObstacle {
        id: String,
        x1: i32,
        y1: i32,
        x2: i32,
        y2: i32,
    },
    /// Insert a spawn, or replace the one already carrying `spawn.id`.
    AddSpawn { spawn: LevelSpawn },
    /// Drop the spawn with this id.
    RemoveSpawn { id: String },
    /// Move an existing spawn / retag its team in place.
    SetSpawn {
        id: String,
        x: i32,
        y: i32,
        #[serde(default)]
        team: u8,
    },
    /// Set (or insert) a tunable parameter.
    SetParam { key: String, value: i64 },
    /// Remove a tunable parameter.
    RemoveParam { key: String },
}
