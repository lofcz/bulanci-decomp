# Ghidra MCP — bulanci workflow

Repo: [bethington/ghidra-mcp](https://github.com/bethington/ghidra-mcp) @ `C:\Users\mstagl-dev\Documents\GitHub\ghidra-mcp`

Cursor exposes it as **`user-ghidra-mcp`** (stdio bridge → Ghidra HTTP plugin).

## Architecture

```
Cursor agent  →  bridge_mcp_ghidra.py (stdio MCP)  →  Ghidra plugin HTTP :8089  →  bulanci.exe
Batch scripts →  direct HTTP :8089 (same plugin, use _ghidra_mcp_client.py)
```

## One-time setup (Windows)

```text
cd C:\Users\mstagl-dev\Documents\GitHub\ghidra-mcp
scripts\local-setup.bat C:\path\to\ghidra_12.1_PUBLIC
```

In **CodeBrowser** with `bulanci.exe` open:

1. **File → Configure → Configure All Plugins** — enable **GhidraMCP**
2. **Tools → GhidraMCP → Start MCP Server** (default `http://127.0.0.1:8089/`)
3. **Analysis → Auto Analyze** if the binary is fresh

Health check:

```text
curl http://127.0.0.1:8089/check_connection
```

## Cursor (agents)

1. MCP server `user-ghidra-mcp` enabled in Cursor settings.
2. First call **`connect_instance`** with project substring (e.g. `bulanci`) so the bridge registers all ~250 tools.
3. Pass **`program`: `bulanci.exe`** on mutating calls when multiple programs are open.
4. Read tool schemas under `.cursor/projects/.../mcps/user-ghidra-mcp/tools/*.json` before calling.

Typical mutation order (member functions):

1. `set_function_prototype` — signature **without** explicit `this` (see below)
2. `set_function_this_type` — e.g. `CDSView *` (struct must exist in DT manager)
3. `rename_function_by_address`
4. `set_decompiler_comment` / `set_plate_comment`
5. `force_decompile` to refresh
6. `save_program` once per batch

### Parameter names (common mistakes)

| Tool | Address parameter |
|------|-------------------|
| `rename_function_by_address`, `set_function_prototype`, `set_function_this_type` | `function_address` |
| `force_decompile`, `decompile_function` | `address` (HTTP **GET**, not POST) |
| `set_decompiler_comment` | `address` |

Upstream doc: `ghidra-mcp/docs/THIS_POINTER_TYPING.md`

## HTTP batch scripts (this repo)

Use the shared client — **do not** `json.loads()` every response:

- `ghidra_analysis/engine/_ghidra_mcp_client.py`

```python
from _ghidra_mcp_client import post, set_prototype, PROGRAM, check_connection

ok, msg = post("/rename_function_by_address", {
    "function_address": "0x0041a390",
    "new_name": "CGaming_AddEntity",
})
ok, msg = set_prototype("0x0042cf50", "void CDSView_OnLButtonDownAcquireFocus(void)", calling_convention="__thiscall")
ok, msg = post("/set_function_this_type", {"function_address": "0x0042cf50", "this_type": "CDSView *"})
```

### Why `set_function_prototype` “failed” in older apply logs

The plugin returns **plain text** (`Successfully set prototype…`), not JSON. Scripts that only parsed JSON reported `Expecting value: line 1 column 1`. The client treats plain-text success correctly.

### Prototype rules (32-bit PE / `__thiscall`)

| Do | Don't |
|----|--------|
| `void Foo(void)` + `calling_convention: __thiscall` | `void __thiscall Foo(CDSView *this)` in one string |
| Then `set_function_this_type` with `CDSView *` | `set_local_variable_type` on auto-`this` (immutable) |
| `rename_function_by_address` for the real name | Rely on prototype string to rename |

String handle slots (not `CBulanci*`): use `void *` as `this_type` or decompiler comments; see task 19 reports.

## Environment variables

| Variable | Default |
|----------|---------|
| `GHIDRA_MCP_URL` | `http://127.0.0.1:8089` |
| `GHIDRA_MCP_PROGRAM` | `bulanci.exe` |

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| `Not connected` (Cursor) | Ghidra running, server started, `connect_instance("bulanci")` |
| Connection refused | Start MCP server; check port in **Edit → Tool Options → GhidraMCP HTTP Server** |
| Rename warnings (PascalCase) | Warnings only; rename still applied |
| `set_function_this_type` fails | Create struct first (`create_struct`); set `__thiscall` first |
| MCP disconnect mid-batch | Re-run apply script; ops are mostly idempotent |

## Apply scripts (Round 6)

| Script | Purpose |
|--------|---------|
| `_apply_r6_ghidra.py` | Pass 1 cluster/sim renames + this types |
| `_apply_r6_dispatch_codec.py` | IJG/libmad dispatch band |
| `_apply_r6_pass3.py` | Modal, RECT, IDSStream, storage |
| `_apply_r6_pass4_strings.py` | CDSString handle `void *` this |
| `_apply_r6_prototypes.py` | Deferred `__thiscall` prototypes (correct form) |

Logs: `ghidra_analysis/engine/logic_recovery/r6_*_apply_log.json`

## References

- `ghidra_analysis/engine/ROUND6_LOGIC_PROTOCOL.md` — agent rules
- `ghidra-mcp/README.md` — full upstream install
- `scripts/ghidra_to_decomp_goal.py` — read-only decompile pull
