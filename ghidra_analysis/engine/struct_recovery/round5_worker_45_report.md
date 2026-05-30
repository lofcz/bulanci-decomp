# Round 5 — Worker 45 Report (struct_recovery N–S UNK)

## Task

| Field | Value |
|-------|-------|
| **worker** | 45 / 50 |
| **mode** | WRITE |
| **scope** | Struct deliverables `C[N-S]*`, `CDS[N-S]*`, `ODSImage` — UNK triage + evidence-only Ghidra fixes |
| **bucket size** | 33 `.md` deliverables (alphabet slice after `C` prefix) |

## Status

**PARTIAL** — three provable UNK closures in Ghidra; remainder surveyed and left documented (shared `CDSChained` shells, factory-only paths, no new field xrefs).

## Evidence (Ghidra apply)

| Claim | Address | Evidence |
|-------|---------|----------|
| `CScore.pEndMatchAudio` is `CDSAudioPlayer *` @ `+0x70` | `0x00411010` | `CScore_ctor`: `TriggerBankSample` → `CDSAudioPlayer_Play`; stores `this->pEndMatchAudio` |
| Same field released on dtor | `0x0040e120` | `CScore::Destructor`: `CDSAudioPlayer_Stop(this->pEndMatchAudio,1)` + vtable release |
| Factory clears tail slot | `0x0040efd0` | `CScore::Create`: `puVar1[0x1c] = 0` (= offset `0x70`) |
| `CScore_ctor` / dtor `this` | `0x00411010`, `0x0040e120` | `set_function_this_type` → `CScore *`; was `CBulanci *` / `__fastcall` untyped |
| `CPoemScroller` CDS classId **2044 (`0x7fc`)** | `0x0047c1b0` | Static-init cluster: `HandleClassRegister(factory@0x426570, classId 0x7fc, meta@0x4b3998)`; `GetClassIdentifier@0x00425da0` |
| `CShot_Ctor` `this` | `0x0041edf0` | `set_function_this_type` → `CShot *` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CScore.pEndMatchAudio` | `CDSAudioPlayer *` @ `+0x70` |
| `modify_struct_field` | `CScore` offset `0x70` | Field name `pEndMatchAudio` |
| `set_function_this_type` | `0x00411010` | `CScore_ctor` in class `CScore` |
| `set_function_prototype` + `set_function_this_type` | `0x0040e120` | `void __thiscall CScore_Destructor(CScore *)` |
| `set_function_this_type` | `0x0041edf0` | `CShot_Ctor` → `CShot *` |
| `set_decompiler_comment` | `0x00411010`, `0x0047c1b0` | R5 w45 notes |
| `force_decompile` | `0x00411010`, `0x0040e120` | `this->pEndMatchAudio` visible |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CScore.md](./CScore.md) — `pEndMatchAudio` typed; R5 apply log; UNK struck
- [CPoemScroller.md](./CPoemScroller.md) — classId `0x7fc`; UNK struck
- [CShot.md](./CShot.md) — `CShot_Ctor` this-type note

## Bucket survey (no Ghidra change — remaining UNK)

| Struct | UNK theme | Why no change |
|--------|-----------|---------------|
| `CDSObject` … `CDSStrmResInfo` | Exception/stream facets, ctor-zero pads | No new per-field consumers in N–S slice |
| `CPauseDlg`, `CSetupDlg`, `CSessionList`, `CScrollBar`, `CSwitch` | Shared `CDSChained` / dialog shells | Prior R3/R4 passes; no struct-specific writers |
| `CSpells` | `+0x78` mask vs ammo bytes | Disjoint lifetimes; no single-type proof |
| `CStartGame2` | `pad_be`, overlap @ `+0x40` | Documented overlap with `CWindow` band |
| `CScoreItem`, `CPoem` | Wire/schema UNK | Closed in R3/R4 |
| `ODSImage` | `pOwner` host typing | R4 rejected `CDSView *`; keep `CBulanci *` namespace |
| `CSessionItem` | (if present) | No actionable xref beyond chain |

## Remaining UNK (worker scope)

- Shared `CDSChained` dword bands on UI types (`CScrollBar`, `CSwitch`, dialogs) — inherit [CDSChained.md](./CDSChained.md).
- `CSpells` dual use of `+0x78..+0x7a`.
- `CShot` scheduler interior `+0x88..+0x9f` and `+0x44..+0x67` view band.
- `CPoemScroller` `pad_30_67`, `TextShaper` triple @ `+0x94`.
- Whether `InitializeByClassId(0x7fc)` runs outside menu embed (factory registered; menu uses stack ctor).
