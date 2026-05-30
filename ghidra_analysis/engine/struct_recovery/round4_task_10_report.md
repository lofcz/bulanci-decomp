# Round 4 — Task 10 report

## Task

| Field | Value |
|-------|--------|
| **id** | 10 |
| **title** | CHelpView classId 0x81d and wChainFlag46 vs CHistoryView |
| **types** | `CHelpView`, `CHistoryView`, `CDSChained`, `CHelpDlg` |
| **addresses** | `CHelpView_ctor@0x004218a0`, `CHistoryView_ctor@0x00422a70`, static init `@0x0047bf60`, `CHelpDlg_LoadHelpPage@0x00421c10`, `CHelpDlg_ctor@0x00421e40` |
| **supersedes** | R3 todo 10 (embed dedup — done); R4 closes R3 UNK follow-ups |

## Status

**DONE**

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| **classId 0x81d** (2077) | Static init `@0x0047bf60` | `HandleClassRegister(&DAT_004b38c0, 0x81d, CreateObject@0x00421ba0)` — `FUN_0047e380` atexit tail comment in `bulanci.ghidra.exe.c` |
| Factory | `CreateObject@0x00421ba0` | `OperatorNew(0x68)` → `CHelpView_ctor` → returns `&this->chain` |
| GetClassTable | `CHelpView_GetClassTable@0x00421900` | Returns `&DAT_004b38c0`; xref from registration |
| Sibling **CHelpScript 0x81c** | `CHelpDlg_ctor@0x00421e40` | Menu walk `*(entry+0xc) == 0x81c`; registry @ `0x0047bed0` |
| **CHistoryView classId 0x80f** | Static init `@0x0047c080` | Meta `&DAT_004b3928`; `CHistoryView_GetClassTable@0x00422ad0` |
| **wChainFlag46 bit 0** | `CHelpView_ctor@0x004218eb` | `OR word [ESI+0x46], 1` — same insn pattern as `CWindow_BuildAt@0x004055b3` |
| Focus consumer | `CDSView_AcquireKeyboardFocus@0x0042c8f9` | `TEST byte [ESI+0x46], 1` before delegating to parent focus chain |
| History omits bit 0 | `CHistoryView_ctor@0x00422a70` | Sets `wViewFlags \|= 0x67f` only; no `+0x46` OR |
| View-flag delta | ctors compare | Help `0x77f` vs history `0x67f` on `wViewFlags` (`+0x14`) — delta **0x100** (semantic bit TBD) |
| Bbox delta | ctors | Help bottom `0x1ea` (490); history `0x1fe` (510) — same width `0x212` |

## Ghidra deltas

- **`set_decompiler_comment`** @ `0x004218a0`, `0x004218eb`, `0x00421900`, `0x00421ba0` — classId registry + `wChainFlag46` bit-0 semantics vs `CHistoryView`.
- **`CDSChained.wChainFlag46`** — layout row documents bit 0 keyboard-focusable consumers.
- **`save_program bulanci.exe`**.

## Struct doc updates

| File | Change |
|------|--------|
| [CHelpView.md](./CHelpView.md) | Class registry table **0x81d**; closed `wChainFlag46` UNK; R4 apply note |
| [CHistoryView.md](./CHistoryView.md) | Corrected classId **0x80f** (was stale **0x801**); registry table |
| [CDSChained.md](./CDSChained.md) | `wChainFlag46` bit-0 evidence row |

## Remaining UNK

- COL display string for classIds **0x81d** / **0x80f** (ids and factories proven).
- Meaning of `wViewFlags` bit **0x100** set on help (`0x77f`) but not history (`0x67f`).
