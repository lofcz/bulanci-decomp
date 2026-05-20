# Netcode Subsystem Status

Confidence levels:
* **Empty** — not yet investigated.
* **Sketched** — high-level shape only; details TBD.
* **Partial** — most of the surface mapped; specific corners uncertain.
* **Verified** — Ghidra-cross-checked: layouts, dispatch tables and callsites are tied to specific addresses in `bulanci.exe`.

---

## Network Subsystems

These govern the network layer, DirectPlay wrapper, and remote state synchronization.

### Network (DirectPlay)

* **Classes:** `CDSDirectPlay`, `CDSDirectPlaySender`
* **Bytes:** 0.26 KB
* **Confidence:** **Verified**
* **What's known:**
  * DirectX 7 `IDirectPlay4A` + `IDirectPlayLobby3A` (CoCreated via `CLSID_DirectPlay` / `CLSID_DirectPlayLobby`).
  * Transports: TCP/IP (`DPSPGUID_TCPIP`) and IPX (`DPSPGUID_IPX`).
  * App GUID `{739ECD1D-7DDC-4116-8444-38F677CB329F}` @ `0x00481a50`.
  * Object layout (`CDSDirectPlay` @ `0x8c` bytes) and embedded `CDSDirectPlaySender` (queue + worker thread draining via `IDirectPlay4::Send` vtbl[26] @ +0x68 with `DPSEND_GUARANTEED`) mapped end-to-end.
  * Lifecycle: `CoCreateInstance` → `EnumConnections` → `InitializeConnection` → `Open(CREATE|JOIN)` → `CreatePlayer` → `Sender::Bind`.
  * Host migration flag set on host (`DPSESSION_MIGRATEHOST|KEEPALIVE|DIRECTPLAYPROTOCOL = 0x2044`).
* **Open questions:** Engine-level host migration is not wired (the `DPSYS_HOST = 0x000D` system msg is not handled in the dispatcher).
* **Artefacts:** `./net_protocol.md`

### Net dispatcher

* **Classes:** `CGame::ProcessNetMessage` (`FUN_00415290`, **not** `FUN_004185a0`)
* **Bytes:** —
* **Confidence:** **Verified**
* **What's known:**
  * Full first-byte switch over msg types `0x00..0x19, 0x64` mapped to engine handlers and to each of the 27 `CDSDirectPlay::Send` call-sites (25 unique type bytes).
  * Recording playback (`CGaming::FUN_00415f80`) and live receive (`CGame::FUN_00416030` case 0 via `CDSDirectPlay::Receive` `FUN_0043ab70`, vtbl[25] @ +0x64) both funnel through this entry point.
  * `FUN_004185a0` is the case-`0x15` body that pushes the payload into `cgame+0x1ac` (`CChainedStrm`) and fires script export #9.
  * System-msg pseudo-player (`idFrom==0`) cases 5/0x31/0x101 documented.
  * Replication model is event-only (no per-tick snapshots):
    * player state via msg `0x0d`
    * projectiles via `0x0f`
    * hits via `0x10`
    * damage via `0x0c`
    * death via `0x11`
    * item placement via `0x18`
    * round timing via `0x12` / `0x16` / `0x17`
* **Open questions:** None at this level. The bodies of `FUN_0041d280`, `FUN_0041f010`, `FUN_0041f210`, `FUN_004209f0`, `FUN_00420510`, `FUN_00420a70` (per-msg handlers inside `CGaming`) still need their field-layout passes if/when we match `CGaming` byte-exactly.
* **Artefacts:** `./net_protocol.md`
