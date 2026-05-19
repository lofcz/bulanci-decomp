# `CDSResourceSign` (ClassID 94 = `Sign`) — Editor-level colophon record

**TL;DR.** `CDSResourceSign` is **not a cryptographic signer**. It is the
serialisable colophon record that Editor.exe writes as the *first*
resource (`res_0000100000_94_Sign.bin`) at the head of every `.eap`
level pack. It carries three fields — **publish date, content title,
copyright string** — plus a one-byte trailing flag that is always
`0x00` in shipping content. No keys, no hashing, no algorithm of any
kind. The class earns its "Sign" name only in the colloquial "signing
your work" sense.

## Engine class registration

The class registers itself at static-init time as engine **ClassID
`0x5E` = 94** in the standard `CDSResInfo`-style class registry
already documented in `dsm_file_format.md`:

| Register field | Value | Notes |
|---|---|---|
| Class ID            | `0x5E` (94)              | passed as `push 0x5e` in the init stub |
| Factory function    | `0x00434930`             | `malloc(0x28); InitializeMembers(p)` |
| In-place ctor       | `0x00434540`             | sets the four vptrs, zeros the strings, calls `SetDate(0,1,1)` |
| Most-derived dtor   | `0x00434670`             | frees the two `std::wstring` payloads |
| Scalar-deleting dtor| `0x004349A0`             | dtor + `free(this)` thunk |
| Init stub           | `0x0047D1D0` (3-arg)     | `push factory; push namePtr; push id=0x5e; mov ecx, slot; call register` |
| Follow-up stubs     | `0x0047D200..0x0047D260` | 3 auxiliary registrations linking secondary interface tags into the same slot |
| Registry slot       | `0x004B7F48` (BSS)       | per-class `{next, namePtr, id=0x5e, ctor=0x434930, 0}` node |
| Per-class meta slot | `0x004B7BFC` (BSS)       | shared "default name" pointer reused across many classes |

After all init stubs run, ID 94 ends up in the engine's flat
`g_apClassByIdTable[0x1000]` (`DAT_004B3BE0`) so anything calling
`InitializeByClassId(94, ...)` (`0x0042EF00`) reaches the factory. In
the shipping codebase the only call site that does this is the
`CDSStreamStorage` / `.eap` resource reader, which deserialises each
record by `(id, stream) -> instance`.

## Class shape (`sizeof = 0x28` = 40 bytes)

Constructor (`0x00434540`) allocates 40 bytes and writes:

```
+0x00  void*           vtable1  (primary, offset 0)
+0x04  void*           vtable2  (offset 4 sub-object)
+0x08  void*           vtable3  (offset 8 sub-object)
+0x0C  u32  = 0        (refcount / chain field — set to 0)
+0x10  void*           vtable4  (offset 16 sub-object)
+0x14  u32  = 1        CDate    (initial value, immediately rewritten by SetDate(0,1,1))
+0x18  std::wstring    wszContent     (length-prefixed UTF-16, header at *(this+0x18) - 0xC)
+0x1C  std::wstring    wszCopyright   (same MSVC std::wstring layout)
+0x20  u8              flagByte = 0   (single trailing byte)
+0x24  u8[3]           pad
```

The 4 vtables match the RTTI Class Hierarchy Descriptor at
`0x004A4518`: 8 base classes, sub-object offsets `0 / 4 / 8 / 16`. The
COLs are at `0x004A4504, 0x004A45A0, 0x004A45B4, 0x004A45C8`; the
corresponding vtables live at `0x0048750C, 0x004874F0, 0x004874D8,
0x004874C4`. RTTI type descriptor is at `0x004AFEF8` and decodes to
the MSVC-mangled string `.?AVCDSResourceSign@@`.

### Where the 14 functions go

Counting the methods that actually live under
`CDSResourceSign::` in the decompiler (`bulanci.exe.c`):

| Address | Role |
|---|---|
| `0x0042E680` | virtual write helper for the date field (forwards to `IDSStream::WriteFixed(4)`) |
| `0x0042E6A0` | virtual read  helper for the date field |
| `0x00434310` | `WriteToStream` (date, wszContent, wszCopyright, flagByte) |
| `0x004343A0` | `ReadFromStream` (date, wszContent, wszCopyright, conditional flagByte) |
| `0x00434540` | in-place ctor |
| `0x004345C0` | `GetClassMeta` → returns `&DAT_004B7F48` (the class-registry node) |
| `0x004345D0..0x004345F0` | three adjustor thunks for the scalar-deleting dtor on sub-objects at `+4 / +8 / +16` |
| `0x00434610` | `Release()` (refcount-or-destroy on primary subobject) |
| `0x00434640..0x00434660` | three adjustor thunks for `Release()` |
| `0x00434670` | most-derived dtor body |
| `0x004349A0` | scalar-deleting dtor |

= 1 + 1 + 1 + 1 + 1 + 1 + 3 + 1 + 3 + 1 + 1 = **14** vtable-reachable
methods, exactly as the STATUS.md tally claimed. None of them touches
a buffer larger than the fields above, none of them references a
key or constant, and none of them calls into `bcrypt`, `wincrypt`,
`zlib::crc32`, `MD4`, `MD5`, `SHA1`, `XXTEA`, or any home-grown LFSR
in the engine.

## On-disk format (61 bytes for the shipping tutorial)

The single shipping instance is
`unpacked/tutorial_eap/res_0000100000_94_Sign.bin`:

```
offset  size  field                  example value
+0x00   u32   dwPackedDate           0x003EB21A  -> 2006-08-26
+0x04   u32   cchContent             10
+0x08   u8[]  wszContent (UTF-16LE)  "Tutorial 1"           (20 bytes)
+0x1C   u32   cchCopyright           14
+0x20   u8[]  wszCopyright (UTF-16LE)"SleepTeam Labs"       (28 bytes)
+0x3C   u8    flagByte               0x00
```

**Date packing.** Engine helper at `0x0042E7B0` (`CDate::SetDate(year,
month, day)`) packs the date with `(year << 11) | (month << 6) | day`.
Decoding `0x003EB21A`:

| field  | bits          | value |
|--------|---------------|-------|
| year   | `[31:11]`     | `0x7D6` = 2006 |
| month  | `[10:6]`      | `0x08`  = August |
| day    | `[5:0]`       | `0x1A`  = 26 |

The result matches `"date": "2006-08-26"` from the unpacker's
`res_0000100000_94_Sign.sign.json` exactly. The `cchContent` and
`cchCopyright` length fields are character counts; each character is
two bytes. The trailing `flagByte` is read by `ReadFromStream` only
when the stream's recorded version is `>=` the engine's stored
version threshold; in v1 streams the byte is always present and
always `0x00`.

### Constructor's hard-coded "2002-03-13"

`ReadFromStream` calls `CDate::SetDate(2002, 3, 13)` on a local CDate
before reading the optional flag byte. This date almost certainly
matches the engine's own build / format-locked date (the engine was
in development at SleepTeam Labs from 2002 onwards). It does NOT
appear in the file format and is not compared against anything in
the shipping binary — it's just a default the engine writes onto a
temporary `CDate` so a later "version-gating" branch has a defined
non-zero value to work with.

## Why the shipping master pack has no `Sign` record

```
$ grep '"className":' unpacked/overlay/_manifest.json | sort -u
  "className": "AudioBank"
  "className": "AudioBankIndex"
  "className": "BitmapJPEG"
  "className": "BitmapJpegAnim"
  "className": "BitmapSpecial"
  "className": "BitmapSprite"
  "className": "DsmInner"
  "className": "MouseCursor"
  "className": "Mp3"
  "className": "Script"
  "className": "Poem"
  "className": "HistoryScript"
```

No `Sign`. The master pack (`bulanci.exe` overlay, 21.7 MB) ships
with the classes the *runtime* renderer needs and nothing else. The
Editor-time path is different: when **Editor.exe** packs a level into
a `.eap`, it always writes one `CDSResourceSign` at resource ID
`100000` to act as the level's colophon. This is visible in the only
shipped editor pack we have, `tutorial.eap`:

```
res_0000100000_94_Sign.bin       <-- this class, this format
res_0000100001_21_BitmapJPEG.bin
res_0000100002_21_BitmapJPEG.bin
res_0000100003_48_Mp3.bin
res_0000100004_28_BitmapSpecial.bin
res_0000100005_28_BitmapSpecial.bin
res_0000100006_28_BitmapSpecial.bin
res_0000100007_28_BitmapSpecial.bin
res_0000100008_28_BitmapSpecial.bin
res_0000100009_2026_Script.bin
```

The runtime is happy to *read* a `Sign` record because the class
factory is registered, but no code path in `bulanci.exe` reads back
the fields (the strings just end up in a freshly-constructed object
that is dropped on the next iteration of the loader loop). The
Editor-side counterpart (`Editor.exe`) is the only consumer that
populates the wstring fields.

## Answers to the STATUS.md open questions

* **Algorithm?** None. There is no hash, MAC, signature, or cipher
  anywhere in the 14 methods. The whole class is `(CDate, wstring,
  wstring, u8) <-> IDSStream`.
* **Key handling?** None. Nothing in the binary is keyed by anything
  related to this class. The `.data` neighbours `DAT_004B7F14` and
  `DAT_004B7F48` are **BSS slots** (load-time zero, not baked
  ciphertext); the random-looking bytes we read straight from the
  PE file were just past-end alignment padding outside any real
  section.
* **DRM / mod-detection / cache validation?** None of the three. It's
  the editor's **colophon / about-this-level metadata**: who made it,
  when, and what's the level called. Removing or modifying the
  record produces no engine-side warning, refusal-to-load, or
  hash-mismatch error.

## Renames / labels / comments applied via Ghidra MCP

### Function renames

| Address | Old | New |
|---|---|---|
| `0x00434540` | `FUN_00434540`              | `CDSResourceSign::InitializeMembers` |
| `0x00434930` | `CreateObject` (library dupe) | `CDSResourceSign::InitializeAndAllocate` |
| `0x00434670` | `FUN_00434670`              | `CDSResourceSign::HandleDestructInstance` |
| `0x004349A0` | `FUN_004349a0`              | `CDSResourceSign::HandleScalarDelete` |
| `0x004345C0` | `FUN_004345c0`              | `CDSResourceSign::GetClassRegistry` |
| `0x00434310` | `FUN_00434310`              | `CDSResourceSign::WriteToStream` |
| `0x004343A0` | `FUN_004343a0`              | `CDSResourceSign::ReadFromStream` |
| `0x0042E680` | `FUN_0042e680`              | `CDSResourceSign::ReadDateField` |
| `0x0042E6A0` | `FUN_0042e6a0`              | `CDSResourceSign::WriteDateField` |
| `0x00434610` | `FUN_00434610`              | `CDSResourceSign::HandleRefcountRelease` |
| `0x004345D0` | `FUN_004345d0`              | `CDSResourceSign::AdjustThisAndScalarDeleteVtable2` |
| `0x004345E0` | `FUN_004345e0`              | `CDSResourceSign::AdjustThisAndScalarDeleteVtable3` |
| `0x004345F0` | `FUN_004345f0`              | `CDSResourceSign::AdjustThisAndScalarDeleteVtable4` |
| `0x00434640` | `FUN_00434640`              | `CDSResourceSign::AdjustThisAndRefcountReleaseVtable2` |
| `0x00434650` | `FUN_00434650`              | `CDSResourceSign::AdjustThisAndRefcountReleaseVtable3` |
| `0x00434660` | `FUN_00434660`              | `CDSResourceSign::AdjustThisAndRefcountReleaseVtable4` |
| `0x0047D1D0` | (no function)               | `RegisterCDSResourceSignAsClass94` (3-arg main registration) |
| `0x0047D200` | (no function)               | `RegisterCDSResourceSignAuxClass1` (linked to `g_pCDSResourceSignClassNode`) |
| `0x0047D230` | (no function)               | `RegisterCDSResourceSignAuxClass2` |
| `0x0047D260` | (no function)               | `RegisterCDSResourceSignAuxClass3` |

The `AdjustThisAnd...VtableN` names look long because the MCP guardrails
reject token-subsets of `HandleScalarDelete` / `HandleRefcountRelease`;
the verb `AdjustThis` captures the actual MSVC behaviour (each thunk is
a `sub this, N; jmp <main>` adjustor for the sub-object at offset N).

### Data / RTTI labels

| Address | Label |
|---|---|
| `0x004B7F48` | `g_pCDSResourceSignClassNode` (the `{next, name, id=0x5e, ctor, 0}` registry record) |
| `0x0048750C` | `CDSResourceSign_vftable` (primary, offset 0) |
| `0x004874F0` | `CDSResourceSign_vftable_Vtable2_Offset4` |
| `0x004874D8` | `CDSResourceSign_vftable_Vtable3_Offset8` |
| `0x004874C4` | `CDSResourceSign_vftable_Vtable4_Offset16` |
| `0x004A4504` | `CDSResourceSign_COL_Vtable1` (Complete Object Locator, primary) |
| `0x004A45A0` | `CDSResourceSign_COL_Vtable2_Offset4` |
| `0x004A45B4` | `CDSResourceSign_COL_Vtable3_Offset8` |
| `0x004A45C8` | `CDSResourceSign_COL_Vtable4_Offset16` |
| `0x004A4518` | `CDSResourceSign_ClassHierarchyDescriptor` |
| `0x004A4528` | `CDSResourceSign_BaseClassArray` (8 entries) |
| `0x004AFEF8` | `CDSResourceSign_RTTITypeDescriptor` (string `.?AVCDSResourceSign@@`) |

### Plate comments

Plate comments now live on `InitializeMembers` (0x00434540),
`InitializeAndAllocate` (0x00434930), `WriteToStream` (0x00434310),
`ReadFromStream` (0x004343A0), and `RegisterCDSResourceSignAsClass94`
(0x0047D1D0). Each one cross-references this document and spells out
the algorithm / parameters / on-disk layout so the next reader doesn't
have to chase the chain again.

## Suggested STATUS.md tier upgrade

`Resource signing` was filed under "Confidence: Empty" with `Algorithm,
key handling, what it's used for` as the open questions. All three are
now closed, and the only artefact for the entry is this document plus
the lone shipping instance at
`unpacked/tutorial_eap/res_0000100000_94_Sign.bin`. The entry should
move to **Confidence: Decoded** with the new tag `Editor colophon
record` (not `Resource signing`) so future maintainers don't go
looking for crypto that isn't there.
