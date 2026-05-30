//! [`LevelSurface`] — a declarative level wrapped as an [`HmrSurface`].
//!
//! This is the level peer of [`SceneManager`](crate::scene::SceneManager): it
//! holds the live [`LevelDoc`] behind a `RefCell` (interior mutability so the
//! editor patches through `&self`, matching the scene host's HMR contract) plus
//! the VFS dep paths that, when their bytes change, force a from-scratch reload
//! instead of an in-place patch. The gameplay sim reads [`LevelSurface::doc`]
//! to seed obstacles/spawns and to look up tunables; the editor mutates it
//! through [`HmrSurface::apply_raw_patch`].

use std::cell::{Ref, RefCell};

use crate::scene::patch::peek_surface;
use crate::scene::{HmrSurface, PatchOutcome};

use super::doc::LevelDoc;
use super::patch::{LevelOp, LevelPatch};

/// A hot-patchable level surface.
pub struct LevelSurface {
    /// Routing key (the level name). Stable across patches and kept outside the
    /// `RefCell` so [`surface_name`](HmrSurface::surface_name) can hand back a
    /// borrow that outlives a transient doc borrow.
    name: String,
    doc: RefCell<LevelDoc>,
    deps: Vec<String>,
}

impl LevelSurface {
    /// Wrap an already-parsed doc. `deps` are the VFS paths to watch for
    /// rebuilds (typically just `levels/<name>.level.json`).
    pub fn new(doc: LevelDoc, deps: Vec<String>) -> Self {
        Self {
            name: doc.name.clone(),
            doc: RefCell::new(doc),
            deps,
        }
    }

    /// Load `levels/<name>.level.json` through a VFS reader (same reader shape
    /// [`SceneManager::load`](crate::scene::SceneManager) uses). Returns `None`
    /// when the doc is missing or malformed.
    pub fn load(name: &str, read: &dyn Fn(&str) -> Option<Vec<u8>>) -> Option<Self> {
        let path = format!("levels/{name}.level.json");
        let bytes = read(&path)?;
        let mut doc = match LevelDoc::from_json(&bytes) {
            Ok(d) => d,
            Err(e) => {
                eprintln!("[level] {path}: parse failed: {e}");
                return None;
            }
        };
        // The doc's own name is authoritative for routing, but default it from
        // the requested name when the author left it blank.
        if doc.name.is_empty() {
            doc.name = name.to_string();
        }
        Some(Self {
            name: doc.name.clone(),
            doc: RefCell::new(doc),
            deps: vec![path],
        })
    }

    /// Borrow the live doc (read-only) — what the gameplay sim reads to seed
    /// obstacles/spawns and to look up tunables each tick.
    pub fn doc(&self) -> Ref<'_, LevelDoc> {
        self.doc.borrow()
    }

    /// Apply a typed level patch in place. Returns `false` when the patch is
    /// addressed to a different level (callers normally route through
    /// [`apply_raw_patch`](HmrSurface::apply_raw_patch)).
    pub fn apply(&self, patch: &LevelPatch) -> bool {
        if patch.surface != self.name {
            return false;
        }
        let mut doc = self.doc.borrow_mut();
        for op in &patch.ops {
            apply_op(&mut doc, op);
        }
        true
    }
}

/// Apply one op to the doc. Adds upsert by `id` (an empty `id` always appends,
/// since a blank id can't address an existing entry).
fn apply_op(doc: &mut LevelDoc, op: &LevelOp) {
    match op {
        LevelOp::AddObstacle { obstacle } => {
            match doc
                .obstacles
                .iter_mut()
                .find(|o| !o.id.is_empty() && o.id == obstacle.id)
            {
                Some(existing) => *existing = obstacle.clone(),
                None => doc.obstacles.push(obstacle.clone()),
            }
        }
        LevelOp::RemoveObstacle { id } => doc.obstacles.retain(|o| o.id != *id),
        LevelOp::SetObstacle { id, x1, y1, x2, y2 } => {
            if let Some(o) = doc.obstacles.iter_mut().find(|o| o.id == *id) {
                o.x1 = *x1;
                o.y1 = *y1;
                o.x2 = *x2;
                o.y2 = *y2;
            }
        }
        LevelOp::AddSpawn { spawn } => {
            match doc
                .spawns
                .iter_mut()
                .find(|s| !s.id.is_empty() && s.id == spawn.id)
            {
                Some(existing) => *existing = spawn.clone(),
                None => doc.spawns.push(spawn.clone()),
            }
        }
        LevelOp::RemoveSpawn { id } => doc.spawns.retain(|s| s.id != *id),
        LevelOp::SetSpawn { id, x, y, team } => {
            if let Some(s) = doc.spawns.iter_mut().find(|s| s.id == *id) {
                s.x = *x;
                s.y = *y;
                s.team = *team;
            }
        }
        LevelOp::SetParam { key, value } => {
            doc.params.insert(key.clone(), *value);
        }
        LevelOp::RemoveParam { key } => {
            doc.params.remove(key);
        }
    }
}

impl HmrSurface for LevelSurface {
    fn surface_name(&self) -> &str {
        &self.name
    }

    fn deps(&self) -> &[String] {
        &self.deps
    }

    fn apply_raw_patch(&self, raw: &[u8]) -> PatchOutcome {
        // Peek the envelope's routing key first so a patch addressed to a
        // *scene* (or any other surface) is cleanly passed on rather than
        // failing to parse as a LevelOp — the contract that lets heterogeneous
        // surfaces share one router.
        match peek_surface(raw) {
            Some(s) if s == self.name => {}
            Some(_) => return PatchOutcome::NotMine,
            None => return PatchOutcome::Failed,
        }
        match LevelPatch::from_json(raw) {
            Ok(patch) => {
                if self.apply(&patch) {
                    PatchOutcome::Applied
                } else {
                    PatchOutcome::NotMine
                }
            }
            Err(e) => {
                eprintln!("[level-patch] parse error: {e}");
                PatchOutcome::Failed
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::assets::hash_path;
    use crate::level::doc::{LevelObstacle, LevelSpawn};
    use std::collections::HashSet;

    const ARENA: &str = r#"{
        "name": "arena",
        "obstacles": [
            { "id": "wall_n", "x1": 0, "y1": 0, "x2": 800, "y2": 16 }
        ],
        "spawns": [
            { "id": "p1", "x": 100, "y": 100, "team": 0 }
        ],
        "params": { "time_limit": 180 }
    }"#;

    fn arena() -> LevelSurface {
        let read = |p: &str| (p == "levels/arena.level.json").then(|| ARENA.as_bytes().to_vec());
        LevelSurface::load("arena", &read).expect("arena loads")
    }

    #[test]
    fn loads_doc_and_records_dep_path() {
        let lvl = arena();
        assert_eq!(lvl.surface_name(), "arena");
        assert_eq!(lvl.deps(), &["levels/arena.level.json".to_string()]);
        let doc = lvl.doc();
        assert_eq!(doc.obstacles.len(), 1);
        assert_eq!(doc.spawns[0].x, 100);
        assert_eq!(doc.params.get("time_limit"), Some(&180));
    }

    #[test]
    fn patch_adds_moves_and_removes_obstacles_by_id() {
        let lvl = arena();

        // Add a new obstacle; upsert the existing one by id; tweak via set.
        let add = r#"{ "surface":"arena", "ops":[
            { "op":"add_obstacle", "obstacle": { "id":"wall_s", "x1":0,"y1":584,"x2":800,"y2":600 } },
            { "op":"set_obstacle", "id":"wall_n", "x1":0,"y1":0,"x2":800,"y2":32 }
        ] }"#;
        assert_eq!(lvl.apply_raw_patch(add.as_bytes()), PatchOutcome::Applied);
        {
            let doc = lvl.doc();
            assert_eq!(doc.obstacles.len(), 2);
            let n = doc.obstacles.iter().find(|o| o.id == "wall_n").unwrap();
            assert_eq!(n.y2, 32);
        }

        // Re-adding wall_s by the same id replaces (upsert), never duplicates.
        let readd = r#"{ "surface":"arena", "ops":[
            { "op":"add_obstacle", "obstacle": { "id":"wall_s", "x1":0,"y1":580,"x2":800,"y2":600 } }
        ] }"#;
        assert_eq!(lvl.apply_raw_patch(readd.as_bytes()), PatchOutcome::Applied);
        assert_eq!(lvl.doc().obstacles.len(), 2);

        // Remove by id.
        let rm = r#"{ "surface":"arena", "ops":[ { "op":"remove_obstacle", "id":"wall_n" } ] }"#;
        assert_eq!(lvl.apply_raw_patch(rm.as_bytes()), PatchOutcome::Applied);
        let doc = lvl.doc();
        assert_eq!(doc.obstacles.len(), 1);
        assert_eq!(doc.obstacles[0].id, "wall_s");
    }

    #[test]
    fn patch_edits_spawns_and_params() {
        let lvl = arena();
        let patch = r#"{ "surface":"arena", "ops":[
            { "op":"set_spawn", "id":"p1", "x":120, "y":140, "team":1 },
            { "op":"add_spawn", "spawn": { "id":"p2", "x":700, "y":500, "team":2 } },
            { "op":"set_param", "key":"time_limit", "value":120 },
            { "op":"set_param", "key":"score_cap", "value":10 },
            { "op":"remove_param", "key":"time_limit" }
        ] }"#;
        assert_eq!(lvl.apply_raw_patch(patch.as_bytes()), PatchOutcome::Applied);
        let doc = lvl.doc();
        assert_eq!(doc.spawns.len(), 2);
        let p1 = doc.spawns.iter().find(|s| s.id == "p1").unwrap();
        assert_eq!((p1.x, p1.y, p1.team), (120, 140, 1));
        assert_eq!(doc.params.get("score_cap"), Some(&10));
        assert!(doc.params.get("time_limit").is_none());
    }

    #[test]
    fn foreign_surface_patch_is_not_mine_not_failed() {
        let lvl = arena();
        // A patch addressed to a *scene* must be passed on, not mis-parsed —
        // even though its ops are a completely different vocabulary.
        let scene_patch = r#"{ "surface":"main_menu", "rev":3, "ops":[
            { "op":"set_text", "id":"title", "text":"hi" } ] }"#;
        assert_eq!(
            lvl.apply_raw_patch(scene_patch.as_bytes()),
            PatchOutcome::NotMine
        );
    }

    #[test]
    fn malformed_payload_fails() {
        let lvl = arena();
        // Right surface name, but ops it can't decode -> Failed (caller may
        // fall back to a rebuild), distinct from NotMine.
        let bad = r#"{ "surface":"arena", "ops":[ { "op":"nonsense" } ] }"#;
        assert_eq!(lvl.apply_raw_patch(bad.as_bytes()), PatchOutcome::Failed);
        // Not even JSON.
        assert_eq!(lvl.apply_raw_patch(b"{ not json"), PatchOutcome::Failed);
    }

    #[test]
    fn needs_rebuild_tracks_dep_path_hash() {
        let lvl = arena();
        let dirty: HashSet<u64> = [hash_path("levels/arena.level.json")].into_iter().collect();
        assert!(lvl.needs_rebuild(&dirty));
        let other: HashSet<u64> = [hash_path("levels/other.level.json")].into_iter().collect();
        assert!(!lvl.needs_rebuild(&other));
    }

    #[test]
    fn new_from_doc_uses_doc_name() {
        let doc = LevelDoc {
            name: "inline".into(),
            obstacles: vec![LevelObstacle { id: "a".into(), x1: 0, y1: 0, x2: 1, y2: 1 }],
            spawns: vec![LevelSpawn { id: "s".into(), x: 5, y: 6, team: 0 }],
            ..Default::default()
        };
        let lvl = LevelSurface::new(doc, vec!["levels/inline.level.json".into()]);
        assert_eq!(lvl.surface_name(), "inline");
        assert_eq!(lvl.doc().obstacles[0].id, "a");
    }
}
