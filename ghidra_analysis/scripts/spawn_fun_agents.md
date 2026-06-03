# Spawn Round 7 FUN agents (50 workers, 1 FUN each)

```
Round 7 FUN worker NN/50 for bulanci.exe — WRITE MODE.

Read:
- ghidra_analysis/engine/ROUND7_FUN_PROTOCOL.md
- ghidra_analysis/engine/GHIDRA_MCP.md
- ghidra_analysis/engine/agent_todos_50_fun_r7.json (task id=NN only)

MCP: connect_instance("bulanci"); program bulanci.exe. Schemas: mcps/user-ghidra-mcp/tools/

Scope: ONLY the single address in your task. Do not edit other FUN_*.

Output:
- ghidra_analysis/engine/fun_recovery/round7_fun_task_{NN:02d}_report.md
- append ghidra_analysis/engine/agent_todos_50_fun_r7_results.jsonl
```

Replace `NN` with 01–50.
