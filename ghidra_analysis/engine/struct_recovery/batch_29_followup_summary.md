# Struct recovery batch 29 follow-up (round 2)

**Prior:** `batch_29_summary.md`  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_COMPLETE**

## Actions taken

| Follow-up (batch 29) | Result |
|----------------------|--------|
| Disassemble factory `0x00433d80` | Created `CDSStrmResInfo_factory` — `OperatorNew(0x28)`, vtables `0x4873f8`/`0x4873dc`, zeros `+0x10`/`+0x14` |
| Recover `CDSCollection` / `CDSChain` | `CDSCollection` already **VERIFIED** (`CDSCollection.md`, batch 27). New **`CDSChain.md`**: embedded **20-byte** list-head at `CDSStreamStorage+0x34` |
| Name `resField0` / `streamKey` | Renamed Ghidra fields to **`dwResourceId`** / **`dwClassId`**; evidence: `CDSStrmResInfo_CompareKey` (`+8` sort key), `CDSResInfo_Load`/`Save`, `.eap` `ResourceHeader.id` / `class_id` |
| `CDSStrmResInfo` status | Upgraded to **VERIFIED** (`CDSStrmResInfo.md`) |
| `CDSStreamStorage` status | Doc upgraded to **VERIFIED**; chain row collapsed to nested `CDSChain` |

## Ghidra deltas

- `create_function` @ `0x00433d80` (`CDSStrmResInfo_factory`), `0x004342f0` (`CDSStrmResInfo_CompareKey`)
- `CDSStrmResInfo`: renamed key fields; replaced tail with `streamExtent` (`uint64` @ `+0x14`), `streamFlags` @ `+0x1c`, `pad_tail` @ `+0x20`
- `CDSChain`: defined **20-byte** embedded layout (was 1-byte placeholder)
- `save_program bulanci.exe` (once)

## Remaining UNK

- `CDSStrmResInfo.loaderAux`, `pad_tail`
- Full **`CDSChain`** object (`CDSChain_ctor` / size `0xa4` on `CBulanci`) — not the embedded list-head
- `CDSStreamStorage_GetStreamEntry` / `GetStreamCount` interface `this` adjustment vs `CloseStreamByKey` collection base

## Handoff

No open follow-ups from batch 29. Next batches may take **`CDSChain` 0xa4** body or `CDSResInfo` base layout for other class IDs.
