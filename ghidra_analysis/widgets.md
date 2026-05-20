# UI widget catalog

Per-widget reverse-engineering notes for the on-screen controls used
by the main menu and its sub-screens (`CMenu`, `CStartGame1`,
`CStartGame2`, `CExitDlg`, `CHistoryDlg`, `CMsgDialog`, `CHelpDlg`,
`CGameTypeDlg`, `CSetupDlg`, `CTcpIpConfig`, `CSessionList`, etc.).

The deep dive on the **three menu-specific widgets** (`CSwitch`,
`CRuch`, `CPoemScroller`) lives in `main_menu.md §9`; this file
collects every other reusable widget so a faithful re-implementation
of the dialog layer can pick a control out of the catalogue without
reading the binary.

## Conventions used in this file

- "Layout (size ~0xNN)" lists field offsets on the primary subobject.
- "Vtable slots" are the entries on the **primary** vftable (other
  vftables — chain / event-handler / anim — are listed only when
  they carry widget-specific overrides).
- All addresses are PE virtual addresses inside `bulanci.exe`.

Every widget below inherits from `CDSView` and `CDSChained`, so it
participates in the parent-child view tree, receives `OnTick` /
`OnDraw` from the per-frame pump, and posts commands to
`parent + 0x10` (the parent's `CDSEventHandler` subobject) via
`Scheduler_PostMessage(target, 0x100, cmd, …)`.

------------------------------------------------------------------------

## 1. `CButton` — dialog push button

The generic dialog button (Yes/No/OK/Cancel/Apply/Continue/...). Used
by every modal dialog. Distinct from `CSwitch` (the main-menu radio
buttons), which has its own animation track and "sibling" coupling
and lives in `main_menu.md §9.1`.

### Layout (size ~0x98)

| Offset | Type / size | Field |
|--------|-------------|-------|
| `+0x00` | u32 | primary vftable |
| `+0x04` | u32 | chain-subobject vftable |
| `+0x10` | u32 | event-handler-subobject vftable |
| `+0x18` | u32 | anim-subobject vftable |
| `+0x14` | u16 | CDSView flags (OR'd with `0x239` by `BuildAt`) |
| `+0x20` | s32 | bounding-box left  (= screen `x`) |
| `+0x24` | s32 | bounding-box top   (= screen `y`) |
| `+0x28` | s32 | bounding-box right (= `x` + sprite width) |
| `+0x2c` | s32 | bounding-box bottom (= `y` + sprite height) |
| `+0x30..+0x3c` | s32 ×4 | on-screen render rect (per-frame copy of `+0x20`) |
| `+0x44` | u8 | CDSView state flags (bit 0 = enabled, bit 1 = highlighted, bit 2 = ?, bit 3 = focused) |
| `+0x46` | u8 | widget flags (bit 0 = keyboard-focusable; OR'd with `1` by `BuildAt`) |
| `+0x4c` | `CDSChained*` | parent-view pointer (post-cmd target is `(+0x4c)+0x10`) |
| `+0x68` | `CDSImage*` | bitmap[0] – normal state |
| `+0x6c` | `CDSImage*` | bitmap[1] – hover state |
| `+0x70` | `CDSImage*` | bitmap[2] – pressed state |
| `+0x74` | `CDSFont*` | font (loaded from resource ID `0x100ae`) |
| `+0x78` | u32 | text-shape state (passed to `TextShaper`) |
| `+0x7c`..`+0x84` | mixed | text style / shape context |
| `+0x88` | u32 | current tint colour (`0xffffffff` = none) |
| `+0x8c` | u16 | **cmd word**, fired via `Scheduler_PostMessage(parent+0x10, 0x100, cmd, …)` |
| `+0x8e` | u8 | keyboard-focus-eligible flag |
| `+0x8f` | u8 | **pressed flag** (`1` while held down) |
| `+0x90` | `wchar_t*` | label text (default `PTR_DAT_004ae414` = empty string) |
| `+0x94` | s32 | **click audio slot** (`-1` = no sound; else passed to `PlayBankSample`) |

### Function map

| Address | Name | Purpose |
|---------|------|---------|
| `0x00404b10` | `CButton_ctor` | bare ctor (called by `BuildAt` after `CDSChained` parent) |
| `0x004087f0` | `CButton_BuildAt` | placement ctor (x, y, cmd, focusFlag, audioSlot, …, &resourceIds) |
| `0x00405600` | `CButton_dtor` | destructor body |
| `0x00406670` | `CButton_vDtor` | virtual destructor wrapper (`if (param & 1) free(this)`) |
| `0x00404ba0` | `CButton_GetTypeDescriptor` | returns `&DAT_004b3370` (RTTI) |
| `0x00404bb0` | `CButton_AdjustorThunk04_Dtor` | `dtor(this - 0x04)` |
| `0x00404bc0` | `CButton_AdjustorThunk10_Dtor` | `dtor(this - 0x10)` |
| `0x00404bd0` | `CButton_AdjustorThunk18_Dtor` | `dtor(this - 0x18)` |
| `0x004056b0` | `CButton_Render` | draws bitmap[state] + label via TextShaper |
| `0x004030b0` | `CButton_PlayClickSound` | `PlayBankSample(slot @ +0x94)` if not `-1` |
| `0x004030e0` | `CButton_OnMouseDown` | sets pressed-flag, plays sound, marks dirty |
| `0x00403f20` | `CButton_OnMouseUp` | clears pressed-flag, fires cmd if still hovering |
| `0x00403f90` | `CButton_OnKeyDown` | Enter = fire cmd; arrow keys = move focus via `CWindow::FocusSibling(parent, dir)` |
| `0x00404060` | `CButton_OnFocusReceived` | parent-driven focus callback (event id `6`) |

### Algorithms

- **Click sequence**: `OnMouseDown` → `pressed=1` + play sound (slot `+0x94`) + redraw → `OnMouseUp` → if mouse still over bounding box → `pressed=0` + redraw + `Scheduler_PostMessage(parent+0x10, 0x100, cmd@+0x8c, 0, 0)`.
- **Render**: `tint = FUN_00402f40(flags & 6)`; state index `i ∈ {0,1,2}` derived from `pressed` and the hover/disabled bits; blit `bitmap[i]` then `TextShaper_LayOutAndRender(label, rect, ctx, &shape_state@+0x78)`. Pressed state offsets text by +1 px in Y for the "depressed" look.
- **Keyboard nav**: arrow keys forward to `CWindow::FUN_0042ccd0(parent, prev/next)` which walks the sibling chain to focus the next focusable widget.

------------------------------------------------------------------------

## 2. `CStaticText` — text label

Inline-styled text label. Drawn once per frame, no input handling.
The dialog's "Are you sure?" prompt, every menu caption, and every
score-line title is a `CStaticText` instance.

### Layout (size ~0x98)

| Offset | Type / size | Field |
|--------|-------------|-------|
| `+0x00` | u32 | primary vftable |
| `+0x04` | u32 | chain-subobject vftable |
| `+0x10` | u32 | event-handler-subobject vftable |
| `+0x18` | u32 | anim-subobject vftable |
| `+0x20..+0x2c` | s32 ×4 | bounding box (l, t, r, b) |
| `+0x30..+0x3c` | s32 ×4 | on-screen rect (per-frame copy of `+0x20`) |
| `+0x44` | u8 | view flags (bit 2 = render in disabled tint) |
| `+0x68` | `CDSFont*` | font handle |
| `+0x6c` | u32 | TextShaper layout flag word (`0x10` default) |
| `+0x70` | u32 | user data / cmd target |
| `+0x74` | u32 | reserved |
| `+0x78` | `void*` | pointer to inline style block (= `this+0x84`) |
| `+0x7c` | u32 | tint colour (`0xffffffff` default, `0x909090` = grey, `0x303030` = highlight-grey) |
| `+0x80` | `wchar_t*` | label text (smart-pointer slot; default fallback `PTR_DAT_004ae414`) |
| `+0x84..+0x93` | u32 ×4 | inline default style block (4 ARGB colour ramps; seeded by `SetStyle` from `DAT_004ae418`) |
| `+0x94` | u8 | "force-default-colour" override (skip the auto-grey path) |

### Function map

| Address | Name | Purpose |
|---------|------|---------|
| `0x00404910` | `CStaticText_ctor` | bare ctor |
| `0x00406ba0` | `CStaticText_BuildAt` | placement ctor (w, h, text, evtTarget, …, shapeFlags, fontResId) |
| `0x00406a50` | `CStaticText_BuildAtAuto` | shorter ctor that auto-sizes the bounding box from the rendered text |
| `0x004052e0` | `CStaticText_dtor` | destructor body |
| `0x004064c0` | `CStaticText_vDtor` | virtual destructor wrapper |
| `0x00404970` | `CStaticText_GetTypeDescriptor` | returns `&DAT_004b3348` (RTTI) |
| `0x00404980` | `CStaticText_AdjustorThunk04_Dtor` | `dtor(this - 0x04)` |
| `0x00404990` | `CStaticText_AdjustorThunk10_Dtor` | `dtor(this - 0x10)` |
| `0x004049a0` | `CStaticText_AdjustorThunk18_Dtor` | `dtor(this - 0x18)` |
| `0x004054d0` | `CStaticText_Render` | tint + `TextShaper_LayOutAndRender` |
| `0x00403040` | `CStaticText_SetStyle` | `memcpy(this+0x84, src, n*4)` — replaces the inline style block |

### Algorithms

- **Render**:
  1. If `+0x94 == 0`, derive tint from view flags:
     `tint = (flags & 4) ? 0x303030 : 0x909090` (grey or
     highlight-grey for disabled labels); stash at `+0x7c`.
  2. Copy on-screen rect from `+0x30..+0x3c` to locals.
  3. Resolve text pointer from `+0x80`; fall back to
     `PTR_DAT_004ae414` (empty string) when NULL.
  4. `TextShaper_LayOutAndRender(text, rect, ctx, &shape_flags@+0x6c)`.
  5. Restore tint to `0xffffffff`.
- **Style block**: `+0x84..+0x93` is a 16-byte block of four packed
  ARGB colour ramps (default body / highlight body / shadow /
  outline). `SetStyle(src, 4)` is the in-place replace.

------------------------------------------------------------------------

## 3. `CIcon` — clickable image (no text)

Text-less variant of `CButton`. Same 3-bitmap state machine
(normal/hover/pressed), no font, no label, and a **shared**
click sound (audio slot `0x14`, hard-coded in
`CIcon_OnMouseUp`) instead of a per-instance slot. Used by
toolbar-style controls.

### Layout (size ~0x78)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0x18` | u32 ×4 | four vftables |
| `+0x14` | u16 | view flags (`|=0x38`) |
| `+0x20..+0x2c` | s32 ×4 | bounding box |
| `+0x30..+0x3c` | s32 ×4 | render rect |
| `+0x44` | u8 | state flags (bit 2 = disabled, bit 3 = hover) |
| `+0x68`/`+0x6c`/`+0x70` | `CDSImage*` ×3 | bitmap[normal/hover/pressed] |
| `+0x74` | u8 | pressed flag |
| `+0x76` | u16 | cmd word |

### Function map

| Address | Name |
|---------|------|
| `0x004049e0` | `CIcon_ctor` |
| `0x004086e0` | `CIcon_BuildAt` (x, y, cmd, &resIds[3]) |
| `0x00404aa0` | `CIcon_dtor` |
| `0x004065e0` | `CIcon_vDtor` |
| `0x00404a60` | `CIcon_GetTypeDescriptor` → `&DAT_004b3360` |
| `0x00404a70` | `CIcon_AdjustorThunk04_Dtor` |
| `0x00404a80` | `CIcon_AdjustorThunk10_Dtor` |
| `0x00404a90` | `CIcon_AdjustorThunk18_Dtor` |
| `0x00403e40` | `CIcon_Render` |
| `0x00403070` | `CIcon_OnMouseDown` |
| `0x00403ea0` | `CIcon_OnMouseUp` |
| `0x004030a0` | `CIcon_Invalidate` |

### Algorithms

- **Render**: select bitmap from `pressed > hover > normal`; if
  `flags & 4` (disabled) force `alpha = 0x80` and use `bitmap[0]`.
  Blit via `BlitDispatch`.
- **OnMouseUp**: if still inside hit-test, play **slot `0x14`** via
  `PlayBankSample`, then `Scheduler_PostMessage(parent+0x10, 0x100,
  cmd@+0x76, 0, 0)`.

------------------------------------------------------------------------

## 4. `CRadio` — horizontal radio group

A row of N labelled cells; exactly one is selected at a time. Used
by `CSetupDlg` for input-device pickers (Keyboard / Joystick /
Network), resolution choosers, audio routing, etc.

### Layout (size ~0xa8+)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0x18` | u32 ×4 | four vftables |
| `+0x14` | u16 | view flags (`|=0x78`) |
| `+0x20..+0x2c` | s32 ×4 | bounding box (cell-row spans this) |
| `+0x46` | u8 | widget flags (`|=1` focusable) |
| `+0x4c` | `CDSChained*` | parent |
| `+0x68` | u8 | **selected option index** (`0xff` = none) |
| `+0x69` | u8 | pressed-on-down option (`0xff` = none) |
| `+0x6a` | u8 | hovered option (`0xff` = none) |
| `+0x6b` | u8 | is-pressed flag |
| `+0x6c` | `wchar_t**` | array of N label pointers |
| `+0x70` | `u8*` | per-label state bits (bit 0 = grey-out) |
| `+0x74` | u8 | option count N |
| `+0x78` | u32 | cell padding / line height (default `8`) |
| `+0x7c` | `CDSFont*` | label font (resource `DAT_004ae404`) |
| `+0x80..+0x8b` | u32 ×3 | TextShaper layout state |
| `+0x8c` | `void*` | style block (default `&DAT_004ae418`) |
| `+0x90` | u32 | tint colour (`0xffffffff` = none) |
| `+0x94..+0xa4` | `CDSImage*` ×5 | cell bitmaps |

Cell bitmap states:
- `[0]` unselected, neutral
- `[1]` unselected, hover/pressed
- `[2]` selected, neutral
- `[3]` selected, hover/pressed
- `[4]` focused-by-keyboard

### Function map

| Address | Name |
|---------|------|
| `0x00407300` | `CRadio_ctor` |
| `0x00408540` | `CRadio_BuildAt` (x, y) |
| `0x00407500` | `CRadio_dtor` |
| `0x00407f30` | `CRadio_vDtor` |
| `0x004073a0` | `CRadio_GetTypeDescriptor` |
| `0x004073b0..0x004073d0` | adjustor-thunk dtors |
| `0x00403ac0` | `CRadio_Render` |
| `0x00402fb0` | `CRadio_OnMouseDown` |
| `0x00405240` | `CRadio_OnMouseUp` |
| `0x00403cc0` | `CRadio_SetSelected(idx)` |
| `0x00403d10` | `CRadio_SetSelectedFromPtr` |
| `0x00403020` | `CRadio_Invalidate` |
| `0x00403010` | `CRadio_IsRadioMarker` → `1` |

### Algorithms

- **Click**: `OnMouseDown` acquires keyboard focus and sets
  `+0x69 = +0x6a`, `+0x6b = 1`, plays audio slot `0x0e` (radio-cell
  click). `OnMouseUp` commits via `SetSelected(+0x69)` when the
  release lands on the same cell — which stores `+0x68 = idx` and
  posts `Scheduler_PostMessage(parent+0x10, 0x400, 0xce, this, 0)`.
- **Render**: per cell, the state index is computed from `+0x68`
  (selected), `+0x6a` (hover/click-target), and `+0x69` (focus
  highlight) and indexes `bitmap[0..4]`. Cell labels rendered with
  TextShaper using the per-instance font (`DAT_004ae404`).

------------------------------------------------------------------------

## 5. `CEdit` — single-line text input

The default text-entry control. Used by `CTcpIpConfig`
(server-IP / port), `CStartGame2` (player nick), `CSetupDlg`
(keybind capture), and the chat composer line in `CChatList`.

Distinct from `CStaticText`: `CEdit` accepts keyboard focus,
maintains a caret with a blink timer, and posts validated
character input back to its parent.

### Layout (size ~0xb8)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0x18` | u32 ×4 | view vftables |
| `+0x68` | object | `CDSUpdatedItem` subobject — caret-blink timer host |
| `+0x68 +0x00` | u32 | 5th vftable (the timer interface) |
| `+0x14` | u16 | view flags (`|=0x215`) |
| `+0x20..+0x2c` | s32 ×4 | bounding box (height = `0x17 + font_row`) |
| `+0x46` | u8 | widget flags (`|=0x41`: focusable + char-input) |
| `+0x80` | `CDSFont*` | font |
| `+0x84` | u32 | TextShaper flag word |
| `+0x88` | u32 | char-padding pixel count |
| `+0x90` | `void*` | style block (`&DAT_004ae418`) |
| `+0x94` | u32 | tint colour (`0xffffffff` = none) |
| `+0x98` | `wchar_t*` | text buffer (smart-pointer; length at `[buf-0xc]`) |
| `+0x9c` | u32 | **maxLen** (max char count accepted) |
| `+0xa0` | u8 | focused flag |
| `+0xa1` | u8 | select-all-on-focus flag |
| `+0xa4` | s32 | caret X pixel |
| `+0xa8` | s32 | caret Y pixel |
| `+0xac` | s32 | caret right pixel |
| `+0xb0` | s32 | caret bottom pixel |
| `+0xb4` | u32 | caret column (char index) |

### Function map

| Address | Name |
|---------|------|
| `0x00404d10` | `CEdit_ctor` |
| `0x00407760` | `CEdit_BuildAt` (x, y, parent, maxLen, …, flags, fontResId) |
| `0x00406050` | `CEdit_dtor` |
| `0x0040ba10` | `CEdit_vDtor` |
| `0x00404dc0` | `CEdit_GetTypeDescriptor` |
| `0x00404dd0..0x00406760` | adjustor-thunk dtors |
| `0x00406100` | `CEdit_Render` |
| `0x00404530` | `CEdit_MeasureCharWidth` |
| `0x00406eb0` | `CEdit_LayoutToCaret(pos)` |
| `0x00407040` | `CEdit_OnFocusEvent(evtId, target)` |
| `0x00406390` | `CEdit_InsertTextAt(pos, count, &src)` |
| `0x00407b40` | `CEdit_OnChar(ch)` |
| `0x00406230` | `CEdit_OnTimerTick` (caret blink) |
| `0x00407d20` | `CEdit_Invalidate` |
| `0x00406270` | `CEdit_SetText` |
| `0x00403150` | `CEdit_GetTextLength` |
| `0x00403170` | `CEdit_CopyText` |
| `0x00403190` | `CEdit_IsEditMarker` |

### Algorithms

- **Caret blink**: the constructor registers
  `Scheduler_RegisterEventSlot(this+0x68, slot=0, period=1000ms,
  evtId=7)`. While focused, the tick callback at `OnTimerTick`
  toggles a visibility bit; on un-focus it stops the timer.
- **Layout to caret**: `LayoutToCaret(newPos)` clamps `newPos` to
  `[0, len]`, walks the prefix char by char accumulating pixel
  widths via `MeasureCharWidth`, and stores the resulting
  pixel-X at `+0xa4`. The caret height is `font_row + 2 px`
  (from `font+0x564`).
- **Character input**: `OnChar(ch)` rejects controls (`ch < 0x20`),
  honours select-all semantics (`+0xa1`) by replacing the buffer
  on the first keystroke, otherwise inserts at the caret column;
  enforces `maxLen`; calls the parent validation callback
  `_Globals::FUN_004070c0(this, …)`; on accept, advances caret,
  resets blink to "on", and marks dirty.
- **Focus**: parent posts focus events to `OnFocusEvent`:
  evt `2` (focus-in) sets `+0xa0=1`, `+0xa1=1`, lays out to end,
  starts the blink timer; evt `3` (focus-out) clears `+0xa0`,
  stops the blink timer.
- **Render**: background fill at the disabled-tint colour
  (`FUN_00402f40(flags & 6)` → `FUN_00404e80` rect-fill); 5-px
  horizontal padding; render text via TextShaper; if focused,
  draw caret rect at `(+0xa4-1, +0xa8-1, +0xac-1, +0xb0-1)` in
  two-tone (`0xcecece` / `0xa0a0a0`) via `FUN_004365f0`.

------------------------------------------------------------------------

## 6. `CNumEdit` — integer input field

Subclass of `CEdit`, adds 12 bytes (size `0xc4` vs `CEdit`'s
`0xb8`) for `{min, max, parsedInt}` and overrides three vtable
slots to filter input and re-emit canonical numeric text on
focus-out. Used wherever the engine needs a bounded int from
the user: server port (`CTcpIpConfig`), level number (`CHistoryDlg`),
score caps, etc.

### Layout (extends `CEdit`)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0xb7` | — | `CEdit` base |
| `+0xb8` | s32 | min value (inclusive) |
| `+0xbc` | s32 | max value (inclusive) |
| `+0xc0` | s32 | parsed integer value |

### Function map

| Address | Name |
|---------|------|
| `0x004073e0` | `CNumEdit_Allocate` (factory, `malloc(0xc4)` + base ctor + vtable overrides) |
| `0x00407e00` | `CNumEdit_vDtor` |
| `0x00407de0` | `CNumEdit_AdjustorThunk04_Dtor` |
| `0x00406750` | `CNumEdit_GetTypeDescriptor` |
| `0x00406290` | `CNumEdit_OnTextChanged` (parses buffer → `+0xc0`) |
| `0x00407220` | `CNumEdit_OnFocusOut` (re-formats `+0xc0` → canonical text) |
| `0x00404550` | `CNumEdit_ValidateRange` (`min ≤ +0xc0 ≤ max`) |
| `0x004031a0` | `CNumEdit_GetValue` |

### Algorithms

- **Parse**: `_Globals::FUN_0042d2f0(&buf)` — a `wcstol`-style
  helper writes the integer to `+0xc0`. Failed parse leaves
  `+0xc0` untouched.
- **Re-emit**: on focus-out, the int at `+0xc0` is formatted
  back to canonical decimal (`"5"`, not `"0005"`) and pushed
  through the `_Globals::FUN_004070c0` validation pipe — so the
  display normalises after the user leaves the field.

------------------------------------------------------------------------

## 7. `CNumCounter` — animated 3-digit rolling counter

Slot-machine-style display. Three independent digit slots
animate by one step per tick between their `current` index
and `target` index, with a vertical scrolling blit between
glyphs. Used by the HUD/score displays and the level
round-end statistics.

### Layout (size ~0xa8)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0x18` | u32 ×4 | view vftables |
| `+0x68` | object | `CDSUpdatedItem` subobject (animation timer) |
| `+0x68+0x00` | u32 | 5th vftable (timer interface) |
| `+0x80` | `CDSImage*` | digit-strip atlas (vertical column of N glyphs) |
| `+0x84` | `CDSImage*` | cell-template bitmap (single digit-cell frame) |
| `+0x88` | u8 | direction (`0` = decrement, `1` = increment) |
| `+0x8c` | s32 | red-highlight flag (negative → draws red frame around each cell) |
| `+0x90..+0x98` | s32 ×3 | **current** digit indices, slots 0/1/2 |
| `+0x9c..+0xa4` | s32 ×3 | **target** digit indices, slots 0/1/2 |

### Function map

| Address | Name |
|---------|------|
| `0x00427340` | `CNumCounter_ctor` |
| `0x00427430` | `CNumCounter_dtor` |
| `0x00427af0` | `CNumCounter_vDtor` |
| `0x004273e0` | `CNumCounter_GetTypeDescriptor` |
| `0x004273f0..0x00427420` | adjustor-thunk dtors |
| `0x00427740` | `CNumCounter_Render` |
| `0x00427020` | `CNumCounter_UpdateDigit(idx, &changed)` |
| `0x004270a0` | `CNumCounter_OnTimerTick` (advances all 3 slots) |

### Algorithms

- **Tick** (`OnTimerTick`): calls `UpdateDigit` for slots 0, 1, 2.
  Each `UpdateDigit` advances the slot index by ±1 (per `+0x88`)
  with cyclic wrap (modulo the glyph count read from the digit
  strip at `bitmap+8`). If no digit changed, the timer is stopped
  via `_Globals::FUN_0042f300(this+0x68, 0)`.
- **Render** (per slot): blit the digit-strip bitmap with a
  vertical scroll-offset proportional to the in-between progress;
  when the scroll exceeds the cell height, blit the wrapped tail
  too (for the wrap edge). Overlay the cell-template frame.
  If `+0x8c < 0`, paint a red two-tone frame
  (`0xff0000` / `0xaf0000`) before the glyph. Slots are spaced
  by `cell_width + 2 px`.

------------------------------------------------------------------------

## 8. `CColorSwitch` — player-colour palette picker

Horizontal strip of 7-pixel-wide colour swatches used by
`CStartGame2` to pick the player bullet colour. Each cell shows
one entry from a supplied palette LUT; clicking a cell selects
it (no event fired — parent reads `+0x68` on commit).

### Layout (size ~0x74)

| Offset | Type | Field |
|--------|------|-------|
| `+0x68` | u8 | selected swatch index (`0xff` = none) |
| `+0x6c` | `void*` | palette-list pointer (`+0x36` base, `+0x37` count) |
| `+0x70` | `CDSImage*` | swatch atlas (resource `0x10014`) |

### Function map

| Address | Name |
|---------|------|
| `0x0040ebf0` | `CColorSwitch_ctor` |
| `0x004100d0` | `CColorSwitch_BuildAt` (x, y, &paletteList) |
| `0x0040b990` | `CColorSwitch_dtor` |
| `0x0040ec80` | `CColorSwitch_vDtor` |
| `0x0040b950` | `CColorSwitch_GetTypeDescriptor` |
| `0x0040b960..0x0040b980` | adjustor-thunk dtors |
| `0x0040ccd0` | `CColorSwitch_Render` |
| `0x0040ce00` | `CColorSwitch_OnMouseClick(mouseInfo)` |
| `0x0040aa10` | `CColorSwitch_OnEvent` |
| `0x0040aa40` | `CColorSwitch_SetSelected(idx)` |
| `0x0040aa60` | `CColorSwitch_SetSelectedFromPtr` |

### Algorithms

- **Click → index**: `idx = clamp((mouseX - this+0x30) / 7,
  0, count-1)` then `SetSelected(idx)`.
- **Render**: for each swatch `i`,
  `paletteLUT(baseIdx + i)` → atlas X-cell index; blit 7-px wide
  region from `atlas + paletteLUT*6, 0`. Selected swatch gets a
  white-on-grey frame (`FUN_00436530`).

------------------------------------------------------------------------

## 9. `CProgressBar` — bounded progress indicator

Static horizontal bar showing `value/total`. No input. Used for
loading screens and the `CSetupDlg` volume bars (alongside
`CVolume` thumb sliders).

### Layout (size ~0x70)

| Offset | Type | Field |
|--------|------|-------|
| `+0x68` | u32 | value |
| `+0x6c` | u32 | total |

### Function map

| Address | Name |
|---------|------|
| `0x0040df70` | `CProgressBar_ctor` |
| `0x0040bd50` | `CProgressBar_Allocate` (factory: `malloc(0x70)` + ctor) |
| `0x0040ad80` | `CProgressBar_GetTypeDescriptor` |
| `0x0040b1f0` | `CProgressBar_Render` |

### Algorithms

- **Render**: black-on-white outer frame
  (`FUN_00436530(rect, 0x000000, 0xffffff)`), then if `value != 0`
  draw a grey/white inner-padded fill of width
  `((rect.w - 2) * value) / total` via
  `FUN_004365f0(inner_rect, 0xa0a0a0, 0xffffff)`.

------------------------------------------------------------------------

## 10. `CVolume` — slider with audio preview

Subclass of `CScrollBar`, adds 4 bytes (size `0xd0` vs base
`~0xcc`) for an `audio handle`. Overrides three vtable slots:

- `OnMouseDown` plays audio slot `0x1e` (preview tone) on
  mouse-press; the global `DAT_004b3b94` "music-volume widget"
  comparison sets the loop bit so the music slider gets a
  sustained preview while SFX sliders get one-shots.
- `OnMouseUp` stops the preview and falls through to the base
  `CScrollBar::OnMouseUp` which commits the new value.
- `dtor` ensures the preview handle is released before the
  base scrollbar tears down.

### Layout (extends `CScrollBar`)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0xcb` | — | `CScrollBar` base |
| `+0xcc` | `CDSAudioPlayer*` | active preview handle (refcounted) |

### Function map

| Address | Name |
|---------|------|
| `0x0040f140` | `CVolume_Allocate` (factory) |
| `0x0040fa70` | `CVolume_dtor` |
| `0x0040fc00` | `CVolume_vDtor` |
| `0x0040e1b0` | `CVolume_BuildAt(rect, &param)` |
| `0x0040e230` | `CVolume_StopAudioPreview` |
| `0x0040fb00` | `CVolume_OnMouseDown` |
| `0x0040e270` | `CVolume_OnMouseUp` |
| `0x0040be30` | `CVolume_GetTypeDescriptor` |
| `0x0040be40..0x0040be70` | adjustor-thunk dtors |

------------------------------------------------------------------------

## 11. `CKeybShow` — keyboard-bind capture indicator

Lightweight display shown by `CSetupDlg` during keybind capture
("Press a key..." indicator). Only two C++-visible members:
the factory and the type-descriptor accessor — the actual
rendering reuses the host dialog's widgets, so this class is
effectively a "scanned-key holder" tagged with an RTTI marker
the input pipe recognises.

### Function map

| Address | Name |
|---------|------|
| `0x0040edb0` | `CKeybShow_Allocate` (`malloc(0x70)` + CDSChained ctor + 4 vftables) |
| `0x0040bbf0` | `CKeybShow_GetTypeDescriptor` → `&DAT_004b35b4` |

The vtable installed by Allocate routes the OnDraw / OnKeyDown
hooks through inherited slots; the captured scan code lands at
`+0x6c` (set in the ctor pre-zeroed) and is read by the parent
dialog when committing.

------------------------------------------------------------------------

## 12. `CScrollBar` — linear scroll bar / slider

The base scrollbar widget. Used standalone for tracks beside
`CListBox` / `CListViewer` / `CLevelList` / `CChatList`, and
subclassed by `CVolume` for the audio-preview sliders.
Orientation is auto-detected from the constructor rect's aspect.

### Visual model

9 state bitmaps in a 3×3 grid:
- 3 button bitmaps for the **top/left** arrow button (normal /
  pressed / disabled).
- 3 button bitmaps for the **thumb** (normal / pressed / disabled).
- 3 button bitmaps for the **bottom/right** arrow button.

The track between the two buttons is a repeat-blit of a fragment
of the chosen state bitmap; horizontal vs vertical orientation
chooses between `BlitHorizontalTrack` and `BlitVerticalTrack`.

### Layout (size ~0xcc)

| Offset | Type | Field |
|--------|------|-------|
| `+0x68` | object | `CDSUpdatedItem` (auto-repeat timer) |
| `+0x80..+0xa0` | `CDSImage*` ×9 | state bitmaps (see above) |
| `+0xa4` | u8 | packed state: `top:2 \| thumb:2 \| bot:2 \| _:2` (each: `0`=normal, `1`=pressed, `2`=disabled) |
| `+0xa8` | s32 | **min value** |
| `+0xac` | s32 | **max value** |
| `+0xb0` | s32 | **current value** |
| `+0xb4` | s32 | step (per arrow-button click) |
| `+0xb8` | s32 | page (per track-area click; `0x14` default) |
| `+0xbc` | u8 | orientation (`0` = vertical, `1` = horizontal) |
| `+0xbd` | u8 | is-dragging flag |
| `+0xc0` | s32 | drag mode (`0`=idle, `1`=topBtn, `2`=above-thumb, `3`=thumb, `4`=below-thumb, `5`=botBtn) |
| `+0xc4` | s32 | drag start offset (mouse-inside-thumb) |
| `+0xc8` | s32 | drag start mouse coord |

### Function map

| Address | Name |
|---------|------|
| `0x00404790` | `CScrollBar_ctor` |
| `0x00408370` | `CScrollBar_BuildAt(left, top, right, bottom, &bitmaps[9])` |
| `0x00404fe0` | `CScrollBar_dtor` |
| `0x00406430` | `CScrollBar_vDtor` |
| `0x00404840` | `CScrollBar_GetTypeDescriptor` |
| `0x00404850..0x00404880` | adjustor-thunk dtors |
| `0x00403530` | `CScrollBar_Render` |
| `0x00403860` | `CScrollBar_HitTest(mouseInfo)` → drag mode |
| `0x00402e80` | `CScrollBar_BlitVerticalTrack` |
| `0x00402ee0` | `CScrollBar_BlitHorizontalTrack` |
| `0x00407460` | `CScrollBar_OnMouseDown(mousePos, isDown)` |
| `0x004074c0` | `CScrollBar_OnMouseUp` |
| `0x00402f60` | `CScrollBar_OnMouseMove` |
| `0x00403980` | `CScrollBar_OnKeyDown` |
| `0x00403a80` | `CScrollBar_OnTimerTick` (press-and-hold auto-repeat) |
| `0x00406770` | `CScrollBar_Invalidate` |

### Algorithms

- **Thumb position** (linear interpolation):
  `range = trackLen − topBtnSz − botBtnSz − thumbSz;
   thumbStart = ((value − min) × range) / (max − min) + topBtnSz`.
- **Hit-test** maps the mouse onto one of 5 click zones
  (top-button, above-thumb, thumb, below-thumb, bottom-button).
- **Press-and-hold**: drag modes `1/2/4/5` trigger the registered
  `eventSlot 7` (period 0 → tick every frame) which the
  `OnTimerTick` consumes to scroll by `step` (for buttons) or
  `page` (for track clicks), with the standard "scroll fast
  while held" feel.

------------------------------------------------------------------------

## 13. `CScroller` → `CListViewer` → `CListBox` chain

A three-level inheritance chain that supplies every list-style
widget in the game:

```
CDSChained
  └─ CScroller       (viewport + 2 child CScrollBars)
       └─ CListViewer (column×row item grid + per-row callback)
             ├─ CListBox        (flat text rows; dropdown body)
             ├─ CLevelList      (level thumbnails grid)
             ├─ CChatList       (chat history, auto-scroll-to-bottom)
             └─ CSessionList    (network-session rows)
```

The "CListBoxItem" in the project headers refers to the *inline
row struct* the base list owns; it is not a separate C++ class.

### `CScroller` (size ~0x98)

| Offset | Type | Field |
|--------|------|-------|
| `+0x68` | object | `CDSUpdatedItem` (200 ms tick) |
| `+0x80` | s32 | horizontal scroll value (px) |
| `+0x84` | s32 | vertical scroll value (px) |
| `+0x88` | s32 | horizontal content size (px) |
| `+0x8c` | s32 | vertical content size (px) |
| `+0x90` | `CScrollBar*` | vertical scrollbar child (cmd `0xe`) |
| `+0x94` | `CScrollBar*` | horizontal scrollbar child (cmd `0xd`) |
| `+0x98` | s32 | last-render scroll snapshot |

Function map (excerpt):

| Address | Name |
|---------|------|
| `0x004033d0` | `CScroller_ctor` |
| `0x004089c0` | `CScroller_BuildAt(l, t, r, b)` |

The `BuildAt` allocates both scrollbars via `FUN_00447c42` +
`CScrollBar_BuildAt`, wires them as children of `this` via
`_Globals::FUN_0042d0b0`, and adjusts each scrollbar's track
length so they don't overlap each other's button regions.

### `CListViewer` (size ~0xcc, extends `CScroller`)

| Offset | Type | Field |
|--------|------|-------|
| `+0x9c` | u32 | flags (bit 0 = column-major iteration) |
| `+0xa0` | s32 | column width (px) |
| `+0xa4` | s32 | row height (px) |
| `+0xac` | `T**` | items array |
| `+0xb0` | s32 | capacity |
| `+0xb4` | s32 | item count |
| `+0xb8` | s32 | items per page |
| `+0xc8` | s32 | scroll step |

Function map (excerpt):

| Address | Name |
|---------|------|
| `0x00407f50` | `CListViewer_ctor` |
| `0x00408c00` | `CListViewer_BuildAt(l, t, r, b, colW, rowH, flags)` |
| `0x00408020` | `CListViewer_dtor` |
| `0x00408330` | `CListViewer_vDtor` |
| `0x00405b30` | `CListViewer_HitTestItem(mouseX, mouseY)` |
| `0x00405dd0` | `CListViewer_OnMouseClick` |
| `0x00405e10` | `CListViewer_AddItem` |
| `0x00407fd0` | `CListViewer_GetTypeDescriptor` |

`HitTestItem` formula: `col = mouseX/colW`, `row = mouseY/rowH`,
`idx = row*nPerLine + col` (or vice-versa when `+0x9c & 1`).
The per-row draw callback lives on the subclass vtable.

### `CListBox` (size `0xe4`, extends `CListViewer`)

The dropdown-body widget. Adds font + style state and a
trivial `RenderItem` that pads the row by 2 px and shapes the
inline-row struct's wchar_t* via TextShaper.

Item struct layout (allocated by callers):

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0x08` | — | refcount + chain pointers |
| `+0x0c` | u8 | flags (bit 0 = enabled/selectable) |
| `+0x10` | `wchar_t*` | row label |

| Address | Name |
|---------|------|
| `0x00408df0` | `CListBox_Allocate` (`malloc(0xe4)` + chain) |
| `0x00408cc0` | `CListBox_BuildAt(l, t, w, h, _, fontResId)` |
| `0x00408200` | `CListBox_dtor` |
| `0x00408350` | `CListBox_vDtor` |
| `0x00405fc0` | `CListBox_RenderItem(rect, item)` |
| `0x00408290` | `CListBox_GetTypeDescriptor` |

Subclass-added fields (`+0xcc..+0xe0`):

| Offset | Type | Field |
|--------|------|-------|
| `+0xcc` | `CDSFont*` | row label font |
| `+0xd0` | u32 | TextShaper flag word |
| `+0xdc` | `void*` | style block (`&DAT_004ae418`) |
| `+0xe0` | u32 | tint (`0xffffffff` = none, `0x858585` = disabled grey) |

`RenderItem` applies `0x858585` tint when `item.flags & 1 == 0`
(disabled), inflates left/right by 2 px, and renders the text
via `TextShaper_LayOutAndRender` with the per-row layout flags.

#### `CListBoxItem` (size ~0x14) — list-row payload class

The row payload is *not* an opaque struct: it has its own vftable
and is constructed via `CListBoxItem_ctor` (`0x0040b640`). Concrete
rows derive from it (`CSessionItem`, history rows, the lobby chat
rows). The base layout:

| Offset | Type | Field |
|--------|------|-------|
| `+0x00` | u32 | vftable |
| `+0x04..+0x08` | mixed | chain / refcount links |
| `+0x0c` | u32 | flags (bit 0 = enabled / selectable) |
| `+0x10` | `wchar_t*` | row label (smart-ptr, refcounted) |

Subclasses tack on data after `+0x14`:

- **`CSessionItem`** (lobby session row, see §14.3): `+0x14` host IP,
  `+0x18` port, `+0x1c` player count, `+0x20` session flags.

------------------------------------------------------------------------

## 14. `CListBox` subclasses (`CLevelList`, `CChatList`, `CSessionList`)

Three subclasses extend `CListBox` (or its base `CListViewer`) to
fill specialised roles in the menu/lobby flow.

### 14.1 `CLevelList` — level chooser (level-select panel)

| Address | Name | Notes |
|---------|------|-------|
| `0x0040b0c0` | `CLevelList_ctor` | Calls `CListBox_BuildAt` with hard-coded rect `(30, 270, 230, 440)` and font `0xbe`, then installs 5 vftables. |
| `0x0040b120` | `CLevelList_GetTypeDescriptor` | |
| `0x0040d490` | `CLevelList_RenderItem(rect, item)` | Identical to `CListBox_RenderItem` but with **5 px** left pad (vs 2 px) and the fallback text pointer is `PTR_DAT_004ae7a8` instead of the generic blank string. |

Rendered inside the right-hand "Start game" panel. Populated by
the level scanner that walks `CGame+0x66 / +0x6e` (see
`main_menu.md §8.6`) — one `CListBoxItem` per discovered level.

### 14.2 `CChatList` — multiplayer chat history strip

| Address | Name | Notes |
|---------|------|-------|
| `0x0040ff20` | `CChatList_Allocate` | `malloc(0xe8)` (4 bytes more than `CListBox`), invokes `CListViewer_ctor` directly (no `CListBox` allocation needed), zeroes `+0xcc / +0xd4 / +0xd8`. |
| `0x0040d010` | `CChatList_BuildAt(this, &rect[4], fontResId, ownerCallback)` | Calls `CListBox_BuildAt(rect[0..3], font=arg2, ?=8)`, stores `ownerCallback` at `+0xe4`, sets `+0x14 |= 0x200` (no keyboard focus — the chat composer `CChatEdit` owns it). |
| `0x0040d090` | `CChatList_GetTypeDescriptor` | |
| `0x0040d0b0` | `CChatList_OnEvent` | Forwards line-add events; ticks auto-scroll to bottom on new entries. |

Layout (extends `CListBox`, size `0xe8`):

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0xe3` | — | inherited `CListBox` |
| `+0xe4` | `CDSEventHandler*` | owner callback (target for line-added notifications) |

Lines are appended via the inherited `CListViewer_AddItem`. Auto-
stick-to-bottom is implemented by the inherited
`CScroller_OnTimerTick`: if the viewport was already at the
bottom before `AddItem`, the scroll offset advances to keep the
newest message visible.

### 14.3 `CSessionList` — multiplayer session-browser dialog

**Misnomer:** this is *not* a list widget, it's the whole
"Choose session" dialog (a `CWindow` subclass). The actual list
of sessions is one of this dialog's children — a `CListBox` —
and each row is a `CSessionItem` (subclass of `CListBoxItem`).

| Address | Name |
|---------|------|
| `0x0040c2d0` | `CSessionList_BuildDialog(this)` |
| `0x0040c530` | `CSessionList_GetTypeDescriptor` |
| `0x0040adc0` | `CSessionList_OnEvent` |

Dialog layout (extends `CWindow`, size ~0x80 + 4 child slots):

| Offset | Type | Field |
|--------|------|-------|
| `+0x20..+0x2c` | rect | `(0, 0, 0x14f=335, 0xf4=244)` initial; resized to width 300 at the end of `BuildDialog` |
| `+0x68` | u8 | `modalFlag = 1` |
| `+0x1c` | `CListBox*` | session rows (rect `(20, 50, 311, 230)`, font `0x118`) |
| `+0x1d` | `CStaticText*` | "Vyber relaci:" caption at `(20, 20)` from `g_apCDSStaticTextsSingleton[2][+0x80]` |
| `+0x1e` | `CButton*` | **Join**   at `(83, 200)`, cmd `0x8002`, audio slot `0x14`, label `[+0x84]` |
| `+0x1f` | `CButton*` | **Cancel** at `(173, 200)`, cmd `0x8003`, audio slot `0x14`, label `[+0x78]` |

The "Join" button is set as the default-focused child via
`_Globals::FUN_0042d080`. When clicked it posts cmd `0x8002` to
the parent (`CGame`), which extracts the selected `CSessionItem`
from `+0x1c` and connects to the session.

#### `CSessionItem` — one row of the session listbox

`CSessionItem` extends `CListBoxItem`:

| Address | Name |
|---------|------|
| `0x0040b770` | `CSessionItem_GetTypeDescriptor` |
| `0x0040e8b0` | `CSessionItem_Initialize(this, sessionInfo)` |

Layout (extends `CListBoxItem`, size ~0x24):

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0x13` | — | inherited `CListBoxItem` (vftable, chain, flags, label) |
| `+0x14` | u32 | host IP (network-order) |
| `+0x18` | u32 | port |
| `+0x1c` | u32 | current player count |
| `+0x20` | u32 | session flags (locked/passworded) |

`Initialize` converts the session name from MBCS to UTF-16 (via
`_AtlGetThreadACPThunk_004afbfc` + `FUN_00401290`), wraps it in a
refcounted string handle, calls `CListBoxItem_ctor` with that
handle, then stashes the four session-metadata fields.

------------------------------------------------------------------------

## 15. `CChatEdit` — single-line chat composer

The text-input box that sits below the lobby `CChatList`. Inherits
all behaviour from `CEdit` but overrides `OnChar` to intercept
**Enter** and forward the line to the network layer (with a small
debug-command sidecar).

| Address | Name |
|---------|------|
| `0x0040eca0` | `CChatEdit_Allocate` (`malloc(0xc4)` + `CEdit_ctor` + 5 vftables) |
| `0x0040ce50` | `CChatEdit_BuildAt(this, &rect[4], stateCtxOrNull, chatListPtr, gamePtr)` |
| `0x0040b9f0` | `CChatEdit_GetTypeDescriptor` |
| `0x0040cee0` | `CChatEdit_OnChar(scancode, modifiers)` |

Layout (size `0xc4`, extends `CEdit`):

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0xb7` | — | inherited `CEdit` |
| `+0xb8` | `void*` | optional host-state ptr (may be NULL); when set its `+0x68` byte is added to the channel id. |
| `+0xbc` | `CChatList*` | sibling chat list (target for `clearchat` wipe) |
| `+0xc0` | `CGame*` | game host (target for `NetSendChat`) |

`BuildAt` forwards to `CEdit_BuildAt(rect[0..3], maxLen=0x78=120
chars, flag=1, _, font=0x100af)`.

`OnChar` flow on **Enter** (`\r`, no modifiers):

```
if (buffer is empty) { fall through to CEdit base; }
else if (buffer == L"clearchat") {
    _Globals::FUN_00405c40(chatList);   // clear chatList model
    chatList->vt[0x24]();               // dirty/redraw
} else {
    channel = game->channel
            + (stateCtx ? stateCtx[0x68] : 0);
    CGame::NetSendChat(game, channel, buffer, NULL);
}
buffer = L"";                           // empty the CEdit
_Globals::FUN_004070c0(this, NULL, 1);  // post 'submitted' event
return 1;                               // handled
```

All other keystrokes forward to `CEdit_OnChar` (the inherited
single-line editor).

------------------------------------------------------------------------

## 16. `CWindow` — base dialog / modal container

Every dialog in the binary derives from `CWindow`: `CExitDlg`,
`CMsgDialog`, `CHelpDlg`, `CSetupDlg`, `CStartGame2`,
`CTcpIpConfig`, `CSessionList`, `CHistoryDlg`, `CGameTypeDlg`,
plus the implicit "menu screen" containers used by `CMenu`.

### Layout (size ~0x70 before subclass extension)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00` | u32 | primary vftable |
| `+0x04` | u32 | chain-subobject vftable |
| `+0x10` | u32 | event-handler vftable |
| `+0x18` | u32 | anim-subobject vftable |
| `+0x14` | u16 | view flags (OR'd with `0x77f` by `BuildAt`) |
| `+0x20..+0x2c` | s32 ×4 | bounding rect `(l, t, r, b)` |
| `+0x46` | u16 | widget flags (OR'd with `1` — keyboard-focusable container) |
| `+0x68` | u8 | modalFlag (0 = transparent, 1 = modal — captures input + dims background) |
| `+0x6c` | `CDSView*` | default-focus child (`NULL` unless set via `FUN_0042d080`) |

### Function map

| Address | Name |
|---------|------|
| `0x0040baa0` | `CWindow_ctor` |
| `0x00405560` | `CWindow_BuildAt(l, t, r, b, modalFlag)` |
| `0x004055c0` | `CWindow_Render` |
| `0x004064e0` | `CWindow_dtor` |
| `0x0040f120` | `CWindow_vDtor` |
| `0x004049b0` | `CWindow_GetTypeDescriptor` |
| `0x0042c660` | `CWindow_FindNextFocusable(this, direction)` |
| `0x0042ccd0` | `CWindow_FocusSibling(this, direction)` |

`FindNextFocusable` walks the chain of child views and returns
the first one whose `+0x14 & 0x40` (keyboard-focusable bit) is
set, starting from the current focus and stepping in `direction`
(0 = previous, 1 = next).

`FocusSibling` then calls `CDSView_AcquireKeyboardFocus` on the
result — this is the back-end of TAB-like navigation (CButton's
arrow-key handler at `0x00405820` / `CButton_OnKeyDown` invokes
it for left/right/up/down).

------------------------------------------------------------------------

## 17. `CPanel` — in-game player HUD strip

Despite the generic-sounding name, `CPanel` is the **in-game HUD
strip** that draws each player's avatar, name, and alive/dead
state at the top of the screen during a match. It is not a
general-purpose dialog container — that's `CWindow` (§16).

### Function map

| Address | Name |
|---------|------|
| `0x00427510` | `CPanel_ctor` (chain + 4 vftables + `eh_vector_constructor` × 2 sub-objects at `+0x70`, stride 4) |
| `0x004275d0` | `CPanel_dtor` |
| `0x00427c10` | `CPanel_vDtor` |
| `0x00427590` | `CPanel_GetTypeDescriptor` |
| `0x004275a0` | `CPanel_AdjustorThunk04_Dtor` |
| `0x004275b0` | `CPanel_AdjustorThunk10_Dtor` |
| `0x004275c0` | `CPanel_AdjustorThunk18_Dtor` |
| `0x00426ce0` | `CPanel_OnEvent(this, eventId)` — refreshes alive/dead overlay when `eventId == 0xe9` (player-state-changed broadcast). |
| `0x00427640` | `CPanel_Render(this, doFullRedraw)` — iterates `this[0x6c]` players and re-tints/relabels each row. |
| `0x00412550` | `CPanel_FindPlayerSlot(this, playerId)` — linear search of `this[0xda]` slots looking for the slot whose `+0x168 + i*0xd` equals `*players[playerId]`. |

### Layout (size > `0xf0`)

| Offset | Type | Field |
|--------|------|-------|
| `+0x00..+0x18` | u32 ×4 | vftables |
| `+0x68` | `CGameState*` | game host (passed to the per-player accessor helpers `_Globals::FUN_00412930 / FUN_00412910 / FUN_00412590`) |
| `+0x6c` | u8 | player count (loop bound for `Render`) |
| `+0x70 + i*?` | sub-object ×2 | constructed inline by `eh_vector_constructor_iterator_` (stride 4, count 2) — internal layout containers |
| `+0xbc + i*4` | `CBulPicture*` | per-player avatar bitmap (tinted by `CBulPicture_RecolorTo`) |
| `+0xcc + i*4` | `CDSView*` | per-player name `CStaticText` (relabeled each frame) |
| `+0xdc + i*4` | `CDSView*` | per-player "alive" overlay (shown/hidden in `OnEvent`) |
| `+0xec + i*4` | `CDSView*` | per-player "dead/x" overlay |
| `+0xda` | u8 | total slot count for `FindPlayerSlot` |

### Render algorithm

For each player slot `i` in `[0, this[0x6c])`:

1. `state = _Globals::FUN_00412930(host, i)` — fetch the player's
   colour / team id.
2. `CBulPicture_RecolorTo(this[+0xbc+i*4], state)` — re-tint the
   avatar bitmap to the player's colour.
3. `name = _Globals::FUN_00412910(host, i)` — UTF-16 player name.
4. `CEdit-like assign` (`FUN_0042d490`) on the name CStaticText.
5. `_Globals::FUN_00405370(this[+0xcc+i*4])` — schedule a
   redraw of that row.

### OnEvent (`0xe9 = player-state-changed`)

For each player slot, the helper `_Globals::FUN_00412590` returns
the current "alive?" boolean. If false, the alive overlay
(`+0xec + i*4`) and name banner (`+0xdc + i*4`) are removed from
the active draw chain (`FUN_0042d040`); if true they are
re-added (`FUN_0042c990`).
