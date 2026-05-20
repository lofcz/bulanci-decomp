# Match Orchestration Subsystem Analysis

The **Match Orchestration** subsystem controls the full lifecycle of a gameplay session—from lobby configurations and asset loading, to active gameplay ticks, round-end conditions, and high-score synchronization.

The subsystem is composed of several key components:
* **`CGame`** (or `CBulanci`'s game sub-object): DirectPlay host/join network driver and player roster repository.
* **`CGaming`**: The modal `CDSView` that drives active gameplay and orchestrates active level scripts, entity updates, item spawns, and match ticks.
* **`CLevelList`**: A custom `CListBox` widget displaying selectable map files.
* **`CLoadingLevel`**: A progress-bar and message window displayed during level loading/syncing.
* **`CLevelScore`**: A high-score table tracker persisting up to 6 custom score records (`CScoreItem`) per level.

---

## State Machine Overview

The high-level match state machine is tracked by `CGame::m_state` (at byte offset `0x30` of the `CGame` structure):

```
       +---------------------------------------------+
       |                  1. LOBBY                   |
       |  CStartGame2 modal screen / slot planning  |
       +----------------------+----------------------+
                              |
                     [Admin Starts Game]
                              v
       +---------------------------------------------+
       |          2. INITIALIZING / SYNCING          |
       |  State 4 | CGaming is allocated on stack   |
       |  Loads level script, runs OnInit export #1  |
       +----------------------+----------------------+
                              |
                              v
       +---------------------------------------------+
       |              3. LOADING ASSETS              |
       |  State 5 | Displays CLoadingLevel modally   |
       |  Downloads map assets, ticks CProgressBar   |
       |  Spins up CDSWavStream background music     |
       +----------------------+----------------------+
                              |
                              v
       +---------------------------------------------+
       |             4. ACTIVE GAMEPLAY              |
       |  State 6 | Shows CGaming view modally       |
       |  Pumps Win32, Scheduler, NetMessage, Render  |
       +----------------------+----------------------+
                              |
                     [Round End / Exit]
                              v
       +---------------------------------------------+
       |         5. TRANSITION & HIGH SCORES         |
       |  Sends net msg 0x0B (Round End result)      |
       |  Presents CPoemScroller transition         |
       |  Sorts / updates CLevelScore High Scores    |
       +----------------------+----------------------+
                              |
                              v
                    [Back to CStartGame2]
```

---

## Detailed Component Analysis

### 1. Match Init & Asset Loading (`CBulanci::CGame_StartGame @ 0x00413ce0`)
When a match is scheduled to start, `CGame_StartGame` transitions `CGame::m_state` to `4` (Initializing) and manages the setup sequence:
1. **Network Sync**: In multiplayer, the host sends net message `0x08` (state wait) to coordinate clients.
2. **`CGaming` Allocation**: It instantiates `CGaming` (`CGaming_ctor @ 0x0041ff90`) directly on the stack.
3. **Script Lifecycle Integration**: During construction, `CGaming_ctor` loads the map script and fires script export #1 (`OnInit`).
4. **Transition to State `5`**: The state becomes `5` (Assets loading). It instantiates `CLoadingLevel` (`CLoadingLevel_ctor @ 0x0040dfb0`) and displays it modally.
5. **Asset Download / Stream Chunking**:
   * The map file is downloaded or stream-copied in **16 KB (0x4000 byte) chunks** using `CheckedVirtualBaseCast` streams.
   * `CLoadingLevel_SetProgress` (`0x0040abc0`) is called to step the loading progress bar.
   * App ticks are pumped using `CDSApp_FrameBody` to ensure the window doesn't hang.
6. **Background Music Setup**: `CGaming_LoadBackgroundMusic @ 0x0041b6d0` instantiates a `CDSWavStream`, binds it to the level file, and hands it off to `CDSAudioPlayer` to start background music loop play.
7. **Modal Transition**: Once fully loaded, `CLoadingLevel` is popped and destroyed, state becomes `6` (Playing), and the `CGaming` view is pushed modally.

---

### 2. Active Match Frame Ticking (`CGaming::DoModal`)
While in state `6`, the modal message pump `CDSView_DoModal` drives the frame body. Every frame ticks via `CDSApp_FrameBody`, invoking:

#### A. Scheduler Tasks & Timers (`CGaming_OnSchedulerTimer @ 0x0041f050`)
`CGaming` registers multiple task timers with the engine's scheduler:
* **Event `0` — Ambient Animations** (`CGaming_TickAmbientAnimations @ 0x0041b500`):
  Runs only if in single-player or tutorial mode. Picks a random slot among pre-loaded ambient sprites (e.g., flies, background flickering, rain effects at `0x324..0x330`) and updates their frames.
* **Event `1` — Pickup Spawning** (`CGaming_RandomPickupSpawner_Tick @ 0x0041e350`):
  Called regularly. The host rolls random percentages to spawning item pickups (`world_slots 101..107` for weapons and `world_slot 100` for special pickups) and broadcasts them over the wire via net message types `0x13` or `0x18`.
* **Event `2` — Match Timers**:
  Tracks round countdown triggers and ticking clock syncs.

#### B. Network Receive & Dispatch (`CGame_ProcessNetMessage @ 0x00415290`)
The main loop processes network packets in real-time or replays recorded inputs from demos:
* **`0x0D` (Player State)**: Coordinates positional synchronization of player avatars.
* **`0x0E` (Primary Action)**: Fires weapon pistol shots, drops mines, or throws grenades.
* **`0x0F` (Bullet Spawning)**: Deterministically spawns projectiles on all clients simultaneously.
* **`0x10` (Hit Confirmation)**: Registers bullet impact coordinates.
* **`0x11` (Player Die)**: Triggers respawn countdowns.
* **`0x15` (OnNetCustom)**: Integrates level-script custom network events through script export #9 (`OnNetCustom`).

#### C. Active Rendering (`CDSView::Draw @ 0x0042ccf0`)
Invokes standard double-buffered view renders, drawing the static layer, active entity sprites, projectiles, hit markers, and HUD counters.

---

### 3. Match End Sequence (`CGaming_OnCmd @ 0x0041d5f0`)
The round terminates when a player quits or time/scores run out. The active view exits through:
1. **Modal Exit Code Setup**: The command ID (e.g., `0x80cc`, `0x80cd`) is saved as the view exit code.
2. **Network Broadcast**: The host or client transmits net message `0x0B` (round end result packet) via `CGame_NetSendRoundResult_t0b @ 0x00413180`.
3. **Transition Screens**:
   * It spins up the scrolling credit/poem transition widget (`CPoemScroller` via `_Globals::FUN_00422430`).
   * It increments an orchestration wait-counter (`this+0x338`).
   * It blocks the modal exit by continuing to pump `CDSApp_PumpTick` in a `while` loop until `OnCustomEvent` (`0x004206a0` case 1) receives notification that the screen finished scroll-fading (reducing the wait-counter back to `0`).
4. **Script Deinitialization**: Upon modal loop break, `CGaming` is torn down, and `CGaming_dtor @ 0x0041b850` triggers level script export #2 (`OnDeinit`).

---

### 4. Persistence & High Scores (`CLevelScore`)
Match scores are retained and aggregated per-level in a specialized high-scores list:
* **Roster Allocation**: `CGame_GetOrCreateLevelScore @ 0x00414280` ensures that a high-score table exists for the level. If not, it allocates `0x28` bytes on the heap, setups vftables (`CLevelScore::vftable` at `0x480450`), and calls `CLevelScore_InitializeDefaultScores @ 0x00409620` to fill 6 slots with default mock players.
* **Adding Records**: `CLevelScore_AddPlayerScore @ 0x00409b10` encapsulates the player’s stats inside a `CScoreItem` (containing broad-string name, kills, and deaths).
* **Sorting and Pruning**:
   * It sorts the records descending by points using `FUN_0042fae0` and comparison function `LAB_00408fd0`.
   * It prunes the database by deleting any records beyond index 6, maintaining only the top 6.

---

## Function Rename Manifest (Ghidra & Mapping)

The following addresses have been successfully renamed in Ghidra to align with this analysis:

| Address | Original Symbol | Assigned Name | Role |
| :--- | :--- | :--- | :--- |
| `0x0041b850` | `FUN_0041b850` | `CGaming_dtor` | Destructs CGaming; calls Level Script `OnDeinit` export #2. |
| `0x0041ff90` | `FUN_0041ff90` | `CGaming_ctor` | Constructs CGaming; triggers Level Script `OnInit` export #1. |
| `0x0041d2e0` | `FUN_0041d2e0` | `CGaming_LoadLevelAssetAndMusic` | Handles temporary file extraction and stream initialization. |
| `0x0041b6d0` | `FUN_0041b6d0` | `CGaming_LoadBackgroundMusic` | Opens map wav stream and starts CDSAudioPlayer. |
| `0x0040abc0` | `FUN_0040abc0` | `CLoadingLevel_SetProgress` | Translates loading chunks into progressive bar updates. |
| `0x0040dfb0` | `FUN_0040dfb0` | `CLoadingLevel_ctor` | Standard dialog/window builder for the loading progress. |
| `0x0041f050` | `FUN_0041f050` | `CGaming_OnSchedulerTimer` | Coordinates ambient anims, item spawning, and clock events. |
| `0x0041b500` | `FUN_0041b500` | `CGaming_TickAmbientAnimations` | Performs random sprite adjustments for ambient background details. |
| `0x0041d5f0` | `FUN_0041d5f0` | `CGaming_OnCmd` | Implements round-end transitions and exit loops. |
| `0x004206a0` | `FUN_004206a0` | `CGaming_OnCustomEvent` | Routes custom child UI updates back to the parent. |
| `0x00414280` | `FUN_00414280` | `CGame_GetOrCreateLevelScore` | Instantiates or obtains the active high-score list. |
| `0x00409b10` | `FUN_00409b10` | `CLevelScore_AddPlayerScore` | Builds, registers, sorts, and limits scores to Top 6. |
| `0x00409620` | `FUN_00409620` | `CLevelScore_InitializeDefaultScores` | Populates mock records for unplayed levels. |
| `0x00409a40` | `FUN_00409a40` | `CLevelScore_scalar_deleting_dtor` | Standard scalar-deleting destructor wrapper. |
| `0x00409330` | `FUN_00409330` | `CLevelScore_dtor` | Cleanly dismantles collected items and score views. |
| `0x0040b0c0` | `FUN_0040b0c0` | `CLevelList_ctor` | Build level chooser CListBox. |
| `0x0040d490` | `FUN_0040d490` | `CLevelList_DrawItem` | Lay outs and rasterizes UTF-16 level name labels. |
