# Round 12 — Pointer Task 04 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **title** | `CBulanek_ClampMoveRectByCollision` pointer arithmetic @ `0x004195f0` |
| **seed_address** | `0x004195f0` |
| **reference** | [r11_ai_task_10_report.md](../ai_recovery/r11_ai_task_10_report.md), [CBulanek.md](../struct_recovery/CBulanek.md), [CGameView.md](../struct_recovery/CGameView.md) |

## Status

**PARTIAL** — `CBulanek` struct fields at all five hot offsets are disasm-proven and typed in Ghidra (`pChainParent@+0x4C`, `pGaming_host@+0x84`, `bPlayerSlot@+0x70`, `videoTrackManager.nCurrentTrackIdx@+0xD4`, `vftable_IDSReferenced@+0x10`). Stored prototype is `void __thiscall CBulanek_ClampMoveRectByCollision(CBulanek *this, …)`. Decompiler header/body still show `void *this` and `(int)this+0xNN` because Ghidra MCP cannot retype the `__thiscall` ECX auto-parameter. Program saved.

## Known issues (pre-fix)

| Issue | Pre-fix decompile |
|-------|-------------------|
| Prototype header | `void __thiscall _Globals::CBulanek_ClampMoveRectByCollision(void *this, …)` |
| Gaming host | `*(void **)((int)this + 0x84)` |
| Map bbox parent | `*(int *)((int)this + 0x4c)` then deref `+0x20..+0x2c` |
| Facing axis | `switch(*(undefined4 *)((int)this + 0xd4))` |
| Player slot | `*(byte *)((int)this + 0x70)` |
| Bot reroll message | `CDSView__PostMessage((void *)((int)this + 0x10), 0x200, 0x102, 0, 0)` |

## Disassembly evidence (offsets)

`EDI` = `this` after `MOV EDI,ECX` @ `0x004195f4`. Saved `this` on stack @ `[ESP+0x10]` for switch/slot reads.

### `+0x84` → `pGaming_host` (`CGaming *`)

| Instruction | Address | Proof |
|-------------|---------|-------|
| `MOV ECX, dword ptr [EDI + 0x84]` | `0x004195f6` | Early-out null test before `SpatialQuery` |
| `CALL 0x00418300` (args include `ECX`) | `0x0041965e` | First arg to `SpatialQuery` is gaming host |

`get_struct_layout CBulanek` → `pGaming_host` @ offset **132 = 0x84**, type `CGaming *` (renamed from `pGamingHostScratch`).

### `+0x4C` → `pChainParent` (`CDSView *`)

| Instruction | Address | Proof |
|-------------|---------|-------|
| `MOV ECX, dword ptr [EDI + 0x4c]` | `0x00419663` | Load parent view pointer |
| `MOV EDX, dword ptr [ECX + 0x2c]` / `SUB EDX, dword ptr [ECX + 0x24]` | `0x00419666`–`0x0041966c` | `mapH = parent.bottom − parent.top` |
| `MOV EDI, dword ptr [ECX + 0x28]` / `SUB EDI, dword ptr [ECX + 0x20]` | `0x00419669`–`0x0041966f` | `mapW = parent.right − parent.left` |

Cross-function proof (CDSChained header band, not child_chain sibling link):

| Instruction | Address | Proof |
|-------------|---------|-------|
| `MOV dword ptr [ECX + 0x4c], EAX` | `0x0042beca` | `CDSChained_ResetChainCounters` clears **`pChainParent`** |

Matches `CGameView.md` / `get_struct_layout CGameView` → `pChainParent` @ **76 = 0x4C**. Parent bbox layout `+0x20..+0x2C` is the standard CDSView origin/bounds band (playfield shell defaults 800×600 from ctor).

### `+0xD4` → `videoTrackManager.nCurrentTrackIdx`

| Instruction | Address | Proof |
|-------------|---------|-------|
| `MOV ECX, dword ptr [ECX + 0xd4]` | `0x00419676` | Facing switch input (`ECX` = saved `this`) |
| `JMP dword ptr [ECX*4 + 0x41979c]` | `0x00419685` | Jump table cases 0..3 (L/R/U/D) |

Offset arithmetic:

- `get_struct_layout CBulanek` → `videoTrackManager` @ **+0xA8** (168)
- `get_struct_layout CDSVideoPlayer` → `nCurrentTrackIdx` @ **+0x2C** (44) inside embed
- **168 + 44 = 212 = 0xD4**

### `+0x70` → `bPlayerSlot`

| Instruction | Address | Proof |
|-------------|---------|-------|
| `MOV AL, byte ptr [ECX + 0x70]` | `0x00419763` | Slot filter before `PostMessage` |
| `CMP AL, 0x20` / `CMP AL, 0x24` / `CMP AL, 0x28` | `0x00419766`–`0x00419774` | Coop `0x20..0x23` and practice `0x24..0x27` bands |

`get_struct_layout CBulanek` → `bPlayerSlot` @ **112 = 0x70**.

### `+0x10` → `vftable_IDSReferenced` (PostMessage facet)

| Instruction | Address | Proof |
|-------------|---------|-------|
| `ADD ECX, 0x10` | `0x00419788` | `this` → CDSView messaging facet |
| `CALL 0x0042f390` (`CDSView__PostMessage`) | `0x0041978b` | Posts `0x200` / `0x102` (bot wall reroll) |

**Note:** facet is **`vftable_IDSReferenced` @ +0x10**, not `vftable_IDSEventHandler` @ +0x18. Event dispatch still reaches `CBulanek_OnEvent` case `0x102`.

## Ghidra MCP actions applied

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x004195f0` | `void __thiscall CBulanek_ClampMoveRectByCollision(CBulanek *, int *pOrigin, tagRECT *pProposed, int *pCollisionAtOrigin, tagRECT *pHitBoundsOut)` — **accepted**; warns ECX `this` not retyped via API |
| `modify_struct_field` | `CBulanek.pGamingHostScratch` | → `pGaming_host` (`CGaming *`) @ +0x84 |
| `modify_struct_field` | `CBulanek.dwField_40` | → `dwChainRoot` @ +0x40 |
| `modify_struct_field` | `CBulanek.dwView_flags` | → `wChainInit44` (`ushort`) @ +0x44 |
| `add_struct_field` | `CBulanek` @ +0x46..+0x50 | `wChainFlag46`, `wChainFlag48`, `wChainFlag4a`, `pChainParent`, `dwChainField_50` — aligns chain band with `CGameView` |
| `set_parameter_type` / `set_local_variable_type` | `this` → `CBulanek *` | **Failed** — register auto-parameter (`ECX:4 (auto)`) |
| `force_decompile` | `0x004195f0` | redecompiled |
| `save_program` | `bulanci.exe` | saved |

## Decompile before / after

### Before (header + hot accesses)

```c
void __thiscall
_Globals::CBulanek_ClampMoveRectByCollision
          (void *this, int *pOrigin, tagRECT *pProposed, int *pCollisionAtOrigin, tagRECT *pHitBoundsOut)

{
  ...
  if ((*(void **)((int)this + 0x84) != (void *)0x0) && ...) {
    piVar3 = _Globals__SpatialQuery(*(void **)((int)this + 0x84), ...);
    iVar7 = *(int *)((int)this + 0x4c);
    iVar5 = *(int *)(iVar7 + 0x2c) - *(int *)(iVar7 + 0x24);
    iVar7 = *(int *)(iVar7 + 0x28) - *(int *)(iVar7 + 0x20);
    switch(*(undefined4 *)((int)this + 0xd4)) { ... }
    bVar1 = *(byte *)((int)this + 0x70);
    ...
    CDSView__PostMessage((void *)((int)this + 0x10), 0x200, 0x102, 0, 0);
  }
}
```

### After (current — struct fixed, `this` typing blocked)

Header and body **unchanged** in live decompiler output (same `(int)this+0xNN` forms). Stored signature and `CBulanek` layout now match asm offsets.

### Intended symbolic form (manual ECX retype in Ghidra UI)

```c
void __thiscall CBulanek_ClampMoveRectByCollision(
    CBulanek *this,
    int *pOrigin,
    tagRECT *pProposed,
    int *pCollisionAtOrigin,
    tagRECT *pHitBoundsOut)
{
  CDSView *parent;
  int mapW, mapH;

  if (this->pGaming_host == NULL) return;
  if (rects_equal(pCollisionAtOrigin, pHitBoundsOut)) return;

  hit = SpatialQuery(this->pGaming_host, pCollisionAtOrigin, pHitBoundsOut, &local_hit, 0, 0);
  parent = this->pChainParent;
  mapW = parent->nSpatial_bucket_x - parent->nOrigin_x;   /* +0x28 − +0x20 */
  mapH = parent->nSpatial_bucket_y - parent->nOrigin_y;   /* +0x2c − +0x24 */
  switch (this->videoTrackManager.nCurrentTrackIdx) { ... }  /* +0xD4 */
  ...
  if ((coop_or_dummy_slot(this->bPlayerSlot)) && blocked)
    CDSView__PostMessage(&this->vftable_IDSReferenced, 0x200, 0x102, 0, 0);
}
```

## Struct layout delta (`CBulanek` chain header)

| Offset | Before | After |
|--------|--------|-------|
| `+0x40` | `dwField_40` | `dwChainRoot` |
| `+0x44` | `dwView_flags` (4 B) | `wChainInit44` + `wChainFlag46/48/4a` (matches `CGameView`) |
| `+0x4C` | unnamed byte inside `pChain_pad_48` | **`pChainParent`** (`CDSView *`) |
| `+0x50` | pad | `dwChainField_50` |
| `+0x54` | `child_chain` | unchanged |
| `+0x84` | `pGamingHostScratch` (`pointer`) | **`pGaming_host`** (`CGaming *`) |

Size remains **412 (0x19C)** bytes.

## Remaining UNK / blockers

| Item | Notes |
|------|-------|
| `void *this` in decompiler | Ghidra MCP: `set_parameter_type` / `set_local_variable_type` cannot retype ECX auto-parameter; manual UI retype or future `set_function_this_type` plugin |
| `_Globals::` name prefix | Cosmetic namespace on decompiler label; function symbol is `CBulanek_ClampMoveRectByCollision` |
| Runtime `pChainParent` target | Expected playfield `CGaming` shell view; not re-traced from spawn this session |

## Frida

**none** — static disasm + struct layout sufficient.
