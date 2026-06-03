//! Editor → engine live-mod link (native dev build).
//!
//! The native twin of [`super::web_bridge`].  A browser tab can't host a
//! listening socket, so the web shell (`open_bulanci/web/index.html`)
//! connects *out* to the asset studio's Vite dev-server broker over a
//! WebSocket and applies the frames it receives.  This module does the
//! exact same thing from native Rust: instead of hosting a loopback HTTP
//! server and waiting to be pushed to, the dev client **probes for the
//! editor and connects to it**, so the data flow matches the web build.
//!
//! ## Why this closes the HMR gap
//!
//! The broker replays its full current overlay set the instant a client
//! connects, so a native client (re)opened *after* edits catches up to the
//! editor's live state — including unsaved/transient pushes — rather than
//! booting from stale on-disk paks.  After the snapshot it streams a frame
//! for every subsequent change.  Each frame funnels into the same
//! [`AssetServer::install_overlay`] / [`AssetServer::remove_overlay`] /
//! [`AssetServer::push_scene_patch`] sinks the disk watcher uses, so a WS
//! push and a `Ctrl+S` are indistinguishable downstream.
//!
//! ## Security posture
//!
//! Off by default.  It only connects when `BULANCI_DEV_EDITOR` is set
//! (`1` → the default loopback broker URL, or an explicit
//! `ws://host:port/path`).  No asset *names* cross the wire: assets are
//! addressed by the same 64-bit handle hash the codegen baked in, so
//! `.rodata` stays slug-free.
//!
//! ## Wire protocol (matches the broker + the web shell)
//!
//! Binary frames, little-endian lengths, ASCII hash:
//!   * op=1 install: `[1][hash:16 ascii][len:u32le][payload]`
//!   * op=2 clear:   `[2][hash:16 ascii]`
//!   * op=3 scene:   `[3][len:u32le][utf8 ScenePatch json]`

#![cfg(not(target_arch = "wasm32"))]

use std::net::TcpStream;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::thread::JoinHandle;
use std::time::Duration;

use tungstenite::stream::MaybeTlsStream;
use tungstenite::{Message, WebSocket};

use super::handle::RawHandle;
use super::server::AssetServer;

/// Default broker endpoint when `BULANCI_DEV_EDITOR=1`.  The asset studio's
/// Vite dev server defaults to `:5173` on the same host, and the mod-broker
/// plugin claims `/__engine/subscribe` on the upgrade.
///
/// The host is `localhost` (not `127.0.0.1`) on purpose: Vite/Node bind
/// `localhost` to the **IPv6** loopback `[::1]` only on Windows, so an IPv4
/// `127.0.0.1` connect is refused.  `localhost` resolves to both `::1` and
/// `127.0.0.1`; [`tungstenite::connect`] tries each resolved address in turn,
/// so it reaches the broker regardless of which stack the dev server listens on.
const DEFAULT_URL: &str = "ws://localhost:5173/__engine/subscribe";

/// How long to wait between connection attempts / after a drop.  Mirrors
/// the web shell's 2 s reconnect schedule.
const RECONNECT_DELAY: Duration = Duration::from_millis(2000);

/// Read timeout on the underlying socket.  A timed-out read just re-checks
/// the shutdown flag, so `Drop` returns within this window even while the
/// link sits idle waiting for the next frame — the same cooperative-poll
/// trick the old HTTP bridge used with `recv_timeout`.
const READ_TIMEOUT: Duration = Duration::from_millis(250);

/// Owns the dev link's background thread.  Held for the lifetime of
/// `ClientApp`; `Drop` signals the thread to stop and joins it so the
/// socket closes cleanly.
pub struct WsDevClient {
    url: String,
    running: Arc<AtomicBool>,
    thread: Option<JoinHandle<()>>,
}

impl WsDevClient {
    /// Start the link if `BULANCI_DEV_EDITOR` is set.  Returns `None`
    /// (silently) when the variable is unset — the common case for shipped
    /// builds.
    pub fn start(server: Arc<AssetServer>) -> Option<Self> {
        let url = match std::env::var("BULANCI_DEV_EDITOR") {
            Ok(v) if v.trim() == "1" => DEFAULT_URL.to_string(),
            Ok(v) if !v.trim().is_empty() => v.trim().to_string(),
            _ => return None,
        };

        let running = Arc::new(AtomicBool::new(true));
        let thread = {
            let running = running.clone();
            let url = url.clone();
            std::thread::Builder::new()
                .name("dev-editor-link".into())
                .spawn(move || run(url, server, running))
                .ok()?
        };

        eprintln!("[dev-link] probing editor broker at {url} (live HMR — set BULANCI_DEV_EDITOR=ws://host:port/path to override)");
        Some(Self { url, running, thread: Some(thread) })
    }

    /// The broker URL we connect to (for a future status-bar readout).
    #[allow(dead_code)]
    pub fn url(&self) -> &str { &self.url }
}

impl Drop for WsDevClient {
    fn drop(&mut self) {
        self.running.store(false, Ordering::SeqCst);
        if let Some(t) = self.thread.take() {
            // The read loop wakes from its `set_read_timeout` within
            // READ_TIMEOUT, sees the flag, and returns; the backoff loop
            // sleeps in matching slices, so the join is prompt.
            let _ = t.join();
        }
    }
}

/// Outer reconnect loop: keep probing the broker until it answers, pump
/// frames while connected, and reconnect after a short delay on any drop —
/// so the link self-heals when the editor restarts.
fn run(url: String, server: Arc<AssetServer>, running: Arc<AtomicBool>) {
    while running.load(Ordering::SeqCst) {
        match tungstenite::connect(url.as_str()) {
            Ok((mut socket, _resp)) => {
                eprintln!("[dev-link] connected to editor broker {url}");
                apply_read_timeout(&mut socket);
                pump(&mut socket, &server, &running);
                let _ = socket.close(None);
                if running.load(Ordering::SeqCst) {
                    eprintln!("[dev-link] editor broker disconnected — retrying");
                }
            }
            // Broker not up yet (the common case at boot) — quietly retry.
            Err(_e) => {}
        }
        backoff(&running);
    }
    eprintln!("[dev-link] stopped");
}

/// Bound the blocking read so the loop can re-check the shutdown flag.
/// Best-effort: if the stream isn't a plain TCP socket (a TLS broker, which
/// we never use) we fall back to a fully blocking read — `Drop` still
/// unblocks it by closing the socket.
fn apply_read_timeout(socket: &mut WebSocket<MaybeTlsStream<TcpStream>>) {
    if let MaybeTlsStream::Plain(s) = socket.get_mut() {
        let _ = s.set_read_timeout(Some(READ_TIMEOUT));
    }
}

/// Sleep `RECONNECT_DELAY` in `READ_TIMEOUT` slices so a pending shutdown
/// returns promptly instead of blocking the whole backoff.
fn backoff(running: &Arc<AtomicBool>) {
    let mut waited = Duration::ZERO;
    while running.load(Ordering::SeqCst) && waited < RECONNECT_DELAY {
        std::thread::sleep(READ_TIMEOUT);
        waited += READ_TIMEOUT;
    }
}

/// Inner read loop: drain frames until the socket errors/closes or shutdown
/// is requested.  A read timeout (`WouldBlock`/`TimedOut`) just re-checks
/// the running flag.
fn pump(
    socket: &mut WebSocket<MaybeTlsStream<TcpStream>>,
    server: &AssetServer,
    running: &Arc<AtomicBool>,
) {
    while running.load(Ordering::SeqCst) {
        match socket.read() {
            Ok(Message::Binary(buf)) => handle_frame(&buf, server),
            Ok(Message::Close(_)) => break,
            // Text / ping / pong / raw frame — the broker only sends binary;
            // tungstenite auto-answers pings internally.
            Ok(_) => {}
            Err(tungstenite::Error::Io(e))
                if e.kind() == std::io::ErrorKind::WouldBlock
                    || e.kind() == std::io::ErrorKind::TimedOut =>
            {
                // Idle read timeout — loop back and re-check `running`.
                continue;
            }
            // Connection reset / protocol error → drop out and reconnect.
            Err(_) => break,
        }
    }
}

/// Parse one broker frame and funnel it into the asset server.  Mirrors the
/// `handleFrame` parser in `open_bulanci/web/index.html`.
fn handle_frame(buf: &[u8], server: &AssetServer) {
    let Some(&op) = buf.first() else { return };
    match op {
        // op=3 scene: [3][len:u32le][utf8 ScenePatch json]
        3 => {
            if buf.len() < 5 {
                return;
            }
            let len = u32::from_le_bytes([buf[1], buf[2], buf[3], buf[4]]) as usize;
            let Some(payload) = buf.get(5..5 + len) else { return };
            if let Ok(json) = std::str::from_utf8(payload) {
                server.push_scene_patch(json.to_string());
            }
        }
        // op=1 install / op=2 clear: both carry the 16-char ascii hash.
        1 | 2 => {
            if buf.len() < 17 {
                return;
            }
            let Some(hash) = parse_hash(&buf[1..17]) else { return };
            if op == 1 {
                if buf.len() < 21 {
                    return;
                }
                let len = u32::from_le_bytes([buf[17], buf[18], buf[19], buf[20]]) as usize;
                let Some(payload) = buf.get(21..21 + len) else { return };
                server.install_overlay(RawHandle(hash), payload.to_vec());
            } else {
                server.remove_overlay(RawHandle(hash));
            }
        }
        _ => {}
    }
}

/// Decode a 16-hex-char handle hash (ASCII) into a `u64`.
fn parse_hash(bytes: &[u8]) -> Option<u64> {
    let hex = std::str::from_utf8(bytes).ok()?;
    u64::from_str_radix(hex.trim(), 16).ok()
}
