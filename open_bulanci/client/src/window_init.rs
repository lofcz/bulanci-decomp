//! Initial window placement.
//!
//! Mirrors the original game's centering arithmetic from
//! `CBulanci::CDSApp_InitDirectDraw` at `0x00429990`:
//!
//! ```text
//!     sw = GetSystemMetrics(SM_CXSCREEN);  // primary monitor width
//!     sh = GetSystemMetrics(SM_CYSCREEN);  // primary monitor height
//!     cw = client_rect.right - client_rect.left;   // 800
//!     ch = client_rect.bottom - client_rect.top;   // 600
//!     SetWindowPos(g_pHwnd, HWND_TOPMOST,
//!                  (sw - cw) >> 1,  (sh - ch) >> 1,
//!                  cw, ch, SWP_SHOWWINDOW);
//! ```
//!
//! Two things deliberately differ from the original:
//!
//! 1. **No HWND_TOPMOST.** The original forced the window above every
//!    other Z-order group — pleasant on Win9x but jarring on modern
//!    multi-monitor desktops. miniquad's `set_window_position` only
//!    moves the window, it does not touch Z-order, which is exactly
//!    the modern-friendly behavior we want.
//! 2. **`(sw - cw) / 2` is computed in u32 arithmetic with an early
//!    `saturating_sub`** so a screen smaller than the client area
//!    yields `(0, 0)` instead of underflowing to a huge offset. The
//!    original's `>> 1` on an unsigned `int` would land off-screen
//!    in that pathological case.
//!
//! Cross-platform note: native macOS / Linux / Windows all support
//! window repositioning identically through raylib's `SetWindowPosition`.
//! WebAssembly (browser canvas) has no movable top-level window concept —
//! the canvas is embedded in the page DOM — so this module is a no-op
//! under `target_arch = "wasm32"`.

use raylib::prelude::RaylibHandle;

/// The client area we always render to — must match the `init().size(..)`
/// call in `main.rs`.
pub const CLIENT_WIDTH: u32 = 800;
pub const CLIENT_HEIGHT: u32 = 600;

/// Centers the application window on the primary monitor using the
/// original game's exact arithmetic. No-op when:
///
/// * we cannot determine the screen size (offscreen render, headless
///   CI, browser canvas);
/// * the screen is smaller than the client area (we'd compute a
///   negative offset that the OS would either clamp or reject —
///   leaving the window at its default position is friendlier).
pub fn center_window_on_primary_monitor(rl: &mut RaylibHandle) {
    let Some((sw, sh)) = primary_screen_size() else {
        return;
    };
    if sw < CLIENT_WIDTH || sh < CLIENT_HEIGHT {
        return;
    }
    let x = ((sw - CLIENT_WIDTH) / 2) as i32;
    let y = ((sh - CLIENT_HEIGHT) / 2) as i32;
    rl.set_window_position(x, y);
}

// ============================================================================
// Per-target primary-screen-size resolution.
// ============================================================================

/// Returns the primary monitor's pixel dimensions, or `None` if the
/// host cannot be queried. The exact API varies per platform:
///
/// * **Windows** — `GetSystemMetrics(SM_CXSCREEN / SM_CYSCREEN)`, the
///   same call the original game uses. We FFI directly into `user32`
///   to avoid pulling in a multi-thousand-line crate for two integers.
/// * **Linux / macOS / *BSD** — `display-info` enumerates monitors via
///   X11 / Wayland / AppKit and we pick the one flagged `is_primary`
///   (falling back to the first if no primary is reported, which is
///   the X11 default on a single-display setup).
/// * **WebAssembly** — always `None`. The browser DOM, not the game,
///   decides where the canvas sits in the page.
#[cfg(target_os = "windows")]
fn primary_screen_size() -> Option<(u32, u32)> {
    // No `#[link(name = "user32")]` here on purpose: `sola-raylib-sys`
    // already links `user32` (after its own rlib in the link order).
    // Re-declaring the link forces `user32.lib` *before* the raylib rlib,
    // which makes user32's `ShowCursor` import win and turns raylib's
    // static `ShowCursor` into an LNK2005 duplicate. Just declaring the
    // symbol lets it resolve against the raylib-provided `user32.lib`.
    extern "system" {
        fn GetSystemMetrics(n_index: i32) -> i32;
    }
    const SM_CXSCREEN: i32 = 0;
    const SM_CYSCREEN: i32 = 1;
    let (w, h) = unsafe {
        (
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN),
        )
    };
    if w > 0 && h > 0 {
        Some((w as u32, h as u32))
    } else {
        None
    }
}

#[cfg(all(not(target_arch = "wasm32"), not(target_os = "windows")))]
fn primary_screen_size() -> Option<(u32, u32)> {
    let infos = display_info::DisplayInfo::all().ok()?;
    let primary = infos
        .iter()
        .find(|d| d.is_primary)
        .or_else(|| infos.first())?;
    if primary.width == 0 || primary.height == 0 {
        return None;
    }
    Some((primary.width, primary.height))
}

#[cfg(target_arch = "wasm32")]
fn primary_screen_size() -> Option<(u32, u32)> {
    None
}
