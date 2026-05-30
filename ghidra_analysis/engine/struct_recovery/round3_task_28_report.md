# Round 3 — Task 28 report

## Task

| Field | Value |
|-------|-------|
| **id** | 28 |
| **title** | MCP ECX retype bundle: CDSCollection_InsertKeyed, CDSDirectSound_InitPrimary, CListBoxItem row ctors |
| **types** | CDSCollection, CDSDirectSound, CDSApp, CListBoxItem, CLevelScore |
| **priority** | high (blocker) |
| **acceptance** | `set_function_this_type` on each __thiscall site; decompile shows typed field access (`m_items`/`m_count`, COM/PCM tail, list-row fields) |

## Status

**DONE**

## Evidence

| Function | Address | `this` type (after) | Field-access proof |
|----------|---------|---------------------|-------------------|
| `CDSCollection_InsertKeyed` | `0x004312c0` | `CDSCollection *` | Decompile: `this->pM_items`, `this->nM_count`; `_Globals::CDSCollection_EnsureCapacity(this, this->nM_count + 1)`. Asm @ `0x00431303`..`0x00431321`: `[ESI+0x8]`/`[ESI+0xc]` = `m_items`/`m_count` |
| `CDSDirectSound_InitPrimary` | `0x0043cbc0` | `CDSDirectSound *` | Decompile: `pM_pDirectSound`, `pM_pPrimaryBuffer`, `wM_wPcmChannels`, `wM_wBitsPerSample`, `nM_nSamplesPerSec`, `dwM_reserved_40` |
| `CListBoxItem_ctor` | `0x0040b640` | `CListBoxItem *` | Decompile: `pVftable`, `dwAssocOrNext`, `dwRowFlags`, `dwLabelStringHandle`; `+0x04` not written |
| `CListBoxItem_ctorWithAssoc` | `0x0040b6d0` | `CListBoxItem *` | Decompile: `dwAssocOrNext = (dword)pAssoc`; same label/vftable path as default ctor |

### Call-site asm (InitPrimary / embed)

| Site | Address | Evidence |
|------|---------|----------|
| `CDSApp_OnCreate` → InitPrimary | `0x0042a2fa` | `LEA ECX,[ESI+0x200]` → `CALL 0x0043cbc0` (PCM args `2`, `0x5622`, `0x10` on stack) |
| `CDSApp_ctor` → `CDSDirectSound_ctor` | `0x0042b27d` | `LEA ECX,[ESI+0x200]` → `CALL 0x0043c7c0`; next init band @ `ESI+0x254` → **0x54** byte embed |

### Prior blocker vs fix

| API | Prior (R2 / todo 43) | R3 task 28 |
|-----|----------------------|------------|
| `set_function_prototype(CDSCollection *)` | ECX stayed `CDSUpdatedItem *`; header `CDSUpdatedItem::CDSCollection_InsertKeyed` | `set_function_this_type` moved fn into class `CDSCollection`; decompile header `CDSCollection::CDSCollection_InsertKeyed(CDSCollection *this, …)` |
| `set_function_prototype(CDSDirectSound *)` | Decompiler used `CDSApp *` / `field_0x200` | `CDSDirectSound *this` with `pM_pDirectSound` @ +0x38, etc. |
| List-row ctors | Raw `param_1` / dword indices | Named `CListBoxItem` fields |

**Residual:** `CDSDirectSound_InitPrimary` still casts `(CDSApp *)this` when calling `CDSApp_CreateSoundBuffer` (helper lives on app shell; embed offset +0x200). Layout offsets in body are correct on `CDSDirectSound *`.

## Ghidra deltas

- `set_function_this_type` @ `0x004312c0` → `CDSCollection *` (moved into class `CDSCollection`)
- `set_function_this_type` @ `0x0043cbc0` → `CDSDirectSound *` (moved into class `CDSDirectSound`)
- `set_function_this_type` @ `0x0040b640` → `CListBoxItem *` (confirmed in class `CListBoxItem`)
- `set_function_this_type` @ `0x0040b6d0` → `CListBoxItem *` (moved `CListBoxItem_ctorWithAssoc` into class `CListBoxItem`)
- `save_program bulanci.exe`

## Struct doc updates

- [CDSCollection.md](./CDSCollection.md) — follow-up (R3 task 28): ECX retype unblocks decompiler
- [CDSDirectSound.md](./CDSDirectSound.md) — follow-up (R3 task 28): InitPrimary `this` + field names
- [CListBoxItem.md](./CListBoxItem.md) — follow-up (R3 task 28): ctor decompile field names

## Remaining UNK

- `CDSDirectSound_InitPrimary`: `CDSApp_CreateSoundBuffer((CDSApp *)this, …)` — consider app-shell helper prototype with `CDSDirectSound *` embed base (cosmetic).
- `CDSCollection_InsertKeyed`: `FindKeyIndex` call still typed `(CDSStreamStorage *)this` in one branch (signature quirk; vector path uses typed `CDSCollection`).
