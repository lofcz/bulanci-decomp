#ifndef _C_GAME
#define _C_GAME

#include <globals.h>

class CGame {
public:
	// !DECL 0x00401d00 BEGIN
	/* 401D00 */ void* CGame_InitWidePathFromMbcs(CHAR* param_1);
	// !DECL 0x00401d00 END
	// !DECL 0x0040a840 BEGIN
	/* 40A840 */ uchar CGame_FocusLobbyScroller(int param_1);
	// !DECL 0x0040a840 END
	// !DECL 0x00412800 BEGIN
	/* 412800 */ uchar CGame_OnSysMsg_DestroyPlayer(int param_1);
	// !DECL 0x00412800 END
	// !DECL 0x00412da0 BEGIN
	/* 412DA0 */ void CGame_NetSendCountdown_t07(char param_1);
	// !DECL 0x00412da0 END
	// !DECL 0x00413120 BEGIN
	/* 413120 */ int CGame_FindScheduledEventIndexByKey(uchar param_1, uchar param_2);
	// !DECL 0x00413120 END
	// !DECL 0x00413160 BEGIN
	/* 413160 */ uint CGame_GetControlBindingTable(uchar param_1);
	// !DECL 0x00413160 END
	// !DECL 0x00413840 BEGIN
	/* 413840 */ uint CGame_RemoveScheduledEventByKey(uchar param_1, uchar param_2, char param_3);
	// !DECL 0x00413840 END
	// !DECL 0x00413880 BEGIN
	/* 413880 */ uint CGame_FindScheduledEventByKey(uchar param_1, uchar param_2);
	// !DECL 0x00413880 END
	// !DECL 0x004138b0 BEGIN
	/* 4138B0 */ uchar CGame_NetSendOpposingEvent_t17(uint param_1, uint param_2, uint param_3);
	// !DECL 0x004138b0 END
	// !DECL 0x00413b90 BEGIN
	/* 413B90 */ uchar CGame_NetSendChat_t06(uchar param_1, int* param_2, void* param_3);
	// !DECL 0x00413b90 END
	// !DECL 0x004144f0 BEGIN
	/* 4144F0 */ uchar CGame_DeferTeamScoreSchedulerEvent(uchar param_1, uchar param_2, uint param_3, char param_4);
	// !DECL 0x004144f0 END
	// !DECL 0x00414550 BEGIN
	/* 414550 */ uchar CGame_NetSendTeamScoreEvent_t16(uint param_1, uint param_2, uint param_3);
	// !DECL 0x00414550 END
	// !DECL 0x00414c60 BEGIN
	/* 414C60 */ uchar* CGame_GetTypeInfo();
	// !DECL 0x00414c60 END
	// !DECL 0x00414c70 BEGIN
	/* 414C70 */ uchar CGame_ScalarDeletingDtor_thunk_Sub1c(uchar param_1);
	// !DECL 0x00414c70 END
	// !DECL 0x00414c80 BEGIN
	/* 414C80 */ uchar CGame_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x00414c80 END
	// !DECL 0x00414c90 BEGIN
	/* 414C90 */ uchar* CGame_GetTypeInfo_EventHandler();
	// !DECL 0x00414c90 END
	// !DECL 0x00414ca0 BEGIN
	/* 414CA0 */ void CGame_dtor(uchar param_1);
	// !DECL 0x00414ca0 END
	// !DECL 0x00415220 BEGIN
	/* 415220 */ uchar CGame_RebuildLevelListFromPlayerRecs(uint* param_1, int param_2);
	// !DECL 0x00415220 END
	// !DECL 0x00415290 BEGIN
	/* 415290 */ uchar CGame_ProcessNetMessage(int param_1, int* param_2);
	// !DECL 0x00415290 END
	// !DECL 0x00415f60 BEGIN
	/* 415F60 */ void* CGame_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00415f60 END
	// !DECL 0x00416407 BEGIN
	/* 416407 */ uint Catch_00416407();
	// !DECL 0x00416407 END
	// !DECL 0x004169a0 BEGIN
	/* 4169A0 */ uchar CGame_PostTeamScoreHudEvent_0xEE(uchar param_1, uchar param_2, uchar param_3);
	// !DECL 0x004169a0 END
	// !DECL 0x00416a10 BEGIN
	/* 416A10 */ uchar CGame_PostOpposingHudEvent_0xEF(uchar param_1, uchar param_2, uchar param_3);
	// !DECL 0x00416a10 END
	// !DECL 0x004180c0 BEGIN
	/* 4180C0 */ uchar CGaming_OnNetMsg_t11_PlayerDie(uchar param_1);
	// !DECL 0x004180c0 END
	// !DECL 0x004185a0 BEGIN
	/* 4185A0 */ uchar CGame_OnNetMsg_t15_FireOnNetCustom(uint param_1);
	// !DECL 0x004185a0 END
	// !DECL 0x0041a0c0 BEGIN
	/* 41A0C0 */ uchar CGame_DestroyObjectAtSlot(uchar param_1);
	// !DECL 0x0041a0c0 END
	// !DECL 0x0041d280 BEGIN
	/* 41D280 */ uchar CGame_OnNetMsg_t18_PlaceWorldPickup(uint param_1, uint param_2, int* param_3);
	// !DECL 0x0041d280 END
	// !DECL 0x0041d2c0 BEGIN
	/* 41D2C0 */ uchar CGame_OnNetMsg_t13_PlaceSpecialPickup(int* param_1);
	// !DECL 0x0041d2c0 END
	// !DECL 0x0041f010 BEGIN
	/* 41F010 */ uchar CGame_OnNetMsg_t19_PlayerPickedUpWorldObj(uchar param_1, uchar param_2, uchar param_3);
	// !DECL 0x0041f010 END
	// !DECL 0x0041f030 BEGIN
	/* 41F030 */ uchar CGame_OnNetMsg_t14_PlayerPickedUpSpecial(uchar param_1);
	// !DECL 0x0041f030 END
	// !DECL 0x00420510 BEGIN
	/* 420510 */ uchar CGaming_OnNetMsg_t10_Hit(uchar param_1, uchar param_2, int* param_3);
	// !DECL 0x00420510 END
	// !DECL 0x004209b0 BEGIN
	/* 4209B0 */ uint CGame_DispatchPlayerAction(uchar param_1, uchar param_2, char param_3);
	// !DECL 0x004209b0 END
	// !DECL 0x004209f0 BEGIN
	/* 4209F0 */ uchar CGaming_OnNetMsg_t0d_PlayerState(uchar param_1, uchar param_2, int* param_3);
	// !DECL 0x004209f0 END
	// !DECL 0x00420a70 BEGIN
	/* 420A70 */ uchar CGame_OnNetMsg_t0e_PrimaryAction(uchar param_1);
	// !DECL 0x00420a70 END
	// !DECL 0x00428e20 BEGIN
	/* 428E20 */ uchar CDSChain_AdjustThisOffset_ThisMinus18(int param_1);
	// !DECL 0x00428e20 END
	// !DECL 0x0042eff0 BEGIN
	/* 42EFF0 */ uchar BroadcastEvent(ushort param_1, uint param_2, uint param_3, void* param_4);
	// !DECL 0x0042eff0 END
	// !DECL 0x0043ab70 BEGIN
	/* 43AB70 */ uchar CDSDirectPlay_Receive(uint param_1, uint param_2, uint param_3);
	// !DECL 0x0043ab70 END
	// !DECL 0x0043ac00 BEGIN
	/* 43AC00 */ void CDSDirectPlay_EnumSessions();
	// !DECL 0x0043ac00 END
	// !DECL 0x0043ca40 BEGIN
	/* 43CA40 */ uchar CDSObject_ReleaseViaVtable_ThisMinus1c(int param_1);
	// !DECL 0x0043ca40 END

	// !DECL 0x00413030 BEGIN
	/* 413030 */ short CMenu_DoModalChild(void* param_1, char param_2);
	// !DECL 0x00413030 END
	// !DECL 0x00413ce0 BEGIN
	/* 413CE0 */ void CGame_StartGame();
	// !DECL 0x00413ce0 END
	// !DECL 0x00414170 BEGIN
	/* 414170 */ uchar CMenu_PickSession(uint* param_1);
	// !DECL 0x00414170 END
	// !DECL 0x00414280 BEGIN
	/* 414280 */ void CGame_GetOrCreateLevelScore();
	// !DECL 0x00414280 END
	// !DECL 0x00414790 BEGIN
	/* 414790 */ void CMenu_ShowLobby(uchar param_1);
	// !DECL 0x00414790 END
};

#endif
