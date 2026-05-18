"""Generate config/bulanci/mapping.csv from a running Ghidra MCP server.

The output format is byte-identical to what
scripts/ghidra/GenerateMapping.java would write if you ran it via Ghidra
headless: one CSV row per function, fields separated by `;`:

    <mangled_name>;<qualified_name>;0x<addr>;0x<size>;<convention>;<varargs>;<rettype>;<arg1>;<arg2>;...

Empty fields are blank. Conventions look like __thiscall, __cdecl,
__stdcall, __fastcall. `varargs` is the literal string "varargs" or empty.
Types use the same Ghidra-normalised names as GenerateMapping.java
(uchar/ushort/uint, with pointer suffix `*`).

Requires the Ghidra MCP plugin to be running. Talks to it over plain HTTP
on http://127.0.0.1:8089 (override with --url).
"""
from __future__ import annotations

import argparse
import concurrent.futures
import json
import os
import re
import sys
import time
import urllib.parse
import urllib.request
from pathlib import Path
from typing import Iterable

DEFAULT_URL = os.environ.get("GHIDRA_MCP_URL", "http://127.0.0.1:8089")
DEFAULT_PROGRAM = os.environ.get("GHIDRA_MCP_PROGRAM", "bulanci.exe")
DEFAULT_OUTPUT = Path(__file__).parent.parent.parent / "config" / "bulanci" / "mapping.csv"
DEFAULT_WORKERS = 16

# Drop these compiler / Ghidra synthetic entries entirely. They never end up
# in a real translation unit and just pollute the unit listing.
SKIP_PREFIXES = ("switchD_", "switchdataD_", "caseD_")


def http_get(base_url: str, path: str, params: dict | None = None, timeout: float = 60.0) -> str:
    qs = ""
    if params:
        qs = "?" + urllib.parse.urlencode(params)
    url = f"{base_url.rstrip('/')}{path}{qs}"
    req = urllib.request.Request(url, headers={"Accept": "application/json"})
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return resp.read().decode("utf-8", errors="replace")


def http_get_json(base_url: str, path: str, params: dict | None = None, timeout: float = 60.0) -> dict:
    body = http_get(base_url, path, params, timeout)
    return json.loads(body)


# --- type normalisation, mirroring GenerateMapping.java ---------------------

_TYPE_NORMALISE = {
    "i8": "char",
    "undefined": "uchar",
    "undefined1": "uchar",
    "byte": "uchar",
    "u8": "uchar",
    "i16": "short",
    "undefined2": "ushort",
    "u16": "ushort",
    "i32": "int",
    "undefined4": "uint",
    "u32": "uint",
    "f32": "float",
    "f64": "double",
}

_PRIMITIVES = {
    "bool", "char", "uchar", "short", "ushort", "int", "uint",
    "long", "ulong", "float", "double", "void",
}


def _normalise_type(ty: str) -> str:
    ty = ty.strip()
    if not ty:
        return "void"
    # peel off pointer levels
    pointer_suffix = ""
    while ty.endswith("*"):
        ty = ty[:-1].rstrip()
        pointer_suffix += "*"
    if ty in _TYPE_NORMALISE:
        ty = _TYPE_NORMALISE[ty]
    elif ty in _PRIMITIVES:
        pass
    return ty + pointer_suffix


# --- signature parsing ------------------------------------------------------

# Ghidra prints signatures like:
#   void __thiscall CFoo::bar(CFoo * this, int x)
#   undefined * __stdcall Catch@00401b70(void)
#   undefined CFoo::baz()
_CONVENTIONS = ("__thiscall", "__cdecl", "__stdcall", "__fastcall", "__vectorcall", "__regcall")


def _split_args(args_blob: str) -> list[str]:
    """Split a signature parameter list while respecting nested templates."""
    args_blob = args_blob.strip()
    if not args_blob or args_blob == "void":
        return []
    parts: list[str] = []
    depth = 0
    cur = ""
    for ch in args_blob:
        if ch == "<":
            depth += 1
        elif ch == ">":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        parts.append(cur.strip())
    return parts


def _split_param_type(param: str) -> str:
    """Drop the parameter name from `int param_1` -> `int`, keeping pointers."""
    param = param.strip()
    if not param:
        return ""
    # If it ends with `*` it's all type, no name (Ghidra sometimes emits this).
    if param.endswith("*"):
        return param.replace(" ", "")
    tokens = param.split()
    if len(tokens) == 1:
        return tokens[0]
    # Drop trailing identifier (the param name)
    type_tokens = tokens[:-1]
    return " ".join(type_tokens).replace(" *", "*")


def parse_signature(sig: str) -> tuple[str, str, str, bool, list[str]]:
    """Parse a Ghidra signature line.

    Returns (qualified_name, calling_convention, return_type, has_varargs, args)
    """
    if not sig:
        return "", "", "void", False, []
    # Strip trailing semicolons that Ghidra sometimes appends.
    sig = sig.strip().rstrip(";").strip()

    # Pull the parameter list out first.
    open_idx = sig.find("(")
    close_idx = sig.rfind(")")
    if open_idx < 0 or close_idx < 0 or close_idx < open_idx:
        # No parens at all -- defensive fallback.
        return sig.strip(), "", "void", False, []

    args_blob = sig[open_idx + 1 : close_idx]
    head = sig[:open_idx].strip()

    has_varargs = False
    raw_args = _split_args(args_blob)
    if raw_args and raw_args[-1] == "...":
        has_varargs = True
        raw_args = raw_args[:-1]
    args = [_normalise_type(_split_param_type(a)) for a in raw_args if _split_param_type(a)]

    # Split head: `<return_type> [convention] <qualified_name>`
    tokens = head.split()
    convention = ""
    for conv in _CONVENTIONS:
        if conv in tokens:
            convention = conv
            tokens.remove(conv)
            break

    if not tokens:
        return "", convention, "void", has_varargs, args

    qualified_name = tokens[-1]
    return_type_tokens = tokens[:-1]
    return_type = " ".join(return_type_tokens).replace(" *", "*") if return_type_tokens else "void"
    return_type = _normalise_type(return_type or "void")

    return qualified_name, convention, return_type, has_varargs, args


# --- get_function_by_address text parsing -----------------------------------

_FUNC_BY_ADDR_RE = re.compile(
    r"^Function:\s*(?P<name>.*?)\s*at\s*(?P<entry_a>[0-9a-fA-F]+)\s*\n"
    r"Signature:\s*(?P<sig>.*?)\s*\n"
    r"Entry:\s*(?P<entry_b>[0-9a-fA-F]+)\s*\n"
    r"Body:\s*(?P<min>[0-9a-fA-F]+)\s*-\s*(?P<max>[0-9a-fA-F]+)",
    re.MULTILINE,
)


def parse_function_text(body: str) -> dict | None:
    m = _FUNC_BY_ADDR_RE.search(body)
    if not m:
        return None
    body_min = int(m.group("min"), 16)
    body_max = int(m.group("max"), 16)
    return {
        "name": m.group("name").strip(),
        "entry": int(m.group("entry_a"), 16),
        "signature": m.group("sig").strip(),
        "body_min": body_min,
        "body_max": body_max,
        "size": body_max - body_min + 1,
    }


# --- main pipeline ----------------------------------------------------------

def list_all_functions(base_url: str, program: str) -> list[dict]:
    page_size = 5000
    out: list[dict] = []
    offset = 0
    while True:
        data = http_get_json(
            base_url, "/list_functions_enhanced",
            {"program": program, "offset": offset, "limit": page_size},
        )
        page = data.get("functions") or []
        out.extend(page)
        if len(page) < page_size:
            break
        offset += page_size
    return out


def fetch_function_record(base_url: str, program: str, addr: str) -> dict | None:
    body = http_get(base_url, "/get_function_by_address", {"program": program, "address": addr}, timeout=30.0)
    if body.startswith("No function found") or body.startswith("Error"):
        return None
    return parse_function_text(body)


def format_csv_row(name: str, addr: int, size: int, sig: str) -> str | None:
    qualified, conv, rettype, varargs, args = parse_signature(sig)
    # GenerateMapping.java prefers the qualified name from the signature when
    # present; fall back to the short Ghidra name otherwise.
    qualified_name = qualified or name
    parts: list[str] = [
        "",                      # mangled_name (not exposed via MCP)
        qualified_name,
        f"0x{addr:x}",
        f"0x{size:x}",
        conv,
        "varargs" if varargs else "",
        rettype,
    ]
    parts.extend(args)
    return ";".join(parts)


def export(base_url: str, program: str, output: Path, workers: int, limit: int | None) -> int:
    print(f"[mcp] fetching function list from {base_url} (program={program})", file=sys.stderr)
    funcs = list_all_functions(base_url, program)
    funcs = [f for f in funcs if not f.get("isThunk") and not f.get("isExternal")]
    funcs = [f for f in funcs if not any(f.get("name", "").startswith(p) for p in SKIP_PREFIXES)]
    if limit:
        funcs = funcs[:limit]
    print(f"[mcp] {len(funcs)} candidate functions; pulling per-function details with {workers} workers", file=sys.stderr)

    rows: list[tuple[int, str]] = []  # (sort_key, csv_line)
    started = time.time()

    def _fetch(idx_and_func):
        idx, f = idx_and_func
        addr = f.get("address") or ""
        try:
            rec = fetch_function_record(base_url, program, f"0x{addr}")
        except Exception as e:
            print(f"[mcp] failed {addr}: {e}", file=sys.stderr)
            return idx, None
        if rec is None:
            return idx, None
        line = format_csv_row(rec["name"], rec["entry"], rec["size"], rec["signature"])
        return idx, line

    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as ex:
        for idx, line in ex.map(_fetch, list(enumerate(funcs))):
            if line is not None:
                rows.append((idx, line))
            if idx % 500 == 0 and idx > 0:
                elapsed = time.time() - started
                print(f"[mcp] progress {idx}/{len(funcs)} ({elapsed:.1f}s)", file=sys.stderr)

    rows.sort(key=lambda x: x[0])

    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("w", encoding="utf-8", newline="\n") as out:
        for _, line in rows:
            out.write(line + "\n")

    elapsed = time.time() - started
    print(f"[mcp] wrote {output} ({len(rows)} rows in {elapsed:.1f}s)", file=sys.stderr)
    return len(rows)


def main(argv: Iterable[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--url", default=DEFAULT_URL, help="Ghidra MCP HTTP base URL")
    parser.add_argument("--program", default=DEFAULT_PROGRAM, help="Open Ghidra program to dump")
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT), help="Where to write mapping.csv")
    parser.add_argument("--workers", type=int, default=DEFAULT_WORKERS, help="Concurrent HTTP workers")
    parser.add_argument("--limit", type=int, default=None, help="Stop after N functions (debugging)")
    args = parser.parse_args(argv)

    output = Path(args.output)
    n = export(args.url, args.program, output, args.workers, args.limit)
    print(f"wrote {n} mapping rows to {output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
