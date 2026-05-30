//! The declarative control model — the "XAML" half of the Avalonia-style
//! scene. A scene's visual tree is authored as data ([`ControlNode`]s) and
//! laid out with [`taffy`]; the imperative half (state machines, timers,
//! input) lives in attached Luau scripts that drive the named controls.
//!
//! Controls derive [`bevy_reflect::Reflect`] so the editor's property grid
//! and a JSON type registry can be generated from one source of truth (see
//! [`super::registry`]), and `serde` so a scene round-trips as clean,
//! hand/editor-authorable JSON (`scenes/<name>.scene.json`).
//!
//! At load the [`build_into`] pass mirrors the control tree into a [`taffy`]
//! layout tree, computes rects against the scene viewport, and writes the
//! resolved positions/sizes into the engine's primitive [`Scene`] graph —
//! so the renderer (`client::scene_runtime::draw_scene`) is untouched. It
//! returns the `name -> NodeId` index (the `x:Name` lookup scripts use) and
//! the flattened binding list the host applies each frame.

use std::collections::HashMap;

use bevy_reflect::Reflect;
use serde::{Deserialize, Serialize};

use taffy::geometry::{Rect, Size};
use taffy::prelude::{auto, length, AvailableSpace, Style, TaffyTree};
use taffy::style::{AlignItems, Display, FlexDirection, JustifyContent, LengthPercentage, Position};

use super::graph::{NodeId, NodeKind, Scene, TextAnchor, ROOT};

/// Default scene design space (the original game's logical resolution).
pub const DEFAULT_VIEWPORT: (f32, f32) = (800.0, 600.0);

/// Layout style for one control — a small, JSON-friendly subset of a
/// [`taffy::Style`]. `absolute` mode (the default) reproduces the menu's
/// pixel-exact placement via `position: absolute` + inset; `flex` mode
/// turns the control into a flexbox container for designer-authored stacks.
#[derive(Reflect, Serialize, Deserialize, Clone, Debug)]
#[serde(default)]
pub struct Layout {
    /// `"absolute"` (default) or `"flex"`.
    pub mode: String,
    /// Absolute-mode offset from the parent content box (inset left/top).
    pub x: f32,
    pub y: f32,
    /// Explicit size in px; `None` = auto (intrinsic for sprite/text).
    pub width: Option<f32>,
    pub height: Option<f32>,
    /// Flex-mode container knobs.
    pub direction: String, // "row" | "column"
    pub justify: String,   // "start" | "center" | "end" | "between" | "stretch"
    pub align: String,     // "start" | "center" | "end" | "stretch"
    pub gap: f32,
    pub padding: f32,
}

impl Default for Layout {
    fn default() -> Self {
        Layout {
            mode: "absolute".into(),
            x: 0.0,
            y: 0.0,
            width: None,
            height: None,
            direction: "row".into(),
            justify: "start".into(),
            align: "start".into(),
            gap: 0.0,
            padding: 0.0,
        }
    }
}

/// A one-way data binding: copy a script-published value (keyed by `key`
/// in the host's binding store, written via `bulanci.bind(key, value)`)
/// into the control's `prop` every frame. Supported props:
/// `visible` (bool), `x`/`y` (number), `frame` (number), `text` (string),
/// `tint`/`color` (number, 0xRRGGBBAA).
#[derive(Reflect, Serialize, Deserialize, Clone, Debug, Default)]
pub struct Binding {
    pub prop: String,
    pub key: String,
}

/// One node in the declarative control tree. A flat shape (kind selected by
/// the `kind` string + the relevant typed fields) keeps the JSON clean, the
/// `Reflect` registry simple, and the editor's property grid trivial to
/// generate. Unused fields for a given `kind` are simply ignored.
#[derive(Reflect, Serialize, Deserialize, Clone, Debug)]
#[serde(default)]
pub struct ControlNode {
    /// Editor-stable id (opaque; not used at runtime).
    pub id: String,
    /// `x:Name` — the handle scripts resolve via `bulanci.node(name)`. Empty
    /// = anonymous (not addressable from script).
    pub name: String,
    /// One of: `group`, `sprite`, `text`, `quad`, `input_region`, `button`.
    pub kind: String,
    pub layout: Layout,
    /// Extra transform beyond layout position (layout owns x/y/size).
    pub sx: f32,
    pub sy: f32,
    pub rot: f32,
    /// Single conditional-state tag gating visibility; empty = always shown.
    pub state: String,
    /// RGBA8 tint as `"rrggbbaa"` hex; empty = untinted (white).
    pub tint: String,

    // --- kind-specific (all optional) ---
    /// `sprite`: canonical asset path (hashed to a handle at build).
    pub handle: String,
    pub frame: u32,
    /// `text`/`button`: the string and font size (font picked by size).
    pub text: String,
    pub size: f32,
    pub spacing: i32,
    pub anchor: String, // "baseline" | "top"
    /// `quad`/`button`: fill color `"rrggbbaa"`.
    pub color: String,
    /// `button`: label text.
    pub label: String,

    pub bindings: Vec<Binding>,
    pub children: Vec<ControlNode>,
}

impl Default for ControlNode {
    fn default() -> Self {
        ControlNode {
            id: String::new(),
            name: String::new(),
            kind: "group".into(),
            layout: Layout::default(),
            sx: 1.0,
            sy: 1.0,
            rot: 0.0,
            state: String::new(),
            tint: String::new(),
            handle: String::new(),
            frame: 0,
            text: String::new(),
            size: 16.0,
            spacing: 0,
            anchor: "baseline".into(),
            color: String::new(),
            label: String::new(),
            bindings: Vec::new(),
            children: Vec::new(),
        }
    }
}

/// The engine node(s) a single [`ControlNode`] owns, keyed by the control's
/// stable editor `id`. Most controls map to one primitive node; a `button`
/// is a composite, so we keep all four so an HMR patch can address the right
/// piece (bg color, label text, region size, group transform/state).
#[derive(Clone, Copy, Debug)]
pub enum NodeRef {
    /// A plain primitive (`group`/`sprite`/`text`/`quad`/`input_region`).
    Single(NodeId),
    /// A `button` composite: positioned group + bg quad + label text + region.
    Button { group: NodeId, bg: NodeId, label: NodeId, region: NodeId },
}

impl NodeRef {
    /// The node that owns the control's transform, tint, and state mask.
    pub fn primary(&self) -> NodeId {
        match self {
            NodeRef::Single(id) => *id,
            NodeRef::Button { group, .. } => *group,
        }
    }

    /// Every engine node this control directly owns (for map pruning).
    pub fn all_nodes(&self) -> Vec<NodeId> {
        match self {
            NodeRef::Single(id) => vec![*id],
            NodeRef::Button { group, bg, label, region } => vec![*group, *bg, *label, *region],
        }
    }
}

/// The result of [`build_into`]: the script-facing name index, the flattened
/// bindings the host applies each frame, and the stable `id -> NodeRef` index
/// the editor's HMR patches address nodes through.
#[derive(Default)]
pub struct BuiltScene {
    /// `name -> NodeId` for `bulanci.node(name)`. For a `button`, the name
    /// resolves to its interactive region; the positioned group is also
    /// addressable as `"<name>/group"`.
    pub names: HashMap<String, NodeId>,
    /// `(node, prop, key)` triples applied each frame from the binding store.
    pub bindings: Vec<(NodeId, String, String)>,
    /// Stable editor `ControlNode.id -> NodeRef`. Empty-id controls (legacy /
    /// hand-authored) are skipped — HMR only addresses id-tagged nodes.
    pub ids: HashMap<String, NodeRef>,
}

/// Parse an `"rrggbbaa"` hex string into packed `0xRRGGBBAA`. Empty/invalid
/// falls back to `default`.
pub fn parse_color(s: &str, default: u32) -> u32 {
    let t = s.trim().trim_start_matches('#');
    if t.is_empty() {
        return default;
    }
    let hex: String = t.chars().filter(|c| c.is_ascii_hexdigit()).collect();
    let padded = match hex.len() {
        6 => format!("{hex}ff"), // rgb -> rgba (opaque)
        8 => hex,
        _ => return default,
    };
    u32::from_str_radix(&padded, 16).unwrap_or(default)
}

fn anchor_of(s: &str) -> TextAnchor {
    TextAnchor::from_str(s).unwrap_or(TextAnchor::Baseline)
}

// ---------------------------------------------------------------------
// taffy layout
// ---------------------------------------------------------------------

fn map_justify(s: &str) -> JustifyContent {
    match s {
        "center" => JustifyContent::Center,
        "end" => JustifyContent::End,
        "between" => JustifyContent::SpaceBetween,
        "around" => JustifyContent::SpaceAround,
        "evenly" => JustifyContent::SpaceEvenly,
        "stretch" => JustifyContent::Stretch,
        _ => JustifyContent::Start,
    }
}

fn map_align(s: &str) -> AlignItems {
    match s {
        "center" => AlignItems::Center,
        "end" => AlignItems::End,
        "stretch" => AlignItems::Stretch,
        "baseline" => AlignItems::Baseline,
        _ => AlignItems::Start,
    }
}

fn to_style(layout: &Layout) -> Style {
    let mut s = Style::default();
    match layout.mode.as_str() {
        "flex" => {
            // A flex container (and itself an in-flow item of its parent).
            s.display = Display::Flex;
            s.position = Position::Relative;
            s.flex_direction = if layout.direction == "column" {
                FlexDirection::Column
            } else {
                FlexDirection::Row
            };
            s.justify_content = Some(map_justify(&layout.justify));
            s.align_items = Some(map_align(&layout.align));
            s.gap = Size { width: length(layout.gap), height: length(layout.gap) };
            let p: LengthPercentage = length(layout.padding);
            s.padding = Rect { left: p, right: p, top: p, bottom: p };
        }
        "block" | "flow" | "relative" => {
            // In-flow item — flows inside a flex/block parent (no inset).
            s.display = Display::Block;
            s.position = Position::Relative;
        }
        _ => {
            // Absolute (default): place at inset(left,top); reproduces the
            // menu's pixel-exact placement and is out of the parent's flow.
            s.position = Position::Absolute;
            s.inset = Rect { left: length(layout.x), top: length(layout.y), right: auto(), bottom: auto() };
        }
    }
    s.size = Size {
        width: layout.width.map(|w| length(w)).unwrap_or_else(|| auto()),
        height: layout.height.map(|h| length(h)).unwrap_or_else(|| auto()),
    };
    s
}

fn build_taffy(tree: &mut TaffyTree<()>, c: &ControlNode) -> taffy::NodeId {
    let kids: Vec<taffy::NodeId> = c.children.iter().map(|ch| build_taffy(tree, ch)).collect();
    let style = to_style(&c.layout);
    if kids.is_empty() {
        tree.new_leaf(style).expect("taffy leaf")
    } else {
        tree.new_with_children(style, &kids).expect("taffy node")
    }
}

/// Build a control tree into `scene` under [`ROOT`]: lay it out with taffy
/// at `viewport`, create the primitive nodes, and return the name index +
/// bindings. Existing scene contents are left intact (this appends).
pub fn build_into(scene: &mut Scene, controls: &[ControlNode], viewport: (f32, f32)) -> BuiltScene {
    let mut tree: TaffyTree<()> = TaffyTree::new();
    let roots: Vec<taffy::NodeId> = controls.iter().map(|c| build_taffy(&mut tree, c)).collect();

    let root_style = Style {
        size: Size { width: length(viewport.0), height: length(viewport.1) },
        ..Default::default()
    };
    let troot = tree.new_with_children(root_style, &roots).expect("taffy root");
    let _ = tree.compute_layout(
        troot,
        Size {
            width: AvailableSpace::Definite(viewport.0),
            height: AvailableSpace::Definite(viewport.1),
        },
    );

    let mut built = BuiltScene::default();
    for (c, &tnode) in controls.iter().zip(roots.iter()) {
        instantiate(scene, ROOT, c, &tree, tnode, &mut built);
    }
    built
}

fn instantiate(
    scene: &mut Scene,
    parent: NodeId,
    c: &ControlNode,
    tree: &TaffyTree<()>,
    tnode: taffy::NodeId,
    built: &mut BuiltScene,
) {
    let (lx, ly, lw, lh) = match tree.layout(tnode) {
        Ok(l) => (l.location.x, l.location.y, l.size.width, l.size.height),
        Err(_) => (c.layout.x, c.layout.y, c.layout.width.unwrap_or(0.0), c.layout.height.unwrap_or(0.0)),
    };

    // `button` is a composite: a positioned group with bg + label + region.
    if c.kind == "button" {
        instantiate_button(scene, parent, c, lx, ly, lw, lh, built);
        return;
    }

    let node = scene.create(parent, control_kind(c, lw, lh));
    if let Some(n) = scene.node_mut(node) {
        n.transform.x = lx;
        n.transform.y = ly;
        n.transform.sx = if c.sx == 0.0 { 1.0 } else { c.sx };
        n.transform.sy = if c.sy == 0.0 { 1.0 } else { c.sy };
        n.transform.rot = c.rot;
        n.tint = parse_color(&c.tint, 0xFFFF_FFFF);
        if !c.name.is_empty() {
            n.name = Some(c.name.clone());
        }
    }
    if !c.state.is_empty() {
        let bit = scene.state_bit(&c.state);
        if let Some(n) = scene.node_mut(node) {
            n.state_mask = bit;
        }
    }
    if !c.name.is_empty() {
        built.names.insert(c.name.clone(), node);
    }
    if !c.id.is_empty() {
        built.ids.insert(c.id.clone(), NodeRef::Single(node));
    }
    for b in &c.bindings {
        built.bindings.push((node, b.prop.clone(), b.key.clone()));
    }

    if let Ok(tchildren) = tree.children(tnode) {
        for (ch, &tc) in c.children.iter().zip(tchildren.iter()) {
            instantiate(scene, node, ch, tree, tc, built);
        }
    }
}

fn control_kind(c: &ControlNode, lw: f32, lh: f32) -> NodeKind {
    match c.kind.as_str() {
        "sprite" => NodeKind::Sprite {
            handle: crate::assets::hash_path(&c.handle),
            frame: c.frame,
        },
        "text" => NodeKind::Text {
            text: c.text.clone(),
            size: if c.size <= 0.0 { 16.0 } else { c.size },
            spacing: c.spacing,
            anchor: anchor_of(&c.anchor),
        },
        "quad" => NodeKind::Quad {
            w: c.layout.width.unwrap_or(lw),
            h: c.layout.height.unwrap_or(lh),
            color: parse_color(&c.color, 0xFFFF_FFFF),
        },
        "input_region" => NodeKind::InputRegion {
            w: c.layout.width.unwrap_or(lw),
            h: c.layout.height.unwrap_or(lh),
            events: 0,
        },
        _ => NodeKind::Group,
    }
}

#[allow(clippy::too_many_arguments)]
fn instantiate_button(
    scene: &mut Scene,
    parent: NodeId,
    c: &ControlNode,
    lx: f32,
    ly: f32,
    lw: f32,
    lh: f32,
    built: &mut BuiltScene,
) {
    let (w, h) = (c.layout.width.unwrap_or(lw).max(1.0), c.layout.height.unwrap_or(lh).max(1.0));
    let group = scene.create(parent, NodeKind::Group);
    if let Some(n) = scene.node_mut(group) {
        n.transform.x = lx;
        n.transform.y = ly;
        n.transform.sx = if c.sx == 0.0 { 1.0 } else { c.sx };
        n.transform.sy = if c.sy == 0.0 { 1.0 } else { c.sy };
        n.tint = parse_color(&c.tint, 0xFFFF_FFFF);
    }
    let state_bit = if c.state.is_empty() { 0 } else { scene.state_bit(&c.state) };
    if state_bit != 0 {
        if let Some(n) = scene.node_mut(group) {
            n.state_mask = state_bit;
        }
    }

    let bg = scene.create(group, NodeKind::Quad { w, h, color: parse_color(&c.color, 0x202C_3AFF) });
    let _ = bg;
    let size = if c.size <= 0.0 { 16.0 } else { c.size };
    let label = scene.create(
        group,
        NodeKind::Text { text: c.label.clone(), size, spacing: c.spacing, anchor: TextAnchor::Top },
    );
    if let Some(n) = scene.node_mut(label) {
        n.transform.x = 8.0;
        n.transform.y = ((h - size) / 2.0).max(0.0);
    }
    let region = scene.create(group, NodeKind::InputRegion { w, h, events: 0 });

    if !c.name.is_empty() {
        // The clickable region is the primary handle; the group is also
        // addressable for transform/visibility tweaks.
        built.names.insert(c.name.clone(), region);
        built.names.insert(format!("{}/group", c.name), group);
    }
    if !c.id.is_empty() {
        built.ids.insert(c.id.clone(), NodeRef::Button { group, bg, label, region });
    }
    for b in &c.bindings {
        built.bindings.push((group, b.prop.clone(), b.key.clone()));
    }
}

// ---------------------------------------------------------------------
// Incremental HMR helpers
//
// [`build_into`] creates *and* positions in a single taffy pass at load.
// The editor's live patching instead (1) splices new subtrees in place via
// [`create_control`] — preserving every surviving NodeId so scripts' cached
// handles + the live VM stay valid — then (2) re-runs one taffy pass and
// writes the resolved geometry back onto the existing nodes via
// [`relayout_into`]. State masks are refreshed from the doc tags via
// [`recompute_masks`]. The host orchestrates these from `apply_patch`.
// ---------------------------------------------------------------------

/// Create the node(s) for `c` and its whole subtree as the `index`-th child
/// of `parent`, setting kind-specific + scale/rotation/tint/state props and
/// registering names / bindings / ids into `built`. Position and flex size
/// are deliberately left for a following [`relayout_into`] pass (taffy needs
/// the full tree to resolve them). Mirrors [`instantiate`] minus layout.
pub fn create_control(
    scene: &mut Scene,
    parent: NodeId,
    index: usize,
    c: &ControlNode,
    built: &mut BuiltScene,
) {
    if c.kind == "button" {
        create_button(scene, parent, index, c, built);
        return;
    }
    let pw = c.layout.width.unwrap_or(0.0);
    let ph = c.layout.height.unwrap_or(0.0);
    let node = scene.create_at(parent, index, control_kind(c, pw, ph));
    if let Some(n) = scene.node_mut(node) {
        n.transform.sx = if c.sx == 0.0 { 1.0 } else { c.sx };
        n.transform.sy = if c.sy == 0.0 { 1.0 } else { c.sy };
        n.transform.rot = c.rot;
        n.tint = parse_color(&c.tint, 0xFFFF_FFFF);
        if !c.name.is_empty() {
            n.name = Some(c.name.clone());
        }
    }
    if !c.state.is_empty() {
        let bit = scene.state_bit(&c.state);
        if let Some(n) = scene.node_mut(node) {
            n.state_mask = bit;
        }
    }
    if !c.name.is_empty() {
        built.names.insert(c.name.clone(), node);
    }
    if !c.id.is_empty() {
        built.ids.insert(c.id.clone(), NodeRef::Single(node));
    }
    for b in &c.bindings {
        built.bindings.push((node, b.prop.clone(), b.key.clone()));
    }
    for ch in &c.children {
        create_control(scene, node, usize::MAX, ch, built);
    }
}

fn create_button(scene: &mut Scene, parent: NodeId, index: usize, c: &ControlNode, built: &mut BuiltScene) {
    let w = c.layout.width.unwrap_or(1.0).max(1.0);
    let h = c.layout.height.unwrap_or(1.0).max(1.0);
    let group = scene.create_at(parent, index, NodeKind::Group);
    if let Some(n) = scene.node_mut(group) {
        n.transform.sx = if c.sx == 0.0 { 1.0 } else { c.sx };
        n.transform.sy = if c.sy == 0.0 { 1.0 } else { c.sy };
        n.tint = parse_color(&c.tint, 0xFFFF_FFFF);
    }
    let state_bit = if c.state.is_empty() { 0 } else { scene.state_bit(&c.state) };
    if state_bit != 0 {
        if let Some(n) = scene.node_mut(group) {
            n.state_mask = state_bit;
        }
    }
    let bg = scene.create(group, NodeKind::Quad { w, h, color: parse_color(&c.color, 0x202C_3AFF) });
    let size = if c.size <= 0.0 { 16.0 } else { c.size };
    let label = scene.create(
        group,
        NodeKind::Text { text: c.label.clone(), size, spacing: c.spacing, anchor: TextAnchor::Top },
    );
    if let Some(n) = scene.node_mut(label) {
        n.transform.x = 8.0;
        n.transform.y = ((h - size) / 2.0).max(0.0);
    }
    let region = scene.create(group, NodeKind::InputRegion { w, h, events: 0 });
    if !c.name.is_empty() {
        built.names.insert(c.name.clone(), region);
        built.names.insert(format!("{}/group", c.name), group);
    }
    if !c.id.is_empty() {
        built.ids.insert(c.id.clone(), NodeRef::Button { group, bg, label, region });
    }
    for b in &c.bindings {
        built.bindings.push((group, b.prop.clone(), b.key.clone()));
    }
}

/// Re-run taffy over `controls` at `viewport` and write the resolved local
/// position (and flex-resolved size) back onto the EXISTING nodes named by
/// `ids`, reordering each parent's children to the control order so draw
/// order tracks the doc after add/reparent. Surviving NodeIds are untouched.
pub fn relayout_into(
    scene: &mut Scene,
    controls: &[ControlNode],
    viewport: (f32, f32),
    ids: &HashMap<String, NodeRef>,
) {
    let mut tree: TaffyTree<()> = TaffyTree::new();
    let roots: Vec<taffy::NodeId> = controls.iter().map(|c| build_taffy(&mut tree, c)).collect();
    let root_style = Style {
        size: Size { width: length(viewport.0), height: length(viewport.1) },
        ..Default::default()
    };
    let troot = tree.new_with_children(root_style, &roots).expect("taffy root");
    let _ = tree.compute_layout(
        troot,
        Size {
            width: AvailableSpace::Definite(viewport.0),
            height: AvailableSpace::Definite(viewport.1),
        },
    );
    reorder_doc_children(scene, ROOT, controls, ids);
    for (c, &tnode) in controls.iter().zip(roots.iter()) {
        position_control(scene, c, &tree, tnode, ids);
    }
}

fn reorder_doc_children(scene: &mut Scene, parent: NodeId, controls: &[ControlNode], ids: &HashMap<String, NodeRef>) {
    let order: Vec<NodeId> = controls.iter().filter_map(|c| ids.get(&c.id).map(|r| r.primary())).collect();
    if !order.is_empty() {
        scene.reorder_children(parent, &order);
    }
}

fn position_control(
    scene: &mut Scene,
    c: &ControlNode,
    tree: &TaffyTree<()>,
    tnode: taffy::NodeId,
    ids: &HashMap<String, NodeRef>,
) {
    let (lx, ly, lw, lh) = match tree.layout(tnode) {
        Ok(l) => (l.location.x, l.location.y, l.size.width, l.size.height),
        Err(_) => (c.layout.x, c.layout.y, c.layout.width.unwrap_or(0.0), c.layout.height.unwrap_or(0.0)),
    };
    match ids.get(&c.id).copied() {
        Some(NodeRef::Button { group, bg, label, region }) => {
            let w = c.layout.width.unwrap_or(lw).max(1.0);
            let h = c.layout.height.unwrap_or(lh).max(1.0);
            if let Some(n) = scene.node_mut(group) {
                n.transform.x = lx;
                n.transform.y = ly;
            }
            if let Some(n) = scene.node_mut(bg) {
                if let NodeKind::Quad { w: qw, h: qh, .. } = &mut n.kind {
                    *qw = w;
                    *qh = h;
                }
            }
            if let Some(n) = scene.node_mut(region) {
                if let NodeKind::InputRegion { w: rw, h: rh, .. } = &mut n.kind {
                    *rw = w;
                    *rh = h;
                }
            }
            let size = if c.size <= 0.0 { 16.0 } else { c.size };
            if let Some(n) = scene.node_mut(label) {
                n.transform.x = 8.0;
                n.transform.y = ((h - size) / 2.0).max(0.0);
            }
            // `button` is a leaf composite — it ignores doc children.
        }
        Some(NodeRef::Single(node)) => {
            if let Some(n) = scene.node_mut(node) {
                n.transform.x = lx;
                n.transform.y = ly;
                match &mut n.kind {
                    NodeKind::Quad { w, h, .. } => {
                        *w = c.layout.width.unwrap_or(lw);
                        *h = c.layout.height.unwrap_or(lh);
                    }
                    NodeKind::InputRegion { w, h, .. } => {
                        *w = c.layout.width.unwrap_or(lw);
                        *h = c.layout.height.unwrap_or(lh);
                    }
                    _ => {}
                }
            }
            reorder_doc_children(scene, node, &c.children, ids);
            recurse_children(scene, c, tree, tnode, ids);
        }
        None => recurse_children(scene, c, tree, tnode, ids),
    }
}

fn recurse_children(
    scene: &mut Scene,
    c: &ControlNode,
    tree: &TaffyTree<()>,
    tnode: taffy::NodeId,
    ids: &HashMap<String, NodeRef>,
) {
    if let Ok(tchildren) = tree.children(tnode) {
        for (ch, &tc) in c.children.iter().zip(tchildren.iter()) {
            position_control(scene, ch, tree, tc, ids);
        }
    }
}

/// Write a control's in-place props — kind-specific fields (sprite handle/
/// frame, text/size/spacing/anchor, quad color, button label/color) plus
/// scale/rotation/tint/name — onto its existing engine node(s). Position,
/// size and state mask are handled by [`relayout_into`]/[`recompute_masks`],
/// so this is the cheap per-edit visual sync. The node *variant* never
/// changes (the editor has no kind-change action), so reassigning `kind` only
/// refreshes the variant's fields.
pub fn apply_props(scene: &mut Scene, r: NodeRef, c: &ControlNode) {
    match r {
        NodeRef::Single(node) => {
            let kind = control_kind(c, c.layout.width.unwrap_or(0.0), c.layout.height.unwrap_or(0.0));
            if let Some(n) = scene.node_mut(node) {
                n.kind = kind;
                n.transform.sx = if c.sx == 0.0 { 1.0 } else { c.sx };
                n.transform.sy = if c.sy == 0.0 { 1.0 } else { c.sy };
                n.transform.rot = c.rot;
                n.tint = parse_color(&c.tint, 0xFFFF_FFFF);
                n.name = if c.name.is_empty() { None } else { Some(c.name.clone()) };
            }
        }
        NodeRef::Button { group, bg, label, .. } => {
            if let Some(n) = scene.node_mut(group) {
                n.transform.sx = if c.sx == 0.0 { 1.0 } else { c.sx };
                n.transform.sy = if c.sy == 0.0 { 1.0 } else { c.sy };
                n.tint = parse_color(&c.tint, 0xFFFF_FFFF);
            }
            if let Some(n) = scene.node_mut(bg) {
                if let NodeKind::Quad { color, .. } = &mut n.kind {
                    *color = parse_color(&c.color, 0x202C_3AFF);
                }
            }
            let size = if c.size <= 0.0 { 16.0 } else { c.size };
            if let Some(n) = scene.node_mut(label) {
                if let NodeKind::Text { text, size: s, spacing, .. } = &mut n.kind {
                    *text = c.label.clone();
                    *s = size;
                    *spacing = c.spacing;
                }
            }
        }
    }
}

/// Recompute every control's `state_mask` from its current `state` tag (via
/// the scene's name->bit table) so an HMR edit to states or node tags shows
/// without a rebuild. Empty tag => always visible (mask 0).
pub fn recompute_masks(scene: &mut Scene, controls: &[ControlNode], ids: &HashMap<String, NodeRef>) {
    for c in controls {
        let mask = if c.state.is_empty() { 0 } else { scene.state_bit(&c.state) };
        if let Some(r) = ids.get(&c.id) {
            let primary = r.primary();
            if let Some(n) = scene.node_mut(primary) {
                n.state_mask = mask;
            }
        }
        recompute_masks(scene, &c.children, ids);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parse_color_handles_rgb_rgba_and_default() {
        assert_eq!(parse_color("ff0000ff", 0), 0xFF00_00FF);
        assert_eq!(parse_color("00ff00", 0), 0x00FF_00FF); // rgb -> opaque
        assert_eq!(parse_color("", 0x1234_5678), 0x1234_5678);
        assert_eq!(parse_color("nonsense!!", 7), 7);
    }

    #[test]
    fn build_places_absolute_nodes_and_indexes_names() {
        let mut scene = Scene::new();
        let controls = vec![ControlNode {
            kind: "group".into(),
            name: "panel".into(),
            layout: Layout { x: 100.0, y: 50.0, ..Default::default() },
            children: vec![ControlNode {
                kind: "text".into(),
                name: "title".into(),
                text: "Play".into(),
                size: 24.0,
                layout: Layout { x: 10.0, y: 5.0, ..Default::default() },
                ..Default::default()
            }],
            ..Default::default()
        }];
        let built = build_into(&mut scene, &controls, DEFAULT_VIEWPORT);

        let panel = built.names["panel"];
        let title = built.names["title"];
        // panel at (100,50); title local (10,5) -> world (110,55).
        assert_eq!(scene.world_of(panel).x, 100.0);
        assert_eq!(scene.world_of(title).x, 110.0);
        assert_eq!(scene.world_of(title).y, 55.0);
    }

    #[test]
    fn quad_takes_explicit_size() {
        let mut scene = Scene::new();
        let controls = vec![ControlNode {
            kind: "quad".into(),
            color: "102030ff".into(),
            layout: Layout { x: 0.0, y: 0.0, width: Some(64.0), height: Some(12.0), ..Default::default() },
            ..Default::default()
        }];
        build_into(&mut scene, &controls, DEFAULT_VIEWPORT);
        let dl = scene.draw_list();
        assert_eq!(dl.len(), 1);
        match dl[0].kind {
            NodeKind::Quad { w, h, color } => {
                assert_eq!((w, h), (64.0, 12.0));
                assert_eq!(color, 0x1020_30FF);
            }
            _ => panic!("expected quad"),
        }
    }

    #[test]
    fn button_composite_indexes_region_and_group() {
        let mut scene = Scene::new();
        let controls = vec![leaf_button()];
        let built = build_into(&mut scene, &controls, DEFAULT_VIEWPORT);
        assert!(built.names.contains_key("play"));
        assert!(built.names.contains_key("play/group"));
    }

    fn leaf_button() -> ControlNode {
        ControlNode {
            kind: "button".into(),
            name: "play".into(),
            label: "Play".into(),
            layout: Layout { x: 80.0, y: 130.0, width: Some(200.0), height: Some(30.0), ..Default::default() },
            ..Default::default()
        }
    }

    #[test]
    fn flex_row_lays_children_left_to_right() {
        let mut scene = Scene::new();
        let controls = vec![ControlNode {
            kind: "group".into(),
            name: "bar".into(),
            layout: Layout {
                mode: "flex".into(),
                direction: "row".into(),
                gap: 10.0,
                width: Some(300.0),
                height: Some(40.0),
                ..Default::default()
            },
            children: vec![
                ControlNode { kind: "quad".into(), name: "a".into(), layout: Layout { mode: "block".into(), width: Some(40.0), height: Some(40.0), ..Default::default() }, ..Default::default() },
                ControlNode { kind: "quad".into(), name: "b".into(), layout: Layout { mode: "block".into(), width: Some(40.0), height: Some(40.0), ..Default::default() }, ..Default::default() },
            ],
            ..Default::default()
        }];
        let built = build_into(&mut scene, &controls, DEFAULT_VIEWPORT);
        let a = built.names["a"];
        let b = built.names["b"];
        // Row layout with a 10px gap -> b starts 50px right of a.
        let ax = scene.world_of(a).x;
        let bx = scene.world_of(b).x;
        assert!((bx - ax - 50.0).abs() < 0.01, "ax={ax} bx={bx}");
    }
}
