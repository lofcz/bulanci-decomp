# Agent todos round 5 (2026-05-30)

Manifest: [agent_todos_50_r5.json](./agent_todos_50_r5.json) | Results: [agent_todos_50_r5_results.jsonl](./agent_todos_50_r5_results.jsonl) | Protocol: [ROUND5_TASK_PROTOCOL.md](./ROUND5_TASK_PROTOCOL.md)

Coordinator: worker **50** — [round5_worker_50_report.md](struct_recovery/round5_worker_50_report.md) (no Ghidra; tasks **1–49** only in manifest).

Handoffs derived from R4 **Remaining UNK**; primary tool: **`set_function_this_type`** (+ prototype/decompile proof).

| id | pri | src | title |
|----|-----|-----|-------|
| 1 | high | handoff | Retype CBulanci_DestroyEmbedFields CDSApp MI vtable restore |
| 2 | medium | handoff | Refresh CDSApp keyLatch exports; nested CBulanci OnCreate decompile |
| 3 | low | handoff | CAdvertising 108B backdrop type; fix CWindow_dtor decompiler namespace |
| 4 | high | handoff | set_function_this_type on CGameView slot/displaced script callees |
| 5 | medium | handoff | Model CBulPicture pBitmap release path vs cast FLX/bitmap faces |
| 6 | low | verify | R4 verify: CDeath mode flags consumers (closure) |
| 7 | low | handoff | set_function_this_type CDirectKeyb ctor/dtor; fix m_keyState export ids |
| 8 | medium | handoff | Name CGaming entity_list @+0x31c; CExplosion_OnEvent IDSAnim this |
| 9 | medium | handoff | Type CGameCounter CDSString members; CGameTypeDlg radio notify byte |
| 10 | medium | handoff | CHelpView wViewFlags bit 0x100 vs CHistoryView; classId COL strings |
| 11 | low | backlog | Reparent HhAddChildToParentView shared namespace; CDSScript scriptData |
| 12 | medium | handoff | Trace non-null pSubObjStash writers beyond help/history |
| 13 | medium | handoff | Cast CLevelList rows to CLevelScriptResource* in enumerate loops |
| 14 | high | handoff | set_function_this_type CLevelScore_AddPlayerScore row as CScoreItem* |
| 15 | medium | handoff | Type CMovieView InitTrackSequence pStreamMgr scratch locals |
| 16 | low | handoff | CPauseDlg wChainCounter_48/4a semantics; overlay classId 2056 path |
| 17 | medium | handoff | set_function_this_type CPoem IDSStream Deserialize/GetText MI thunks |
| 18 | medium | verify | R4 verify: CScoreItem_MatchesKillsDeathsAndName highlight this |
| 19 | medium | handoff | Type CGame.pActiveCGaming CGaming*; fix scheduler case-7 this cast |
| 20 | medium | handoff | CScrollBar +0x80..+0xa3 band; CSpells jumptable decompiler label |
| 21 | medium | handoff | Name CDSChained dwField_5c/60/50; CTcpIpConfig extends CWindow |
| 22 | high | handoff | Trace respawn gate pOverlap_entity runtime fill before OnEvent |
| 23 | low | handoff | Fix CDSAudioBank helper namespace on CDSPtrSlotVec callees |
| 24 | medium | handoff | Label DAT_004b83c4 RTTI; IDSAudioSource vtable+0x14 AcquireRead |
| 25 | low | handoff | Prove shipped asset deserializes heap CDSVideoPlayer classId 0x31 |
| 26 | medium | handoff | CDSBitmap chain pad band; CMovieView_InitTrackSequence formal this |
| 27 | high | handoff | Replace stale CBulanciConfigStore 133B with CDSChain_full 164B |
| 28 | low | backlog | Embed-base prototype CDSDirectSound_InitPrimary; CLevelScore chain casts |
| 29 | medium | handoff | set_function_this_type CDSEasyMemStream IDSEventHandler facet map |
| 30 | medium | handoff | set_function_this_type CDSFilterStream WriteBytes/Seek/Tell IDSStream plates |
| 31 | low | handoff | Name CGunMouse/CWeapon ODSImage vfn slots beyond CBitmap row |
| 32 | medium | handoff | Split FLX flags vs DSM dwFrameCount semantics at seq +0x10/+0x14 |
| 33 | low | verify | R4 verify: CDSFlxFile bodySeekBiasLo (dead field closure) |
| 34 | low | handoff | Document CDSFont +0x560/+0x564 dead runtime metrics (negative result) |
| 35 | high | handoff | CDSImage streamHost/eventFacet scalar-deleting dtor thunk graph |
| 36 | low | verify | R4 verify: CDSQueueStream vs CDSJpegImage classId 0x4b (closure) |
| 37 | medium | handoff | Fix mad_stream/mad_synth get_struct_layout export name drift |
| 38 | medium | handoff | Type MPx stashFacet.pSubObjStash payload; registry 0x48 vs decode 0x98c8 |
| 39 | high | handoff | Fix CDSSimpleException_Throw InitFields base promotion; reparent resource throw |
| 40 | medium | handoff | Rename CDSChain splice FUN_0042f9d0 band; RegisterThreadSlice plate |
| 41 | high | handoff | CBulanek prefix +0x48..+0x87 pad names; abReserved_preAmmo consumers |
| 42 | low | handoff | set_function_this_type CBulanekCtor scheduler; prefix band cosmetic export |
| 43 | medium | handoff | set_function_this_type CDSUpdatedItem_ctor vs collection field view |
| 44 | low | handoff | Fix CDSTrackVector dwTracks* export; CDSVideoPlayer_EnsureCapacity namespace |
| 45 | medium | handoff | set_function_prototype CloseStreamByKey CDSStrmResInfo key facet |
| 46 | medium | handoff | Identify collection facet vcall +0x10 callee at InitRootSafeStream |
| 47 | low | handoff | R4 verify: CDSStrmResInfo pad_tail +0x24 (closure) |
| 48 | low | handoff | ODSImage.pOwner CDSView* embedder; shared CDSChained view base |
| 49 | low | handoff | CWeapon pTrackHolder mine/grenade paths; track helper class namespaces |

