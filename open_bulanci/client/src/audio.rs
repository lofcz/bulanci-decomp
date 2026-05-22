//! Background-music + SFX playback via `rodio`. Two sinks: one looped for
//! ambient music, one fire-and-forget for menu/gameplay sound effects.

use rodio::cpal::traits::{DeviceTrait, HostTrait};
use rodio::{Decoder, OutputStream, OutputStreamHandle, Sink, Source};
use std::cell::Cell;
use std::sync::atomic::{AtomicBool, Ordering};

const MIN_VOLUME_PERCENT: i32 = 0;
const MAX_VOLUME_PERCENT: i32 = 100;

// SFX playback used to apply a `Source::fade_in` here to mask a perceived
// "click" at the start of every hover. That was a misdiagnosis: the real
// cause was rodio's unfiltered linear-interpolation `SampleRateConverter`
// aliasing our 22050 Hz source up to the 48 kHz device rate (FFT shows
// 1000-12000x more energy than retail in the 17-20 kHz aliasing band).
// The fix is upstream of the runtime: SFX are pre-resampled to 48 kHz
// with a Kaiser-windowed polyphase filter inside
// `scripts/build_menu_assets.py`, which makes rodio's `from == to`
// short-circuit kick in (see `rodio/src/conversions/sample_rate.rs:70-72`
// and `:129-132`) and pass the samples through unmodified to cpal/WASAPI.
// See `ghidra_analysis/gameplay/main_menu_hover_audio.md` §8 for the
// full investigation, FFT comparisons and offline pipeline tests.

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct AudioFade {
    target_percent: i32,
    step_percent: i32,
}

impl AudioFade {
    pub fn linear_percent(target_percent: i32, step_percent: i32) -> Self {
        Self {
            target_percent: clamp_volume_percent(target_percent),
            step_percent: step_percent.abs().max(1),
        }
    }
}

pub struct AudioManager {
    _stream: OutputStream,
    _stream_handle: OutputStreamHandle,
    bg_sink: Sink,
    sfx_sink: Sink,
    bg_volume_percent: Cell<i32>,
    bg_fade: Cell<Option<AudioFade>>,
    focused: AtomicBool,
}

impl AudioManager {
    pub fn new() -> Option<Self> {
        // Diagnostic: print the actual cpal output device config so we know
        // what sample rate rodio's `SampleRateConverter` is being asked to
        // resample _to_. If `from == to` (i.e. our asset sample rate matches
        // the device's default rate), the converter short-circuits and the
        // raw PCM is passed straight to WASAPI. Otherwise rodio interpolates
        // linearly, which adds audible aliasing for percussive sources like
        // `sfx_hover.wav` (22050 Hz → 48000 Hz creates ~5 kHz noise during
        // the first 100 ms; see ghidra_analysis/gameplay/main_menu_hover_audio.md
        // §8 / "high-pitched intro" investigation).
        if let Some(dev) = rodio::cpal::default_host().default_output_device() {
            let name = dev.name().unwrap_or_else(|_| "<unnamed>".into());
            if let Ok(cfg) = dev.default_output_config() {
                eprintln!(
                    "[audio] cpal default output: name={:?} sample_rate={} channels={} format={:?}",
                    name,
                    cfg.sample_rate().0,
                    cfg.channels(),
                    cfg.sample_format(),
                );
            } else {
                eprintln!("[audio] cpal default output: name={:?} (no default config)", name);
            }
        }

        let (_stream, _stream_handle) = OutputStream::try_default().ok()?;
        let bg_sink = Sink::try_new(&_stream_handle).ok()?;
        let sfx_sink = Sink::try_new(&_stream_handle).ok()?;
        Some(AudioManager {
            _stream,
            _stream_handle,
            bg_sink,
            sfx_sink,
            bg_volume_percent: Cell::new(MAX_VOLUME_PERCENT),
            bg_fade: Cell::new(None),
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

    pub fn play_bg_music_at_percent(&self, music_bytes: &[u8], volume_percent: i32) {
        self.bg_sink.stop();
        self.bg_fade.set(None);
        let cursor = std::io::Cursor::new(music_bytes.to_vec());
        if let Ok(source) = Decoder::new(cursor) {
            let looped = source.repeat_infinite();
            self.bg_sink.append(looped);
            self.set_bg_volume_percent(volume_percent);
            if self.focused.load(Ordering::SeqCst) {
                self.bg_sink.play();
            } else {
                self.bg_sink.pause();
            }
        }
    }

    pub fn set_bg_volume_percent(&self, volume_percent: i32) {
        let volume_percent = clamp_volume_percent(volume_percent);
        self.bg_volume_percent.set(volume_percent);
        self.bg_sink.set_volume(volume_percent_to_linear_gain(volume_percent));
    }

    pub fn start_bg_fade(&self, fade: AudioFade) {
        self.bg_fade.set(Some(fade));
    }

    pub fn step_bg_fade(&self) -> bool {
        let Some(fade) = self.bg_fade.get() else {
            return false;
        };
        let current = self.bg_volume_percent.get();
        if current == fade.target_percent {
            self.bg_fade.set(None);
            return false;
        }

        let next = step_toward(current, fade.target_percent, fade.step_percent);
        self.set_bg_volume_percent(next);
        if next == fade.target_percent {
            self.bg_fade.set(None);
            false
        } else {
            true
        }
    }

    pub fn play_sfx(&self, sfx_bytes: &[u8]) {
        let _ = self.play_sfx_with_duration(sfx_bytes);
    }

    pub fn play_sfx_with_duration(&self, sfx_bytes: &[u8]) -> Option<u64> {
        if !self.focused.load(Ordering::SeqCst) {
            return None;
        }
        let cursor = std::io::Cursor::new(sfx_bytes.to_vec());
        let source = Decoder::new(cursor).ok()?;
        let duration_ms = source
            .total_duration()
            .map(|duration| duration.as_millis().min(u128::from(u64::MAX)) as u64);
        // No fade-in / convert — SFX are pre-rendered to 48 kHz by
        // `scripts/build_menu_assets.py` (see the module-level comment
        // above SFX_FADE_IN section) so the decoded i16 samples are
        // already at the device rate and rodio's `SampleRateConverter`
        // short-circuits.
        if let Ok(sink) = Sink::try_new(&self._stream_handle) {
            sink.append(source);
            sink.detach();
        }
        duration_ms
    }

    pub fn play_hover_sfx(&self, sfx_bytes: &[u8]) {
        // Retail CSwitch_PlayHoverTrack creates a fresh one-shot
        // CDSAudioPlayer for AudioBank slot 0 on every eligible mouse-enter.
        // Eligibility is handled in menu.rs (idle switch only), not by
        // suppressing overlap while a previous hover sample is still playing.
        self.play_sfx(sfx_bytes);
    }
}

fn clamp_volume_percent(volume_percent: i32) -> i32 {
    volume_percent.clamp(MIN_VOLUME_PERCENT, MAX_VOLUME_PERCENT)
}

fn step_toward(current: i32, target: i32, step: i32) -> i32 {
    let step = step.abs().max(1);
    if current < target {
        (current + step).min(target)
    } else {
        (current - step).max(target)
    }
}

fn volume_percent_to_linear_gain(volume_percent: i32) -> f32 {
    let volume_percent = clamp_volume_percent(volume_percent);
    if volume_percent == 0 {
        return 0.0;
    }
    // Retail passes percent through DirectSound's logarithmic SetVolume:
    // ((0 dB + 10000) * percent) / 100 - 10000, in hundredths of a dB.
    let attenuation_db100 = volume_percent * 100 - 10_000;
    10.0_f32.powf(attenuation_db100 as f32 / 2000.0)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn directsound_percent_mapping_matches_trace() {
        assert_eq!(volume_percent_to_linear_gain(100), 1.0);
        assert!((volume_percent_to_linear_gain(70) - 0.031_622_78).abs() < 0.000_001);
        assert!((volume_percent_to_linear_gain(90) - 0.316_227_76).abs() < 0.000_001);
    }

    #[test]
    fn step_toward_clamps_at_target() {
        assert_eq!(step_toward(70, 100, 1), 71);
        assert_eq!(step_toward(99, 100, 5), 100);
        assert_eq!(step_toward(100, 70, 5), 95);
        assert_eq!(step_toward(72, 70, 5), 70);
    }

    #[test]
    fn shipped_sfx_assets_are_pre_resampled_to_48k() {
        // Regression guard for the polyphase-resampling fix.
        //
        // `scripts/build_menu_assets.py` must pre-resample every menu SFX
        // from the retail bank's native 22050 Hz to the runtime's target
        // rate of 48 kHz before it ships to `open_bulanci/assets/audio/`.
        // If this regresses, rodio's `SampleRateConverter` will alias the
        // 22050 Hz signal up to 48 kHz with a triangular kernel and we get
        // the audible high-pitched "intro" back on every hover/click.
        for (name, bytes) in [
            ("sfx_hover.wav",       include_bytes!("../../assets/audio/sfx_hover.wav").as_ref()),
            ("sfx_radio_click.wav", include_bytes!("../../assets/audio/sfx_radio_click.wav").as_ref()),
            ("sfx_history.wav",     include_bytes!("../../assets/audio/sfx_history.wav").as_ref()),
            ("sfx_quit.wav",        include_bytes!("../../assets/audio/sfx_quit.wav").as_ref()),
            ("sfx_force_exit.wav",  include_bytes!("../../assets/audio/sfx_force_exit.wav").as_ref()),
            ("sfx_alt_exit.wav",    include_bytes!("../../assets/audio/sfx_alt_exit.wav").as_ref()),
            ("sfx_start.wav",       include_bytes!("../../assets/audio/sfx_start.wav").as_ref()),
        ] {
            let cursor = std::io::Cursor::new(bytes);
            let dec = Decoder::new(cursor).expect("decoder");
            assert_eq!(
                dec.sample_rate(), 48000,
                "{} must be pre-resampled to 48 kHz (run scripts/build_menu_assets.py)", name,
            );
            assert_eq!(dec.channels(), 1, "{} must be mono", name);
        }
    }

    #[test]
    fn hover_pcm_dominant_frequency_matches_retail() {
        // Regression guard for the WASAPI A/B comparison: the loudest
        // 256 ms slice of retail's hover capture has an FFT centroid of
        // ~2050 Hz (see scripts/fft_hover_compare.py).  Because the 48 kHz
        // hover asset is now pre-rendered, rodio's path is a pass-through
        // (sample-rate converter short-circuits when `from == to`) plus a
        // mono → stereo channel-doubling — so the dominant per-channel
        // frequency of the early samples must still cluster near 2 kHz.
        // A spike to ~4 kHz would mean an asset accidentally shipped at
        // 22050 Hz and rodio resampled it on the fly.
        use rodio::source::UniformSourceIterator;
        let bytes = include_bytes!("../../assets/audio/sfx_hover.wav");
        let cursor = std::io::Cursor::new(bytes.as_ref());
        let decoder = Decoder::new(cursor).expect("decoder");
        let iter: UniformSourceIterator<_, f32> =
            UniformSourceIterator::new(decoder, 2, 48000);
        let stereo: Vec<f32> = iter.take(48000 * 2 / 100).collect(); // 10 ms
        let left: Vec<f32> = stereo.iter().step_by(2).copied().collect();
        let crossings = left.windows(2)
            .filter(|w| (w[0] < 0.0) != (w[1] < 0.0))
            .count();
        let dominant_hz = crossings as f32 / 2.0 / (left.len() as f32 / 48000.0);
        assert!(
            dominant_hz < 3500.0,
            "early dominant freq {:.0} Hz is too high; retail centroid is ~2 kHz",
            dominant_hz,
        );
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

