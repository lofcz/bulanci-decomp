//! Startup `CAdvertising` splash shown before `CMenu` is constructed.
//!
//! Original path:
//! `CBulanci::OnEvent(0xf7)` builds `CAdvertising`, loads resource
//! `0x1013a`, runs it modally, then enters the main menu via event `0xcc`.

use crate::app::{register_menu_slots, ClientApp};
use crate::state::AppPhase;

impl ClientApp {
    /// Deliver the post-splash menu event. This is where the original
    /// constructs `CMenu`, so all menu-owned timers and one-shot effects
    /// start here rather than at process boot.
    ///
    /// The *dismissal policy* that gets us here — the 4s timeout, a click,
    /// or any key — now lives entirely in `scenes/intro/intro.luau` (it raises
    /// `bulanci.scene.pop()` when any trigger fires). Only this menu-entry
    /// lifecycle stays in Rust, since it crosses the app-phase + scheduler
    /// boundary a scene can't own.
    pub(crate) fn enter_menu_from_intro(&mut self, now_ms: u64) {
        if self.menu_started {
            return;
        }
        self.menu_started = true;
        self.phase = AppPhase::Menu;
        self.last_frame_ms = now_ms;

        // Retract the splash overlay, revealing the menu base beneath.
        self.surfaces.pop_overlay_named("intro");

        register_menu_slots(&mut self.scheduler, now_ms);

        // CMenu ctor immediately clicks Start, playing slot 0x1b exactly
        // when the menu appears, not while the CAdvertising modal is live.
        // (The data-driven menu scene mirrors the rest of that auto-click —
        // opening the StartGame1 sub-screen + lighting the Start dial — in
        // Luau; this keeps the audio cue on the Rust lifecycle boundary so
        // it lands on the exact intro→menu transition frame.)
        self.play_sfx_h(crate::generated::assets::menu::sfx::SFX_START);
        self.on_day_night_tick();
    }
}
