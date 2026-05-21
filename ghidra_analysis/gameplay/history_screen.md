# History Screen & Cinematic Playback Subsystem

The **History Screen** (accessed from the Main Menu "Historie" button) displays the rich narrative lore and poetic intros of *Bulánci*. Behind this interface lies a specialized scripting, rendering, and audio orchestration engine. 

This document details the architecture of the History Screen (`CHistoryDlg`, `CHistoryView`, `CHistoryScript`), cinematic movie rendering (`CMovieView`), and the engine-level audio mixing/fading logic.

---

## 1. Subsystem Architecture & Core Classes

The History screen is built on a tripartite model of dialog controller, render view, and execution script:

```
[ CMenu / Main Menu ]
        │ (Command 0xCA)
        ▼
 [ CHistoryDlg ] ──(Manages Page Turning)──► [ Navigation Icons (CIcon) ]
        │
        ├──► [ CHistoryView ] (Active render bounds)
        │
        └──► [ CHistoryScript ] (Executes page LUA/bytecode)
                    │
                    ├──► Adds Static Texts (CStaticText)
                    ├──► Loads JPEGs (CDSBitmap)
                    └──► Plays Movies (CMovieView) ──► CDSAudioVideoPlayer
```

### 1.1 `CHistoryDlg` — Dialog Controller
* **Class ID:** `2050` (`0x802`)
* **Size:** `0x9c` (156 bytes)
* **Vftable:** `0x00483488`
* **Purpose:** Acts as the modal container. Upon construction, it scans the global resource pool (`g_pApp + 0x70`) for all resources of class `0x802` (`CHistoryScript`) and compiles an index of available history pages. It also instantiates and positions four navigation `CIcon` elements representing:
  - First Page (`0xe5`)
  - Previous Page (`0xe6`)
  - Next Page (`0xe7`)
  - Last Page (`0xe8`)

### 1.2 `CHistoryView` — Page Render View
* **Class ID:** `2049` (`0x801`)
* **Size:** `0x6c` (108 bytes)
* **Vftable:** `0x00483180`
* **Purpose:** A rectangular view (`0, 0, 530, 510`) registered under `CHistoryDlg` that acts as the visual canvas for the currently selected history page. All visual objects spawned by the page script are added as child nodes of this view.

### 1.3 `CHistoryScript` — Page Script Executor
* **Class ID:** `2048` (`0x800`)
* **Size:** `0x110` (272 bytes)
* **Purpose:** An extension of the standard `CDSScript` virtual machine. It loads level-style scripts (`res_0000065843_2050_HistoryScript.lua` to `res_0000065845_2050_HistoryScript.lua`) representing pages. It registers custom opcodes specifically for constructing history pages.

---

## 2. Page Scripting & VM Extensions

When a page is loaded via `CHistoryDlg_LoadHistoryPage(pageIdx)`, the engine invokes `CDSScript::CallExport` to execute the script's initialization code. To support page layout, the `CHistoryScript` VM defines four specialized opcodes:

| Opcode | Handler Address | Name (Reversed) | Arguments | Function & Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **`45`** | `0x00422b00` | `CreateStaticTextAuto` | `x, y, textId, alignment, width` | Spawns a `CStaticText` widget at the specified location, automatically formatting the text flow. |
| **`46`** | `0x00422bc0` | `CreateStaticText` | `x, y, textId, fontFlags, ...` | Spawns a custom-positioned static text block. |
| **`47`** | `0x00423530` | `CreateImage` | `x, y, jpegResourceId` | Loads a JPEG resource from the EAP bundle, constructs a `CDSBitmap` wrapper, and renders it at `(x, y)`. |
| **`52`** | `0x00423820` | `CreateMovieView` | `x, y, audioResId, videoResId` | Spawns a `CMovieView` to play high-quality audio-synchronized cinematics. |

---

## 3. Cinematic Movie Playback (`CMovieView`)

When the history page script invokes `CreateMovieView` (Opcode 52), the engine instantiates a `CMovieView`. 

Unlike standard modern formats where audio and video are interleaved in a single container, *Bulánci* decouples them:
1. **Audio Track:** Stored as an MPX resource (decoded via `CDSMpx`).
2. **Video Track:** Stored as a sequence of JPEG frames.

These tracks are fed into `CDSAudioVideoPlayer` (allocated at `0x00422d50`), which handles frame timing, audio streaming to DirectSound, and vertical-sync aligned redraws.

### Playback Event Triggers:
* **Start (`0x00422d50`):** `CMovieView::StartPlayback` is invoked. It immediately broadcasts an event code of `0xf0` (Movie Starting) to its parent container (`CHistoryView`).
* **Finish/Stop (`0x00422860`):** Once playback ends or is skipped by the user, `CMovieView::OnEvent` receives code `1` (Finished) or `0xf0` (Skipped). It stops playback, destroys the active `CDSAudioVideoPlayer`, and posts event `0xf1` (Movie Stopped) to the parent container.

---

## 4. Engine-Level Audio Fading & Mixing

To prevent the game's ambient main menu music from clashing with a movie's custom soundtrack, the engine implements a highly sophisticated volume cross-fader. This is coordinated between the active render view (`CHistoryView`), the menu state machine (`CMenu`), and the scheduler.

### 4.1 Transition Hook (`CHistoryView_OnEvent`)
The event loop in `CHistoryView` acts as the bridge. When a movie starts or stops, it catches the `0xf0` / `0xf1` events and requests `CMenu` to adjust background audio states:

```cpp
void __thiscall CHistoryView::CHistoryView_OnEvent(CHistoryView *this, short param_1, uint param_2, uint param_3) {
    CBulanci::OnEvent_Default((CBulanci *)this, param_1, param_2, param_3);
    
    // Get the CMenu instance from parent fields
    void* pMenu = *(void **)(*(int *)(this + 0x4c) + 0x4c);
    
    if (param_1 == 0xf0) { // Movie Started!
        _Globals::CMenu_EnableBackgroundState(pMenu, 0); // Fade out ambient music
        return;
    }
    if (param_1 == 0xf1) { // Movie Stopped!
        _Globals::CMenu_EnableBackgroundState(pMenu, 1); // Fade in ambient music
    }
    return;
}
```

### 4.2 State Machine Controller (`CMenu_EnableBackgroundState`)
This function changes the music active state and controls the fade timer tick.

* **Address:** `0x00424010`
* **VTable Slot:** Member of `_Globals` / `CMenu`
* **Variables:**
  - `CMenu + 0xe2`: `m_bBackgroundMusicActiveState` (0 = Fade Out / Paused, 1 = Fade In / Playing)
  - `CMenu + 0xc0`: `m_pBackgroundMusicPlayer` (Pointer to ambient music `CDSAudioPlayer`)
  - `CMenu + 0x68`: `SchedulerSubobject` (Coordinates ticking update slots)

```cpp
void __thiscall _Globals::CMenu_EnableBackgroundState(void *this, char param_1) {
    void *pEventSlot;
    
    if (*(char *)((int)this + 0xe2) != param_1) {
        *(char *)((int)this + 0xe2) = param_1; // Update target state
        
        // If enabling and background music isn't already playing: play it loop-enabled (1)
        if ((param_1 != '\0') && (*(char *)(*(int *)((int)this + 0xc0) + 0x22) == '\0')) {
            CDSAudioPlayer_Play(*(void **)((int)this + 0xc0), 1);
        }
        
        // Retrieve scheduler event slot 0 (Music Fade tick timer)
        pEventSlot = Scheduler_GetEventSlot((void *)((int)this + 0x68), 0);
        
        if ((*(byte *)((int)pEventSlot + 8) & 1) != 0) {
            // If the fade slot is currently paused, unpause/resume it to begin ticking
            _Globals::Scheduler_UnpauseEventSlot((void *)((int)this + 0x68), 0, -1);
        }
        
        // If we are fading out/disabling background music:
        if (*(char *)((int)this + 0xe2) == '\0') {
            // Disable CMenu "Ruch" (insect/ambient crawling) animations so movie has full CPU focus
            CBulanci::CMenu_EnableAllRuch(this, '\0');
        }
    }
    return;
}
```

### 4.3 Incremental Volume Tick Fader (`CMenu_OnMusicFadeTick`)
A scheduled task bound to the `CMenu + 0x68` scheduler subobject. It triggers every **120 milliseconds** (registered with period `0x78` during menu setup) and modifies the volume linearly.

* **Address:** `0x00424080`
* **Tick Period:** 120 ms
* **Parameters:**
  - `param_1`: Pointer to the `CDSUpdatedItem` subobject (`CMenu + 0x68`)
  - `CMenu` instance is resolved via relative offset: `param_1 - 0x68`
  - Target State `m_bBackgroundMusicActiveState` is resolved at: `param_1 + 0x7a`

```cpp
void __fastcall CMenu::CMenu_OnMusicFadeTick(void *param_1) {
    void *pPlayer;
    int curVolumePercent;
    
    // Resolve audio player from the CMenu structure (Menu starts at param_1 - 0x68, player is at +0xC0)
    pPlayer = *(void **)((int)param_1 + 0x58); // equivalent to: (param_1 - 0x68) + 0xC0
    
    if (*(char *)((int)param_1 + 0x7a) == '\0') { // Fade Out Target (Movie is Playing)
        // Decrement volume percent linearly by 1% per 120ms
        curVolumePercent = *(int *)((int)pPlayer + 0x54) + -1;
        if (curVolumePercent < 70) {
            curVolumePercent = 70; // Clamp fading to a minimum of 70%
        }
        
        _Globals::CDSAudioPlayer_SetVolumePercent(pPlayer, curVolumePercent);
        
        // Once background music volume drops to 70%:
        if (curVolumePercent == 70) {
            _Globals::Scheduler_PauseEventSlot(param_1, 0); // Halt fading tick timer
            _Globals::CDSAudioPlayer_Stop(pPlayer, 1);      // Pause the background stream entirely
        }
    }
    else { // Fade In Target (Movie Stopped, returning to Menu)
        // Increment volume percent linearly by 1% per 120ms
        curVolumePercent = *(int *)((int)pPlayer + 0x54) + 1;
        if (100 < curVolumePercent) {
            curVolumePercent = 100; // Clamp fading to a maximum of 100%
        }
        
        _Globals::CDSAudioPlayer_SetVolumePercent(pPlayer, curVolumePercent);
        
        // Once background music returns to full volume (100%):
        if (curVolumePercent == 100) {
            _Globals::Scheduler_PauseEventSlot(param_1, 0); // Halt fading tick timer
            // Re-enable ambient main menu Ruch animations
            CBulanci::CMenu_EnableAllRuch((CBulanci *)((int)param_1 + -0x68), '\x01');
        }
    }
    return;
}
```

---

## 5. DirectSound Volume Translation Mechanics

DirectSound processes volume on a logarithmic decibel scale ranging from `0` (full volume) to `-10000` (silence, in hundredths of a decibel).

When `CDSAudioPlayer_SetVolumePercent` is called with a linear percentage (e.g., `70` to `100`), the engine maps this value to decibels inside `_Globals::FUN_0043a060`:

$$\text{DB Volume} = \frac{(\text{DB}_{\text{modifier}} + 10000) \times \text{VolumePercent}}{100} - 10000$$

Where:
* $\text{DB}_{\text{modifier}}$ is fetched from a scale table at `&DAT_004b8370`.
* The final volume is passed to DirectSound COM interface Slot 15 (`IDirectSoundBuffer::SetVolume`):
  ```cpp
  // COM Call offset 0x3c corresponds to SetVolume
  iVar3 = (**(code **)(*piSoundBuffer + 0x3c))(piSoundBuffer, calculatedDbVolume);
  ```

This translation ensures smooth logarithmic fade-out curves that map perfectly to human audio perception.

---

## 6. Verification Status

* **Status:** **Verified**
* **Confidence:** 100%
* **Summary of Changes:** 
  - Renamed `FUN_00422f70` to `CHistoryDlg::CHistoryDlg_LoadHistoryPage`.
  - Renamed `FUN_00423130` to `CHistoryDlg::CHistoryDlg_OnEvent`.
  - Renamed `FUN_00422670` to `CHistoryView::CHistoryView_OnEvent`.
  - Renamed `FUN_0043a0d0` to `_Globals::CDSAudioPlayer_SetVolumePercent`.
  - Renamed `FUN_0043a4a0` to `_Globals::CDSAudioPlayer_Stop`.
  - Verified full structure-offset alignment of `CMenu` scheduler tick, active background states (`+0xe2`), and fading thresholds.
