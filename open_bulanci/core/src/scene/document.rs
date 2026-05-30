//! The scene document — the on-disk, editor-authored description of a
//! scene: a name, the declarative control tree, the attached Luau scripts
//! ("code-behind"), and the initial conditional-state values.
//!
//! Stored as clean JSON (`scenes/<name>.scene.json`) via `serde` so the
//! editor and humans can author it directly. The control types also derive
//! [`bevy_reflect::Reflect`] (see [`super::control`]) so the editor's
//! property grid is generated from the same definitions via
//! [`super::registry`] — the doc IO uses `serde` for a compact format while
//! reflection powers the type metadata, not a second hand-kept schema.

use bevy_reflect::Reflect;
use serde::{Deserialize, Serialize};

use super::control::{build_into, BuiltScene, ControlNode, DEFAULT_VIEWPORT};
use super::graph::Scene;

/// One initial conditional-state value. Order is preserved so the state-bit
/// allocation is deterministic across loads.
#[derive(Reflect, Serialize, Deserialize, Clone, Debug, Default)]
#[serde(default)]
pub struct StateInit {
    pub name: String,
    pub on: bool,
}

/// A complete scene definition. `scripts` are VFS paths to Luau modules the
/// host loads and runs after building the tree (in order); they drive the
/// named controls via the `bulanci.*` API.
#[derive(Reflect, Serialize, Deserialize, Clone, Debug, Default)]
#[serde(default)]
pub struct SceneDoc {
    /// Scene id (matches the pak `scenes` table key / `scenes/<name>`).
    pub name: String,
    /// Attached Luau script paths, run in order after the tree is built.
    pub scripts: Vec<String>,
    /// Initial conditional states (allocated + set before scripts run).
    pub states: Vec<StateInit>,
    /// Optional design viewport `[w, h]`; defaults to 800x600.
    pub viewport: Option<[f32; 2]>,
    /// The declarative control tree (top-level controls under ROOT).
    pub root: Vec<ControlNode>,
}

impl SceneDoc {
    /// Parse a `.scene.json` document.
    pub fn from_json(bytes: &[u8]) -> anyhow::Result<Self> {
        Ok(serde_json::from_slice(bytes)?)
    }

    /// Serialize to pretty JSON (what the editor writes back).
    pub fn to_json(&self) -> anyhow::Result<String> {
        Ok(serde_json::to_string_pretty(self)?)
    }

    pub fn viewport(&self) -> (f32, f32) {
        self.viewport.map(|v| (v[0], v[1])).unwrap_or(DEFAULT_VIEWPORT)
    }

    /// Set the initial states then build the control tree into `scene`,
    /// returning the name index + bindings for the host to drive.
    pub fn instantiate(&self, scene: &mut Scene) -> BuiltScene {
        for st in &self.states {
            scene.set_state(&st.name, st.on);
        }
        build_into(scene, &self.root, self.viewport())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    const DOC: &str = r#"
    {
      "name": "demo",
      "scripts": ["scenes/demo/logic.luau"],
      "states": [{ "name": "main", "on": true }],
      "root": [
        { "kind": "sprite", "name": "bg", "handle": "menu/backgrounds/bg_menu" },
        { "kind": "text", "name": "title", "text": "Hi", "size": 24,
          "layout": { "x": 40, "y": 48 } }
      ]
    }
    "#;

    #[test]
    fn round_trips_json_and_instantiates() {
        let doc = SceneDoc::from_json(DOC.as_bytes()).unwrap();
        assert_eq!(doc.name, "demo");
        assert_eq!(doc.scripts, vec!["scenes/demo/logic.luau".to_string()]);
        assert_eq!(doc.root.len(), 2);

        let mut scene = Scene::new();
        let built = doc.instantiate(&mut scene);
        assert!(scene.is_state("main"));
        assert!(built.names.contains_key("bg"));
        assert!(built.names.contains_key("title"));

        // Re-serialize and re-parse: stable.
        let json = doc.to_json().unwrap();
        let doc2 = SceneDoc::from_json(json.as_bytes()).unwrap();
        assert_eq!(doc2.root.len(), 2);
    }
}
