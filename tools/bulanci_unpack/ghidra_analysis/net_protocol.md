# Network protocol — DirectPlay transport + `CGame::ProcessNetMessage`

The multiplayer stack is two layers thick:

```
+----------------------------------------------------+
| CGame::ProcessNetMessage  (FUN_00415290)           |  ←— first-byte dispatch
| switch ((char)msg[0]) { case 0x00..0x19, 0x64 }    |
+----------------------------------------------------+
       ▲ (called per-frame from CGame::FUN_00416030  ▲ (and from the
       ▲  case 0, which pulls one message via Recv)  ▲  recording-playback
       ▲                                             ▲  loop in
       ▲ CDSDirectPlay::Receive (FUN_0043ab70)       ▲  CGaming::FUN_00415f80)
       ▲   = IDirectPlay4::Receive vtbl[25] @+0x64   ▲
       ▲                                             ▲
+----------------------------------------------------+
| CDSDirectPlay (60 + sender ≈ 0x8c bytes)           |
|   +0x00 : vftable                                  |
|   +0x04 : IDirectPlay4A*   ←— Send / Receive       |
|   +0x08 : IDirectPlayLobby3A* ←— CreateCompound…   |
|   +0x0c..+0x18 : GUID  serviceProvider             |
|   +0x1c..+0x28 : GUID  application                 |
|   +0x2c : LPVOID dpConnectionAddr (heap)           |
|   +0x30 : DWORD  dpConnectionAddrSize              |
|   +0x34 : byte   sessionOpen (0/1)                 |
|   +0x35 : byte   initialized   (0/1)               |
|   +0x38 : DPID   localPlayerId                     |
|   +0x3c : CDSDirectPlaySender  (embedded)          |
|     ┌─────────────────────────────────────────────┐|
|     | +0x00 : primary  vftable  (CDSDirectPlay)   ||
|     | +0x04 : secondary vftable (CDSWorkingThread)||
|     | +0x18..+0x34 : CDSMemQueue (lock+ringbuf)   ||
|     | +0x38 : void* scratchBuf  (re-used on drain)||
|     | +0x40 : HANDLE wakeEvent                    ||
|     | +0x44 : byte   stopFlag                     ||
|     | +0x48 : IDirectPlay4A*  (bound by parent)   ||
|     | +0x4c : DPID   idFrom                       ||
|     └─────────────────────────────────────────────┘|
+----------------------------------------------------+
        ▲
        ▲ outbound: CDSDirectPlay::Send (FUN_0043b6d0)
        ▲   = sender.EnqueueSend(buf, len) — non-blocking
        ▲   worker thread (FUN_0043ad20) drains via
        ▲   IDirectPlay4::Send  vtbl[26] @+0x68
        ▲   args: (idFrom, idTo=0, flags=DPSEND_GUARANTEED, buf, len)
```

`CGame` stores its `CDSDirectPlay*` at `CGame+0x1dc`. CGaming inherits from
CGame at offset 4, so the same field is `CGaming+0x1d8`.

## RTTI / vtable anchors

```
.?AVCDSDirectPlay@@        TypeDescriptor   @ 0x004b026c
  COL                      @ 0x004a4ab8 → vftable @ 0x004877e4
    vft[0] = 0x0043b2d0   CDSDirectPlay::GetTypeID  (returns &DAT_004b83c8)
    vft[1] = 0x0043b600   CDSDirectPlay::~CDSDirectPlay (scalar-delete dtor)
    vft[2] = 0x004245c0   shared CDSObject helper (Clone? Hash?)

.?AVCDSDirectPlaySender@@  TypeDescriptor   @ 0x004b022c
  COL (primary base)       @ 0x004a49fc → vftable @ 0x004877d4
    vft[0] = 0x0043b130   CDSDirectPlaySender::GetTypeID
    vft[1] = 0x0043b530   CDSDirectPlaySender::~CDSDirectPlaySender
    vft[2] = 0x004245c0   inherited helper
  COL (secondary base)     @ 0x004a4aa4 → vftable @ 0x004877bc
    vft[0] = 0x0043b120   CDSDirectPlaySender::GetTypeID (this-adjustor)
    vft[1] = 0x0042ac90   CDSChain::FUN_0042ac90 (this-adjust → primary)
    vft[2] = 0x0041aa40   CDSWorkingThread::Run (?)
    vft[3] = 0x0043b140   thunk → primary dtor
    vft[4] = 0x0043ad20   CDSDirectPlaySender::ThreadEntry (drain loop)
```

`CDSDirectPlaySender` is multiple-inheritance: primary base = `CDSDirectPlay`
slot derivative, secondary base = a `CDSWorkingThread`-shaped thread holder.

## DirectPlay COM objects

`CDSDirectPlay::SetGuids` (`FUN_0043ae70`) is the one place where the engine
talks to COM:

```c
CoCreateInstance(CLSID_DirectPlay,      NULL, CLSCTX_INPROC, IID_IDirectPlay4A,      &this[+0x04]);
CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC, IID_IDirectPlayLobby3A, &this[+0x08]);
```

GUIDs are loaded from `.rdata` at the addresses below.

| `.rdata` addr | GUID                                            | Meaning                       |
|---|---|---|
| `0x00486d50` | `{D1EB6D20-8923-11D0-9D97-00A0C90A43CB}`        | `CLSID_DirectPlay`            |
| `0x00486d60` | `{0AB1C531-4745-11D1-A7A1-0000F803ABFC}`        | `IID_IDirectPlay4A`           |
| `0x00486d10` | `{2FE8F810-B2A5-11D0-A787-0000F803ABFC}`        | `CLSID_DirectPlayLobby`       |
| `0x00486d20` | `{2DB72491-652C-11D1-A7A8-0000F803ABFC}`        | `IID_IDirectPlayLobby3A`      |
| `0x00486d30` | `{36E95EE0-8577-11CF-960C-0080C7534E82}`        | `DPSPGUID_TCPIP`              |
| `0x00486d40` | `{685BC400-9D2C-11CF-A9CD-00AA006886E3}`        | `DPSPGUID_IPX`                |
| `0x00486cf0` | `{C4A54DA0-E0AF-11CF-9C4E-00A0C905425E}`        | `DPAID_INet` (hostname chunk) |
| `0x00486d00` | `{07D916C0-E0AF-11CF-9C4E-00A0C905425E}`        | `DPAID_ServiceProvider`       |
| `0x00481a50` | `{739ECD1D-7DDC-4116-8444-38F677CB329F}`        | **Bulánci application GUID**  |

DirectPlay 4 (DirectX 7 era) is the surface. Bulánci uses `IDirectPlay4A`
exclusively (ANSI variant). No DPlay 8 / DirectPlay Voice traces.

## `CDSDirectPlay` public API (engine-side wrappers)

| Address | Name (best-effort)        | Vtbl call             | Notes                                                                                                  |
|---|---|---|---|
| `0x0043b270` | `CDSDirectPlay::CDSDirectPlay()`             | —              | Zeroes GUIDs and member ptrs; ctors the embedded sender, which spins up the worker thread.            |
| `0x0043b2e0` | `CDSDirectPlay::~CDSDirectPlay()` (impl)     | —              | Free conn buffer, dtor sender, drop COM refs.                                                          |
| `0x0043ae70` | `CDSDirectPlay::SetGuids(sp, app)`           | `CoCreateInstance` ×2 | Stores SP GUID at +0x0c, app GUID at +0x1c. Always called by the host/join helpers.                   |
| `0x0043b620` | `CDSDirectPlay::ConnectLobby(sp, app)`       | `EnumConnections` @+0x8c | Used for hosting **and** for the IPX fallback path; uses the in-process service provider directly.    |
| `0x0043b360` | `CDSDirectPlay::ConnectTCP(sp, app, host)`   | `IDirectPlayLobby3::CreateCompoundAddress` @+0x38 | Builds a DPADDRESS with DPAID_ServiceProvider + DPAID_INet(host string), then `InitializeConnection`. |
| `0x0043abd0` | `CDSDirectPlay::InitializeConnection()`      | `InitializeConnection` @+0x98 | Marks `+0x35 = 1`.                                                                                    |
| `0x0043af00` | `CDSDirectPlay::HostSession(name)`           | `Open(SD, DPOPEN_CREATE)` @+0x60 + `CreatePlayer` @+0x18 | `DPSESSIONDESC2.dwFlags = 0x2044` = `DIRECTPLAYPROTOCOL\|MIGRATEHOST\|KEEPALIVE`. **Host migration is on.** |
| `0x0043afa0` | `CDSDirectPlay::JoinSession(instanceGuid)`   | `Open(SD, DPOPEN_JOIN)` @+0x60 + `CreatePlayer` @+0x18 | `dwFlags = 0x2000` (just `DIRECTPLAYPROTOCOL`). The session instance GUID comes from the lobby's session list. |
| `0x0043aca0` | `CDSDirectPlay::CreateLocalPlayer()`         | `CreatePlayer` @+0x18 + sender bind | Writes the local DPID into `+0x38` and propagates it to the sender's `+0x4c`.                         |
| `0x0043aba0` | `CDSDirectPlaySender::Bind(dp, idFrom)`      | — | Sender remembers its `IDirectPlay4*` (`+0x48`) and `idFrom` (`+0x4c`).                                 |
| `0x0043abc0` | `CDSDirectPlaySender::Unbind()`              | — | Just `sender->dp = NULL`; the worker keeps draining harmlessly.                                       |
| `0x0043ac00` | `CDSDirectPlay::EnumSessions(cb, ctx)`       | `EnumSessions` @+0x34 | Used by `CGaming` case 1 every refresh tick to populate the session list.                              |
| `0x0043ace0` | `CDSDirectPlay::Close()`                     | `DestroyPlayer` @+0x24 + `Close` @+0x10 | Tears down the local player then the session.                                                          |
| `0x0043ae20` | `CDSDirectPlay::Shutdown()`                  | `Release` @+0x08 ×2 + `Close()` | Full teardown including `IUnknown::Release` on both COM objects.                                       |
| `0x0043b6d0` | `CDSDirectPlay::Send(target=0, buf, len)`    | (forwards)     | Enqueues into `sender`. `target` is unused (always 0 by every caller).                                |
| `0x0043b680` | `CDSDirectPlaySender::EnqueueSend(buf, len)` | — | Lock → write u32 length → write payload → unlock → `SetEvent(wake)`.                                  |
| `0x0043ad20` | `CDSDirectPlaySender::ThreadEntry()`         | `IDirectPlay4::Send` @+0x68 | Worker loop: `WaitForSingleObject(wake)`, drain queue, `Send(idFrom, 0, DPSEND_GUARANTEED, buf, len)`. |
| `0x0043ab70` | `CDSDirectPlay::Receive(idFrom*, buf, &len)` | `IDirectPlay4::Receive` @+0x64 | Pumped from `CGame::FUN_00416030` case 0 every tick. Out-param `idTo` is forced to 0 (broadcast).      |

### Wire format

Every payload that crosses the wire is:

```
DWORD  dwFromID    ┐  added by DirectPlay
DWORD  dwLength    ┘  (used by recording playback only)
byte   msgType     ←  first byte of the payload Bulánci itself emits
…      msgBody     ←  type-specific, always little-endian
```

Everything is sent with `idTo=0` (**broadcast to every player in the session**)
and `DPSEND_GUARANTEED` (reliable, ordered — DirectPlay's TCP-like delivery
on top of either real TCP or IPX). No client-server filtering exists below
the dispatcher; filtering happens in `case` bodies via `this[0x30] == 0x6`
("we're in the playing state") and similar state checks.

## `CGame::ProcessNetMessage` (`FUN_00415290`)

The previous `STATUS.md` note pointed at `FUN_004185a0` as the dispatcher;
that's a misread — `FUN_004185a0` is just the **case 0x15 body** (it fires
the `OnNetCustom` script export). The real entry is **`FUN_00415290`**.

```c
void CGame::ProcessNetMessage(CGame *this, DPID idFrom, void *msg)
{
    if (idFrom == 0) {                  // DirectPlay system messages
        switch (*(uint *)msg) {
            case 5:    handleDpsysDestroyPlayer(this, msg[2]); break;
            case 0x31: throw CDSDirectXException(0x88770136); break;
            case 0x101:throw CDSSimpleException(2000, 0x1b);  break;
        }
        return;
    }
    switch ((char)msg[0]) {             // engine-defined messages
        case 0x00: ... case 0x19: ... case 0x64: ...
    }
}
```

Two callers feed into this dispatcher:

* **Live receive** — `CGame::FUN_00416030` case 0 (the per-tick `Receive`
  pump). `iVar4 = CDSDirectPlay::Receive(...)` → `-0x7788FFE2` =
  `DPERR_BUFFERTOOSMALL` (grow buffer & retry); `-0x7788FF42` =
  `DPERR_NOMESSAGES` (done for this tick); `0` = dispatch.
* **Recording playback** — `CGaming::FUN_00415f80` reads `DWORD idFrom +
  DWORD len + payload` records from a stream (`CGaming+0x1bc`,
  `IDSStream`-shaped) and replays them through the same dispatcher. The
  same stream is **also** the *writer*: when `CGame+0x209 != 0` (recording
  mode) and the live message type is in `0x0c..0x19`, the live receive path
  appends the record to the stream **instead of** dispatching it (see
  `FUN_00416030` case 0). The split is deliberate: gameplay state messages
  are journaled for demos; lobby / chat / setup messages are not.

### Game-defined message types

Byte-exact layouts. Offsets are from the byte handed to
`IDirectPlay4::Send`; the wire-level `DPID idFrom` is added by
DirectPlay and is **not** in the buffer. All multi-byte ints are
little-endian. `WCHAR` is UTF-16 LE. ANSI strings come from
`AtlGetThreadACPThunk` (system ACP).

#### Lobby / setup

| Type | Total | Layout (offset · field · meaning) | Sender (renamed in Ghidra) | Dispatcher handler |
|---:|---:|---|---|---|
| `0x00` | `0x2A + N*0x24` | `+0 u8 type=0` · `+1 u8 dpInit` (`CGame[0x35]`) · `+2 u8 N` (≤4) · `+3 u32 N` (loop count) · `+7..+0x29` 35 bytes uninitialised · `+0x2A` `N × PlayerRec[36]` (ANSI, zero-padded) | `CGame_OpenNetworkSession_AndSendJoin_t00` @ `0x00414dd0` | case 0 — host adds players, then broadcasts `0x01`. |
| `0x01` | `0x23 + M*0x24` | `+0 u8 type=1` · `+1 u32 echoedClientDPID` · `+5 u8 totalSlots` · `+6 u8 M` (≤4) · `+7 u8 hostSlot[4]` · `+0xB u32 dpid[4]` (always 4 slots; unused are noise) · `+0x1B u32 randSeed` · `+0x1F u32 M` · `+0x23` `M × PlayerRec[36]` | `CGame_ProcessNetMessage` case 0 itself (`0x00415290`) | case 1 — client transitions `CGame[0x30]: 1→2` and fills the roster. |
| `0x02` | `wcslen·2 + 4` | `+0 u8 type=2` · `+1 u8 slot` · `+2 WCHAR name[wcslen+1]` (UTF-16, null-terminated) | `CGame_NetSendRename_t02` @ `0x004139b0` | case 2 — `CBulanci::FUN_0042d510(slot, name)` + chat-list event 0xE1. |
| `0x03` | `3` | `+0 u8 type=3` · `+1 u8 slot` · `+2 u8 avatarIdx` | `CGame_NetSendSetAvatar_t03` @ `0x00412c70` | case 3 — `CGame[slot*0x23 + 0xdc] = avatar`. |
| `0x04` | `6` | `+0 u8 type=4` · `+1 u8 selectedIdx` · `+2 u32 selectedValue` | `CGame_NetSendSlotCount_t04` @ `0x00412cd0` | case 4 — `CGame[0x19c] = idx; CGame[idx*4 + 0x19e] = value`. |
| `0x05` | `0x26` | `+0 u8 type=5` · `+1 CHAR name[37]` (ANSI, zero-padded, null-terminated) | `CGame_NetSendSetLevel_t05` @ `0x00414340` | case 5 — `CGame[0xcc]` (level title) updated. **Fixed 38 bytes regardless of name length.** |
| `0x06` | `wcslen·2 + 4` | `+0 u8 type=6` · `+1 u8 slot` · `+2 WCHAR text[wcslen+1]` (UTF-16, null-terminated) | `CGame_NetSendChat_t06` @ `0x00413b90` | case 6 — chat-list event 0xE1 with the text. |
| `0x07` | `2` | `+0 u8 type=7` · `+1 u8 countdown` | `CGame_NetSendCountdown_t07` @ `0x00412da0` | case 7 — `CGame[0x1e8] = countdown`. |
| `0x08` | `3` | `+0 u8 type=8` · `+1 u8 slot` · `+2 u8 newState` (hard-coded `2` = "level loaded" at the only call-site) | `CGame_NetSendSlotChange_t08_t09` @ `0x00413ce0` (2nd Send) | case 8 — `CGame[slot*0xd + 0x16f] = state`. |
| `0x09` | `6` | `+0 u8 type=9` · `+1 u8 slot` (hard-coded `1`) · `+2 u32 value` (hard-coded `0`) | `CGame_NetSendSlotChange_t08_t09` @ `0x00413ce0` (1st Send) | case 9 — chat-list event 0xE3. |
| `0x0A` | `3` | `+0 u8 type=0xA` · `+1 u8 senderSlot` (`CGame[0xdb]`) · `+2 u8 newStatus` | `CGame_NetSendKick_t0a` @ `0x004124a0` | case A — only `CGame[0x30] == 6`. Sets `CGame[slot*0xd + 0x174] = newStatus` (per-slot mid-game status). |
| `0x64` | `2` | `+0 u8 type=0x64` · `+1 u8 adminByte` | `CGame_NetSendAdminByte_t64` @ `0x00412d40` | case `'d'` — `CGame[0x19d] = v`. |

`PlayerRec` is exactly `CHAR name[36]` (zero-padded). The engine widens
back to UTF-16 with `MultiByteToWideChar(ACP, …)` after copying off the
wire — there is no length prefix, just the null-terminated name in a
36-byte slot.

#### Match (gameplay)

All match cases no-op unless `CGame[0x30] == 6` (playing state).

| Type | Total | Layout                                                                                                                | Sender (renamed) | Dispatcher handler (in `CGame::CGame_ProcessNetMessage`) |
|---:|---:|---|---|---|
| `0x0B` | `2` | `+0 u8 type=0xB` · `+1 u8 isWin` (1 → engine event `0x80CC`, 0 → `0x80CD`)                                            | `CGame_NetSendRoundResult_t0b` @ `0x00413180` | case B — round-end UI event. |
| `0x0C` | `3` | `+0 u8 type=0xC` · `+1 u8 attacker` · `+2 u8 victim`                                                                  | `CGame_NetSendDamage_t0c` @ `0x00412a40` | case C — `FUN_0041f210(cgaming, atk, vic)`. |
| `0x0D` | `7` | `+0 u8 type=0xD` · `+1 u8 slot` · `+2 u8 animState` · `+3 S16 x` · `+5 S16 y`                                         | `CGame_NetSendPlayerState_t0d` @ `0x00412b10` (called from `CBulanek::FUN_00420910` on every anim change) | case D — `FUN_004209f0(cgaming, slot, animState, &xy)`. Position is signed 16-bit (truncated). |
| `0x0E` | `2` | `+0 u8 type=0xE` · `+1 u8 slot`                                                                                       | `CGame_NetSendPlayerEvent1_t0e` @ `0x00412b60` | case E — `FUN_00420a70(cgaming, slot)` → `FUN_004208c0(player)` → **`FUN_00420650(player)`** = "do primary action with currently-held item": `switch(*(byte*)(player+0x64))` → fire pistol (case 0) / throw grenade (case 1) / place mine (case 2 or 5) / case 3 / case 4. The weapon kind is implicit because it's already mirrored on every peer via `0x0d` / state messages. |
| `0x0F` | `8` | `+0 u8 type=0xF` · `+1 u8 shooter` · `+2 u8 target` (`0xFF` = none) · `+3 u8 weapon` · `+4 S16 x` · `+6 S16 y`        | `CGame_NetSendShotSpawn_t0f` @ `0x00412b90` | case F — `FUN_00417e80(cgaming, shooter, target, weapon, &xy, 0)`. Sent **alongside `0x0e`** for bullet-style weapons so the projectile spawns deterministically on every peer (kind + seed + velocity baked in). |
| `0x10` | `7` | `+0 u8 type=0x10` · `+1 u8 shooter` · `+2 u8 victim` · `+3 S16 x` · `+5 S16 y`                                        | `CGame_NetSendHit_t10` @ `0x00412bf0` | case 10 — `FUN_00420510(cgaming, shooter, victim, &xy)`. |
| `0x11` | `2` | `+0 u8 type=0x11` · `+1 u8 slot`                                                                                      | `CGame_NetSendPlayerDie_t11` @ `0x00412c40` | case 11 — `FUN_004180c0(cgaming, slot)` → `CBulanek::FUN_00417570` (death + respawn timer). |
| `0x12` | `3` | `+0 u8 type=0x12` · `+1 S16 secondsLeft`                                                                              | `CGame_NetSendRoundTimer_t12` @ `0x00412a00` | case 12 — `CGame[0x20e] = secs; FUN_00419d60(cgaming, secs, …)`. |
| `0x13` | `5` | `+0 u8 type=0x13` · `+1 S16 x` · `+3 S16 y`                                                                           | `CGame_NetSendWorldEvent_t13` @ `0x00412950` (sent **by the host** from `CGaming::FUN_0041e350`, the random pickup-spawner) | case 13 — `FUN_0041d2c0(cgaming, &xy) = FUN_0041d280(cgaming, kind=1, slot=100, &xy)` → **spawns the "special" world pickup** (kind=1, sprite `&DAT_004827f4[1] = 0x000100b3`) at world-slot 100. |
| `0x14` | `2` | `+0 u8 type=0x14` · `+1 u8 slot`                                                                                      | `CGame_NetSendPlayerEvent2_t14` @ `0x00412990` (sent by the picking player from `FUN_0041eba0`) | case 14 — `FUN_0041f030(cgaming, slot) = FUN_0041eba0(player, kind=1, world_slot=100)` → **player `slot` picked up the kind=1 world pickup**: destroys world-slot 100, increments `player[0x11C + 1]` (kind-1 inventory). |
| `0x15` | `3 + L` | `+0 u8 type=0x15` · `+1 u16 L` (= `stream->Length()`) · `+3 byte payload[L]`                                          | `CGame_NetSendCustomScriptPayload_t15` @ `0x00412df0` | case 15 — write `payload` into `cgame+0x1ac` (`CChainedStrm`), reset, then `CGame_OnNetMsg_t15_FireOnNetCustom` fires script export #9. |
| `0x16` | `3` | `+0 u8 type=0x16` · `+1 u8 category` (0..3) · `+2 u8 arg`                                                             | `CGame_NetSendTeamScoreEvent_t16` @ `0x00414550` (only when `isReplay==0`) | case 16 — calls back with `isReplay=1` to run the local effect without re-broadcasting. |
| `0x17` | `3` | `+0 u8 type=0x17` · `+1 u8 a` · `+2 u8 b`                                                                             | `CGame_NetSendOpposingEvent_t17` @ `0x004138b0` | case 17 — opposite-team counterpart of `0x16`. |
| `0x18` | `7` | `+0 u8 type=0x18` · `+1 u8 kind` · `+2 u8 world_slot` · `+3 S16 x` · `+5 S16 y`                                       | `CGame_NetSendPlaceObject_t18` @ `0x00412ac0` (sent **by the host** from `CGaming::FUN_0041e350`, allocates `world_slot ∈ [101..107]` via `FUN_00416810`) | case 18 — `FUN_0041d280(cgaming, kind, world_slot, &xy)` → **spawns world pickup of `kind ∈ [2..5]`** (sprite from `&DAT_004827f4[kind]`) at `world_slot`. This is the general "weapon-on-the-ground" / item-drop spawner. |
| `0x19` | `4` | `+0 u8 type=0x19` · `+1 u8 player` · `+2 u8 kind` · `+3 u8 world_slot`                                                | `CGame_NetSendTriByteEvent_t19` @ `0x00412a80` (sent by the picking player from `FUN_0041eba0`) | case 19 — `FUN_0041f010(cgaming, player, kind, world_slot) = FUN_0041eba0(player, kind, world_slot)` → **player picked up `kind` from `world_slot`**: destroys the world-object, increments `player[0x11C + kind]` inventory, plays pickup anim. |

`idFrom == 0` (DirectPlay system message) cases:

| `dwType` | Engine handler | Action |
|---:|---|---|
| `5`     | `CGame::FUN_00412800(this, msg[2])` | `DPSYS_DESTROYPLAYERORGROUP`. If host (`this[0x30] != 1`) and the dropped DPID matches a known slot, throw `CDSSimpleException(2000, 0x1b)` ("player vanished"). |
| `0x31`  | `CDSDirectXException(2, 0x88770136)` | Non-standard DPlay code — treated as fatal error. |
| `0x101` | `CDSSimpleException(2000, 0x1b)`     | Same outcome as case 5; likely an alternate path in this branch of DPlay. |

Everything else from the system pseudo-player is ignored.

### State byte (`CGame+0x30`)

The case bodies gate themselves on this:

| Value | State                              |
|---:|---|
| `0` | menu / not networking              |
| `1` | client, waiting for roster (msg 0) |
| `2` | host, roster locked                |
| `3` | … (level loading)                  |
| `6` | **in-game / playing**              |

All in-game (`0x0a..0x14`, `0x16..0x19`) message handlers no-op unless
`this[0x30] == 6`.

## Replication of player / projectile state

The engine ships only **events**, never per-tick snapshots. Conceptually:

* **Player position & animation** — `CBulanek::FUN_00420910` is called every
  time the local player changes animation state (idle → walk → run, jump,
  punch, …). It calls `FUN_00412b10`, emitting type **`0x0d`** with the
  player's slot, the new animation code, and the current `(x, y)` from
  `this+0x20`. Remote peers receive it, run `FUN_004209f0` which sets the
  remote `CBulanek`'s position and starts the matching animation. Between
  state changes there is no traffic — interpolation/extrapolation, if any,
  is local-only.
* **Shots / projectiles** — `FUN_00417e80(shooter, target, weapon, &xy,
  isLocal)` is called both by AI/local shoot code and the dispatcher.
  When `isLocal != 0` (the local player fired), it broadcasts type
  **`0x0f`** so peers can spawn the same projectile locally. Hit
  resolution (type **`0x10`**) and damage application (**`0x0c`**) are
  separate messages: the shooter's machine doesn't decide damage — the
  authoritative slot does (typically the host).
* **Death** — type **`0x11`** is emitted from `CBulanek::FUN_00412c40`,
  which is in turn called by the death state machine after damage drains
  the slot's HP. The dispatcher's case body re-runs the local death
  sequence (`CBulanek::FUN_00417570`).
* **Pickup spawn** (weapons / ammo / power-ups dropped into the world) —
  driven by the **host** from `CGaming::FUN_0041e350`, a periodic
  random-pickup spawner. It picks `kind ∈ [1..5]` (skipping kinds whose
  sprite ID is `0xFFFFFFFF` in the table at `0x004827f4`), allocates a
  synthetic "world slot" (100 reserved for kind 1, 101..107 for the
  rest), spawns the icon locally via `FUN_0041d240 → CBulanci::FUN_0041cfb0`,
  and broadcasts:
    * type **`0x13`** when `kind == 1` (the "special" pickup),
    * type **`0x18`** otherwise (general world pickup).
  Each peer's dispatcher re-runs the same spawn code, so the icon
  appears at the same coordinates on every machine.
* **Pickup collection** — when a local player walks over a world pickup,
  `FUN_0041eba0(player, kind, world_slot)` runs on every peer to (1)
  destroy the world-object at `world_slot`, (2) increment the player's
  per-kind inventory counter at `player + 0x11C + kind`, (3) play the
  pickup animation. The picking peer broadcasts:
    * type **`0x14`** for the kind-1 special pickup (just the slot),
    * type **`0x19`** for everything else (`player, kind, world_slot`).
  This is **the** "I picked up a weapon" event — it was hidden behind
  `PlayerEvent2` / `TriByteEvent` because the engine doesn't have a
  dedicated "pickup" message; the world-slot byte selects which dropped
  pickup is being collected.
* **"Use my current weapon" / mine drop / grenade throw** — type
  **`0x0e`** (`PlayerEvent1`) carries only the player slot, but the
  effect on each peer is determined by `FUN_00420650`, which switches on
  `*(byte*)(player + 0x64)` (the player's currently-held weapon kind):
  case 0 fires a projectile (and additionally emits `0x0f` ShotSpawn),
  case 1 throws a grenade, case 2 places a mine, etc. Because the
  current-weapon byte is mirrored on every peer (it changes only via
  the synced pickup / state messages), the implicit dispatch works
  without ever sending the weapon ID on `0x0e`. **There is no separate
  "place mine" packet — placing a mine is `0x0e` issued while the
  player's current-weapon byte is the mine kind.**
* **Round timing / banners** — types **`0x0b`, `0x12`, `0x16`, `0x17`** —
  server-driven; the timer (`this+0x20e`) is mirrored from msg `0x12`
  and adjusted by `0x16`.
* **Scripted custom traffic** — type **`0x15`** is the only payload the
  bytecode VM gets to peek at: bytes go into `cgame+0x1ac` (a
  `CChainedStrm`-shaped stream) and the engine fires script export #9
  (`OnNetCustom(handle)`). Levels can therefore define their own
  sub-protocol on top of this single multiplexed channel — see
  `script_lifecycle.md` for the `OnNetCustom` end of the contract and
  `unpacked/overlay/res_0000065859`'s `fn export#10`, which `StrmRead(_,
  1)` and switches on the first byte to demux level-internal events.

The complete absence of per-tick state-sync packets (no "snapshot" /
"delta" / "frame‑id" types in the table) means rollback / interpolation
**is not implemented** — peers diverge if they miss a guaranteed packet.
The `DPSEND_GUARANTEED` + `DPSESSION_DIRECTPLAYPROTOCOL` combination
makes this safe in practice: DirectPlay re-transmits indefinitely on
behalf of the engine.

## Host migration

`DPSESSION_MIGRATEHOST` is set when **hosting** (`FUN_0043af00`, flags
`0x2044`). DirectPlay 4 promotes another player to host on disconnect
and re-issues a `DPSYS_HOST` system message. The dispatcher's
`idFrom==0` branch does not currently handle that DPSYS id (none of 5 /
0x31 / 0x101 is `DPSYS_HOST = 0x000D`), so on host failure the new host
keeps the session technically open but the engine state machine doesn't
re-elect itself. Practical effect: host migration is wired at the
transport layer but not exercised at the engine layer — left as a
DirectPlay default.

## How to verify

* `FUN_00415290` decompile shows the full switch with all 26 cases.
* `FUN_0043ad20` decompile shows the `IDirectPlay4::Send` call at vtbl
  offset `0x68`, with `DPSEND_GUARANTEED = 1` hard-coded.
* `FUN_0043ab70` decompile shows the `IDirectPlay4::Receive` call at
  vtbl offset `0x64`.
* Cross-check the type-byte for every sender by running
  `_scratch_decode_brun.py`-style backward walks (see the inline Ghidra
  script comment in this folder; the 27 senders all set one of the 25
  unique type bytes listed above).
* The COM GUIDs at `.rdata` (`0x00486cf0..0x00486d70` and `0x00481a50`)
  are copied verbatim from `dplay.h` / `dplobby.h` (DirectX 7 SDK)
  except for the application GUID which is Bulánci's own.
