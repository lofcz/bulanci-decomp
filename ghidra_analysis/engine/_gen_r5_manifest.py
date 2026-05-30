#!/usr/bin/env python3
"""Generate agent_todos_50_r5.json from R4 manifest + round4 UNK sections."""
import json
import re
from pathlib import Path

base = Path(__file__).parent
r4 = json.loads((base / "agent_todos_50_r4.json").read_text(encoding="utf-8"))
structs_dir = base / "struct_recovery"


def extract_unk(report_path: Path) -> list[str]:
    if not report_path.exists():
        return []
    text = report_path.read_text(encoding="utf-8", errors="replace")
    m = re.search(r"## Remaining UNK\s*\n(.*?)(?=\n## |\Z)", text, re.DOTALL)
    if not m:
        return []
    items: list[str] = []
    for line in m.group(1).split("\n"):
        line = line.strip()
        if not line.startswith("-"):
            continue
        item = re.sub(r"^-\s+", "", line)
        item = re.sub(r"\*\*([^*]+)\*\*", r"\1", item)
        if item.lower().startswith("none for this task"):
            return []
        items.append(item)
    return items


def infer_priority(unk: str, r4_pri: str) -> str:
    u = (unk or "").lower()
    if any(w in u for w in ("critical", "blocker", "fault", "wrong size", "stale", "should be")):
        return "high"
    if any(
        w in u
        for w in (
            "cosmetic",
            "doc only",
            "display-only",
            "authoring intent",
            "different binary",
            "dead reserved",
            "alignment / reserved",
        )
    ):
        return "low"
    if r4_pri == "critical":
        return "high"
    return r4_pri if r4_pri in ("high", "medium", "low") else "medium"


TITLE_OVERRIDES: dict[int, str] = {
    1: "Retype CBulanci_DestroyEmbedFields CDSApp MI vtable restore",
    2: "Refresh CDSApp keyLatch exports; nested CBulanci OnCreate decompile",
    3: "CAdvertising 108B backdrop type; fix CWindow_dtor decompiler namespace",
    4: "set_function_this_type on CGameView slot/displaced script callees",
    5: "Model CBulPicture pBitmap release path vs cast FLX/bitmap faces",
    6: "R4 verify: CDeath mode flags consumers (closure)",
    7: "set_function_this_type CDirectKeyb ctor/dtor; fix m_keyState export ids",
    8: "Name CGaming entity_list @+0x31c; CExplosion_OnEvent IDSAnim this",
    9: "Type CGameCounter CDSString members; CGameTypeDlg radio notify byte",
    10: "CHelpView wViewFlags bit 0x100 vs CHistoryView; classId COL strings",
    11: "Reparent HhAddChildToParentView shared namespace; CDSScript scriptData",
    12: "Trace non-null pSubObjStash writers beyond help/history",
    13: "Cast CLevelList rows to CLevelScriptResource* in enumerate loops",
    14: "set_function_this_type CLevelScore_AddPlayerScore row as CScoreItem*",
    15: "Type CMovieView InitTrackSequence pStreamMgr scratch locals",
    16: "CPauseDlg wChainCounter_48/4a semantics; overlay classId 2056 path",
    17: "set_function_this_type CPoem IDSStream Deserialize/GetText MI thunks",
    18: "R4 verify: CScoreItem_MatchesKillsDeathsAndName highlight this",
    19: "Type CGame.pActiveCGaming CGaming*; fix scheduler case-7 this cast",
    20: "CScrollBar +0x80..+0xa3 band; CSpells jumptable decompiler label",
    21: "Name CDSChained dwField_5c/60/50; CTcpIpConfig extends CWindow",
    22: "Trace respawn gate pOverlap_entity runtime fill before OnEvent",
    23: "Fix CDSAudioBank helper namespace on CDSPtrSlotVec callees",
    24: "Label DAT_004b83c4 RTTI; IDSAudioSource vtable+0x14 AcquireRead",
    25: "Prove shipped asset deserializes heap CDSVideoPlayer classId 0x31",
    26: "CDSBitmap chain pad band; CMovieView_InitTrackSequence formal this",
    27: "Replace stale CBulanciConfigStore 133B with CDSChain_full 164B",
    28: "Embed-base prototype CDSDirectSound_InitPrimary; CLevelScore chain casts",
    29: "set_function_this_type CDSEasyMemStream IDSEventHandler facet map",
    30: "set_function_this_type CDSFilterStream WriteBytes/Seek/Tell IDSStream plates",
    31: "Name CGunMouse/CWeapon ODSImage vfn slots beyond CBitmap row",
    32: "Split FLX flags vs DSM dwFrameCount semantics at seq +0x10/+0x14",
    33: "R4 verify: CDSFlxFile bodySeekBiasLo (dead field closure)",
    34: "Document CDSFont +0x560/+0x564 dead runtime metrics (negative result)",
    35: "CDSImage streamHost/eventFacet scalar-deleting dtor thunk graph",
    36: "R4 verify: CDSQueueStream vs CDSJpegImage classId 0x4b (closure)",
    37: "Fix mad_stream/mad_synth get_struct_layout export name drift",
    38: "Type MPx stashFacet.pSubObjStash payload; registry 0x48 vs decode 0x98c8",
    39: "Fix CDSSimpleException_Throw InitFields base promotion; reparent resource throw",
    40: "Rename CDSChain splice FUN_0042f9d0 band; RegisterThreadSlice plate",
    41: "CBulanek prefix +0x48..+0x87 pad names; abReserved_preAmmo consumers",
    42: "set_function_this_type CBulanekCtor scheduler; prefix band cosmetic export",
    43: "set_function_this_type CDSUpdatedItem_ctor vs collection field view",
    44: "Fix CDSTrackVector dwTracks* export; CDSVideoPlayer_EnsureCapacity namespace",
    45: "set_function_prototype CloseStreamByKey CDSStrmResInfo key facet",
    46: "Identify collection facet vcall +0x10 callee at InitRootSafeStream",
    47: "R4 verify: CDSStrmResInfo pad_tail +0x24 (closure)",
    48: "ODSImage.pOwner CDSView* embedder; shared CDSChained view base",
    49: "CWeapon pTrackHolder mine/grenade paths; track helper class namespaces",
}


def short_title(tid: int, unk: str, r4_title: str) -> str:
    if tid in TITLE_OVERRIDES:
        return TITLE_OVERRIDES[tid]
    if not unk:
        return f"R4 verify: {r4_title[:72]}"
    t = unk.replace("`", "").split("—")[0].split("–")[0].strip()
    if len(t) > 115:
        t = t[:112] + "..."
    return t


r5: list[dict] = []
for t in r4:
    tid = int(t["id"])
    if tid >= 50:
        continue
    report = structs_dir / f"round4_task_{tid:02d}_report.md"
    unks = extract_unk(report)
    unk0 = unks[0] if unks else ""
    title = short_title(tid, unk0, t["title"])
    actions = [
        "set_function_this_type",
        "set_function_prototype",
        "force_decompile",
        "set_decompiler_comment",
        "modify_struct_field",
        "get_struct_layout",
        "save_program",
    ]
    evidence = [
        f"ghidra_analysis/engine/struct_recovery/round4_task_{tid:02d}_report.md",
        "ghidra_analysis/engine/agent_todos_50_r4_results.jsonl",
        "ghidra_analysis/engine/ROUND5_TASK_PROTOCOL.md",
    ]
    for p in t.get("evidence_paths", []):
        if p not in evidence:
            evidence.append(p)
    src = "verify" if not unks else ("backlog" if "cosmetic" in unk0.lower() else "handoff")
    entry: dict = {
        "id": tid,
        "title": title,
        "types": t.get("types", []),
        "addresses": t.get("addresses", []),
        "ghidra_actions": actions,
        "evidence_paths": evidence[:9],
        "priority": infer_priority(unk0, t.get("priority", "medium")),
        "source": src,
        "supersedes_todo_id": tid,
        "r4_status": "done",
        "r4_unk_summary": unk0[:300] if unk0 else "R4 DONE; verify-only pass",
    }
    if unk0:
        entry["acceptance"] = unk0[:400]
    r5.append(entry)

out = base / "agent_todos_50_r5.json"
out.write_text(json.dumps(r5, indent=2) + "\n", encoding="utf-8")
print(f"wrote {out} ({len(r5)} tasks)")
