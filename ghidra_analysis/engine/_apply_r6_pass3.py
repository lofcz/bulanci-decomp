#!/usr/bin/env python3
"""Round 6 pass 3: deferred sim/cluster mutations from worker reports (evidence-only)."""
from __future__ import annotations

import json
from pathlib import Path

from _ghidra_mcp_client import force_decompile, post

RENAMES: list[tuple[str, str]] = [
    ("0x0041b190", "CAnim_OnSchedulerEnqueueSlot0"),
    ("0x00433280", "CDSRect_Intersect"),
    ("0x004339e0", "Eh_LeaveCriticalSection"),
    ("0x00434110", "CDSStreamStorage_OpenStream"),
    ("0x00434140", "CDSStreamStorage_GetStreamByIndex"),
    ("0x00434380", "CDSStreamStorage_AddRefHeldObject"),
    ("0x004344d0", "CDSResourceException_CreateObject"),
    ("0x00435db0", "SampleSourcePixel_Indexed8"),
    ("0x00435e20", "Blit_ReadDstPixel_32bpp"),
    ("0x00462f80", "process_restart"),
    ("0x0042cf60", "CDSView_OnMouseUpModalInputRefresh"),
    ("0x0042cfa0", "CDSView_RefreshModalFocusFromChildren"),
    ("0x0042cff0", "CDSView_TeardownModalFocusChain"),
    ("0x0042d3f0", "CDsString_CompareHandles"),
    ("0x0042d7a0", "CDsString_CompareHandles"),
]

THIS_TYPES: list[tuple[str, str]] = [
    ("0x0041b6d0", "CGaming"),
    ("0x0041ba60", "CGaming"),
    ("0x0041a140", "CGaming"),
    ("0x0041b500", "CGaming"),
    ("0x0042a210", "CBulanci"),
    ("0x0042f8b0", "CDSChain"),
    ("0x0042fae0", "CLevelScore"),
    ("0x00433200", "tagRECT"),
    ("0x00433280", "tagRECT"),
    ("0x00430490", "IDSStream"),
    ("0x00430500", "IDSStream"),
    ("0x004305c0", "IDSStream"),
    ("0x00430640", "IDSStream"),
]

# Use _apply_r6_prototypes.py for prototypes (__thiscall without explicit this)
PROTOTYPES: list[tuple[str, str]] = []

DECOMMENTS: list[tuple[str, str]] = [
    ("0x0042d1a0", "Modal exit code at +0x4a (not wChainCounter_4a); R6 task 18"),
    ("0x00433200", "ECX=dest RECT*; in-place intersect (pair with CDSRect_Intersect@0x33280)"),
    ("0x00433280", "Stack RECT* intersect; callers pass RECT not CPoemScroller (R6 task 31)"),
    ("0x00430490", "IDSStream vtable slot WriteBytes; filter outer face +0x0c (R6 task 25)"),
    ("0x00430a70", "CDSEasyMemStream readable guard: null backing -> stream errno 8"),
]

PLATE_COMMENTS: list[tuple[str, str]] = [
    (
        "0x0042cf60",
        "R6: mouse-up on modal view — clear clip focus DAT_004b3b94; vfn+0x1c hit-test; "
        "miss -> CDSView_UpdateInputChainOnMouseMove(g_pModalFocus)",
    ),
    (
        "0x0042cfa0",
        "R6: walk child chain for view with wViewStateFlags&8 then call OnMouseUpModalInputRefresh",
    ),
    (
        "0x0042cff0",
        "R6: hide/default-focus teardown when wViewStateFlags&8; callers CDSView__Hide, SetAsDefaultFocusChild",
    ),
]

FORCE_DECOMPILE = [
    "0x0042ccf0",
    "0x0042d0b0",
    "0x0042d1a0",
    "0x0042cff0",
    "0x0042d3f0",
    "0x00430490",
    "0x00430500",
    "0x00433200",
    "0x00433280",
    "0x00434110",
    "0x00434140",
]


def main() -> None:
    log: list[dict] = []
    ok = fail = 0

    def record(op: str, ok_flag: bool, msg: str, **extra: object) -> None:
        nonlocal ok, fail
        log.append({"op": op, "ok": ok_flag, "msg": msg, **extra})
        if ok_flag:
            ok += 1
        else:
            fail += 1

    for addr, name in RENAMES:
        good, msg = post(
            "/rename_function_by_address",
            {"function_address": addr, "new_name": name},
        )
        record("rename", good, msg, addr=addr, name=name)

    for addr, cls in THIS_TYPES:
        good, msg = post(
            "/set_function_this_type",
            {"function_address": addr, "this_type": f"{cls} *"},
        )
        record("this_type", good, msg, addr=addr, cls=cls)

    for addr, text in DECOMMENTS:
        good, msg = post("/set_decompiler_comment", {"address": addr, "comment": text})
        record("decompiler_comment", good, msg, addr=addr)

    for addr, text in PLATE_COMMENTS:
        good, msg = post("/set_plate_comment", {"address": addr, "comment": text})
        record("plate_comment", good, msg, addr=addr)

    for addr in FORCE_DECOMPILE:
        good, msg = force_decompile(addr)
        record("force_decompile", good, msg, addr=addr)

    good, msg = post("/save_program", {})
    record("save_program", good, msg)

    out = Path(__file__).resolve().parent / "logic_recovery" / "r6_pass3_apply_log.json"
    out.write_text(json.dumps({"ok": ok, "fail": fail, "entries": log}, indent=2), encoding="utf-8")
    print(f"pass3: ok={ok} fail={fail} save={good}")
    print(f"log -> {out}")


if __name__ == "__main__":
    main()
