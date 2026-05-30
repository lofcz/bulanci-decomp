# Round 5 worker 42/50 — struct_recovery C–D UNK sweep

## Task

| Field | Value |
|-------|-------|
| Worker | 42 / 50 |
| Mode | WRITE — evidence-only struct UNK (`AGENT_PROTOCOL.md`) |
| Scope | `struct_recovery/{StructName}.md` where **StructName** matches `C*` or `D*` and has a `## UNK` section |
| Exclusions | **CStartGame2** (worker 40 dedicated); **CDeath/CDeath2** tombstone bands (worker 35); manifest todo **42** (`CBulanekCtor`) not duplicated |
| Program | `bulanci.exe` |

## Status

**PARTIAL** — **C–D UNK inventory** + `CDSSafeStreamInfo.m_streamSlice` lazy-path documentation; `CColorSet` ctor/render `this` re-verified (layout bands closed by [round5_worker_39_report.md](./round5_worker_39_report.md)). Remaining items **BLOCKED** (SDK padding, EH-only helpers, init-only dwords).

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `CColorSet_ctor_slotPillar` `this` | `0x0040ffd0` | `set_function_this_type(CColorSet *)` — was `CMenu::CColorSet_ctor_slotPillar(CMenu *)`; decompile now `CColorSet::CColorSet_ctor_slotPillar(CColorSet *, int, int, byte)` |
| `wWidgetFlags \|= 1` | `0x0040ffd0` | Decompile `this->wWidgetFlags = this->wWidgetFlags \| 1` (field @ `+0x46`) — closes prior `pPad_40_67+6` mis-parse |
| `bColorIndex` / `bTeamTint` | `0x0040ffd0` | `this->bColorIndex = 0xff`; `this->bTeamTint = teamTint` @ `+0x68` / `+0x69` |
| `pPillarBitmap` load | `0x0040ffd0` | res `0x10013`; `CheckedVirtualBaseCast(..., DAT_004b826c)` → `this->pPillarBitmap` |
| `CColorSet_Render` tint | `0x0040aed0` | `WidgetStateFlags_ToTintColor(this->bViewStateFlags & 2)`; `*(char*)(pPillarBitmap+0x18) = tint` |
| `CDSSafeStreamInfo.m_streamSlice` lazy path | `0x00446d90` | `CheckedVirtualBaseCast(pv,2)` → vfn `+0x10` → `CheckedVirtualBaseCast(...,7)` → `puVar4[5]`; plate @ `0x00446e50` |
| `CDeath` placement offsets | `CDeath_ctor` / bind | Decompile + asm: `nM_placementOffsetX/Y` only zeroed in ctors; consumed in `CBulanek_ResolveAndBindAnimTrack@0x0041c790` (no non-zero writer) |
| `CDPEnumSessionInfo` header gap | `0x0040e8b0` | `CSessionItem_Initialize` reads only `+0x08..+0x14`, `+0x30` — `0x00..07` / `0x18..2F` SDK padding |
| `CDSResInfo_ReleaseViaChainedFace` | `0x00433aa0` | `get_xrefs_to` → sole `Unwind@00479e80` (EH); no game callsite |
| `CDSDirectSound.m_reserved_40` | `0x0043cbc0` | Zeroed in `InitPrimary` only; no consumer (R4 todo 26) |
| `CDSMouse` factory | `0x0042aa30` | Registration-only `OperatorNew(0xc)`; retail has no `InitializeByClassId(0x1d)` callsite |

## Ghidra deltas

- `set_function_this_type` `CColorSet *` @ `CColorSet_ctor_slotPillar@0x0040ffd0`, `CColorSet_Render@0x0040aed0`
- `set_decompiler_comment` @ `0x0040ffd0`, `0x00446e50` (`CDSSafeStream_GetThreadSlice` lazy slice path)
- `save_program bulanci.exe`

## Struct doc updates

- [CColorSet.md](./CColorSet.md) — layout sync, R5 worker 42 log, UNK trim (`wWidgetFlags` proven)
- [CDSSafeStreamInfo.md](./CDSSafeStreamInfo.md) — `m_streamSlice` lazy alloc path (interface index `7`)

## C–D UNK inventory (scan)

| Struct | UNK outcome this pass |
|--------|------------------------|
| **CColorSet** | **SKIP layout** (worker 39) — worker 42 re-verified `CColorSet *` `this` @ ctor/render |
| **CDSSafeStreamInfo** | **PARTIAL** — `m_streamSlice` = post-cast stream face (idx `7`) |
| **CDeath** / **CDeath2** | **SKIP** — worker 35 closed bind bands |
| **CDPEnumSessionInfo** / **DPEnumSessionInfo** | **BLOCKED** — DirectPlay descriptor padding |
| **CDSResInfo** | **BLOCKED** — `ReleaseViaChainedFace` EH-only |
| **CDSDirectSound** | **BLOCKED** — `m_reserved_40` init-only |
| **CDSMouse** | **CLOSED** — abstract registration shell (R3 task 14) |
| **CDSTrackVector** | **CLOSED** — layout verified; Ghidra `dwTracks*` naming cosmetic |
| **CDSCollection** / **CDSPtrSlotVec** | **CLOSED** — UNK section documents no layout gaps |
| **CListBoxItem** / **CItemInfo** | **BLOCKED** — `dwPad_04` / `dwReserved_08` never read |
| **CDSQueueStream** | **BLOCKED** — DSM overlay vs standalone (R3 task 18) |
| **CDSBitmap** | **BLOCKED** — `+0x30..0x67` chain band semantics (defer `CDSChained`) |
| **CStartGame2** | **SKIP** — worker 40 |

(~90 additional `C*.md` files retain inherited UNK bullets; no duplicate Ghidra edits.)

## Remaining UNK

- `CColorSet`: `pad_40`, `pPad_50`, `bTeamTint` vs `CGameGetPlayerColorByte` semantics
- `CDSSafeStreamInfo`: map interface index `7` to a named `IDS*` struct in Ghidra
- Bulk of `C*` engine/gameplay tails deferred to alphabet workers 43+ / per-class R4 todos

## References

- [ROUND5_TASK_PROTOCOL.md](../ROUND5_TASK_PROTOCOL.md)
- [round5_worker_35_report.md](./round5_worker_35_report.md) — `CDeath` / `CDeath2`
- [round4_task_42_report.md](./round4_task_42_report.md) — `CBulanek` scheduler (manifest todo 42, out of band)
