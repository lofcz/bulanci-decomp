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
use macroquad::prelude::*;
use macroquad::window::miniquad::{BlendFactor, BlendState, BlendValue, Equation, PipelineParams};

use bulanci_core::engine::EngineSimulation;
use bulanci_core::assets::AssetFileSystem;
use bulanci_core::{FrameInputs, Player, WorldState};
use bulanci_core::{EngineClock, Scheduler};

use crate::audio::{AudioFade, AudioManager};
use crate::cursor::GunMouse;
use crate::cursor_atlas::CursorAtlasSet;
use crate::font::BitmapFontProvider;
use crate::poem::{load_poems, PoemLine};
use crate::ruch::Ruch;
use crate::state::{
    AppPhase, MenuBtn, SubScreen, CURSOR_TICK_MS, GAME_TICK_MS, RUCH_SLOTS,
    SLOT_CURSOR_DOT_FRAME, SLOT_CURSOR_TICK,
    SLOT_AUDIO_FADE, SLOT_DAY_NIGHT, SLOT_GAME_TICK, SLOT_POEM_SCROLL, SLOT_RUCH_0,
};

pub struct ClientApp {
    pub phase: AppPhase,
    pub sub_screen: SubScreen,
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
    pub intro_started_ms: u64,
    pub menu_started: bool,

    // Network
    pub socket: Option<UdpSocket>,
    pub server_addr: SocketAddr,
    pub assigned_id: Option<u8>,
    pub player_name: String,

    // Assets
    pub vfs: Arc<AssetFileSystem>,
    pub audio: Option<AudioManager>,
    /// Active text renderer. Implements [`crate::font::FontProvider`] so
    /// every screen module renders text through one indirection — swap
    /// the field's type to plug in a different provider (TTF, GPU SDF,
    /// etc.) without touching any of the call sites.
    pub fonts: BitmapFontProvider,
    pub textures: HashMap<String, Texture2D>,
    pub logical_render_target: RenderTarget,

    // Day/night ambient music latch
    pub day_night_hour: i32, // -1 = unset
    pub no_main_menu_bg: bool,

    // Menu interaction
    pub hovered_btn: Option<MenuBtn>,
    pub pressed_btn: Option<MenuBtn>,
    pub menu_btn_off: crate::atlas::Atlas,
    pub menu_btn_on: crate::atlas::Atlas,
    pub menu_btn_frames: [f32; 3], // current frame (0.0 to 7.0)
    pub menu_btn_tracks: [u8; 3],  // current track (0 = glow-up, 1 = dim-down)
    /// Per-button gate flag, mirroring the retail
    /// `Scheduler_GetEventSlot(CSwitch+0x80, 0)[+8] & 1` bit:
    /// `true` ⇒ the brightness animation is **paused** at the end of
    /// the last track (state-machine "rest"); `false` ⇒ the animation
    /// is dispatching frames and `PlayHoverTrack`/`PlayIdleTrack`
    /// would bail at the gate. Mouse-driven direction switches and
    /// the hover voice cue only fire while the gate is open (armed).
    /// The transition `false → true` happens when the active track
    /// reaches `frame >= 7.0` (retail's `FUN_00439b90` →
    /// `TM_PauseAndStampClock` → `Scheduler_ArmSlot` chain). See
    /// `ghidra_analysis/gameplay/main_menu_hover_audio.md` and
    /// `menu.rs::update_menu_input`.
    pub menu_btn_hover_armed: [bool; 3],

    // Poem scroller (CPoemScroller — see §9.3 of main_menu.md)
    pub poems: Vec<Vec<PoemLine>>,
    pub current_poem_idx: usize,
    pub poem_scroll_y: f32,
    pub poem_text_height: f32,
    /// Offscreen surface the poem text is rasterised into (logical 205×166,
    /// mirrors the original `CDSBmpImage` allocated in `PickNextPoem`).
    pub poem_render_target: RenderTarget,
    /// Custom material that applies the 40-px top/bottom alpha ramp to the
    /// rendered text per-pixel (mirrors the original `this[+0x118]` /
    /// `this[+0x11c]` pre-baked gradient masks).
    pub poem_material: Material,

    // CRuch actors (4 vertical scanline flashers)
    pub ruch: Vec<Ruch>,

    // CGunMouse — software-rendered red sniper-scope cursor (replaces the
    // OS cursor across the entire main-menu phase). `cursor_atlas` ships
    // the per-track frame strips + motion paths the dot plays through;
    // it's loaded once at boot and never reloaded.
    pub gun_mouse: GunMouse,
    pub cursor_atlas: CursorAtlasSet,

    // StartGame1 selection state
    pub sg1_total_players: usize, // 1..=4
    pub sg1_local_players: usize, // 1..=3
    pub sg1_server_mode: usize,   // 0 = Vytvořit (host), 1 = Připojit se (join)
    pub sg1_connection_type: usize, // 0 = TCP/IP, 1 = IPX
    pub sg1_active_item: usize,   // active item index (relative to visible list)
    pub sg1_focused_group: usize, // 0..=3, mirrors the CRadio view holding keyboard focus
    pub ip_input_buffer: String,

    // History screen (CHistoryDlg)
    pub history_page: usize,

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
        server_ip: &str,
        vfs: Arc<AssetFileSystem>,
        audio: Option<AudioManager>,
        no_main_menu_bg: bool,
    ) -> Self {
        // Original-game bitmap fonts (CDSFont, ClassID 54). Failure to
        // load is fatal because every UI string goes through them.
        let fonts = BitmapFontProvider::load(&vfs)
            .expect("missing bitmap font assets under assets/fonts/bitmap/");
        let poems = load_poems(&vfs);
        let current_poem_idx = if !poems.is_empty() {
            rand::gen_range(0, poems.len())
        } else {
            0
        };

        // 205×166 offscreen surface, matching the original game's clip
        // rect size (10, 384, 215, 550). Nearest filter so the bitmap
        // CDSFont glyphs stay pixel-perfect when this RT is composited
        // onto the (possibly upscaled by the OS / window manager)
        // backbuffer — any interpolation here turns the 1-bit glyph
        // edges into mush.
        let poem_render_target = render_target(205, 166);
        poem_render_target.texture.set_filter(FilterMode::Nearest);
        let logical_render_target = render_target(
            crate::window_init::CLIENT_WIDTH,
            crate::window_init::CLIENT_HEIGHT,
        );
        logical_render_target.texture.set_filter(FilterMode::Linear);

        // Per-pixel alpha ramp: 0 → 1 over the top 40 rows, 1 → 0 over the
        // bottom 40 rows. uv.y is normalised to [0, 1] across the 166-row
        // surface, so the breakpoints land at 40/166 ≈ 0.2410 and
        // 126/166 ≈ 0.7590 — exactly the original's `0x28`-row gradient
        // bands described in `PickNextPoem` (FUN_00425df0).
        let poem_material = load_material(
            ShaderSource::Glsl {
                vertex: r#"#version 100
attribute vec3 position;
attribute vec2 texcoord;
attribute vec4 color0;

varying lowp vec2 uv;
varying lowp vec4 color;

uniform mat4 Model;
uniform mat4 Projection;

void main() {
    gl_Position = Projection * Model * vec4(position, 1);
    color = color0 / 255.0;
    uv = texcoord;
}"#,
                fragment: r#"#version 100
precision lowp float;

varying vec4 color;
varying vec2 uv;

uniform sampler2D Texture;

void main() {
    vec4 tex = texture2D(Texture, uv) * color;
    float a = 1.0;
    if (uv.y < 0.2409638) {
        a = uv.y / 0.2409638;
    } else if (uv.y > 0.7590362) {
        a = (1.0 - uv.y) / 0.2409638;
    }
    gl_FragColor = vec4(tex.rgb, tex.a * a);
}"#,
            },
            MaterialParams {
                pipeline_params: PipelineParams {
                    color_blend: Some(BlendState::new(
                        Equation::Add,
                        BlendFactor::Value(BlendValue::SourceAlpha),
                        BlendFactor::OneMinusValue(BlendValue::SourceAlpha),
                    )),
                    ..Default::default()
                },
                ..Default::default()
            },
        )
        .expect("poem fade material failed to compile");

        // Build the engine clock immediately. The CMenu-owned scheduler
        // slots are armed only when the original would construct CMenu:
        // after the modal CAdvertising splash returns and the 0xcc menu
        // event is delivered.
        let mut clock = EngineClock::new();
        clock.tick();
        let now_ms = clock.elapsed_ms;
        let scheduler = Scheduler::new();

        let ruch: Vec<Ruch> = RUCH_SLOTS
            .iter()
            .map(|&slot| Ruch::new(800.0, slot))
            .collect();

        // Load the three idle-twitch motion-track atlases. Failure here
        // is fatal because the cursor is what every menu interaction
        // goes through; rendering a blank dot would silently break the
        // animation→motion sync. See cursor_atlas.rs.
        let cursor_atlas = CursorAtlasSet::load(&vfs)
            .expect("cursor atlases missing or malformed — \
                     re-run scripts/build_cursor_atlas.py and pack_assets.py");
        for (i, atlas) in cursor_atlas.atlases().iter().enumerate() {
            let engine_id = atlas.meta.get("engineSpriteId").and_then(|v| v.as_i64()).unwrap_or(0);
            let first_rect = atlas.frames.get(0).map(|f| f.rect).unwrap_or(crate::atlas::AtlasRect { x: 0, y: 0, w: 0, h: 0 });
            eprintln!(
                "[cursor_atlas] track {} = sprite {} ({} frames @ {}x{})",
                i,
                engine_id,
                atlas.frame_count(),
                first_rect.w,
                first_rect.h
            );
        }

        // Boot diagnostics — these one-liners help us spot a missing audio
        // backend or a stripped-out asset before anything is rendered.
        if audio.is_none() {
            eprintln!("[audio] AudioManager init failed — no SFX/music will play");
        } else {
            for asset in [
                "audio/day_ambient.mp3",
                "audio/night_ambient.mp3",
                "audio/sfx_start.wav",
                "audio/sfx_history.wav",
                "audio/sfx_quit.wav",
                "audio/sfx_force_exit.wav",
                "audio/sfx_hover.wav",
            ] {
                if vfs.read(asset).is_none() {
                    eprintln!("[audio] missing asset: {asset}");
                }
            }
        }

        let menu_btn_off = crate::atlas::Atlas::load(&vfs, "menu_button_off")
            .expect("menu_button_off atlas missing or malformed");
        let menu_btn_on = crate::atlas::Atlas::load(&vfs, "menu_button_on")
            .expect("menu_button_on atlas missing or malformed");

        let app = ClientApp {
            phase: AppPhase::Intro,
            sub_screen: SubScreen::StartGame1,
            exit_at_ms: None,
            clock,
            last_frame_ms: now_ms,
            scheduler,
            intro_started_ms: now_ms,
            menu_started: false,
            socket: None,
            server_addr: server_ip.parse().unwrap_or_else(|_| "127.0.0.1:34568".parse().unwrap()),
            assigned_id: None,
            player_name: "Bulanek".to_string(),
            vfs,
            audio,
            fonts,
            textures: HashMap::new(),
            logical_render_target,
            day_night_hour: -1,
            no_main_menu_bg,
            hovered_btn: None,
            pressed_btn: Some(MenuBtn::Start),
            menu_btn_off,
            menu_btn_on,
            menu_btn_frames: [7.0, 7.0, 7.0], // Initially fully dimmed (since track 1 at frame 7.0 is dim)
            menu_btn_tracks: [1, 1, 1],       // Initially on track 1 (dimming down / fully dim)
            // Retail seeds slot 0 with `pending=1, bit0=1` in
            // `Scheduler_RegisterEventSlot` (flags=7) — the very first
            // mouse-enter on each button consumes that pending trigger.
            menu_btn_hover_armed: [true; 3],
            poems,
            current_poem_idx,
            poem_scroll_y: 550.0,
            poem_text_height: 0.0,
            poem_render_target,
            poem_material,
            ruch,
            gun_mouse: GunMouse::new(),
            cursor_atlas,
            sg1_total_players: 3,
            sg1_local_players: 3,
            sg1_server_mode: 0,
            sg1_connection_type: 0,
            sg1_active_item: 0,
            sg1_focused_group: 0,
            ip_input_buffer: "127.0.0.1".to_string(),
            history_page: 0,
            client_tick: 0,
            last_server_tick: 0,
            predicted_sim: EngineSimulation::new(),
            authoritative_state: WorldState::default(),
            input_history: HashMap::new(),
            players_cache: HashMap::new(),
            pending_input: FrameInputs::default(),
        };

        app
    }

    pub fn play_sfx(&self, path: &str) {
        if let (Some(audio), Some(bytes)) = (&self.audio, self.vfs.read(path)) {
            audio.play_sfx(bytes);
        }
    }

    pub fn play_sfx_with_duration(&self, path: &str) -> Option<u64> {
        if let (Some(audio), Some(bytes)) = (&self.audio, self.vfs.read(path)) {
            audio.play_sfx_with_duration(bytes)
        } else {
            None
        }
    }

    pub fn play_hover_sfx(&self, path: &str) {
        if let (Some(audio), Some(bytes)) = (&self.audio, self.vfs.read(path)) {
            audio.play_hover_sfx(bytes);
        }
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
    pub fn update(&mut self) {
        // ---- 0. Update focus status for audio. ----
        let focused = crate::audio::is_window_focused();
        if let Some(ref audio) = self.audio {
            audio.set_focused(focused);
        }

        // ---- 1. Refresh the monotonic engine clock. ----
        self.clock.tick();
        let now_ms = self.clock.elapsed_ms;

        // ---- 2. Sample per-frame input (mouse delta + gameplay keys).
        //         This is the equivalent of WM_MOUSEMOVE / KeyDown drain
        //         in the original wndproc. We must do it BEFORE the
        //         scheduler tick so any catch-up game-sim steps see the
        //         freshest input. ----
        if self.phase == AppPhase::Menu {
            self.gun_mouse.update_frame();
        }
        if self.phase == AppPhase::Playing {
            self.pending_input = FrameInputs {
                up:    is_key_down(KeyCode::Up)    || is_key_down(KeyCode::W),
                down:  is_key_down(KeyCode::Down)  || is_key_down(KeyCode::S),
                left:  is_key_down(KeyCode::Left)  || is_key_down(KeyCode::A),
                right: is_key_down(KeyCode::Right) || is_key_down(KeyCode::D),
                shoot: is_key_pressed(KeyCode::Space),
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
            std::process::exit(0);
        }

        // ---- 4. Drain network packets. ----
        self.poll_network();

        // ---- 5. Per-phase input/UI handling (events that are NOT timer-
        //         driven — clicks, key-presses, sub-screen state). ----
        match self.phase {
            AppPhase::Intro      => self.update_intro_input(now_ms),
            AppPhase::Menu       => self.update_menu_input(),
            AppPhase::Connecting => self.update_connecting_input(),
            AppPhase::Playing    => {
                // The actual sim step happens inside `on_game_tick`,
                // fired by SLOT_GAME_TICK with catch-up.
            }
        }
        self.last_frame_ms = now_ms;
    }

    /// Connecting-phase input — `Esc` aborts and rewinds to the StartGame1
    /// sub-screen.
    fn update_connecting_input(&mut self) {
        if is_key_pressed(KeyCode::Escape) {
            self.phase = AppPhase::Menu;
            self.sub_screen = SubScreen::StartGame1;
            self.socket = None;
        }
    }

    /// Fan-out for a scheduler-fired slot ID. The slot might fire many times
    /// per frame during a catch-up burst — each call corresponds to exactly
    /// one logical tick of that subsystem.
    fn dispatch_slot(&mut self, slot_id: u32) {
        match slot_id {
            // SLOT_CURSOR_TICK runs the 30-slot mouse ring push + pop at
            // a fixed 5 ms cadence (with catch-up). This matches the
            // trace-empirical "lag = 30 × 5 ms = 150 ms" perception of
            // the original at era-typical fps, regardless of the
            // current render fps. See cursor.rs module docs for why
            // this is preferred over the literal per-Draw gate.
            SLOT_CURSOR_TICK        => self.gun_mouse.on_cursor_tick(),
            // SLOT_CURSOR_DOT_FRAME advances the active idle-twitch
            // track and re-rolls when its loop completes (every track
            // is closed by design, see cursor_atlas.rs). No separate
            // "drift" slot — switching at loop boundaries is what the
            // original Frida-traced behaviour does.
            SLOT_CURSOR_DOT_FRAME   => {
                self.gun_mouse
                    .on_sprite_frame_tick(self.cursor_atlas.atlases());
            }
            s if (SLOT_RUCH_0..=SLOT_RUCH_0 + 3).contains(&s) => {
                let idx = (s - SLOT_RUCH_0) as usize;
                if let Some(r) = self.ruch.get_mut(idx) {
                    let next_delay = r.on_toggle_tick(800.0);
                    self.scheduler.set_delay_ms(r.slot_id, next_delay);
                }
            }
            SLOT_POEM_SCROLL => self.on_poem_scroll_tick(),
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
        let track = if is_day {
            "audio/day_ambient.mp3"
        } else {
            "audio/night_ambient.mp3"
        };
        if let (Some(audio), Some(bytes)) = (&self.audio, self.vfs.read(track)) {
            audio.play_bg_music_at_percent(bytes, 70);
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

    // ---- Compute poem scroll text height (laid out top-down). ----
    pub fn poem_total_height(&self, poem_idx: usize) -> f32 {
        if poem_idx >= self.poems.len() {
            return 0.0;
        }
        let lines = &self.poems[poem_idx];
        // Same atlas the original `CPoemScroller` is hard-wired to
        // (resource `0x100af`, 13 px line height).
        lines.len() as f32 * self.fonts.medium.line_height as f32
    }

    /// SLOT_POEM_SCROLL handler — fires drift-free at strict 120ms cadence,
    /// matches `CPoemScroller::OnScheduledTick` (FUN_00425df0). One pixel
    /// per fire upward; loops to a new random poem when fully scrolled past
    /// the top of the rect.
    fn on_poem_scroll_tick(&mut self) {
        self.poem_scroll_y -= 1.0;
        let height = self.poem_total_height(self.current_poem_idx);
        self.poem_text_height = height;
        if self.poem_scroll_y + height < 384.0 {
            if !self.poems.is_empty() {
                self.current_poem_idx = rand::gen_range(0, self.poems.len());
            }
            self.poem_scroll_y = 550.0;
        }
    }
}

/// Registers the fixed (non-per-instance) scheduler slots when CMenu is built.
pub(crate) fn register_menu_slots(scheduler: &mut Scheduler, now_ms: u64) {
    // ---- CGunMouse slots ----
    //      Two beats: a 5 ms mouse-queue tick (push+pop) and an 85 ms
    //      sprite-frame tick (advance idle-twitch motion path). The
    //      5 ms cadence matches the trace-empirical effective push
    //      interval the original engine ran at on era hardware; see
    //      cursor.rs module docs.
    scheduler.register(SLOT_CURSOR_TICK, CURSOR_TICK_MS, true, now_ms);
    scheduler.register(SLOT_CURSOR_DOT_FRAME, 85, true, now_ms);

    // ---- CPoemScroller — 120ms cadence with 3000ms initial delay
    //      (matches CPoemScroller_ctor: Scheduler_SetEventLastFireMs to
    //      `now + 3000`, so first scroll lands at t = 3120ms). ----
    scheduler.register(SLOT_POEM_SCROLL, 120, true, now_ms);
    scheduler.set_last_fire_ms(SLOT_POEM_SCROLL, now_ms + 3_000);

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
