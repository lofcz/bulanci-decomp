# CEdit_OnKeyDown (`0x004078f0`)

Evidence: Ghidra MCP decompile/disasm, jump-table bytes @ `0x00407af8` / `0x00407b18`, callee decompiles, `CEdit_BuildAt` @ `0x00407760`, `CDSApp_DispatchInputEvent` @ `0x00429c00` family, `ghidra_analysis/engine/struct_recovery/CDSApp.md`, `ghidra_analysis/engine/vftable_methods.csv`.

## Summary

`CEdit_OnKeyDown` is the **vtable slot 22** (`+0x58`) key-down handler for `CEdit` and `CNumEdit`. It runs only on **key-down** (`param_2 == 0`) and only when **`g_pApp->keyLatchByVk[VK_CONTROL]`** (`g_pApp+0x111`) is zero (engine input-block latch). It dispatches on the low byte of `param_1` (virtual key) via a **dense jump table** for VK codes `0x08`–`0x2E` (subtract 8, max index `0x26`); all other keys fall through to **return 0**. Seven keys are handled: **Backspace** (delete char before caret + submit), **Delete** (delete char at caret + submit), **Home / End / Left / Right** (move caret via `CEdit_LayoutToCaret`), and **Insert** (toggle byte at `this+0xa1`, always treated as handled). On any path that sets the success flag (`BL` / return value non-zero), it sets `this+0xa0 = 1`, resets caret-blink scheduler slot 0 on the embedded `CDSUpdatedItem` at `this+0x68`, and calls **`CDSView_InvalidateRectClipped`** through **primary vtable `+0x24`** with `(NULL, 0)`.

## Signature

| Field | Value | Evidence |
|-------|--------|----------|
| Symbol | `CEdit_OnKeyDown` | `get_function_by_address@0x004078f0` |
| Address | `0x004078f0` – `0x00407af5` (size `0x206`) | Ghidra function bounds; `config/bulanci/mapping.csv` |
| Convention | **`__thiscall`** | Ghidra signature |
| Return | **`char`** — non-zero if event consumed / edit applied | `MOV AL,BL` @ `0x00407995`; early paths `XOR AL,AL` @ `0x00407ae0` |
| `this` | **`ECX`** (`CEdit *`) | Disasm `MOV ESI,ECX` @ `0x00407912` |
| `param_1` | **`Stack[0x4]`** — decompiler `undefined1 *`; used as **`(uint)param_1 & 0xff`** (VK) | `MOVZX` from `[ESP+0x1c]` @ `0x00407931`; switch on low byte |
| `param_2` | **`Stack[0x8]`** — key-up / repeat edge flag | `CMP byte ptr [ESP+0x20],0` @ `0x00407914`; same pattern as `CButton_OnKeyDown` (`bulanci.ghidra.exe.c` comment: `isKeyUp`) |

**Callers (verified xrefs):**

| From | Role |
|------|------|
| `0x0048001c`, `0x004800fc` | `CEdit` / `CNumEdit` vftable slot 22 data |
| `0x0040cff5` | `CChatEdit::CChatEdit_OnChar` — after Enter handling fails, delegates `CEdit_OnKeyDown(&this->base, …)` |

**Input dispatch (param semantics):** `CDSApp_DispatchInputEvent` @ `0x00429c00` calls `vtable+0x58` with `(event+8, event+9)` for key-down (`uVar2==1` @ `0x00429c65`). `param_1` = VK byte; `param_2` = non-zero on key-up (`0x00429c79` uses `+0x5c` for key-up). `CEdit_OnKeyDown` returns immediately when `param_2 != 0` @ `0x00407919`.

## Switch / dispatch table

**Mechanism** @ `0x00407931`–`0x0040794b`:

1. `EAX = (uint8)param_1`
2. `EAX -= 8` — only VK `0x08`…`0x2E` are in range
3. If `EAX > 0x26` → **default** @ `0x00407a5c` (`BL=0`, return 0)
4. `ECX = byte_table[EAX]` @ `0x00407b18`
5. `JMP dword ptr [ECX*4 + 0x00407af8]`

**Index byte table** @ `0x00407b18` (39 bytes, keys `VK = index + 8`):

| Index | VK | Hex | Jump-table index | Target |
|------:|-----|-----|------------------|--------|
| 0 | Back | `0x08` | 0 | `0x00407a63` |
| 1–26 | — | `0x09`–`0x22` | 7 | `0x00407a5c` (default) |
| 27 | End | `0x23` | 1 | `0x004079af` |
| 28 | Home | `0x24` | 2 | `0x004079ab` |
| 29 | Left | `0x25` | 3 | `0x00407952` |
| 30 | — | `0x26` | 7 | default |
| 31 | Right | `0x27` | 4 | `0x0040795e` |
| 32–36 | — | `0x28`–`0x2c` | 7 | default |
| 37 | Insert | `0x2d` | 5 | `0x004079c2` |
| 38 | Delete | `0x2e` | 6 | `0x004079d4` |

**Jump pointer table** @ `0x00407af8` (7 dwords):

| JT index | Address | Case VK |
|---------|---------|---------|
| 0 | `0x00407a63` | `0x08` Backspace |
| 1 | `0x004079af` | `0x23` End |
| 2 | `0x004079ab` | `0x24` Home |
| 3 | `0x00407952` | `0x25` Left |
| 4 | `0x0040795e` | `0x27` Right |
| 5 | `0x004079c2` | `0x2d` Insert |
| 6 | `0x004079d4` | `0x2e` Delete |
| 7 | `0x00407a5c` | default (via index byte `0x07`) |

**Handled case count:** **7** distinct VK branches + **1** shared default (`0x00407a5c`). Switch fully mapped (no unknown JT indices).

**Dispatch slot count:** **39** index bytes (`VK = index + 8`, range `0x08`–`0x2E`). **32** slots use jump-table index **7** (immediate default); **7** slots use indices **0–6** (unique handlers). Keys outside `0x08`–`0x2E` never enter the table (`JA` @ `0x0040793e` → default epilog).

### Plain-language summary

Ghidra’s decompiler shows many `switchD_0040794b::caseD_*` labels — **one per virtual-key case in the switch**, not one per distinct code path. Only **seven** VK codes run edit logic (Backspace, Delete, Home, End, Left, Right, Insert). The other **32** in-range keys (`0x09`–`0x22`, `0x26`, `0x28`–`0x2c`) all indirect-jump to the **same** default stub (`0x00407a5c`: set `BL=0`, return). Those VKs each get their own `caseD_<vk_hex>` label at that address for decompiler bookkeeping; behavior is identical to `CEdit_OnKeyDown_case_default`.

## Complete jump table (all VK indices)

Verified `read_memory` @ `0x00407b18` (39 bytes) and `0x00407af8` (8× dword pointers). Index formula: **`VK = table_index + 8`** from `ADD EAX,-0x8` @ `0x00407936` and `CMP EAX,0x26` @ `0x00407939`.

**Jump pointer table** @ `0x00407af8`:

| JT idx | Pointer @ `0x00407af8` | Target | Ghidra label(s) at target |
|-------:|------------------------|--------|---------------------------|
| 0 | `+0x00` | `0x00407a63` | `caseD_8`, `CEdit_OnKeyDown_case_back` |
| 1 | `+0x04` | `0x004079af` | `caseD_23`, `CEdit_OnKeyDown_case_end` |
| 2 | `+0x08` | `0x004079ab` | `caseD_24`, `CEdit_OnKeyDown_case_home` |
| 3 | `+0x0c` | `0x00407952` | `caseD_25`, `CEdit_OnKeyDown_case_left` |
| 4 | `+0x10` | `0x0040795e` | `caseD_27`, `CEdit_OnKeyDown_case_right` |
| 5 | `+0x14` | `0x004079c2` | `caseD_2d`, `CEdit_OnKeyDown_case_insert` |
| 6 | `+0x18` | `0x004079d4` | `caseD_2e`, `CEdit_OnKeyDown_case_delete` |
| 7 | `+0x1c` | `0x00407a5c` | `caseD_9`…`caseD_22`, `caseD_26`, `caseD_28`…`caseD_2c`, `CEdit_OnKeyDown_case_default` |

**Per-index byte table** @ `0x00407b18` (`byte[i]` → JT idx → target):

| Idx | VK | VK name | `byte[i]` | JT | Target | Handler |
|----:|----|---------|----------:|---:|--------|---------|
| 0 | `0x08` | VK_BACK | `0x00` | 0 | `0x00407a63` | **unique** — backspace delete |
| 1 | `0x09` | VK_TAB | `0x07` | 7 | `0x00407a5c` | default (no-op) |
| 2 | `0x0a` | — | `0x07` | 7 | `0x00407a5c` | default |
| 3 | `0x0b` | — | `0x07` | 7 | `0x00407a5c` | default |
| 4 | `0x0c` | VK_CLEAR | `0x07` | 7 | `0x00407a5c` | default |
| 5 | `0x0d` | VK_RETURN | `0x07` | 7 | `0x00407a5c` | default |
| 6 | `0x0e` | — | `0x07` | 7 | `0x00407a5c` | default |
| 7 | `0x0f` | — | `0x07` | 7 | `0x00407a5c` | default |
| 8 | `0x10` | VK_SHIFT | `0x07` | 7 | `0x00407a5c` | default |
| 9 | `0x11` | VK_CONTROL | `0x07` | 7 | `0x00407a5c` | default |
| 10 | `0x12` | VK_MENU | `0x07` | 7 | `0x00407a5c` | default |
| 11 | `0x13` | VK_PAUSE | `0x07` | 7 | `0x00407a5c` | default |
| 12 | `0x14` | VK_CAPITAL | `0x07` | 7 | `0x00407a5c` | default |
| 13 | `0x15` | VK_KANA | `0x07` | 7 | `0x00407a5c` | default |
| 14 | `0x16` | — | `0x07` | 7 | `0x00407a5c` | default |
| 15 | `0x17` | VK_JUNJA | `0x07` | 7 | `0x00407a5c` | default |
| 16 | `0x18` | VK_FINAL | `0x07` | 7 | `0x00407a5c` | default |
| 17 | `0x19` | VK_HANJA | `0x07` | 7 | `0x00407a5c` | default |
| 18 | `0x1a` | — | `0x07` | 7 | `0x00407a5c` | default |
| 19 | `0x1b` | VK_ESCAPE | `0x07` | 7 | `0x00407a5c` | default |
| 20 | `0x1c` | VK_CONVERT | `0x07` | 7 | `0x00407a5c` | default |
| 21 | `0x1d` | VK_NONCONVERT | `0x07` | 7 | `0x00407a5c` | default |
| 22 | `0x1e` | VK_ACCEPT | `0x07` | 7 | `0x00407a5c` | default |
| 23 | `0x1f` | VK_MODECHANGE | `0x07` | 7 | `0x00407a5c` | default |
| 24 | `0x20` | VK_SPACE | `0x07` | 7 | `0x00407a5c` | default |
| 25 | `0x21` | VK_PRIOR | `0x07` | 7 | `0x00407a5c` | default |
| 26 | `0x22` | VK_NEXT | `0x07` | 7 | `0x00407a5c` | default |
| 27 | `0x23` | VK_END | `0x01` | 1 | `0x004079af` | **unique** — caret to end |
| 28 | `0x24` | VK_HOME | `0x02` | 2 | `0x004079ab` | **unique** — caret to 0 |
| 29 | `0x25` | VK_LEFT | `0x03` | 3 | `0x00407952` | **unique** — caret −1 |
| 30 | `0x26` | VK_UP | `0x07` | 7 | `0x00407a5c` | default |
| 31 | `0x27` | VK_RIGHT | `0x04` | 4 | `0x0040795e` | **unique** — caret +1 |
| 32 | `0x28` | VK_DOWN | `0x07` | 7 | `0x00407a5c` | default |
| 33 | `0x29` | VK_SELECT | `0x07` | 7 | `0x00407a5c` | default |
| 34 | `0x2a` | VK_PRINT | `0x07` | 7 | `0x00407a5c` | default |
| 35 | `0x2b` | VK_EXECUTE | `0x07` | 7 | `0x00407a5c` | default |
| 36 | `0x2c` | VK_SNAPSHOT | `0x07` | 7 | `0x00407a5c` | default |
| 37 | `0x2d` | VK_INSERT | `0x05` | 5 | `0x004079c2` | **unique** — toggle `+0xa1` |
| 38 | `0x2e` | VK_DELETE | `0x06` | 6 | `0x004079d4` | **unique** — delete at caret |

Raw index bytes (hex): `00 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 07 01 02 03 07 04 07 07 07 07 07 05 06`.

**Counts:** **7** unique-handler indices · **32** default-only indices · **8** distinct target addresses (7 handlers + 1 default).

## Ghidra switch case labels explained

Ghidra names switch cases `caseD_<vk_hex>` where `<vk_hex>` is the **virtual-key code** (not the table index). Multiple `caseD_*` labels at **one address** means those VKs share the same jump target.

| Ghidra label | VK | VK name | Code address | Same as default? | Behavior vs default |
|--------------|----|---------|--------------|------------------|---------------------|
| `caseD_8` | `0x08` | VK_BACK | `0x00407a63` | no | Unique — wchar delete before caret + submit |
| `caseD_9` | `0x09` | VK_TAB | `0x00407a5c` | **yes** (alias) | Identical — return 0 |
| `caseD_a` | `0x0a` | — | `0x00407a5c` | **yes** | Identical |
| `caseD_b` | `0x0b` | — | `0x00407a5c` | **yes** | Identical |
| `caseD_c` | `0x0c` | VK_CLEAR | `0x00407a5c` | **yes** | Identical |
| `caseD_d` | `0x0d` | VK_RETURN | `0x00407a5c` | **yes** | Identical |
| `caseD_e` | `0x0e` | — | `0x00407a5c` | **yes** | Identical |
| `caseD_f` | `0x0f` | — | `0x00407a5c` | **yes** | Identical |
| `caseD_10` | `0x10` | VK_SHIFT | `0x00407a5c` | **yes** | Identical |
| `caseD_11` | `0x11` | VK_CONTROL | `0x00407a5c` | **yes** | Identical |
| `caseD_12` | `0x12` | VK_MENU | `0x00407a5c` | **yes** | Identical |
| `caseD_13` | `0x13` | VK_PAUSE | `0x00407a5c` | **yes** | Identical |
| `caseD_14` | `0x14` | VK_CAPITAL | `0x00407a5c` | **yes** | Identical |
| `caseD_15` | `0x15` | VK_KANA | `0x00407a5c` | **yes** | Identical |
| `caseD_16` | `0x16` | — | `0x00407a5c` | **yes** | Identical |
| `caseD_17` | `0x17` | VK_JUNJA | `0x00407a5c` | **yes** | Identical |
| `caseD_18` | `0x18` | VK_FINAL | `0x00407a5c` | **yes** | Identical |
| `caseD_19` | `0x19` | VK_HANJA | `0x00407a5c` | **yes** | Identical |
| `caseD_1a` | `0x1a` | — | `0x00407a5c` | **yes** | Identical |
| `caseD_1b` | `0x1b` | VK_ESCAPE | `0x00407a5c` | **yes** | Identical |
| `caseD_1c` | `0x1c` | VK_CONVERT | `0x00407a5c` | **yes** | Identical |
| `caseD_1d` | `0x1d` | VK_NONCONVERT | `0x00407a5c` | **yes** | Identical |
| `caseD_1e` | `0x1e` | VK_ACCEPT | `0x00407a5c` | **yes** | Identical |
| `caseD_1f` | `0x1f` | VK_MODECHANGE | `0x00407a5c` | **yes** | Identical |
| `caseD_20` | `0x20` | VK_SPACE | `0x00407a5c` | **yes** | Identical |
| `caseD_21` | `0x21` | VK_PRIOR | `0x00407a5c` | **yes** | Identical |
| `caseD_22` | `0x22` | VK_NEXT | `0x00407a5c` | **yes** | Identical |
| `caseD_23` | `0x23` | VK_END | `0x004079af` | no | Unique — caret to string end |
| `caseD_24` | `0x24` | VK_HOME | `0x004079ab` | no | Unique — caret to 0 |
| `caseD_25` | `0x25` | VK_LEFT | `0x00407952` | no | Unique — caret −1 |
| `caseD_26` | `0x26` | VK_UP | `0x00407a5c` | **yes** | Identical |
| `caseD_27` | `0x27` | VK_RIGHT | `0x0040795e` | no | Unique — caret +1 |
| `caseD_28` | `0x28` | VK_DOWN | `0x00407a5c` | **yes** | Identical |
| `caseD_29` | `0x29` | VK_SELECT | `0x00407a5c` | **yes** | Identical |
| `caseD_2a` | `0x2a` | VK_PRINT | `0x00407a5c` | **yes** | Identical |
| `caseD_2b` | `0x2b` | VK_EXECUTE | `0x00407a5c` | **yes** | Identical |
| `caseD_2c` | `0x2c` | VK_SNAPSHOT | `0x00407a5c` | **yes** | Identical |
| `caseD_2d` | `0x2d` | VK_INSERT | `0x004079c2` | no | Unique — toggle `this+0xa1` |
| `caseD_2e` | `0x2e` | VK_DELETE | `0x004079d4` | no | Unique — delete at caret |
| `CEdit_OnKeyDown_case_default` | *(JT 7 / out of range)* | — | `0x00407a5c` | — | Canonical default label |
| `CEdit_OnKeyDown_case_*` | *(see above)* | — | same as `caseD_*` | — | User-defined aliases on the 8 targets |

**User labels** (`CEdit_OnKeyDown_case_back`, `_end`, `_home`, `_left`, `_right`, `_insert`, `_delete`, `_default`) sit on the same addresses as the corresponding `caseD_*` entries; they do not add new code paths.

### Case summary table

| Case (VK) | Hex | Entry | Behavior | Return |
|-----------|-----|-------|----------|--------|
| *(guard)* | — | `0x00407914` | If `param_2 != 0` OR `*(g_pApp+0x111) != 0` → exit | `0` @ `0x00407ae0` |
| default | not in table / index 7 | `0x00407a5c` | No-op | `0` |
| Backspace | `0x08` | `0x00407a63` | See pseudocode | `0` or `1` |
| End | `0x23` | `0x004079af` | Caret → end of text | `CEdit_LayoutToCaret` result |
| Home | `0x24` | `0x004079ab` | Caret → `0` | same |
| Left | `0x25` | `0x00407952` | Caret → `caret-1` | same |
| Right | `0x27` | `0x0040795e` | Caret → `caret+1` | same |
| Insert | `0x2d` | `0x004079c2` | Toggle `this+0xa1`; skip layout | `1` (`BL` preset @ `0x0040793c`) |
| Delete | `0x2e` | `0x004079d4` | Delete at caret + submit | `0` or `1` |

## Per-case pseudocode

Registers: `this` in `ESI`, success in `BL` (init `1` @ `0x0040793c` only used for Insert fast-path).

### Guards @ `0x004078f0`–`0x0040792b`

```
if (param_2 != 0) return 0;                    // 0x00407914–0x00407919
if (g_pApp->keyLatchByVk[VK_CONTROL] != 0)     // byte at g_pApp+0x111, 0x0040791f–0x0040792b
    return 0;                                    // 0x00407ae0
BL = 1;
```

### Default @ `0x00407a5c`

```
BL = 0;
goto epilog;                                   // 0x00407995
```

### Home @ `0x004079ab` → shared caret path @ `0x00407968`

```
push 0;
goto LayoutToCaret_call;                       // 0x004079ad → 0x00407968
```

### End @ `0x004079af`

```
if (this->text_at_0x98 == NULL)
    push 0;
else
    push *(uint*)(this->text_at_0x98 - 0xc);   // wchar length, 0x004079b9
goto LayoutToCaret_call;
```

### Left @ `0x00407952`

```
push *(int*)(this+0xb4) - 1;                   // 0x00407952–0x0040795b
goto LayoutToCaret_call;
```

### Right @ `0x0040795e`

```
push *(int*)(this+0xb4) + 1;                   // 0x0040795e–0x00407967
```

### Shared navigation @ `0x00407968`–`0x0040796f`

```
LayoutToCaret_call:
BL = (char)CEdit_LayoutToCaret(this, pop_arg); // 0x0040796a, ECX=this
goto post_layout;                              // 0x00407971
```

`CEdit_LayoutToCaret` @ `0x00406eb0`: clamps index to `[0, len]`, updates `this+0xb4`, recomputes caret pixel fields `+0xa4`/`+0xac`; returns **0** if caret unchanged, **1** if moved (`0x00406f20`–`0x00406f28`).

### Insert @ `0x004079c2`–`0x004079d2`

```
this+0xa1 = (this+0xa1 == 0);                  // SETZ toggle, 0x004079c2–0x004079cc
goto success_side_effects;                     // 0x00407975 — skips TEST BL
// BL still 1 from entry
```

### Backspace @ `0x00407a63`–`0x00407adb`

```
if (*(int*)(this+0xb4) < 1) return 0;          // 0x00407a63–0x00407a6a
// SEH frame @ 0x00407a6c
CDsString_InitFromHandle(&tmp, &this->field_0x98);
CDsStringInsertWide(&tmp, caret-1, 1, NULL, 0); // delete 1 wchar before caret
CDsString_InitFromHandle(&stackCopy, &tmp);
if (!CEdit_SubmitText(this, widePtr, 0)) {     // third arg 0 @ 0x00407a9d
    BL = 0;
} else {
    CEdit_LayoutToCaret(this, caret-1);        // 0x00407ad2
    BL = 1;
}
release tmp if non-NULL;                       // LAB_00407a3f @ 0x00407a3f
goto post_layout;
```

### Delete @ `0x004079d4`–`0x00407a57`

```
len = text ? *(int*)(text-0xc) : 0;            // 0x004079d4–0x004079e9
if (len <= *(int*)(this+0xb4)) return 0;       // 0x004079eb–0x004079f1
CDsString_InitFromHandle(&tmp2, &this->field_0x98);
CDsStringInsertWide(&tmp2, caret, 1, NULL, 0); // delete at caret
CDsString_InitFromHandle(&stackCopy, &tmp2);
BL = (CEdit_SubmitText(this, widePtr, 1) != 0); // third arg 1 @ 0x00407a1b
release tmp2;                                  // LAB_00407a3f
goto post_layout;
```

`CDsStringInsertWide` @ `0x0042d5a0` with `insertLen=1`, `src=NULL`, `srcCount=0`: removes one code unit at `param_2` (MSVC string edit).

### Post-layout success @ `0x00407971`–`0x00407993`

```
post_layout:
if (BL == 0) goto epilog;                      // 0x00407971–0x00407973

success_side_effects:                          // 0x00407975
this+0xa0 = 1;                                 // 0x0040797c
Scheduler_SetEventLastFireMs(this+0x68, 0, -1); // 0x00407979–0x00407983
(*(this->vftable_primary+0x24))(this, 0, 0);   // CDSView_InvalidateRectClipped, 0x00407988–0x00407993

epilog:                                        // 0x00407995
return BL;
```

## Callees & data

| Address | Symbol | Role in `CEdit_OnKeyDown` |
|---------|--------|---------------------------|
| `0x004011b0` | `CDsString_InitFromHandle` | Stack copies of edit buffer before `CDsStringInsertWide` / `CEdit_SubmitText` |
| `0x0042d5a0` | `CDsStringInsertWide` | Backspace/Delete: remove 1 wchar at computed index |
| `0x004070c0` | `CEdit_SubmitText` | Validate layout, assign text, notify parent; `param_3` `0`=backspace, `1`=delete |
| `0x00406eb0` | `CEdit_LayoutToCaret` | Navigation + post-backspace caret sync |
| `0x0042d2d0` | `CDsStringReleaseHeader` | Release temp string headers (`ptr-0xc`) @ `0x00407a52` |
| `0x0042f290` | `Scheduler_SetEventLastFireMs` | Reset embed timer slot `0` on `this+0x68` |
| `0x0042ca30` | `CDSView_InvalidateRectClipped` | Primary vtable `+0x24`; `(this, NULL, 0)` |
| `0x004b3b88` | `g_pApp` | Global app; `+0x111` = `keyLatchByVk[VK_CONTROL]` input block |
| `0x00407af8` | `switchdataD_00407af8` | 7× dword jump targets |
| `0x00407b18` | *(byte index table)* | Maps `VK-8` → jump-table index |
| `0x00474bf0` | `LAB_00474bf0` | SEH handler node |

### `this` field offsets (usage in this function)

| Offset | Use in handler | Notes |
|--------|----------------|-------|
| `+0x00` | vtable → `+0x24` invalidate | Primary `CEdit` vftable @ `0x0047ffc4` |
| `+0x68` | `Scheduler_SetEventLastFireMs` target | `CDSUpdatedItem` embed (`CEdit_BuildAt`) |
| `+0x98` | `CDsString` text handle | Length at `handle-0xc` |
| `+0xa0` | Set to `1` on success | Also cleared in `CEdit_BuildAt` |
| `+0xa1` | Toggled on VK_INSERT | `CEdit_BuildAt` names “select-all-on-focus”; INSERT path toggles |
| `+0xb4` | Caret index (signed used as int) | Compared/clamped in Delete guard |

### `CEdit_SubmitText` (one level deeper, for reimplementation)

@ `0x004070c0`: compares against level-name table @ `this+0x98`; lays out with `TextShaper_LayOutAndRender`; on success assigns string, and if `param_2 != 0` calls `FUN_00407020` @ `0x00407020`, then `vtable+0x24(0)`, then optional `FUN_0042ecc0` notify. Failure returns 0 (backspace/delete leave text unchanged).

## Reimplementation notes

```c
// Verified control flow only — types per Ghidra.
char __thiscall CEdit_OnKeyDown(CEdit *this, uint8_t vk, char is_key_up) {
    if (is_key_up != 0) return 0;
    if (g_pApp->keyLatchByVk[VK_CONTROL] != 0) return 0;

    char handled = 0;
    switch (vk) {
    case 0x08: /* VK_BACK — 0x00407a63 */
        if ((int)this->caret_col < 1) return 0;
        handled = edit_delete_run(this, this->caret_col - 1, /*submit_flags*/ 0);
        if (handled) CEdit_LayoutToCaret(this, this->caret_col - 1);
        break;
    case 0x23: /* VK_END — 0x004079af */
        handled = (char)CEdit_LayoutToCaret(this, cds_string_len(this->text));
        break;
    case 0x24: /* VK_HOME — 0x004079ab */
        handled = (char)CEdit_LayoutToCaret(this, 0);
        break;
    case 0x25: /* VK_LEFT — 0x00407952 */
        handled = (char)CEdit_LayoutToCaret(this, this->caret_col - 1);
        break;
    case 0x27: /* VK_RIGHT — 0x0040795e */
        handled = (char)CEdit_LayoutToCaret(this, this->caret_col + 1);
        break;
    case 0x2D: /* VK_INSERT — 0x004079c2 */
        this->byte_a1 ^= 1;  // toggle 0/1
        handled = 1;
        break;
    case 0x2E: /* VK_DELETE — 0x004079d4 */
        if (cds_string_len(this->text) <= (int)this->caret_col) return 0;
        handled = edit_delete_run(this, this->caret_col, /*submit_flags*/ 1);
        break;
    default:
        return 0;  // 0x00407a5c
    }

    if (handled) {
        this->byte_a0 = 1;
        Scheduler_SetEventLastFireMs(&this->updated_item, 0, -1);
        this->vtable->InvalidateRectClipped(this, NULL, 0);
    }
    return handled;
}
```

`edit_delete_run` is the shared `CDsString_InitFromHandle` → `CDsStringInsertWide(..., pos, 1, NULL, 0)` → `CEdit_SubmitText` sequence @ `0x004079f3`–`0x00407a30` / `0x00407a72`–`0x00407ab2`.

**Repo stubs:** `include/bulanci/CEdit.h` (`CEdit_OnKeyDown`), `src/bulanci/CEdit.cpp` (`STUB_BODY`). **Shared impl:** `CNumEdit` vftable slot 22 → same function (`vftable_methods.csv` line 2783).

## Open questions

*(none for switch/case coverage — all 39 index bytes and 8 jump pointers verified `read_memory` 2026-05-30)*

- **Semantic names** for `this+0xa0` / `this+0xa1`: INSERT toggles `+0xa1` @ `0x004079cc`; `CEdit_BuildAt` comment calls `+0xa1` “select-all-on-focus” — behavior here is INSERT-toggle only (no additional Ghidra proof of overtype vs select-all in this function).

## Ghidra persistence

Applied to program `bulanci.exe` via Ghidra MCP (2026-05-30); verified with `read_memory`, `get_function_labels`, `disassemble_bytes`; `save_program`.

| Tool | Address | Text / name |
|------|---------|-------------|
| `set_plate_comment` | `0x004078f0` | Plate: vftable slot 22 key-down handler; guards; jump-table dispatch `0x00407b18`/`0x00407af8`; seven VK cases; success path `+0xa0`, scheduler `+0x68`, `vtable+0x24` invalidate. |
| `batch_set_comments` (plate) | `0x00407af8` | Plate: 8-dword JT[0..6]=BACK/END/HOME/LEFT/RIGHT/INSERT/DELETE handlers; JT[7]=default `0x00407a5c`. |
| `batch_set_comments` (plate) | `0x00407b18` | Plate: 39-byte map `(VK-8)→JT`; idx 0,27–29,31,37–38 unique; idx 1–26,30,32–36 → JT[7] default (32 VKs). See `CEdit_OnKeyDown.md`. |
| `set_disassembly_comment` | `0x00407af8` | EOL: jump ptr table summary. |
| `set_disassembly_comment` | `0x00407b18` | EOL: VK index bytes for `CEdit_OnKeyDown`. |
| `batch_create_labels` | `0x00407a5c` | `CEdit_OnKeyDown_case_default` (VK default / index 7) |
| `batch_create_labels` | `0x00407a63` | `CEdit_OnKeyDown_case_back` (VK `0x08`) |
| `batch_create_labels` | `0x004079af` | `CEdit_OnKeyDown_case_end` (VK `0x23`) |
| `batch_create_labels` | `0x004079ab` | `CEdit_OnKeyDown_case_home` (VK `0x24`) |
| `batch_create_labels` | `0x00407952` | `CEdit_OnKeyDown_case_left` (VK `0x25`) |
| `batch_create_labels` | `0x0040795e` | `CEdit_OnKeyDown_case_right` (VK `0x27`) |
| `batch_create_labels` | `0x004079c2` | `CEdit_OnKeyDown_case_insert` (VK `0x2d`) |
| `batch_create_labels` | `0x004079d4` | `CEdit_OnKeyDown_case_delete` (VK `0x2e`) |
| `set_decompiler_comment` | `0x00407914` | Guard: key-up (`param_2!=0`) → return 0. |
| `set_decompiler_comment` | `0x0040791f` | Guard: `g_pApp+0x111` Ctrl latch non-zero → return 0. |
| `set_decompiler_comment` | `0x00407938` | Switch dispatch: `(VK-8)`, byte table, indirect JMP. |
| `set_decompiler_comment` | `0x00407971` | `post_layout`: skip success if `BL==0`. |
| `set_decompiler_comment` | `0x00407975` | Success: `+0xa0`, scheduler, `vtable+0x24` invalidate. |
| `set_decompiler_comment` | `0x004079c2` | VK_INSERT: toggle `this+0xa1`. |
| `set_decompiler_comment` | `0x00407ae0` | Early exit return 0. |
| `set_disassembly_comment` | `0x0040796a` | EOL: `CEdit_LayoutToCaret` call. |
| `set_disassembly_comment` | `0x00407983` | EOL: `Scheduler_SetEventLastFireMs`. |
| `set_disassembly_comment` | `0x00407988` | EOL: `vtable+0x24` invalidate. |
| `set_plate_comment` | `0x00406eb0` | `CEdit_LayoutToCaret` role in this handler. |
| `set_plate_comment` | `0x004070c0` | `CEdit_SubmitText` backspace/delete `param_3` semantics. |
| `set_plate_comment` | `0x004011b0` | `CDsString_InitFromHandle` temp copy in edit paths. |
| `set_plate_comment` | `0x0042d5a0` | `CDsStringInsertWide` delete-one-wchar in Back/Delete. |
| `set_plate_comment` | `0x0042d2d0` | `CDsStringReleaseHeader` after temp edit. |
| `set_plate_comment` | `0x0042f290` | `Scheduler_SetEventLastFireMs` on success. |
| `set_plate_comment` | `0x0042ca30` | `CDSView_InvalidateRectClipped` via `vtable+0x24`. |

**Not applied (per markdown / tool limits):** `set_plate_comment` rejected at `0x00407af8`/`0x00407b18` (not a function address); data plates written via `batch_set_comments` instead. No renames (`g_pApp`, `switchdataD_00407af8`, byte table) — md documents usage/addresses only, not new global symbols.
