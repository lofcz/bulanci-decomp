# Static-text pool — `CDSStaticTexts` singleton

The shipped Czech build of `bulanci.exe` carries **one** populated
`CDSStaticTexts` object: a compile-time singleton baked into `.data`.
It feeds every dialog and error message the engine ever prints.

## Class shape

`CDSStaticTexts` is a 16-byte object. Its constructor at
`FUN_0042eeb0` zeroes the data slot and installs the two vftables:

```asm
0042eeb0  PUSH 0x10                          ; sizeof(CDSStaticTexts) = 16
0042eeb2  CALL operator_new
0042eec0  MOV [EAX+0x08], ECX                ; +0x8 = NULL  (pool ptr)
0042eec3  MOV [EAX+0x0c], ECX                ; +0xc = 0     (pool count)
0042eec6  MOV [EAX+0x00], 0x00486cc4         ; +0x0 = primary vftable
0042eecc  MOV [EAX+0x04], 0x00486cb0         ; +0x4 = IDSTexts vftable
0042eed3  RET
```

```c
struct CDSStaticTexts {
    void**           primary_vftable;     // 0x00486cc4 (3 slots)
    void**           ids_texts_vftable;   // 0x00486cb0 (4 slots, IDSTexts subobject)
    const wchar_t**  pool;                // pointer to wchar_t* table
    int32_t          count;               // number of pool entries
};
```

Both vftables are stub-shaped — every slot either returns the class
metadata blob or thunks to a `CDSObject` destructor. There is **no**
"GetText(int idx)" method anywhere. UI code reads the pool directly.

| vftable                       | addr         | slots | content (decompiled)                                                  |
|---|---|---|---|
| Primary (`CDSStaticTexts`)    | `0x00486cc4` | 3     | `[0]` returns `&class_meta`; `[1..2]` are `CDSObject` dtor helpers    |
| Secondary (`IDSTexts` @ +4)   | `0x00486cb0` | 4     | `[0]` returns `&class_meta`; `[1..3]` adjust-`this`-by-(-4) thunks    |

`IDSTexts` is therefore just a *tag* interface — the class registry
uses it to recognise CDSStaticTexts as "implements IDSTexts", but it
declares no methods of its own.

## RTTI

The relevant addresses (recoverable by searching `.rdata` for any
4-byte reference to each TypeDescriptor and walking the surrounding
BCD/CHD/COL graph):

| symbol                              | va           |
|---|---|
| `CDSStaticTexts` TypeDescriptor     | `0x004afbc4` |
| `IDSTexts` TypeDescriptor           | `0x004afbe4` |
| `CDSStaticTexts` ClassHierarchyDesc | `0x004a37b0` (4 base classes) |
| `CDSStaticTexts` BCD                | `0x004a37d4` (3 contained bases) |
| `IDSTexts` BCD inside CDSStaticTexts| `0x004a37f0` (offset +4 in derived) |

## Class-registry registration

`CDSStaticTexts` registers itself as class id **8** with the global
class registry at `g_pClassRegHead` (`0x004b7be4`). The static-init
block is at `0x0047c850..0x0047c8b9` and calls four runtime helpers:

```asm
0047c850  PUSH 0x8                ; classId of IDSTexts (interface tag)
0047c852  MOV  ECX, 0x004b7c58
0047c857  CALL FUN_0042e8f0       ; prepend to interface-registry list at DAT_004b7be0

0047c870  PUSH 0x0042eeb0         ; factory
0047c875  PUSH 0x004b7bfc         ; parent = *(&CDSObject_meta)
0047c87a  PUSH 0x8                ; classId
0047c87c  MOV  ECX, 0x004b7c60
0047c881  CALL HandleClassRegister (FUN_0042e910)

0047c8a0  PUSH 0x0042fd30         ; "this-to-IDSTexts adjuster" (returns this+4)
0047c8a5  PUSH 0x004b7c58         ; IDSTexts interface meta
0047c8aa  PUSH 0x004b7c60         ; CDSStaticTexts class meta
0047c8af  MOV  ECX, 0x004b7c74
0047c8b4  CALL FUN_0042e980       ; record "implements IDSTexts via thunk +4"
```

```c
struct ClassRegEntry {                       // size = 0x14 = 20 bytes
    ClassRegEntry* next;                     // +0x0   intrusive list
    void**         parent_meta_ptr;          // +0x4   ptr-to-ptr (often &CDSObject)
    int32_t        class_id;                 // +0x8
    void*          factory;                  // +0xc
    InterfaceEntry* implements_head;         // +0x10
};
```

The classID lookup function `InitializeByClassId` (`0x0042ef00`) is
the only public entry point that calls a registered factory:

```c
void* InitializeByClassId(int classId, ...) {
    if (classId < 0x1000 && g_apClassByIdTable[classId] != NULL)
        return ((ClassRegEntry*)g_apClassByIdTable[classId])->factory();
    // fall back to linear search of g_pClassRegHead by parent_meta_ptr
    ...
    if (no_match) MessageBoxW(NULL, ..., L"Text", 0);
    ...
}
```

`InitializeByClassId` is invoked from **three** sites only, all of
which read the `classId` from a *stream*:

| caller                              | role                                      |
|---|---|
| `CDSChain::FUN_0042fb70`            | per-element deserialiser for `CDSChain`   |
| `CDSCollection::FUN_0042fd40`       | single-object deserialiser for `CDSCollection` |
| `CDSStreamStorage::FUN_00434760`    | top-level resource-stream loader          |

No payload in the shipped overlay pack (`unpacked/overlay/`) carries
class id 8 — every resource there is in the 2000-2078 range. So the
factory at `0x0042eeb0` is **registered but never actually called by
data**. The only live `CDSStaticTexts` is the static instance below.

## The static instance

```
0x004afbb4   00486cc4    primary vftable
0x004afbb8   00486cb0    IDSTexts vftable
0x004afbbc   004af9b8    -> wchar_t* pool table
0x004afbc0   0000007f    pool count = 127
```

`PTR_PTR_004afbbc` (Ghidra's label for `0x004afbbc`) is the **only**
slot in the singleton that the rest of the engine ever reads.

## Pool layout

`0x004af9b8` is a `wchar_t*[127]` table. Each entry is either:

* NULL (e.g. `pool[0]` and `pool[98]` both point at `0x0047f668`, which
  starts with a NUL — effectively an empty string), or
* a pointer to a NUL-terminated UTF-16LE C string sitting in `.rdata`.

Dumped contents:
[`static_texts.json`](static_texts.json) (regenerate with
[`static_texts.py`](static_texts.py)).

Highlights:

| range     | language | role                                                          |
|---|---|---|
| `[1..29]` | Czech    | Engine error messages (`"Nelze alokovat paměť…"`, `"Neplatný formát souboru."`, `"DirectDraw"/"DirectSound"/"DirectPlay"`, …) |
| `[30..77]`| Czech    | Main UI strings (menu labels, dialog headers, control hints, key names) |
| `[78..86]`| Czech    | About / credits / pause overlay                               |
| `[87..97]`| Czech    | Score-screen, pause, "version 1.80", connection errors        |
| `[99..111]`| Czech   | Game-setup dialog (host/join, mode picker, "Koupit", weapon mode) |
| `[112..126]`| English | Trial / registration / "Buy"-button overlay (used only by the international "iwannaplay.com" build path) |

Examples of paired Czech/English entries (10 indices apart):

| Czech idx → text             | English idx → text          |
|---|---|
| 107 → `"1 hráč (trénink)"`   | 117 → `"1 player - training"`|
| 108 → `"Mód zbraní:"`        | 118 → `"Weapon mode:"`       |
| 109 → `"pouze brokovnice"`   | 119 → `"shotgun only"`       |
| 110 → `"všechny zbraně"`     | 120 → `"all weapons"`        |
| 111 → `"Koupit"`             | 121 → `"Buy"`                |

Two strings are duplicated (`pool[112] == pool[122]` and
`pool[116] == pool[126]`) — both pairs point at the same .rdata
address, so they came in through different build inputs and were never
deduped.

## How UI widgets index it

There is **no runtime language flag**. Every dialog hardcodes the
pool indices it needs as compile-time `(PTR_PTR_004afbbc + idx*4)`
loads. Czech dialogs reference the first ~107 entries; English
dialogs (registration/trial) reference 112..126.

Example: `CStartGame2::FUN_0040d930` decompiles to a sequence of

```c
CBulanci::FUN_0042d510(&tmp, *(short**)(PTR_PTR_004afbbc + 0xc8));  // 0xc8/4 = 50 -> "Typ hry:"
piVar2 = CStaticText::FUN_00406a50(widget, 0x14, 0x14, 0, 2, 0x100ae);
...
CBulanci::FUN_0042d510(&tmp, *(short**)(PTR_PTR_004afbbc + 0xec));  // 59 -> "na zásahy"
FUN_004075f0(radio, ...);
CBulanci::FUN_0042d510(&tmp, *(short**)(PTR_PTR_004afbbc + 0xf0));  // 60 -> "na čas"
FUN_004075f0(radio, ...);
CBulanci::FUN_0042d510(&tmp, *(short**)(PTR_PTR_004afbbc + 0x17c)); // 95 -> "na životy"
FUN_004075f0(radio, ...);
```

`xrefs_to 0x004af9b8` returns ~20 such consumer functions —
`FUN_0040be80`, `FUN_0040c060`, `FUN_0040c2d0`, `FUN_0040c650`,
`FUN_0040d570`, `FUN_0040d930`, `FUN_004102d0`, `FUN_004104f0`,
`FUN_00411df0`, `FUN_00416030`, `FUN_00434b80`, etc. — every one of
which is a UI dialog setup.

The compile-time destructor for the singleton lives at `0x0047e4b0`
(rewrites `0x004afbb4` to the `CDSObject` vftable at `0x0047f6a8`)
and is registered for atexit via `FUN_00447e72` from `0x0047c2f0`.

## Relationship with the `language = 1` script call

The single existing language selector in `bulanci.exe` (the immediate
`1` pushed before every `CDSScript::CallExport(slot=0, ...)` — see
[`script_lifecycle.md`](script_lifecycle.md)) is **orthogonal** to
the static-text pool. It only affects per-script bytecode constants
that the script writes back to `globals[0..2]` for the level/help/history
enumerator. The static-text pool is set at link time and never
rewired by the engine.

Translations of the `CDSStaticTexts` pool to a non-Czech locale would
therefore require rebuilding `bulanci.exe` with a different string
table; nothing in the shipped binary supports a runtime swap.

## Misleading neighbours

`unpacked/overlay/res_*_2043_TextBlock*` and
`res_*_2050_TextTable*` are **not** `CDSStaticTexts` resources, even
though the unpacker labels them `TextBlock`/`TextTable`. The actual
classes behind those IDs (recovered via factory→RTTI walks) are:

| classId | factory      | actual class       | shipped count |
|---:|---|---|---:|
| 2043   | `0x00409ab0` | `CPoem`            | 13 (the in-game intro poems) |
| 2050   | `0x004229b0` | `CHistoryScript`   |  3 (history-page scripts)   |
| 2076   | `0x00421830` | `CHelpScript`      |  -                          |

The classID→factory→RTTI walk is straightforward — start from
[`class_registry.py`](class_registry.py) for the (classId, factory,
meta) table, follow the factory's first `MOV [EAX], imm32` to the
primary vftable, then back the COL out of the vftable's `-4` slot.

## Repro / regenerate

```text
python tools/bulanci_unpack/ghidra_analysis/static_texts.py
# -> writes static_texts.json next to this file
```

The script reads the PE directly with `pefile`; no Ghidra MCP needed.
It asserts the singleton's layout matches the addresses above, so a
schema break will fail loudly.
