# Agent todos round 3 (2026-05-30)

Manifest: [gent_todos_50_r3.json](./agent_todos_50_r3.json) | Results: [gent_todos_50_r3_results.jsonl](./agent_todos_50_r3_results.jsonl) | Workers: [gent_todos_50_r3_workers.json](./agent_todos_50_r3_workers.json)

Gather: G0-G4 -> 	odos_gather_r3_*.json (29 handoff, 13 blocker, 8 backlog).

| id | pri | src | title |
|----|-----|-----|-------|
| 1 | critical | blocker | CBulanci audio tail xrefs and CDSObject image +0x50 |
| 2 | critical | handoff | Name CDSApp interior fields +0x100..+0x1ff |
| 3 | high | handoff | Embed CWindow win @ 0 on CAdvertising in Ghidra |
| 4 | high | blocker | MCP ECX plate CAnim and CGameView __thiscall sites |
| 5 | medium | backlog | Retype CBulPicture pBitmap to CDSBitmap* |
| 6 | high | blocker | Xref CDeath dwM_modeFlags and bM_tourneyFlag consumers |
| 7 | high | blocker | Disasm-prove CDirectKeyb_GetKeyEdge ECX is CDSApp* |
| 8 | medium | handoff | Prototype CExplosion blast helpers with CGaming* arg |
| 9 | medium | backlog | Apply CGameCounter and CGameTypeDlg Ghidra layouts |
| 10 | medium | handoff | Dedup CHelpView to CDSChained embed @ 0 |
| 11 | medium | handoff | Name CHelpScript CHistoryDlg CWindow prefix +0x00..+0x6f |
| 12 | medium | handoff | Prove CHistoryScript +0x438 stash and decode classId 0x802 |
| 13 | high | backlog | Trace CGame+0x66 writer for CLevelList resource table |
| 14 | critical | blocker | Verify CLevelScore 0x28 Ghidra apply; name CListBoxItem+0x04 |
| 15 | medium | handoff | Decompile CMovieView InitTrackSequence audio bind path |
| 16 | high | blocker | Fix CPauseDlg_Build decompiler ECX opaque this |
| 17 | medium | handoff | Prove CPoem wstrHandle vs pCachedText after Deserialize |
| 18 | medium | handoff | Document CScoreItem m_deaths omit from Serialize schema |
| 19 | medium | blocker | Retype CSessionList scheduler helpers ECX this |
| 20 | medium | handoff | Type CSetupDlg CSpells dialog tails and spell mask bytes |
| 21 | medium | handoff | Name CSwitch CDSChained tail +0x30..+0x64 and CTcpIpConfig dialog chain band |
| 22 | high | blocker | CTeleportPoint gate overlap_entity runtime fill vs gaming_host +0x84; OnEvent param_2==0 partner path |
| 23 | high | handoff | Align CDSAudioBank Ghidra layout to 0x40 heap object; name factory tail dwField_20/38 |
| 24 | medium | backlog | CDSAudioBankSample dwReadCursor semantics and IDSAudioSource MI map |
| 25 | high | handoff | Retype CDSAudioVideoPlayer videoTrackManager byte[72] to nested CDSVideoPlayer (0x48) |
| 26 | medium | handoff | Apply CDSBmpImage Load/Save stream-host MI at +0x54; CDSBitmap CMovieView tail parity |
| 27 | critical | handoff | CDSChain_full profile/registry tail + pAuxHeap allocator; sentinel vs InitCircularNode **done** (worker 27) |
| 28 | high | blocker | MCP ECX retype bundle: CDSCollection_InsertKeyed, CDSDirectSound_InitPrimary, CListBoxItem row ctors |
| 29 | medium | blocker | Replace CDSException 1-byte placeholder; decode CDSEasyMemStream dwIdsStream_state==7 |
| 30 | medium | handoff | Name CDSFilterStream post-BindSource dword; dedup CDSSimpleException vs CDSException embed |
| 31 | medium | backlog | Name FLX opcode 0x0C subscriber vfn[4] in Ghidra |
| 32 | medium | backlog | Ghidra comments for nSeqTotalDurationMs / TM seq[0x10] |
| 33 | high | handoff | Verify CDSFlxFile Ghidra fields vs flx_file_format.md |
| 34 | high | handoff | CDSFont fontFooter dword semantics @ +0x50c |
| 35 | high | handoff | CDSImage MI facets + nField_44/nField_48 naming |
| 36 | low | backlog | Disasm CDSJpegImage registry factory @ 0x0043c160 |
| 37 | medium | handoff | mad_stream/mad_frame libmad Ghidra rename pass |
| 38 | medium | handoff | CDSMpxStream persist band vs live mad_stream |
| 39 | high | handoff | Dedup exception structs: inherited prefix + tail fields |
| 40 | medium | backlog | CDSSafeStream auxHeap allocator + VERIFIED pass |
| 41 | critical | blocker | Clean CBulanek tail Ghidra renames + pre-ammo pads |
| 42 | critical | blocker | Fix CBulanekCtor scheduler + videoTrackManager decompiler |
| 43 | medium | handoff | Plate CDSCollection_InsertKeyed true CDSCollection* ECX |
| 44 | low | handoff | Cosmetic CDSTrackVector embed @ CDSVideoPlayer+0x1c |
| 45 | medium | handoff | Verify CDSStreamStorage nested CDSCollection/CDSChain decompile |
| 46 | high | handoff | Sweep CDSStreamStorage IDSStorage stub comments + loader protos |
| 47 | low | handoff | Verify CDSStrmResInfo pad_tail @ +0x20 AppendOrReuseStream read |
| 48 | medium | handoff | Prove ODSImage.pOwner CBulanek* on gameplay SetOwner paths |
| 49 | high | blocker | Retype CWeapon_ctor this; name trackManager interior +0x08..+0x4F |
| 50 | low | handoff | Handoff CDSWav polymorphic dtor vs CDSWavStream ownership |
