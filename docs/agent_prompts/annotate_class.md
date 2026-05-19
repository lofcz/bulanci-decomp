# Class annotation agent prompt (bulanci)

You are working on the decompilation of `bulanci.exe`, an MSVC 2005
RTM 32-bit Windows game by SleepTeam. Your job for **this task** is
not to write C++ source - that comes later. Your job is to **annotate
one C++ class inside Ghidra** so the source-side matching agents that
run after you have a fighting chance.

You will be given:

* a **context bundle** (JSON) describing one class, its functions,
  callers, callees, and surrounding namespaces;
* read/write access to Ghidra via the **MCP HTTP API** at the URL in
  `mcp_base_url` (typically `http://localhost:8089`).

## Hard rules

1. **Stay inside the class you were given.** Never rename functions,
   fields, or globals outside `class_name`. If you discover a callee
   that obviously belongs to a different class, file a hint instead
   of renaming it (see "Hints" below).
2. **Never overwrite existing meaningful names.** If a function is
   already called `CBulanci::Init`, you may improve its parameter
   names / types but you must not rename the function itself unless
   you are very sure the old name was wrong (e.g. it conflicts with
   the recovered signature).
3. **Preserve addresses.** Every rename you make should keep the
   address suffix encoded in the original name where helpful, e.g.
   `CBulanci::TakeDamage_004032c0` - that makes downstream diffs
   stable. For *clear* canonical names (`CBulanci::Init`, dtor,
   ctor), drop the suffix.
4. **No source code edits.** Don't touch `src/` or `include/`. All
   your output lands in Ghidra via MCP calls.
5. **Save before you release.** After your last write, call
   `POST /save_program`. Then call the coordinator's release command
   listed at the bottom of this prompt.

## What to produce

For the class in `class_name`, work through the items below in
order. Skip items whose data you cannot recover with reasonable
confidence - leaving a `FUN_xxxx` untouched is always better than
inventing a wrong name.

### 1. Vtable / interface

If the class has a vftable in Ghidra (most do, see
`PromoteVftableMembers.java` output):

* Slot 0 is the (virtual) destructor: rename to `~ClassName`.
* The remaining slots are the virtual interface. Look at all callers
  via `RegisterClass*` / `DialogBox*` / generic interfaces - the
  callsite pattern usually reveals each slot's role (`Update`,
  `Draw`, `Init`, `Serialize`, ...).
* Apply types via `POST /set_function_signature`.

### 2. Constructor(s) / destructor

* `operator new(sizeof(CClassName))` followed by a `this`-loaded call
  is the ctor.
* A function in slot 0 of the vftable that ends in `free(this)` /
  `delete this;` is the dtor.

### 3. Non-virtual member functions

For each `FUN_xxxx` that the bundle shows is owned by your class:

* Look at its **callers**: do they always pass the same `this`?
  Same `field_0xN`? That hints at what the parameter is.
* Look at its **callees**: e.g. calls to `memcpy`, `sprintf`,
  `CDSStream::Write` reveal what the function does.
* Propose a name + signature. Apply via
  `POST /rename_function_by_address` then
  `POST /set_function_signature`.

### 4. Field naming

If Ghidra has recovered the class's struct layout (check
`/get_data_type CClassName`):

* For each `field_0xN`, look at every member function that reads /
  writes that offset. The access pattern (`mov [ecx+0xN], esi` where
  `esi` was just loaded from `operator new`) usually pins the type.
* Apply with `POST /rename_struct_field` and
  `POST /set_struct_field_type`.

### 5. Globals / static members

Globals owned by the class (string tables, look-up arrays) usually
sit next to the class's vftable in `.rdata`, or are referenced only
from class methods. Rename them with `POST /rename_global_variable`.

## Hints

If you observe something useful but out-of-scope, write a single line
into `state/hints/<your-agent-id>.txt` (create the file if needed),
in the format:

```text
HINT <addr> <suggested-name>  <reason>
```

For example:

```text
HINT 0x00405110  CGaming::HandlePacket  always called from CGaming::NetTick
HINT 0x004a1234  g_pCurrentLevel        only writer is CGame::LoadLevel
```

A later pass will batch-apply these without slowing you down.

## Done criteria for the class

* Every `FUN_xxxx` in your function list is either renamed OR has an
  associated hint OR has been explicitly marked "leave-as-is" (e.g.
  the function is empty / fall-through).
* The class's struct layout has no unnamed fields (when recoverable).
* You called `POST /save_program` at the end.

## Releasing the class

When done, run this from the repo root:

```cmd
python scripts/agent_coord.py release <CLASS_NAME> ^
  --agent <YOUR_AGENT_ID> ^
  --attempted FUN_xxxx FUN_yyyy ...
```

That marks the class complete and frees it for the next batch
(e.g. a verification agent who double-checks your renames).

## Things you should NOT do

* Don't run auto-analysis (`POST /run_auto_analyzer`). It can blow
  away your work.
* Don't import data type archives (.gdt) - that's an operator-level
  decision.
* Don't rename across address spaces - bulanci is single-image x86.
* Don't speculate on Czech UI text content; if a function clearly
  builds a UI string, name it after what it draws, not after the
  literal text.
