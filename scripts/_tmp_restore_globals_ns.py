"""One-shot helper: send a Ghidra inline-script via MCP that reparents every
function listed in `_globals_addrs.txt` back into the `_Globals` namespace.

Used to undo a prior partial-promotion that left ~2000 functions at global
scope while their callers and `_Globals.cpp` body still expected them under
`_Globals::`.

Reads addresses from `_globals_addrs.txt` in the repo root.  Posts to the
MCP `/run_script_inline` endpoint.

Idempotent: a function that is already inside `_Globals` is left alone.
"""
import json
import sys
import urllib.parse
import urllib.request
from pathlib import Path

MCP_URL = "http://127.0.0.1:8089"
ADDR_FILE = Path(__file__).parent.parent / "_globals_addrs.txt"


def build_java(addrs: list[str]) -> str:
    addr_literals = ", ".join(f'0x{int(a, 16):x}L' for a in addrs)
    return f"""
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.SymbolTable;
import ghidra.program.model.address.Address;

public class McpInline extends GhidraScript {{
    @Override public void run() throws Exception {{
        long[] addrs = new long[] {{ {addr_literals} }};
        SymbolTable st = currentProgram.getSymbolTable();
        Namespace globalsNs = st.getNamespace("_Globals", currentProgram.getGlobalNamespace());
        if (globalsNs == null) {{
            println("ERROR: _Globals namespace not found");
            return;
        }}
        int moved = 0;
        int kept = 0;
        int missing = 0;
        for (long a : addrs) {{
            Address addr = currentAddress.getAddress(Long.toHexString(a));
            Function f = getFunctionAt(addr);
            if (f == null) {{ missing++; continue; }}
            Namespace cur = f.getParentNamespace();
            if (cur != null && cur.equals(globalsNs)) {{ kept++; continue; }}
            f.getSymbol().setNamespace(globalsNs);
            moved++;
        }}
        println("done: moved=" + moved + " kept=" + kept + " missing=" + missing);
    }}
}}
"""


def post_inline(code: str) -> str:
    url = f"{MCP_URL}/run_script_inline"
    body = json.dumps({"code": code}).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=body,
        headers={"Content-Type": "application/json", "Accept": "application/json"},
        method="POST",
    )
    with urllib.request.urlopen(req, timeout=300) as resp:
        return resp.read().decode("utf-8", errors="replace")


def main() -> int:
    addrs = [a.strip() for a in ADDR_FILE.read_text().splitlines() if a.strip()]
    print(f"Loaded {len(addrs)} addresses")
    code = build_java(addrs)
    print(f"Java source size: {len(code)} chars")
    out = post_inline(code)
    print(out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
