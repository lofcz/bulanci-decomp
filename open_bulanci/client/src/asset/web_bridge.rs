//! Editor -> engine live-overlay bridge (web build).
//!
//! The wasm twin of the native dev link (`super::ws_dev`). A browser tab
//! can't host a listening socket, so the data flow is inverted: the web shell
//! (`web/index.html`) opens a WebSocket *out* to the dev-time broker in
//! the asset studio's Vite server, receives overlay pushes, and calls
//! the exported functions here. Both ultimately reach the same
//! [`AssetServer::install_overlay`] -> `mark_dirty` -> `apply_hot_reload`
//! path as the native bridge and the disk watcher, so a web push and a
//! `Ctrl+S` are indistinguishable downstream.
//!
//! Assets are addressed purely by their 64-bit handle hash (16 hex
//! chars), so no slug or path crosses into the binary, matching the
//! no-`.rodata`-leak invariant.

#![cfg(target_arch = "wasm32")]

use std::ffi::CStr;
use std::os::raw::c_char;
use std::sync::{Arc, OnceLock};

use super::handle::RawHandle;
use super::server::AssetServer;

/// The running engine's asset server, stashed at boot so the exported
/// C functions (which can't carry Rust state) can reach it. Single
/// writer at startup; wasm is single-threaded so reads never race.
static ENGINE_SERVER: OnceLock<Arc<AssetServer>> = OnceLock::new();

/// Publish the live `AssetServer` so the JS bridge can drive overlays.
/// Called once during `ClientApp` construction.
pub fn init(server: Arc<AssetServer>) {
    let _ = ENGINE_SERVER.set(server);
}

/// Decode a NUL-terminated 16-hex-char handle hash from JS.
///
/// # Safety
/// `ptr` must be a valid C string pointer (or null), as passed by
/// emscripten's `ccall` `'string'` marshalling.
unsafe fn hash_from_hex(ptr: *const c_char) -> Option<u64> {
    if ptr.is_null() {
        return None;
    }
    let text = CStr::from_ptr(ptr).to_str().ok()?;
    u64::from_str_radix(text.trim(), 16).ok()
}

/// Install (or replace) a live overlay for `hash_hex` with `len` bytes
/// read from `data`. Exported to the web shell via
/// `Module.ccall('ob_install_overlay', ...)`; see the `EXPORTED_FUNCTIONS`
/// link arg in `.cargo/config.toml`. A no-op before [`init`] runs.
///
/// # Safety
/// `data` must point at `len` readable bytes (the web shell `_malloc`s a
/// buffer, copies the WebSocket payload in, and frees it after the call).
#[no_mangle]
pub extern "C" fn ob_install_overlay(hash_hex: *const c_char, data: *const u8, len: i32) {
    let Some(server) = ENGINE_SERVER.get() else {
        return;
    };
    let Some(hash) = (unsafe { hash_from_hex(hash_hex) }) else {
        return;
    };
    if data.is_null() || len < 0 {
        return;
    }
    let bytes = unsafe { std::slice::from_raw_parts(data, len as usize) }.to_vec();
    server.install_overlay(RawHandle(hash), bytes);
}

/// Drop a live overlay so the embedded asset returns on the next tick.
/// A no-op before [`init`] runs.
#[no_mangle]
pub extern "C" fn ob_remove_overlay(hash_hex: *const c_char) {
    let Some(server) = ENGINE_SERVER.get() else {
        return;
    };
    let Some(hash) = (unsafe { hash_from_hex(hash_hex) }) else {
        return;
    };
    server.remove_overlay(RawHandle(hash));
}

/// Queue a scene-HMR patch (`ScenePatch` JSON, NUL-terminated) from the
/// broker. The web twin of the native bridge's `POST /scene/patch`: the
/// payload is pushed onto the same queue [`ClientApp::update`] drains and
/// applies to the live `SceneManager` each frame. A no-op before [`init`].
/// Exported via `Module.ccall('ob_scene_patch', null, ['string'], [json])`;
/// see `EXPORTED_FUNCTIONS` in `.cargo/config.toml`.
///
/// # Safety
/// `json` must be a valid NUL-terminated C string (emscripten `'string'`
/// marshalling) or null.
#[no_mangle]
pub extern "C" fn ob_scene_patch(json: *const c_char) {
    let Some(server) = ENGINE_SERVER.get() else {
        return;
    };
    if json.is_null() {
        return;
    }
    let Ok(text) = (unsafe { CStr::from_ptr(json) }).to_str() else {
        return;
    };
    server.push_scene_patch(text.to_string());
}
