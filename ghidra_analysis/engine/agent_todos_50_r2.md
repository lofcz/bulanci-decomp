# Agent todos round 2 (2026-05-30)

Manifest: [`agent_todos_50_r2.json`](./agent_todos_50_r2.json) | Results: [`agent_todos_50_r2_results.jsonl`](./agent_todos_50_r2_results.jsonl) | Workers: [`agent_todos_50_r2_workers.json`](./agent_todos_50_r2_workers.json)

Gather: G0–G4 → `todos_gather_r2_*.json` (34 handoff, 14 blocker, 2 backlog).

| id | pri | src | title |
|----|-----|-----|-------|
| 1 | critical | blocker | Field-name CGame scheduler embed +0x04..+0x1b |
| 2 | critical | handoff | Name CDSApp MI gaps +0x08..+0x67 on CBulanci.app |
| 3 | high | handoff | Retype CGameView spatial/event helpers as CGameView* |
| 4 | high | handoff | Name CDSScript VM header dwords +0x08..+0x27 |
| 5 | high | blocker | Wire CAnim.track_manager CDSTrackVector @ +0xc4 |
| 6 | high | blocker | Xref CDeath field_100/field_104 and rename tail |
| 7 | medium | handoff | Resolve CGame field_4b4 and field_4c4 semantics |
| 8 | medium | handoff | Name CBulAnim CDSView drawable prefix +0x08..+0x67 |
| 9 | medium | blocker | Document CDSObject image scheduler overlay +0x04..+0x38 |
| 10 | medium | handoff | Field-split CGunMouse embedded CDSImage @ +0x10..+0x190 |
| 11 | high | handoff | Relate CDSView 128B shell to CWindow 0x70 dialog base |
| 12 | critical | blocker | Embed CGaming.game as CGame 0x248 not 872B |
| 13 | high | backlog | Re-xref CGame level table via chain vs +0x66 |
| 14 | high | blocker | Fix CPauseDlg_Build decompiler ECX opaque this |
| 15 | medium | handoff | Type CMovieView pAudioSequence as CDSAnimSequence* |
| 16 | high | backlog | Apply CLevelScore struct 0x28 in Ghidra (slice 13) |
| 17 | medium | blocker | Resolve CScore m_link_prev Serialize vs chain overlap |
| 18 | medium | handoff | Name CHelpScript CHistoryDlg CWindow prefix fields |
| 19 | medium | blocker | Retype CSessionList scheduler helpers ECX this |
| 20 | medium | handoff | Map CAnim header band +0x28..+0x67 on CMina.animBase |
| 21 | medium | handoff | Name CSwitch CDSView shell +0x28..+0x67 and resolve +0xa8/+0xb4 click aliases |
| 22 | high | handoff | Prove CTeleportPoint overlap_entity writer and CAnim pLinked_bulanek store path |
| 23 | high | handoff | Disambiguate CDSAudioBank vs CDSWavStream on 0x40 alloc (class 43 factory) |
| 24 | medium | handoff | Catalog non-menu CDSAudioPlayer pEventTarget hosts beyond CMenu_OnEvent |
| 25 | high | handoff | Retype CDSAudioVideoPlayer videoTrackManager to CDSVideoPlayer; name CDSBackBuffer embed tail |
| 26 | medium | handoff | Apply CDSBitmap drawable shell parity and CDSBmpImage Load/Save MI at +0x54 |
| 27 | critical | handoff | Resolve CDSChain_full sentinel semantics and standalone CBulanci chain object |
| 28 | high | handoff | MCP retry ECX retype: CDSCollection_InsertKeyed, InitPrimary, slice 13 CListBoxItem row |
| 29 | medium | handoff | Decode CDSEasyMemStream dwIdsStream_state==7 and CDSDirectXException What vs inline buffer |
| 30 | medium | handoff | Name CDSFilterStream dwField_10 after BindSource; dedup CDSSimpleException vs CDSException embed |
| 31 | high | handoff | Trace FLX DecodeFrame dispatch + opcode 0x0C subscribers |
| 32 | high | blocker | Document nSeqTotalDurationMs dual semantics in Ghidra |
| 33 | medium | handoff | Verify FLX header Ghidra names match flx_file_format.md |
| 34 | medium | handoff | Prove CDSFont fontFooter dword pair @ +0x50c |
| 35 | high | handoff | CDSImage MI facets + field_44/field_48 proof |
| 36 | low | handoff | Audit CDSJpegImage +0x60 post-factory stores |
| 37 | medium | handoff | Finish mad_stream/mad_frame libmad field rename pass |
| 38 | medium | handoff | CDSMpxStream persist band vs live mad_stream overlay |
| 39 | high | handoff | Dedup exception types: embed CDSException in CDSSimpleException |
| 40 | medium | blocker | CDSSafeStream RegisterThreadSlice ECX + m_chain_auxHeap |
| 41 | critical | blocker | Lay out CBulanek tail +0xA0..+0x19b in Ghidra |
| 42 | critical | blocker | Fix CBulanek_ctor field names +0x88/+0xA8 decompiler |
| 43 | medium | handoff | Retype CDSCollection_InsertKeyed this to CDSCollection* |
| 44 | medium | handoff | Name CDSVideoPlayer dwSchedulerCookie @ +0x40 |
| 45 | high | handoff | Nest CDSCollection/CDSChain on CDSStreamStorage struct |
| 46 | high | handoff | Prototype CDSStreamStorage loader chain helpers |
| 47 | medium | handoff | Document CDSStrmResInfo pad_tail @ +0x20 semantics |
| 48 | high | handoff | Type ODSImage.pOwner CBulanek* and name SetImage AddRef |
| 49 | high | blocker | Retype CWeapon_ctor this and map trackManager interior |
| 50 | high | blocker | Prove CDSWav vs DSM/bank HandleResourceRead this bases |
