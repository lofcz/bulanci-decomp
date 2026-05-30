# Audio Banks

This document describes the structure, loading, and comprehensive sample mapping of the audio banks used in `bulanci.exe`.

## 1. CDSAudioBankIndex and CDSAudioBank Resources

The application uses two distinct resource types to manage its short-lived sound effects:

1. **`CDSAudioBankIndex` (Class ID 67 / `res_0000065540_67_AudioBankIndex.bank.json`)**:
   - Represents the metadata, index, and size configuration for a sound bank.
   - On-wire format inside the resource stream:
     ```
     u32 bankResourceID + u32 reserved + u32 sampleCount + u32[] byteLengths
     ```
   - In the default config, this points to bank `0x10152` (`65874`) containing 41 samples.

2. **`CDSAudioBank` (Class ID 43 / `res_0000065874_43_AudioBank.wav`)**:
   - Represents the raw PCM data blob (mono 16-bit PCM @ 22050 Hz).
   - The unpacker tool (`bulanci_unpack.py`) wraps this raw PCM in a standard RIFF/WAVE container (44-byte header) for file-system storage.
   - The byte offset inside the `.wav` file for any sample index `N` is calculated as:
     ```
     [44 + samples[N].offsetInBank, 44 + samples[N].offsetInBank + samples[N].byteLen)
     ```

## 2. Triggering Audio Playback (`TriggerBankSample`)

The core engine helper to play an audio slot is `_Globals::TriggerBankSample` (located at `0x00422430` in `bulanci.exe` and mapped in `_Globals.cpp` / `_Globals.h`).

### Function Prototype
```cpp
void __cdecl _Globals::TriggerBankSample(
    undefined* flags, 
    int bankResourcePtr, 
    int slot, 
    int preDelay, 
    uint eventTarget, 
    char looping
);
```

### Parameter Breakdown:
- **`flags`**: Player parameters (usually passed as `1` / `0x1` to indicate a normal short-lived SFX).
- **`bankResourcePtr`**: Pointer to the loaded `CDSAudioBankIndex` object. If `0`, it defaults to the main app audio bank index stored at `g_pApp + 0x4c0` (Resource `0x10004`).
- **`slot`**: The index mapping to the sample within the sound bank.
- **`preDelay`**: Delay before playing (always `0` at call sites).
- **`eventTarget`**: `IDSEventHandler*` stored on `CDSAudioPlayer` at `+0x18`. On playback end, `CDSAudioPlayer_OnPlaybackTick` posts WM `0x200`/1 to `CDSDirectSound`’s view facet; `CDSDirectSound_OnPlaybackCompleteMessage@0x0043cdc0` reads `+0x18` and enqueues to the handler (e.g. `CMenu_OnEvent(1)` after `DispatchHotkey` X/F12). If `0`, no completion callback.
- **`looping`**: A boolean flag determining whether the sample loops or is standard (usually passed as `1` / `'\x01'` to enable proper buffer arming).

---

## 3. `CDSAudioPlayer` Volume Percent Mapping

Menu instrumentation (`scripts/frida/menu_audio_mixer_trace.js`) verified
the original player volume model:

* `CDSAudioPlayer_SetVolumePercent @ 0x0043a0d0` stores an integer
  percent on the player (`player+0x54`).
* `CDSAudioPlayer_ApplyEffectiveVolume @ 0x0043a060` converts that
  percent into DirectSound attenuation in hundredths of a decibel:

```c
attenuationDb100 = ((busDb100 + 10000) * percent) / 100 - 10000;
```

For the main-menu ambient music bus, `busDb100 == 0`, so:

| Percent | DirectSound attenuation | Linear gain |
|---------|-------------------------|-------------|
| `100` | `0` | `1.0` |
| `90` | `-1000` | `~0.3162` |
| `70` | `-3000` | `~0.03162` |

The main menu's fade is **linear in percent**, not linear in amplitude:
`CMenu_OnMusicFadeTick @ 0x00424080` changes the percent by exactly `1`
every `120 ms`. The non-linear audible ramp comes from the DirectSound
dB conversion above.

---

## 4. Comprehensive Sample Mapping (bank_65874 / Resource 0x10152)

Through a thorough analysis of `TriggerBankSample` call sites, player quip tables, and event dispatchers, the exact purpose and triggering context of every single sample in the main audio bank have been identified:

| Slot (Hex) | Slot (Dec) | Unpacked File | Duration (s) | Playing/Triggering Function | Role / In-Game Event Description |
|:---|:---|:---|:---|:---|:---|
| `0x00` | 0 | `sample_00.wav` | 0.361s | `_Globals::CSwitch_PlayHoverTrack` | Short button hover tick / whoosh sound (CSwitch widgets). |
| `0x01` | 1 | `sample_01.wav` | 0.304s | `_Globals::CGaming_RespawnPlayer` via `CBulanek_GetSpawnQuipSlot` | **Red Player** Spawn/re-entry quip (plays when player spawns on map). |
| `0x02` | 2 | `sample_02.wav` | 0.666s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetHitQuipSlot` | **Red Player** Delayed event `0xdb` quip (plays when player gets hit/scared). |
| `0x03` | 3 | `sample_03.wav` | 1.054s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetDelayedQuipSlot` | **Red Player** Delayed event `0x01` quip (general player quip). |
| `0x04` | 4 | `sample_04.wav` | 0.394s | `_Globals::CGaming_RespawnPlayer` via `CBulanek_GetSpawnQuipSlot` | **Green Player** Spawn/re-entry quip. |
| `0x05` | 5 | `sample_05.wav` | 1.010s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetHitQuipSlot` | **Green Player** Delayed event `0xdb` quip. |
| `0x06` | 6 | `sample_06.wav` | 0.482s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetDelayedQuipSlot` | **Green Player** Delayed event `0x01` quip. |
| `0x07` | 7 | `sample_07.wav` | 0.482s | `_Globals::CGaming_RespawnPlayer` via `CBulanek_GetSpawnQuipSlot` | **Blue Player** Spawn/re-entry quip. |
| `0x08` | 8 | `sample_08.wav` | 0.505s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetHitQuipSlot` | **Blue Player** Delayed event `0xdb` quip. |
| `0x09` | 9 | `sample_09.wav` | 0.447s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetDelayedQuipSlot` | **Blue Player** Delayed event `0x01` quip. |
| `0x0a` | 10 | `sample_10.wav` | 0.807s | `_Globals::CGaming_RespawnPlayer` via `CBulanek_GetSpawnQuipSlot` | **Brown Player** Spawn/re-entry quip. |
| `0x0b` | 11 | `sample_11.wav` | 0.308s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetHitQuipSlot` | **Brown Player** Delayed event `0xdb` quip. |
| `0x0c` | 12 | `sample_12.wav` | 0.392s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetDelayedQuipSlot` | **Brown Player** Delayed event `0x01` quip. |
| `0x0d` | 13 | `sample_13.wav` | 0.887s | `_Globals::CGaming_SpawnBulletAndPlaySound`, `_Globals::FUN_0041fdf0` | **Shotgun Fire** SFX / Weapon fire shot sound. |
| `0x0e` | 14 | `sample_14.wav` | 0.209s | `CRadio::CRadio_OnMouseDown` | Radio Button checkmark tick / click sound. |
| `0x0f` | 15 | `sample_15.wav` | 0.570s | `_Globals::CBulanek_PlayItemPickupSound` | **Item Pickup** confirmation SFX (plays when picking up weapon/ammo/shield). |
| `0x10` | 16 | `sample_16.wav` | 0.583s | `CBulanek::CBulanek_ResetAmmoAndPlayReload` | **Weapon Reload / Max Ammo** SFX (plays on pistol ammo reset or reload). |
| `0x11` | 17 | `sample_11.wav` | 1.001s | `CGameCounter::OnEvent` | Match start voice cue (**"Vpřed!"**). |
| `0x12` | 18 | `sample_12.wav` | 0.276s | `_Globals::CGaming_OnTimerCountdown`, `CGameCounter::OnEvent` | Delayed round match **countdown tick** beep SFX. |
| `0x13` | 19 | `sample_13.wav` | 0.081s | `_Globals::CGaming_SpawnBulletAndPlaySound`, `_Globals::FUN_0041fce0` | **Dry-Fire (Out of Ammo)** click SFX (empty weapon click sound). |
| `0x14` | 20 | `sample_14.wav` | 0.353s | `CIcon::CIcon_OnMouseUp`, dialog confirmation buttons | Standard menu button / **option select** confirmation SFX. |
| `0x15` | 21 | `sample_15.wav` | 0.200s | `_Globals::CGaming_SpawnBulletAndPlaySound`, `_Globals::FUN_0041fce0` | **Pistol/Standard Gun Shot** SFX. |
| `0x16` | 22 | `sample_16.wav` | 0.159s | `CBulanek::CBulanek_OnDeath` | **Female Bulanek** death squeak / high-pitch pain squeal SFX. |
| `0x17` | 23 | `sample_17.wav` | 0.203s | `CBulanek::CBulanek_OnDeath` | **Male Bulanek** death squeak / standard pain groan SFX. |
| `0x18` | 24 | `sample_18.wav` | 1.714s | `CMenu::Cmd_Dispatch` (case `0xca`) | Main menu selection voice cue: **"Historie"**. |
| `0x19` | 25 | `sample_19.wav` | 1.645s | `CMenu::Cmd_Dispatch` (case `0xcb`) | Main menu selection voice cue: **"Konec"**. |
| `0x1a` | 26 | `sample_20.wav` | 2.083s | `CMenu::DispatchHotkey` (cmd `0x8004`) | Main menu exit-confirm voice cue: **"Konec hry"**. |
| `0x1b` | 27 | `sample_21.wav` | 1.944s | `CMenu::Cmd_Dispatch` (case `0xc9`) | Main menu selection voice cue: **"Start hry"** (startup bark). |
| `0x1c` | 28 | `sample_22.wav` | 1.660s | `CMenu::DispatchHotkey` (cmd `0x80ce`) | Main menu exit alternate voice cue. |
| `0x1d` | 29 | `sample_23.wav` | 2.990s | `CScore::CScore_ctor` (or `CBulanci::FUN_00411010`) | **Lobby Scoreboard Background Loop** (general ambient sound). |
| `0x1e` | 30 | `sample_30.wav` | 1.469s | `CVolume::CVolume_OnMouseDown` | Volume slider adjust tick / click preview SFX. |
| `0x1f` | 31 | `sample_31.wav` | 1.001s | `_Globals::CGaming_OnPlayerCollectItem` (case 3) | Player response quip 3 upon weapon collection. |
| `0x20` | 32 | `sample_32.wav` | 0.440s | `_Globals::CGaming_OnPlayerCollectItem` (case 0) | Player response quip 0 upon weapon collection. |
| `0x21` | 33 | `sample_33.wav` | 1.036s | `_Globals::CGaming_OnPlayerCollectItem` (case 2) | Player response quip 2 upon weapon collection. |
| `0x22` | 34 | `sample_34.wav` | 0.781s | `_Globals::CGaming_OnPlayerCollectItem` (default/special) | Player response quip default/special upon weapon collection. |
| `0x23` | 35 | `sample_35.wav` | 0.951s | `_Globals::CGaming_OnPlayerCollectItem` (case 1) | Player response quip 1 upon weapon collection. |
| `0x24` | 36 | `sample_36.wav` | 0.087s | `CBulanek::CBulanek_OnTakeDamage` | **Bullet Hit / Pain Grunt** SFX (short "oof"). |
| `0x25` | 37 | `sample_37.wav` | 1.800s | *(Unused / level-loaded / alternative quip)* | Unused / general alternative quip. |
| `0x26` | 38 | `sample_38.wav` | 1.495s | `_Globals::CGaming_RespawnPlayer` via `CBulanek_GetSpawnQuipSlot` | **Special/Spell Character** spawn/re-entry response quip. |
| `0x27` | 39 | `sample_39.wav` | 1.329s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetHitQuipSlot` | **Special/Spell Character** event `0xdb` response quip. |
| `0x28` | 40 | `sample_40.wav` | 1.294s | `CBulanek::CBulanek_OnEvent` via `CBulanek_GetDelayedQuipSlot` | **Special/Spell Character** event `0x01` response quip. |

*(Note: Durations rounded to milliseconds. Character quips play on spawning, taking damage, picking up items, and on random delayed ticks.)*

---

## 5. Level-Specific Soundbanks and Scripting Integration

While the global soundbank (`0x10152` / `65874`) contains the core game mechanics audio (UI, player voice lines, standard weapon fire), specific levels override or extend this using custom level soundbanks. 

### 5.1 Architecture & Lifecycle of Level Soundbanks

During initialization, the gameplay system sets up the current level's audio environment. This involves loading a custom `CDSAudioBankIndex` that dynamically overrides slot configurations.

1. **Fallback/Default Gameplay Soundbank**:
   In `CBulanci::CGaming_ctor` (at `0x00420180`), the engine explicitly pre-loads **Resource `0x10003`** (which corresponds to `CDSAudioBankIndex` `65539` / AudioBank `65873` containing 8 samples).
   ```assembly
   00420180  PUSH  0x10003  ; Load the default gameplay soundbank
   00420185  CALL  CDSAudioLoader::LoadAudioBankIndex
   ...
   004201a6  MOV   dword ptr [DAT_004b3730], ECX  ; Save default soundbank index pointer
   ```
   This means that any level starting without an explicit custom audio bank automatically falls back to Bank `65539` for standard level and item-pickup effects.

2. **Custom Audio Bank Bindings via Level Scripts**:
   The level's `.lua` scripts can dynamically load and bind custom level-specific audio banks via the script dispatch command `engine.setMusic(musicTrackId, soundBankIndexId)`.
   - The handler `CLevelScript_SetMusic` is located at `0x0041bb00` in `bulanci.exe` and delegates to `_Globals::CGaming_SetMusicAndSoundBank(this, musicTrackId, soundBankIndexId)` (at `0x0041ba60`).
   - If `soundBankIndexId` is non-zero, the custom `CDSAudioBankIndex` is loaded from resources and replaces the default pointer stored in `g_pApp + 0x284` and `DAT_004b3730`.

The following four level-specific soundbanks are loaded and mapped based on this infrastructure:

| AudioBankIndex ID | AudioBank ID | Level Name (CZ) | Level Name (EN) | Music/Ambient MP3 ID | Level Script File |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **`65539`** *(Default)* | `65873` | *(Fallback default)* | *(Fallback default)* | `100003` / `65861` etc. | (Used by all fallback levels) |
| **`65536`** | `65870` | V zajetí 8-bitu | 8-bit | `65864` | `res_0000065859_2026_Script.lua` |
| **`65537`** | `65871` | Na dobrou noc | Bedtime story | `65868` | `res_0000065856_2026_Script.lua` |
| **`65538`** | `65872` | Noční směna | Steel works | `65863` | `res_0000065857_2026_Script.lua` |

### 5.2 Script-Triggered Audio (`PlaySoundAtView` / `SpawnAtView`)

In Lua scripts, sounds are triggered through a specific script dispatch command that was historically mislabeled as `spawnAtView` due to overlapping argument types.
- **Real Name / Purpose**: `PlaySoundAtView(sampleId, slotId)`
- **Opcode Address**: `0x00418950` (formerly listed as `SpawnAtView` in the dispatch table)
- **Implementation**: Delegates to `_Globals::TriggerLevelScriptSound(this, slotId, sampleId)` (located at `0x00418000`).
  - If `slotId` is valid, the engine computes 3D panning/attenuation based on the object's spatial coordinates and triggers the sample with panning.
  - If `slotId` is `255` (0xFF), the sound is played as a global stereo track with no spatialization.

---

### 5.3 Level Soundbank Sample Mappings

#### A. Bank `65873` (Default Gameplay Soundbank - Fallback)
This bank provides the standard combat and environmental audio triggers for generic levels:

| Slot | Unpacked File | Duration (s) | Event / Trigger Description |
| :--- | :--- | :--- | :--- |
| **`0`** | `sample_00.wav` | 0.198s | Default item pickup slot `0` (used for standard ammo/weapon pickup). |
| **`1`** | `sample_01.wav` | 1.083s | **Hardcoded Level Explosion** sound (triggered via `CExplosion_Ctor` when a mine, rocket, or grenade detonates). |
| **`2`** | `sample_02.wav` | 0.468s | Default item pickup slot `2` (weapon pickup confirmation). |
| **`3`** | `sample_03.wav` | 0.101s | Bullet ricochet/wall impact secondary sound effect. |
| **`4`** | `sample_04.wav` | 0.348s | Default item pickup slot `4`. |
| **`5`** | `sample_05.wav` | 0.231s | Default item pickup slot `5`. |
| **`6`** | `sample_06.wav` | 0.382s | General water puddle splash effect (puddle step alternative). |
| **`7`** | `sample_07.wav` | 1.425s | Match start level-specific horn / countdown siren. |

#### B. Bank `65870` (V zajetí 8-bitu / 8-bit Level Soundbank)
Provides chip-tune style retro sound effects for the 8-bit themed map:

| Slot | Unpacked File | Duration (s) | Event / Trigger Description |
| :--- | :--- | :--- | :--- |
| **`0`** | `sample_00.wav` | 0.285s | Retro opponent spawn effect (triggered globally on 8-bit map via `engine.spawnAtView(0, 9)`). |
| **`1`** | `sample_01.wav` | 0.156s | **Retro 8-bit Explosion** sound (triggered via `CExplosion_Ctor` on detonation, or script event `engine.spawnAtView(1, 9)`). |
| **`2`** | `sample_02.wav` | 0.419s | Retro level event completion tick (triggered via script event `engine.spawnAtView(2, 9)`). |
| **`3`** | `sample_03.wav` | 0.244s | Retro low-beeping tick for arcade cabinet animations (triggered via `engine.spawnAtView(3, slot)`). |
| **`4`** | `sample_04.wav` | 0.666s | Retro high-beeping sweep for arcade cabinet animation triggers (triggered via `engine.spawnAtView(4, slot)`). |

#### C. Bank `65871` (Na dobrou noc / Bedtime Story Level Soundbank)
Provides whimsical and toy-like sounds for the kids' bedroom themed map:

| Slot | Unpacked File | Duration (s) | Event / Trigger Description |
| :--- | :--- | :--- | :--- |
| **`0`** | `sample_00.wav` | 0.514s | Whimsical lamp turn on/off click (triggered via bedroom script event `engine.spawnAtView(0, 4)`). |
| **`1`** | `sample_01.wav` | 0.332s | **Whimsical Pillow Pop / Explosion** sound (triggered via `CExplosion_Ctor` or bedroom script event `engine.spawnAtView(1, 6)`). |
| **`2`** | `sample_02.wav` | 0.202s | Toy box squeak / bedside clock wind-up sound (triggered via bedroom script event `engine.spawnAtView(2, 4)`). |
| **`3`** | `sample_03.wav` | 0.235s | Bed creaking sound or toy squeak effect (triggered via bedroom script event `engine.spawnAtView(3, 9)`). |
| **`4`** | `sample_04.wav` | 0.466s | Monster/alien sleep snort or bed monster roar (triggered via bedroom script event `engine.spawnAtView(4, 9)`). |

#### D. Bank `65872` (Noční směna / Steel Works Level Soundbank)
Heavy machinery and industrial sounds for the steel mills themed map:

| Slot | Unpacked File | Duration (s) | Event / Trigger Description |
| :--- | :--- | :--- | :--- |
| **`0`** | `sample_00.wav` | 4.090s | **Hydraulic Press Heavy Ambient Loop** (triggered globally on Steel Works map via `engine.spawnAtView(0, 255)`). |
| **`1`** | `sample_01.wav` | 0.429s | **Heavy Industrial Detonation / Crush** sound (triggered via `CExplosion_Ctor` or script event `engine.spawnAtView(1, slot)`). |
| **`2`** | `sample_02.wav` | 0.380s | **Molten Metal Spark Clang / Metal Plate Hit** (triggered on spark/projectile spawn via script event `engine.spawnAtView(2, slot)`). |
