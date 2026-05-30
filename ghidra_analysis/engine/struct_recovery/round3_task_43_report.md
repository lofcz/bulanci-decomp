# Round 3 — Task 43 report

## Task

| Field | Value |
|-------|-------|
| **id** | 43 |
| **title** | Plate CDSCollection_InsertKeyed true CDSCollection* ECX |
| **source** | `agent_todos_50_r3.json` (handoff; supersedes todo 43) |
| **types** | CDSCollection, CDSUpdatedItem, CDSStreamStorage |
| **addresses** | `0x004312c0`, `0x0042f060`, `0x00431170`, `0x00433cb0` |
| **acceptance** | `set_function_this_type(CDSCollection*)` on `CDSCollection_InsertKeyed`; plate/asm ECX proof; caller comment; xref ECX bases |

## Status

**DONE** — `set_function_this_type` succeeded where prior `set_function_prototype` / `set_parameter_type` were blocked. Decompiler header is `CDSCollection::CDSCollection_InsertKeyed(CDSCollection *this, …)` with `this->pM_items` / `this->nM_count`.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `this` is `CDSCollection *` (ECX) | `0x004312c0` | `get_function_variables`: `this` → `CDSCollection *`, storage `ECX:4 (auto)` |
| Decompiler class namespace | `0x004312c0` | `CDSCollection::CDSCollection_InsertKeyed(CDSCollection *this, int key, void *compareFn, char allowDuplicate)` |
| Field access on collection | `0x004312c0` | Decompile uses `this->pM_items`, `this->nM_count`; growth path `CDSCollection_EnsureCapacity(this, this->nM_count + 1)` |
| Asm: ECX → ESI, collection fields | `0x004312d2` / `0x00431303` | `MOV ESI,ECX`; `[ESI+0x8]` / `[ESI+0xc]` before `CALL 0x00431070` (m_items / m_count) |
| `FindKeyIndex` call: ECX = standalone collection | `0x004312dc` | `CALL 0x00431170` with `ECX` unchanged from `ESI` (no `+0x1c` skip) |
| Embedded collection path (contrast) | `0x00433cb0` | `CloseStreamByKey`: `LEA ECX,[ESI+0x1c]` before `FindKeyIndex` — `CDSStreamStorage` embeds `CDSCollection` at parent `+0x1c` |
| Caller: first arg is `g_pTaskList`, not ctor `this` | `0x0042f107` | `CDSUpdatedItem_ctor` → `CDSCollection_InsertKeyed(g_pTaskList, …)`; PRE comment documents heap `CDSCollection` (0x18 B), not `CDSUpdatedItem*` |

### ECX xref bases (InsertKeyed vs FindKeyIndex consumers)

| Site | Instruction | ECX meaning |
|------|-------------|-------------|
| `InsertKeyed@0x004312dc` | `CALL FindKeyIndex` | `CDSCollection*` base (`MOV ESI,ECX` @ `0x004312d2`) |
| `CloseStreamByKey@0x00433d09` | `LEA ECX,[ESI+0x1c]` | Embedded `CDSCollection` inside `CDSStreamStorage` |

Prior misread: sole xref from `CDSUpdatedItem_ctor` made Ghidra attribute `InsertKeyed` to `CDSUpdatedItem`; `+0x8`/`+0xc` on a `CDSUpdatedItem*` looked like `pSelf` / `pEventSlots` instead of `m_items` / `m_count`.

## Ghidra deltas

- `set_function_this_type` @ `0x004312c0` → **`CDSCollection *`** (function moved into class `CDSCollection`)
- `set_plate_comment` @ `0x004312c0` — Parameters / asm / xref contrast
- `set_decompiler_comment` @ `0x004312d2`, `0x004312dc`, `0x00433d09`, `0x0042f107`
- `force_decompile` @ `0x004312c0`, `0x0042f060`, `0x00431170`, `0x00433cb0`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSCollection.md](./CDSCollection.md) — follow-up (agent todo 43 r3) closed: ECX retype via `set_function_this_type`

## Remaining UNK

- `CDSCollection_InsertKeyed` still calls `CDSStreamStorage_FindKeyIndex((CDSStreamStorage *)this, …)` in decompile — callee namespace / prototype on `0x00431170` not retargeted in this task (same ECX, wrong static type).
- `FindKeyIndex` decompile still maps `+0x8`/`+0xc` to wrong member names when `this` is typed as `CDSStreamStorage*` — separate `set_function_this_type` on `0x00431170`.
