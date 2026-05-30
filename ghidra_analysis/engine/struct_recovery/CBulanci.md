# CBulanci

## Status

**PARTIAL** — total size `0x4cc` (1228 bytes); **`CDSApp app`** @ `+0x00` (644 B); **`CGame game`** @ `+0x284` (584 B / `0x248`); **`pMainMenu`** @ `+0x280`. Agent todo **12** (2026-05-30): global **`CGame`** is **584 B**; embed uses same type; duplicate tail @ `+0x4b0` removed; **`CGame_embedded`** deleted.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CBulanci) == 0x4cc` | `0x00402ab1` | `CBulanci_CreateObject`: `PUSH 0x4cc` → `OperatorNewWithBadAlloc` |
| Embedded `CGame` at `+0x284` | `0x004026f0` | `CBulanci_ctor`: `CGame_ctor(&param_1->game)` |
| Embedded span `0x248` | (derived) | `0x4cc - 0x284 = 0x248`; `get_struct_layout` → `game` `CGame` @ 644 |
| Audio-bank tail inside `game` | `0x004026f0` | `(param_1->game).pAudioBankArray` … `bByte_4c8` (was duplicate `CBulanci+0x4b0..`) |
| `CBulanci_dtor` / `FUN_00402bc0` | `0x00402c8a`, `0x00402bf8` | `this+0x4b0` ≡ `&this->game.pAudioBankArray` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | `0x284` | `CDSApp` | `app` | `CBulanci_ctor@0x004026f0` → `CDSApp_ctor(this,…)` — `CDSApp.md` |
| `+0x280` | 4 | `CMenu *` | `pMainMenu` | ctor `= 0`; `CBulanci_OnEvent_MenuStateMachine@0x00402490` |
| `+0x284` | `0x248` | `CGame` | `game` | `CGame_ctor(&this->game)`; tail @ `game+0x22c..+0x247` |

## Ghidra apply

**Agent todo 12 (2026-05-30):**

- Verified **`CGame`** = **584 B** (`0x248`); deleted **`CGame_embedded`**.
- Recreated **`CBulanci`**: `CDSApp app` @ 0, `pMainMenu` @ 640, **`game` `CGame`** @ 644; removed duplicate `+0x4b0..+0x4c8` tail fields.
- `set_function_prototype` `CGame_ctor@0x00414a80`, `CPauseDlg_Build@0x00411df0`.
- `CBulanci_ctor` decompile: `(this->game).pAudioBankArray` init.
- `save_program bulanci.exe`.

**Agent todo 2 r2 (2026-05-30):** `CDSApp app` @ 0 (644 B); `get_struct_layout` shows `app` + `pMainMenu` @ 640 + `game` @ 644. CDSApp MI/view prefix `+0x08..+0x67` named in Ghidra (see `CDSApp.md`). Decompiler still emits `field_0xNN` on `CDSApp_ctor` / view helpers (ECX `this` typing limit).

**Agent todo 7 (2026-05-30):** `pMainMenu` `CMenu *` @ `0x280`.

**Agent todo 1 r3 (2026-05-30):** Verified asm `CBulanci+0x4b0..+0x4c8` aliases embedded `game+0x22c..+0x247` (no duplicate struct tail). Decompile `CBulanci_ctor` / `CBulanci_dtor` / `CBulanci_ReleaseAudioBank` use `(this->game).pAudioBankArray` … Comments @ `0x00402761`, `0x00402d5b`; `save_program bulanci.exe`.

| CBulanci abs | `game` rel | Field |
|--------------|------------|-------|
| `+0x4b0` | `+0x22c` | `pAudioBankArray` |
| `+0x4b8` | `+0x234` | `dwAudioBankCount` |
| `+0x4c0` | `+0x23c` | `pReleaseOnDestroy` |

## UNK

- `CDSApp` interior gaps (`CDSApp.md`, `app_shell.md`).
- `CGame` padded bands (`CGame.md`).

## Follow-up

- Name `CDSApp` / `CBulanci` prefix in decompile (`field_0xNN` until `CDSApp` fields fully wired).
- `CGaming` stack `game` vs embedded `g_pApp->game` (separate from embed sizing).
