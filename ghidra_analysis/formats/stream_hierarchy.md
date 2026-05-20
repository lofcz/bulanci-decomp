# Stream hierarchy — `IDSStream` byte-stream contract

The "polymorphic byte-stream contract underpinning every loader and the
network/save layers" is the `IDSStream` abstract interface. This file
pins down its vtable slot ordering by walking MSVC RTTI for every
concrete implementer in `bulanci.exe` and cross-checking against the
call-sites in `CDSFlxFile::BindStream` and `CDSFlxFile::CloseStream`.

> **Important nomenclature fix.** `CDSStreamStorage` is *not* the
> byte-stream base. Its RTTI shows it inherits from `IDSStorage` (a
> *container of streams*), with a 9-slot vtable whose getters/setters
> are array-indexed lookups — not byte-IO. The byte-stream contract
> lives on **`IDSStream`** (TypeDescriptor `0x004ae29c`, string
> `.?AVIDSStream@@`), and `CDSStreamStorage` does not inherit from it.

## 1. The 15-slot `IDSStream` vtable

The interface contributes a single subobject with **15 virtual slots**.
The first four are MSVC/lifetime housekeeping; slots **4..12** are the
nine-method byte-IO contract; slots **13..14** are identity and
teardown.

| Slot | Off  | Method                                                          | Errno on failure |
|-----:|-----:|-----------------------------------------------------------------|-----------------:|
|   0  | 0x00 | `GetClassRegistry()` — returns class-info singleton             |   —              |
|   1  | 0x04 | `AddRef()` — no-op (returns adjusted `this`; engine doesn't refcount streams directly) | — |
|   2  | 0x08 | `Release()` — decrement refcount at `this+8`; on zero, propagates to owner | — |
|   3  | 0x0c | `~ScalarDeletingDtor(byte freeFlag)` — MSVC standard            |   —              |
|   4  | 0x10 | `Read(void* buf, DWORD count)`                                  | `1`              |
|   5  | 0x14 | `Write(const void* buf, DWORD count)`                           | `2`              |
|   6  | 0x18 | `Flush()`                                                       | `7`              |
|   7  | 0x1c | `GetSize() : int64`                                             |   —              |
|   8  | 0x20 | `Tell() : int64`                                                |   —              |
|   9  | 0x24 | `SetSize(int64 newSize)`                                        | `4`              |
|  10  | 0x28 | `Seek(int32 off_lo, int32 off_hi, DWORD origin)` — origin `0=BEGIN`, `1=CURRENT`, `2=END` | `3` |
|  11  | 0x2c | `Lock(DWORD off_lo, DWORD off_hi, DWORD len_lo, DWORD len_hi)`  | `5`              |
|  12  | 0x30 | `Unlock(DWORD off_lo, DWORD off_hi, DWORD len_lo, DWORD len_hi)`| `6`              |
|  13  | 0x34 | `GetName(wstring* out)` — writes a human-readable identifier    |   —              |
|  14  | 0x38 | `Close()` — releases backing handle/buffer, sets state to 0x20  |   —              |

Errno `8` ("stream not initialised") is raised by every slot when the
backing buffer/handle is null. Errors are thrown via the helpers
`FUN_00430270(errno, this)` (no Win32 code) and
`FUN_004302e0(errno, this, win32err)` — both unwind through a
`CDSStreamException` object whose RTTI is `.?AVCDSStreamException@@` @
`0x004afd6c`.

## 2. RTTI walk

For each class we list the address of:

* its TypeDescriptor (the MSVC `.?AV...@@` blob);
* its Complete-Object-Locator (COL) for the `IDSStream` subobject —
  this is what the vtable slot at `vtable-4` points at; and
* the start of the `IDSStream` subobject's vtable.

All five vtables agree on the same 15-slot ordering described above.

| Class             | TypeDesc       | IDSStream subobj COL | IDSStream subobj vtable | IDSStream mdisp |
|-------------------|----------------|----------------------|-------------------------|----------------:|
| `CDSFileStream`   | `0x004ae264`   | `0x0049f3b4`         | `0x0047f72c`            | `+0x0c`         |
| `CDSEasyMemStream`| `0x004ae630`   | `0x004a01f0`         | `0x004803dc`            | `+0x0c`         |
| `CDSGZipStream`   | `0x004ae6d0`   | `0x004a0464`         | `0x00480594`            | `+0x0c`         |
| `CDSFilterStream` | `0x004afda4`   | `0x004a3ea0`         | `0x004870fc`            | `+0x0c`         |
| `CDSQueueStream`  | `0x004afc00`   | `0x004a38ec`         | `0x00486dac`            | `+0x04`         |

The `IDSStream` subobject sits at `+0x0c` in every class except
`CDSQueueStream`, where it sits at `+0x04` because `CDSQueueStream`
puts `IDSStream` as its first non-primary base.

### 2.1 Per-class slot contents

The "C++-level" semantics column is what the same slot does on each
class. The shared rows (slots 0..3, 6, 11, 12) are utility/lifetime;
the working rows are 4, 5, 7, 8, 9, 10, 13, 14.

| Slot | `CDSFileStream`   | `CDSEasyMemStream` | `CDSGZipStream`   | `CDSFilterStream` | `CDSQueueStream`  |
|-----:|-------------------|--------------------|-------------------|-------------------|-------------------|
|   0  | `0x00401620`      | `0x00401620`       | `0x00401620`      | `0x00401620`      | `0x00401620`      |
|   1  | `0x00409450`      | `0x00409450`       | `0x00409450`      | `0x00409450`      | `0x0042ac90`      |
|   2  | `0x00409490`      | `0x00409490`       | `0x00409490`      | `0x00409490`      | `0x00428d30`      |
|   3  | `0x004016b0`      | `0x00409250`       | `0x00409990`      | `0x004300c0`      | `0x00428a40`      |
|   4  | `0x00433400` Read | `0x004307f0` Read  | `0x00435220` Read | `0x00430420` Read | `0x0043c000` Read |
|   5  | `0x00433470`Write | `0x004308c0`Write  | `0x004352e0`Write | `0x00430490`Write | `0x0043c0b0`Write |
|   6  | `0x00433720`Flush | `0x00467430` ø     | `0x00446c10`Flush | `0x0042ff90`Flush | `0x00467430` ø    |
|   7  | `0x00433660` Size | `0x00409220` Size  | `0x00435360` Size | `0x0042ffa0` Size | `0x0043beb0` Size |
|   8  | `0x004336c0` Tell | `0x00409230` Tell  | `0x00435390` Tell | `0x00430090` Tell | `0x0043bee0` Tell |
|   9  | `0x004335e0` SetS | `0x004309f0` SetS  | `0x00446c20` SetS | `0x00430d00` SetS | `0x0043bf10` SetS |
|  10  | `0x00433560` Seek | `0x00430980` Seek  | `0x004353c0` Seek | `0x00430500` Seek | `0x0043bf80` Seek |
|  11  | `0x004334c0` Lock | `0x00430db0` ✗     | `0x0043bff0` ✗    | `0x004305c0` ✗    | `0x0043bff0` ✗    |
|  12  | `0x00433510` Unlk | `0x00430dc0` ✗     | `0x00434f90` ✗    | `0x00430640` ✗    | `0x00434f90` ✗    |
|  13  | `0x00401740` Name | `0x00430a40` Name  | `0x004099b0` Name | `0x004300d0` Name | `0x0043c1a0` Name |
|  14  | `0x00401640` Close| `0x00409200` Close | `0x00409030` Close| `0x00430c10` Close| `0x00428960` Close|

* `ø` — implementation is a no-op (memory and queue streams don't
  buffer anything to flush).
* `✗` — implementation throws `CDSStreamException` with errno `5`/`6`
  ("operation not supported"). Only `CDSFileStream` actually
  implements `LockFile` / `UnlockFile`.

### 2.2 What each working slot does on `CDSFileStream`

The Win32-backed implementation makes the contract unambiguous.

* **`Read` (`0x00433400`)** — `ReadFile(this+0x10, buf, count, &got, NULL)`.
  Throws errno `1` on `BOOL == 0`; throws errno `1` (Win32 code
  `0x26`) on short read.
* **`Write` (`0x00433470`)** — `WriteFile(this+0x10, buf, count, &put, NULL)`.
  Throws errno `2` on failure or short write.
* **`Flush` (`0x00433720`)** — `FlushFileBuffers(this+0x10)`.
  Throws errno `7` on failure.
* **`GetSize` (`0x00433660`)** — `GetFileSize(this+0x10, &hi)`,
  composes the 64-bit return via `__allmul`.
* **`Tell` (`0x004336c0`)** — `SetFilePointer(this+0x10, 0, &hi, FILE_CURRENT)`.
* **`SetSize` (`0x004335e0`)** — Tell → Seek(new) → `SetEndOfFile` → Seek(old).
* **`Seek` (`0x00433560`)** — origin switch (`0/1/2` → `BEGIN/CURRENT/END`) →
  `SetFilePointer(this+0x10, lo, &hi, origin)`.
* **`Lock` (`0x004334c0`)** — `LockFile(this+0x10, off_lo, off_hi, len_lo, len_hi)`.
* **`Unlock` (`0x00433510`)** — `UnlockFile(...)`.
* **`GetName` (`0x00401740`)** — copies the path string at `this+0xc`
  into the caller's `wstring`.
* **`Close` (`0x00401640`)** — calls `CloseHandle(this+0x1c)`, sets
  state flag at `this+4` to `0x20`.

The Win32 handle lives at `this+0x10` (file handle) and `this+0x1c`
(the same handle, copied for close-path; legacy slot). The state field
at `this+0x4` is the "open/closed" sentinel (`0x20` = closed).

### 2.3 `CDSEasyMemStream` field layout (validated against slots 4/5/7/8/10)

| Off | Field |
|----:|-------|
| `+0x08` | cursor (read/write position) |
| `+0x0c` | logical size (returned by `GetSize`) |
| `+0x10` | buffer capacity for the ring math |
| `+0x18` | ring-buffer head offset (non-zero for ring/queue mode) |
| `+0x1c` | backing buffer base pointer (`NULL` ⇒ `Read/Write/SetSize/Seek` throw errno `8`) |
| `+0x28` | (referenced by `Close` to free the backing buffer) |

### 2.4 `CDSFilterStream` is a transparent passthrough

The `Read` slot at `0x00430420` decompiles to a bounds-check followed
by `(this->inner->vtbl[4])(buf, count)` (i.e. `inner+0x10` =
**slot 4**), then advances the local cursor. `Write` at `0x00430490`
does the same via `inner->vtbl[5]` (`inner+0x14`). This is the
cleanest cross-check that the slot indices in the inner stream are the
same as in the outer — confirming the contract is uniform.

The inner stream pointer lives at `this+0x24`, with cursor at
`this+0x0c..0x13` (u64) and size at `this+0x1c..0x23` (u64);
`this+0x20` is a "negative-allowed" flag (`-1` ⇒ no upper bound,
i.e. an open-ended filter).

## 3. Caller-side evidence — `CDSFlxFile`

These functions invoke `IDSStream` through raw vtable offsets, so they
double-check both the slot indices and the call-signatures.

### 3.1 `CDSFlxFile::BindStream` (`0x00432ac0`)

```c
(**(code **)(*param_1 + 0x10))(&local_24, 0x24);                          // Read(buf, 36)
uVar1 = (**(code **)(**(int **)(this + 0x14) + 0x20))();                  // Tell() → u64
(**(code **)(**(int **)(this + 0x14) + 8))();                             // Release()   ← on the old stream
```

Slot offsets used: `0x10` (slot 4 = Read), `0x20` (slot 8 = Tell),
`0x08` (slot 2 = Release).

### 3.2 `CDSFlxFile::CloseStream` (`0x00432b60`)

```c
(**(code **)(**(int **)(p+0x34) + 0x28))(off_lo, off_hi, 0);              // Seek(off, BEGIN)
(**(code **)(**(int **)(p+0x34) + 0x10))(buf, len);                       // Read
(**(code **)(**(int **)(p+0x34) + 8))();                                  // Release
```

Slot offsets used: `0x28` (slot 10 = Seek with `origin=0`), `0x10`
(slot 4 = Read), `0x08` (slot 2 = Release). Same as 3.1.

## 4. `CDSStreamException` errno table

Used by every implementer via `FUN_004302e0(errno, this, win32err)` /
`FUN_00430270(errno, this)`. The codes are *engine-side*, not
`GetLastError`.

| errno | Meaning              | Stock string @ 0x0049e260+ |
|------:|----------------------|----------------------------|
|   1   | read failed          | "stream error" / "stream end" |
|   2   | write failed        | "stream error" |
|   3   | seek failed         | "stream error" |
|   4   | setsize failed      | "stream error" |
|   5   | lock unsupported    | "stream error" |
|   6   | unlock unsupported  | "stream error" |
|   7   | flush failed        | "stream error" |
|   8   | stream not initialised | "stream error" |

## 5. Who *actually* inherits `IDSStream`

The xrefs to `IDSStream`'s TypeDescriptor (`0x004ae29c`) plus
`CDSFilterStream` (`0x004afda4`) give the concrete byte-stream classes:

* **Direct `IDSStream` subobjects** (MSVC didn't dedupe their BCD):
  `CDSFileStream`, `CDSQueueStream`.
* **Inherit via `CDSFilterStream`** (which itself is an `IDSStream`):
  `CDSEasyMemStream`, `CDSGZipStream`, `CDSSafeStream`,
  `CDSFilterStream` (the abstract base instances are never created
  directly — `CDSFilterStream`'s vtable is the default passthrough that
  `CDSGZipStream` / `CDSSafeStream` override).

### Not part of this hierarchy

The original `STATUS.md` entry lumped three companion classes into the
stream-hierarchy bullet that aren't actually `IDSStream` subclasses.
Their RTTI BCD chains contain no IDSStream entry; they're data/state
helpers held *by* a stream:

* `CDSGZipStreamData` (`0x004b0010`) — zlib state held inside
  `CDSGZipStream`.
* `CDSSafeStreamInfo` (`0x004b0de4`) — checksum/footer block for
  `CDSSafeStream`.
* `CDSMemQueue` (`0x004b0888`) — bare ring-buffer used by
  `CDSQueueStream` (no virtuals; it owns the storage but not the
  vtable).

## 6. Reproducing the RTTI walk

The walk uses three MCP primitives — `get_xrefs_to`, `read_memory`,
`decompile_function` — and a known fact: in MSVC RTTI, the
TypeDescriptor is at `string_addr - 8`, the COL is at `vtable_ptr - 4`,
and the chain is `COL → TypeDescriptor → CHD → BCD[]`.

For each concrete stream class:

1. Find the TypeDescriptor: subtract 8 from the address of the
   `.?AVCDSXxxStream@@` ascii string.
2. `get_xrefs_to(TypeDesc)` returns one COL per subobject (offset 0,
   4, 12, etc.) plus one BCD per ancestor that mentions it.
3. Read 20 bytes at each xref; the COL pattern is `sig(0), offset,
   cdOffset, pTypeDesc, pClassDescriptor`.
4. Pick the COL whose `offset` field equals the `IDSStream` mdisp
   (12 for the file/mem/gzip/filter classes, 4 for the queue class).
5. `get_xrefs_to(COL_addr)` returns exactly one address — the
   vtable-pointer slot in `.rdata`. The vtable starts at that
   address `+ 4`.
6. Read 60 bytes (15 slots × 4) to dump the vtable.

The same walk was repeated for the five classes in §2 and the slot
contents in §2.1 were generated by chaining `decompile_function` over
each address.
