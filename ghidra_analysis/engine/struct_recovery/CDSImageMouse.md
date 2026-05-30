# CDSImageMouse

## Status

**VERIFIED** — size `0x88` (136 bytes). Layout **PARTIAL** (header + `pCursorSprite` + embedded `CDSImage` `savedBackground` @ `+0x18` + dirty rect; inner `CDSImage` slot vector / MI edges still UNK per `CDSImage.md`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSImageMouse) == 0x88` | class factory @ `0x0042af00` | `PUSH 0x88` → `OperatorNewWithBadAlloc` (`0x00447c42`) before vtable/`CDSImage_InitDefaults` init |
| Ctor zeroes through `+0x84` | factory @ `0x0042af00` | `MOV [ESI+0x78..0x84], 0` after `CDSImage_InitDefaults` at `ESI+0x18` |
| `_free` on delete | `CDSImageMouse_vDtor` @ `0x0042af60` | `CDSImageMouse_dtor` then `MSVCRT::_free(this)` when `param_1&1` |
| Class id `0x35` (53) | `CDSImageMouse_StaticClassRegister` @ `0x0047c660` | `HandleClassRegister(&DAT_004b3b74, 0x35, &g_CDSMouse_classMeta, CDSImageMouse_CreateObject)`; sole factory push: `0x0047c660` |
| **Not** parent of `CGunMouse` | compare factories / ctors | `CGunMouse` uses `0x218` alloc, vtables `0x4837c0` / `0x4837ac` / `0x483794`, class id `0x7d1` @ `0x0047c0b0`; no call from `CGunMouse_ctor` to `0x0042af00` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVf_IDSChained` | factory @ `0x0042af00` `= 0x00486f7c` |
| `0x04` | 4 | `void *` | `pVf_IDSEventHandler` | factory @ `0x0042af00` `= 0x00486f68` |
| `0x08` | 4 | `uint` | `dwM_activeState` | factory @ `0x0042af00` `= 1` |
| `0x0c` | 4 | `CDSImage *` | `pCursorSprite` | factory `= 0`; `CDSImageMouse_dtor@0x0042acb0` `(**(vtable+8))()` release; `CDSImageMouse_Draw@0x0042b9a0` reads `pCursorSprite->m_width` / `m_height` (`+4`/`+8`), `BlitDispatch` source |
| `0x10` | 4 | `int` | `nM_hotspotX` | factory `= 0`; `CDSImageMouse_Draw@0x0042b9a0` subtracts from mouse coords |
| `0x14` | 4 | `int` | `nM_hotspotY` | factory `= 0`; `CDSImageMouse_Draw@0x0042b9a0` |
| `0x18` | `0x60` | `CDSImage` | `savedBackground` | factory calls `CDSImage_InitDefaults@0x00425580` on `this+0x18`; `CDSImageMouse_dtor@0x0042acb0` `CDSObject_dtor(this+0x18)`; Draw/Erase use `this+0x1c` as `CDSImage__Allocate` / `BlitDispatch` `this` (`&savedBackground.m_width`) |
| `0x78` | 4 | `int` | `nM_dirtyLeft` | factory `= 0`; `CDSImageMouse_Draw@0x0042b9a0` / `Erase@0x0042ba90` |
| `0x7c` | 4 | `int` | `nM_dirtyTop` | same |
| `0x80` | 4 | `int` | `nM_dirtyRight` | `CDSImageMouse_Draw@0x0042b9a0` |
| `0x84` | 4 | `int` | `nM_dirtyBottom` | `CDSImageMouse_Draw@0x0042b9a0` |

### `pCursorSprite` (`CDSImage *`, `+0x0c`)

Refcounted **resource-pool drawable** (held reference), not an embedded subobject.

| Use | func@addr | Detail |
|-----|-----------|--------|
| Dimensions | `CDSImageMouse_Draw@0x0042b9a0` | `[pCursorSprite+4]`, `[pCursorSprite+8]` → dirty rect and `CDSImage__Allocate` on `savedBackground` |
| Compositing | `CDSImageMouse_Draw@0x0042b9a0` | Second `CPoemScroller::BlitDispatch@0x004368d0` — `param_2 = pCursorSprite` (full `CDSImage` blit layout) |
| Release | `CDSImageMouse_dtor@0x0042acb0` | `(**(code **)(*pCursorSprite + 8))()` — primary vtable `Release` (IDSReferenced), not `CDSImage_ReleaseRefcount@+0x50` |
| Init | `CDSImageMouse_CreateObject@0x0042af00` | `pCursorSprite = NULL` |

Same **width@+4 / height@+8** header as [`CDSImage.md`](./CDSImage.md) and the engine resource-handle convention (`anim_runtime.md`: inner face at `resource_handle + 4`).

## Ghidra apply

**Slice 32 (2026-05-30):** `get_struct_layout` → **Size: 136** (`0x88`): `pCursorSprite` **`CDSImage *`** @ `+0x0c`, **`CDSImage savedBackground`** @ `+0x18`, dirty rect `+0x78..+0x84`. Prototypes: `CDSImageMouse_CreateObject` → `CDSImageMouse *`; `CDSImageMouse_Draw` / `Erase` use `CDSImageMouse *` (field-aware decompile: `pCursorSprite->nM_width`, `savedBackground.nM_height`, hotspots). Static init: **`CDSImageMouse_StaticClassRegister`** @ `0x0047c660` (class id `0x35`, factory `0x0042af00`; plate comment also tags `0x0047c676` atexit tail).

## Related types

| Type | Size | Factory | Class id | Notes |
|------|------|---------|----------|-------|
| **`CDSImageMouse`** | `0x88` | `CDSImageMouse_CreateObject` `0x0042af00` | `0x35` | Sprite + single backing `CDSImage`; `Draw`/`Erase` @ `0x0042b9a0` / `0x0042ba90` |
| **`CGunMouse`** | `0x218` | `CGunMouse_CreateObject` `0x00426500` | `0x7d1` | Menu sniper cursor; **does not** call `CDSImageMouse_CreateObject`. Live instance: `CBulanci_InitResourceBank@0x00402286` → `OperatorNew(0x218)` + `CGunMouse_ctor` |

**VERIFIED:** `CGunMouse` is a **separate** class (not a subclass or factory consumer of `CDSImageMouse`). Shared behavior is limited to the engine mouse idiom (dual interface vtables + `CDSImage_InitDefaults` on embedded images).

## UNK

- Inner `CDSImage` fields inside `savedBackground` — see `CDSImage.md` (slot vector / MI still PARTIAL).
- **Store site** for `pCursorSprite` at runtime (no direct `MOV [this+0xc]` in exe besides factory; likely class-id `0x35` / overlay deserialize).
- Concrete resource class (`CDSBmpImage` vs `BitmapSprite` pool wrapper) behind the `CDSImage *` — only `CDSImage` consumer layout is proven.

## Follow-up

- `savedBackground` draw path uses `this+0x1c` (`&m_width`) for `CDSImage__Allocate` / `GetColorPlane` / `BlitDispatch`, so cached dimensions appear at `savedBackground+0x08` / `+0x0c` (`m_height` / `m_format` slots) after allocate — intentional engine convention, not separate fields.
