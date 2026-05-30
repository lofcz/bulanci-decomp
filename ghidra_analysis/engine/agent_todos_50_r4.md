# Agent todos round 4 (2026-05-30)

Manifest: [agent_todos_50_r4.json](./agent_todos_50_r4.json) | Results: [agent_todos_50_r4_results.jsonl](./agent_todos_50_r4_results.jsonl) | Workers: [agent_todos_50_r4_workers.json](./agent_todos_50_r4_workers.json)

Gather: G0-G4 -> todos_gather_r4_*.json (27 handoff, 14 blocker, 3 backlog, 6 verify from R3-done fill).

| id | pri | src | title |
|----|-----|-----|-------|
| 1 | high | handoff | CDSObject image MI +0x54/+0x5c and CGame cmdline dtor @0x4028d0 |
| 2 | medium | handoff | CDSApp keyLatch display names and OnCreate CBulanci* param |
| 3 | low | verify | Verify R3 closure: Embed CWindow win @ 0 on CAdvertising in Ghidra |
| 4 | high | handoff | Retype CGameView gaming_host to CGaming* in InitGamingFields |
| 5 | medium | backlog | Align CDSBitmap FLX face +4/+8 for CBulPicture extent reads |
| 6 | low | verify | Verify R3 closure: Xref CDeath dwM_modeFlags and bM_tourneyFlag consumers |
| 7 | low | handoff | Rename CDirectKeyb m_keyState and CGame.pDirectKeyb @+0x204 |
| 8 | medium | handoff | Name CGaming entity-list field @+0x31c for blast collectors |
| 9 | medium | backlog | Fix CGameTypeDlg_OnNotify decompile widget tail indexing |
| 10 | medium | handoff | CHelpView classId 0x81d and wChainFlag46 vs CHistoryView |
| 11 | medium | blocker | Fix HhAddChildToParentView stack; name CDSScript pad band |
| 12 | medium | handoff | Trace CHistoryScript pSubObjStash writers and target type |
| 13 | medium | handoff | Create Ghidra CLevelScriptResource 0x14 row struct |
| 14 | high | blocker | Retype CLevelScore helpers ECX; fix GetOrCreateLevelScore |
| 15 | medium | blocker | Merge CMovieView InitTrackSequence unaff_ESI into this |
| 16 | low | handoff | Rename CPauseDlg pGame component at +0x70 |
| 17 | medium | handoff | Plate CPoem IDSStream MI thunks; xref GetText |
| 18 | medium | blocker | Retype CScoreItem_MatchesKillsDeathsAndName highlight this |
| 19 | medium | handoff | Type scheduler case-1 pActiveCGaming as CSessionList* |
| 20 | medium | blocker | Decompile CSpells ammo invalidate jumptable; CScrollBar+0xb0 |
| 21 | medium | handoff | Prove CSwitch/CDSChained dwField_30..64 writers beyond ctor zero |
| 22 | high | blocker | CTeleportPoint respawn overlap_entity writer; decompile UpdateStateFromParams +0x64 |
| 23 | high | blocker | Retype CDSAudioBank_Deserialize MI facet this at +0x14 adjustor |
| 24 | medium | handoff | CDSAudioBankSample IDSStream vtable 0x486efc slots; bank decoder facet |
| 25 | low | handoff | Xref heap CDSVideoPlayer classId 0x31 factory consumer path |
| 26 | medium | blocker | CDSBitmap chain band +0x30..64 semantics; InitTrackSequence set_function_this_type |
| 27 | critical | handoff | CDSChain_full dtor on 0xa4 object; CGame.chain vs CBulanciConfigStore parity |
| 28 | medium | handoff | CDSDirectSound InitPrimary app-shell helper; InsertKeyed FindKeyIndex this cast |
| 29 | medium | blocker | CDSEasyMemStream IDSStream ReadBytes ECX plate at outer+0x0c |
| 30 | medium | blocker | CDSFilterStream_ReadBytes retype IDSStream* plate at embed +0x0c |
| 31 | medium | handoff | Trace FrameTimeHint u16 and IDSAnim slots 0..3 |
| 32 | medium | handoff | Audit FLX inMemSize 0x470 vs sizeof(CBulPicture) and other seq types |
| 33 | low | handoff | Xref CDSFlxFile bodySeekBiasLo non-zero writers |
| 34 | medium | handoff | Trace CDSFont fontFooter to nDefaultAdvance/nLineSpacing copy path |
| 35 | high | blocker | CDSImage bpp tag semantics and dtor thunk namespace fix |
| 36 | low | backlog | Plate CDSQueueStream vs CDSJpegImage classId 0x4b registry sites |
| 37 | medium | blocker | Manual libmad member renames; mad_frame.body interior map |
| 38 | medium | handoff | Fix CDSMpxStream payload offsets and pSubObjStash type |
| 39 | high | blocker | Exception throw-site this types and vtable slot catalogs |
| 40 | medium | **done** | CDSChain pAuxHeap allocator and CDSSafeStream vs CDSFilterStream MI — [round4_task_40_report.md](struct_recovery/round4_task_40_report.md) |
| 41 | critical | blocker | CBulanek pre-ammo band semantics + apWalk/apAi array names |
| 42 | low | verify | Verify R3 closure: Fix CBulanekCtor scheduler + videoTrackManager decompiler |
| 43 | medium | handoff | Retype FindKeyIndex; fix InsertKeyed callee cast |
| 44 | low | handoff | CDSTrackVector InsertOrFindTrack this; cTracks* layout labels |
| 45 | medium | handoff | CloseStreamByKey pass CDSCollection* to FindKeyIndex |
| 46 | medium | handoff | InitRootSafeStream vcall through collection facet |
| 47 | low | verify | Verify R3 closure: Verify CDSStrmResInfo pad_tail @ +0x20 AppendOrReuseStream read |
| 48 | low | handoff | Evaluate ODSImage.pOwner as CDSView* embedder host |
| 49 | low | verify | Verify R3 closure: Retype CWeapon_ctor this; name trackManager interior +0x08..+0x4F |
| 50 | low | verify | Verify R3 closure: Handoff CDSWav polymorphic dtor vs CDSWavStream ownership |
