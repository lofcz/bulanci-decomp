# Round 6 — Pass 3 apply (sim/cluster deferred)

Script: [`../_apply_r6_pass3.py`](../_apply_r6_pass3.py)

## Results

| Metric | Value |
|--------|------:|
| OK | 49 |
| Fail | 11 (all `set_function_prototype` — MCP returns empty body; same as pass 1) |
| `save_program` | yes |

Log: [`r6_pass3_apply_log.json`](./r6_pass3_apply_log.json)

## Applied

- **Modal focus:** `CDSView_OnMouseUpModalInputRefresh`, `RefreshModalFocusFromChildren`, `TeardownModalFocusChain` + plate comments
- **RECT:** `CDSRect_Intersect` @ `0x00433280`; `tagRECT *` on `0x00433200` / `0x00433280`
- **Streams:** `IDSStream *` on filter Write/Seek/Lock/Unlock @ `0x00430490`–`0x00430640`
- **Storage:** `CDSStreamStorage_OpenStream` / `GetStreamByIndex` / `AddRefHeldObject`; `CDSResourceException_CreateObject`
- **Blit:** `SampleSourcePixel_Indexed8`, `Blit_ReadDstPixel_32bpp`
- **Anim/scheduler:** `CAnim_OnSchedulerEnqueueSlot0` @ `0x0041b190`
- **EH:** `Eh_LeaveCriticalSection` @ `0x004339e0`
- **Strings:** `CDsString_CompareHandles` @ `0x0042d3f0` / `0x0042d7a0`
- **Chain:** `CLevelScore *` / `CDSChain *` on list walkers; `CBulanci *` @ `0x0042a210`; `CGaming *` on BGM helpers

## Correction

`0x0041ad80` was briefly mis-renamed to `CGaming_SchedulerDispatch_AppendNode` (task 06 table error). **Reverted** to `Scheduler_EnqueueEvent` via [`../_fix_r6_scheduler_rename.py`](../_fix_r6_scheduler_rename.py) — matches tasks 01/04/05/07.

## Pass 4 (strings)

[`../_apply_r6_pass4_strings.py`](../_apply_r6_pass4_strings.py) — `void *` this on CDSString handle-slot APIs. Log: [`r6_pass4_strings_log.json`](./r6_pass4_strings_log.json)

## Still blocked

- `set_function_prototype` batch (MCP HTTP empty JSON) — use Ghidra UI or fix MCP plugin
- `CreateObject` @ `0x0042ce30` (classId 0x17)
- Dispatch-band UNK `FUN_*` without IJG/libmad symbol proof (tasks 45–47)
