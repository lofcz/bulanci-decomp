# Spawn naming batch agents (Cursor Task)

Replace `W` (wave number) and `NN` (01–50). One background `Task` subagent per batch.

```
Wave W Ghidra naming batch NN/50 for bulanci.exe.

Read first:
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\naming_batches_waveW\agent_instructions.md
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\naming_batches_waveW\batch_NN.json

Write results to:
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\naming_batches_waveW\batch_NN_results.json

If item_count is 0: still write results JSON with "entries": [] and note empty batch.

Rules: never guess; use xrefs/MCP; UNCERTAIN when not confident; save_program after Ghidra edits.
program: bulanci.exe. Read mcps/user-ghidra-mcp/tools/ schemas before MCP calls.
```

Wave 5 generator (re-queue still-unnamed FUN_* from prior waves):

    python ghidra_analysis/scripts/generate_naming_batches.py --wave 5 \
      --out-dir ghidra_analysis/naming_batches_wave5 \
      --exclude-glob "ghidra_analysis/naming_batches/batch_*.json" \
      --exclude-glob "ghidra_analysis/naming_batches_wave2/batch_*.json" \
      --exclude-glob "ghidra_analysis/naming_batches_wave3/batch_*.json" \
      --exclude-glob "ghidra_analysis/naming_batches_wave4/batch_*.json" \
      --exclude-only-if-named --func-source mapping --probe-mcp --vftable-quota 50
