//! The scene router — loads a scene *by name* from the VFS, owns the live
//! [`SceneHost`], and resolves `bulanci.scene.goto(name)` by rebuilding the
//! host for the target scene. A small name stack backs future overlay/back
//! semantics (goto replaces the top for now).
//!
//! A scene resolves in two flavours, in priority order:
//!   1. **Declarative** — `scenes/<name>.scene.json` ([`SceneDoc`]): the
//!      control tree is laid out + instantiated, then each attached script
//!      runs (code-behind).
//!   2. **Legacy module** — `scenes/<name>` (a Luau module that builds its
//!      own tree imperatively via `bulanci.*`). Lets pre-port scenes keep
//!      working unchanged during the migration.
//!
//! The host is renderer/audio-free; the client feeds [`FrameInput`] and
//! reads the draw list. Every VFS path the load touched (doc/module, the
//! attached scripts, and all candidate libraries) is recorded in [`deps`],
//! so the client's hot-reload can rebuild *this* scene whenever any of its
//! dependencies goes dirty — no hand-kept per-scene path list.
//!
//! [`deps`]: SceneManager::deps

use super::document::SceneDoc;
use super::host::{FrameInput, HostCommand, HostServices, SceneHost};
use super::patch::ScenePatch;
use super::surface::{HmrSurface, PatchOutcome};

/// Reads a VFS asset's bytes by canonical path (the client wires this to its
/// layered asset stack); `None` = not mounted.
pub type SceneReader<'a> = dyn Fn(&str) -> Option<Vec<u8>> + 'a;

pub struct SceneManager {
    host: SceneHost,
    current: String,
    /// Scene-name stack; the top is the active scene.
    stack: Vec<String>,
    /// VFS paths whose change should rebuild this scene (doc/module +
    /// scripts + candidate libs).
    deps: Vec<String>,
}

impl SceneManager {
    /// Build a manager by loading `name` with `libs` preloaded and
    /// `services` installed. `None` if the scene can't be found/built.
    pub fn load(
        name: &str,
        read: &SceneReader,
        libs: &[(&str, &str)],
        services: HostServices,
    ) -> Option<Self> {
        let (host, deps) = build_host(name, read, libs, services)?;
        Some(SceneManager { host, current: name.to_string(), stack: vec![name.to_string()], deps })
    }

    pub fn host(&self) -> &SceneHost {
        &self.host
    }

    pub fn current(&self) -> &str {
        &self.current
    }

    pub fn deps(&self) -> &[String] {
        &self.deps
    }

    pub fn stack(&self) -> &[String] {
        &self.stack
    }

    /// Rebuild the host for `name` in place (a `scene.goto`). Replaces the
    /// stack top. Returns `false` and keeps the current scene if the target
    /// can't be built — a broken edit never strands the player on a blank
    /// screen.
    pub fn swap_to(
        &mut self,
        name: &str,
        read: &SceneReader,
        libs: &[(&str, &str)],
        services: HostServices,
    ) -> bool {
        match build_host(name, read, libs, services) {
            Some((host, deps)) => {
                self.host = host;
                self.deps = deps;
                match self.stack.last_mut() {
                    Some(top) => *top = name.to_string(),
                    None => self.stack.push(name.to_string()),
                }
                self.current = name.to_string();
                true
            }
            None => false,
        }
    }

    /// Apply an editor HMR patch to the live scene, in place, when it targets
    /// the active scene. Returns `false` when the patch is for a different
    /// scene (the editor is editing something not currently shown) or the
    /// scene isn't doc-driven — the caller can then ignore it.
    pub fn apply_patch(&self, patch: &ScenePatch) -> bool {
        if patch.surface != self.current {
            return false;
        }
        self.host.apply_patch(patch)
    }

    /// Advance one frame; return queued audio handles, a pending
    /// `scene.goto`, and lifecycle commands for the client to act on.
    pub fn tick(
        &self,
        now_ms: u64,
        input: FrameInput,
    ) -> (Vec<u64>, Option<String>, Vec<HostCommand>) {
        if let Err(e) = self.host.tick(now_ms, input) {
            eprintln!("[scene] tick error: {e}");
        }
        (self.host.drain_audio(), self.host.take_goto(), self.host.drain_commands())
    }
}

/// A scene is the first [`HmrSurface`]: it routes by its active scene name,
/// rebuilds when its `deps` go dirty, and applies a [`ScenePatch`] in place.
/// This is what lets the client's surface router treat the menu — and every
/// future scene/overlay — uniformly.
impl HmrSurface for SceneManager {
    fn surface_name(&self) -> &str {
        &self.current
    }

    fn deps(&self) -> &[String] {
        &self.deps
    }

    fn apply_raw_patch(&self, raw: &[u8]) -> PatchOutcome {
        // Peek the routing key first so a patch addressed to a *different*
        // surface (possibly with a level/sim op vocabulary this scene can't
        // parse) is cleanly passed on rather than mistaken for a parse failure.
        match super::patch::peek_surface(raw) {
            Some(s) if s == self.current => {}
            Some(_) => return PatchOutcome::NotMine,
            None => return PatchOutcome::Failed,
        }
        let patch = match ScenePatch::from_json(raw) {
            Ok(p) => p,
            Err(e) => {
                eprintln!("[scene-patch] parse error: {e}");
                return PatchOutcome::Failed;
            }
        };
        if self.host.apply_patch(&patch) {
            PatchOutcome::Applied
        } else {
            // Right scene, but not doc-driven (legacy module) — can't patch.
            PatchOutcome::Failed
        }
    }

    // `needs_rebuild` uses the trait default (any dep path hash in `dirty`).
}

/// Build a fresh host for `name`, returning it plus the dependency paths.
fn build_host(
    name: &str,
    read: &SceneReader,
    libs: &[(&str, &str)],
    services: HostServices,
) -> Option<(SceneHost, Vec<String>)> {
    let host = match SceneHost::new() {
        Ok(h) => h,
        Err(e) => {
            eprintln!("[scene] host init failed: {e}");
            return None;
        }
    };
    host.set_services(services);

    let mut deps: Vec<String> = Vec::new();

    // Preload every candidate library so scenes can `require("@name")`. The
    // path is a dep even when absent, so adding a lib later hot-reloads.
    for (mod_name, path) in libs {
        deps.push((*path).to_string());
        if let Some(code) = read(path) {
            if let Err(e) = host.preload_module(mod_name, &code) {
                eprintln!("[scene] @{mod_name} preload failed: {e}");
            }
        }
    }

    let doc_path = format!("scenes/{name}.scene.json");
    let module_path = format!("scenes/{name}");
    deps.push(doc_path.clone());
    deps.push(module_path.clone());

    if let Some(bytes) = read(&doc_path) {
        let doc = match SceneDoc::from_json(&bytes) {
            Ok(d) => d,
            Err(e) => {
                eprintln!("[scene] {doc_path} parse failed: {e}");
                return None;
            }
        };
        host.instantiate_document(&doc);
        for script in &doc.scripts {
            deps.push(script.clone());
            match read(script) {
                Some(code) => {
                    if let Err(e) = host.run_script(&code) {
                        eprintln!("[scene] script {script} failed: {e}");
                    }
                }
                None => eprintln!("[scene] script {script} missing"),
            }
        }
        eprintln!("[scene] '{name}' (declarative) active: {} nodes", host.node_count());
        Some((host, deps))
    } else if let Some(code) = read(&module_path) {
        if let Err(e) = host.load_module(&code) {
            eprintln!("[scene] {module_path} load failed: {e}");
            return None;
        }
        eprintln!("[scene] '{name}' (luau module) active: {} nodes", host.node_count());
        Some((host, deps))
    } else {
        eprintln!("[scene] '{name}' not found ({doc_path} / {module_path})");
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashMap;

    fn vfs(entries: &[(&str, &[u8])]) -> HashMap<String, Vec<u8>> {
        entries.iter().map(|(k, v)| (k.to_string(), v.to_vec())).collect()
    }

    #[test]
    fn loads_declarative_scene_and_runs_scripts() {
        let fs = vfs(&[
            (
                "scenes/title.scene.json",
                br#"{ "name":"title", "scripts":["scenes/title/logic.luau"],
                     "root":[ { "kind":"text", "name":"hdr", "text":"Hi", "size":24,
                                "layout": { "x": 10, "y": 10 } } ] }"#,
            ),
            (
                "scenes/title/logic.luau",
                br#"local b = require("@bulanci")
                    b.bind("hdr_text", "Hello")
                    return { update = function() end }"#,
            ),
        ]);
        let read = move |p: &str| fs.get(p).cloned();
        let mgr = SceneManager::load("title", &read, &[], HostServices::default()).unwrap();
        assert_eq!(mgr.current(), "title");
        // Doc + scripts + both name candidates are deps.
        assert!(mgr.deps().iter().any(|d| d == "scenes/title.scene.json"));
        assert!(mgr.deps().iter().any(|d| d == "scenes/title/logic.luau"));
        // One text node drew.
        assert_eq!(mgr.host().draw_list().len(), 1);
        let (_a, goto, _c) = mgr.tick(0, FrameInput::default());
        assert!(goto.is_none());
    }

    #[test]
    fn falls_back_to_legacy_luau_module() {
        let fs = vfs(&[(
            "scenes/legacy",
            br#"local b = require("@bulanci")
                b.text(b.ROOT, "X", 16)"#,
        )]);
        let read = move |p: &str| fs.get(p).cloned();
        let mgr = SceneManager::load("legacy", &read, &[], HostServices::default()).unwrap();
        assert_eq!(mgr.host().draw_list().len(), 1);
    }

    #[test]
    fn swap_to_changes_scene_and_keeps_stack_depth() {
        let fs = vfs(&[
            ("scenes/a", b"local b=require(\"@bulanci\") b.text(b.ROOT,\"A\",16)"),
            ("scenes/b", b"local b=require(\"@bulanci\") b.text(b.ROOT,\"B\",16) b.text(b.ROOT,\"B2\",16)"),
        ]);
        let read = move |p: &str| fs.get(p).cloned();
        let mut mgr = SceneManager::load("a", &read, &[], HostServices::default()).unwrap();
        assert_eq!(mgr.host().draw_list().len(), 1);
        assert!(mgr.swap_to("b", &read, &[], HostServices::default()));
        assert_eq!(mgr.current(), "b");
        assert_eq!(mgr.stack().len(), 1);
        assert_eq!(mgr.host().draw_list().len(), 2);
    }

    #[test]
    fn missing_scene_yields_none_and_swap_keeps_current() {
        let fs = vfs(&[("scenes/a", b"local b=require(\"@bulanci\") b.text(b.ROOT,\"A\",16)")]);
        let read = move |p: &str| fs.get(p).cloned();
        assert!(SceneManager::load("nope", &read, &[], HostServices::default()).is_none());
        let mut mgr = SceneManager::load("a", &read, &[], HostServices::default()).unwrap();
        assert!(!mgr.swap_to("nope", &read, &[], HostServices::default()));
        assert_eq!(mgr.current(), "a"); // unchanged
    }
}
