#include "CGaming.h"

// !FUNC 0x004033b0 BEGIN
/* 4033B0-4033CB 0001B */
bool CGaming::CGaming_IsSchedulerSlotActive(uint param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004033b0 END

// !FUNC 0x00412520 BEGIN
/* 412520-41254A 0002A */
uchar CGaming::CGaming_BeginDemoRecording(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412520 END

// !FUNC 0x004129c0 BEGIN
/* 4129C0-4129F2 00032 */
uchar CGaming::CGaming_OnResumeSyncRoundTimer(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004129c0 END

// !FUNC 0x00412fa0 BEGIN
/* 412FA0-413023 00083 */
uchar CGaming::CGaming_RegisterPreMatchSchedulerHooks(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00412fa0 END

// !FUNC 0x00413180 BEGIN
/* 413180-4131FA 0007A */
uchar CGaming::CGame_NetSendRoundResult_t0b(char param_1, short param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00413180 END

// !FUNC 0x00413390 BEGIN
/* 413390-41340D 0007D */
uchar CGaming::CGaming_AppendKeyToInputBuffer(char param_1, char param_2) { STUB_BODY(); return 0; }
// !FUNC 0x00413390 END

// !FUNC 0x00415f80 BEGIN
/* 415F80-416025 000A5 */
void CGaming::CGaming_DrainRecordedNetMessages(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00415f80 END

// !FUNC 0x00416810 BEGIN
/* 416810-41682A 0001A */
uint CGaming::CGaming_GetObjectAtSlotSafe(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416810 END

// !FUNC 0x00416860 BEGIN
/* 416860-41689A 0003A */
uchar CGaming::CGaming_OnKeyDown(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416860 END

// !FUNC 0x004168a0 BEGIN
/* 4168A0-4168B3 00013 */
uchar CGaming::CGaming_OnKeyUp(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004168a0 END

// !FUNC 0x004168e0 BEGIN
/* 4168E0-4168EB 0000B */
bool CGaming::CGaming_IsRoundEndTransitionComplete(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004168e0 END

// !FUNC 0x004168f0 BEGIN
/* 4168F0-416933 00043 */
void CGaming::CGaming_PostMouseMoveToOccupiedHudSlot(uchar param_1) { STUB_BODY(); }
// !FUNC 0x004168f0 END

// !FUNC 0x00416940 BEGIN
/* 416940-416969 00029 */
int CGaming::CGaming_CountActiveEntities(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00416940 END

// !FUNC 0x00419ca0 BEGIN
/* 419CA0-419D14 00074 */
void CGaming::CGaming_UnregisterAndRemoveObject(uchar param_1) { STUB_BODY(); }
// !FUNC 0x00419ca0 END

// !FUNC 0x00419d20 BEGIN
/* 419D20-419D60 00040 */
uchar CGaming::CGaming_ClearAllEntities(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00419d20 END

// !FUNC 0x00419e40 BEGIN
/* 419E40-419E84 00044 */
uchar CGaming::CGaming_UnregisterRoundHudObjects(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00419e40 END

// !FUNC 0x0041a270 BEGIN
/* 41A270-41A294 00024 */
uchar CGaming::CGaming_ReleaseAndClearSlotRef(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041a270 END

// !FUNC 0x0041a2a0 BEGIN
/* 41A2A0-41A2EC 0004C */
uchar CGaming::CGaming_CleanupInactiveBullets(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041a2a0 END

// !FUNC 0x0041ab70 BEGIN
/* 41AB70-41AC93 00123 */
uchar CGaming::CGaming_Ctor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041ab70 END

// !FUNC 0x0041aca0 BEGIN
/* 41ACA0-41ACA6 00006 */
uchar* CGaming::CGaming_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b3788);
}
// !FUNC 0x0041aca0 END

// !FUNC 0x0041acb0 BEGIN
/* 41ACB0-41ACB8 00008 */
uchar CGaming::CGaming_ScalarDeletingDtor_thunk_Sub4(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041acb0 END

// !FUNC 0x0041acc0 BEGIN
/* 41ACC0-41ACC8 00008 */
uchar CGaming::CGaming_ScalarDeletingDtor_thunk_Sub68(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041acc0 END

// !FUNC 0x0041acd0 BEGIN
/* 41ACD0-41ACD8 00008 */
uchar CGaming::CGaming_ScalarDeletingDtor_thunk_Sub10(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041acd0 END

// !FUNC 0x0041ace0 BEGIN
/* 41ACE0-41ACE8 00008 */
uchar CGaming::CGaming_ScalarDeletingDtor_thunk_Sub18(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041ace0 END

// !FUNC 0x0041b3b0 BEGIN
/* 41B3B0-41B417 00067 */
uchar CGaming::CGaming_OnCustomEvent_0xF5_RemoveEntitiesBySlotId(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041b3b0 END

// !FUNC 0x0041b500 BEGIN
/* 41B500-41B590 00090 */
void CGaming::CGaming_TickAmbientAnimations(uchar param_1) { STUB_BODY(); }
// !FUNC 0x0041b500 END

// !FUNC 0x0041b5a0 BEGIN
/* 41B5A0-41B61A 0007A */
uchar CGaming::CGaming_RetestTraceAreasForEntity(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041b5a0 END

// !FUNC 0x0041b620 BEGIN
/* 41B620-41B6D0 000B0 */
uchar CGaming::CGaming_SeedMatchSchedulerEvents(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041b620 END

// !FUNC 0x0041b850 BEGIN
/* 41B850-41BA51 00201 */
uchar CGaming::CGaming_dtor(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041b850 END

// !FUNC 0x0041bab0 BEGIN
/* 41BAB0-41BAFB 0004B */
uchar CGaming::CGaming_EnqueuePreMatchSchedulerSlots(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041bab0 END

// !FUNC 0x0041bee0 BEGIN
/* 41BEE0-41BEFE 0001E */
void* CGaming::CGaming_ScalarDeletingDtor(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041bee0 END

// !FUNC 0x0041c140 BEGIN
/* 41C140-41C284 00144 */
uchar CGaming::CGaming_OnResumeOrStartGame(char param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041c140 END

// !FUNC 0x0041c290 BEGIN
/* 41C290-41C525 00295 */
uchar CGaming::CGaming_RunPreMatchModal(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041c290 END

// !FUNC 0x0041d5f0 BEGIN
/* 41D5F0-41D6DA 000EA */
uchar CGaming::CGaming_OnCmd(ushort param_1, int param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041d5f0 END

// !FUNC 0x0041e350 BEGIN
/* 41E350-41E3DA 0008A */
int CGaming::CGaming_RandomPickupSpawner_Tick(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041e350 END

// !FUNC 0x0041f050 BEGIN
/* 41F050-41F0BE 0006E */
uchar CGaming::CGaming_OnSchedulerTimer(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041f050 END

// !FUNC 0x00420480 BEGIN
/* 420480-420509 00089 */
uchar CGaming::CGaming_OnCustomEvent_0xF6_RespawnPlayer(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00420480 END

// !FUNC 0x004206a0 BEGIN
/* 4206A0-42079C 000FC */
uchar CGaming::CGaming_OnCustomEvent(ushort param_1, int* param_2, uint param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004206a0 END

// !FUNC 0x00429d90 BEGIN
/* 429D90-429DAD 0001D */
uchar CGaming::CGaming_ClearSchedulerSlotFlags(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00429d90 END

// !FUNC 0x00429f70 BEGIN
/* 429F70-429FFB 0008B */
uchar CGaming::CGaming_SyncKeyLatchAfterModal(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00429f70 END

// !FUNC 0x00439880 BEGIN
/* 439880-4398B0 00030 */
void CGaming::CGaming_ArmTrackMgrSchedulerIfUnpaused(CDSVideoPlayer* param_1) { STUB_BODY(); }
// !FUNC 0x00439880 END

// !FUNC 0x00439970 BEGIN
/* 439970-43998E 0001E */
uchar CGaming::CGaming_PauseClockIfActive(CDSVideoPlayer* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00439970 END

// !FUNC 0x0043aa70 BEGIN
/* 43AA70-43AADE 0006E */
uchar CGaming::CGaming_PreMatchModal_RestartDeferredAudio(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0043aa70 END

// !FUNC 0x004168c0 BEGIN
/* 4168C0-4168CD 0000D */
uchar CGaming::CGaming_SetEntityRegisterMode(uchar param_1) {
    *(reinterpret_cast<uchar*>(this) + 0x341) = param_1;
    return param_1;
}
// !FUNC 0x004168c0 END

// !FUNC 0x004180e0 BEGIN
/* 4180E0-418255 00175 */
void CGaming::CGaming_OnKillCredit_UpdateScoreHud(CBulanek* param_1) { STUB_BODY(); }
// !FUNC 0x004180e0 END

// !FUNC 0x004183d0 BEGIN
/* 4183D0-41849C 000CC */
int CGaming::CExplosion_CollectEntitiesInBlastRect(int* param_1, int* param_2, char param_3) { STUB_BODY(); return 0; }
// !FUNC 0x004183d0 END

// !FUNC 0x004184a0 BEGIN
/* 4184A0-418595 000F5 */
uchar CGaming::CGaming_InsertEntityByDepth(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x004184a0 END

// !FUNC 0x0041a020 BEGIN
/* 41A020-41A0AA 0008A */
void CGaming::CGaming_OnPlayerCollectItem(void* param_1, void* param_2, uint param_3) { STUB_BODY(); }
// !FUNC 0x0041a020 END

// !FUNC 0x0041a140 BEGIN
/* 41A140-41A266 00126 */
uchar CGaming::CGaming_RespawnPlayerAtSafeLocation(CBulanek* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041a140 END

// !FUNC 0x0041a2f0 BEGIN
/* 41A2F0-41A38D 0009D */
int CGaming::CExplosion_CollectLandminesInBlastRect(int* param_1, int* param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041a2f0 END

// !FUNC 0x0041a390 BEGIN
/* 41A390-41A441 000B1 */
void CGaming::CGaming_AddEntity(void* param_1, char param_2) { STUB_BODY(); }
// !FUNC 0x0041a390 END

// !FUNC 0x0041b420 BEGIN
/* 41B420-41B4A0 00080 */
uint* CGaming::CGaming_AppendDangerZoneNode(uint param_1, uint* param_2, uint param_3, int param_4) { STUB_BODY(); return 0; }
// !FUNC 0x0041b420 END

// !FUNC 0x0041b6d0 BEGIN
/* 41B6D0-41B821 00151 */
uchar CGaming::CGaming_LoadBackgroundMusic(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041b6d0 END

// !FUNC 0x0041ba60 BEGIN
/* 41BA60-41BAA6 00046 */
uchar CGaming::CGaming_SetMusicAndSoundBank(uint param_1, uint param_2) { STUB_BODY(); return 0; }
// !FUNC 0x0041ba60 END

// !FUNC 0x0041ff90 BEGIN
/* 41FF90-420455 004C5 */
void* CGaming::CGaming_ctor(CGame* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041ff90 END

