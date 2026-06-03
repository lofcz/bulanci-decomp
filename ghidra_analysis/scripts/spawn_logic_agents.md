# Spawn logic/internals batch agents (Cursor Task)

Replace `NN` (01–50). One background `Task` subagent per worker. **WRITE mode** — not `readonly`.

```
Round 6 logic internals worker NN/50 for bulanci.exe — WRITE MODE.

Read first:
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\ROUND6_LOGIC_PROTOCOL.md
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\agent_todos_50_r6_logic.json (task id=NN)

MCP: user-ghidra-mcp, program bulanci.exe. Read ghidra_analysis/engine/GHIDRA_MCP.md + mcps/user-ghidra-mcp/tools/*.json before calls. connect_instance("bulanci") first.

Rules: NEVER GUESS; Frida verify scripts under scripts/frida/ when static proof fails;
set_function_prototype (no explicit this) + set_function_this_type for wrong ECX/this (GHIDRA_MCP.md);
Batch HTTP: engine/_ghidra_mcp_client.py — not raw json.loads on every response.
ghidra_analysis prior docs may be outdated — re-verify.

Output:
- ghidra_analysis/engine/logic_recovery/round6_logic_task_{NN}_report.md
- append one line to ghidra_analysis/engine/agent_todos_50_r6_logic_results.jsonl

Worker 50: coordinator only — no Ghidra; publish checklist + round6_logic_worker_50_report.md.
```
