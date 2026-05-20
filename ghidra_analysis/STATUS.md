# Subsystem Understanding Tracker Index

Confidence levels:

* **Empty** — not yet investigated.
* **Sketched** — high-level shape only; details TBD.
* **Partial** — most of the surface mapped; specific corners uncertain.
* **Verified** — Ghidra-cross-checked: layouts, dispatch tables and
  callsites are tied to specific addresses in `bulanci.exe`.

A subsystem at **Verified** is ready to drive the match track — see the root [README.md](README.md) ("Handoff to the match track").

---

## Subsystem Trackers

The reverse-engineering status tracking is split into logical folders:

### 1. [File & Payload Formats](./formats/status.md)
* **Confidence:** High coverage (mostly **Verified**)
* **Components:**
  * Resource container (`.eap` / `.eapres` / overlay) — **Verified**
  * BitmapSprite (`ClassID 52`) — **Verified**
  * MPx audio stream — **Verified**
  * DSM multi-stream container — **Verified**
  * FLX animation file — **Verified**
  * JPEG codec — **Partial**
  * BMP decoder — **Verified**
  * Stream hierarchy — **Verified**
  * GZip compression — **Verified**
  * Editor colophon record — **Decoded**
  * Localised text — **Verified**

### 2. [Netcode Subsystems](./netcode/status.md)
* **Confidence:** High coverage (**Verified**)
* **Components:**
  * Network (DirectPlay) — **Verified**
  * Net dispatcher — **Verified**

### 3. [Engine Subsystems & Runtimes](./engine/status.md)
* **Confidence:** Mixed coverage (from **Empty** to **Verified**)
* **Components:**
  * Script VM — **Verified**
  * App shell — **Verified**
  * Animation runtime — **Partial**
  * DirectSound wrapper — **Empty**
  * Audio bank — **Empty**
  * Threading — **Empty**
  * Input — **Empty**
  * Exception hierarchy — **Empty**
  * Widget base contract — **Empty**

### 4. [Gameplay & Game Logic](./gameplay/status.md)
* **Confidence:** Mixed coverage (from **Empty** to **Partial**)
* **Components:**
  * Player entity (Bulánci) — **Empty**
  * Combat / projectiles — **Empty**
  * Match orchestration — **Sketched**
  * Menu / start state machine — **Partial**
  * Multiplayer lobby UI — **Empty**
  * Scoring / HUD counters — **Empty**
  * Movie / cinema views — **Verified**
  * Save / profile format — **Empty**
  * Advertising — **Sketched**
  * Help / History scripts — **Sketched**

---

## Updating Status Trackers

When you produce new findings:
1. Locate the correct sub-tracker (e.g., `formats/status.md`, `engine/status.md`).
2. Update the entry's confidence level, knowledge summary, and open questions.
3. Keep the root `STATUS.md` index in sync.
