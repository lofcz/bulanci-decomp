#ifndef _C_GAMING
#define _C_GAMING

#include <globals.h>

class CGaming {
public:
	// !DECL 0x004033b0 BEGIN
	/* 4033B0 */ bool CGaming_IsSchedulerSlotActive(uint param_1);
	// !DECL 0x004033b0 END
	// !DECL 0x00412520 BEGIN
	/* 412520 */ uchar CGaming_BeginDemoRecording(int param_1);
	// !DECL 0x00412520 END
	// !DECL 0x004129c0 BEGIN
	/* 4129C0 */ uchar CGaming_OnResumeSyncRoundTimer(int param_1);
	// !DECL 0x004129c0 END
	// !DECL 0x00412fa0 BEGIN
	/* 412FA0 */ uchar CGaming_RegisterPreMatchSchedulerHooks(void* param_1);
	// !DECL 0x00412fa0 END
	// !DECL 0x00413180 BEGIN
	/* 413180 */ uchar CGame_NetSendRoundResult_t0b(char param_1, short param_2);
	// !DECL 0x00413180 END
	// !DECL 0x00413390 BEGIN
	/* 413390 */ uchar CGaming_AppendKeyToInputBuffer(char param_1, char param_2);
	// !DECL 0x00413390 END
	// !DECL 0x00415f80 BEGIN
	/* 415F80 */ void CGaming_DrainRecordedNetMessages(uchar param_1);
	// !DECL 0x00415f80 END
	// !DECL 0x00416810 BEGIN
	/* 416810 */ uint CGaming_GetObjectAtSlotSafe(uchar param_1);
	// !DECL 0x00416810 END
	// !DECL 0x00416860 BEGIN
	/* 416860 */ uchar CGaming_OnKeyDown(char param_1);
	// !DECL 0x00416860 END
	// !DECL 0x004168a0 BEGIN
	/* 4168A0 */ uchar CGaming_OnKeyUp(char param_1);
	// !DECL 0x004168a0 END
	// !DECL 0x004168e0 BEGIN
	/* 4168E0 */ bool CGaming_IsRoundEndTransitionComplete(int param_1);
	// !DECL 0x004168e0 END
	// !DECL 0x004168f0 BEGIN
	/* 4168F0 */ void CGaming_PostMouseMoveToOccupiedHudSlot(uchar param_1);
	// !DECL 0x004168f0 END
	// !DECL 0x00416940 BEGIN
	/* 416940 */ int CGaming_CountActiveEntities(int param_1);
	// !DECL 0x00416940 END
	// !DECL 0x00419ca0 BEGIN
	/* 419CA0 */ void CGaming_UnregisterAndRemoveObject(uchar param_1);
	// !DECL 0x00419ca0 END
	// !DECL 0x00419d20 BEGIN
	/* 419D20 */ uchar CGaming_ClearAllEntities(void* param_1);
	// !DECL 0x00419d20 END
	// !DECL 0x00419e40 BEGIN
	/* 419E40 */ uchar CGaming_UnregisterRoundHudObjects(uchar param_1);
	// !DECL 0x00419e40 END
	// !DECL 0x0041a270 BEGIN
	/* 41A270 */ uchar CGaming_ReleaseAndClearSlotRef(int* param_1);
	// !DECL 0x0041a270 END
	// !DECL 0x0041a2a0 BEGIN
	/* 41A2A0 */ uchar CGaming_CleanupInactiveBullets(void* param_1);
	// !DECL 0x0041a2a0 END
	// !DECL 0x0041ab70 BEGIN
	/* 41AB70 */ uchar CGaming_Ctor(uchar param_1);
	// !DECL 0x0041ab70 END
	// !DECL 0x0041aca0 BEGIN
	/* 41ACA0 */ uchar* CGaming_GetClassTable();
	// !DECL 0x0041aca0 END
	// !DECL 0x0041acb0 BEGIN
	/* 41ACB0 */ uchar CGaming_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x0041acb0 END
	// !DECL 0x0041acc0 BEGIN
	/* 41ACC0 */ uchar CGaming_ScalarDeletingDtor_thunk_Sub68(uchar param_1);
	// !DECL 0x0041acc0 END
	// !DECL 0x0041acd0 BEGIN
	/* 41ACD0 */ uchar CGaming_ScalarDeletingDtor_thunk_Sub10(uchar param_1);
	// !DECL 0x0041acd0 END
	// !DECL 0x0041ace0 BEGIN
	/* 41ACE0 */ uchar CGaming_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x0041ace0 END
	// !DECL 0x0041b3b0 BEGIN
	/* 41B3B0 */ uchar CGaming_OnCustomEvent_0xF5_RemoveEntitiesBySlotId(int param_1);
	// !DECL 0x0041b3b0 END
	// !DECL 0x0041b500 BEGIN
	/* 41B500 */ void CGaming_TickAmbientAnimations(uchar param_1);
	// !DECL 0x0041b500 END
	// !DECL 0x0041b5a0 BEGIN
	/* 41B5A0 */ uchar CGaming_RetestTraceAreasForEntity(int* param_1);
	// !DECL 0x0041b5a0 END
	// !DECL 0x0041b620 BEGIN
	/* 41B620 */ uchar CGaming_SeedMatchSchedulerEvents(void* param_1);
	// !DECL 0x0041b620 END
	// !DECL 0x0041b850 BEGIN
	/* 41B850 */ uchar CGaming_dtor(void* param_1);
	// !DECL 0x0041b850 END
	// !DECL 0x0041bab0 BEGIN
	/* 41BAB0 */ uchar CGaming_EnqueuePreMatchSchedulerSlots(void* param_1);
	// !DECL 0x0041bab0 END
	// !DECL 0x0041bee0 BEGIN
	/* 41BEE0 */ void* CGaming_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0041bee0 END
	// !DECL 0x0041c140 BEGIN
	/* 41C140 */ uchar CGaming_OnResumeOrStartGame(char param_1);
	// !DECL 0x0041c140 END
	// !DECL 0x0041c290 BEGIN
	/* 41C290 */ uchar CGaming_RunPreMatchModal(void* param_1);
	// !DECL 0x0041c290 END
	// !DECL 0x0041d5f0 BEGIN
	/* 41D5F0 */ uchar CGaming_OnCmd(ushort param_1, int param_2);
	// !DECL 0x0041d5f0 END
	// !DECL 0x0041e350 BEGIN
	/* 41E350 */ int CGaming_RandomPickupSpawner_Tick(void* param_1);
	// !DECL 0x0041e350 END
	// !DECL 0x0041f050 BEGIN
	/* 41F050 */ uchar CGaming_OnSchedulerTimer(int param_1);
	// !DECL 0x0041f050 END
	// !DECL 0x00420480 BEGIN
	/* 420480 */ uchar CGaming_OnCustomEvent_0xF6_RespawnPlayer(int* param_1);
	// !DECL 0x00420480 END
	// !DECL 0x004206a0 BEGIN
	/* 4206A0 */ uchar CGaming_OnCustomEvent(ushort param_1, int* param_2, uint param_3);
	// !DECL 0x004206a0 END
	// !DECL 0x00429d90 BEGIN
	/* 429D90 */ uchar CGaming_ClearSchedulerSlotFlags(int param_1);
	// !DECL 0x00429d90 END
	// !DECL 0x00429f70 BEGIN
	/* 429F70 */ uchar CGaming_SyncKeyLatchAfterModal(uint* param_1);
	// !DECL 0x00429f70 END
	// !DECL 0x00439880 BEGIN
	/* 439880 */ void CGaming_ArmTrackMgrSchedulerIfUnpaused(CDSVideoPlayer* param_1);
	// !DECL 0x00439880 END
	// !DECL 0x00439970 BEGIN
	/* 439970 */ uchar CGaming_PauseClockIfActive(CDSVideoPlayer* param_1);
	// !DECL 0x00439970 END
	// !DECL 0x0043aa70 BEGIN
	/* 43AA70 */ uchar CGaming_PreMatchModal_RestartDeferredAudio(void* param_1);
	// !DECL 0x0043aa70 END

	// !DECL 0x004168c0 BEGIN
	/* 4168C0 */ uchar CGaming_SetEntityRegisterMode(uchar param_1);
	// !DECL 0x004168c0 END
	// !DECL 0x004180e0 BEGIN
	/* 4180E0 */ void CGaming_OnKillCredit_UpdateScoreHud(CBulanek* param_1);
	// !DECL 0x004180e0 END
	// !DECL 0x004183d0 BEGIN
	/* 4183D0 */ int CExplosion_CollectEntitiesInBlastRect(int* param_1, int* param_2, char param_3);
	// !DECL 0x004183d0 END
	// !DECL 0x004184a0 BEGIN
	/* 4184A0 */ uchar CGaming_InsertEntityByDepth(void* param_1);
	// !DECL 0x004184a0 END
	// !DECL 0x0041a020 BEGIN
	/* 41A020 */ void CGaming_OnPlayerCollectItem(void* param_1, void* param_2, uint param_3);
	// !DECL 0x0041a020 END
	// !DECL 0x0041a140 BEGIN
	/* 41A140 */ uchar CGaming_RespawnPlayerAtSafeLocation(CBulanek* param_1);
	// !DECL 0x0041a140 END
	// !DECL 0x0041a2f0 BEGIN
	/* 41A2F0 */ int CExplosion_CollectLandminesInBlastRect(int* param_1, int* param_2);
	// !DECL 0x0041a2f0 END
	// !DECL 0x0041a390 BEGIN
	/* 41A390 */ void CGaming_AddEntity(void* param_1, char param_2);
	// !DECL 0x0041a390 END
	// !DECL 0x0041b420 BEGIN
	/* 41B420 */ uint* CGaming_AppendDangerZoneNode(uint param_1, uint* param_2, uint param_3, int param_4);
	// !DECL 0x0041b420 END
	// !DECL 0x0041b6d0 BEGIN
	/* 41B6D0 */ uchar CGaming_LoadBackgroundMusic(uchar param_1);
	// !DECL 0x0041b6d0 END
	// !DECL 0x0041ba60 BEGIN
	/* 41BA60 */ uchar CGaming_SetMusicAndSoundBank(uint param_1, uint param_2);
	// !DECL 0x0041ba60 END
	// !DECL 0x0041ff90 BEGIN
	/* 41FF90 */ void* CGaming_ctor(CGame* param_1);
	// !DECL 0x0041ff90 END
};

#endif
