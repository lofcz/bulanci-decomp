# Spawn type-recovery batch agents (Cursor Task)

Replace `W` (wave number) and `NN` (01–50). One background `Task` subagent per batch.

```
Wave W Ghidra type recovery batch NN/50 for bulanci.exe.

Read first:
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\type_recovery_waveW\agent_instructions.md
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\type_recovery_waveW\batch_NN.json

Write results to:
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\type_recovery_waveW\batch_NN_results.json

If item_count is 0: still write results JSON with "entries": [] and note empty batch.

Rules: evidence-only types; create_struct / set_function_prototype when confident;
UNCERTAIN comments otherwise; save_program once per batch.
program: bulanci.exe. Read mcps/user-ghidra-mcp/tools/ schemas before MCP calls.
```

## Wave 1 generator

From repo root:

```bash
python ghidra_analysis/scripts/generate_type_recovery_batches.py --wave 1 --game-only --min-size 48 --max-items 500 --probe-mcp
```

Outputs: `ghidra_analysis/type_recovery_wave1/` (`batch_01.json` … `batch_50.json`, `summary.json`, `agent_instructions.md`, `metrics_baseline.json`).

Regenerate wave 2 excluding wave 1 addresses:

```bash
python ghidra_analysis/scripts/generate_type_recovery_batches.py --wave 2 --game-only --min-size 48 --max-items 500 \
  --exclude-glob "ghidra_analysis/type_recovery_wave1/batch_*.json" --probe-mcp
```
