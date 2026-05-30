# Round 5 — Worker 18 Report

## 1. Task

| Field | Value |
|-------|--------|
| **worker** | 18 / 50 |
| **title** | FUN_* destructor / scalar-deleting paths |
| **mode** | WRITE (Ghidra MCP) |
| **acceptance** | Disasm + vtable xref proof for every rename; document blockers |

## 2. Status

**DONE** — Renamed **30** destructor-related symbols that were still generic (`Destructor`, `ScalarDeletingDestructor`, `DeletingDestructorThunk_*`) on the score / level-score / poem / UI-counter teardown spine. MSVC scalar-deleting pattern and MI adjustor thunks verified before each rename.

## 3. Proof pattern (MSVC)

**Scalar-deleting dtor** (canonical):

```text
MOV ESI, ECX
CALL <class_dtor>
TEST byte ptr [ESP+8], 1
JZ  skip
PUSH ESI
CALL _free
skip:
MOV EAX, ESI
RET 4
```

Example: `CScoreItem_ScalarDeletingDtor@0x00409a90` → `CALL CScoreItem_dtor@0x004093c0` @ `0x00409a93`.

**MI adjustor thunk** (vtable slot +3 on IDSChained / IDSEventHandler facets):

```text
SUB ECX, <adjustor>
JMP  <canonical ScalarDeletingDtor>
```

Example: `CScoreItem_ScalarDeletingDtor_thunk_n0x4@0x004093b0` — `SUB ECX,0x4` / `JMP 0x00409a90`; vtable `CScoreItem` @ `0x48046c` slot 3 per `vftable_methods.csv`.

## 4. Evidence table (renames)

### 4.1 Destructor bodies (was `Destructor`)

| New name | Address | Proof |
|----------|---------|-------|
| `CScoreItem_dtor` | `0x004093c0` | `[param_1+4]` → `CDsStringReleaseHeader`; `CDSChained_InitIDSReferencedVtable`; sole callee from `CScoreItem_ScalarDeletingDtor` |
| `CPoem_dtor` | `0x004094a0` | `[param_1+6]` string release; `IDSChainedTail_ClearSubObjStash`; callee of `CPoem_ScalarDeletingDtor` |
| `CScore_dtor` | `0x0040e120` | Restores four `CScore` vtables; `CDSAudioPlayer_Stop` + release `pEndMatchAudio`; `CWindow_dtor` |
| `CPoemScroller_dtor` | `0x00425ca0` | `CPoemScroller_ReleaseOwnedResources`; vector dtors; `CDSView_dtor` |
| `CShotCounter_dtor` | `0x004272d0` | `_eh_vector_destructor_iterator_` @ `+0x68`; `CDSView_dtor` |
| `CWeapon_dtor` | `0x0041c550` | `CWeapon_SetTrackHolder(NULL)`; refcount releases; `CDSVideoPlayer_TM_Destructor` |

### 4.2 Scalar-deleting wrappers (was `ScalarDeletingDestructor`)

| New name | Address | Inner dtor | `_free` gate |
|----------|---------|------------|--------------|
| `CScoreItem_ScalarDeletingDtor` | `0x00409a90` | `CScoreItem_dtor` | `TEST [ESP+8],1` @ `0x00409a98` |
| `CPoem_ScalarDeletingDtor` | `0x00409af0` | `CPoem_dtor` | same |
| `CLevelScore_scalar_deleting_dtor` | `0x00409a40` | `CLevelScore_dtor` | *(pre-named)* |
| `CScore_ScalarDeletingDtor` | `0x0040f070` | `CScore_dtor` | same |
| `CGameCounter_ScalarDeletingDtor` | `0x0040ef40` | `_Globals::CGameCounter_dtor` | same |
| `CShotCounter_ScalarDeletingDtor` | `0x00427a60` | `CShotCounter_dtor` | same |
| `CPoemScroller_ScalarDeletingDtor` | `0x004264e0` | `CPoemScroller_dtor` | same |

### 4.3 MI thunks (was `DeletingDestructorThunk_*`)

| Class | Address | Disasm | Vtable (slot 3) |
|-------|---------|--------|-----------------|
| `CLevelScore` | `0x00409320` | `SUB ECX,4` → `JMP 0x00409a40` | `0x480440` |
| `CScoreItem` | `0x004093b0` | `SUB ECX,4` → `JMP 0x00409a90` | `0x48046c` |
| `CPoem` | `0x00409430` | `SUB ECX,0xc` → `JMP 0x00409af0` | `0x4804b4` |
| `CPoem` | `0x00409440` | `SUB ECX,0x14` → `JMP 0x00409af0` | `0x480498` |
| `CPoem` | `0x00409460` | `SUB ECX,4` → `JMP 0x00409af0` | `0x4804cc` |
| `CGameCounter` | `0x0040bcc0` | `SUB ECX,0x18` → `JMP 0x0040ef40` | `0x480f40` |
| `CGameCounter` | `0x0040bcb0` | `SUB ECX,0x10` → `JMP 0x0040ef40` | `0x480f54` |
| `CGameCounter` | `0x0040bcd0` | `SUB ECX,4` → `JMP 0x0040ef40` | `0x480f6c` |
| `CScore` | `0x0040bde0` | `SUB ECX,0x18` → `JMP 0x0040f070` | `0x481000` |
| `CScore` | `0x0040be00` | `SUB ECX,0x10` → `JMP 0x0040f070` | `0x481014` |
| `CScore` | `0x0040bdf0` | `SUB ECX,4` → `JMP 0x0040f070` | `0x48102c` |
| `CShotCounter` | `0x004272b0` | `SUB ECX,0x18` → `JMP 0x00427a60` | `0x4838c4` |
| `CShotCounter` | `0x004272a0` | `SUB ECX,0x10` → `JMP 0x00427a60` | `0x4838d8` |
| `CShotCounter` | `0x004272c0` | `SUB ECX,4` → `JMP 0x00427a60` | `0x4838f0` |
| `CPoemScroller` | `0x00425db0` | `SUB ECX,4` → `JMP 0x004264e0` | `0x4837f8` |
| `CPoemScroller` | `0x00425dd0` | `SUB ECX,0x10` → `JMP 0x004264e0` | `0x48380c` |
| `CPoemScroller` | `0x00425de0` | `SUB ECX,0x18` → `JMP 0x004264e0` | `0x4837e0` |
| `CPoemScroller` | `0x00425dc0` | `SUB ECX,0x68` → `JMP 0x004264e0` | `0x483824` *(old Ghidra label `Thunk_1c` was wrong)* |

## 5. Ghidra deltas

- `rename_function_by_address` × 30 (see §4)
- `save_program bulanci.exe`

## 6. Struct doc updates

- [CScoreItem.md](./CScoreItem.md) — dtor / scalar-deleting / thunk map
- [CLevelScore.md](./CLevelScore.md) — scalar-deleting + thunk
- [CScore.md](./CScore.md) — `CScore_dtor` + three MI thunks + scalar wrapper

## 7. Remaining UNK / out of scope

| Symbol | Address | Reason |
|--------|---------|--------|
| `FUN_004097d0` | `0x004097d0` | Vector elem ctor (6 B); not a dtor — `_eh_vector_constructor_iterator_` helper from `CDSChain_ctor` |
| `FUN_0041b4a0` | `0x0041b4a0` | `CMina` heap-node release (`_free`); not scalar-deleting vtable path — needs separate CMina danger-zone proof |
| Widget `CButton_vDtor` etc. | `0x00406670`… | Already class-named; outside this worker’s score/poem/counter batch |
| `DeletingDestructorThunk_18` @ `0x004032a0` | `0x004032a0` | **Not** a dtor — returns 0/1 from bbox compare (misclassified name); left unchanged |

## 8. Cross-check

Post-rename decompile `CScoreItem_ScalarDeletingDtor_thunk_n0x4@0x004093b0` calls `CScoreItem_ScalarDeletingDtor` with adjusted `this` — chain closed.
