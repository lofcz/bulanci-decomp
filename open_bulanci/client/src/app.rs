//! `ClientApp` — the central state container + per-frame lifecycle.
//!
//! Mirrors the original engine's `CBulanci` shell + `CMenu` aggregate. The
//! struct owns every piece of menu and gameplay state; per-feature `impl`
//! blocks (in `menu.rs`, `startgame1.rs`, `network.rs`, etc.) hang their
//! methods off this type.
//!
//! The `update()` body is the OpenBulanci equivalent of `CDSApp_FrameBody`
//! (`@ 0x0042bda0`). See `ghidra_analysis/engine/tick_system.md`.

use std::collections::HashMap;
use std::net::{SocketAddr, UdpSocket};
use std::sync::Arc;
use chrono::Timelike;
use raylib::prelude::*;

use bulanci_core::engine::EngineSimulation;
use bulanci_core::assets::AssetFileSystem;
use bulanci_core::{FrameInputs, Player, WorldState};
use bulanci_core::{EngineClock, Scheduler};

use crate::asset::{AssetHandle, AssetServer};
use crate::asset::types::{AudioSample, Mp3};
use crate::audio::{AudioFade, AudioManager};
use crate::cursor_atlas::CursorAtlasSet;
use crate::font::BitmapFontProvider;
use crate::state::{
    AppPhase, GAME_TICK_MS, SLOT_AUDIO_FADE, SLOT_DAY_NIGHT, SLOT_GAME_TICK,
};
use crate::surface::{Surface, SurfaceLayer, SurfaceStack};

/// Build the small magenta placeholder texture handed back by
/// [`ClientApp::texture`] for any un-warmed handle (a visible "this asset
/// is missing" marker rather than a silent blank).
fn make_fallback_texture(rl: &mut RaylibHandle, thread: &RaylibThread) -> Texture2D {
    let img = Image::gen_image_color(2, 2, Color::new(255, 0, 255, 255));
    rl.load_texture_from_image(thread, &img)
        .expect("fallback placeholder texture alloc")
}

pub struct ClientApp {
    pub phase: AppPhase,
    pub exit_at_ms: Option<u64>,

    // Engine clock + scheduler — the heart of the app's timing.
    // EngineClock mirrors the original `g_dwElapsedMs` global (refreshed
    // once per frame at the top of CDSApp_FrameBody). All periodic
    // activity in the menu and gameplay flows through `scheduler` so it
    // catches up drift-free during frame stalls. See
    // ghidra_analysis/engine/tick_system.md.
    pub clock: EngineClock,
    pub last_frame_ms: u64,
    pub scheduler: Scheduler,
    pub menu_started: bool,

    // Network
    pub socket: Option<UdpSocket>,
    pub server_addr: SocketAddr,
    pub assigned_id: Option<u8>,
    pub player_name: String,

    // Assets — every typed load funnels through [`AssetServer`].  No
    // call site outside of this module sees a string path: handles
    // come from `crate::generated::assets`, the server caches
    // `Arc<[u8]>` per hash, hands back class-typed placeholders on
    // misses, and exposes [`AssetServer::drain_dirty`] for the
    // (phase 2) hot-reload tick.  The raw [`AssetFileSystem`] is
    // *not* exposed on the struct: anything that needs path-keyed
    // access can call [`AssetServer::vfs`] but new code should use
    // typed handles only — keeping the only public surface area
    // typed is what stops slugs from accidentally re-entering call
    // sites and re-leaking into `.rodata`.
    pub assets: Arc<AssetServer>,
    pub audio: Option<AudioManager>,
    /// Active text renderer. Implements [`crate::font::FontProvider`] so
    /// every screen module renders text through one indirection — swap
    /// the field's type to plug in a different provider (TTF, GPU SDF,
    /// etc.) without touching any of the call sites.
    pub fonts: BitmapFontProvider,
    /// Texture cache keyed by the raw 64-bit hash of the source
    /// [`AssetHandle`].  Decoded GPU textures are kept here so the
    /// per-frame draw doesn't re-decode the JPEG/PNG every time.
    /// A hot-reload of an asset evicts the matching entry inside
    /// `update()`'s `drain_dirty` block (see below) so the next
    /// `get_texture_h` re-uploads from the new bytes.
    pub textures: HashMap<u64, Texture2D>,
    /// Magenta placeholder returned by [`ClientApp::texture`] for any
    /// handle that somehow wasn't warmed (should never happen — every
    /// drawn handle is in `warm_textures`). Kept resident so the draw
    /// path can hand back a `&Texture2D` without an `Option`.
    pub fallback_texture: Texture2D,
    /// Hot-reload watcher root + state.  Held for the lifetime of
    /// the app; dropping it would stop the watcher.  `None` on
    /// platforms / configurations where the mods directory couldn't
    /// be located (web, or release builds without a `mods/` sibling
    /// of the executable).  The editor → engine bridge (Phase 2b)
    /// pushes overlays directly through `AssetServer::install_overlay`
    /// and doesn't need this.  Never read explicitly — its `Drop`
    /// stops the `notify` watcher — so silence the lint.
    #[cfg(not(target_arch = "wasm32"))]
    #[allow(dead_code)]
    pub mods: Option<crate::asset::mods::Mods>,
    /// Editor → engine live-mod link.  Held for the lifetime of the app
    /// so its background WebSocket thread stays alive (its `Drop` closes
    /// the socket and joins the thread).  `None` unless `BULANCI_DEV_EDITOR`
    /// is set.  Never read explicitly — silence the lint.
    #[cfg(not(target_arch = "wasm32"))]
    #[allow(dead_code)]
    pub dev_link: Option<crate::asset::ws_dev::WsDevClient>,

    // Day/night ambient music latch
    pub day_night_hour: i32, // -1 = unset
    pub no_main_menu_bg: bool,

    /// The active surface stack — the screen's base scene (the menu at boot)
    /// plus any pushed overlays (dialogs / HUD / cursor). Driven through the
    /// core scene router. Empty only if the base-UI pak is missing, in which
    /// case the renderer falls back to a bare background. `scene.goto(name)`
    /// swaps the base in place, `scene.push/pop` manage overlays, and editor
    /// HMR patches route to whichever surface owns the target. See
    /// [`crate::surface`] and [`crate::scene_runtime`].
    pub surfaces: SurfaceStack,

    // Per-frame input snapshot (sampled once at the top of `update` from
    // the live `RaylibHandle`, then read by the `&self` draw path and the
    // sub-screen input handlers — keeps drawing pure and input centralised).
    /// OS cursor mapped into the fixed 800×600 logical space.
    pub mouse_logical: (f32, f32),
    /// Left mouse button held this frame (used by hover/press visuals).
    pub mouse_left_down: bool,

    // Menu dial atlases + cursor idle-twitch tracks. The dial state machine,
    // cursor ring, poem, ruch, and every sub-screen now live in the Luau menu
    // scene; Rust keeps only these decoded GPU resources and serves their
    // frame metadata to the scene via `scene_runtime::build_services`. Loaded
    // once at boot, hot-reloaded by `apply_hot_reload`.
    pub menu_btn_off: crate::atlas::Atlas,
    pub menu_btn_on: crate::atlas::Atlas,
    pub cursor_atlas: CursorAtlasSet,

    // Game prediction state — tick advances are driven by SLOT_GAME_TICK
    // (17ms, ~58.8Hz). Local input is sampled once per frame, then fed to
    // every scheduler tick that fires that frame so the prediction catches
    // up if rendering stuttered.
    pub client_tick: u64,
    pub last_server_tick: u64,
    pub predicted_sim: EngineSimulation,
    pub authoritative_state: WorldState,
    pub input_history: HashMap<u64, FrameInputs>,
    pub players_cache: HashMap<u8, Player>,
    pub pending_input: FrameInputs,
}

impl ClientApp {
    pub fn new(
        rl: &mut RaylibHandle,
        thread: &RaylibThread,
        server_ip: &str,
        vfs: Arc<AssetFileSystem>,
        audio: Option<AudioManager>,
        no_main_menu_bg: bool,
    ) -> Self {
        // Build the engine clock immediately. The CMenu-owned scheduler
        // slots are armed only when the original would construct CMenu:
        // after the modal CAdvertising splash returns and the 0xcc menu
        // event is delivered.
        let mut clock = EngineClock::new();
        clock.tick();
        let now_ms = clock.elapsed_ms;
        let scheduler = Scheduler::new();

        // Build the typed asset server.  The shipped pack is keyed
        // by the same `blake2b("<folder>/<slug>")` digest the codegen
        // baked into each `AssetHandle`, so the server resolves bytes
        // with a single VFS lookup — no manifest, no path table, no
        // slug or folder string anywhere in `.rodata`.  See
        // `open_bulanci/client/src/asset/server.rs` for details.
        let assets = Arc::new(AssetServer::new(vfs.clone()));
        eprintln!("[asset] server ready — {} packed entries", assets.handle_count());

        // Web build: expose the server to the JS overlay bridge so the
        // asset studio (over its dev-server WebSocket broker) can push live
        // overrides into this tab. Native uses the disk watcher + HTTP
        // bridge below instead.
        #[cfg(target_arch = "wasm32")]
        crate::asset::web_bridge::init(assets.clone());

        // Bootstrap the mods/hot-reload system. The watcher (if it
        // initialised) keeps running for the lifetime of `ClientApp`
        // and pushes disk changes into `assets.install_overlay`,
        // which marks the matching handle dirty.  `update()`'s
        // `drain_dirty` block then drops the GPU texture / decoded
        // audio so the next access re-loads from the new bytes.
        #[cfg(not(target_arch = "wasm32"))]
        let mods = crate::asset::mods::Mods::bootstrap(assets.clone());
        #[cfg(not(target_arch = "wasm32"))]
        if let Some(ref m) = mods {
            if m.initial_loaded > 0 {
                eprintln!("[mods] running with {} active override(s)", m.initial_loaded);
            }
        }

        // Editor → engine live-mod link.  Off unless `BULANCI_DEV_EDITOR`
        // is set; connects *out* to the asset studio's broker and shares the
        // same `AssetServer` as the watcher, so a WS push and a disk save
        // funnel through the identical `install_overlay` → dirty → tick path.
        // On connect the broker replays its current overlay set, so a
        // (re)opened client catches up to the editor's live state.
        #[cfg(not(target_arch = "wasm32"))]
        let dev_link = crate::asset::ws_dev::WsDevClient::start(assets.clone());

        // Original-game bitmap fonts (CDSFont, ClassID 54). Failure to
        // load is fatal because every UI string goes through them, but
        // individual missing files fall through to the magenta /
        // empty-JSON placeholders so we get a visible rather than
        // silent failure.
        let fonts = BitmapFontProvider::from_handles(rl, thread, &assets)
            .expect("bitmap font metrics JSON failed to parse");

        // Load the three idle-twitch motion-track atlases. Failure here
        // is fatal because the cursor is what every menu interaction
        // goes through; rendering a blank dot would silently break the
        // animation→motion sync. See cursor_atlas.rs.  Track order
        // matches `DAT_004af900` (`CGunMouse_ctor`): A first, then B,
        // then C — the same order the cursor's `(rand() * 48) >> 19`
        // re-roll picks from.
        use crate::generated::assets::menu as menu_h;
        let cursor_atlas = CursorAtlasSet::load(
            rl,
            thread,
            &assets,
            menu_h::cursor::CURSOR_COMPOSITION,
            &[
                (menu_h::cursor::CURSOR_IDLE_A, menu_h::cursor::CURSOR_IDLE_A_PNG),
                (menu_h::cursor::CURSOR_IDLE_B, menu_h::cursor::CURSOR_IDLE_B_PNG),
                (menu_h::cursor::CURSOR_IDLE_C, menu_h::cursor::CURSOR_IDLE_C_PNG),
            ],
        )
        .expect("CursorAtlasSet load failed — re-run the asset pipeline");
        for (i, atlas) in cursor_atlas.atlases().iter().enumerate() {
            let engine_id = atlas.meta.get("engineSpriteId").and_then(|v| v.as_i64()).unwrap_or(0);
            let first_rect = atlas.frames.get(0).map(|f| f.rect).unwrap_or(crate::atlas::AtlasRect { x: 0, y: 0, w: 0, h: 0 });
            eprintln!(
                "[pointer-sprite] track {} = sprite {} ({} frames @ {}x{})",
                i,
                engine_id,
                atlas.frame_count(),
                first_rect.w,
                first_rect.h
            );
        }

        // Boot diagnostics — one line per known menu audio handle, so
        // a missing voice line is loud at startup instead of silent at
        // play time.  `AssetServer::is_real` is cheap (manifest +
        // VFS-has-key check, no decode) so calling it here costs us
        // nothing measurable even if the list grows.  The diagnostic
        // identifies an asset by the LAST 4 bytes of its hash (4 hex
        // chars).  That's a stable, unique fingerprint in our hash
        // space and — crucially — leaves no slug / path literal in
        // `.rodata` of the release binary.
        if audio.is_none() {
            eprintln!("[audio] AudioManager init failed — no SFX/music will play");
        } else {
            use crate::asset::handle::RawHandle;
            macro_rules! probe {
                ($handle:expr) => {{
                    let h = $handle;
                    if !assets.is_real(h) {
                        eprintln!(
                            "[audio] missing handle {:016x} (will play placeholder)",
                            RawHandle::from(h).0,
                        );
                    }
                }};
            }
            probe!(menu_h::music::DAY_AMBIENT);
            probe!(menu_h::music::NIGHT_AMBIENT);
            probe!(menu_h::sfx::SFX_START);
            probe!(menu_h::sfx::SFX_HISTORY);
            probe!(menu_h::sfx::SFX_QUIT);
            probe!(menu_h::sfx::SFX_FORCE_EXIT);
            probe!(menu_h::sfx::SFX_HOVER);
        }

        // Atlas load failures fall back to placeholder bytes inside
        // `Atlas::load`, so the expect strings stay generic — no slug
        // ever needs to appear in `.rodata`.
        let menu_btn_off = crate::atlas::Atlas::load(
            rl,
            thread,
            &assets,
            menu_h::widgets::menu_button::MENU_BUTTON_OFF,
            menu_h::widgets::menu_button::MENU_BUTTON_OFF_PNG,
        )
        .expect("menu button (off) atlas failed to parse");
        let menu_btn_on = crate::atlas::Atlas::load(
            rl,
            thread,
            &assets,
            menu_h::widgets::menu_button::MENU_BUTTON_ON,
            menu_h::widgets::menu_button::MENU_BUTTON_ON_PNG,
        )
        .expect("menu button (on) atlas failed to parse");

        let mut app = ClientApp {
            phase: AppPhase::Intro,
            exit_at_ms: None,
            clock,
            last_frame_ms: now_ms,
            scheduler,
            menu_started: false,
            socket: None,
            server_addr: server_ip.parse().unwrap_or_else(|_| "127.0.0.1:34568".parse().unwrap()),
            assigned_id: None,
            player_name: "Bulanek".to_string(),
            audio,
            fonts,
            textures: HashMap::new(),
            fallback_texture: make_fallback_texture(rl, thread),
            #[cfg(not(target_arch = "wasm32"))]
            mods,
            #[cfg(not(target_arch = "wasm32"))]
            dev_link,
            day_night_hour: -1,
            no_main_menu_bg,
            surfaces: SurfaceStack::new(),
            mouse_logical: (400.0, 300.0),
            mouse_left_down: false,
            menu_btn_off,
            menu_btn_on,
            cursor_atlas,
            assets,
            client_tick: 0,
            last_server_tick: 0,
            predicted_sim: EngineSimulation::new(),
            authoritative_state: WorldState::default(),
            input_history: HashMap::new(),
            players_cache: HashMap::new(),
            pending_input: FrameInputs::default(),
        };

        // Upload every intro/menu/sub-screen texture once up-front so the
        // per-frame `&self` draw path only does immutable cache lookups
        // (texture loading needs `&mut RaylibHandle`, which can't happen
        // inside a `begin_texture_mode`/draw scope).
        app.warm_textures(rl, thread);

        // The data-driven menu is the boot scene — the base of the surface
        // stack. Built last, after the fonts / textures / atlases it measures
        // + renders through are all resident, so `build_services` can capture
        // them. A checkout without the base-UI pak yields an empty stack
        // (logged); the menu then renders only its background until the pak is
        // mounted / hot-reloaded in.
        if let Some(menu) = crate::scene_runtime::load_menu(&app) {
            app.surfaces = SurfaceStack::from_base(Surface::scene(menu, SurfaceLayer::Base));
        }

        // The CAdvertising splash is a modal overlay over the (now-built) menu
        // base; it's popped when the splash dismisses (see
        // `enter_menu_from_intro`). Authored as `scenes/intro.scene.json` so
        // it's HMR-editable like every other screen.
        app.push_overlay("intro");

        app
    }

    /// Editor preview jump (web build): boot straight into the scene under
    /// edit. Finish the intro handoff if it's still up, drop every overlay
    /// (the splash) down to the base, then swap the base to `name`, so the
    /// embedded engine shows exactly the edited scene — which then hot-reloads
    /// in place from the broker's live overlay/patch stream.
    #[cfg(target_arch = "wasm32")]
    pub(crate) fn preview_goto(&mut self, name: &str, now_ms: u64) {
        if self.phase == AppPhase::Intro {
            self.enter_menu_from_intro(now_ms);
        }
        while self.surfaces.pop_overlay().is_some() {}
        if self.surfaces.name_at(0) == Some(name) {
            return;
        }
        match crate::scene_runtime::load_scene(self, name) {
            Some(s) => {
                self.surfaces.set_base(Surface::scene(s, SurfaceLayer::Base));
                eprintln!("[preview] goto base scene '{name}'");
            }
            None => eprintln!("[preview] scene '{name}' missing — preview unchanged"),
        }
    }

    /// Load `name` through the scene router and push it as a modal overlay on
    /// top of the current stack (intro / connecting / dialogs). A missing
    /// target is logged, not fatal — the surface beneath stays shown.
    pub(crate) fn push_overlay(&mut self, name: &str) {
        match crate::scene_runtime::load_scene(self, name) {
            Some(s) => {
                self.surfaces.push(Surface::scene(s, SurfaceLayer::Overlay));
                eprintln!("[scene] push overlay '{name}'");
            }
            None => eprintln!("[scene] overlay '{name}' missing — not shown"),
        }
    }

    // ─── Typed-handle audio entry points ─────────────────────────────
    //
    // Every audio playback goes through [`AssetServer::bytes`]: the
    // bytes are `Arc<[u8]>`-cached, missing assets fall back to the
    // class-typed silent WAV without crashing, and the hot-reload
    // seam (phase 2) flushes the cache between frames.  No `&str`
    // overload exists — string paths can't leak into `.rodata`
    // because no call site can construct one for an asset.

    /// Play a one-shot SFX from a typed [`AssetHandle<AudioSample>`].
    pub fn play_sfx_h(&self, handle: AssetHandle<AudioSample>) {
        let Some(audio) = &self.audio else { return };
        let bytes = self.assets.bytes(handle);
        audio.play_sfx(&bytes);
    }

    /// Play a one-shot SFX and return its decoded duration in ms.
    /// Used by the exit confirmation modal to schedule its retail-
    /// faithful fade-and-quit unwind off the actual sample length.
    pub fn play_sfx_with_duration_h(&self, handle: AssetHandle<AudioSample>) -> Option<u64> {
        let audio = self.audio.as_ref()?;
        let bytes = self.assets.bytes(handle);
        audio.play_sfx_with_duration(&bytes)
    }

    // ============================================================
    // Hot reload — propagate a batch of invalidated asset hashes into
    // every cache that holds *decoded* data.  The asset server's byte
    // cache was already cleared by `drain_dirty`; what's left are the
    // GPU-side / decoder-side caches that won't re-ask the server on
    // their own:
    //   * `textures`     — raw `Texture2D`s built by `get_texture_h`.
    //   * `menu_btn_*`    — long-lived button frame atlases.
    //   * `cursor_atlas`  — the three idle-twitch sprite strips.
    //   * `fonts`         — the three CDSFont bitmap atlases.
    //   * background music — the looping `bg_sink`, which holds the
    //                        decoded ambient track for the whole menu.
    // Everything else (one-shot SFX, backgrounds drawn via
    // `get_texture_h`) re-fetches from the server on its next access,
    // so it picks up new bytes for free.
    // ============================================================
    fn apply_hot_reload(
        &mut self,
        rl: &mut RaylibHandle,
        thread: &RaylibThread,
        dirty: &std::collections::HashSet<u64>,
    ) {
        // Clone the Arc so the holder reloads below can borrow the
        // server immutably while we mutate `self`'s decoded caches.
        let assets = self.assets.clone();

        // 1. Raw texture cache — drop any decoded entry whose hash is
        //    dirty, then re-warm below so the next `&self` draw still
        //    finds it (the draw path can't upload textures itself).
        for h in dirty {
            self.textures.remove(h);
        }
        self.warm_textures(rl, thread);

        // 2. Menu-button frame atlases.
        if self.menu_btn_off.touched_by(dirty) {
            if let Err(e) = self.menu_btn_off.reload(rl, thread, &assets) {
                eprintln!("[asset] menu button (off) reload failed: {e:#}");
            }
        }
        if self.menu_btn_on.touched_by(dirty) {
            if let Err(e) = self.menu_btn_on.reload(rl, thread, &assets) {
                eprintln!("[asset] menu button (on) reload failed: {e:#}");
            }
        }

        // 3. Cursor idle-track atlases + composition.
        if self.cursor_atlas.touched_by(dirty) {
            if let Err(e) = self.cursor_atlas.reload(rl, thread, &assets) {
                eprintln!("[asset] cursor atlas reload failed: {e:#}");
            }
        }

        // 4. Bitmap fonts.
        if self.fonts.touched_by(dirty) {
            if let Err(e) = self.fonts.reload(rl, thread, &assets) {
                eprintln!("[asset] font reload failed: {e:#}");
            }
        }

        // 5. Background ambient music.  The track currently playing is
        //    decoded into `bg_sink`; force a re-evaluation of the
        //    day/night switch (which re-fetches the bytes and restarts
        //    the loop with a fresh fade-in) when either ambient track
        //    changed.  Resetting `day_night_hour` defeats the early-out
        //    in `on_day_night_tick`.
        {
            use crate::generated::assets::menu::music;
            let music_dirty = dirty.contains(&music::DAY_AMBIENT.raw())
                || dirty.contains(&music::NIGHT_AMBIENT.raw());
            if music_dirty {
                self.day_night_hour = -1;
                self.on_day_night_tick();
            }
        }

        // 6. Active surfaces. A scene's doc, its attached scripts, and every
        //    library it requires are pak assets; an editor push / disk edit
        //    marks their hash dirty, so rebuild each affected surface from the
        //    fresh bytes — this is what makes the author -> push -> persist
        //    -> reload loop visible in-game for ANY scene (base or overlay).
        //    The dependency set comes from each live surface itself
        //    (`SceneManager::deps`, which includes its scripts), so only the
        //    surfaces that actually changed rebuild, in place, keeping their
        //    stack position + layer. Rebuilt textures/atlases/fonts are
        //    already re-warmed above, so the reload recaptures fresh services.
        if self.surfaces.is_empty() {
            // No surface yet (base-UI pak wasn't mounted at boot) — boot the
            // menu base once any of its candidate paths appears.
            let hash = bulanci_core::assets::hash_path;
            if crate::scene_runtime::HOT_RELOAD_PATHS.iter().any(|p| dirty.contains(&hash(p))) {
                if let Some(menu) = crate::scene_runtime::load_menu(self) {
                    self.surfaces.set_base(Surface::scene(menu, SurfaceLayer::Base));
                    eprintln!("[scene] booted menu base after pak mount");
                }
            }
        } else {
            let plan = self.surfaces.rebuild_plan(dirty);
            for (idx, name, layer) in plan {
                match crate::scene_runtime::load_scene(self, &name) {
                    Some(s) => {
                        self.surfaces.replace_at(idx, Surface::scene(s, layer));
                        eprintln!("[scene] hot-reloaded surface '{name}'");
                    }
                    None => eprintln!("[scene] hot-reload skipped — scene '{name}' missing"),
                }
            }
        }

        eprintln!("[asset] hot-reloaded {} entries this frame", dirty.len());
    }

    // ============================================================
    // Per-frame body — mirrors CDSApp_FrameBody @ 0x0042bda0:
    //   1. CDSApp_UpdateClock      — refresh g_dwElapsedMs once.
    //   2. CDSApp_PulseTasks       — dispatch all due scheduler events
    //                                (drift-free catch-up; a slot can
    //                                fire many times in one frame).
    //   3. Win32 message pump      — replaced by macroquad's frame loop +
    //                                our `poll_network()`.
    //   4. CDSApp_RenderFrame      — handled by the caller after update().
    // ============================================================
    pub fn update(&mut self, rl: &mut RaylibHandle, thread: &RaylibThread) {
        // ---- 0. Update focus status + pump streamed audio. ----
        let focused = crate::audio::is_window_focused();
        if let Some(ref audio) = self.audio {
            audio.set_focused(focused);
            // Streams the web build's music + reaps finished SFX; no-op native.
            audio.update();
        }

        // ---- 0b. Drain any hot-reload invalidations queued since
        //          the previous frame.  Free (one uncontended read
        //          + early-return) when nothing's dirty.  The file
        //          watcher (see `asset::mods`) and the editor →
        //          engine bridge both push hashes through
        //          `AssetServer::install_overlay`, which marks them
        //          dirty; here we propagate that into every cache
        //          that holds *decoded* data (GPU textures, atlases,
        //          fonts, the playing music sink) since those don't
        //          re-consult the asset server on their own.
        let dirty = self.assets.drain_dirty();
        if !dirty.is_empty() {
            self.apply_hot_reload(rl, thread, &dirty);
        }

        // ---- 0c. Drain editor scene-HMR patches and route each to the live
        //          surface that owns its target, applied in place — preserving
        //          the Luau VM, scheduler, timers and runtime state. The
        //          router tries surfaces top→bottom and ignores patches aimed
        //          at a surface that isn't currently shown. Like the dirty
        //          drain, free when nothing's streaming. ----
        let scene_patches = self.assets.drain_scene_patches();
        if !scene_patches.is_empty() {
            use bulanci_core::scene::PatchOutcome;
            for raw in &scene_patches {
                let bytes = raw.as_bytes();
                let target = bulanci_core::scene::patch::peek_surface(bytes)
                    .unwrap_or_else(|| "<unparseable>".into());
                match self.surfaces.route_patch(bytes) {
                    PatchOutcome::Applied => {
                        eprintln!("[scene-patch] applied -> '{target}' ({} bytes)", bytes.len());
                    }
                    PatchOutcome::NotMine => {
                        let stack: Vec<&str> = self.surfaces.iter().map(|s| s.name()).collect();
                        eprintln!(
                            "[scene-patch] no live surface owns '{target}' (stack now: [{}])",
                            stack.join(", "),
                        );
                    }
                    PatchOutcome::Failed => {
                        // Vite-style fallback: the surface owns this target but
                        // couldn't apply the patch in place (parse error or a
                        // non-patchable change). Mark its scene-doc hash dirty
                        // so next frame's `drain_dirty` rebuilds the surface
                        // from scratch off the current bytes — the full-doc
                        // overlay the broker replays — instead of leaving it
                        // stale. `scenes/<name>.scene.json` is the surface's
                        // doc dep (see `SceneManager::load`).
                        let doc_path = format!("scenes/{target}.scene.json");
                        let hash = bulanci_core::assets::hash_path(&doc_path);
                        self.assets.mark_dirty(crate::asset::handle::RawHandle(hash));
                        eprintln!("[scene-patch] '{target}' refused patch — scheduling full rebuild");
                    }
                }
            }
        }

        // ---- 0d. Editor preview-goto (web build only). The editor's Live
        //          panel boots the embedded engine with `?scene=<name>`; the
        //          shell calls `ob_goto_scene`, queued here. Jump straight to
        //          the scene under edit so the iframe previews exactly it. ----
        #[cfg(target_arch = "wasm32")]
        if let Some(name) = crate::asset::web_bridge::take_preview_goto() {
            let now = self.clock.elapsed_ms;
            self.preview_goto(&name, now);
        }

        // ---- 1. Refresh the monotonic engine clock. ----
        self.clock.tick();
        let now_ms = self.clock.elapsed_ms;

        // ---- 2. Sample per-frame input (mouse delta + gameplay keys).
        //         This is the equivalent of WM_MOUSEMOVE / KeyDown drain
        //         in the original wndproc. We must do it BEFORE the
        //         scheduler tick so any catch-up game-sim steps see the
        //         freshest input. The mouse position is mapped into the
        //         fixed 800×600 logical space and cached so the `&self`
        //         draw path never touches the input layer. ----
        self.mouse_logical = crate::gfx::logical_mouse_position(rl);
        self.mouse_left_down =
            rl.is_mouse_button_down(raylib::consts::MouseButton::MOUSE_BUTTON_LEFT);
        if self.phase == AppPhase::Playing {
            use raylib::consts::KeyboardKey::*;
            self.pending_input = FrameInputs {
                up:    rl.is_key_down(KEY_UP)    || rl.is_key_down(KEY_W),
                down:  rl.is_key_down(KEY_DOWN)  || rl.is_key_down(KEY_S),
                left:  rl.is_key_down(KEY_LEFT)  || rl.is_key_down(KEY_A),
                right: rl.is_key_down(KEY_RIGHT) || rl.is_key_down(KEY_D),
                shoot: rl.is_key_pressed(KEY_SPACE),
            };
        }

        // ---- 3. CDSApp_PulseTasks — drive every scheduled subsystem,
        //         with drift-free catch-up.
        //
        // We use the interleaved [`Scheduler::next_due_event`] loop so the
        // handler runs *between* a slot's consecutive catch-up iterations.
        // This matches `Scheduler_DispatchDueEvents @ 0x0042eb30`, which
        // calls `(*dispatcher)(slot_idx)` from inside its inner `while`
        // loop — see scheduler.rs and tick_system.md for the rationale.
        //
        // The critical case is `CRuch`: its handler re-programs its slot
        // from 100ms (visible phase) to a random 0..4000ms (hidden phase)
        // and back. Without interleaving, a single slow frame could
        // observe the 100ms cadence as stale and fire enough toggles to
        // entirely skip the visible window. ----
        let mut total_fires = 0u32;
        const MAX_FIRES_PER_FRAME: u32 = 4096;
        while let Some(slot_id) = self.scheduler.next_due_event(now_ms) {
            self.dispatch_slot(slot_id);
            total_fires += 1;
            if total_fires >= MAX_FIRES_PER_FRAME {
                eprintln!("[scheduler] catch-up cap hit ({MAX_FIRES_PER_FRAME}); snapping forward");
                self.scheduler.snap_all_to(now_ms);
                break;
            }
        }
        if self.exit_at_ms.is_some_and(|exit_at_ms| now_ms >= exit_at_ms) {
            request_app_exit(rl);
            return;
        }

        // ---- 4. Drain network packets. ----
        self.poll_network();

        // ---- 5. Per-phase input/UI handling (events that are NOT timer-
        //         driven — clicks, key-presses, sub-screen state). ----
        match self.phase {
            AppPhase::Intro      => self.update_intro(rl, now_ms),
            AppPhase::Menu       => self.update_surfaces(rl, now_ms),
            AppPhase::Connecting => self.update_connecting_input(rl),
            AppPhase::Playing    => {
                // The actual sim step happens inside `on_game_tick`,
                // fired by SLOT_GAME_TICK with catch-up.
            }
        }
        self.last_frame_ms = now_ms;
    }

    /// Intro-phase update: tick *only* the splash overlay (the stack top) so
    /// its Luau timers + input run, while the menu base stays dormant until the
    /// handoff. The dismiss policy (4s timeout / click / any key) lives in the
    /// intro scene now; Rust just fulfils the `scene.pop` it raises (routed to
    /// `enter_menu_from_intro` in `on_scene_command`). If the splash never
    /// mounted there's nothing to show, so we hand off immediately rather than
    /// strand the player on a blank Intro phase.
    fn update_intro(&mut self, rl: &mut RaylibHandle, now_ms: u64) {
        if self.surfaces.top().map(|s| s.name()) != Some("intro") {
            self.enter_menu_from_intro(now_ms);
            return;
        }

        let (mx, my) = self.mouse_logical;
        let input = bulanci_core::scene::FrameInput {
            x: mx,
            y: my,
            pressed: self.mouse_left_down,
            keys_pressed: collect_keys_pressed(rl),
            keys_down: Vec::new(),
            chars: collect_chars(rl),
        };

        let (audio, _goto, commands) = match self.surfaces.top() {
            Some(top) => top.tick(now_ms, input),
            None => return,
        };
        for h in audio {
            self.play_sfx_h(AssetHandle::<AudioSample>::new(h));
        }
        for cmd in commands {
            self.on_scene_command(cmd, now_ms);
        }
    }

    /// Tick the whole surface stack with this frame's input, then act on what
    /// each surface queued: play SFX handles, follow per-surface `scene.goto`,
    /// and run lifecycle commands (`net.connect`, `app.exit`, `scene.push/pop`).
    ///
    /// Every surface advances each frame so its timers keep running, but click
    /// / key **events** go only to the topmost surface (modal); pointer
    /// **position** is fed to all so e.g. a global cursor keeps following
    /// beneath a dialog. Surfaces are borrowed only to tick + drain, then
    /// released before mutating `self`, so a command can freely touch the app.
    fn update_surfaces(&mut self, rl: &mut RaylibHandle, now_ms: u64) {
        let n = self.surfaces.len();
        if n == 0 {
            return;
        }
        let (mx, my) = self.mouse_logical;
        let keys_pressed = collect_keys_pressed(rl);
        let chars = collect_chars(rl);

        let mut audio_all: Vec<u64> = Vec::new();
        let mut gotos: Vec<(usize, String)> = Vec::new();
        let mut commands: Vec<bulanci_core::scene::HostCommand> = Vec::new();
        for (i, s) in self.surfaces.iter().enumerate() {
            let is_top = i + 1 == n;
            let input = bulanci_core::scene::FrameInput {
                x: mx,
                y: my,
                pressed: is_top && self.mouse_left_down,
                keys_pressed: if is_top { keys_pressed.clone() } else { Vec::new() },
                keys_down: Vec::new(),
                chars: if is_top { chars.clone() } else { Vec::new() },
            };
            let (audio, goto, cmds) = s.tick(now_ms, input);
            audio_all.extend(audio);
            if let Some(g) = goto {
                gotos.push((i, g));
            }
            commands.extend(cmds);
        }

        for h in audio_all {
            self.play_sfx_h(AssetHandle::<AudioSample>::new(h));
        }
        for cmd in commands {
            self.on_scene_command(cmd, now_ms);
        }
        // `scene.goto(name)` — swap the surface that raised it in place. Build
        // the new one first (borrowing `&self` for services/VFS) and only then
        // move it into the stack, so a failed target leaves the surface live.
        for (idx, target) in gotos {
            if self.surfaces.name_at(idx) == Some(target.as_str()) {
                continue;
            }
            let layer = self.surfaces.layer_at(idx).unwrap_or(SurfaceLayer::Base);
            match crate::scene_runtime::load_scene(self, &target) {
                Some(s) => {
                    self.surfaces.replace_at(idx, Surface::scene(s, layer));
                    eprintln!("[scene] goto '{target}'");
                }
                None => eprintln!("[scene] goto '{target}' failed — scene missing"),
            }
        }
    }

    /// Fulfil a lifecycle command a surface raised. `now_ms` is the current
    /// frame clock, needed for the intro->menu handoff a `scene.pop` triggers.
    fn on_scene_command(&mut self, cmd: bulanci_core::scene::HostCommand, now_ms: u64) {
        use bulanci_core::scene::HostCommand;
        match cmd {
            HostCommand::Connect(addr) => {
                let parsed = addr
                    .parse()
                    .or_else(|_| format!("{addr}:34568").parse());
                match parsed {
                    Ok(sa) => {
                        self.server_addr = sa;
                        match self.try_connect() {
                            // Show the "joining the server" screen as a modal
                            // overlay over the preserved menu, so an Esc abort
                            // reveals the menu in its prior StartGame1 state.
                            Ok(()) => self.push_overlay("connecting"),
                            Err(e) => eprintln!("[scene] connect to {addr} failed: {e}"),
                        }
                    }
                    Err(_) => eprintln!("[scene] connect: unparseable address '{addr}'"),
                }
            }
            HostCommand::Exit => self.begin_retail_exit(),
            HostCommand::PushScene(name) => {
                // Build the overlay scene (borrowing `&self` for services/VFS),
                // then push it on top. A missing target is logged, not fatal.
                match crate::scene_runtime::load_scene(self, &name) {
                    Some(s) => {
                        self.surfaces.push(Surface::scene(s, SurfaceLayer::Overlay));
                        eprintln!("[scene] push overlay '{name}'");
                    }
                    None => eprintln!("[scene] push '{name}' failed — scene missing"),
                }
            }
            HostCommand::PopScene => {
                if self.phase == AppPhase::Intro {
                    // The intro splash decided (in Luau) it's done — timeout,
                    // click, or key. Run the intro->menu handoff: this pops the
                    // splash overlay and starts the menu's lifecycle (slots, the
                    // appearance sfx, day/night).
                    self.enter_menu_from_intro(now_ms);
                } else if self.surfaces.pop_overlay().is_some() {
                    eprintln!("[scene] pop overlay");
                }
            }
        }
    }

    /// Connecting-phase input — `Esc` aborts and rewinds to the menu (the
    /// Luau scene keeps its StartGame1 sub-screen open across the excursion,
    /// since the menu base stays live beneath the popped connecting overlay).
    fn update_connecting_input(&mut self, rl: &mut RaylibHandle) {
        if rl.is_key_pressed(raylib::consts::KeyboardKey::KEY_ESCAPE) {
            self.phase = AppPhase::Menu;
            self.socket = None;
            self.surfaces.pop_overlay_named("connecting");
        }
    }

    /// Fan-out for a scheduler-fired slot ID. The slot might fire many times
    /// per frame during a catch-up burst — each call corresponds to exactly
    /// one logical tick of that subsystem.
    fn dispatch_slot(&mut self, slot_id: u32) {
        // The menu's former per-effect slots (cursor ring + idle-twitch
        // frame, ruch scanlines, poem scroll) are gone — those effects run
        // on the Luau scene's own `b.every` timers now. Only the lifecycle
        // slots Rust still owns remain.
        match slot_id {
            SLOT_AUDIO_FADE  => self.on_audio_fade_tick(),
            SLOT_DAY_NIGHT   => self.on_day_night_tick(),
            SLOT_GAME_TICK if self.phase == AppPhase::Playing => self.on_game_tick(),
            _ => {}
        }
    }

    // ---- Day/night ambient music switch (CMenu::PollDayNight §2.7).
    //      Driven by SLOT_DAY_NIGHT (5s cadence). ----
    pub(crate) fn on_day_night_tick(&mut self) {
        let hour = chrono::Local::now().hour() as i32;
        if hour == self.day_night_hour {
            return;
        }
        self.day_night_hour = hour;
        if self.no_main_menu_bg {
            self.scheduler.pause(SLOT_AUDIO_FADE);
            return;
        }
        // Predicate from §9.4: day = 06:00..21:59 local.
        let is_day = (6..=21).contains(&hour);
        use crate::generated::assets::menu::music;
        let track: AssetHandle<Mp3> = if is_day {
            music::DAY_AMBIENT
        } else {
            music::NIGHT_AMBIENT
        };
        if let Some(audio) = &self.audio {
            let bytes = self.assets.bytes(track);
            audio.play_bg_music_at_percent(&bytes, 70);
            audio.start_bg_fade(AudioFade::linear_percent(100, 1));
            self.scheduler.unpause(SLOT_AUDIO_FADE, self.clock.elapsed_ms);
        }
    }

    fn on_audio_fade_tick(&mut self) {
        if let Some(audio) = &self.audio {
            if !audio.step_bg_fade() {
                self.scheduler.pause(SLOT_AUDIO_FADE);
            }
        } else {
            self.scheduler.pause(SLOT_AUDIO_FADE);
        }
    }

    /// Begin the retail app-exit unwind (formerly `CExitDlg`). Raised by the
    /// menu scene's `bulanci.app.exit()` command. Plays AudioBank slot 0x1a
    /// ("Konec hry") and schedules the actual quit off the sample's decoded
    /// length, fading the ambient music meanwhile — the retail X/close-event
    /// path, where the `CDSAudioPlayer` completion event unwound the modal.
    pub(crate) fn begin_retail_exit(&mut self) {
        if self.exit_at_ms.is_some() {
            return;
        }
        let delay_ms = self
            .play_sfx_with_duration_h(crate::generated::assets::menu::sfx::SFX_FORCE_EXIT)
            .unwrap_or(RETAIL_EXIT_FALLBACK_DELAY_MS);
        self.exit_at_ms = Some(self.clock.elapsed_ms.saturating_add(delay_ms));
        if let Some(audio) = &self.audio {
            audio.start_bg_fade(AudioFade::linear_percent(70, 1));
            self.scheduler.unpause(SLOT_AUDIO_FADE, self.clock.elapsed_ms);
        }
    }
}

/// Fallback exit delay when the "Konec hry" SFX can't be decoded for its
/// length (matches the retail clip duration).
const RETAIL_EXIT_FALLBACK_DELAY_MS: u64 = 2_083;

/// Registers the fixed (non-per-instance) scheduler slots when CMenu is built.
///
/// The menu's visual-effect slots (cursor ring/frame, poem scroll, ruch) are
/// gone — those run on the Luau scene's own `b.every` timers now. What's left
/// here are the lifecycle slots Rust still owns: ambient day/night music, its
/// fade ramp, and the gameplay simulation tick.
pub(crate) fn register_menu_slots(scheduler: &mut Scheduler, now_ms: u64) {
    // ---- Day/night ambient music — 5s poll. The very first evaluation
    //      happens synchronously when the intro hands off to CMenu; this
    //      slot then simply maintains the 5s refresh cadence. ----
    scheduler.register(SLOT_DAY_NIGHT, 5_000, true, now_ms);

    // ---- Audio fades — retail CMenu_OnMusicFadeTick cadence. Paused
    //      until an audio player arms a concrete fade. ----
    scheduler.register(SLOT_AUDIO_FADE, 120, true, now_ms);
    scheduler.pause(SLOT_AUDIO_FADE);

    // ---- Gameplay simulation tick — 17ms (~58.8Hz). Stays armed in all
    //      phases but is only consumed while AppPhase::Playing. ----
    scheduler.register(SLOT_GAME_TICK, GAME_TICK_MS, true, now_ms);
}

/// Quit the application once the retail exit delay elapses.
///
/// The two platforms can't share one mechanism: native ends the process, but
/// the browser tab can't be closed and `libc::exit()` under emscripten aborts
/// the runtime (the "panic in a function that cannot unwind" crash) because the
/// registered main loop is still on the stack. On web we instead ask the
/// game loop to cancel itself cleanly (`request_quit` → `QUIT_REQUESTED`, which
/// `game_loop::run`'s emscripten trampoline observes) and hand off to an HTML
/// "restart" overlay that can reboot the game with a page reload.
fn request_app_exit(rl: &mut RaylibHandle) {
    #[cfg(not(target_arch = "wasm32"))]
    {
        let _ = rl;
        std::process::exit(0);
    }
    #[cfg(target_arch = "wasm32")]
    {
        rl.request_quit();
        web_exit::show_restart_overlay();
    }
}

/// Map this frame's freshly-pressed keys to the lowercase engine names the
/// menu scene listens for (`bulanci.on_key` / `key_pressed`). Only the keys
/// the menu + sub-screens actually use are reported, keeping the per-frame
/// vector tiny.
fn collect_keys_pressed(rl: &mut RaylibHandle) -> Vec<String> {
    use raylib::consts::KeyboardKey::*;
    const MAP: &[(raylib::consts::KeyboardKey, &str)] = &[
        (KEY_S, "s"),
        (KEY_H, "h"),
        (KEY_K, "k"),
        (KEY_X, "x"),
        (KEY_Y, "y"),
        (KEY_N, "n"),
        (KEY_ENTER, "enter"),
        (KEY_ESCAPE, "escape"),
        (KEY_UP, "up"),
        (KEY_DOWN, "down"),
        (KEY_LEFT, "left"),
        (KEY_RIGHT, "right"),
        (KEY_HOME, "home"),
        (KEY_END, "end"),
        (KEY_BACKSPACE, "backspace"),
    ];
    let mut out = Vec::new();
    for &(key, name) in MAP {
        if rl.is_key_pressed(key) {
            out.push(name.to_string());
        }
    }
    out
}

/// Drain raylib's typed-character queue into single-codepoint strings for
/// `bulanci.on_char` (the StartGame1 IP entry box).
fn collect_chars(rl: &mut RaylibHandle) -> Vec<String> {
    let mut out = Vec::new();
    while let Some(c) = rl.get_char_pressed() {
        out.push(c.to_string());
    }
    out
}

/// Web-only bridge to the page's restart overlay (defined in `web/index.html`).
#[cfg(target_arch = "wasm32")]
mod web_exit {
    use std::ffi::CString;
    use std::os::raw::c_char;

    extern "C" {
        fn emscripten_run_script(script: *const c_char);
    }

    /// Reveal the `#restart` overlay. `obShowRestart` is idempotent, so calling
    /// this on a stray extra frame before the loop actually cancels is harmless.
    pub fn show_restart_overlay() {
        if let Ok(script) = CString::new("if(window.obShowRestart)window.obShowRestart();") {
            unsafe { emscripten_run_script(script.as_ptr()) };
        }
    }
}
