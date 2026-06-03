#!/usr/bin/env python3
"""Revert 0x0041ad80 to Scheduler_EnqueueEvent (task 01/04/05 proof; task 06 rename was wrong)."""
import json
import urllib.request

MCP = "http://127.0.0.1:8089"
body = {
    "program": "bulanci.exe",
    "function_address": "0x0041ad80",
    "new_name": "Scheduler_EnqueueEvent",
}
req = urllib.request.Request(
    f"{MCP}/rename_function_by_address",
    data=json.dumps(body).encode(),
    headers={"Content-Type": "application/json"},
    method="POST",
)
with urllib.request.urlopen(req, timeout=60) as r:
    print(r.read().decode())
req2 = urllib.request.Request(
    f"{MCP}/save_program",
    data=json.dumps({"program": "bulanci.exe"}).encode(),
    headers={"Content-Type": "application/json"},
    method="POST",
)
with urllib.request.urlopen(req2, timeout=60) as r:
    print(r.read().decode())
