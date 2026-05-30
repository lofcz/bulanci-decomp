# Round 3 — task 22 report

## Task

| Field | Value |
|-------|--------|
| **id** | 22 |
| **title** | CTeleportPoint gate overlap_entity runtime fill vs gaming_host +0x84; OnEvent param_2==0 partner path |
| **source** | blocker (supersedes R2 todo 22) |
| **acceptance** | Document `overlap_entity` @ `+0x64` vs `gaming_host` @ `+0x84`; `set_function_this_type` on `__thiscall` handlers; update CTeleportPoint.md / CAnim.md |

## Status

**DONE** — `overlap_entity` and `gaming_host` are distinct slots with distinct writers; `AddEntity` disasm/decompile shows `*(entity+0x84)=owner` only; `OnEvent` `param_2==0` branch documented (still uses paired-alloc `this[-1]` for predecessor gate). Ghidra `__thiscall` `this` typing applied; `save_program bulanci.exe`.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| `overlap_entity` zero init | `CDSChained_ctor@0x00403366` | `param_1[0x19]=0` → `MOV [obj+0x64],0` (CAnim ctor chain) |
| `overlap_entity` net fill (non-teleport views) | `CBulanek_UpdateStateFromParams@0x004178f7` | `MOV [ECX+0x64], EDX` from `param_3+4` |
| `gaming_host` owner bind | `CBulanek_AddEntity@0x0041a390` | `MOV dword ptr [EDI+0x84], ESI` @ `0x0041a3a6` |
| AddEntity does not touch `+0x64` | `CBulanek_AddEntity@0x0041a390` | Body: only `[entity+0x84]=this`; no `+0x64` store |
| Respawn pair alloc `0xf8` | `CBulanek_CreateRespawnTeleportPair@0x0041d090` | `OperatorNew(0xf8)` ×2; `CTeleportPoint_Ctor` + `AddEntity` |
| Ctor host param unused for `+0x64` | `CTeleportPoint_Ctor@0x0041c9a0` | Stores `partner` @ `+0xf0` only; `host` not written to `overlap_entity` |
| OnEvent reads `+0x64` | `CTeleportPoint_OnEvent@0x0041fed0` | `MOV ECX,[ESI+0x64]` @ `0x0041feff`; decompile `(this->canim_base).pOverlap_entity` |
| OnEvent `param_2==-1` overlap probe | `CTeleportPoint_OnEvent@0x0041fed0` | `CTeleportPoint_TryActivateFromOverlap((uint)overlap_entity)` |
| OnEvent `param_2==0` partner teleport | `CTeleportPoint_OnEvent@0x0041fed0` | Uses `overlap_entity` vtable `+0x70`, `CDSView__SetPosition`, `TriggerTeleportFX`; predecessor via `this[-1].canim_base.*` @ `0x0041ff16` (paired stride `0xf8`) |
| `param_2==0` not `gaming_host` | `CTeleportPoint_OnEvent@0x0041fed0` | No read of `canim_base.pGaming_host` / `+0x84` on this branch |

**Address note:** R2 comment anchor `0x00403951` lies inside `CScrollBar_HitTest`; canonical `AddEntity` is **`0x0041a390`** (store insn **`0x0041a3a6`**).

## Ghidra deltas (this task)

| Action | Address | Result |
|--------|---------|--------|
| `set_function_this_type` | `0x0041fed0` | `CTeleportPoint::CTeleportPoint_OnEvent` → `CTeleportPoint *` |
| `set_function_this_type` | `0x0041c9a0` | `CTeleportPoint::CTeleportPoint_Ctor` → `CTeleportPoint *` |
| `set_function_this_type` | `0x0041d090` | `CBulanek::CBulanek_CreateRespawnTeleportPair` → `CBulanek *` |
| `set_function_this_type` | `0x0041a390` | `CBulanek::AddEntity` → `CBulanek *` |
| `set_function_this_type` | `0x004178f7` | `CBulanek::CBulanek_UpdateStateFromParams` → `CBulanek *` |
| `set_function_prototype` | `0x0041a390` | `void __thiscall AddEntity(CBulanek *this, void *entity, char register_flag)` — decompile shows `*(CBulanek **)((int)entity + 0x84) = this` |
| `force_decompile` | above | Refreshed `OnEvent` / `AddEntity` / `CreateRespawnTeleportPair` |
| `save_program` | `bulanci.exe` | saved |

Prior R2 work retained: `CAnim.pLinked_bulanek` → `overlap_entity` (`CBulanek *`); PRE_COMMENTs @ ctor chain, OnEvent, AddEntity.

## Struct doc updates

- [CTeleportPoint.md](./CTeleportPoint.md) — agent todo 22 block; `AddEntity` @ `0x0041a390` / `0x0041a3a6`
- [CAnim.md](./CAnim.md) — `overlap_entity` / `gaming_host` writer table

## Remaining UNK

- **`overlap_entity` on respawn teleports** — stays zero after ctor; `OnEvent` `param_2==0` still dereferences `pOverlap_entity` (runtime must set via unpinned path or paired-gate convention).
- **`param_2==0` `this[-1]`** — heap adjacency hack until `CTeleportPoint` / `CAnim` layout exposes predecessor partner without `this-8`.
- **`CBulanek_UpdateStateFromParams` decompile** — insn `MOV [ECX+0x64],EDX` @ `0x004178f7` not yet surfaced as `this->overlap_entity` (ECX/view typing).
