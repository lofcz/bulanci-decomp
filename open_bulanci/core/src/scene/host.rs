//! The scene host: a Luau VM wired to a [`Scene`] graph via the versioned
//! `bulanci.*` binding API, plus a phase scheduler, timers, and pointer
//! input dispatch.
//!
//! A *scene* is a Luau module (source in dev, stripped bytecode in dist)
//! that, when loaded, calls `bulanci.*` to build the node tree, declare
//! conditional states, and register behaviors (`subscribe`, `every`,
//! `on_input`). Each frame the client feeds pointer input and calls
//! [`SceneHost::tick`], then renders [`SceneHost::draw_list`] through its
//! atlas/font stack. Audio/scene-transition requests are returned as
//! queued commands so the *engine core stays renderer- and audio-free*.
//!
//! Borrow discipline: the `bulanci.*` Rust closures mutate shared state
//! through an `Rc<RefCell<Inner>>`. `tick` always clones the Luau
//! callbacks out of that cell *before* invoking them, so a behavior is
//! free to create nodes / register timers re-entrantly without a double
//! borrow.

use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;

use mlua::prelude::*;

use crate::assets::hash_path;
use crate::scheduler::Scheduler;

use super::control::{self, Binding, NodeRef};
use super::document::SceneDoc;
use super::graph::{DrawItem, InputEvent, Node, NodeId, NodeKind, Scene, TextAnchor, ROOT};
use super::patch::{self, PatchOp, ScenePatch};

/// The current `bulanci.*` API contract version. Scene modules can read
/// `bulanci.API_VERSION` to feature-gate; bumped on breaking changes.
pub const API_VERSION: i64 = 1;

/// Per-frame input the client feeds the host: pointer, mouse button, and
/// keyboard (key names are the engine's lowercase names; chars are typed
/// codepoints for text entry).
#[derive(Clone, Debug, Default)]
pub struct FrameInput {
    pub x: f32,
    pub y: f32,
    pub pressed: bool,
    /// Keys that transitioned to down this frame.
    pub keys_pressed: Vec<String>,
    /// Keys currently held this frame.
    pub keys_down: Vec<String>,
    /// Characters typed this frame (single-codepoint strings).
    pub chars: Vec<String>,
}

/// A lifecycle request the scene raised that only the client can fulfill
/// (networking, app exit, ...). Drained each frame via
/// [`SceneHost::drain_commands`]. Pure rendering/audio stay out of here.
#[derive(Clone, Debug, PartialEq, Eq)]
pub enum HostCommand {
    /// `bulanci.net.connect(addr)` — join/host a match at `addr`.
    Connect(String),
    /// `bulanci.app.exit()` — begin the retail exit sequence.
    Exit,
    /// `bulanci.scene.push(name)` — push `name` as a modal overlay surface on
    /// top of this one (the client's surface stack draws it above and routes
    /// input to it). Unlike `goto`, the pushed-from surface stays live beneath.
    PushScene(String),
    /// `bulanci.scene.pop()` — pop the top overlay surface, revealing the one
    /// beneath. A no-op for the client when only the base surface remains.
    PopScene,
}

/// Per-frame atlas metadata the client hands to Luau so cursor/dial
/// behaviors can drive frame motion without the engine core knowing about
/// raylib atlases. Returned by the `image`/`atlas` host service.
#[derive(Clone, Debug, Default)]
pub struct AtlasInfo {
    pub frame_count: u32,
    pub origin_x: f32,
    pub origin_y: f32,
    /// Per-frame `(dx, dy)` motion deltas (cursor idle-twitch paths).
    pub motion: Vec<(f32, f32)>,
}

/// Client-provided services the host exposes to Luau. The engine core has
/// no fonts/textures/VFS of its own, so the client injects these closures
/// (built from its asset stack) before loading a scene. All are optional;
/// a missing service makes the matching `bulanci.*` call return a benign
/// default (0 / nil).
#[derive(Default)]
pub struct HostServices {
    /// Measure a string's pixel width in the bitmap font picked by `size`,
    /// with the given char-spacing (mirrors `ClientApp::measure_t_sp`).
    pub measure: Option<Rc<dyn Fn(&str, f32, i32) -> f32>>,
    /// Read a VFS text asset by path (poems).
    pub read_text: Option<Rc<dyn Fn(&str) -> Option<String>>>,
    /// `(line_height, baseline)` of the bitmap font picked by `size`. Used
    /// for vertical centering of button labels.
    pub font_metrics: Option<Rc<dyn Fn(f32) -> (f32, f32)>>,
    /// Pixel size of an image asset by handle hash (hero/reticle anchoring).
    pub image_size: Option<Rc<dyn Fn(u64) -> Option<(f32, f32)>>>,
    /// Atlas metadata by handle hash (frame count, origin, motion paths).
    pub atlas_info: Option<Rc<dyn Fn(u64) -> Option<AtlasInfo>>>,
}

/// Tick phases, dispatched in this order each frame (Draw last, right
/// before the client reads the draw list).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Phase {
    PreUpdate,
    Update,
    PostUpdate,
    Draw,
}

impl Phase {
    fn from_str(s: &str) -> Option<Self> {
        match s {
            "preupdate" => Some(Phase::PreUpdate),
            "update" => Some(Phase::Update),
            "postupdate" => Some(Phase::PostUpdate),
            "draw" => Some(Phase::Draw),
            _ => None,
        }
    }
}

struct Timer {
    func: LuaFunction,
    recurring: bool,
}

/// A fixed-timestep tick bound via `bulanci.on_fixed`. Unlike a `Timer`,
/// the host coalesces all due steps for one frame into a **single** Luau
/// call `func(steps)` — so a 5 ms cursor beat costs one boundary crossing
/// per frame regardless of fps, and the body decides how to apply N steps.
/// Drift-free: `last_ms` advances by `interval_ms * steps` (the residue is
/// kept), exactly like the [`crate::scheduler::Scheduler`].
struct FixedTick {
    interval_ms: u64,
    /// Wall-clock anchor of the last dispatched step. Set lazily on the
    /// first tick that observes this bind (see `anchored`).
    last_ms: u64,
    /// `false` until the first tick anchors `last_ms` to the live clock.
    /// This is what stops a cold-start burst: a bind registered at scene
    /// load (now = 0) and first ticked seconds later anchors to *that*
    /// frame instead of replaying the whole gap.
    anchored: bool,
    func: LuaFunction,
}

/// Largest number of fixed steps handed to a single `on_fixed` body in one
/// frame. A catastrophic gap (resume-from-sleep) is clamped to this and the
/// anchor snapped forward, mirroring the scheduler's `MAX_CATCH_UP_PER_DISPATCH`
/// safety bail. 256 covers a ~1.3 s stall at the 5 ms cursor cadence.
const MAX_FIXED_STEPS_PER_FRAME: u64 = 256;

/// A value a script publishes via `bulanci.bind(key, value)`; copied into
/// every control property bound to `key` (see [`SceneHost::apply_bindings`])
/// once per frame. Numbers cover positions, frames, and packed RGBA tints.
#[derive(Clone, Debug)]
enum BindValue {
    Num(f64),
    Bool(bool),
    Str(String),
}

/// Mutable state shared between the host and the `bulanci.*` closures.
struct Inner {
    scene: Scene,
    scheduler: Scheduler,
    subs: Vec<(Phase, LuaFunction)>,
    timers: HashMap<u32, Timer>,
    /// Fixed-timestep ticks (`bulanci.on_fixed`), dispatched once per frame
    /// with the number of due steps. Live for the scene's lifetime.
    fixed: Vec<FixedTick>,
    next_timer: u32,
    /// Recycled timer ids. A self-rearming `after` loop (e.g. ruch) frees
    /// its id on every fire; reusing it keeps the scheduler's slot table
    /// bounded by the *concurrent* timer count instead of growing forever.
    free_ids: Vec<u32>,
    inputs: HashMap<(NodeId, InputEvent), LuaFunction>,
    audio_queue: Vec<u64>,
    goto: Option<String>,
    /// Lifecycle commands (`net.connect`, `app.exit`) for the client.
    commands: Vec<HostCommand>,
    /// `bulanci.on_key(name, fn)` handlers, fired on key-down this frame.
    key_handlers: Vec<(String, LuaFunction)>,
    /// `bulanci.on_char(fn)` handlers, fired per typed character.
    char_handlers: Vec<LuaFunction>,
    /// Keys that went down this frame (for `key_pressed` queries).
    keys_pressed: Vec<String>,
    /// Keys held this frame (for `key_down` queries).
    keys_down: Vec<String>,
    /// Current pointer position (for `bulanci.pointer()`).
    pointer: (f32, f32),
    /// Latest monotonic time fed to [`SceneHost::tick`]. Used as the
    /// anchor for `after`/`every` so a timer registered mid-run fires
    /// relative to *now*, not the epoch.
    now_ms: u64,
    /// Frame delta in ms (for `bulanci.dt()`).
    dt_ms: u64,
    /// Seedable PRNG state (SplitMix64) behind the `bulanci.rand` primitive
    /// — deterministic so faithful effects (e.g. ruch) and tests reproduce.
    rng: u64,
    // pointer tracking for click synthesis
    ptr_down: bool,
    press_node: Option<NodeId>,
    hover_node: Option<NodeId>,
    /// Client-provided services (fonts/VFS/atlas access).
    services: HostServices,
    /// `name -> NodeId` index for declarative controls (populated by
    /// [`SceneHost::instantiate_document`]); backs `bulanci.node(name)`.
    names: HashMap<String, NodeId>,
    /// The retained declarative document (when the scene is doc-driven), kept
    /// authoritative so editor HMR patches can mutate it + relayout. `None`
    /// for legacy Luau-module scenes (which can't be patched).
    doc: Option<SceneDoc>,
    /// Stable `ControlNode.id -> NodeRef`, the address space for HMR patches.
    ids: HashMap<String, NodeRef>,
    /// Flattened `(node, prop, key)` bindings applied each frame from
    /// [`Inner::binding_store`].
    bindings: Vec<(NodeId, String, String)>,
    /// Script-published values copied into bound control props each frame
    /// (`bulanci.bind(key, value)`).
    binding_store: HashMap<String, BindValue>,
    /// `update(dt)` hooks from scripts that returned a code-behind table,
    /// called once per frame after the Update phase.
    update_hooks: Vec<LuaFunction>,
    /// `key(name)` wildcard key-down hooks from code-behind tables.
    key_hooks: Vec<LuaFunction>,
    /// `input(node, event)` wildcard input hooks from code-behind tables,
    /// fired for every dispatched pointer event.
    input_hooks: Vec<LuaFunction>,
}

impl Inner {
    fn new() -> Self {
        Inner {
            scene: Scene::new(),
            scheduler: Scheduler::new(),
            subs: Vec::new(),
            timers: HashMap::new(),
            fixed: Vec::new(),
            next_timer: 1,
            free_ids: Vec::new(),
            inputs: HashMap::new(),
            audio_queue: Vec::new(),
            goto: None,
            commands: Vec::new(),
            key_handlers: Vec::new(),
            char_handlers: Vec::new(),
            keys_pressed: Vec::new(),
            keys_down: Vec::new(),
            pointer: (0.0, 0.0),
            now_ms: 0,
            dt_ms: 0,
            rng: 0x9E37_79B9_7F4A_7C15,
            ptr_down: false,
            press_node: None,
            hover_node: None,
            services: HostServices::default(),
            names: HashMap::new(),
            doc: None,
            ids: HashMap::new(),
            bindings: Vec::new(),
            binding_store: HashMap::new(),
            update_hooks: Vec::new(),
            key_hooks: Vec::new(),
            input_hooks: Vec::new(),
        }
    }

    /// Hand out a timer id, preferring a recycled one so the scheduler's
    /// slot table stays bounded by the number of *live* timers.
    fn alloc_timer_id(&mut self) -> u32 {
        if let Some(id) = self.free_ids.pop() {
            id
        } else {
            let id = self.next_timer;
            self.next_timer += 1;
            id
        }
    }

    /// One SplitMix64 step → a full 64-bit draw.
    fn next_rand(&mut self) -> u64 {
        self.rng = self.rng.wrapping_add(0x9E37_79B9_7F4A_7C15);
        let mut z = self.rng;
        z = (z ^ (z >> 30)).wrapping_mul(0xBF58_476D_1CE4_E5B9);
        z = (z ^ (z >> 27)).wrapping_mul(0x94D0_49BB_1331_11EB);
        z ^ (z >> 31)
    }
}

pub struct SceneHost {
    lua: Lua,
    inner: Rc<RefCell<Inner>>,
}

impl SceneHost {
    pub fn new() -> LuaResult<Self> {
        let lua = Lua::new();
        let inner = Rc::new(RefCell::new(Inner::new()));
        install_api(&lua, &inner)?;
        Ok(SceneHost { lua, inner })
    }

    /// Access the underlying Lua (for the client to register extra host
    /// functions, e.g. networking, before loading scene modules).
    pub fn lua(&self) -> &Lua {
        &self.lua
    }

    /// Evaluate a Luau library module and register its return value under
    /// `@name`, so scene modules can `require("@name")`. Used to make the
    /// base-UI widget library (and mod-provided widget packs) available
    /// before a scene loads. Both source and stripped bytecode work.
    pub fn preload_module(&self, name: &str, code: &[u8]) -> LuaResult<()> {
        let value: LuaValue = self.lua.load(code).eval()?;
        self.lua.register_module(&format!("@{name}"), value)?;
        Ok(())
    }

    /// Load + run a scene module (Luau source or stripped bytecode). The
    /// module is expected to build its tree and register behaviors via
    /// `bulanci.*`. Replaces nothing — for hot reload, build a fresh host
    /// (re-`preload_module` the widgets, then `load_module` the new bytes)
    /// and swap it in; the live-mod bridge delivers the changed bytes.
    pub fn load_module(&self, code: &[u8]) -> LuaResult<()> {
        self.lua.load(code).exec()
    }

    /// Run one of a scene's attached "code-behind" scripts (Luau source or
    /// bytecode). A script may build/drive the tree imperatively via
    /// `bulanci.*` (its body *is* its on-load), and/or `return` a table of
    /// lifecycle hooks the host wires onto the existing scheduler/input:
    ///
    /// ```luau
    /// return {
    ///   load   = function() end,        -- called once now
    ///   update = function(dt) end,      -- each frame after Update
    ///   key    = function(name) end,    -- any key-down this frame
    ///   input  = function(node, ev) end -- any pointer event
    /// }
    /// ```
    ///
    /// Multiple scripts compose: each is run in order and its hooks appended,
    /// so a scene's logic can be split across files (dials, shortcuts, ...).
    pub fn run_script(&self, code: &[u8]) -> LuaResult<()> {
        let ret: LuaValue = self.lua.load(code).eval()?;
        if let LuaValue::Table(t) = ret {
            let load_fn: Option<LuaFunction> = t.get::<Option<LuaFunction>>("load").ok().flatten();
            {
                let mut inner = self.inner.borrow_mut();
                if let Ok(Some(f)) = t.get::<Option<LuaFunction>>("update") {
                    inner.update_hooks.push(f);
                }
                if let Ok(Some(f)) = t.get::<Option<LuaFunction>>("key") {
                    inner.key_hooks.push(f);
                }
                if let Ok(Some(f)) = t.get::<Option<LuaFunction>>("input") {
                    inner.input_hooks.push(f);
                }
            }
            if let Some(f) = load_fn {
                f.call::<()>(())?;
            }
        }
        Ok(())
    }

    /// Advance one frame: dispatch input, run Pre/Update phases, fire due
    /// timers, then PostUpdate + Draw. `now_ms` is monotonic wall-clock ms.
    pub fn tick(&self, now_ms: u64, input: FrameInput) -> LuaResult<()> {
        {
            let mut inner = self.inner.borrow_mut();
            inner.dt_ms = now_ms.saturating_sub(inner.now_ms);
            inner.now_ms = now_ms;
        }
        self.dispatch_input(input)?;
        self.run_phase(Phase::PreUpdate)?;
        self.run_phase(Phase::Update)?;
        self.run_update_hooks()?;
        self.run_timers(now_ms)?;
        self.run_fixed(now_ms)?;
        self.run_phase(Phase::PostUpdate)?;
        // Bindings settle after all logic has published this frame's values,
        // just before the client reads the draw list.
        self.apply_bindings();
        self.run_phase(Phase::Draw)?;
        Ok(())
    }

    /// Copy each bound property from the binding store into its control.
    /// A no-op until a scene declares bindings. Runs without any Lua call,
    /// so it freely mutates the scene under a single borrow.
    fn apply_bindings(&self) {
        let mut inner = self.inner.borrow_mut();
        if inner.bindings.is_empty() {
            return;
        }
        // Move the list out so we can mutate `scene` while reading the store.
        let bindings = std::mem::take(&mut inner.bindings);
        for (node, prop, key) in &bindings {
            let val = match inner.binding_store.get(key) {
                Some(v) => v.clone(),
                None => continue,
            };
            if let Some(n) = inner.scene.node_mut(*node) {
                apply_one(n, prop, &val);
            }
        }
        inner.bindings = bindings;
    }

    fn run_phase(&self, phase: Phase) -> LuaResult<()> {
        let funcs: Vec<LuaFunction> = {
            let inner = self.inner.borrow();
            inner.subs.iter().filter(|(p, _)| *p == phase).map(|(_, f)| f.clone()).collect()
        };
        for f in funcs {
            f.call::<()>(())?;
        }
        Ok(())
    }

    /// Call each code-behind `update(dt)` hook once with the frame delta.
    fn run_update_hooks(&self) -> LuaResult<()> {
        let (funcs, dt) = {
            let inner = self.inner.borrow();
            (inner.update_hooks.clone(), inner.dt_ms as i64)
        };
        for f in funcs {
            f.call::<()>(dt)?;
        }
        Ok(())
    }

    fn run_timers(&self, now_ms: u64) -> LuaResult<()> {
        // Bounded loop; handlers may re-register timers (next_due_event
        // observes those immediately, like CRuch's self-reprogramming slot).
        for _ in 0..crate::scheduler::MAX_CATCH_UP_PER_DISPATCH {
            let id = match self.inner.borrow_mut().scheduler.next_due_event(now_ms) {
                Some(id) => id,
                None => break,
            };
            let (func, recurring) = {
                let inner = self.inner.borrow();
                match inner.timers.get(&id) {
                    Some(t) => (t.func.clone(), t.recurring),
                    None => continue,
                }
            };
            func.call::<()>(())?;
            if !recurring {
                let mut inner = self.inner.borrow_mut();
                if inner.timers.remove(&id).is_some() {
                    inner.free_ids.push(id);
                }
            }
        }
        Ok(())
    }

    /// Dispatch every `on_fixed` bind for this frame. Each due bind is
    /// called exactly once with the number of `interval_ms` steps that have
    /// matured since its last dispatch (drift-free; the sub-interval residue
    /// is carried). Binds are anchored lazily on first observation so one
    /// registered before the first tick never replays the gap.
    fn run_fixed(&self, now_ms: u64) -> LuaResult<()> {
        // Resolve the (handler, steps) work first; never hold the borrow
        // across a Lua call (a body may call back into `bulanci.*`).
        let mut calls: Vec<(LuaFunction, u64)> = Vec::new();
        {
            let mut inner = self.inner.borrow_mut();
            for ft in inner.fixed.iter_mut() {
                if !ft.anchored {
                    ft.last_ms = now_ms;
                    ft.anchored = true;
                    continue;
                }
                if ft.interval_ms == 0 || now_ms < ft.last_ms + ft.interval_ms {
                    continue;
                }
                let mut steps = (now_ms - ft.last_ms) / ft.interval_ms;
                if steps > MAX_FIXED_STEPS_PER_FRAME {
                    steps = MAX_FIXED_STEPS_PER_FRAME;
                    ft.last_ms = now_ms; // safety bail: drop the excess backlog
                } else {
                    ft.last_ms += steps * ft.interval_ms;
                }
                calls.push((ft.func.clone(), steps));
            }
        }
        for (f, steps) in calls {
            f.call::<()>(steps)?;
        }
        Ok(())
    }

    fn dispatch_input(&self, input: FrameInput) -> LuaResult<()> {
        // Resolve hover + transitions without holding the borrow across
        // any Lua call.
        let (hover, prev_hover, was_down, press_node) = {
            let inner = self.inner.borrow();
            let hover = inner.scene.hit_test(input.x, input.y);
            (hover, inner.hover_node, inner.ptr_down, inner.press_node)
        };

        // Pointer + keyboard frame state for queries.
        {
            let mut inner = self.inner.borrow_mut();
            inner.pointer = (input.x, input.y);
            inner.keys_pressed = input.keys_pressed.clone();
            inner.keys_down = input.keys_down.clone();
        }

        let mut fire: Vec<(NodeId, InputEvent)> = Vec::new();
        if hover != prev_hover {
            // Leave the previous region, then enter the new one.
            if let Some(p) = prev_hover {
                fire.push((p, InputEvent::Leave));
            }
            if let Some(h) = hover {
                fire.push((h, InputEvent::Hover));
            }
        }
        if input.pressed && !was_down {
            if let Some(h) = hover {
                fire.push((h, InputEvent::Press));
            }
        }
        if !input.pressed && was_down {
            if let Some(h) = hover {
                fire.push((h, InputEvent::Release));
                if Some(h) == press_node {
                    fire.push((h, InputEvent::Click));
                }
            }
        }

        // Commit pointer state.
        {
            let mut inner = self.inner.borrow_mut();
            inner.hover_node = hover;
            if input.pressed && !was_down {
                inner.press_node = hover;
            }
            if !input.pressed {
                inner.press_node = None;
            }
            inner.ptr_down = input.pressed;
        }

        let input_hooks: Vec<LuaFunction> = self.inner.borrow().input_hooks.clone();
        for (node, ev) in fire {
            let handler = self.inner.borrow().inputs.get(&(node, ev)).cloned();
            if let Some(f) = handler {
                f.call::<()>(node)?;
            }
            for f in &input_hooks {
                f.call::<()>((node, ev.name()))?;
            }
        }

        // Keyboard: fire on_key handlers for keys pressed this frame, then
        // on_char handlers per typed character. Clone handlers out first so
        // a handler may (re)subscribe without a double borrow.
        if !input.keys_pressed.is_empty() {
            let handlers: Vec<(String, LuaFunction)> = self.inner.borrow().key_handlers.clone();
            let key_hooks: Vec<LuaFunction> = self.inner.borrow().key_hooks.clone();
            for key in &input.keys_pressed {
                for (name, f) in &handlers {
                    if name == key {
                        f.call::<()>(key.clone())?;
                    }
                }
                for f in &key_hooks {
                    f.call::<()>(key.clone())?;
                }
            }
        }
        if !input.chars.is_empty() {
            let handlers: Vec<LuaFunction> = self.inner.borrow().char_handlers.clone();
            for ch in &input.chars {
                for f in &handlers {
                    f.call::<()>(ch.clone())?;
                }
            }
        }
        Ok(())
    }

    // ---- client-facing reads ------------------------------------------

    pub fn draw_list(&self) -> Vec<DrawItem> {
        self.inner.borrow().scene.draw_list()
    }

    /// Drain queued `bulanci.audio.play(handle)` requests for the client
    /// to actually play.
    pub fn drain_audio(&self) -> Vec<u64> {
        std::mem::take(&mut self.inner.borrow_mut().audio_queue)
    }

    /// Drain queued lifecycle commands (`net.connect`, `app.exit`).
    pub fn drain_commands(&self) -> Vec<HostCommand> {
        std::mem::take(&mut self.inner.borrow_mut().commands)
    }

    /// Install client-provided services (fonts/VFS/atlas access). Call once
    /// after [`SceneHost::new`] and before loading scene modules.
    pub fn set_services(&self, services: HostServices) {
        self.inner.borrow_mut().services = services;
    }

    /// Build a declarative [`SceneDoc`] into the scene: set its initial
    /// states, lay out + instantiate the control tree, and register the
    /// `name -> NodeId` index and bindings. Call after [`SceneHost::new`]
    /// (and any `preload_module`) and before running the doc's scripts, so
    /// the scripts can resolve named controls via `bulanci.node(name)`.
    pub fn instantiate_document(&self, doc: &SceneDoc) {
        let mut inner = self.inner.borrow_mut();
        let built = doc.instantiate(&mut inner.scene);
        inner.names.extend(built.names);
        inner.bindings.extend(built.bindings);
        inner.ids.extend(built.ids);
        // Retain the doc so editor HMR patches can mutate it + relayout in
        // place. Cloned once at load — cheap for a menu-sized control tree.
        inner.doc = Some(doc.clone());
    }

    /// Apply an editor **HMR patch** to the live scene in place. Returns
    /// `false` (a no-op) for a legacy module scene with no retained doc.
    ///
    /// Every surviving `NodeId` is preserved — only added/removed/reparented
    /// nodes change identity — so the Luau VM, scheduler, timers, runtime
    /// states, and scripts' cached node handles all stay valid across the
    /// edit. After applying the ops the doc is re-masked (state tags) and
    /// re-laid-out (taffy) so geometry + visibility track the new document.
    /// Runs under a single borrow with no Lua calls, so it's free to mutate
    /// the scene throughout.
    pub fn apply_patch(&self, patch: &ScenePatch) -> bool {
        let mut guard = self.inner.borrow_mut();
        let inner = &mut *guard;
        let doc = match inner.doc.as_mut() {
            Some(d) => d,
            None => return false,
        };
        let scene = &mut inner.scene;
        let ids = &mut inner.ids;
        let names = &mut inner.names;
        let bindings = &mut inner.bindings;
        let inputs = &mut inner.inputs;

        for op in &patch.ops {
            match op {
                PatchOp::State { name, on } => {
                    scene.set_state(name, *on);
                }
                PatchOp::States { list } => {
                    doc.states = list.clone();
                    let want: Vec<String> = list.iter().map(|s| s.name.clone()).collect();
                    scene.reset_states(&want);
                }
                PatchOp::Viewport { w, h } => {
                    doc.viewport = Some([*w, *h]);
                }
                PatchOp::Prop { id, node } => {
                    if let Some(dn) = patch::doc_find_mut(&mut doc.root, id) {
                        let old_name = dn.name.clone();
                        patch::copy_props(dn, node);
                        if let Some(&r) = ids.get(id) {
                            control::apply_props(scene, r, node);
                            rebind(bindings, r.primary(), &node.bindings);
                            remap_name(names, r, &old_name, &node.name);
                        }
                    }
                }
                PatchOp::Bindings { id, bindings: bs } => {
                    if let Some(dn) = patch::doc_find_mut(&mut doc.root, id) {
                        dn.bindings = bs.clone();
                    }
                    if let Some(&r) = ids.get(id) {
                        rebind(bindings, r.primary(), bs);
                    }
                }
                PatchOp::Add { parent, index, node } => {
                    if patch::doc_insert(&mut doc.root, parent, *index, (**node).clone()) {
                        let parent_node = resolve_parent(ids, parent);
                        let mut built = control::BuiltScene::default();
                        control::create_control(scene, parent_node, *index, node, &mut built);
                        ids.extend(built.ids);
                        names.extend(built.names);
                        bindings.extend(built.bindings);
                    }
                }
                PatchOp::Remove { id } => {
                    if let Some(&r) = ids.get(id) {
                        let dead: std::collections::HashSet<NodeId> =
                            scene.subtree_ids(r.primary()).into_iter().collect();
                        scene.remove(r.primary());
                        names.retain(|_, v| !dead.contains(&*v));
                        bindings.retain(|t| !dead.contains(&t.0));
                        inputs.retain(|k, _| !dead.contains(&k.0));
                    }
                    let mut sub_ids = Vec::new();
                    if let Some(dn) = patch::doc_find(&doc.root, id) {
                        patch::doc_collect_ids(dn, &mut sub_ids);
                    }
                    for cid in sub_ids {
                        ids.remove(&cid);
                    }
                    patch::doc_remove(&mut doc.root, id);
                }
                PatchOp::Reparent { id, parent, index } => {
                    if let Some(node) = patch::doc_remove(&mut doc.root, id) {
                        patch::doc_insert(&mut doc.root, parent, *index, node);
                    }
                    if let Some(&r) = ids.get(id) {
                        let parent_node = resolve_parent(ids, parent);
                        scene.reparent(r.primary(), parent_node);
                    }
                }
            }
        }

        // Re-mask (state tags) + relayout (taffy) from the updated doc so
        // visibility + geometry track the edit without recreating any node.
        control::recompute_masks(scene, &doc.root, ids);
        let vp = doc.viewport();
        control::relayout_into(scene, &doc.root, vp, ids);
        true
    }

    /// Take a pending `bulanci.scene.goto(name)` request, if any.
    pub fn take_goto(&self) -> Option<String> {
        self.inner.borrow_mut().goto.take()
    }

    pub fn set_state(&self, name: &str, on: bool) {
        self.inner.borrow_mut().scene.set_state(name, on);
    }

    pub fn is_state(&self, name: &str) -> bool {
        self.inner.borrow().scene.is_state(name)
    }

    pub fn node_count(&self) -> usize {
        self.inner.borrow().scene.node_count()
    }
}

// ---------------------------------------------------------------------
// bulanci.* binding API
// ---------------------------------------------------------------------

fn install_api(lua: &Lua, inner: &Rc<RefCell<Inner>>) -> LuaResult<()> {
    let b = lua.create_table()?;
    b.set("API_VERSION", API_VERSION)?;
    b.set("ROOT", ROOT as i64)?;

    // --- name -> handle hash (dev convenience; dist resolves at compile) ---
    b.set("handle", lua.create_function(|_, name: String| Ok(hash_path(&name) as i64))?)?;

    // --- node construction -------------------------------------------------
    macro_rules! shared {
        () => {
            Rc::clone(inner)
        };
    }

    let s = shared!();
    b.set("group", lua.create_function(move |_, parent: u32| {
        Ok(s.borrow_mut().scene.create(parent, NodeKind::Group))
    })?)?;

    // Asset references are canonical path STRINGS, hashed here in Rust.
    // (A u64 handle can't round-trip through Luau's f64 — >2^53 rounds.)
    let s = shared!();
    b.set("sprite", lua.create_function(move |_, (parent, path, frame): (u32, String, Option<u32>)| {
        let handle = hash_path(&path);
        Ok(s.borrow_mut().scene.create(parent, NodeKind::Sprite { handle, frame: frame.unwrap_or(0) }))
    })?)?;

    // text(parent, text, size?, spacing?, anchor?) — the font is picked by
    // `size` exactly like draw_t; `anchor` is "baseline" (default) or "top".
    let s = shared!();
    b.set("text", lua.create_function(move |_, (parent, text, size, spacing, anchor): (u32, String, Option<f32>, Option<i32>, Option<String>)| {
        let anchor = anchor.as_deref().and_then(TextAnchor::from_str).unwrap_or(TextAnchor::Baseline);
        Ok(s.borrow_mut().scene.create(parent, NodeKind::Text {
            text,
            size: size.unwrap_or(16.0),
            spacing: spacing.unwrap_or(0),
            anchor,
        }))
    })?)?;

    let s = shared!();
    b.set("quad", lua.create_function(move |_, (parent, w, h, color): (u32, f32, f32, i64)| {
        Ok(s.borrow_mut().scene.create(parent, NodeKind::Quad { w, h, color: color as u32 }))
    })?)?;

    let s = shared!();
    b.set("input_region", lua.create_function(move |_, (parent, w, h): (u32, f32, f32)| {
        Ok(s.borrow_mut().scene.create(parent, NodeKind::InputRegion { w, h, events: 0 }))
    })?)?;

    // --- node mutation -----------------------------------------------------
    let s = shared!();
    b.set("set_pos", lua.create_function(move |_, (id, x, y): (u32, f32, f32)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) { n.transform.x = x; n.transform.y = y; }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_scale", lua.create_function(move |_, (id, sx, sy): (u32, f32, f32)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) { n.transform.sx = sx; n.transform.sy = sy; }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_rot", lua.create_function(move |_, (id, rot): (u32, f32)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) { n.transform.rot = rot; }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_visible", lua.create_function(move |_, (id, vis): (u32, bool)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) { n.visible = vis; }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_text", lua.create_function(move |_, (id, text): (u32, String)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) {
            if let NodeKind::Text { text: t, .. } = &mut n.kind { *t = text; }
        }
        Ok(())
    })?)?;

    // Swap a sprite's atlas/texture (by path) and/or frame. The dial state
    // machine flips between menu_button_off/on this way.
    let s = shared!();
    b.set("set_sprite", lua.create_function(move |_, (id, path, frame): (u32, String, Option<u32>)| {
        let handle = hash_path(&path);
        if let Some(n) = s.borrow_mut().scene.node_mut(id) {
            if let NodeKind::Sprite { handle: h, frame: f } = &mut n.kind {
                *h = handle;
                if let Some(fr) = frame { *f = fr; }
            }
        }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_frame", lua.create_function(move |_, (id, frame): (u32, u32)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) {
            if let NodeKind::Sprite { frame: f, .. } = &mut n.kind { *f = frame; }
        }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_tint", lua.create_function(move |_, (id, rgba): (u32, i64)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) { n.tint = rgba as u32; }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_state_mask", lua.create_function(move |_, (id, mask): (u32, i64)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) { n.state_mask = mask as u64; }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("set_name", lua.create_function(move |_, (id, name): (u32, String)| {
        if let Some(n) = s.borrow_mut().scene.node_mut(id) { n.name = Some(name); }
        Ok(())
    })?)?;

    let s = shared!();
    b.set("reparent", lua.create_function(move |_, (id, parent): (u32, u32)| {
        s.borrow_mut().scene.reparent(id, parent);
        Ok(())
    })?)?;

    // --- declarative controls: name lookup + data bindings ----------------
    // Resolve an `x:Name`d control from a SceneDoc to its NodeId (nil if the
    // scene was built imperatively or the name is unknown).
    let s = shared!();
    b.set("node", lua.create_function(move |_, name: String| {
        Ok(s.borrow().names.get(&name).copied())
    })?)?;

    // Publish a value a declarative binding copies into a control property
    // each frame. Keeps script logic decoupled from node ids: the doc says
    // which control prop reads which key, the script just sets the key.
    let s = shared!();
    b.set("bind", lua.create_function(move |_, (key, value): (String, LuaValue)| {
        let bv = match value {
            LuaValue::Boolean(x) => BindValue::Bool(x),
            LuaValue::Integer(i) => BindValue::Num(i as f64),
            LuaValue::Number(n) => BindValue::Num(n),
            LuaValue::String(s2) => BindValue::Str(s2.to_str()?.to_string()),
            _ => return Ok(()),
        };
        s.borrow_mut().binding_store.insert(key, bv);
        Ok(())
    })?)?;

    let s = shared!();
    b.set("remove", lua.create_function(move |_, id: u32| {
        s.borrow_mut().scene.remove(id);
        Ok(())
    })?)?;

    // --- state bit allocation + masks -------------------------------------
    let s = shared!();
    b.set("state_bit", lua.create_function(move |_, name: String| {
        Ok(s.borrow_mut().scene.state_bit(&name) as i64)
    })?)?;

    // --- deterministic RNG primitive --------------------------------------
    // Lives in Rust so effects stay reproducible (and one day shareable with
    // the rollback sim). Luau gets integers in an inclusive [lo, hi] range.
    let s = shared!();
    b.set("srand", lua.create_function(move |_, seed: i64| {
        s.borrow_mut().rng = seed as u64;
        Ok(())
    })?)?;
    let s = shared!();
    b.set("rand", lua.create_function(move |_, (lo, hi): (i64, i64)| {
        let (lo, hi) = if hi >= lo { (lo, hi) } else { (hi, lo) };
        let span = (hi - lo) as u64 + 1;
        let z = s.borrow_mut().next_rand();
        Ok(lo + (z % span) as i64)
    })?)?;

    // --- behaviors: phase subscriptions + timers --------------------------
    let s = shared!();
    b.set("subscribe", lua.create_function(move |_, (phase, func): (String, LuaFunction)| {
        let p = Phase::from_str(&phase).ok_or_else(|| LuaError::RuntimeError(format!("unknown phase '{phase}'")))?;
        s.borrow_mut().subs.push((p, func));
        Ok(())
    })?)?;

    let s = shared!();
    b.set("after", lua.create_function(move |_, (ms, func): (u64, LuaFunction)| {
        let mut inner = s.borrow_mut();
        let id = inner.alloc_timer_id();
        // Anchor at *now* so a one-shot registered mid-run fires `ms` later,
        // not immediately (a fresh slot's last_fire defaults to 0).
        let base = inner.now_ms;
        inner.scheduler.register(id, ms, false, base);
        inner.timers.insert(id, Timer { func, recurring: false });
        Ok(id)
    })?)?;

    let s = shared!();
    b.set("every", lua.create_function(move |_, (ms, func): (u64, LuaFunction)| {
        let mut inner = s.borrow_mut();
        let id = inner.alloc_timer_id();
        // Anchor recurring timers at *now* too, so registering mid-run
        // doesn't unleash a catch-up burst back to the epoch.
        let base = inner.now_ms;
        inner.scheduler.register(id, ms, true, base);
        inner.timers.insert(id, Timer { func, recurring: true });
        Ok(id)
    })?)?;

    let s = shared!();
    b.set("cancel", lua.create_function(move |_, id: u32| {
        let mut inner = s.borrow_mut();
        inner.scheduler.deactivate(id);
        if inner.timers.remove(&id).is_some() {
            inner.free_ids.push(id);
        }
        Ok(())
    })?)?;

    // The first-class way to bind a hot, fixed-cadence tick action. Unlike
    // `every` (one Luau call per fired step), the host coalesces all due
    // steps for the frame and calls `func(steps)` ONCE — so a 5 ms beat is
    // a single boundary crossing per frame, and the body runs a tight
    // `for _ = 1, steps do ... end`. Drift-free + anchored on first tick
    // (no cold-start catch-up burst). Lives for the scene's lifetime.
    let s = shared!();
    b.set("on_fixed", lua.create_function(move |_, (ms, func): (u64, LuaFunction)| {
        if ms == 0 {
            return Err(LuaError::RuntimeError("on_fixed interval must be > 0".into()));
        }
        s.borrow_mut().fixed.push(FixedTick {
            interval_ms: ms,
            last_ms: 0,
            anchored: false,
            func,
        });
        Ok(())
    })?)?;

    // --- input handlers ----------------------------------------------------
    let s = shared!();
    b.set("on_input", lua.create_function(move |_, (id, event, func): (u32, String, LuaFunction)| {
        let ev = InputEvent::from_str(&event).ok_or_else(|| LuaError::RuntimeError(format!("unknown event '{event}'")))?;
        let mut inner = s.borrow_mut();
        if let Some(n) = inner.scene.node_mut(id) {
            if let NodeKind::InputRegion { events, .. } = &mut n.kind { *events |= ev.bit(); }
        }
        inner.inputs.insert((id, ev), func);
        Ok(())
    })?)?;

    // --- state sub-table ---------------------------------------------------
    let state = lua.create_table()?;
    let s = shared!();
    state.set("set", lua.create_function(move |_, (name, on): (String, bool)| {
        s.borrow_mut().scene.set_state(&name, on);
        Ok(())
    })?)?;
    let s = shared!();
    state.set("active", lua.create_function(move |_, name: String| {
        Ok(s.borrow().scene.is_state(&name))
    })?)?;
    b.set("state", state)?;

    // --- audio sub-table (queued; the client plays) -----------------------
    let audio = lua.create_table()?;
    let s = shared!();
    audio.set("play", lua.create_function(move |_, path: String| {
        let handle = hash_path(&path);
        s.borrow_mut().audio_queue.push(handle);
        Ok(())
    })?)?;
    b.set("audio", audio)?;

    // --- scene sub-table (queued transition + overlay stack) --------------
    let scene = lua.create_table()?;
    let s = shared!();
    scene.set("goto", lua.create_function(move |_, name: String| {
        s.borrow_mut().goto = Some(name);
        Ok(())
    })?)?;
    let s = shared!();
    scene.set("push", lua.create_function(move |_, name: String| {
        s.borrow_mut().commands.push(HostCommand::PushScene(name));
        Ok(())
    })?)?;
    let s = shared!();
    scene.set("pop", lua.create_function(move |_, ()| {
        s.borrow_mut().commands.push(HostCommand::PopScene);
        Ok(())
    })?)?;
    b.set("scene", scene)?;

    // --- net sub-table (lifecycle: connect) -------------------------------
    let net = lua.create_table()?;
    let s = shared!();
    net.set("connect", lua.create_function(move |_, addr: String| {
        s.borrow_mut().commands.push(HostCommand::Connect(addr));
        Ok(())
    })?)?;
    b.set("net", net)?;

    // --- app sub-table (lifecycle: exit) ----------------------------------
    let app = lua.create_table()?;
    let s = shared!();
    app.set("exit", lua.create_function(move |_, ()| {
        s.borrow_mut().commands.push(HostCommand::Exit);
        Ok(())
    })?)?;
    b.set("app", app)?;

    // --- queries: pointer / time / hover ----------------------------------
    let s = shared!();
    b.set("pointer", lua.create_function(move |_, ()| {
        let p = s.borrow().pointer;
        Ok((p.0, p.1))
    })?)?;
    let s = shared!();
    b.set("now", lua.create_function(move |_, ()| Ok(s.borrow().now_ms as i64))?)?;
    let s = shared!();
    b.set("dt", lua.create_function(move |_, ()| Ok(s.borrow().dt_ms as i64))?)?;
    let s = shared!();
    b.set("hovered", lua.create_function(move |_, id: u32| {
        Ok(s.borrow().hover_node == Some(id))
    })?)?;

    // --- keyboard ----------------------------------------------------------
    let s = shared!();
    b.set("on_key", lua.create_function(move |_, (name, func): (String, LuaFunction)| {
        s.borrow_mut().key_handlers.push((name, func));
        Ok(())
    })?)?;
    let s = shared!();
    b.set("on_char", lua.create_function(move |_, func: LuaFunction| {
        s.borrow_mut().char_handlers.push(func);
        Ok(())
    })?)?;
    let s = shared!();
    b.set("key_pressed", lua.create_function(move |_, name: String| {
        Ok(s.borrow().keys_pressed.iter().any(|k| *k == name))
    })?)?;
    let s = shared!();
    b.set("key_down", lua.create_function(move |_, name: String| {
        Ok(s.borrow().keys_down.iter().any(|k| *k == name))
    })?)?;

    // --- client services: measure / vfs read / image+atlas metadata -------
    let s = shared!();
    b.set("measure", lua.create_function(move |_, (text, size, spacing): (String, f32, Option<i32>)| {
        let f = s.borrow().services.measure.clone();
        Ok(f.map(|m| m(&text, size, spacing.unwrap_or(0))).unwrap_or(0.0))
    })?)?;
    let s = shared!();
    b.set("read_text", lua.create_function(move |_, path: String| {
        let f = s.borrow().services.read_text.clone();
        Ok(f.and_then(|r| r(&path)))
    })?)?;
    let s = shared!();
    b.set("font_metrics", lua.create_function(move |_, size: f32| {
        let f = s.borrow().services.font_metrics.clone();
        match f.map(|r| r(size)) {
            Some((lh, base)) => Ok((LuaValue::Number(lh as f64), LuaValue::Number(base as f64))),
            None => Ok((LuaValue::Nil, LuaValue::Nil)),
        }
    })?)?;
    let s = shared!();
    b.set("image_size", lua.create_function(move |_, path: String| {
        let handle = hash_path(&path);
        let f = s.borrow().services.image_size.clone();
        match f.and_then(|r| r(handle)) {
            Some((w, h)) => Ok((LuaValue::Number(w as f64), LuaValue::Number(h as f64))),
            None => Ok((LuaValue::Nil, LuaValue::Nil)),
        }
    })?)?;
    let s = shared!();
    b.set("atlas", lua.create_function(move |lua, path: String| {
        let handle = hash_path(&path);
        let f = s.borrow().services.atlas_info.clone();
        match f.and_then(|r| r(handle)) {
            Some(info) => {
                let t = lua.create_table()?;
                t.set("count", info.frame_count)?;
                t.set("ox", info.origin_x)?;
                t.set("oy", info.origin_y)?;
                let motion = lua.create_table()?;
                for (i, (dx, dy)) in info.motion.iter().enumerate() {
                    let m = lua.create_table()?;
                    m.set("dx", *dx)?;
                    m.set("dy", *dy)?;
                    motion.set(i + 1, m)?;
                }
                t.set("motion", motion)?;
                Ok(LuaValue::Table(t))
            }
            None => Ok(LuaValue::Nil),
        }
    })?)?;

    // --- log ---------------------------------------------------------------
    b.set("log", lua.create_function(|_, msg: String| {
        println!("[scene] {msg}");
        Ok(())
    })?)?;

    // Register both as a require()-able module and a global.
    lua.register_module("@bulanci", b.clone())?;
    lua.globals().set("bulanci", b)?;
    Ok(())
}

// ---------------------------------------------------------------------
// HMR patch helpers
// ---------------------------------------------------------------------

/// Resolve a patch op's `parent` field ("" = root) to an engine NodeId,
/// falling back to ROOT when the id is unknown (drifted editor state).
fn resolve_parent(ids: &HashMap<String, NodeRef>, parent: &str) -> NodeId {
    if parent.is_empty() {
        ROOT
    } else {
        ids.get(parent).map(|r| r.primary()).unwrap_or(ROOT)
    }
}

/// Replace the host's binding entries for `node` with `bs`. Bindings are
/// applied each frame, so swapping them takes effect on the next tick.
fn rebind(bindings: &mut Vec<(NodeId, String, String)>, node: NodeId, bs: &[Binding]) {
    bindings.retain(|t| t.0 != node);
    for b in bs {
        if !b.prop.is_empty() && !b.key.is_empty() {
            bindings.push((node, b.prop.clone(), b.key.clone()));
        }
    }
}

/// Keep the `name -> NodeId` index in step when a control's `x:Name` changes
/// during a live edit (so `bulanci.node(name)` resolves the new name).
fn remap_name(names: &mut HashMap<String, NodeId>, r: NodeRef, old: &str, new: &str) {
    if old == new {
        return;
    }
    if !old.is_empty() {
        names.remove(old);
        names.remove(&format!("{old}/group"));
    }
    if !new.is_empty() {
        match r {
            NodeRef::Single(id) => {
                names.insert(new.to_string(), id);
            }
            NodeRef::Button { group, region, .. } => {
                names.insert(new.to_string(), region);
                names.insert(format!("{new}/group"), group);
            }
        }
    }
}

/// Apply one resolved binding value to a control property. Type mismatches
/// (e.g. a string bound to `x`) are silently ignored — bindings are a
/// convenience layer, not a hard contract.
fn apply_one(n: &mut Node, prop: &str, val: &BindValue) {
    match prop {
        "visible" => {
            n.visible = match val {
                BindValue::Bool(b) => *b,
                BindValue::Num(x) => *x != 0.0,
                BindValue::Str(_) => n.visible,
            }
        }
        "x" => {
            if let BindValue::Num(x) = val {
                n.transform.x = *x as f32;
            }
        }
        "y" => {
            if let BindValue::Num(y) = val {
                n.transform.y = *y as f32;
            }
        }
        "tint" => {
            if let BindValue::Num(c) = val {
                n.tint = *c as u32;
            }
        }
        "frame" => {
            if let BindValue::Num(f) = val {
                if let NodeKind::Sprite { frame, .. } = &mut n.kind {
                    *frame = *f as u32;
                }
            }
        }
        "text" => {
            if let BindValue::Str(t) = val {
                if let NodeKind::Text { text, .. } = &mut n.kind {
                    *text = t.clone();
                }
            }
        }
        "color" => {
            if let BindValue::Num(c) = val {
                if let NodeKind::Quad { color, .. } = &mut n.kind {
                    *color = *c as u32;
                }
            }
        }
        _ => {}
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    const SCENE: &str = r#"
        local root = bulanci.ROOT
        bulanci.set_name(root, "root")

        -- conditional states
        local MAIN = bulanci.state_bit("main")
        bulanci.state.set("main", true)

        -- aframe of widgets
        local frame = bulanci.group(root)
        bulanci.set_pos(frame, 100, 50)
        bulanci.set_state_mask(frame, MAIN)

        local title = bulanci.text(frame, "Play", 24)

        local btn = bulanci.input_region(frame, 120, 30)
        bulanci.set_pos(btn, 0, 40)

        _G.clicks = 0
        bulanci.on_input(btn, "click", function()
            _G.clicks = _G.clicks + 1
            bulanci.audio.play("menu/sfx/click")
            bulanci.scene.goto("game")
        end)

        _G.frames = 0
        bulanci.subscribe("update", function() _G.frames = _G.frames + 1 end)

        _G.ticks = 0
        bulanci.every(100, function() _G.ticks = _G.ticks + 1 end)
    "#;

    #[test]
    fn loads_scene_builds_tree_and_runs_behaviors() {
        let host = SceneHost::new().unwrap();
        host.load_module(SCENE.as_bytes()).unwrap();

        // root + frame + title + input_region = 4
        assert_eq!(host.node_count(), 4);
        assert!(host.is_state("main"));

        // draw list: title only (group + input_region don't draw).
        let dl = host.draw_list();
        assert_eq!(dl.len(), 1);
        assert!(matches!(dl[0].kind, NodeKind::Text { .. }));
        // title world pos = frame (100,50) + local (0,0)
        assert_eq!(dl[0].world.x, 100.0);

        // run a couple of frames; update sub increments each tick.
        host.tick(0, FrameInput::default()).unwrap();
        host.tick(16, FrameInput::default()).unwrap();
        let frames: i64 = host.lua().globals().get("frames").unwrap();
        assert_eq!(frames, 2);
    }

    #[test]
    fn click_fires_handler_and_queues_commands() {
        let host = SceneHost::new().unwrap();
        host.load_module(SCENE.as_bytes()).unwrap();

        // The input region sits at world (100,90)-(220,120).
        // Press then release inside -> click.
        host.tick(0, FrameInput { x: 110.0, y: 100.0, pressed: true, ..Default::default() }).unwrap();
        host.tick(16, FrameInput { x: 110.0, y: 100.0, pressed: false, ..Default::default() }).unwrap();

        let clicks: i64 = host.lua().globals().get("clicks").unwrap();
        assert_eq!(clicks, 1);
        assert_eq!(host.drain_audio().len(), 1);
        assert_eq!(host.take_goto().as_deref(), Some("game"));
    }

    #[test]
    fn every_timer_catches_up() {
        let host = SceneHost::new().unwrap();
        host.load_module(SCENE.as_bytes()).unwrap();
        // Jump 350ms -> a 100ms recurring timer fires 3 times.
        host.tick(350, FrameInput::default()).unwrap();
        let ticks: i64 = host.lua().globals().get("ticks").unwrap();
        assert_eq!(ticks, 3);
    }

    #[test]
    fn scene_push_pop_queue_overlay_commands() {
        // `bulanci.scene.push/pop` enqueue overlay lifecycle commands that the
        // client's surface stack fulfils — proving the overlay API is wired.
        let host = SceneHost::new().unwrap();
        host.load_module(
            br#"local b = require("@bulanci")
                b.on_key("e", function() b.scene.push("exit_confirm") end)
                b.on_key("q", function() b.scene.pop() end)"#,
        )
        .unwrap();

        host.tick(
            0,
            FrameInput { keys_pressed: vec!["e".into()], ..Default::default() },
        )
        .unwrap();
        host.tick(
            16,
            FrameInput { keys_pressed: vec!["q".into()], ..Default::default() },
        )
        .unwrap();

        let cmds = host.drain_commands();
        assert_eq!(cmds.len(), 2);
        assert_eq!(cmds[0], HostCommand::PushScene("exit_confirm".into()));
        assert_eq!(cmds[1], HostCommand::PopScene);
    }

    const FIXED_SCENE: &str = r#"
        local b = require("@bulanci")
        _G.calls = 0       -- number of host->Luau crossings
        _G.steps = 0       -- total fixed steps applied
        b.on_fixed(5, function(n)
            _G.calls = _G.calls + 1
            _G.steps = _G.steps + n
        end)
    "#;

    #[test]
    fn on_fixed_anchors_on_first_tick_then_batches_steps() {
        let host = SceneHost::new().unwrap();
        host.load_module(FIXED_SCENE.as_bytes()).unwrap();
        let g = || host.lua().globals();

        // Cold start: scene built at t=0, first ticked at t=4000 (post-intro).
        // The bind anchors here and fires NOTHING (no catch-up burst).
        host.tick(4000, FrameInput::default()).unwrap();
        assert_eq!(g().get::<i64>("calls").unwrap(), 0);
        assert_eq!(g().get::<i64>("steps").unwrap(), 0);

        // +16ms -> 3 steps of 5ms, delivered in ONE call.
        host.tick(4016, FrameInput::default()).unwrap();
        assert_eq!(g().get::<i64>("calls").unwrap(), 1);
        assert_eq!(g().get::<i64>("steps").unwrap(), 3);

        // +5ms (drift-free residue from the 16ms frame: 4015 anchor) -> 1 step.
        host.tick(4021, FrameInput::default()).unwrap();
        assert_eq!(g().get::<i64>("calls").unwrap(), 2);
        assert_eq!(g().get::<i64>("steps").unwrap(), 4);
    }

    #[test]
    fn on_fixed_clamps_a_catastrophic_gap() {
        let host = SceneHost::new().unwrap();
        host.load_module(FIXED_SCENE.as_bytes()).unwrap();
        let g = || host.lua().globals();

        host.tick(0, FrameInput::default()).unwrap(); // anchor at 0
        // Resume-from-sleep: 10s gap at 5ms = 2000 steps, clamped to 256.
        host.tick(10_000, FrameInput::default()).unwrap();
        assert_eq!(g().get::<i64>("calls").unwrap(), 1);
        assert_eq!(g().get::<i64>("steps").unwrap(), 256);
        // Anchor snapped forward, so the next frame resumes cleanly.
        host.tick(10_016, FrameInput::default()).unwrap();
        assert_eq!(g().get::<i64>("steps").unwrap(), 256 + 3);
    }

    // Layered conditional states + click-driven transitions over the new
    // string-handle primitives: proves the scene-module format end to end
    // without coupling to the (separately tested) shipped menu pak.
    const STATES_SCENE: &str = r#"
        local b = require("@bulanci")
        b.state.set("main", true)
        local MAIN = b.state_bit("main")
        local OPTS = b.state_bit("options")

        -- main panel: a labelled dial + a hot region to open options.
        local main = b.group(b.ROOT)
        b.set_state_mask(main, MAIN)
        local dial = b.sprite(main, "atlases/menu_button_off/atlas.bin", 0)
        b.set_pos(dial, 35, 37)
        local open = b.input_region(main, 200, 30)
        b.set_pos(open, 80, 168)
        b.on_input(open, "click", function()
            b.state.set("main", false)
            b.state.set("options", true)
            b.audio.play("audio/sfx_start.wav")
        end)

        -- options panel: a back region.
        local opts = b.group(b.ROOT)
        b.set_state_mask(opts, OPTS)
        b.text(opts, "Options", 16)
        local back = b.input_region(opts, 200, 30)
        b.set_pos(back, 80, 160)
        b.on_input(back, "click", function()
            b.state.set("options", false)
            b.state.set("main", true)
        end)
    "#;

    #[test]
    fn states_scene_composes_and_layers() {
        let host = SceneHost::new().unwrap();
        host.load_module(STATES_SCENE.as_bytes()).unwrap();

        assert!(host.is_state("main"));
        assert!(!host.is_state("options"));
        // main panel draws the dial sprite.
        assert_eq!(host.draw_list().len(), 1);

        // Click "open options" (world (80,168), 200x30).
        host.tick(0, FrameInput { x: 120.0, y: 180.0, pressed: true, ..Default::default() }).unwrap();
        host.tick(16, FrameInput { x: 120.0, y: 180.0, pressed: false, ..Default::default() }).unwrap();
        assert!(!host.is_state("main"));
        assert!(host.is_state("options"));
        assert_eq!(host.drain_audio().len(), 1);
        // options panel draws its label text.
        assert_eq!(host.draw_list().len(), 1);

        // Click "back" (world (80,160), 200x30).
        host.tick(32, FrameInput { x: 120.0, y: 175.0, pressed: true, ..Default::default() }).unwrap();
        host.tick(48, FrameInput { x: 120.0, y: 175.0, pressed: false, ..Default::default() }).unwrap();
        assert!(host.is_state("main"));
        assert!(!host.is_state("options"));
    }

    #[test]
    fn string_handles_hash_in_rust() {
        // sprite/audio take canonical path strings, hashed host-side, so a
        // 64-bit handle never has to survive a Luau f64 round-trip.
        let host = SceneHost::new().unwrap();
        host.load_module(
            br#"
            local b = require("@bulanci")
            b.sprite(b.ROOT, "images/bg_menu.jpg")
            b.audio.play("audio/sfx_hover.wav")
            "#,
        )
        .unwrap();
        let dl = host.draw_list();
        assert_eq!(dl.len(), 1);
        match dl[0].kind {
            NodeKind::Sprite { handle, .. } => {
                assert_eq!(handle, crate::assets::hash_path("images/bg_menu.jpg"));
            }
            _ => panic!("expected sprite"),
        }
        let audio = host.drain_audio();
        assert_eq!(audio, vec![crate::assets::hash_path("audio/sfx_hover.wav")]);
    }

    #[test]
    fn declarative_doc_indexes_names_and_bindings_drive_props() {
        use super::super::document::SceneDoc;
        let host = SceneHost::new().unwrap();
        let doc = SceneDoc::from_json(
            br#"
            {
              "name": "demo",
              "root": [
                { "kind": "sprite", "name": "hero",
                  "handle": "images/hero.png",
                  "layout": { "x": 10, "y": 20 },
                  "bindings": [
                    { "prop": "x", "key": "hero_x" },
                    { "prop": "frame", "key": "hero_frame" }
                  ]
                }
              ]
            }
            "#,
        )
        .unwrap();
        host.instantiate_document(&doc);

        // A script resolves the named control and publishes binding values.
        host.load_module(
            br#"
            local b = require("@bulanci")
            assert(b.node("hero") ~= nil, "named control resolves")
            assert(b.node("missing") == nil, "unknown name -> nil")
            b.bind("hero_x", 123)
            b.bind("hero_frame", 4)
            "#,
        )
        .unwrap();

        // Before tick the binding hasn't been applied (still doc default x=10).
        assert_eq!(host.draw_list()[0].world.x, 10.0);
        host.tick(0, FrameInput::default()).unwrap();
        let dl = host.draw_list();
        assert_eq!(dl[0].world.x, 123.0);
        match dl[0].kind {
            NodeKind::Sprite { frame, .. } => assert_eq!(frame, 4),
            _ => panic!("expected sprite"),
        }
    }

    #[test]
    fn multiple_scripts_compose_with_lifecycle_hooks() {
        let host = SceneHost::new().unwrap();
        // Script A: imperative body + an update hook counting frames.
        host.run_script(
            br#"
            local b = require("@bulanci")
            _G.loaded_a = true
            _G.updates = 0
            return {
                load = function() _G.load_called = (_G.load_called or 0) + 1 end,
                update = function(dt) _G.updates = _G.updates + 1; _G.last_dt = dt end,
            }
            "#,
        )
        .unwrap();
        // Script B: a key hook (wildcard) on the SAME scene/host.
        host.run_script(
            br#"
            _G.keys = {}
            return {
                key = function(name) _G.keys[#_G.keys + 1] = name end,
            }
            "#,
        )
        .unwrap();

        let g = || host.lua().globals();
        assert!(g().get::<bool>("loaded_a").unwrap());
        assert_eq!(g().get::<i64>("load_called").unwrap(), 1);

        // Two frames -> update hook fires twice; dt is the frame delta.
        host.tick(0, FrameInput::default()).unwrap();
        host.tick(16, FrameInput::default()).unwrap();
        assert_eq!(g().get::<i64>("updates").unwrap(), 2);
        assert_eq!(g().get::<i64>("last_dt").unwrap(), 16);

        // A key-down reaches the wildcard hook from the second script.
        host.tick(
            32,
            FrameInput { keys_pressed: vec!["escape".into()], ..Default::default() },
        )
        .unwrap();
        let keys: Vec<String> = g().get("keys").unwrap();
        assert_eq!(keys, vec!["escape".to_string()]);
    }

    // The shipped base-UI fx library — proves a bespoke Rust effect
    // (client/src/ruch.rs) is fully expressible as a Luau scene behavior
    // over the primitives, with its state machine living in the closure.
    const FX: &str = include_str!("../../../paks/base_ui/lib/fx.luau");

    #[test]
    fn ruch_effect_flashes_via_luau_behavior() {
        let host = SceneHost::new().unwrap();
        host.preload_module("fx", FX.as_bytes()).unwrap();
        // One line so the visible/hidden assertion is unambiguous.
        host.load_module(
            br#"
            local fx = require("@fx")
            fx.ruch{ count = 1, width = 800, height = 600 }
            "#,
        )
        .unwrap();

        // root + the single line quad.
        assert_eq!(host.node_count(), 2);
        // Starts hidden -> nothing in the draw list.
        assert_eq!(host.draw_list().len(), 0);

        // Past the [0,4000]ms initial hidden delay the line flashes on.
        // (The re-armed 100ms visible timer fires at now+100, not now, so
        // it doesn't immediately toggle back within this tick — this is the
        // `after` anchoring fix.)
        host.tick(5000, FrameInput::default()).unwrap();
        let dl = host.draw_list();
        assert_eq!(dl.len(), 1, "ruch line should be visible after its delay");
        assert!(matches!(dl[0].kind, NodeKind::Quad { .. }));

        // After the 100ms visible window it hides again (and re-rolls).
        host.tick(5101, FrameInput::default()).unwrap();
        assert_eq!(host.draw_list().len(), 0, "ruch line hides after 100ms");
    }

    // ---- editor HMR (apply_patch) ----------------------------------------

    use super::super::control::{ControlNode, Layout};
    use super::super::document::SceneDoc;
    use super::super::patch::{PatchOp, ScenePatch};

    fn doc_host(json: &[u8]) -> SceneHost {
        let host = SceneHost::new().unwrap();
        let doc = SceneDoc::from_json(json).unwrap();
        host.instantiate_document(&doc);
        host
    }

    fn patch(ops: Vec<PatchOp>) -> ScenePatch {
        ScenePatch { surface: "demo".into(), rev: 1, ops }
    }

    #[test]
    fn hmr_prop_preserves_node_id_and_updates_kind_fields() {
        let host = doc_host(
            br#"{ "name":"demo",
                  "root":[ { "id":"t1","kind":"text","name":"title","text":"Play","size":24,
                             "layout":{ "x":10,"y":20 } } ] }"#,
        );
        let before = host.draw_list();
        assert_eq!(before.len(), 1);
        let id0 = before[0].id;

        host.apply_patch(&patch(vec![PatchOp::Prop {
            id: "t1".into(),
            node: Box::new(ControlNode {
                id: "t1".into(),
                kind: "text".into(),
                name: "title".into(),
                text: "Go".into(),
                size: 24.0,
                tint: "ff0000ff".into(),
                layout: Layout { x: 10.0, y: 20.0, ..Default::default() },
                ..Default::default()
            }),
        }]));

        let after = host.draw_list();
        assert_eq!(after.len(), 1);
        assert_eq!(after[0].id, id0, "NodeId preserved across a prop patch");
        assert_eq!(after[0].tint, 0xFF00_00FF);
        assert_eq!(after[0].world.x, 10.0);
        match &after[0].kind {
            NodeKind::Text { text, .. } => assert_eq!(text, "Go"),
            _ => panic!("expected text"),
        }
    }

    #[test]
    fn hmr_add_then_remove_preserves_surviving_ids() {
        let host = doc_host(
            br#"{ "name":"demo",
                  "root":[ { "id":"t1","kind":"text","name":"title","text":"Play","size":24 } ] }"#,
        );
        let id0 = host.draw_list()[0].id;
        let base = host.node_count();

        host.apply_patch(&patch(vec![PatchOp::Add {
            parent: String::new(),
            index: 1,
            node: Box::new(ControlNode {
                id: "q1".into(),
                kind: "quad".into(),
                color: "112233ff".into(),
                layout: Layout { x: 0.0, y: 80.0, width: Some(64.0), height: Some(12.0), ..Default::default() },
                ..Default::default()
            }),
        }]));
        assert_eq!(host.node_count(), base + 1, "added one node");
        assert!(host.draw_list().iter().any(|d| d.id == id0), "title id survives add");

        host.apply_patch(&patch(vec![PatchOp::Remove { id: "q1".into() }]));
        assert_eq!(host.node_count(), base, "removed back to baseline");
        assert!(host.draw_list().iter().any(|d| d.id == id0), "title id survives remove");
    }

    #[test]
    fn hmr_state_toggle_shows_and_hides_masked_subtree() {
        let host = doc_host(
            br#"{ "name":"demo",
                  "states":[ { "name":"main","on":false } ],
                  "root":[ { "id":"q1","kind":"quad","state":"main","color":"ffffffff",
                             "layout":{ "width":10,"height":10 } } ] }"#,
        );
        assert_eq!(host.draw_list().len(), 0, "main off -> hidden");

        host.apply_patch(&patch(vec![PatchOp::State { name: "main".into(), on: true }]));
        assert!(host.is_state("main"));
        assert_eq!(host.draw_list().len(), 1, "main on -> shown");

        host.apply_patch(&patch(vec![PatchOp::State { name: "main".into(), on: false }]));
        assert_eq!(host.draw_list().len(), 0, "main off again -> hidden");
    }

    #[test]
    fn hmr_states_op_reseeds_bits_and_remasks() {
        let host = doc_host(
            br#"{ "name":"demo",
                  "states":[ { "name":"a","on":true } ],
                  "root":[ { "id":"q1","kind":"quad","state":"a","color":"ffffffff",
                             "layout":{ "width":10,"height":10 } } ] }"#,
        );
        assert_eq!(host.draw_list().len(), 1, "state a on -> shown");

        // Reorder/extend states (b before a) AND retag the node to "b". After
        // the bit table is rebuilt deterministically, the node must follow.
        host.apply_patch(&patch(vec![
            PatchOp::States {
                list: vec![
                    super::super::document::StateInit { name: "b".into(), on: false },
                    super::super::document::StateInit { name: "a".into(), on: true },
                ],
            },
            PatchOp::Prop {
                id: "q1".into(),
                node: Box::new(ControlNode {
                    id: "q1".into(),
                    kind: "quad".into(),
                    state: "b".into(),
                    color: "ffffffff".into(),
                    layout: Layout { width: Some(10.0), height: Some(10.0), ..Default::default() },
                    ..Default::default()
                }),
            },
        ]));
        // "a" stayed active by name; node now tagged "b" (inactive) -> hidden.
        assert!(host.is_state("a"), "active set preserved by name");
        assert_eq!(host.draw_list().len(), 0, "node retagged to inactive state b");

        host.apply_patch(&patch(vec![PatchOp::State { name: "b".into(), on: true }]));
        assert_eq!(host.draw_list().len(), 1, "turning b on reveals the node");
    }

    #[test]
    fn hmr_relayout_reflows_flex_siblings() {
        let host = doc_host(
            br#"{ "name":"demo",
                  "root":[ { "id":"bar","kind":"group",
                             "layout":{ "mode":"flex","direction":"row","width":300,"height":40 },
                             "children":[
                               { "id":"a","kind":"quad","color":"ff0000ff",
                                 "layout":{ "mode":"block","width":40,"height":40 } },
                               { "id":"b","kind":"quad","color":"00ff00ff",
                                 "layout":{ "mode":"block","width":40,"height":40 } } ] } ] }"#,
        );
        let bx = |h: &SceneHost| {
            h.draw_list()
                .iter()
                .find(|d| matches!(d.kind, NodeKind::Quad { color: 0x00FF_00FF, .. }))
                .map(|d| d.world.x)
                .unwrap()
        };
        let bx0 = bx(&host);
        assert_eq!(bx0, 40.0, "b starts right of the 40px-wide a");

        // Widen a to 100 -> the flex row must push b to x=100.
        host.apply_patch(&patch(vec![PatchOp::Prop {
            id: "a".into(),
            node: Box::new(ControlNode {
                id: "a".into(),
                kind: "quad".into(),
                color: "ff0000ff".into(),
                layout: Layout { mode: "block".into(), width: Some(100.0), height: Some(40.0), ..Default::default() },
                ..Default::default()
            }),
        }]));
        assert_eq!(bx(&host), 100.0, "sibling reflowed after width change");
    }

    #[test]
    fn hmr_reparent_moves_subtree_world_position() {
        let host = doc_host(
            br#"{ "name":"demo",
                  "root":[
                    { "id":"g1","kind":"group","layout":{ "x":100,"y":0 } },
                    { "id":"g2","kind":"group","layout":{ "x":300,"y":0 },
                      "children":[ { "id":"q","kind":"quad","color":"ffffffff",
                                     "layout":{ "x":5,"y":0,"width":10,"height":10 } } ] } ] }"#,
        );
        let qx = |h: &SceneHost| h.draw_list().first().map(|d| d.world.x).unwrap();
        assert_eq!(qx(&host), 305.0, "q under g2 at 300+5");

        host.apply_patch(&patch(vec![PatchOp::Reparent {
            id: "q".into(),
            parent: "g1".into(),
            index: 0,
        }]));
        assert_eq!(qx(&host), 105.0, "q now under g1 at 100+5");
    }

    #[test]
    fn hmr_apply_patch_noop_on_module_scene() {
        let host = SceneHost::new().unwrap();
        host.load_module(br#"local b = require("@bulanci") b.text(b.ROOT, "X", 16)"#)
            .unwrap();
        assert!(!host.apply_patch(&patch(vec![PatchOp::State { name: "x".into(), on: true }])));
    }
}
