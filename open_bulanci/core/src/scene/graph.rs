//! The scene graph: a flat arena of nodes with parent/child links,
//! transforms, primitive draw kinds, visibility, and conditional state
//! masks. This is pure data + logic — **no rendering, no Lua, no
//! raylib**. The client walks [`Scene::draw_list`] to render through its
//! atlas/font stack, and feeds pointer input back via [`Scene::hit_test`].
//!
//! Higher-level widgets (button, frame, list) are *not* here — they are
//! composed in Luau out of these primitives (see the scene host + the
//! base UI pak). Keeping the Rust footprint to primitives is the whole
//! point: the engine stays small and moddable, behavior lives in script.

use std::collections::HashMap;

/// Arena index of a node. Stable for the node's lifetime; freed slots are
/// reused, so don't hold an id past a `remove`.
pub type NodeId = u32;

/// The implicit root group, created by [`Scene::new`]. Everything attaches
/// under it (directly or transitively).
pub const ROOT: NodeId = 0;

/// Local 2D transform. Composition is translate + (non-uniform) scale +
/// additive rotation — enough for menu/UI layout; the renderer applies it.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Transform {
    pub x: f32,
    pub y: f32,
    pub sx: f32,
    pub sy: f32,
    pub rot: f32,
}

impl Default for Transform {
    fn default() -> Self {
        Transform { x: 0.0, y: 0.0, sx: 1.0, sy: 1.0, rot: 0.0 }
    }
}

impl Transform {
    pub fn at(x: f32, y: f32) -> Self {
        Transform { x, y, ..Default::default() }
    }

    /// `self` interpreted in the space of `parent` -> a world transform.
    /// Translation is scaled by the parent scale; scales multiply; rotation
    /// adds. (No shear — fine for the UI use case.)
    pub fn compose(parent: &Transform, local: &Transform) -> Transform {
        Transform {
            x: parent.x + local.x * parent.sx,
            y: parent.y + local.y * parent.sy,
            sx: parent.sx * local.sx,
            sy: parent.sy * local.sy,
            rot: parent.rot + local.rot,
        }
    }
}

/// Vertical anchor for a [`NodeKind::Text`] node. `Baseline` matches the
/// engine's `draw_t` convention (y = text baseline); `Top` matches the
/// poem's `draw_cell_top` convention (y = top of the cell box).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum TextAnchor {
    Baseline,
    Top,
}

impl TextAnchor {
    pub fn from_str(s: &str) -> Option<Self> {
        match s {
            "baseline" => Some(TextAnchor::Baseline),
            "top" => Some(TextAnchor::Top),
            _ => None,
        }
    }
}

/// What a node draws. Asset references are 64-bit handle hashes (the same
/// keys the VFS/paks use) — never names — preserving the no-leak invariant.
#[derive(Clone, Debug, PartialEq)]
pub enum NodeKind {
    /// Pure container — no pixels of its own.
    Group,
    /// A sprite/image addressed by handle hash, optional atlas frame.
    Sprite { handle: u64, frame: u32 },
    /// A run of bitmap text. The font is chosen by `size` exactly like the
    /// engine's `draw_t` (`pick(size)`), so the menu's font sizes map 1:1.
    /// `spacing` is the inter-glyph char-spacing; `anchor` selects the
    /// baseline/cell-top y convention.
    Text { text: String, size: f32, spacing: i32, anchor: TextAnchor },
    /// A solid rectangle (RGBA8 packed 0xRRGGBBAA).
    Quad { w: f32, h: f32, color: u32 },
    /// A filled circle of `radius` (world units, scaled by `world.sx`) centred
    /// on the node's world origin. RGBA8 color. A gameplay/shape primitive
    /// (players, bullets) so the sim composes in the same draw list as scenes.
    Circle { radius: f32, color: u32 },
    /// A line segment from the node's world origin to a local offset
    /// `(dx, dy)` (scaled by world scale), `thickness` px, RGBA8 color. A
    /// gameplay/shape primitive (e.g. the facing indicator).
    Line { dx: f32, dy: f32, thickness: f32, color: u32 },
    /// An invisible interactive rectangle. `events` is a bitset of the
    /// [`InputEvent`] kinds a behavior subscribed to (so the renderer can
    /// optionally visualize hot regions in the editor).
    InputRegion { w: f32, h: f32, events: u8 },
}

/// Pointer event kinds an [`NodeKind::InputRegion`] can fire.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub enum InputEvent {
    Hover,
    Press,
    Release,
    Click,
    /// Pointer left the region (hover -> not-this-region). Lets a behavior
    /// restore its idle look (e.g. dial dim-down) — the bespoke menu needs
    /// this and the old engine only fired enter.
    Leave,
}

impl InputEvent {
    pub fn bit(self) -> u8 {
        match self {
            InputEvent::Hover => 1,
            InputEvent::Press => 2,
            InputEvent::Release => 4,
            InputEvent::Click => 8,
            InputEvent::Leave => 16,
        }
    }

    pub fn from_str(s: &str) -> Option<Self> {
        match s {
            "hover" => Some(InputEvent::Hover),
            "press" => Some(InputEvent::Press),
            "release" => Some(InputEvent::Release),
            "click" => Some(InputEvent::Click),
            "leave" => Some(InputEvent::Leave),
            _ => None,
        }
    }

    pub fn name(self) -> &'static str {
        match self {
            InputEvent::Hover => "hover",
            InputEvent::Press => "press",
            InputEvent::Release => "release",
            InputEvent::Click => "click",
            InputEvent::Leave => "leave",
        }
    }
}

#[derive(Clone, Debug)]
pub struct Node {
    pub parent: Option<NodeId>,
    pub children: Vec<NodeId>,
    pub transform: Transform,
    pub kind: NodeKind,
    pub visible: bool,
    /// Conditional-state bitmask. `0` = always visible; otherwise the node
    /// shows only while at least one of its bits is in the active state set.
    pub state_mask: u64,
    /// RGBA8 tint applied to Sprite/Text (0xFFFFFFFF = untinted).
    pub tint: u32,
    /// Dev-only label for the editor/tree (never required at runtime).
    pub name: Option<String>,
}

impl Node {
    fn new(parent: Option<NodeId>, kind: NodeKind) -> Self {
        Node {
            parent,
            children: Vec::new(),
            transform: Transform::default(),
            kind,
            visible: true,
            state_mask: 0,
            tint: 0xFFFF_FFFF,
            name: None,
        }
    }
}

/// One resolved entry in the draw list: a node with its **world** transform
/// already accumulated, ready to render. The renderer matches on `kind`.
#[derive(Clone, Debug)]
pub struct DrawItem {
    pub id: NodeId,
    pub world: Transform,
    pub kind: NodeKind,
    pub tint: u32,
}

/// The scene graph instance.
pub struct Scene {
    nodes: Vec<Option<Node>>,
    free: Vec<usize>,
    state_names: Vec<String>,
    state_bits: HashMap<String, u64>,
    active_states: u64,
}

impl Default for Scene {
    fn default() -> Self {
        Self::new()
    }
}

impl Scene {
    pub fn new() -> Self {
        let mut s = Scene {
            nodes: Vec::new(),
            free: Vec::new(),
            state_names: Vec::new(),
            state_bits: HashMap::new(),
            active_states: 0,
        };
        // Slot 0 is always the root group.
        s.nodes.push(Some(Node::new(None, NodeKind::Group)));
        s
    }

    // ---- node lifecycle ------------------------------------------------

    pub fn create(&mut self, parent: NodeId, kind: NodeKind) -> NodeId {
        let node = Node::new(Some(parent), kind);
        let id = if let Some(slot) = self.free.pop() {
            self.nodes[slot] = Some(node);
            slot as NodeId
        } else {
            self.nodes.push(Some(node));
            (self.nodes.len() - 1) as NodeId
        };
        if let Some(p) = self.node_mut(parent) {
            p.children.push(id);
        }
        id
    }

    /// Remove a node and its whole subtree.
    pub fn remove(&mut self, id: NodeId) {
        if id == ROOT {
            return;
        }
        // Detach from parent.
        if let Some(parent) = self.node(id).and_then(|n| n.parent) {
            if let Some(p) = self.node_mut(parent) {
                p.children.retain(|&c| c != id);
            }
        }
        self.remove_subtree(id);
    }

    fn remove_subtree(&mut self, id: NodeId) {
        let children = match self.node(id) {
            Some(n) => n.children.clone(),
            None => return,
        };
        for c in children {
            self.remove_subtree(c);
        }
        self.nodes[id as usize] = None;
        self.free.push(id as usize);
    }

    /// Create a node as the `index`-th child of `parent` (clamped). Like
    /// [`Scene::create`] but honors draw order, so an editor HMR `add` op can
    /// splice a node between existing siblings instead of always appending.
    pub fn create_at(&mut self, parent: NodeId, index: usize, kind: NodeKind) -> NodeId {
        let id = self.create(parent, kind);
        if let Some(p) = self.node_mut(parent) {
            // `create` pushed `id` to the end; move it to `index`.
            if let Some(pos) = p.children.iter().position(|&c| c == id) {
                let i = index.min(p.children.len().saturating_sub(1));
                if pos != i {
                    let v = p.children.remove(pos);
                    p.children.insert(i, v);
                }
            }
        }
        id
    }

    pub fn reparent(&mut self, id: NodeId, new_parent: NodeId) {
        if id == ROOT || id == new_parent {
            return;
        }
        if let Some(old) = self.node(id).and_then(|n| n.parent) {
            if let Some(p) = self.node_mut(old) {
                p.children.retain(|&c| c != id);
            }
        }
        if let Some(n) = self.node_mut(id) {
            n.parent = Some(new_parent);
        }
        if let Some(p) = self.node_mut(new_parent) {
            p.children.push(id);
        }
    }

    /// Reorder `parent`'s children to match `order` (painter's/draw order).
    /// Ids in `order` that aren't current children are skipped; current
    /// children missing from `order` (e.g. script-created nodes) are appended
    /// after, preserving their relative order. Used by HMR relayout to make
    /// the live tree match the editor's child ordering after add/reparent.
    pub fn reorder_children(&mut self, parent: NodeId, order: &[NodeId]) {
        if let Some(p) = self.node_mut(parent) {
            let mut next: Vec<NodeId> = Vec::with_capacity(p.children.len());
            for &want in order {
                if p.children.contains(&want) && !next.contains(&want) {
                    next.push(want);
                }
            }
            for &c in &p.children {
                if !next.contains(&c) {
                    next.push(c);
                }
            }
            p.children = next;
        }
    }

    /// Collect every node id in the subtree rooted at `id` (inclusive),
    /// depth-first. Used before a remove so the host can prune name /
    /// binding / input-handler entries that point into the doomed subtree.
    pub fn subtree_ids(&self, id: NodeId) -> Vec<NodeId> {
        let mut out = Vec::new();
        self.collect_ids(id, &mut out);
        out
    }

    fn collect_ids(&self, id: NodeId, out: &mut Vec<NodeId>) {
        if let Some(n) = self.node(id) {
            out.push(id);
            for &c in &n.children {
                self.collect_ids(c, out);
            }
        }
    }

    pub fn exists(&self, id: NodeId) -> bool {
        self.nodes.get(id as usize).map(|s| s.is_some()).unwrap_or(false)
    }

    pub fn node(&self, id: NodeId) -> Option<&Node> {
        self.nodes.get(id as usize).and_then(|s| s.as_ref())
    }

    pub fn node_mut(&mut self, id: NodeId) -> Option<&mut Node> {
        self.nodes.get_mut(id as usize).and_then(|s| s.as_mut())
    }

    pub fn node_count(&self) -> usize {
        self.nodes.iter().filter(|s| s.is_some()).count()
    }

    // ---- conditional states --------------------------------------------

    /// Allocate (or fetch) the bit for a named state. Up to 64 states.
    pub fn state_bit(&mut self, name: &str) -> u64 {
        if let Some(&b) = self.state_bits.get(name) {
            return b;
        }
        let idx = self.state_names.len();
        let bit = if idx < 64 { 1u64 << idx } else { 0 };
        self.state_names.push(name.to_string());
        self.state_bits.insert(name.to_string(), bit);
        bit
    }

    pub fn set_state(&mut self, name: &str, on: bool) {
        let bit = self.state_bit(name);
        if on {
            self.active_states |= bit;
        } else {
            self.active_states &= !bit;
        }
    }

    pub fn is_state(&self, name: &str) -> bool {
        self.state_bits.get(name).map(|b| self.active_states & b != 0).unwrap_or(false)
    }

    /// Rebuild the name/bit tables from `names` (the doc's declared state
    /// order), preserving which states are active *by name*. The editor's HMR
    /// uses this when the states list is edited so bit assignment stays
    /// deterministic (matching a fresh load) instead of leaking bits across a
    /// long session of add/rename/delete.
    pub fn reset_states(&mut self, names: &[String]) {
        let active: Vec<String> =
            self.state_names.iter().filter(|n| self.is_state(n)).cloned().collect();
        self.state_names.clear();
        self.state_bits.clear();
        self.active_states = 0;
        for n in names {
            self.state_bit(n);
        }
        for n in &active {
            if self.state_bits.contains_key(n) {
                self.set_state(n, true);
            }
        }
    }

    pub fn active_states(&self) -> u64 {
        self.active_states
    }

    fn state_visible(&self, n: &Node) -> bool {
        n.state_mask == 0 || (n.state_mask & self.active_states) != 0
    }

    // ---- queries -------------------------------------------------------

    /// Accumulated world transform of a node (root-relative).
    pub fn world_of(&self, id: NodeId) -> Transform {
        match self.node(id) {
            None => Transform::default(),
            Some(n) => match n.parent {
                None => n.transform,
                Some(p) => Transform::compose(&self.world_of(p), &n.transform),
            },
        }
    }

    /// Flatten the visible tree into painter's-order draw items. A node is
    /// skipped (with its subtree) when it's hidden or its state mask isn't
    /// active. Groups/empty-input regions produce no `DrawItem` of their
    /// own but still propagate their transform to children.
    pub fn draw_list(&self) -> Vec<DrawItem> {
        let mut out = Vec::new();
        self.collect(ROOT, &Transform::default(), &mut out);
        out
    }

    fn collect(&self, id: NodeId, parent_world: &Transform, out: &mut Vec<DrawItem>) {
        let n = match self.node(id) {
            Some(n) => n,
            None => return,
        };
        if !n.visible || !self.state_visible(n) {
            return;
        }
        let world = Transform::compose(parent_world, &n.transform);
        if !matches!(n.kind, NodeKind::Group | NodeKind::InputRegion { .. }) {
            out.push(DrawItem { id, world, kind: n.kind.clone(), tint: n.tint });
        }
        for &c in &n.children {
            self.collect(c, &world, out);
        }
    }

    /// Topmost visible [`NodeKind::InputRegion`] containing point `(x, y)`
    /// in world space, or `None`. Honors visibility + state masks of the
    /// region and its ancestors.
    pub fn hit_test(&self, x: f32, y: f32) -> Option<NodeId> {
        let mut hits = Vec::new();
        self.collect_regions(ROOT, &Transform::default(), &mut hits);
        // Last in painter order = topmost.
        for &(id, ref world, w, h) in hits.iter().rev() {
            if x >= world.x && y >= world.y && x < world.x + w * world.sx && y < world.y + h * world.sy {
                return Some(id);
            }
        }
        None
    }

    fn collect_regions(&self, id: NodeId, parent_world: &Transform, out: &mut Vec<(NodeId, Transform, f32, f32)>) {
        let n = match self.node(id) {
            Some(n) => n,
            None => return,
        };
        if !n.visible || !self.state_visible(n) {
            return;
        }
        let world = Transform::compose(parent_world, &n.transform);
        if let NodeKind::InputRegion { w, h, .. } = n.kind {
            out.push((id, world, w, h));
        }
        for &c in &n.children {
            self.collect_regions(c, &world, out);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_nest_and_draw_order() {
        let mut s = Scene::new();
        let frame = s.create(ROOT, NodeKind::Group);
        s.node_mut(frame).unwrap().transform = Transform::at(10.0, 20.0);
        let sprite = s.create(frame, NodeKind::Sprite { handle: 0xABCD, frame: 0 });
        s.node_mut(sprite).unwrap().transform = Transform::at(5.0, 5.0);
        let text = s.create(frame, NodeKind::Text { text: "hi".into(), size: 12.0, spacing: 0, anchor: TextAnchor::Baseline });
        s.node_mut(text).unwrap().transform = Transform::at(0.0, 30.0);

        let dl = s.draw_list();
        assert_eq!(dl.len(), 2, "group emits nothing; sprite+text emit");
        // World transform accumulates parent translation.
        assert_eq!(dl[0].world.x, 15.0);
        assert_eq!(dl[0].world.y, 25.0);
        assert!(matches!(dl[1].kind, NodeKind::Text { .. }));
    }

    #[test]
    fn visibility_and_state_masks_gate_drawing() {
        let mut s = Scene::new();
        let main_bit = s.state_bit("main");
        let opts_bit = s.state_bit("options");

        let a = s.create(ROOT, NodeKind::Quad { w: 1.0, h: 1.0, color: 0xFF });
        s.node_mut(a).unwrap().state_mask = main_bit;
        let b = s.create(ROOT, NodeKind::Quad { w: 1.0, h: 1.0, color: 0xFF });
        s.node_mut(b).unwrap().state_mask = opts_bit;

        s.set_state("main", true);
        let dl = s.draw_list();
        assert_eq!(dl.len(), 1);
        assert_eq!(dl[0].id, a);

        s.set_state("options", true);
        assert_eq!(s.draw_list().len(), 2);

        s.set_state("main", false);
        let dl = s.draw_list();
        assert_eq!(dl.len(), 1);
        assert_eq!(dl[0].id, b);
    }

    #[test]
    fn hit_test_picks_topmost_region() {
        let mut s = Scene::new();
        let r1 = s.create(ROOT, NodeKind::InputRegion { w: 100.0, h: 100.0, events: 0 });
        s.node_mut(r1).unwrap().transform = Transform::at(0.0, 0.0);
        let r2 = s.create(ROOT, NodeKind::InputRegion { w: 50.0, h: 50.0, events: 0 });
        s.node_mut(r2).unwrap().transform = Transform::at(10.0, 10.0);

        assert_eq!(s.hit_test(20.0, 20.0), Some(r2)); // overlapping -> last wins
        assert_eq!(s.hit_test(80.0, 80.0), Some(r1));
        assert_eq!(s.hit_test(200.0, 200.0), None);
    }

    #[test]
    fn remove_drops_subtree() {
        let mut s = Scene::new();
        let g = s.create(ROOT, NodeKind::Group);
        let _c1 = s.create(g, NodeKind::Quad { w: 1.0, h: 1.0, color: 0 });
        let _c2 = s.create(g, NodeKind::Quad { w: 1.0, h: 1.0, color: 0 });
        assert_eq!(s.node_count(), 4); // root + g + 2
        s.remove(g);
        assert_eq!(s.node_count(), 1); // just root
        assert!(!s.exists(g));
    }
}
