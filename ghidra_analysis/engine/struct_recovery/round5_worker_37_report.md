# Round 5 — Worker 37 Report

## Task

| Field | Value |
|-------|-------|
| **worker** | 37 / 50 |
| **round** | 5 |
| **title** | `CDSCollection` — `FindKeyIndex` + `m_items` entry layout |
| **structs** | CDSCollection, CDSStrmResInfo, CDSStreamStorage, CDSUpdatedItem |
| **addresses** | `0x00431170`, `0x004312c0`, `0x004312dc`, `0x00433cb0`, `0x00433d04`, `0x004342f0`, `0x0042f060`, `0x0042f182` |
| **prior** | R4 tasks 43 / 45 / 28 — `CDSCollection *` ECX and rename closed |

## Status

**DONE** — Documented `m_items` element semantics per host, `FindKeyIndex` linear vs binary modes, stack search-key layout for `CloseStreamByKey`, and three live caller xrefs. Ghidra PRE + `CompareKey` prototype updated; program saved.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Collection fields in lookup | `0x00431170` | Asm `MOV EBP,ECX`; `[EBP+0x8]` / `[EBP+0xc]` = `m_items` / `m_count`; decompile `pM_items`, `nM_count` |
| Linear mode | `0x0043117b`–`0x0043119d` | `compareFn==0`; walk slots; `CMP [slot], key` |
| Binary mode | `0x004311a8`–`0x004311f6` | `CALL [compareFn]` with `&m_items[mid]` + key local; return `-1-index` on miss |
| Stream embed caller | `0x00433d09`–`0x00433d10` | `LEA ECX,[ESI+0x1c]`; `CALL FindKeyIndex` |
| Stack key + resource id | `0x00433cef`–`0x00433d04` | `[ESP+0x14]`/`+0x18` vtables; `[ESP+0x1c]=EBP` (resource id); key arg `LEA [ESP+0x18]` |
| Entry fetch after lookup | `0x00433d24`–`0x00433d27` | `MOV ECX,[ESI+0x24]`; `MOV EDI,[ECX+EDI*4]` → `m_items[index]` |
| Compare on `+0x08` | `0x004342f0` | `MOV ECX,[ECX+0x8]`; `SUB ECX,[EAX+0x8]` (`dwResourceId`) |
| Task list insert key | `0x0042f08a`–`0x0042f110` | `LEA EDI,[ESI+0x4]` → `InsertKeyed` / slot store |
| Task list remove lookup | `0x0042f182` | `FindKeyIndex(g_pTaskList, &this+0x4, NULL)` |
| InsertKeyed callee | `0x004312dc` | `CALL 0x00431170`; uses insert hint `-1 - index` on miss |

## Ghidra deltas (R5)

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00431170` | R5 w37: slot/compare modes |
| `set_decompiler_comment` | `0x00433d04` | Stack `CDSStrmResInfo` key layout |
| `set_decompiler_comment` | `0x004342f0` | `int **` compare semantics |
| `set_function_prototype` | `0x004342f0` | `int __cdecl CDSStrmResInfo_CompareKey(int **pSlotEntry, int **pSearchKey)` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSCollection.md](./CDSCollection.md) — **`m_items` entry layout** table; **`FindKeyIndex`** algorithm table; R5 follow-up log.

## Remaining UNK

- `FindKeyIndex` formal `key` remains `int` (holds pointer or address of search facet); narrowing to `void *` / `int **` blocked by ECX auto-param pattern — cosmetic.
- `CompareKey` second operand path with stack key at `object+0x4` vs resource id at `object+0x8` — document in struct UNK; works in binary builds (verify if retyping search key to `LEA object+0x14` is required).
