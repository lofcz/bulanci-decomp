#!/usr/bin/env python3
"""Apply deferred __thiscall prototypes (pass 3 failures) — correct GhidraMCP form."""
from __future__ import annotations

import json
import time
from pathlib import Path

from _ghidra_mcp_client import (
    check_connection,
    force_decompile,
    post,
    set_prototype,
)

# prototype string = return + name + params WITHOUT explicit `this`
# calling_convention separate; then set_function_this_type where needed
PROTO_THIS: list[tuple[str, str, str, str]] = [
    ("0x0042cf50", "void CDSView_OnLButtonDownAcquireFocus(void)", "__thiscall", "CDSView"),
    ("0x0042cf60", "void CDSView_OnMouseUpModalInputRefresh(void)", "__thiscall", "CDSView"),
    ("0x0042cfa0", "void CDSView_RefreshModalFocusFromChildren(void)", "__thiscall", "CDSView"),
    ("0x0042cff0", "void CDSView_TeardownModalFocusChain(void)", "__thiscall", "CDSView"),
    ("0x0042d040", "void CDSView__Hide(void)", "__thiscall", "CDSView"),
    ("0x0042d080", "void CDSView_SetAsDefaultFocusChild(void)", "__thiscall", "CDSView"),
    ("0x0042ccf0", "void CDSView_RenderChildrenClipped(void)", "__thiscall", "CDSView"),
]

PROTO_OTHER: list[tuple[str, str, str]] = [
    ("0x0042d3f0", "int CDsString_CompareHandles(void *param_1)", "__cdecl"),
    ("0x00434110", "uchar CDSStreamStorage_OpenStream(uint param_1, void *param_2)", "__thiscall"),
    ("0x00434140", "uchar CDSStreamStorage_GetStreamByIndex(int index, void *out)", "__thiscall"),
    ("0x00434380", "void CDSStreamStorage_AddRefHeldObject(void *param_1)", "__fastcall"),
]


def main() -> None:
    ok_conn, conn_msg = check_connection()
    if not ok_conn:
        print(f"Ghidra not reachable: {conn_msg}")
        print("Start Ghidra + Tools > GhidraMCP > Start MCP Server, then re-run.")
        return

    log: list[dict] = []
    ok = fail = 0

    for addr, proto, cc, cls in PROTO_THIS:
        good, msg = set_prototype(addr, proto, calling_convention=cc)
        log.append({"op": "prototype", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good
        if good and cls:
            g2, m2 = post(
                "/set_function_this_type",
                {"function_address": addr, "this_type": f"{cls} *"},
            )
            log.append({"op": "this_type", "addr": addr, "cls": cls, "ok": g2, "msg": m2})
            ok += g2
            fail += not g2
        time.sleep(0.1)

    for addr, proto, cc in PROTO_OTHER:
        good, msg = set_prototype(addr, proto, calling_convention=cc)
        log.append({"op": "prototype", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good
        time.sleep(0.1)

    for addr in [a for a, _, _, _ in PROTO_THIS] + [a for a, _, _ in PROTO_OTHER]:
        g, m = force_decompile(addr)
        log.append({"op": "force_decompile", "addr": addr, "ok": g, "msg": m})
        ok += g
        fail += not g

    g, m = post("/save_program", {})
    log.append({"op": "save_program", "ok": g, "msg": m})

    out = Path(__file__).resolve().parent / "logic_recovery" / "r6_prototypes_apply_log.json"
    out.write_text(json.dumps({"ok": ok, "fail": fail, "entries": log}, indent=2), encoding="utf-8")
    print(f"prototypes: ok={ok} fail={fail} save={g}")
    print(f"log -> {out}")


if __name__ == "__main__":
    main()
