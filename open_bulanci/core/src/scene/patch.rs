//! Editor → engine **scene HMR** patch protocol.
//!
//! The studio's scene editor diffs successive document revisions and streams a
//! minimal list of [`PatchOp`]s, which [`super::host::SceneHost::apply_patch`]
//! applies *in place* to the live scene — preserving the Luau VM, scheduler,
//! runtime states, and every surviving `NodeId` (so scripts' cached handles
//! keep working). Only `.luau` script/lib code changes fall back to a full
//! host rebuild via the existing asset-overlay hot-reload path; everything in
//! the declarative doc (nodes, props, layout, states, bindings, viewport) is
//! a live patch — the Vite "edit markup = HMR, edit module = reload" split.
//!
//! Nodes are addressed by their stable editor `ControlNode.id`. The doc-tree
//! helpers here keep the host's retained [`SceneDoc`] authoritative so the
//! relayout pass + future diffs stay correct.

use serde::Deserialize;

use super::control::{Binding, ControlNode};
use super::document::StateInit;

/// One streamed HMR transaction for a single [`HmrSurface`](super::surface::HmrSurface).
///
/// The `surface` field is the routing key the client's surface router matches
/// against each live surface's [`surface_name`](super::surface::HmrSurface::surface_name)
/// — a scene name today. The legacy `scene` key is accepted as an alias so an
/// older editor build keeps working during the migration.
#[derive(Deserialize, Debug, Clone)]
pub struct ScenePatch {
    /// Target surface routing key — applied only when it matches a live
    /// surface. Accepts the legacy `scene` key as an alias.
    #[serde(alias = "scene")]
    pub surface: String,
    /// Monotonic editor revision (advisory; for logging / future ordering).
    #[serde(default)]
    pub rev: u64,
    pub ops: Vec<PatchOp>,
}

impl ScenePatch {
    pub fn from_json(bytes: &[u8]) -> anyhow::Result<Self> {
        Ok(serde_json::from_slice(bytes)?)
    }
}

/// Cheaply read just the routing key (`surface`, or the legacy `scene` alias)
/// from a raw patch envelope, *without* parsing its op vocabulary. The surface
/// router uses this so a surface only attempts a full typed parse when the
/// patch is actually addressed to it — letting heterogeneous surfaces (scene
/// ops vs. level/sim ops) coexist behind one router without mis-claiming each
/// other's patches. Unknown fields (`rev`, `ops`, …) are ignored.
pub fn peek_surface(bytes: &[u8]) -> Option<String> {
    #[derive(Deserialize)]
    struct Envelope {
        #[serde(alias = "scene")]
        surface: String,
    }
    serde_json::from_slice::<Envelope>(bytes).ok().map(|e| e.surface)
}

/// A single in-place edit. `node`/`bindings` payloads reuse the canonical
/// [`ControlNode`]/[`Binding`] serde types so the wire format matches the
/// on-disk `.scene.json` exactly (children on a `prop` payload are ignored —
/// structure is driven by `add`/`remove`/`reparent`).
#[derive(Deserialize, Debug, Clone)]
#[serde(tag = "op", rename_all = "lowercase")]
pub enum PatchOp {
    /// Update one control's scalar/visual props (kind fields, transform, tint,
    /// state tag, bindings). Children are ignored.
    Prop { id: String, node: Box<ControlNode> },
    /// Splice a new control subtree under `parent` ("" = root) at `index`.
    Add {
        #[serde(default)]
        parent: String,
        #[serde(default)]
        index: usize,
        node: Box<ControlNode>,
    },
    /// Remove a control subtree by id.
    Remove { id: String },
    /// Move a control under `parent` ("" = root) at `index` (draw order).
    Reparent {
        id: String,
        #[serde(default)]
        parent: String,
        #[serde(default)]
        index: usize,
    },
    /// Toggle a runtime conditional state on/off (live preview into the game).
    State { name: String, on: bool },
    /// Replace the declared states list (defaults + bit order). Re-seeds the
    /// scene's state-bit table deterministically and remasks nodes; the active
    /// set is preserved by name (runtime toggles survive a states edit).
    States { list: Vec<StateInit> },
    /// Replace one control's data bindings.
    Bindings {
        id: String,
        #[serde(default)]
        bindings: Vec<Binding>,
    },
    /// Change the design viewport `[w, h]` (triggers a relayout).
    Viewport { w: f32, h: f32 },
}

// ---------------------------------------------------------------------
// Doc-tree helpers — keep the host's retained control tree authoritative.
// All operate on the flat `Vec<ControlNode>` forest (top-level under ROOT).
// ---------------------------------------------------------------------

/// Find a control by stable id (immutable), depth-first.
pub fn doc_find<'a>(nodes: &'a [ControlNode], id: &str) -> Option<&'a ControlNode> {
    for n in nodes {
        if n.id == id {
            return Some(n);
        }
        if let Some(f) = doc_find(&n.children, id) {
            return Some(f);
        }
    }
    None
}

/// Find a control by stable id (mutable), depth-first.
pub fn doc_find_mut<'a>(nodes: &'a mut [ControlNode], id: &str) -> Option<&'a mut ControlNode> {
    for n in nodes.iter_mut() {
        if n.id == id {
            return Some(n);
        }
        if let Some(f) = doc_find_mut(&mut n.children, id) {
            return Some(f);
        }
    }
    None
}

/// Borrow the children vec of the control with id `id`.
fn doc_children_mut<'a>(nodes: &'a mut [ControlNode], id: &str) -> Option<&'a mut Vec<ControlNode>> {
    for n in nodes.iter_mut() {
        if n.id == id {
            return Some(&mut n.children);
        }
        if let Some(c) = doc_children_mut(&mut n.children, id) {
            return Some(c);
        }
    }
    None
}

/// Remove (and return) the control with id `id` from anywhere in the forest.
pub fn doc_remove(nodes: &mut Vec<ControlNode>, id: &str) -> Option<ControlNode> {
    if let Some(pos) = nodes.iter().position(|n| n.id == id) {
        return Some(nodes.remove(pos));
    }
    for n in nodes.iter_mut() {
        if let Some(found) = doc_remove(&mut n.children, id) {
            return Some(found);
        }
    }
    None
}

/// Insert `node` under `parent` ("" = top-level) at `index` (clamped).
/// Returns false if `parent` is a non-empty id that doesn't exist.
pub fn doc_insert(nodes: &mut Vec<ControlNode>, parent: &str, index: usize, node: ControlNode) -> bool {
    if parent.is_empty() {
        nodes.insert(index.min(nodes.len()), node);
        return true;
    }
    if let Some(children) = doc_children_mut(nodes, parent) {
        children.insert(index.min(children.len()), node);
        true
    } else {
        false
    }
}

/// Collect every control id in the subtree rooted at `node` (inclusive).
pub fn doc_collect_ids(node: &ControlNode, out: &mut Vec<String>) {
    if !node.id.is_empty() {
        out.push(node.id.clone());
    }
    for c in &node.children {
        doc_collect_ids(c, out);
    }
}

/// Copy a control's scalar props from `src` to `dst`, keeping `dst`'s id and
/// children (structure is driven by add/remove/reparent, not prop edits).
pub fn copy_props(dst: &mut ControlNode, src: &ControlNode) {
    dst.name = src.name.clone();
    dst.kind = src.kind.clone();
    dst.layout = src.layout.clone();
    dst.sx = src.sx;
    dst.sy = src.sy;
    dst.rot = src.rot;
    dst.state = src.state.clone();
    dst.tint = src.tint.clone();
    dst.handle = src.handle.clone();
    dst.frame = src.frame;
    dst.text = src.text.clone();
    dst.size = src.size;
    dst.spacing = src.spacing;
    dst.anchor = src.anchor.clone();
    dst.color = src.color.clone();
    dst.label = src.label.clone();
    dst.bindings = src.bindings.clone();
}
