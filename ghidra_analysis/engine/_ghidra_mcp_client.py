#!/usr/bin/env python3
"""HTTP client for GhidraMCP plugin (bethington/ghidra-mcp).

The plugin at http://127.0.0.1:8089 returns:
  - JSON objects for most write tools (rename, set_function_this_type, save_program)
  - **raw plain text** for Response.text() endpoints (set_function_prototype, decompile, …)

Batch scripts must not json.loads() every response blindly.
"""
from __future__ import annotations

import json
import os
import urllib.error
import urllib.request

MCP_BASE = os.environ.get("GHIDRA_MCP_URL", "http://127.0.0.1:8089").rstrip("/")
PROGRAM = os.environ.get("GHIDRA_MCP_PROGRAM", "bulanci.exe")

# Endpoints that return Response.text (not JSON) from GhidraMCPPlugin.sendResponse
PLAIN_TEXT_PATHS = frozenset(
    {
        "/set_function_prototype",
        "/decompile_function",
        "/force_decompile",
        "/get_decompiled_code",
        "/disassemble_function",
        "/validate_function_prototype",
        "/batch_decompile",
    }
)


def post(path: str, body: dict, *, timeout: int = 120) -> tuple[bool, str]:
    """POST JSON to Ghidra HTTP plugin. Returns (success, message snippet)."""
    payload = {**body, "program": body.get("program") or PROGRAM}
    try:
        req = urllib.request.Request(
            f"{MCP_BASE}{path}",
            data=json.dumps(payload).encode(),
            headers={"Content-Type": "application/json"},
            method="POST",
        )
        with urllib.request.urlopen(req, timeout=timeout) as r:
            raw = r.read().decode("utf-8", errors="replace")
        return interpret_response(path, raw)
    except urllib.error.HTTPError as e:
        return False, e.read().decode("utf-8", errors="replace")[:500]
    except Exception as e:
        return False, str(e)[:500]


def get(path: str, params: dict | None = None, *, timeout: int = 60) -> tuple[bool, str]:
    try:
        url = f"{MCP_BASE}{path}"
        if params:
            q = {**params, "program": params.get("program") or PROGRAM}
            from urllib.parse import urlencode

            url = f"{url}?{urlencode(q)}"
        req = urllib.request.Request(url, method="GET")
        with urllib.request.urlopen(req, timeout=timeout) as r:
            raw = r.read().decode("utf-8", errors="replace")
        return interpret_response(path, raw)
    except Exception as e:
        return False, str(e)[:500]


def force_decompile(addr: str) -> tuple[bool, str]:
    return get("/force_decompile", {"address": addr}, timeout=90)


def interpret_response(path: str, raw: str) -> tuple[bool, str]:
    raw = raw.strip()
    if not raw:
        return False, "empty HTTP body"

    if path in PLAIN_TEXT_PATHS or path.endswith("_decompile") or not raw.startswith("{"):
        return success_from_plain_text(path, raw), raw[:500]

    try:
        data = json.loads(raw)
    except json.JSONDecodeError:
        return success_from_plain_text(path, raw), raw[:500]

    if isinstance(data, dict):
        if data.get("error"):
            return False, str(data["error"])[:500]
        if data.get("success") is False:
            return False, raw[:500]
        if data.get("status") == "error":
            return False, raw[:500]
        res = data.get("result", data)
        if isinstance(res, dict) and res.get("status") == "error":
            return False, raw[:500]
    return True, raw[:500]


def success_from_plain_text(path: str, text: str) -> bool:
    low = text.lower()
    if "failed" in low[:80] or low.startswith("error"):
        return False
    if '{"error"' in low:
        return False
    if "decompilation failed" in low or "function not found" in low:
        return False
    if "successfully" in low or "success:" in low or low.startswith("connected"):
        return True
    if "set comment" in low or "set plate comment" in low:
        return True
    if "decompile" in path or "disassemble" in path:
        return len(text.strip()) > 10
    if len(text) > 20 and "failed" not in low[:200]:
        return True
    return False


def set_prototype(
    addr: str,
    prototype: str,
    *,
    calling_convention: str = "",
    program: str = "",
) -> tuple[bool, str]:
    """Set signature; do not embed `this` — use set_function_this_type after."""
    body: dict = {
        "function_address": addr,
        "prototype": prototype,
    }
    if calling_convention:
        body["calling_convention"] = calling_convention
    if program:
        body["program"] = program
    return post("/set_function_prototype", body, timeout=60)


def check_connection() -> tuple[bool, str]:
    return get("/check_connection")
