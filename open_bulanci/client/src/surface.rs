//! The client's **surface stack** — the reusable multi-surface composition +
//! HMR router that replaces the single `scene_menu` slot.
//!
//! A *surface* is one live, addressable unit the game composites and the editor
//! can hot-patch: a declarative scene today (the menu), and — as the system
//! grows — intro/connecting screens, dialogs/HUD overlays, levels, and the
//! gameplay sim. The stack owns an ordered list; index `0` is the **base**
//! (the screen's primary scene) and everything above it is an **overlay**
//! pushed via `bulanci.scene.push(name)` (dialogs, HUD, a future global cursor).
//!
//! The composite rules are uniform and phase-agnostic:
//!   * **draw** — walk the stack bottom→top, blitting each surface's
//!     [`Surface::draw_list`] through the shared renderer (painter order);
//!   * **tick** — every surface advances each frame (so its timers keep
//!     running), but click/key **events** go only to the topmost surface
//!     (modal), while pointer **position** is fed to all (so e.g. a global
//!     cursor keeps following beneath a dialog);
//!   * **patch** — a drained HMR patch is offered top→bottom until a surface
//!     claims it by name ([`SurfaceStack::route_patch`]);
//!   * **rebuild** — a dirty byte set rebuilds exactly the surfaces whose deps
//!     changed, in place, preserving their stack position + layer.
//!
//! `Surface` is an enum (not a `dyn` trait): the renderer needs a generic
//! `RaylibDraw` draw call, which a trait object can't express, and the
//! codebase favours monomorphised enums over vtables. New surface kinds
//! (gameplay, levels) are a variant + a few match arms away.

use std::collections::HashSet;

use bulanci_core::gameplay::GameplaySurface;
use bulanci_core::level::LevelSurface;
use bulanci_core::scene::{
    DrawItem, FrameInput, HmrSurface, HostCommand, PatchOutcome, SceneManager,
};

/// Where a surface sits in the composite.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum SurfaceLayer {
    /// The screen's primary scene (stack index 0). `scene.goto` swaps it.
    Base,
    /// An overlay above the base (dialog / HUD / cursor), pushed/popped.
    Overlay,
}

/// One live surface in the stack.
pub enum Surface {
    /// A declarative/Luau scene driven through the core [`SceneManager`].
    Scene { mgr: SceneManager, layer: SurfaceLayer },
    /// A declarative gameplay level — sim-adjacent data (obstacles / spawns /
    /// tunables) the simulation reads, hot-patched through the same router.
    /// It carries no UI draw list of its own; the gameplay surface renders the
    /// world it seeds.
    Level { surface: LevelSurface, layer: SurfaceLayer },
    /// A live match: the deterministic sim + its hot-patchable tunables. Its
    /// `draw_list` is the world (obstacles/players/bullets) as `DrawItem`s, so
    /// a match composites with a HUD scene overlay. Editor patches are
    /// determinism-gated inside the surface (queued to a tick boundary in a
    /// networked match).
    Gameplay { surface: GameplaySurface, layer: SurfaceLayer },
}

impl Surface {
    pub fn scene(mgr: SceneManager, layer: SurfaceLayer) -> Self {
        Surface::Scene { mgr, layer }
    }

    #[allow(dead_code)]
    pub fn level(surface: LevelSurface, layer: SurfaceLayer) -> Self {
        Surface::Level { surface, layer }
    }

    #[allow(dead_code)]
    pub fn gameplay(surface: GameplaySurface, layer: SurfaceLayer) -> Self {
        Surface::Gameplay { surface, layer }
    }

    /// Routing key (the active scene/level/match name).
    pub fn name(&self) -> &str {
        match self {
            Surface::Scene { mgr, .. } => mgr.surface_name(),
            Surface::Level { surface, .. } => surface.surface_name(),
            Surface::Gameplay { surface, .. } => surface.surface_name(),
        }
    }

    pub fn layer(&self) -> SurfaceLayer {
        match self {
            Surface::Scene { layer, .. } => *layer,
            Surface::Level { layer, .. } => *layer,
            Surface::Gameplay { layer, .. } => *layer,
        }
    }

    /// The flattened draw list this surface contributes, in paint order
    /// (freshly built each call, matching `SceneHost::draw_list`). A level
    /// contributes nothing of its own (the gameplay surface renders the world
    /// it seeds); a gameplay surface emits the live world.
    pub fn draw_list(&self) -> Vec<DrawItem> {
        match self {
            Surface::Scene { mgr, .. } => mgr.host().draw_list(),
            Surface::Level { .. } => Vec::new(),
            Surface::Gameplay { surface, .. } => surface.draw_list(),
        }
    }

    /// VFS paths whose change forces a from-scratch rebuild of this surface.
    pub fn needs_rebuild(&self, dirty: &HashSet<u64>) -> bool {
        match self {
            Surface::Scene { mgr, .. } => mgr.needs_rebuild(dirty),
            Surface::Level { surface, .. } => surface.needs_rebuild(dirty),
            Surface::Gameplay { surface, .. } => surface.needs_rebuild(dirty),
        }
    }

    /// Offer a raw editor HMR patch; the surface claims it iff its routing key
    /// matches the patch envelope's `surface`.
    pub fn apply_raw_patch(&self, raw: &[u8]) -> PatchOutcome {
        match self {
            Surface::Scene { mgr, .. } => mgr.apply_raw_patch(raw),
            Surface::Level { surface, .. } => surface.apply_raw_patch(raw),
            Surface::Gameplay { surface, .. } => surface.apply_raw_patch(raw),
        }
    }

    /// Advance one frame; returns `(queued audio handles, pending goto,
    /// lifecycle commands)` for the client to act on. Level/gameplay surfaces
    /// have no per-frame UI VM — the deterministic sim advances gameplay via
    /// its own `tick(inputs)` driven by the netcode — so they are inert here.
    pub fn tick(
        &self,
        now_ms: u64,
        input: FrameInput,
    ) -> (Vec<u64>, Option<String>, Vec<HostCommand>) {
        match self {
            Surface::Scene { mgr, .. } => mgr.tick(now_ms, input),
            Surface::Level { .. } | Surface::Gameplay { .. } => (Vec::new(), None, Vec::new()),
        }
    }

    /// Borrow the underlying scene manager (for the gameplay HUD / level
    /// surfaces that need scene-specific access in later phases).
    #[allow(dead_code)]
    pub fn as_scene(&self) -> Option<&SceneManager> {
        match self {
            Surface::Scene { mgr, .. } => Some(mgr),
            _ => None,
        }
    }

    /// Borrow the underlying level surface (for the gameplay sim to seed
    /// obstacles/spawns from and to read tunables).
    #[allow(dead_code)]
    pub fn as_level(&self) -> Option<&LevelSurface> {
        match self {
            Surface::Level { surface, .. } => Some(surface),
            _ => None,
        }
    }

    /// Borrow the underlying gameplay surface (for the netcode to step the sim
    /// and the HUD to read scores/tunables).
    #[allow(dead_code)]
    pub fn as_gameplay(&self) -> Option<&GameplaySurface> {
        match self {
            Surface::Gameplay { surface, .. } => Some(surface),
            _ => None,
        }
    }
}

/// An ordered composite of surfaces (base at index 0, overlays above).
#[derive(Default)]
pub struct SurfaceStack {
    surfaces: Vec<Surface>,
}

impl SurfaceStack {
    pub fn new() -> Self {
        Self { surfaces: Vec::new() }
    }

    /// A stack seeded with a single base surface.
    pub fn from_base(base: Surface) -> Self {
        Self { surfaces: vec![base] }
    }

    pub fn is_empty(&self) -> bool {
        self.surfaces.is_empty()
    }

    pub fn len(&self) -> usize {
        self.surfaces.len()
    }

    pub fn iter(&self) -> std::slice::Iter<'_, Surface> {
        self.surfaces.iter()
    }

    #[allow(dead_code)]
    pub fn base(&self) -> Option<&Surface> {
        self.surfaces.first()
    }

    #[allow(dead_code)]
    pub fn top(&self) -> Option<&Surface> {
        self.surfaces.last()
    }

    /// Install / replace the base (index 0) surface, keeping any overlays.
    pub fn set_base(&mut self, base: Surface) {
        if self.surfaces.is_empty() {
            self.surfaces.push(base);
        } else {
            self.surfaces[0] = base;
        }
    }

    /// Push an overlay surface on top.
    pub fn push(&mut self, overlay: Surface) {
        self.surfaces.push(overlay);
    }

    /// Pop the top **overlay** (never the base). `None` when only the base
    /// remains, so `scene.pop()` from a base scene is a safe no-op.
    pub fn pop_overlay(&mut self) -> Option<Surface> {
        if self.surfaces.len() > 1 {
            self.surfaces.pop()
        } else {
            None
        }
    }

    /// Pop the top overlay only when it is the surface named `name` (and not
    /// the base). Lets a phase transition retract exactly the overlay it
    /// pushed (e.g. `intro` / `connecting`) without disturbing anything else.
    pub fn pop_overlay_named(&mut self, name: &str) -> bool {
        if self.surfaces.len() > 1 && self.surfaces.last().map(|s| s.name()) == Some(name) {
            self.surfaces.pop();
            true
        } else {
            false
        }
    }

    /// Replace the surface at `idx` in place (rebuild / goto), keeping order.
    pub fn replace_at(&mut self, idx: usize, surface: Surface) {
        if idx < self.surfaces.len() {
            self.surfaces[idx] = surface;
        }
    }

    pub fn name_at(&self, idx: usize) -> Option<&str> {
        self.surfaces.get(idx).map(|s| s.name())
    }

    pub fn layer_at(&self, idx: usize) -> Option<SurfaceLayer> {
        self.surfaces.get(idx).map(|s| s.layer())
    }

    /// Route a raw HMR patch to the first surface that claims it (top→bottom,
    /// since an overlay shadows the base). Returns the winning outcome, or
    /// `NotMine` when no live surface is editing that target.
    pub fn route_patch(&self, raw: &[u8]) -> PatchOutcome {
        for s in self.surfaces.iter().rev() {
            match s.apply_raw_patch(raw) {
                PatchOutcome::NotMine => continue,
                other => return other,
            }
        }
        PatchOutcome::NotMine
    }

    /// `(index, name, layer)` for every surface whose deps went dirty, so the
    /// client can rebuild each in place. Computed up front (an immutable
    /// borrow) so the client can drop it before mutating the stack.
    pub fn rebuild_plan(&self, dirty: &HashSet<u64>) -> Vec<(usize, String, SurfaceLayer)> {
        self.surfaces
            .iter()
            .enumerate()
            .filter(|(_, s)| s.needs_rebuild(dirty))
            .map(|(i, s)| (i, s.name().to_string(), s.layer()))
            .collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use bulanci_core::gameplay::{GameplayDoc, HmrMode};
    use bulanci_core::level::LevelDoc;
    use bulanci_core::scene::HostServices;
    use std::collections::HashMap;

    fn load(name: &str, json: &str) -> SceneManager {
        let key = format!("scenes/{name}.scene.json");
        let fs: HashMap<String, Vec<u8>> =
            [(key, json.as_bytes().to_vec())].into_iter().collect();
        let read = move |p: &str| fs.get(p).cloned();
        SceneManager::load(name, &read, &[], HostServices::default()).unwrap()
    }

    fn level_surface(name: &str) -> Surface {
        let doc = LevelDoc { name: name.to_string(), ..Default::default() };
        Surface::level(
            LevelSurface::new(doc, vec![format!("levels/{name}.level.json")]),
            SurfaceLayer::Overlay,
        )
    }

    fn base_surface() -> Surface {
        Surface::scene(
            load(
                "base_s",
                r#"{ "name":"base_s", "root":[
                     { "kind":"text", "name":"bt", "text":"B", "size":16,
                       "layout": { "x": 1, "y": 1 } } ] }"#,
            ),
            SurfaceLayer::Base,
        )
    }

    fn overlay_surface() -> Surface {
        Surface::scene(
            load(
                "ovl_s",
                r#"{ "name":"ovl_s", "root":[
                     { "kind":"text", "name":"o1", "text":"O", "size":16,
                       "layout": { "x": 2, "y": 2 } },
                     { "kind":"text", "name":"o2", "text":"O2", "size":16,
                       "layout": { "x": 3, "y": 3 } } ] }"#,
            ),
            SurfaceLayer::Overlay,
        )
    }

    #[test]
    fn composites_draw_lists_base_then_overlays_in_order() {
        let mut stack = SurfaceStack::from_base(base_surface());
        stack.push(overlay_surface());

        // The client walks the stack bottom->top; the base's single item must
        // precede the overlay's two (painter order = overlay on top).
        let composite: Vec<f32> =
            stack.iter().flat_map(|s| s.draw_list()).map(|i| i.world.x).collect();
        assert_eq!(composite, vec![1.0, 2.0, 3.0]);
        assert_eq!(stack.len(), 2);
        assert_eq!(stack.base().unwrap().name(), "base_s");
        assert_eq!(stack.top().unwrap().name(), "ovl_s");
    }

    #[test]
    fn routes_patch_to_the_surface_that_owns_it() {
        let mut stack = SurfaceStack::from_base(base_surface());
        stack.push(overlay_surface());

        let patch = |surface: &str| format!(r#"{{ "surface":"{surface}", "rev":1, "ops":[] }}"#);

        // The overlay shadows the base, so a patch for the overlay is claimed
        // by it; one for the base falls through to the base; an unknown target
        // is claimed by nobody.
        assert_eq!(stack.route_patch(patch("ovl_s").as_bytes()), PatchOutcome::Applied);
        assert_eq!(stack.route_patch(patch("base_s").as_bytes()), PatchOutcome::Applied);
        assert_eq!(stack.route_patch(patch("nope").as_bytes()), PatchOutcome::NotMine);

        // Legacy `scene` envelope key still routes (serde alias).
        let legacy = r#"{ "scene":"base_s", "rev":1, "ops":[] }"#;
        assert_eq!(stack.route_patch(legacy.as_bytes()), PatchOutcome::Applied);
    }

    #[test]
    fn pop_removes_overlay_but_never_the_base() {
        let mut stack = SurfaceStack::from_base(base_surface());
        stack.push(overlay_surface());
        assert_eq!(stack.len(), 2);

        assert!(stack.pop_overlay().is_some());
        assert_eq!(stack.len(), 1);
        // After the overlay is gone, its patches no longer match anything.
        let p = r#"{ "surface":"ovl_s", "rev":1, "ops":[] }"#;
        assert_eq!(stack.route_patch(p.as_bytes()), PatchOutcome::NotMine);

        // The base is protected: pop is a no-op once only it remains.
        assert!(stack.pop_overlay().is_none());
        assert_eq!(stack.len(), 1);
    }

    #[test]
    fn router_dispatches_across_heterogeneous_surface_kinds() {
        // A scene base with a level overlay: the router must hand each its own
        // patch and let the other pass (NotMine), even though scene ops and
        // level ops are entirely different vocabularies.
        let mut stack = SurfaceStack::from_base(base_surface());
        stack.push(level_surface("test_arena"));

        // A level op routes to the level surface and mutates it live.
        let lvl_patch = r#"{ "surface":"test_arena", "ops":[
            { "op":"add_obstacle", "obstacle": { "id":"w", "x1":0,"y1":0,"x2":10,"y2":10 } } ] }"#;
        assert_eq!(stack.route_patch(lvl_patch.as_bytes()), PatchOutcome::Applied);
        assert_eq!(stack.top().unwrap().as_level().unwrap().doc().obstacles.len(), 1);

        // A scene op falls through the level (NotMine) to the scene base.
        let scene_patch = r#"{ "surface":"base_s", "rev":1, "ops":[] }"#;
        assert_eq!(stack.route_patch(scene_patch.as_bytes()), PatchOutcome::Applied);

        // A level surface contributes no UI draw items.
        assert!(stack.top().unwrap().draw_list().is_empty());

        // Nobody owns an unknown target.
        let orphan = r#"{ "surface":"ghost", "ops":[] }"#;
        assert_eq!(stack.route_patch(orphan.as_bytes()), PatchOutcome::NotMine);
    }

    #[test]
    fn gameplay_surface_routes_and_renders_world() {
        // A gameplay match as a base surface: it claims its sim-op patches and
        // emits the world as DrawItems (so it composes with a HUD overlay).
        let doc = GameplayDoc { name: "match1".into(), ..Default::default() };
        let gp = GameplaySurface::new(doc, HmrMode::Dev, vec![]);
        {
            let mut sim = gp.sim_mut();
            sim.add_obstacle(0, 0, 50, 50);
            sim.add_player(0, 0, 100, 100);
        }
        let stack = SurfaceStack::from_base(Surface::gameplay(gp, SurfaceLayer::Base));

        // Sim-op patch routes to the gameplay surface and applies (Dev mode).
        let patch = r#"{ "surface":"match1", "ops":[
            { "op":"set_tunable", "key":"player_speed", "value":7 } ] }"#;
        assert_eq!(stack.route_patch(patch.as_bytes()), PatchOutcome::Applied);
        assert_eq!(
            stack.base().unwrap().as_gameplay().unwrap().sim().tunables.player_speed,
            7
        );

        // The base emits world shapes: obstacle quad + player (circle + line).
        assert_eq!(stack.base().unwrap().draw_list().len(), 3);

        // A scene patch finds no owner here.
        let scene = r#"{ "surface":"main_menu", "ops":[] }"#;
        assert_eq!(stack.route_patch(scene.as_bytes()), PatchOutcome::NotMine);
    }
}
