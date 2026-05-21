//! Background-music + SFX playback via `rodio`. Two sinks: one looped for
//! ambient music, one fire-and-forget for menu/gameplay sound effects.

use rodio::{Decoder, OutputStream, OutputStreamHandle, Sink, Source};
use std::sync::atomic::{AtomicBool, Ordering};

pub struct AudioManager {
    _stream: OutputStream,
    _stream_handle: OutputStreamHandle,
    bg_sink: Sink,
    sfx_sink: Sink,
    focused: AtomicBool,
}

impl AudioManager {
    pub fn new() -> Option<Self> {
        let (_stream, _stream_handle) = OutputStream::try_default().ok()?;
        let bg_sink = Sink::try_new(&_stream_handle).ok()?;
        let sfx_sink = Sink::try_new(&_stream_handle).ok()?;
        Some(AudioManager {
            _stream,
            _stream_handle,
            bg_sink,
            sfx_sink,
            focused: AtomicBool::new(true),
        })
    }

    pub fn set_focused(&self, focused: bool) {
        let was_focused = self.focused.swap(focused, Ordering::SeqCst);
        if was_focused != focused {
            if focused {
                self.bg_sink.play();
                self.sfx_sink.play();
            } else {
                self.bg_sink.pause();
                self.sfx_sink.pause();
            }
        }
    }

    pub fn play_bg_music(&self, music_bytes: &[u8]) {
        self.bg_sink.stop();
        let cursor = std::io::Cursor::new(music_bytes.to_vec());
        if let Ok(source) = Decoder::new(cursor) {
            let looped = source.repeat_infinite();
            self.bg_sink.append(looped);
            // CDSAudioPlayer_SetVolumePercent default for menu music = 0x46 = 70/100.
            self.bg_sink.set_volume(0.70);
            if self.focused.load(Ordering::SeqCst) {
                self.bg_sink.play();
            } else {
                self.bg_sink.pause();
            }
        }
    }

    pub fn play_sfx(&self, sfx_bytes: &[u8]) {
        if !self.focused.load(Ordering::SeqCst) {
            return;
        }
        let cursor = std::io::Cursor::new(sfx_bytes.to_vec());
        if let Ok(source) = Decoder::new(cursor) {
            self.sfx_sink.append(source);
        }
    }
}

#[cfg(target_os = "windows")]
#[allow(clippy::upper_case_acronyms)]
pub fn is_window_focused() -> bool {
    type HWND = *mut std::ffi::c_void;
    type DWORD = u32;

    #[link(name = "user32")]
    extern "system" {
        fn GetForegroundWindow() -> HWND;
        fn GetWindowThreadProcessId(hwnd: HWND, lpdwProcessId: *mut DWORD) -> DWORD;
    }

    unsafe {
        let hwnd = GetForegroundWindow();
        if hwnd.is_null() {
            return false;
        }
        let mut process_id: DWORD = 0;
        GetWindowThreadProcessId(hwnd, &mut process_id);
        process_id == std::process::id()
    }
}

#[cfg(target_os = "windows")]
#[allow(clippy::upper_case_acronyms)]
pub fn is_cursor_in_client_area() -> bool {
    type HWND = *mut std::ffi::c_void;
    #[repr(C)]
    pub struct POINT {
        pub x: i32,
        pub y: i32,
    }
    #[repr(C)]
    pub struct RECT {
        pub left: i32,
        pub top: i32,
        pub right: i32,
        pub bottom: i32,
    }

    #[link(name = "user32")]
    extern "system" {
        fn GetActiveWindow() -> HWND;
        fn GetCursorPos(lpPoint: *mut POINT) -> i32;
        fn ScreenToClient(hWnd: HWND, lpPoint: *mut POINT) -> i32;
        fn GetClientRect(hWnd: HWND, lpRect: *mut RECT) -> i32;
    }

    unsafe {
        let hwnd = GetActiveWindow();
        if hwnd.is_null() {
            return false;
        }
        let mut pt = POINT { x: 0, y: 0 };
        if GetCursorPos(&mut pt) == 0 {
            return false;
        }
        if ScreenToClient(hwnd, &mut pt) == 0 {
            return false;
        }
        let mut rect = RECT { left: 0, top: 0, right: 0, bottom: 0 };
        if GetClientRect(hwnd, &mut rect) == 0 {
            return false;
        }
        pt.x >= 0 && pt.x < rect.right && pt.y >= 0 && pt.y < rect.bottom
    }
}

#[cfg(not(target_os = "windows"))]
pub fn is_window_focused() -> bool {
    true
}

#[cfg(not(target_os = "windows"))]
pub fn is_cursor_in_client_area() -> bool {
    true
}

