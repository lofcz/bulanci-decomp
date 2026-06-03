# Round 6 — Ghidra batch apply (2026-06-03)

Parent-agent pass after workers 1–49 completed at **PARTIAL** (MCP had been down for most).

## Script

[`../_apply_r6_ghidra.py`](../_apply_r6_ghidra.py) — HTTP calls to `user-ghidra-mcp` @ `127.0.0.1:8089`.

## Results

| Metric | Count |
|--------|------:|
| MCP operations succeeded | 84 |
| Failed (HTTP parse / convention) | 3 |
| `save_program bulanci.exe` | **yes** (twice: batch + prototype fix pass) |

Full per-op log: [`r6_ghidra_apply_log.json`](./r6_ghidra_apply_log.json)

## Applied (high-confidence)

### Cluster / gaming (`0x41a`–`0x41b`)

- `CBulanek_StepMovementAndCollision` @ `0x0041af70`
- `CGaming_AddEntity` @ `0x0041a390` + `CGaming *` this + prototype
- `CGaming_SetEntityRegisterMode` @ `0x004168c0`
- `CGaming_OnPlayerCollectItem` @ `0x0041a020` + prototype
- `CGaming_CreateObject`, scheduler seed/enqueue helpers @ `0x0041b620` / `0x0041bab0`
- Comments @ `0x0041a3a6`, `0x0041b180` (MI adjustor)

### Scheduler / app / views (`0x42f`–`0x42d`)

- `CDSUpdatedItem *` on scheduler API band (`0x0042f1e0`–`0x0042eb30` incl. `DispatchDueEvents`)
- `Scheduler_PushHook`, resource-index release, `Runtime_ReallocOrThrow`
- `CDSIntrusiveNode_*` link walkers
- `CDSApp *` on frame/dirty-rect band; dirty-rect list renames
- `CDSView *` on layout/focus/modal slice; input-chain / show helpers renamed
- `CDSChain *` on chain list wrappers

### Sim misc

- CDSString path helpers, class-registry list walk, easy-mem guard, blit helpers

### Dispatch (codec) — pass 1 only

- `emit_byte_s` / `flush_bits_s` @ `0x004680f0` / `0x004681d0`

**Pass 2 (tasks 39–49):** [`r6_dispatch_codec_apply_summary.md`](./r6_dispatch_codec_apply_summary.md) — 64 OK, 8 inline labels.

**Pass 3 (sim deferred):** [`r6_pass3_apply_summary.md`](./r6_pass3_apply_summary.md) — modal/RECT/IDSStream/storage/blit/strings; 49 OK.

**Pass 4 (CDSString slots):** `void *` this on handle-slot APIs — [`r6_pass4_strings_log.json`](./r6_pass4_strings_log.json) 13/13 OK.

## Prototypes (applied)

[`../_apply_r6_prototypes.py`](../_apply_r6_prototypes.py) — **29/29 OK** (modal `CDSView` `__thiscall` + stream storage protos). Log: [`r6_prototypes_apply_log.json`](./r6_prototypes_apply_log.json).

Client + usage: [`../_ghidra_mcp_client.py`](../_ghidra_mcp_client.py), [`../GHIDRA_MCP.md`](../GHIDRA_MCP.md).

## Still deferred
- **`CreateObject` @ `0x0042ce30`** — classId 0x17 unnamed
- **Dispatch UNK** `FUN_004619e0` / `62980` / `61b70` / `63ee0` / zlib locals (tasks 45–47) — need disasm xref closure
- **IJG inline labels** (8 addresses) — parent-only symbols; see dispatch summary

## JSONL

[`../_backfill_r6_jsonl_v2.py`](../_backfill_r6_jsonl_v2.py) — 53 lines from reports + apply logs (authoritative reports unchanged).
