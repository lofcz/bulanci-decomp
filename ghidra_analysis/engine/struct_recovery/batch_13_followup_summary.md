# Struct recovery — batch 13 follow-up (round 2)



**Prior:** `batch_13_summary.md`  

**Structs:** `CMsgDialog`, `CPauseDlg`  

**Program:** `bulanci.exe`  

**Status:** **FOLLOWUP_DONE** (PARTIAL layouts remain)



## Inputs read



- `batch_13_summary.md`

- `CMsgDialog.md`, `CPauseDlg.md`

- `AGENT_PROTOCOL.md`, `FOLLOWUP_ROUND_PROTOCOL.md`



## Follow-up items addressed



### 1. `CDSChained` / pad `0x40–0x4b` (all `CWindow` dialogs)



- Decompiled `CDSChained_ResetChainCounters@0x0042beb0` (called from `CDSChained_ctor@0x004032d0` / `CWindow_BuildAt`).

- Proven band `0x40–0x50` on dialog shells:



| Offset | Name (doc) | Evidence |

|--------|------------|----------|

| `0x40` | `dwChainHead_40` | `*(this+0x40)=0` |

| `0x44` | `wViewStateFlags` | `=1`; `CPauseDlg_OnNotify` tests low byte bit 0 |

| `0x46` | `wWidgetFlags` | `=0`; `CWindow_BuildAt` `\|= 1`; dialog ctors OR extra bits |

| `0x48` / `0x4a` | chain counters | ctor `=0` |

| `0x4c` | `pParent` | ctor `=0`; `CPauseDlg_OnCommand` / `OnNotify` consumers |

| `0x50` | `dwChainField_50` | `*(this+0x50)=0` |



- Updated `CMsgDialog.md` and `CPauseDlg.md` layout tables.



### 2. `pGame` as `CGame *`



- **Deferred:** Ghidra `CGame` is still **872 B** vs embedded span **`0x248`** (`CBulanci.md`). Tail field remains `void *` in Ghidra; docs note semantic `CGame*` with xref proof at `+0x70`.



### 3. Heap `CPauseDlg` vs `CBulanci` embed



- **Resolved:** `CPauseDlg_Build@0x00411df0` sole code xref = `CGaming_ctor@0x00420423` → `OperatorNew(0x7c)` → `Build` → stored **`CGaming+0x334`**, `CDSView__AddChild`.

- **Not** a `CBulanci` subobject; export name `CBulanci::CPauseDlg_Build` is a Ghidra mis-label.

- Separate factory path: `CPauseDlg_Allocate@0x0040f090` (DATA xref class registration only).



## Ghidra deltas



- Re-created **`CPauseDlg`** (was missing from DB): `get_struct_layout` → **124 B** (`0x7c`).

- **`CMsgDialog`**: added `pad_40`, `wFlags1`, `wFlags2`, `wExitCode` (chain band alignment).

- `set_function_prototype` on `CPauseDlg_Build@0x00411df0` → `CPauseDlg * __thiscall CPauseDlg_Build(CPauseDlg *this, CGame *pGame)` (agent todo 14); plate comment + `CGaming_ctor@0x00420423` EOL note; decompiler ECX `this` may stay `CBulanci*` per MCP `__thiscall` limit.

- `save_program bulanci.exe` (once).



## Remaining UNK



- `wChainCounter_48` / `wChainCounter_4a` — ctor-zeroed only.

- `dwChainHead_40` / `dwChainField_50` runtime semantics beyond reset.

- `pGame` Ghidra type until `CGame` batch recovers `0x248` layout.

- `CMsgDialog` class factory id at `CMsgDialog_GetClassTable@0x0040b760`.



## Struct status after follow-up



| Struct | Status | Size |

|--------|--------|------|

| `CMsgDialog` | PARTIAL (richer base) | `0x70` |

| `CPauseDlg` | PARTIAL | `0x7c` |


