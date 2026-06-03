#!/usr/bin/env python3
"""Apply high-confidence Round 6 queued Ghidra mutations via GhidraMCP HTTP API."""
from __future__ import annotations

import json
from pathlib import Path

from _ghidra_mcp_client import post

RENAMES: list[tuple[str, str]] = [
    ("0x0041af70", "CBulanek_StepMovementAndCollision"),
    ("0x0041a390", "CGaming_AddEntity"),
    ("0x004168c0", "CGaming_SetEntityRegisterMode"),
    ("0x0041a020", "CGaming_OnPlayerCollectItem"),
    ("0x0041be70", "CGaming_CreateObject"),
    ("0x0041b620", "CGaming_SeedMatchSchedulerEvents"),
    ("0x0041bab0", "CGaming_EnqueuePreMatchSchedulerSlots"),
    ("0x0042ae50", "CDSApp_DirtyRectList_UpsertRect"),
    ("0x0042ae40", "CDSApp_DirtyRectList_Clear"),
    ("0x0042f7d0", "CDSIntrusiveNode_GetLinkNext"),
    ("0x0042f7e0", "CDSIntrusiveNode_GetNextLinkSkipSentinel"),
    ("0x0042f620", "Scheduler_PushHook"),
    ("0x0042f530", "CBulanci_ReleaseResourceIndexSlots"),
    ("0x0042f730", "Runtime_ReallocOrThrow"),
    ("0x00433200", "CDSRect_IntersectInPlace"),
    ("0x0042c540", "CDSView_AdjustAnchoredEdge"),
    ("0x0042c700", "CDSApp_RefreshInputChainHitTest"),
    ("0x0042c880", "CDSView_UpdateInputChainOnMouseMove"),
    ("0x0042c960", "CDSView__Show_UpdateFocusChain"),
    ("0x0042cc30", "CDSView_OffsetRectAndAdapt"),
    ("0x0042cbb0", "CScroller_OffsetChildViewRect"),
    ("0x0042e960", "ClassRegEntry_ListContains"),
    ("0x0042e2f0", "CDsString_ReadNarrowLengthPrefixedFromStream"),
    ("0x0042e400", "CDsString_WriteNarrowLengthPrefixedToStream"),
    ("0x00430a70", "CDSEasyMemStream_GuardReadable"),
    ("0x00436760", "CPoemScroller_ClearBlitMask"),
    ("0x00436d90", "CDSImage_BindFromSurfaceDesc"),
    ("0x00429f70", "CGaming_SyncKeyLatchAfterModal"),
    ("0x0042dab0", "CDsString_AssignFromMultiByte"),
    ("0x0042db60", "CDsString_WideToMultiByteBuffer"),
    ("0x0042dc50", "CDsString_ClearInPlace"),
    ("0x0042dec0", "CBulanci_SplitPathDirectoryAndBase"),
    ("0x0042dfc0", "CBulanci_JoinPathDirectoryAndBase"),
    ("0x0042d970", "CBulanci_AssignTempPathWithTrailingBackslash"),
    ("0x004680f0", "emit_byte_s"),
    ("0x004681d0", "flush_bits_s"),
]

THIS_TYPES: list[tuple[str, str]] = [
    ("0x0041a390", "CGaming"),
    ("0x004168c0", "CGaming"),
    ("0x004184a0", "CGaming"),
    ("0x0041a020", "CGaming"),
    ("0x0041a140", "CGaming"),
    ("0x0041b500", "CGaming"),
    ("0x0041bab0", "CGaming"),
    ("0x0041b620", "CGaming"),
    ("0x0042f1e0", "CDSUpdatedItem"),
    ("0x0042f210", "CDSUpdatedItem"),
    ("0x0042f290", "CDSUpdatedItem"),
    ("0x0042f2d0", "CDSUpdatedItem"),
    ("0x0042f300", "CDSUpdatedItem"),
    ("0x0042f330", "CDSUpdatedItem"),
    ("0x0042eaa0", "CDSUpdatedItem"),
    ("0x0042eac0", "CDSUpdatedItem"),
    ("0x0042eb00", "CDSUpdatedItem"),
    ("0x0042ebb0", "CDSUpdatedItem"),
    ("0x0042eb30", "CDSUpdatedItem"),
    ("0x0042b5a0", "CDSApp"),
    ("0x0042b8a0", "CDSApp"),
    ("0x0042bc00", "CDSApp"),
    ("0x0042bda0", "CDSApp"),
    ("0x0042be60", "CDSApp"),
    ("0x0042bbe0", "CDSApp"),
    ("0x0042c430", "CDSView"),
    ("0x0042c480", "CDSView"),
    ("0x0042c580", "CDSView"),
    ("0x0042c770", "CDSView"),
    ("0x0042c7d0", "CDSView"),
    ("0x0042cae0", "CDSView"),
    ("0x0042cbf0", "CDSView"),
    ("0x0042cc80", "CDSView"),
    ("0x0042ccf0", "CDSView"),
    ("0x0042cea0", "CDSView"),
    ("0x0042cf50", "CDSView"),
    ("0x0042d1a0", "CDSView"),
    ("0x0042f880", "CDSChain"),
    ("0x0042f890", "CDSChain"),
    ("0x0042fa20", "CDSChain"),
    ("0x0042fa50", "CDSChain"),
    ("0x00436e40", "CDSImage"),
    ("0x00436ef0", "CDSImage"),
    ("0x0041bf70", "CWeapon"),
    ("0x00429db0", "CDSApp"),
]

PROTOTYPES: list[tuple[str, str]] = [
    (
        "0x0041a390",
        "void __thiscall CGaming_AddEntity(CGaming *this, void *entity, char register_flag)",
    ),
    (
        "0x0041a020",
        "void __thiscall CGaming_OnPlayerCollectItem(CGaming *this, void *pPickup, void *pCollector, uint weaponKind)",
    ),
    (
        "0x0042eb30",
        "void __fastcall Scheduler_DispatchDueEvents(CDSUpdatedItem *this)",
    ),
]

COMMENTS: list[tuple[str, str]] = [
    ("0x0041a3a6", "entity+0x84 = CGaming* pGaming_host (R6 task 6)"),
    ("0x0041b180", "MI adjustor: ECX -= 0xA0 -> CBulanek*; delegates to movement step (R6 task 3)"),
    ("0x0042aca0", "CDSImageMouse_ScalarDeletingDtor_thunk — NOT CDSChain_AdjustThisOffset (R6 task 14)"),
]


def main() -> None:
    log: list[dict] = []
    ok = fail = 0

    for addr, name in RENAMES:
        good, msg = post("/rename_function_by_address", {"function_address": addr, "new_name": name})
        log.append({"op": "rename", "addr": addr, "name": name, "ok": good, "msg": msg})
        ok += good
        fail += not good

    for addr, cls in THIS_TYPES:
        good, msg = post(
            "/set_function_this_type",
            {"function_address": addr, "this_type": f"{cls} *"},
        )
        log.append({"op": "this_type", "addr": addr, "cls": cls, "ok": good, "msg": msg})
        ok += good
        fail += not good

    for addr, proto in PROTOTYPES:
        good, msg = post(
            "/set_function_prototype",
            {"function_address": addr, "prototype": proto},
        )
        log.append({"op": "prototype", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good

    for addr, text in COMMENTS:
        good, msg = post(
            "/set_decompiler_comment",
            {"address": addr, "comment": text},
        )
        log.append({"op": "comment", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good

    good, msg = post("/save_program", {})
    log.append({"op": "save_program", "ok": good, "msg": msg})

    out = Path(__file__).resolve().parent / "logic_recovery" / "r6_ghidra_apply_log.json"
    out.write_text(json.dumps({"ok": ok, "fail": fail, "entries": log}, indent=2), encoding="utf-8")
    print(f"done ok={ok} fail={fail} save={good}")
    print(f"log -> {out}")


if __name__ == "__main__":
    main()
