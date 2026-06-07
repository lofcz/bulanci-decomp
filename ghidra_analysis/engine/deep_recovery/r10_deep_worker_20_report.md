# Round 10 — Worker 20 report (deep-recovery coordinator)

## Task

| Field | Value |
|-------|-------|
| **id** | 20 |
| **round** | 10 |
| **kind** | deep_recovery |
| **role** | coordinator (documentation only) |
| **title** | Coordinator: R10 manifest audit |
| **program** | `bulanci.exe` — **no** Ghidra MCP mutations |

## Status

**DONE** — All 19 worker reports on disk; aggregate checklist and R11 gap list published. Coordinator jsonl line appended. **No** `save_program bulanci.exe` (per worker-20 scope).

## Deliverables

| Artifact | Path |
|----------|------|
| Task protocol | [ROUND10_DEEP_PROTOCOL.md](../ROUND10_DEEP_PROTOCOL.md) |
| Manifest (workers 1–19) | [agent_todos_20_r10_deep.json](../agent_todos_20_r10_deep.json) |
| Spawn template | [scripts/spawn_deep_agents_r10.md](../../scripts/spawn_deep_agents_r10.md) |
| Worker reports | `deep_recovery/r10_deep_task_{01..19}_report.md` |
| Coordinator report | this file |
| Aggregate results (append-only) | [agent_todos_20_r10_deep_results.jsonl](../agent_todos_20_r10_deep_results.jsonl) |

## Worker checklist (tasks 1–19)

| id | kind | title (short) | report | jsonl | status |
|----|------|---------------|--------|-------|--------|
| 1 | logic_rerun | `FUN_0041af70` scheduler/gaming cluster | yes | **missing** | **DONE** |
| 2 | logic_rerun | `CWeapon_OnSchedulerEvent` + neighbors | yes | yes | **DONE** |
| 3 | logic_rerun | `CBulanek_AddEntity` + entity factory | yes | **missing** | **DONE** |
| 4 | logic_rerun | sim slice `0x00429d60`–`0x0042ab28` | yes | **missing** | **DONE** |
| 5 | logic_rerun | sim slice `0x0042d440`–`0x0042e140` (CDSString) | yes | yes | **PARTIAL** |
| 6 | logic_rerun | sim slice `0x00430360`–`0x00430ca0` (MPx stream) | yes | **missing** | **DONE** |
| 7 | logic_rerun | sim slice `0x004327e0`–`0x00432be0` (CDSFlxFile) | yes | **missing** | **PARTIAL** |
| 8 | codec_rerun | IJG slice `0x00466f00`–`0x004681d0` | yes | **missing** | **DONE** |
| 9 | struct | `CWeapon` layout + combat path | yes | **missing** | **DONE** |
| 10 | struct | `CShot` projectile | yes | **missing** | **DONE** |
| 11 | struct | `CMina` landmine | yes | yes | **PARTIAL** |
| 12 | struct | `CGaming` match-modal tail `+0x300..+0x36C` | yes | **missing** | **PARTIAL** |
| 13 | struct | `CDSAudioPlayer` DirectSound path | yes | **missing** | **DONE** |
| 14 | struct | `CMenu` UI dispatch | yes | **missing** | **DONE** |
| 15 | struct | `CDSTrackVector` memmove path | yes | **missing** | **DONE** |
| 16 | codec | `FUN_0046A840` dispatch (IJG homolog) | yes | **missing** | **PARTIAL** |
| 17 | codec | `FUN_0046DB50` dispatch (H2V1 colormap) | yes | **missing** | **DONE** |
| 18 | codec | `FUN_00448A97` high-xref sim | yes | yes | **DONE** |
| 19 | logic_rerun | `CGaming_OnPlayerCollectItem` + pickup | yes | yes | **DONE** |
| 20 | coordinator | this audit | yes | yes (this line) | **DONE** |

**Handoff gaps:** 14 of 19 workers wrote reports but did **not** append jsonl lines (ids 1, 3, 4, 6–10, 12–17). Re-run jsonl append only if ledger parity is required; reports are authoritative on disk.

## Aggregate status

| Status | Count | Task ids |
|--------|------:|----------|
| **DONE** | 14 | 1, 2, 3, 4, 6, 8, 9, 10, 13, 14, 15, 17, 18, 19 |
| **PARTIAL** | 5 | 5, 7, 11, 12, 16 |
| **BLOCKED** | 0 | — |
| **NO_ACTION** | 0 | — |

### Highlights (evidence-backed)

| Area | Outcome |
|------|---------|
| Gaming cluster (1–3, 19) | Scheduler tick vs `IDSEventHandler` MI thunk separated; `CWeapon_OnSchedulerEvent` is **not** `CWeapon*`; entity factory vs slot register split; item pickup dispatch table `0x41a0ac` proven |
| Sim slices (4–7) | R6 PARTIAL sim bands largely closed (4, 6 DONE); CDSString COW slice typed but `__thiscall` ECX retype blocked (5); FLX slice verified, MPx `pSubObjStash@+0x40` still null-only (7) |
| IJG / codec (8, 16–18) | Deferred R6 renames verified live; `FUN_00448A97` = CRT thread-hook nop (not blit/IJG); `FUN_0046DB50` = H2V1 YCbCr colormap path; `FUN_0046A840` = `expand_bottom_edge` homolog (rename retained, cosmetic decomp only) |
| Struct deep (9–15) | `CWeapon` sizeof `0x70`, `CShot` collision fields, `CDSAudioPlayer` DS buffer layout (R5 scope confusion resolved), `CMenu` vtable slots, `CDSTrackVector` 8 B elements all **DONE** |
| Partial struct tails | `CMina` `bArmed=1` writer not found statically (11); `CGaming` `vecSlotVec_2e8` / `pad_0x30c` read-free (12) |

## Systemic blockers (cross-worker)

| Blocker | Affected tasks | Notes |
|---------|----------------|-------|
| `set_function_this_type` HTTP **404** on current Ghidra MCP plugin | 5, 13, 15 (+ cosmetic 1, 9) | Disasm/asm authoritative; manual HighVariable retype or plugin fix needed |
| Ghidra API cannot retype `__thiscall` ECX `this` | 5, 13, 15 | Decompiler shows `CBulanci*` / `void*` on correct `CDSString*` / `CDSAudioPlayer*` bodies |
| Register-arg / `unaff_*` decompiler artifacts | 5, 16, 17 | MSVC convention; no API fix |
| Indirect vtable / DATA-only xrefs | 1, 2, 11 | Runtime poster or Frida required for closure |

## R11 recommended gaps

Priority order for next deep-recovery round:

1. **Plugin / typing pass** — Restore `set_function_this_type` or batch manual ECX retype for `CDSString*`, `CDSAudioPlayer*`, `CDSTrackVector*` (tasks 5, 13, 15 handoff lists).
2. **CMina arming (task 11)** — Frida hook `ExplodeMine@0x0041e070` `bArmed@+0x114` from deploy through first `0xF2` event; static search found zero `MOV byte [reg+0x114], 1`.
3. **CGaming modal tail (task 12)** — `vecSlotVec_2e8@+0x2E8` / `nVecSlot2e8Count@+0x2F0` and `pad_0x30c`: ctor/dtor only, no read xrefs; needs consumer scan outside `+0x300..+0x36C` band or gameplay Frida.
4. **MPx / FLX band split (task 7)** — Dedicated R11 slice `0x00432c60+` (`DecodeFrame`, `CDSMpx_dtor`) for `pSubObjStash@+0x40` and WAVEFORMAT dual-use overlay; current task band is **CDSFlxFile** (ClassID 52).
5. **CBulanek vtable slots `+0x20` / `+0x24` (tasks 1–2)** — Indirect-only; closure needs runtime vfn resolve or vtable byte walk from `0x00481e54`.
6. **CWeapon_OnSchedulerEvent stack params (task 2)** — Unused thunk params; trace scheduler dispatcher event-kind semantics.
7. **CGaming entity register mode (task 3)** — Name `CGaming+0x158` quad used by `CountOccupiedPlayerSlots`; `bEntityRegisterMode@+0x341` default unproven in ctor.
8. **Codec hygiene (tasks 16–17)** — `compress_output@0x0046aa00` mislabel vs true `0x00467930`; `pre_process_data@0x0046a870` `_Globals.cpp` still `STUB_BODY()`; objdiff byte match for IJG homologs.
9. **Jsonl ledger backfill** — Append missing lines for tasks 1, 3, 4, 6–10, 12–17 if automated aggregation is required (reports already complete).
10. **Multiplayer pickup sync (task 19)** — Whether remote peers apply weapon loadout on score event `0x16` vs separate `0x0C` path.

## Evidence

| Claim | Source |
|-------|--------|
| 19/19 worker reports present | `deep_recovery/r10_deep_task_{01..19}_report.md` on disk (coordinator scan 2026-06-07) |
| 5/19 worker jsonl lines at audit | `agent_todos_20_r10_deep_results.jsonl` (ids 2, 5, 11, 18, 19) |
| Manifest task definitions | [agent_todos_20_r10_deep.json](../agent_todos_20_r10_deep.json) |
| Coordinator pattern | [round6_logic_worker_50_report.md](../logic_recovery/round6_logic_worker_50_report.md) |

## Ghidra deltas

**none** — coordinator only.

## Frida

**none** — coordinator only. Workers proposed scripts for tasks **11** (mine arming) only.

## Remaining UNK (round-level)

Consolidated from worker **Remaining UNK** sections — see per-task reports for disasm/xref proof tables. Highest-impact open items are listed under **R11 recommended gaps** above.
