# Movie & Cinema Views Subsystem (`CMovieView`, `CBlackView`)

This subsystem manages full-screen video and cinematic transitions in *Bulánci*. It consists of two main classes: `CMovieView` (used to play the intro, outro, and other game movies) and `CBlackView` (a solid-color view used primarily for screen-fade transitions).

---

## 1. `CBlackView` — Solid Color / Fade Transition View

Despite its name, `CBlackView` is a highly generic, solid-color fill view. It inherits from `CDSView` (and thus `CDSChained`) and can render a solid block of color over its bounds.

### Object Layout
* **Size:** `0x6c` (108 bytes)
* **Inheritance:** `CDSView` $\rightarrow$ `CBlackView`
* **Fields:**
  * `+0x00`..`+0x2f`: `CDSView` base (including primary vtable override at `+0x00` and secondary vtables at `+0x04`, `+0x10`, `+0x18`).
  * `+0x30`..`+0x3f`: `bounds` (`RECT` defining view area: `left, top, right, bottom`).
  * `+0x40`..`+0x67`: `CDSView` state flags, parents, and children list links.
  * `+0x68`: `uint32 m_Color` (solid color value, stored as ARGB or 24-bit RGB. Initialized to `0xffffff` / White, and updated at runtime to transition colors).

### Virtual Table & Methods
The class registers its primary vftable at `0x0047fb04` with 33 slots. It overrides:
* **Slot 0 (`+0x00`):** `CBlackView::GetClassTable` (`0x00403520`) — Returns class metadata pointer (`0x004b3428`).
* **Slot 14 (`+0x38`):** `CBlackView::OnDraw` (`0x00404590`) — The dirty-rect render callback. It delegates the fill operation to the app's blending engine/backbuffer:
  ```cpp
  void __fastcall CBlackView::OnDraw(CBlackView *this) {
      if (g_pApp != (void *)0xffffff84) {
          CPoemScroller::DrawSolidRect(
              (CPoemScroller *)((int)g_pApp + 0x80), // m_Blender subobject
              &this->bounds,                         // +0x30 rect
              0, 
              this->m_Color                          // +0x68 color
          );
      }
  }
  ```

### Lifecycle & Registration
Registered under **Class ID `2042` (0x7fa)** inside the static initializer block at `0x0047b2f0`:
```assembly
0047b2f0  PUSH 0x00404e00  ; CBlackView::CreateObject (Factory)
0047b2f5  PUSH 0x004b3bac  ; Parent metadata
0047b2fa  PUSH 0x7fa     ; ClassID (2042)
0047b2ff  MOV  ECX, 0x004b3428 ; Class metadata pointer
0047b304  CALL HandleClassRegister
```

---

## 2. `CMovieView` — Cinematic Playback View

`CMovieView` handles cinematic playback (e.g., game intro/outro). It uses a dual-resource approach where the **audio** and **video** tracks are stored and loaded as two entirely separate assets, which are then synchronized and mixed during playback.

### Object Layout
* **Size:** `0x84` (132 bytes)
* **Inheritance:** `CDSView` $\rightarrow$ `CDSAnim` $\rightarrow$ `CMovieView`
* **Vftables (6):**
  * `+0x00`: Primary vftable (`0x0048323c`)
  * `+0x04`: Secondary vftable 1 (`0x00483220`)
  * `+0x10`: Secondary vftable 2 (`0x00483208`)
  * `+0x18`: Secondary vftable 3 (`0x004831f4`)
  * `+0x68`: Subobject vftable 1 (`0x004831e0`) — Tracks sequence interface / track manager adapter
  * `+0x6c`: Subobject vftable 2 (`0x004831c8`)
* **Fields:**
  * `+0x78`: `CDSResource* m_pVideoResource` — Loaded video stream asset.
  * `+0x7c`: `CDSAnimSequence* m_pAudioSequence` — Decoded audio sequence interface (derived via `CheckedVirtualBaseCast` from the loaded `.mpx` audio resource).
  * `+0x80`: `CDSAudioVideoPlayer* m_pPlayer` — Active audio-video synchronizer/playback engine.

### Playback Logic & Synchronization
1. **Construction (`0x004236a0`):**
   * Takes the view coordinates and two resource IDs: `audioResId` and `videoResId`.
   * Invokes `ResourceManager::Load` on the app's global resource pool (`g_pApp + 0x70`) to fetch both streams.
   * Resolves the audio stream's interface to its sequence interface via RTTI cast.
   * Initializes the track manager subobject (`+0x68`) with the audio sequence via `InitTrackSequence`.
2. **Playback Start (`0x00422d50`):**
   * Stops any currently playing cinematic.
   * Notifies the parent view (usually `CStartGame2`) with code `0xf0`.
   * Allocates a `CDSAudioVideoPlayer` instance (`80 bytes` in size).
   * Extracts track structures from the video resource:
     * `video_track_data` at `video_resource + 0x1c`
     * `video_track_info` at `video_resource + 0x04`
   * Binds the video tracks to the player via `CDSAudioVideoPlayer::SetupTrack`.
   * Connects the player's underlying sequence to the movie view's track manager (`+0x68`).
   * Starts synchronous playback via `CDSAudioVideoPlayer::Play(loop_flag)`.
3. **Event Notification & Termination (`0x00422860`):**
   * Overrides `CDSView::vftable` slot 27 (the standard engine event receiver).
   * When playback naturally completes (notified with code `1`):
     * Stops the player via `CMovieView::StopPlayback`
     * Explicitly destroys the player instance.
     * Posts notification `0xf1` to the parent container (`CStartGame2 + 0x10`) to transition the game state out of the cinematic view.
   * When playback is skipped by the user (notified with code `0xf0`):
     * Aborts playback immediately and cleans up resources.

### Lifecycle & Registration
Registered under **Class ID `2062` (0x80e)** in the static initializer block at `0x0047c050`:
```assembly
0047c050  PUSH 0x00422e30  ; CMovieView::CreateObject (Factory)
0047c055  PUSH 0x004b830c  ; Parent metadata
0047c05a  PUSH 0x80e     ; ClassID (2062)
0047c05f  MOV  ECX, 0x004b3914 ; Class metadata pointer
0047c064  CALL HandleClassRegister
```

---

## 3. `CDSAudioVideoPlayer` — AV Playback Engine

This helper class is allocated dynamically by `CMovieView` during active playback. It coordinates audio decoding (through `CDSMpx`) and video frame sequencing.

* **Size:** `0x50` (80 bytes)
* **Fields:**
  * `+0x04`: Audio player stream handle / pointer
  * `+0x08`: Embedded `CDSVideoPlayer` (track manager) that handles video sequencing.
* **Core Functions:**
  * `0x0043bca0` — Constructor.
  * `0x0043bba0` (`CDSAudioVideoPlayer::SetupTrack`) — Sets up track sources and buffers.
  * `0x0043bbf0` (`CDSAudioVideoPlayer::Play`) — Initiates audio and video playback loops.

---

## 4. Renamed Subsystem Functions (Ghidra & Source Match)

The following functions have been fully cross-referenced and renamed within the `bulanci.exe` Ghidra workspace and mapped to their corresponding C++ implementations:

| Address | New Function Name | Description |
| :--- | :--- | :--- |
| `0x00403520` | `CBlackView::GetClassTable` | Returns class metadata table for `CBlackView`. |
| `0x00404590` | `CBlackView::OnDraw` | Renders a solid color fill to screen bounds (Slot 14 override). |
| `0x00404e00` | `CBlackView::CreateObject` | Factory function/allocator for `CBlackView`. |
| `0x00422630` | `CMovieView::StopPlayback` | Stops player and pumps ticks until player instance is fully deleted. |
| `0x00422840` | `CMovieView::OnMovieStop` | Callback invoked on movie stop/abort (Slot 15 override). |
| `0x00422860` | `CMovieView::OnEvent` | Standard event receiver. Handles finish (code `1`) and skip (code `0xf0`) (Slot 27 override). |
| `0x00422a20` | `CMovieView::GetClassTable` | Returns class metadata table for `CMovieView`. |
| `0x00422cb0` | `CMovieView::Cleanup` | Central destructor body; releases video/audio resource handles. |
| `0x00422d50` | `CMovieView::StartPlayback` | Performs setup, allocates `CDSAudioVideoPlayer`, binds tracks, and starts play loop. |
| `0x00422e30` | `CMovieView::CreateObject` | Factory function/allocator for `CMovieView`. |
| `0x00422ee0` | `CMovieView::ScalarDeletingDtor` | Deletes object scalar allocation. |
| `0x004231a0` | `CMovieView::TogglePlayback` | Playback toggle (starts if idle, stops if active). |
| `0x004236a0` | `CMovieView::Constructor` | Constructor; loads and initializes resources from global pool. |
| `0x004237b0` | `CMovieView::InitTrackSequence` | Binds decoded audio sequence to internal track manager. |
| `0x0043bca0` | `CDSAudioVideoPlayer::Constructor` | Dynamic constructor for the playback coordinator. |
| `0x0043bba0` | `CDSAudioVideoPlayer::SetupTrack` | Prepares and initializes audio/video tracks. |
| `0x0043bbf0` | `CDSAudioVideoPlayer::Play` | Starts playback stream loops. |

The adjustments thunks (`0x00422a30`, `0x00422a40`, `0x00422a50`, `0x00422a60`) have also been mapped to `CMovieView::ScalarDeletingDtor_Thunk_[offset]` corresponding to their multiple-inheritance offsets (`-4`, `-104`, `-16`, `-24`).

---

## 5. Verification Status

* **Status:** **Verified**
* **Confidence:** High
* **Open questions:** None. All functions, RTTI/vftables, resource-binding procedures, and event codes have been fully mapped and verified.
