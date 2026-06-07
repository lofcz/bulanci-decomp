# Spawn Round 10 deep-recovery agents (Cursor Task)

Replace `NN` (01–20). One background `Task` subagent per worker. **WRITE mode** — not `readonly`.

```
Round 10 deep recovery worker NN/20 for bulanci.exe — WRITE MODE.

Read first:
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\ROUND10_DEEP_PROTOCOL.md
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\agent_todos_20_r10_deep.json (task id=NN)
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\GHIDRA_MCP.md

IDA ref: c:\Users\mstagl-dev\Documents\GitHub\bulanci\bulanci.ida.exe.c
Ghidra decomp (may be wrong): c:\Users\mstagl-dev\Documents\GitHub\bulanci\bulanci.ghidra.exe.c

MCP: user-ghidra-mcp, program bulanci.exe. connect_instance("bulanci") first.
Read mcps/user-ghidra-mcp/tools/*.json before calls.

Rules: NEVER GUESS; trace xrefs; correlate IDA when decomp suspicious;
Frida under scripts/frida/ when static proof fails;
set_function_prototype (no explicit this) + set_function_this_type per GHIDRA_MCP.md;
prior ghidra_analysis docs may be outdated — re-verify live.

Output:
- ghidra_analysis/engine/deep_recovery/r10_deep_task_{NN}_report.md
- append one line to ghidra_analysis/engine/agent_todos_20_r10_deep_results.jsonl

Worker 20: coordinator only — no Ghidra; publish r10_deep_worker_20_report.md.
```
