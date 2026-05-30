# Round 4 — Task 15 report

## Task

| Field | Value |
|-------|--------|
| **id** | 15 |
| **title** | Merge CMovieView InitTrackSequence unaff_ESI into this |
| **source** | blocker |
| **supersedes** | R3 todo 15 (audio bind path — **DONE** in R3) |
| **addresses** | `0x004237b0`, `0x004236a0`, `0x00423785`, `0x00422860` |
| **acceptance** | `CMovieView_InitTrackSequence` decompile uses **`CMovieView *this`** with **`this->`** field access (not `unaff_ESI`); Constructor tail no longer passes bogus `this_00` |

**Types:** `CMovieView`, `CDSBitmap`, `CDSAnimSequence`, `ODSImage`

## Status

**DONE** — `unaff_ESI` merged into register-local `this`; ctor tail call fixed.

## Before / after

| | `CMovieView_InitTrackSequence@0x004237b0` | `CMovieView_Constructor@0x004236a0` tail |
|---|---------------------------------------------|------------------------------------------|
| **Before (R4 entry)** | `void __thiscall …(CMovieView *this)` body uses **`unaff_ESI->`**; formal ECX `this` unused | `CMovieView_InitTrackSequence(this_00)` with `this_00 == 0` |
| **After** | `void __stdcall …(void)`; **`CMovieView *this`** (ESI); **`this->pAudioSequence`**, **`this->bitmapBase.trackImage`**, **`this->bitmapBase.wViewFlags`** | `CMovieView_InitTrackSequence()` (JMP tail; no bogus arg) |

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Object pointer in **ESI** | `CMovieView_InitTrackSequence@0x004237b0` | Disasm `[ESI+0x7c]`, `[ESI+0x68]`, `[ESI+0x14]` @ `0x004237b0`–`0x004237f5` |
| Ctor enters via **JMP** | `CMovieView_Constructor@0x004236a0` | `JMP 0x004237b0` @ `0x00423785` (shared stack / SEH) |
| Not ECX __thiscall `this` | `CMovieView_InitTrackSequence@0x004237b0` | Plate: ctor-tail bind; `set_function_this_type` left ECX auto-`this` unused |
| Stream mgr in **EDI** | `CMovieView_InitTrackSequence@0x004237b0` | `MOV ECX,EDI` / vfn `+0x10` @ `0x004237c1`–`0x004237c3` (`g_pApp+0x70` from ctor) |
| Ctor frame in **EBP** | `CMovieView_InitTrackSequence@0x004237b0` | `MOV ECX,[EBP+0x10]` @ `0x004237b6` (fallback `movieId`); SEH `FS:[0]` @ `0x00423800` |
| `OnEvent` already typed | `CMovieView::OnEvent@0x00422860` | `this->bitmapBase.trackImage`, `this->pAudioSequence` (unchanged) |

## Ghidra deltas

1. **`set_function_prototype`** `CMovieView_InitTrackSequence@0x004237b0` → **`void __stdcall CMovieView::CMovieView_InitTrackSequence(void)`** — drops erroneous ECX `this` so ESI can be named `this`.
2. **`rename_variable`** `unaff_ESI` → **`this`**; shadowing load **`this`** → **`pvLoad`**; **`unaff_EDI`** → **`pStreamMgr`**; **`unaff_EBP`** → **`ctorFrame`**.
3. **`set_function_this_type`** `CMovieView *` @ `0x004237b0` — class namespace retained (confirmed).
4. **`set_plate_comment`** @ `0x004237b0` — ESI/EDI/EBP ctor-tail roles.
5. **`force_decompile`** @ `0x004237b0`, `0x004236a0`
6. **`disassemble_function`** @ `0x004237b0` — ESI field offsets verified
7. **`save_program bulanci.exe`**

`set_variable_storage` **esi:4** for formal `this` is **not supported** via MCP (manual HighVariable API only); rename path sufficient.

## Struct doc updates

- [CMovieView.md](./CMovieView.md) — R4 task 15 apply block; UNK updated for InitTrackSequence `this` label

## Remaining UNK

- `pStreamMgr` / `ctorFrame` are named scratch locals (not typed `CDSStreamMgr *` / ctor struct) — shared ctor epilogue artifact
- `CMovieView_InitTrackSequence` remains a **separate function** with `RET 0x14` (not inlined into Constructor decompile body)
- Whether to model tail as `__usercall` with custom storage (low value; current decompile is readable)
