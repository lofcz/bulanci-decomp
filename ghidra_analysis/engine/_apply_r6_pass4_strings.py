#!/usr/bin/env python3
"""Pass 4: CDSString handle-slot this typing (wchar_t** / void*), task 19."""
from __future__ import annotations

import json
from pathlib import Path

from _ghidra_mcp_client import post

# ECX = address of wchar_t* handle slot (not CBulanci*)
THIS_VOID = [
    "0x0042d510",
    "0x0042d7b0",
    "0x0042d7f0",
    "0x0042d8c0",
    "0x0042d970",
    "0x0042dab0",
    "0x0042dc50",
    "0x0042dc70",
    "0x0042dec0",
    "0x0042dfc0",
]

COMMENTS = [
    ("0x0042d510", "ECX = wchar_t** handle slot (CDSString); not CBulanci* (R6 task 19)"),
    ("0x0042d970", "Temp path into caller handle slot via GetTempPathW (R6 task 19)"),
    ("0x0042dab0", "MultiByteToWideChar into CDSString handle (R6 task 19)"),
]


def main() -> None:
    log = []
    ok = fail = 0
    for addr in THIS_VOID:
        good, msg = post(
            "/set_function_this_type",
            {"function_address": addr, "this_type": "void *"},
        )
        log.append({"op": "this_type_void", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good
    for addr, text in COMMENTS:
        good, msg = post("/set_decompiler_comment", {"address": addr, "comment": text})
        log.append({"op": "comment", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good
    good, msg = post("/save_program", {})
    log.append({"op": "save_program", "ok": good, "msg": msg})
    out = Path(__file__).resolve().parent / "logic_recovery" / "r6_pass4_strings_log.json"
    out.write_text(json.dumps({"ok": ok, "fail": fail, "entries": log}, indent=2), encoding="utf-8")
    print(f"pass4 strings: ok={ok} fail={fail}")


if __name__ == "__main__":
    main()
