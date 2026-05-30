# Round 4 — Task 22 report

## Task

| Field | Value |
|-------|--------|
| **id** | 22 |
| **title** | CTeleportPoint respawn overlap_entity writer; decompile UpdateStateFromParams +0x64 |
| **priority** | high (blocker) |
| **supersedes** | R3 todo 22 |
| **addresses** | `0x0041fed0`, `0x004178f7`, `0x0041d090`, `0x0041a390`, `0x0041ff16` |
| **acceptance** | Resolve respawn `pOverlap_entity` writer vs `pGaming_host`; fix `CBulanek_UpdateStateFromParams` decompile for `MOV [ECX+0x64]`; update CTeleportPoint.md / CAnim.md |

**Types:** `CTeleportPoint`, `CAnim`, `CBulanek`, `CDSChained`

## Status

**DONE** — `pOverlap_entity` typed `CBulanek *` @ CAnim `+0x64` (hex); `UpdateStateFromParams` decompile shows `pHeader_tail_58+0xc` (not gate overlap); respawn path documented; OnEvent `param_2==0` predecessor insn annotated; `save_program bulanci.exe`.

## Findings

### 1. `overlap_entity` writers (CAnim / CTeleportPoint `+0x64` hex)

| Writer | Address | Target |
|--------|---------|--------|
| `CDSChained_ctor` | `0x00403366` | `MOV [ESI+0x64], EDI` — zero init in ctor chain |
| `CDSChained_InitWithRect` | `0x0040b611` | `MOV [ESI+0x64], EDI` — map / rect init |
| *(none in respawn path)* | `CreateRespawnTeleportPair@0x0041d090` | Dual `OperatorNew(0xf8)` + ctor + `AddEntity`; **no** store to `+0x64` |

`CBulanek_AddEntity@0x0041a390` writes owner only at **`[entity+0x84]`** (`0x0041a3a6`), not `+0x64`.

**Respawn pair:** `CBulanek_OnTakeDamage@0x0041fb2b` → `CreateRespawnTeleportPair(host, (int *)damaged_player, …)`. Ctor stores **`pPartner_node@+0xf0 = damaged CBulanek*`** (same pointer for both gates), **not** the paired gate object. `overlap_entity` remains zero after ctor.

### 2. `CBulanek_UpdateStateFromParams` — not gate overlap

| Evidence | Detail |
|----------|--------|
| Asm | `MOV dword ptr [ECX+0x64], EDX` @ `0x004178f7` with `ECX = CBulanek *this` |
| Decompile (R4) | `*(undefined4 *)(this->pHeader_tail_58 + 0xc) = *(undefined4 *)(param_3 + 4);` then `bActiveInWorld`..`bView_flag_6b` from `param_3+8` |
| Layout | CBulanek `pHeader_tail_58@+0x58` + `0xc` = **byte offset 100 = asm `+0x64` hex** — distinct from CAnim `pOverlap_entity@+0x64` hex on `CTeleportPoint` |

R3 note linking this function to teleport `overlap_entity` was a **false positive** (same hex offset, different struct).

### 3. `CTeleportPoint_OnEvent` branches

| `param_2` | Gate byte `@+0x68` | Behavior |
|-----------|---------------------|----------|
| `0` | `bView_state_68 == 0` | Partner teleport: read **`pOverlap_entity`** (`MOV ECX,[ESI+0x64]` @ `0x0041feff` / `0x0041ff4c`); `CDSView__SetPosition` + `TriggerTeleportFX` on that `CBulanek*` |
| `-1` | `bView_state_68 != 0` | `TryActivateFromOverlap(pOverlap_entity)`; predecessor partner flag via **`MOV EAX,[ESI-8]`** @ `0x0041feeb` → `this[-1].pPartner_node` (paired alloc stride `0xf8`) |

**`param_2==0` predecessor GetPosition (R4 asm):** `MOV EDX,[ESI-0x8C]` @ `0x0041ff16` = **`(predecessor_gate + 0x6c)`** = `predecessor.canim_base.dwView_aux_6c` used as `CDSView*` for vfn `+0x70` (not `gaming_host`).

Decompile after R4 types `pOverlap_entity` as `CBulanek *` and uses `->vftable_primary+0x70` (was bogus `pCdsViewPrefix`).

### 4. `TryActivateFromOverlap` vs slots

`CTeleportPoint_TryActivateFromOverlap@0x004165d0` expects a **`CBulanek*`** (`+0x16a`, `+0x84`). `CBulanek_OnDeath` passes **`this`** directly. OnEvent `param_2==-1` passes **`pOverlap_entity`** from the gate — must be non-null `CBulanek*` at runtime (collision / net / script path; **no** `.text` store found on respawn-created gates).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CAnim.pOverlap_entity` | `CBulanek *` @ offset `0x64` (hex) |
| `set_decompiler_comment` | `0x004178f0` / `0x004178f7` | CBulanek net band, not gate overlap |
| `set_decompiler_comment` | `0x0041d090` | Respawn partner = damaged `CBulanek*` |
| `set_disassembly_comment` | `0x0041feeb`, `0x0041ff16` | Paired-gate predecessor offsets |
| `force_decompile` | `0x004178f0`, `0x0041fed0`, `0x0041d090` | Verified pseudocode |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CAnim.md](./CAnim.md) — R4 `pOverlap_entity` typing; corrected writer table (drop `UpdateStateFromParams`)
- [CTeleportPoint.md](./CTeleportPoint.md) — R4 respawn / OnEvent / `pPartner_node` semantics

## Remaining UNK

- **Runtime `pOverlap_entity` fill** on respawn gates before `OnEvent param_2==0` — no additional `.text` `MOV [gate+0x64]` beyond `CDSChained_ctor` / `InitWithRect`; likely collision dispatch, net sync, or script (not pinned in this pass).
- **`pPartner_node` dual role** — `CTeleportPoint *` for map paired gates vs `CBulanek *` on respawn (`OnTakeDamage`); consider `void *` / union doc until split proven.
- **OnEvent `param_2==0` with zero overlap** — would fault on `vfn+0x70`; implies fill must occur before event or branch unused on respawn gates.
