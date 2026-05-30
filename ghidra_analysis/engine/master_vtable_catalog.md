# Master vtable catalog

Generated from `vftable_methods.csv` (486 Ghidra `vftable` symbols, 3914 slots).

## Destructor pattern

MSVC Itanium-style layout is consistent across CDS/game types:

| slot | typical role |
|------|----------------|
| 0 | `GetClassTable` / type metadata |
| 1 | scalar-deleting dtor (`~Class` + `operator delete`) |
| 2 | complete object destructor body |

Primary **CDSView**-shaped faces use 28 slots (widgets) or **34** for `CDSApp`/`CBulanci` app shell.
Shared MI thunks at `+0x04/+0x10/+0x18` reuse `CDSChain` / input faces; `+0x68/+0x6c` are update/anim.

## Priority classes (summary)

| class | vftable | off | role | # | slot0 | slot1 | slot2 |
|-------|---------|-----|------|---|-------|-------|-------|
| CDSApp | 0048700c | 0 | primary | 34 | `CDSApp_GetClassTable` | `CDSApp_DtorScalar` | `CDSApp_dtor` |
| CBulanci | 0x0047f7ec |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `CBulanci_EventHandlerDtorBody` |
| CBulanci | 0x0047f800 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `CBulanci_ReferencedDtorBody` |
| CBulanci | 0x0047f818 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `CBulanci_ChainDtorBody` |
| CBulanci | 0x0047f834 | 0x0 | primary | 34 | `CBulanci_GetAppDescriptor` | `CBulanci_DtorScalar` | `CBulanci_dtor` |
| CBulanek | 0x00481ddc | 0x10 | IDSEventHandler | 5 | `FUN_0041b180` | `FUN_004178d0` | `CDSView_OnMouseStub` |
| CBulanek | 0x00481df4 | 0x18 | IDSReferenced | 5 | `FUN_004034d0` | `FUN_0041a980` | `FUN_00418ed0` |
| CBulanek | 0x00481e0c | 0x88 | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CBulanek | 0x00481e20 | 0xa0 | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CBulanek | 0x00481e38 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CBulanek | 0x00481e54 | 0x0 | primary | 31 | `FUN_00418e90` | `FUN_0041bc20` | `CDSObject_ReleaseViaVtable` |
| CButton | 0x0047fe90 |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CButton | 0x0047fea4 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CButton | 0x0047febc |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CButton | 0x0047fedc | 0x0 | primary | 28 | `CButton_GetTypeDescriptor` | `CButton_vDtor` | `CDSObject_ReleaseViaVtable` |
| CDSAnim | 0x00487674 |  | IDSEventHandler | 4 | `GetTypeInfo_Sub78` | `Sub78ChainOp_thunk` | `Sub78_CDSObjectAdjust_thunk` |
| CDSAnim | 0x00487688 |  | IDSChained | 5 | `IDSAnim_NotifyEvents` | `IDSAnim_BindUserData` | `IDSAnim_SetSequence_thunk` |
| CDSAnim | 0x004876a0 |  | IDSEventHandler | 4 | `GetTypeInfo_Sub68` | `FUN_00404de0` | `FUN_004229a0` |
| CDSAnim | 0x004876b4 |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CDSAnim | 0x004876c8 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CDSAnim | 0x004876e0 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSAnim | 0x004876fc | 0x0 | primary | 28 | `GetTypeInfo` | `ScalarDeletingDtor` | `CDSObject_ReleaseViaVtable` |
| CDSApiException | 0x00487564 |  | IDSChained | 5 | `FUN_00434b00` | `FUN_00434e10` | `FUN_0042fff0` |
| CDSApp | 0x00486fc0 | 0x10 | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `CDSApp_EventHandlerDtorBody` |
| CDSApp | 0x00486fd4 | 0x18 | IDSReferenced | 5 | `FUN_00402880` | `FUN_004049c0` | `CDSApp_ReferencedDtorBody` |
| CDSApp | 0x00486fec |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `CDSApp_ChainDtorBody` |
| CDSAudioBank | 0x00486ea4 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_00429300` | `FUN_00409970` |
| CDSAudioBank | 0x00486ec0 |  | IDSChained | 5 | `GetResourceName` | `FUN_00409450` | `FUN_00409490` |
| CDSAudioBank | 0x00486ed8 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSAudioBank | 0x00486eec |  | IDSReferenced | 3 | `FUN_004292f0` | `FUN_004294b0` | `FUN_00401660` |
| CDSAudioBankSample | 0x00486efc | 0x18 | IDSReferenced | 4 | `FUN_00401600` | `FUN_004049d0` | `FUN_00433070` |
| CDSAudioBankSample | 0x00486f10 |  | face_8slots | 8 | `FUN_0041a510` | `FUN_0042ac90` | `FUN_004330a0` |
| CDSAudioBankSample | 0x00486f34 |  | IDSReferenced | 3 | `FUN_00429340` | `FUN_00429510` | `FUN_00433040` |
| CDSAudioPlayer | 0x00487798 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSAudioPlayer | 0x004877ac |  | IDSReferenced | 3 | `FUN_0043a570` | `FUN_0043a740` | `FUN_00401660` |
| CDSAudioVideoPlayer | 0x0048948c |  | IDSEventHandler | 4 | `FUN_0043bc90` | `FUN_0043bd40` | `CDSObject_ReleaseViaVtable` |
| CDSBackBuffer | 0x00486f9c | 0x7c | IDSEventHandler | 4 | `FUN_004255e0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSBackBuffer | 0x00486fb0 | 0x80 | IDSReferenced | 3 | `CDSBackBuffer_GetClassTable` | `CDSBackBuffer_DtorScalar` | `CDSObject_ReleaseViaVtable` |
| CDSBitmap | 0x004830e0 | 0x1a | IDSChained | 5 | `IDSAnim_NotifyEvents` | `IDSAnim_BindUserData` | `IDSAnim_SetSequence_thunk` |
| CDSBitmap | 0x004830f8 |  | IDSEventHandler | 4 | `GetTypeInfo_Sub68` | `FUN_00404de0` | `FUN_004229a0` |
| CDSBitmap | 0x0048310c |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CDSBitmap | 0x00483120 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CDSBitmap | 0x00483138 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSBitmap | 0x00483154 | 0x0 | primary | 28 | `FUN_00422980` | `FUN_004274f0` | `CDSObject_ReleaseViaVtable` |
| CDSBmpImage | 0x004871f0 |  | IDSChained | 5 | `GetResourceName` | `FUN_004322d0` | `FUN_00432320` |
| CDSBmpImage | 0x00487208 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_00425600` | `FUN_00431dd0` |
| CDSBmpImage | 0x00487224 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00431d90` | `FUN_00437550` |
| CDSBmpImage | 0x00487238 |  | IDSEventHandler | 4 | `FUN_004255e0` | `FUN_0042ac90` | `FUN_00431dc0` |
| CDSBmpImage | 0x0048724c |  | IDSReferenced | 3 | `CDSBmpImage_GetClassData` | `CDSBmpImage_scalar_deleting_dtor` | `FUN_004322f0` |
| CDSChain | 0x0047f6b8 | 0x14 | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSChain | 0x0047f6d4 | 0x18 | IDSReferenced | 3 | `FUN_00401200` | `FUN_00401210` | `CDSObject_ReleaseViaVtable` |
| CDSChained | 0x0047f8e0 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSChained | 0x0047f8fc |  | IDSReferenced | 3 | `FUN_004032c0` | `FUN_00433dd0` | `CDSObject_ReleaseViaVtable` |
| CDSCollection | 0x0047f6e4 | 0x1c | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSCollection | 0x0047f700 | 0x20 | IDSReferenced | 3 | `FUN_00401230` | `FUN_00401240` | `CDSObject_ReleaseViaVtable` |
| CDSDirectPlay | 0x004877e4 |  | IDSEventHandler | 4 | `FUN_0043b2d0` | `CDSDirectPlay_scalar_deleting_dtor` | `CDSObject_ReleaseViaVtable` |
| CDSDirectPlaySender | 0x004877bc |  | IDSChained | 5 | `FUN_0043b120` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSDirectPlaySender | 0x004877d4 |  | IDSReferenced | 3 | `FUN_0043b130` | `CDSDirectPlaySender_scalar_deleting_dtor` | `CDSObject_ReleaseViaVtable` |
| CDSDirectSound | 0x004894cc |  | IDSChained | 5 | `FUN_0043b120` | `CDSFlxFile_Sub20ChainOp` | `FUN_0043ca70` |
| CDSDirectSound | 0x004894e4 |  | IDSChained | 5 | `FUN_00402880` | `FUN_00428e20` | `FUN_0043ca40` |
| CDSDirectSound | 0x004894fc |  | IDSChained | 5 | `FUN_004034d0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSDirectSound | 0x00489514 |  | IDSReferenced | 3 | `FUN_0043c8a0` | `FUN_0043cb00` | `CDSObject_ReleaseViaVtable` |
| CDSDirectXException | 0x00489474 |  | IDSChained | 5 | `FUN_0043b750` | `FUN_0043b930` | `FUN_0042fff0` |
| CDSDsmFile | 0x00486dfc |  | IDSChained | 5 | `GetResourceName` | `FUN_00433010` | `FUN_00428e80` |
| CDSDsmFile | 0x00486e14 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_00428e10` | `FUN_00428eb0` |
| CDSDsmFile | 0x00486e30 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00428dd0` | `FUN_00428ea0` |
| CDSDsmFile | 0x00486e44 |  | face_8slots | 8 | `FUN_0041a510` | `FUN_00428e20` | `FUN_00428e70` |
| CDSDsmFile | 0x00486e68 |  | face_10slots | 10 | `GetClassMetaA` | `FUN_0042ac90` | `FUN_00428e90` |
| CDSDsmFile | 0x00486e94 |  | IDSReferenced | 3 | `GetClassRegistry` | `HandleScalarDelete` | `HandleRefcountRelease` |
| CDSEasyMemStream | 0x004803dc |  | face_15slots | 15 | `FUN_00401620` | `FUN_00409450` | `FUN_00409490` |
| CDSEasyMemStream | 0x0048041c |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSEasyMemStream | 0x00480430 |  | IDSReferenced | 3 | `FUN_004091f0` | `FUN_004092f0` | `FUN_00401660` |
| CDSException | 0x00487520 |  | IDSEventHandler | 4 | `FUN_00434a30` | `FUN_00434ae0` | `FUN_0042fff0` |
| CDSFileStream | 0x0047f710 |  | IDSChained | 5 | `FUN_00401610` | `FUN_00429300` | `FUN_00409970` |
| CDSFileStream | 0x0047f72c |  | face_15slots | 15 | `FUN_00401620` | `FUN_00409450` | `FUN_00409490` |
| CDSFileStream | 0x0047f76c |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSFileStream | 0x0047f780 |  | IDSReferenced | 3 | `FUN_00401630` | `FUN_00401770` | `FUN_00401660` |
| CDSFilterStream | 0x004870e4 |  | IDSChained | 5 | `FUN_00401610` | `FUN_00429300` | `FUN_00409970` |
| CDSFilterStream | 0x004870fc |  | face_15slots | 15 | `FUN_00401620` | `FUN_00409450` | `FUN_00409490` |
| CDSFilterStream | 0x0048713c |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSFilterStream | 0x00487150 |  | IDSReferenced | 3 | `FUN_00430080` | `FUN_00430c30` | `FUN_00401660` |
| CDSFlxFile | 0x0048725c |  | IDSChained | 5 | `GetResourceName` | `CDSFlxFile_Sub24ChainOp` | `CDSFlxFile_Sub28Release` |
| CDSFlxFile | 0x00487274 |  | IDSChained | 6 | `FUN_004011f0` | `CDSFlxFile_Sub20ChainOp` | `CDSFlxFile_Sub24Release` |
| CDSFlxFile | 0x00487290 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00428e20` | `CDSFlxFile_Sub1cRelease` |
| CDSFlxFile | 0x004872a4 |  | face_10slots | 10 | `GetClassMetaA` | `FUN_0042ac90` | `CDSFlxFile_Sub04Release` |
| CDSFlxFile | 0x004872d0 |  | IDSReferenced | 3 | `GetClassRegistry` | `ScalarDeletingDtor` | `ReleaseRef` |
| CDSFont | 0x004875c4 |  | IDSChained | 5 | `GetResourceName` | `FUN_004322d0` | `FUN_00432320` |
| CDSFont | 0x004875dc |  | IDSChained | 6 | `FUN_004011f0` | `FUN_00425600` | `FUN_00431dd0` |
| CDSFont | 0x004875f8 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00431d90` | `FUN_00437550` |
| CDSFont | 0x0048760c |  | IDSEventHandler | 4 | `FUN_004255e0` | `FUN_0042ac90` | `FUN_00431dc0` |
| CDSFont | 0x00487620 |  | IDSReferenced | 3 | `FUN_00437510` | `FUN_004375c0` | `FUN_004322f0` |
| CDSGZipStream | 0x0048057c | 0xc | IDSChained | 5 | `FUN_00401610` | `FUN_00429300` | `FUN_00409970` |
| CDSGZipStream | 0x00480594 | 0x14 | face_15slots | 15 | `FUN_00401620` | `FUN_00409450` | `FUN_00409490` |
| CDSGZipStream | 0x004805d4 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSGZipStream | 0x004805e8 |  | IDSReferenced | 3 | `FUN_00409960` | `FUN_004099d0` | `FUN_00401660` |
| CDSGZipStreamData | 0x004875a0 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSGZipStreamData | 0x004875b4 |  | IDSReferenced | 3 | `FUN_00434fa0` | `FUN_00435030` | `FUN_00401660` |
| CDSImage | 0x00483728 | 0x4c | IDSChained | 5 | `GetResourceName` | `FUN_004322d0` | `FUN_00432320` |
| CDSImage | 0x00483740 | 0x54 | IDSChained | 6 | `FUN_004011f0` | `FUN_00425600` | `FUN_00431dd0` |
| CDSImage | 0x0048375c | 0x58 | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00431d90` | `FUN_00437550` |
| CDSImage | 0x00483770 |  | IDSEventHandler | 4 | `FUN_004255e0` | `FUN_0042ac90` | `FUN_00431dc0` |
| CDSImage | 0x00483784 |  | IDSReferenced | 3 | `FUN_004255f0` | `FUN_004375c0` | `FUN_004322f0` |
| CDSImageMouse | 0x00486f68 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSImageMouse | 0x00486f7c |  | IDSChained | 7 | `FUN_0042ac80` | `FUN_0042af60` | `FUN_00401660` |
| CDSJpegImage | 0x00487184 |  | IDSChained | 5 | `GetResourceName` | `FUN_004322d0` | `FUN_00432320` |
| CDSJpegImage | 0x0048719c |  | IDSChained | 6 | `FUN_004011f0` | `FUN_00425600` | `FUN_00431dd0` |
| CDSJpegImage | 0x004871b8 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00431d90` | `FUN_00437550` |
| CDSJpegImage | 0x004871cc |  | IDSEventHandler | 4 | `FUN_004255e0` | `FUN_0042ac90` | `FUN_00431dc0` |
| CDSJpegImage | 0x004871e0 |  | IDSReferenced | 3 | `FUN_00431d50` | `FUN_00432090` | `FUN_004322f0` |
| CDSMemQueue | 0x004894b8 |  | IDSEventHandler | 4 | `FUN_0043c3c0` | `FUN_0043c570` | `CDSObject_ReleaseViaVtable` |
| CDSMemoryException | 0x00487534 |  | IDSChained | 5 | `FUN_00434a90` | `FUN_00434ae0` | `FUN_0042fff0` |
| CDSMouse | 0x00483548 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_0042ac90` | `FUN_00409260` |
| CDSMouse | 0x0048355c |  | IDSChained | 7 | `FUN_00423b10` | `FUN_00434ae0` | `FUN_00401660` |
| CDSMpx | 0x004872e0 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_004049d0` | `FUN_00433070` |
| CDSMpx | 0x004872f4 |  | face_8slots | 8 | `FUN_0041a510` | `FUN_0042ac90` | `FUN_004330a0` |
| CDSMpx | 0x00487318 |  | IDSReferenced | 3 | `FUN_00432f10` | `FUN_00432fc0` | `FUN_00433040` |
| CDSMpxDecoder | 0x00489524 |  | IDSReferenced | 3 | `FUN_00446930` | `FUN_00446ae0` | `CDSObject_ReleaseViaVtable` |
| CDSMpxStream | 0x00487328 |  | IDSChained | 5 | `GetResourceName` | `FUN_00433010` | `FUN_00433090` |
| CDSMpxStream | 0x00487340 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_00428e10` | `FUN_00433080` |
| CDSMpxStream | 0x0048735c |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_004049d0` | `FUN_00433070` |
| CDSMpxStream | 0x00487370 |  | face_8slots | 8 | `FUN_0041a510` | `FUN_0042ac90` | `FUN_004330a0` |
| CDSMpxStream | 0x00487394 |  | IDSReferenced | 3 | `FUN_00432fe0` | `FUN_00433160` | `FUN_00433040` |
| CDSObject | 0x0047f6a8 |  | IDSReferenced | 3 | `CDSObject_GetClassTable` | `FUN_00434ae0` | `CDSObject_ReleaseViaVtable` |
| CDSQueueStream | 0x00486d94 | 0xc | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00409450` | `FUN_00433d60` |
| CDSQueueStream | 0x00486dac |  | face_15slots | 15 | `FUN_00401620` | `FUN_0042ac90` | `FUN_00428d30` |
| CDSQueueStream | 0x00486dec |  | IDSReferenced | 3 | `FUN_00428a20` | `FUN_00428ab0` | `FUN_00433c60` |
| CDSRegKeyException | 0x00487630 |  | IDSChained | 5 | `FUN_00437b20` | `FUN_00438140` | `FUN_0042fff0` |
| CDSResInfo | 0x004873b0 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSResInfo | 0x004873cc |  | IDSReferenced | 3 | `FUN_004339f0` | `FUN_00433a70` | `CDSObject_ReleaseViaVtable` |
| CDSResourceException | 0x004874b0 |  | IDSChained | 5 | `FUN_00434430` | `FUN_004348e0` | `FUN_0042fff0` |
| CDSResourceSign | 0x004874c8 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_004049c0` | `CDSResourceSign_Release_thunk_n0x10` |
| CDSResourceSign | 0x004874dc |  | IDSChained | 5 | `GetResourceName` | `FUN_00434600` | `CDSResourceSign_Release_thunk_n0x8` |
| CDSResourceSign | 0x004874f4 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `CDSResourceSign_Release_thunk_n0x4` |
| CDSResourceSign | 0x00487510 |  | IDSReferenced | 3 | `CDSResourceSign_GetClassData` | `CDSResourceSign_scalar_deleting_dtor` | `CDSResourceSign_Release` |
| CDSSafeStream | 0x00487408 |  | IDSChained | 5 | `FUN_00401610` | `FUN_00429300` | `FUN_00433d70` |
| CDSSafeStream | 0x00487420 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00409450` | `FUN_00433d60` |
| CDSSafeStream | 0x00487434 |  | face_15slots | 15 | `FUN_00401620` | `FUN_0042ac90` | `FUN_00428d30` |
| CDSSafeStream | 0x00487474 |  | IDSReferenced | 3 | `FUN_00433b60` | `FUN_00433c90` | `FUN_00433c60` |
| CDSSafeStreamInfo | 0x00489534 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSSafeStreamInfo | 0x00489550 |  | IDSReferenced | 3 | `FUN_00446cb0` | `FUN_00447080` | `CDSObject_ReleaseViaVtable` |
| CDSScript | 0x00487648 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSScript | 0x00487664 |  | IDSReferenced | 3 | `FUN_004383e0` | `FUN_00438c90` | `CDSObject_ReleaseViaVtable` |
| CDSSimpleException | 0x0048754c |  | IDSChained | 5 | `FUN_00434ac0` | `FUN_00434ae0` | `FUN_0042fff0` |
| CDSStreamException | 0x004870cc |  | IDSChained | 5 | `FUN_0042ffe0` | `FUN_00430b20` | `FUN_0042fff0` |
| CDSStreamStorage | 0x0047f790 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_00434600` | `FUN_00401900` |
| CDSStreamStorage | 0x0047f7a4 |  | face_9slots | 9 | `FUN_004018b0` | `FUN_0042ac90` | `FUN_004018f0` |
| CDSStreamStorage | 0x0047f7cc |  | IDSReferenced | 3 | `FUN_00401850` | `FUN_00401870` | `FUN_004018c0` |
| CDSStrmResInfo | 0x004873dc |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSStrmResInfo | 0x004873f8 |  | IDSReferenced | 3 | `FUN_00433a90` | `FUN_00433a70` | `CDSObject_ReleaseViaVtable` |
| CDSStrmStgLoadingInfo | 0x00487484 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSStrmStgLoadingInfo | 0x004874a0 |  | IDSReferenced | 3 | `FUN_00433db0` | `FUN_00433dd0` | `CDSObject_ReleaseViaVtable` |
| CDSUpdatedItem | 0x004870ac |  | IDSEventHandler | 4 | `FUN_0042ea70` | `FUN_00434ae0` | `CDSApp_PreCreateHook` |
| CDSVideoPlayer | 0x00487770 |  | IDSChained | 5 | `FUN_004034d0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSVideoPlayer | 0x00487788 |  | IDSReferenced | 3 | `FUN_00439d10` | `FUN_00439fc0` | `CDSObject_ReleaseViaVtable` |
| CDSView | 0x0047f90c | 0x10 | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CDSView | 0x0047f920 | 0x18 | IDSReferenced | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CDSView | 0x0047f938 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CDSView | 0x0047f954 | 0x0 | primary | 28 | `CDSView_GetClassTable` | `CDSView_DtorScalar` | `CDSObject_ReleaseViaVtable` |
| CDSWav | 0x0048230c |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_004049d0` | `FUN_00433070` |
| CDSWav | 0x00482320 |  | face_8slots | 8 | `FUN_0041a510` | `FUN_0042ac90` | `FUN_004330a0` |
| CDSWav | 0x00482344 |  | IDSReferenced | 3 | `FUN_0041a520` | `FUN_0041bbe0` | `FUN_00433040` |
| CDSWavStream | 0x00482354 |  | IDSChained | 5 | `GetResourceName` | `FUN_0041a610` | `FUN_0041a5f0` |
| CDSWavStream | 0x0048236c |  | IDSChained | 6 | `FUN_004011f0` | `FUN_00428dd0` | `FUN_0041a5e0` |
| CDSWavStream | 0x00482388 |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_004049d0` | `FUN_00433070` |
| CDSWavStream | 0x0048239c |  | face_8slots | 8 | `FUN_0041a510` | `FUN_0042ac90` | `FUN_004330a0` |
| CDSWavStream | 0x004823c0 |  | IDSReferenced | 3 | `FUN_0041a5c0` | `FUN_0041bc00` | `FUN_00433040` |
| CDSWorkingThread | 0x00489560 |  | IDSReferenced | 3 | `CDSWorkingThread_GetClassTable` | `CDSWorkingThread_DtorScalar` | `CDSObject_ReleaseViaVtable` |
| CDSWorkingThread | 0x00489570 |  | IDSChained | 5 | `FUN_0043b120` | `FUN_0042ac90` | `FUN_0041aa40` |
| CEdit | 0x0047ff64 |  | IDSChained | 5 | `FUN_004034d0` | `FUN_00404de0` | `FUN_004229a0` |
| CEdit | 0x0047ff7c |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CEdit | 0x0047ff90 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CEdit | 0x0047ffa8 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CEdit | 0x0047ffc4 | 0x0 | primary | 30 | `CEdit_GetTypeDescriptor` | `CEdit_vDtor` | `CDSObject_ReleaseViaVtable` |
| CGame | 0x00481a6c |  | IDSEventHandler | 4 | `FUN_00414c90` | `FUN_00428e20` | `FUN_0043ca40` |
| CGame | 0x00481a80 |  | IDSChained | 5 | `FUN_004034d0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CGame | 0x00481a98 |  | IDSReferenced | 3 | `FUN_00414c60` | `FUN_00415f60` | `CDSObject_ReleaseViaVtable` |
| CGameCounter | 0x00480f40 |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CGameCounter | 0x00480f54 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CGameCounter | 0x00480f6c |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CGameCounter | 0x00480f8c | 0x0 | primary | 28 | `GetClassIdentifier` | `ScalarDeletingDestructor` | `CDSObject_ReleaseViaVtable` |
| CGameTypeDlg | 0x00480e80 | 0x10 | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CGameTypeDlg | 0x00480e94 | 0x18 | IDSReferenced | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CGameTypeDlg | 0x00480eac |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CGameTypeDlg | 0x00480ecc | 0x0 | primary | 28 | `FUN_0040bc00` | `CWindow_vDtor` | `CDSObject_ReleaseViaVtable` |
| CGameView | 0x00481acc | 0x10 | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CGameView | 0x00481ae0 | 0x18 | IDSReferenced | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CGameView | 0x00481af8 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CGameView | 0x00481b14 | 0x0 | primary | 31 | `FUN_00416f30` | `CDSView_DtorScalar` | `CDSObject_ReleaseViaVtable` |
| CGaming | 0x00482704 | 0x10 | IDSEventHandler | 5 | `FUN_004034d0` | `FUN_00404de0` | `FUN_004229a0` |
| CGaming | 0x0048271c | 0x18 | IDSReferenced | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CGaming | 0x00482730 | 0x68 | IDSUpdated | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CGaming | 0x00482748 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CGaming | 0x00482764 | 0x0 | primary | 28 | `FUN_0041aca0` | `FUN_0041bee0` | `CDSObject_ReleaseViaVtable` |
| CLevelList | 0x00480718 |  | IDSChained | 5 | `FUN_004034d0` | `FUN_00404de0` | `FUN_004229a0` |
| CLevelList | 0x00480730 |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CLevelList | 0x00480744 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CLevelList | 0x0048075c |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CLevelList | 0x0048077c | 0x0 | primary | 30 | `CLevelList_GetTypeDescriptor` | `CListBox_vDtor` | `CDSObject_ReleaseViaVtable` |
| CLevelScore | 0x00480440 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CLevelScore | 0x0048045c |  | IDSReferenced | 3 | `GetClassIdentifier` | `CLevelScore_scalar_deleting_dtor` | `CDSObject_ReleaseViaVtable` |
| CLevelScript | 0x00481d6c |  | IDSChained | 5 | `FUN_004034d0` | `FUN_004186e0` | `FUN_00418be0` |
| CLevelScript | 0x00481d84 |  | IDSChained | 5 | `GetResourceName` | `FUN_00422770` | `FUN_00421810` |
| CLevelScript | 0x00481d9c |  | IDSEventHandler | 4 | `FUN_00401600` | `FUN_004186c0` | `FUN_00418bf0` |
| CLevelScript | 0x00481db0 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_00421820` |
| CLevelScript | 0x00481dcc |  | IDSReferenced | 3 | `FUN_00418680` | `FUN_0041a450` | `FUN_004216c0` |
| CMenu | 0x00483650 | 0x10 | IDSEventHandler | 5 | `FUN_004034d0` | `FUN_00404de0` | `FUN_004229a0` |
| CMenu | 0x00483668 | 0x18 | IDSReferenced | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CMenu | 0x0048367c | 0x68 | IDSUpdated | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CMenu | 0x00483694 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CMenu | 0x004836b4 | 0x0 | primary | 28 | `CMenu_GetSingleton` | `CMenu_VectorDtorWrapper` | `CDSObject_ReleaseViaVtable` |
| CMovieView | 0x004831c8 |  | IDSChained | 5 | `IDSAnim_NotifyEvents` | `IDSAnim_BindUserData` | `IDSAnim_SetSequence_thunk` |
| CMovieView | 0x004831e0 |  | IDSEventHandler | 4 | `GetTypeInfo_Sub68` | `FUN_00404de0` | `FUN_004229a0` |
| CMovieView | 0x004831f4 |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CMovieView | 0x00483208 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CMovieView | 0x00483220 |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CMovieView | 0x0048323c | 0x0 | primary | 28 | `GetClassTable` | `ScalarDeletingDtor` | `CDSObject_ReleaseViaVtable` |
| CWindow | 0x0047fd10 |  | IDSEventHandler | 4 | `FUN_00402890` | `FUN_004049d0` | `FUN_00423db0` |
| CWindow | 0x0047fd24 |  | IDSChained | 5 | `FUN_00402880` | `FUN_004049c0` | `FUN_004034a0` |
| CWindow | 0x0047fd3c |  | IDSChained | 6 | `FUN_004011f0` | `FUN_0042ac90` | `FUN_0041aa40` |
| CWindow | 0x0047fd5c | 0x0 | primary | 28 | `CWindow_GetTypeDescriptor` | `CWindow_vDtor` | `CDSObject_ReleaseViaVtable` |

## CDSApp / CDSView / CBulanci inheritance

- `CDSApp` MI bases: `0x00486fc0` (+0x18), `0x00486fd4` (+0x10), `0x00486fec` (+0x04).
- `CDSApp` **34-slot primary** at `0x0048700c` (`g_pCDSApp_vftable`) — see `app_shell.md`.
- `CDSView` **28-slot primary** at `0x0047f954`; widgets (`CMenu`, `CSwitch`, …) patch this table.
- `CBulanci` **34-slot** app override at `0x0047f834` (replaces app entry points).

## Still-unnamed slot targets

~393 faces list `FUN_*` in early slots (mostly shared MI thunks).

## Full spreadsheet

See `master_vtable_catalog.csv` for all 487 rows.
