# `CDSAnim` / `CAnim` / `CBulAnim` — animation runtime

The "animation runtime" is the engine's **multi-track playlist player**: an
object that owns a list of *sequence resources* (e.g. the per-pose RLE
strips produced by `CDSFlxFile` and wrapped by `CBulPicture`/`BitmapSprite`)
and plays one of them at a time, advancing frames against the global engine
clock and pushing each decoded frame into the owning view via
`CPoemScroller::BlitDispatch`.

Three sibling concrete classes share the same shell, differing only in
where they live in the engine and in a few overridden slots:

| class      | size  | namespace                | role                                |
|------------|------:|--------------------------|-------------------------------------|
| `CDSAnim`  | 0xd0  | `Engine.DS.Graphics`     | base concrete player                |
| `CBulAnim` | 0xd4  | `Game.Entities`          | `: CDSAnim` + 1B team/variant tag   |
| `CAnim`    | 0xf0  | `Game.UI.Widget`         | sibling; widget-framed variant      |

Inheritance is **not** `CAnim : CDSAnim`. Ghidra's RTTI Type Descriptors
(`.?AV<name>@@` mangling) and the `RTTI_Base_Class_Descriptor_at_(offset,…)`
records (at `0x004a0a04 / 0x004a09cc / 0x004a2014`) show the actual ladder:

```
IDSInterface  (0x004ae22c)
  └─ IDSAnim  (0x004ae8ac)   — pure abstract, 5-slot vtable
       ├─ CDSAnim  (0x004ae84c)   IDSAnim subobject at +0x78
       │    └─ CBulAnim  (0x004ae834)   same layout, sizeof += 4
       └─ CAnim   (0x004af1c4)    IDSAnim subobject at +0x98
```

`CAnim` is the widget-side player (allocated 0xf0 bytes in
`CBulanci::FUN_00411010` for the per-team preview panels). `CBulAnim` is
the game-entity-side player (allocated 0xd4 bytes in `CMenu::FUN_004104f0`
for the start-game roster). `CDSAnim` itself is heap-allocated by
`CreateObject @ 0x004396c0` (factory with `_new(0xd0)`).

## Object layout (CDSAnim / CBulAnim)

```
offset  field
------  -----
+0x00   vftable_primary       (24 slots — "drawable view" face, inherits CDSView shape)
+0x04   vftable_chain         (6 slots  — IDSChained, head = CDSChain::vfn[0..3])
+0x10   vftable_event         (5 slots  — IDSEventHandler face)
+0x18   vftable_ref           (4 slots  — IDSReferenced face)
+0x14   uint16 flags          (`|= 0x200` after construct in CMenu::FUN_004104f0)
+0x20   int x                 (ctor arg 1)
+0x24   int y                 (ctor arg 2)
+0x44   uint8 visibility_mask (bit 0x40 = "drawable / on screen")
+0x68   vftable_update        (4 slots  — secondary IDSChained / IDSUpdated face)
+0x6c   vftable_anim          (5 slots  — **IDSAnim**, the animation event sink)
+0x70   uint32                (vbase identity slot — used as subscriber key)
+0x74   CDSAnimSequence*      currently-active sequence (released on swap)
+0x78   vftable_anim_sub      (4 slots  — thunks back to vtable_chain / dtor)
+0x7c   void* vbase           (CheckedVirtualBaseCast(outer, 0x12) = outer+0x68)
+0x80   CDSAnim* outer_this   (back-pointer to most-derived this)
+0x88   embedded CDSObject    (initialised by CDSObject::ConstructTrackManager
                               @ 0x00439c70 — the track-list manager itself)
+0xb8   CDSAnimSequence*      pre-armed/cached sequence (set before first SetTrack)
+0xd0   uint8 team_index      (CBulAnim only)
```

`CAnim` uses the same field semantics but shifts the inner sub-object
from `+0x78` down to `+0x98` (vbase at `+0x88`, IDSAnim vtable at
`+0x8c`, anim-sub vtable at `+0x98`, embedded `CDSObject` at `+0xa8`),
which adds the 0x20-byte widget chrome `CAnim` needs to slot into the
`Game.UI.Widget` framework.

## RTTI Complete Object Locators (vftable provenance)

The seven vftables for each concrete class are anchored by these COL
records:

| class    | offset | vftable    | COL        |
|----------|-------:|------------|------------|
| CDSAnim  | 0x00   | 0x004876fc | 0x004a48f8 |
| CDSAnim  | 0x04   | 0x004876e0 | 0x004a4880 |
| CDSAnim  | 0x10   | 0x004876c8 | 0x004a4894 |
| CDSAnim  | 0x18   | 0x004876b4 | 0x004a48a8 |
| CDSAnim  | 0x68   | 0x004876a0 | 0x004a48bc |
| CDSAnim  | 0x6c   | 0x00487688 | 0x004a48d0 |
| CDSAnim  | 0x78   | 0x00487674 | 0x004a48e4 |
| CBulAnim | 0x00   | 0x00480a14 | 0x004a07ac |
| CBulAnim | 0x04   | 0x004809f4 | 0x004a0a3c |
| CBulAnim | 0x10   | 0x004809dc | 0x004a0a50 |
| CBulAnim | 0x18   | 0x004809c8 | 0x004a0a64 |
| CBulAnim | 0x68   | 0x004809b4 | 0x004a0a78 |
| CBulAnim | 0x6c   | 0x0048099c | 0x004a0a8c |
| CBulAnim | 0x78   | 0x00480988 | 0x004a0aa0 |
| CAnim    | 0x00   | 0x0048207c | 0x004a1f8c |
| CAnim    | 0x04   | 0x00482060 | 0x004a204c |
| CAnim    | 0x10   | 0x00482048 | 0x004a2060 |
| CAnim    | 0x18   | 0x00482034 | 0x004a2074 |
| CAnim    | 0x88   | 0x00482020 | 0x004a2088 |
| CAnim    | 0x8c   | 0x00482008 | 0x004a209c |
| CAnim    | 0x98   | 0x00481ff4 | 0x004a20b0 |

## IDSAnim — the 5-slot interface at `+0x6c` (CDSAnim/CBulAnim) / `+0x8c` (CAnim)

This is the event-sink face: the active sequence calls **into** the
animation owner to report frame events, request a new track, etc. The
slots are filled identically across `CDSAnim` and `CBulAnim` because
both classes share the same most-derived implementation. `CAnim`
overrides slot 4 to a `CBitmap::FUN_00419280` body.

| slot | meaning                            | CDSAnim/CBulAnim impl | CAnim impl              |
|-----:|------------------------------------|-----------------------|-------------------------|
| 0    | dispatch N user events to vbase    | `0x00438f20`          | (shared)                |
| 1    | bind 2-tuple user-data             | `0x00438f60`          | (shared)                |
| 2    | `OnSequenceBound(seq)`             | `0x004391d0`          | (shared)                |
| 3    | `OnFinished`                       | `CBulanci::FUN_00438f80` (no-op) | (shared)     |
| 4    | pick a random sequence and play it | `0x004392a0`          | `CBitmap::FUN_00419280` |

Slot 0 (`FUN_00438f20`) is the per-frame callback: the sequence hands it
an array of `N` 16-byte user-event records, and the impl dispatches each
through the vbase's vfn at +0x24 (slot 9 of the IDSImage face). This is
the path scripts use to receive `OnBitmapEvt` notifications when an
animation frame is tagged.

## IDSAnim track manager — the embedded `CDSObject` at `+0x88`

The actual playlist state lives in this nested object. The lifecycle
functions all sit in the `0x004386f0..0x00439eb0` cluster (Ghidra
attributes a few to `CDSVideoPlayer` / `CDSAudioPlayer` because the
linker placed them in that range — they're shared track-manager code,
not video/audio-specific):

| addr        | role                                                         |
|-------------|--------------------------------------------------------------|
| `0x00436ef0` | `BroadcastFrameTimeHint(consumer, u16)` — fans out a u16 from FLX opcode 0x0C to the consumer's own subscriber list (slot 4). **NOT** a timing override — see "Frame timing and the clock" below. |
| `0x0042e790` | `CDSApp_UpdateClock` — ticks `g_dwElapsedMs = timeGetTime() - g_dwStartMs` once per frame from `CDSApp_FrameBody`. |
| `0x0042eb30` | `Scheduler_DispatchDueEvents` — walks the CDSObject scheduler's slot table; fires any whose `lastFire + delay <= g_dwElapsedMs` and resnaps `lastFire = lastFire + delay` (anti-drift). |
| `0x0042f210` | `Scheduler_RegisterEventSlot` — allocates a 0x1c-byte slot record `{lastFire, delay, flags, edge, owner, slotIdx, _pad}`. |
| `0x0042f290` | `Scheduler_SetEventLastFireMs` — write `slot[lastFire]`; `value == -1` substitutes `g_dwElapsedMs`. |
| `0x0042f2d0` | `Scheduler_SetEventDelayMs` — write `slot[delay]` (raw ms). |
| `0x00439720` | `TM_SetFrameDelayOverrideMs(trackMgr, ms)` — the **only** writer of `+0x44` in the whole binary; called from `CBulanek::FUN_0041e4b0` with the speed→ms formula. |
| `0x00438f90` | `TM_BindSequence` — addref, then `AddTrackSource`            |
| `0x00438fb0` | `TM_RewindCurrent`                                           |
| `0x00438fe0` | `TM_RenderFrame(bool)` — render-vs-reset entry from vtable 0/15 |
| `0x00439010` | `CBulAnim::SetPalette(uint32 pal[256])` (writes `_Globals::GetPaletteBuffer`) |
| `0x00439080` | `TM_TickBlit` — per-tick `CPoemScroller::BlitDispatch(...)`  |
| `0x00439100` | `TM_SetCurrentSequence(seq)` — release old, addref new, subscribe to seq+0x38, call `CBulanci::FUN_0042cbf0(outer, seq[1], seq[2])` to resize the owner |
| `0x004391e0` | `TM_SetTrack(int trackIdx, int playMode, uint32 *palette)`   |
| `0x00439270` | `CDSAnim::AnimInner_Init` — vbase resolve + back-pointers + auto-start |
| `0x00439350` | `CDSAnim::AnimInner_InitParam(seq, kind)`                    |
| `0x00439470` | `CDSAnim::DefaultCtor` (after `ODSImage::FUN_004228f0`)      |
| `0x00439510` | `CDSAnim::GetTypeInfo` (returns `&DAT_004b832c`)             |
| `0x00439560` | `CDSAnim::ParameterizedCtor(int x, int y, CDSAnimSequence *seq, int kind)` |
| `0x004396f0` | `CBulAnim::ScalarDeletingDtor(bool deleteFlag)`              |
| `0x00439840` | `TM_PauseAndStampClock` — sentinel time `0xffff`, cancel timer |
| `0x004398b0` | `TM_ClearTracks`                                             |
| `0x00439940` | `TM_Play(bool oneshot)` — arm timer for first frame          |
| `0x004399b0` | `TM_AdvanceFrame` — **the core frame stepper**               |
| `0x00439b40` | `BeginCurrentTrackPlayback` — Rewind via `seq->vfn[6]`, prime timer, then chain into `TM_AdvanceFrame` for frame 0 |
| `0x00439c70` | `CDSObject::ConstructTrackManager(this+0x88)`                |
| `0x00439d30` | `CDSVideoPlayer::TM_Destructor` — walks subscriber list, releases active seq |
| `0x00439e00` | `AddTrackSource(this, seq)`                                  |
| `0x00439eb0` | `SetCurrentTrack(this, idx, bool autoplay)`                  |

### Track-manager field layout (`this = outer+0x88`)

```
+0x00  CDSObject base / vftable
+0x04  timer/event source           (FUN_0042f290 / FUN_0042f2d0 / FUN_0042f330 schedule on this)
+0x14  IDSEventHandler subobject    (read by FUN_0042f1e0 to check paused-state bit 0)
+0x1c  Tracks array base            (each entry = 8 bytes: { u32 flags, CDSAnimSequence* seq })
+0x2c  int currentTrackIdx          (init -1 = none)
+0x30  void* renderTarget           (AddRef'd; passed as second arg to seq->vfn[7] each frame)
+0x34  byte playFlags               (set by FUN_00439940's bool arg)
+0x35  byte paused                  (gates FUN_004399b0)
+0x38  int currentFrameIdx          (wraps at seq->frameCount)
+0x40  void* notifyCookie            (= DAT_004b3bd8 — engine's global anim clock cookie)
+0x44  int frameDelayOverrideMs     (-1 = use sequence's numerator/denominator)
```

### Per-frame timing — `TM_AdvanceFrame @ 0x004399b0`

```c
entry      = this[0x1c] + this[0x2c]*8;          // current track entry
seq        = *(entry+4);
frame      = this[0x38];

if (!this[0x35]) {  // not paused
    if (this[0x44] == -1) {
        // sequence-default cadence — Bresenham keeps cumulative time exact
        delay = ((frame+1)*seq[0x10]) / seq[0x14]
              - ( frame   *seq[0x10]) / seq[0x14];
    } else {
        delay = this[0x44];                       // per-instance override
    }
    Scheduler_SetEventDelayMs(this+4, /*event=*/0, delay);
}

(*seq->vtbl[7])(entry, this[0x30]);              // decode frame -> renderTarget
this[0x38]++;
if (this[0x38] == seq[0x14]) this[0x38] = 0;     // wrap to frame 0
```

So a `CDSAnimSequence` exposes four runtime-consumed fields:

```
seq[1]    = frame width    (pushed into the owner via CBulanci::FUN_0042cbf0)
seq[2]    = frame height
seq[0x10] = duration numerator  (total clip duration in **ms**; see below)
seq[0x14] = frame count
```

Frame `f` is held for `(f+1)*num/den − f*num/den` ms — Bresenham-style
"stretch N frames over duration T ms exactly". A non-negative `+0x44`
overrides this with a fixed per-frame delay.

### Frame timing and the clock — unit pinned to milliseconds

Every delay value flowing through the track manager is real wall-clock
**milliseconds**. The chain:

1. `CDSApp_FrameBody @ 0x0042bda0` calls `CDSApp_UpdateClock @ 0x0042e790`
   once per app pump, which sets `g_dwElapsedMs = timeGetTime() -
   g_dwStartMs` (timeGetTime = Win32 multimedia timer, 1 ms resolution).
2. `CDSApp_PulseTasks` invokes `Scheduler_DispatchDueEvents @ 0x0042eb30`,
   which iterates over registered event slots. The predicate that fires
   a slot is `g_dwElapsedMs >= lastFire + delay`; on fire it resnaps
   `lastFire := lastFire + delay` (anti-jitter: drift never accumulates
   even when the app stutters past the deadline).
3. `TM_AdvanceFrame` arms the next animation tick via
   `Scheduler_SetEventDelayMs(this+4, 0, delay_ms)` — value is stored
   verbatim, so the unit at every layer is ms.

### `frameDelayOverrideMs` (+0x44) — set ONLY by construction-time speed

Cross-instruction sweep (every `MOV [reg+0x44], …` in the binary) plus
a callgraph walk confirms a single writer:
`TM_SetFrameDelayOverrideMs @ 0x00439720`, called once from
`CBulanek::FUN_0041e4b0` with the speed parameter `param_7`. The formula
is a fixed-point division pinned by two double constants:

```
g_kSpeedFormulaNumerator_47p25 @ 0x00482908  =  47.25
g_kSpeedFormulaNeutral_100p0   @ 0x00482910  = 100.0

delay_ms = round( 47.25 / (speed / 100.0) )
        =  round( 4725.0 / speed )
```

When `speed == 100` the formula is **skipped** entirely (the call to
`TM_SetFrameDelayOverrideMs` is gated behind `if (param_7 != 100)`),
leaving `+0x44 = -1` so the sequence's own cadence applies. Otherwise:

| speed | delay (ms) | effective fps |
|------:|-----------:|--------------:|
|  40   |  118       |  8.5  |
|  50   |   94       | 10.6  |
| 100   |  *(sequence default)* | — |
| 150   |   32       | 31.2  |
| 200   |   24       | 41.7  |
| 300   |   16       | 62.5  |

The range matches the gameplay tuning expected for a sidescroller (~10
fps slow, ~40 fps full speed).

### FLX opcode 0x0C is a side-channel notification, not a timing override

`CDSFlxFile::DecodeFrame`'s opcode 0x0C handler invokes
`BroadcastFrameTimeHint(consumer, u16)` (`@ 0x00436ef0`, previously
mislabelled `_Globals::NotifyFrameTime`). The function walks the
**consumer's own subscriber list** at `consumer+0x38..consumer+0x40`
and calls each subscriber's vftable slot 4 with `(consumer, u16)`.
There is no path from this fan-out that writes the track manager's
`+0x44`:

* No direct `MOV [_+0x44], _` instruction in the binary lives on the
  `BroadcastFrameTimeHint` → ... → track-manager callgraph (the only
  writer of `+0x44` is `TM_SetFrameDelayOverrideMs`, traced above).
* In the 130-sprite master pack, 31 sprites use opcode 0x0C; the raw
  values are mostly `0` and `1`. If interpreted as ms they'd be
  sub-perceptible — they cannot be a per-frame delay override.

The opcode's actual purpose is still open; plausible roles include
script-VM cadence hooks or a debug timing hint. Whatever it does, it
**does not** drive `TM_AdvanceFrame`'s schedule.

### Connection to `CBulPicture` / FLX decoder

`CBulPicture` is one concrete implementation of `CDSAnimSequence` — the
RLE/delta payload exposed via the editor's BitmapSprite (`ClassID 52`)
container (see `../formats/sprite_container.md`). Two virtual slots glue it to the
anim runtime:

* **`seq->vfn[7]`** (offset `+0x1c`) — called by `TM_AdvanceFrame` with
  `(track_entry, renderTarget)`. The CBulPicture body for this slot is
  the engine wrapper around `CDSFlxFile::DecodeFrame @ 0x00432c60`,
  which dispatches the per-opcode pixel decoders (`FUN_00432740` RLE,
  `FUN_00432780` delta, `FUN_004327e0` memcpy, `FUN_00432800` palette).
* **`seq->vfn[2]`** — Release. The runtime calls this on the old
  sequence whenever a track swap happens (`SetCurrentSequence`).

Resources expose a CBulPicture instance via the standard
`CheckedVirtualBaseCast(resource, DAT_004b8370)` cast — that's the
distinguished offset for the "anim sequence" interface. The unrelated
`CheckedVirtualBaseCast(resource, DAT_004b826c)` cast yields the static
`IDSBitmap` interface used by `CDSBitmap` for non-animated frames.

### Per-tick blit — `TM_TickBlit @ 0x00439080`

```c
seq   = this[0xc];           // active sequence (held by ref)
owner = this[8];             // outer's drawable face
if (seq && owner) {
    CPoemScroller::BlitDispatch(
        DAT_004b3b88 + 0x80,   // global engine singleton's blit context
        owner + 0x30,          // dest rect / surface descriptor
        seq, NULL, NULL);
}
```

`DAT_004b3b88` is the engine's top-level singleton (the same `g_Game`
that the script VM and `CDSDirectPlay::Receive` already use). The
`+0x80` offset selects its drawing-pipeline subobject. `BlitDispatch`
is reused by `CPoemScroller` for the between-round poem screen, which
explains why CDSAnim's update vtable (`+0x68`) inherits two
`CScroller`-named slot bodies (`CScroller::FUN_00404de0`,
`CScroller::FUN_004229a0`) — both classes implement the same
`IDSChained / IDSUpdated` pair.

## Construction templates

Both call-sites in `CMenu::FUN_004104f0` and `CBulanci::FUN_00411010`
follow the same pattern: alloc → base ctor → patch vtables → bind 4
sequences → pick a random starting frame.

```c
this_02 = _new(0xd4);                                 // size of CBulAnim
CDSAnim::FUN_00439560(this_02, x, y, /*seq=*/NULL, 0);

// rewrite the 7 vftable pointers to the CBulAnim variants:
((void**)this_02)[0x00/4] = CBulAnim::vftable_primary;
((void**)this_02)[0x04/4] = CBulAnim::vftable_chain;
((void**)this_02)[0x10/4] = CBulAnim::vftable_event;
((void**)this_02)[0x18/4] = CBulAnim::vftable_ref;
((void**)this_02)[0x68/4] = CBulAnim::vftable_update;
((void**)this_02)[0x6c/4] = CBulAnim::vftable_anim;
((void**)this_02)[0x78/4] = CBulAnim::vftable_anim_sub;
this_02[0xd0]               = team_index;
*(uint16*)(this_02 + 0x14) |= 0x200;                  // "drawable" bit

for (int j = 0; j < 4; j++) {
    seq = CheckedVirtualBaseCast(resource[j], DAT_004b8370);   // IDSAnim face
    FUN_00438f90(this_02 + 0x78, seq);                          // BindSequence
}

palette = {-1, -1, ... 256x};
FUN_00413c50(palette, team_index);                              // recolour table
FUN_004391e0(this_02 + 0x78,
             (rand() * 4) >> 15,    // random start track in [0,3]
             /*playMode=*/ 1,
             palette);
```

This shows that the four "tracks" of a team-slot `CBulAnim` are the
four facing/animation variants packaged into one BitmapSprite resource
(consistent with the 130 master-pack samples documented in
`../formats/sprite_container.md`).

## Cleanup chain

```
CBulAnim::~CBulAnim(bool del)               // FUN_004396f0
  FUN_0040ad20(this)                        // CBulAnim cleanup body
    CDSVideoPlayer::FUN_00439d30(this+0x88) // tear down TrackManager:
      for each subscriber: vfn[8](sub); vfn[2](sub)  // unsubscribe + release
      Release(active sequence)
      CDSObject::FUN_0042f140(this+0x8c)
      *(void**)this+0x88 = CDSObject::vftable
    FUN_0040a7d0(this)                      // CDSAnim shell teardown:
      FUN_004392e0(this+0x68)               //   teardown IDSUpdated + IDSAnim
      CDSView::FUN_0042cea0(this)           //   chain into base dtor
  if (del & 1) free(this)
```

That's also why `CBulAnim::~CBulAnim` shows up as vftable slot 1 in
*every* of the seven vftables (all the thunks pivot back to the same
most-derived `CBulAnim::FUN_004396f0`).

## Key allocation/use sites in the engine

| call site                        | what it builds                       |
|----------------------------------|--------------------------------------|
| `CreateObject @ 0x004396c0`      | bare `CDSAnim` factory               |
| `CBulAnim::FUN_0040eaa0`         | bare `CBulAnim` factory              |
| `CMenu::FUN_004104f0 + 0x239`    | per-team `CBulAnim` in StartGame     |
| `CBulanci::FUN_004112ac`         | per-player `CDSAnim` preview         |
| `CBulanci::FUN_00410729`         | secondary in-game `CDSAnim`          |
| `CBulanci::FUN_00411010` (`CAnim::FUN_00419940`) | per-player `CAnim` widget |

## Open questions

* On-disk source of `seq[0x10]` (sequence-default duration). The
  sequence pointer added to the track is `resource_handle + 4` (the
  second-face vftable of whatever `(*g_pApp[0x70])->vfn[4](resId, 0)`
  returns). For a `CBulPicture` resource, the byte that becomes
  `seq[0x10]` lands at `CBulPicture+0x14` — but that field is left
  uninitialized by `CBulPicture_Create @ 0x0040eb30` (the constructor
  writes vftables at `+0x00/+0x04/+0x10/+0x18` and a 256-dword color
  remap at `+0x6c..+0x46c`, nothing else). The duration must be written
  by the resource-pool wrapper that hands out the sequence, not by the
  bitmap layer. **Not present in any natural alignment of the 0x2c-byte
  BitmapSprite header.** Working hypothesis: a per-resource-type
  default (most plausibly 47.25 ms × frame count, mirroring the
  speed-formula degenerate case at speed=100).
* True semantic of FLX opcode 0x0C. The fan-out target (each
  consumer's subscriber list, slot 4) is mechanically obvious but no
  concrete subscriber for a CBulPicture has been mapped yet.
* The exact ordering of the 24-slot primary vtable (the IDSImage face).
  We have the slot-15 `Render` and slot-14 `Tick` mapped; the rest fall
  out once the shared `CDSView`/IDSImage layout is solved.
* `+0x70` and `+0x74` semantics in the IDSAnim subobject — currently
  inferred as `{vbase identity slot, current-sequence pointer}` from
  the way `FUN_00439100` uses them, but the field at `+0x70` is also
  read by `SetCurrentTrack`'s subscribe path as the cookie.
* Where the "user event" 16-byte record format used by IDSAnim slot 0
  comes from (i.e., what does each entry mean for the consumer at
  `vbase->vfn[9]`). Likely the same `{kind, x, y, frame}` shape the
  script's `OnBitmapEvt` already exposes.

## Cross-references

* Script-side handling of frame events: `script_lifecycle.md`
  (`OnBitmapEvt = exports[3]`, dispatched by the engine when an IDSAnim
  slot-0 record's kind matches the script's registered events).
* RLE / delta / palette payload format: `../formats/flx_file_format.md`.
* BitmapSprite container that holds the sequence headers: `../formats/sprite_container.md`.
