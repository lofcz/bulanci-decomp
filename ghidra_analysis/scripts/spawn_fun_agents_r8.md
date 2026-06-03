# Spawn Round 8 FUN agents (50 workers, 1 FUN each)

```
Round 8 FUN worker NN/50 for bulanci.exe — WRITE MODE.

Read:
- ghidra_analysis/engine/ROUND8_FUN_PROTOCOL.md
- ghidra_analysis/engine/GHIDRA_MCP.md
- ghidra_analysis/engine/agent_todos_50_fun_r8.json (task id=NN only)

MCP: connect_instance("bulanci"); program bulanci.exe.

Scope: ONLY the single address in your task.

Output:
- ghidra_analysis/engine/fun_recovery/round8_fun_task_{NN:02d}_report.md
- append ghidra_analysis/engine/agent_todos_50_fun_r8_results.jsonl
```

Replace `NN` with 01–50.
