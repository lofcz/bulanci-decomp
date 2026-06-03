# Agent todos round 6 — logic/internals (2026-06-03)

Manifest: [agent_todos_50_r6_logic.json](./agent_todos_50_r6_logic.json) | Results: [agent_todos_50_r6_logic_results.jsonl](./agent_todos_50_r6_logic_results.jsonl) | Protocol: [ROUND6_LOGIC_PROTOCOL.md](./ROUND6_LOGIC_PROTOCOL.md)

Bands: cluster=96 | sim=615 | dispatch=196

Coordinator: worker **50** (no Ghidra) — [round6_logic_worker_50_report.md](logic_recovery/round6_logic_worker_50_report.md)

**Worker map:** cluster **1–10** (seeds) | sim **11–38** (22-func slices) | dispatch **39–49** (codec slices) | coordinator **50**

**Per-worker output:** `logic_recovery/round6_logic_task_{NN}_report.md` + one line in `agent_todos_50_r6_logic_results.jsonl`

| id | pri | range | title |
|----|-----|-------|-------|
| 1 | high | cluster_41ab | Logic cluster: CBulanek_SchedulerTick + scheduler/gaming neighbors |
| 2 | high | cluster_41ab | Logic cluster: FUN_0041af70 + scheduler/gaming neighbors |
| 3 | high | cluster_41ab | Logic cluster: CWeapon_OnSchedulerEvent + scheduler/gaming neighbors |
| 4 | high | cluster_41ab | Logic cluster: CShot_SchedulerTick + scheduler/gaming neighbors |
| 5 | high | cluster_41ab | Logic cluster: CMina_OnSchedulerHook + scheduler/gaming neighbors |
| 6 | medium | cluster_41ab | Logic cluster: CBulanek_AddEntity + scheduler/gaming neighbors |
| 7 | medium | cluster_41ab | Logic cluster: CExplosion_CollectLandminesInBlastRect + scheduler/gaming neighbors |
| 8 | medium | cluster_41ab | Logic cluster: CGaming_Ctor + scheduler/gaming neighbors |
| 9 | medium | cluster_41ab | Logic cluster: CGaming_RespawnPlayerAtSafeLocation + scheduler/gaming neighbors |
| 10 | medium | cluster_41ab | Logic cluster: CGaming_OnPlayerCollectItem + scheduler/gaming neighbors |
| 11 | medium | sim_429_436 | Logic sim_429_436: 0x00429040–0x00429470 (22 funcs) |
| 12 | medium | sim_429_436 | Logic sim_429_436: 0x00429480–0x00429d00 (22 funcs) |
| 13 | medium | sim_429_436 | Logic sim_429_436: 0x00429d60–0x0042ab28 (22 funcs) |
| 14 | medium | sim_429_436 | Logic sim_429_436: 0x0042abb9–0x0042b170 (22 funcs) |
| 15 | medium | sim_429_436 | Logic sim_429_436: 0x0042b39b–0x0042be60 (22 funcs) |
| 16 | medium | sim_429_436 | Logic sim_429_436: 0x0042be90–0x0042c3e0 (22 funcs) |
| 17 | medium | sim_429_436 | Logic sim_429_436: 0x0042c430–0x0042cc80 (22 funcs) |
| 18 | medium | sim_429_436 | Logic sim_429_436: 0x0042ccd0–0x0042d3f0 (22 funcs) |
| 19 | medium | sim_429_436 | Logic sim_429_436: 0x0042d440–0x0042e140 (22 funcs) |
| 20 | medium | sim_429_436 | Logic sim_429_436: 0x0042e230–0x0042ea20 (22 funcs) |
| 21 | medium | sim_429_436 | Logic sim_429_436: 0x0042ea40–0x0042f140 (22 funcs) |
| 22 | medium | sim_429_436 | Logic sim_429_436: 0x0042f1e0–0x0042f780 (22 funcs) |
| 23 | medium | sim_429_436 | Logic sim_429_436: 0x0042f7a0–0x0042fc05 (22 funcs) |
| 24 | medium | sim_429_436 | Logic sim_429_436: 0x0042fc30–0x004302e0 (22 funcs) |
| 25 | medium | sim_429_436 | Logic sim_429_436: 0x00430360–0x00430ca0 (22 funcs) |
| 26 | medium | sim_429_436 | Logic sim_429_436: 0x00430d00–0x004316c0 (22 funcs) |
| 27 | medium | sim_429_436 | Logic sim_429_436: 0x00431700–0x00431dc0 (22 funcs) |
| 28 | medium | sim_429_436 | Logic sim_429_436: 0x00431dd0–0x00432780 (22 funcs) |
| 29 | medium | sim_429_436 | Logic sim_429_436: 0x004327e0–0x00432be0 (22 funcs) |
| 30 | medium | sim_429_436 | Logic sim_429_436: 0x00432c60–0x00433180 (22 funcs) |
| 31 | medium | sim_429_436 | Logic sim_429_436: 0x004331e0–0x004339e0 (22 funcs) |
| 32 | medium | sim_429_436 | Logic sim_429_436: 0x004339f0–0x00433dd0 (22 funcs) |
| 33 | medium | sim_429_436 | Logic sim_429_436: 0x00433df0–0x00434440 (22 funcs) |
| 34 | medium | sim_429_436 | Logic sim_429_436: 0x004344a0–0x004349e0 (22 funcs) |
| 35 | medium | sim_429_436 | Logic sim_429_436: 0x00434a10–0x00434f90 (22 funcs) |
| 36 | medium | sim_429_436 | Logic sim_429_436: 0x00434fa0–0x00435c70 (22 funcs) |
| 37 | medium | sim_429_436 | Logic sim_429_436: 0x00435d00–0x00436270 (22 funcs) |
| 38 | medium | sim_429_436 | Logic sim_429_436: 0x004362f0–0x00436ff0 (21 funcs) |
| 39 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x0045a060–0x0045c4e0 (18 funcs) |
| 40 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x0045c800–0x0045dda0 (18 funcs) |
| 41 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x0045dde0–0x0045ec10 (18 funcs) |
| 42 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x0045ecc0–0x0045f860 (18 funcs) |
| 43 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x0045f880–0x004605a0 (18 funcs) |
| 44 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x004606a0–0x00461540 (18 funcs) |
| 45 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x004615b0–0x00463ee0 (18 funcs) |
| 46 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x00463fe0–0x00465a60 (18 funcs) |
| 47 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x00465bb0–0x00466e40 (18 funcs) |
| 48 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x00466f00–0x004681d0 (18 funcs) |
| 49 | medium | dispatch_45a_468 | Logic dispatch_45a_468: 0x00468200–0x00468fd0 (16 funcs) |
| 50 | high | coordinator | R6 logic coordinator: protocol, manifest, checklist, aggregate status |
