# Spawn Round 11 AI recovery agents (Cursor Task)

Replace `NN` (01–30). One background `Task` subagent per worker. **WRITE mode** — not `readonly`.

```
Round 11 AI behavior worker NN/30 for bulanci.exe — WRITE MODE.

Read first:
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\ROUND11_AI_PROTOCOL.md
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\agent_todos_30_r11_ai.json (task id=NN)
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\gameplay\map_slots_spawner.md
- c:\Users\mstagl-dev\Documents\GitHub\bulanci\ghidra_analysis\engine\GHIDRA_MCP.md

IDA: bulanci/bulanci.ida.exe.c | Ghidra decomp: bulanci/bulanci.ghidra.exe.c

MCP: user-ghidra-mcp, connect_instance("bulanci"), program bulanci.exe.

Rules: NEVER GUESS; trace xrefs; document exact algorithms for 1:1 reimplementation;
Frida under scripts/frida/ when static proof fails;
fix Ghidra decomp vs IDA when proven.

Output:
- ghidra_analysis/engine/ai_recovery/r11_ai_task_{NN}_report.md
- append line to ghidra_analysis/engine/agent_todos_30_r11_ai_results.jsonl

Worker 30: coordinator only — r11_ai_worker_30_report.md, no Ghidra mutations.
```
