#ifndef _C_GAME
#define _C_GAME

#include <globals.h>

class CGame {
public:
	// !DECL 0x00401d00 BEGIN
	/* 401D00 */ void* FUN_00401d00(int param_1);
	// !DECL 0x00401d00 END
	// !DECL 0x0040a840 BEGIN
	/* 40A840 */ uchar FUN_0040a840(int param_1);
	// !DECL 0x0040a840 END
	// !DECL 0x0040c570 BEGIN
	/* 40C570 */ uchar FUN_0040c570(uchar* param_1);
	// !DECL 0x0040c570 END
	// !DECL 0x0040c5d0 BEGIN
	/* 40C5D0 */ uchar FUN_0040c5d0(int param_1);
	// !DECL 0x0040c5d0 END
	// !DECL 0x00412160 BEGIN
	/* 412160 */ uchar FUN_00412160(int param_1);
	// !DECL 0x00412160 END
	// !DECL 0x004121a0 BEGIN
	/* 4121A0 */ char FUN_004121a0(uchar param_1);
	// !DECL 0x004121a0 END
	// !DECL 0x00412800 BEGIN
	/* 412800 */ uchar CGame_OnSysMsg_DestroyPlayer(int param_1);
	// !DECL 0x00412800 END
	// !DECL 0x00412da0 BEGIN
	/* 412DA0 */ uchar CGame_NetSendCountdown_t07(char param_1);
	// !DECL 0x00412da0 END
	// !DECL 0x00413120 BEGIN
	/* 413120 */ int FUN_00413120(uchar param_1, uchar param_2);
	// !DECL 0x00413120 END
	// !DECL 0x00413160 BEGIN
	/* 413160 */ uint FUN_00413160(uchar param_1);
	// !DECL 0x00413160 END
	// !DECL 0x00413840 BEGIN
	/* 413840 */ uint FUN_00413840(uchar param_1, uchar param_2, char param_3);
	// !DECL 0x00413840 END
	// !DECL 0x00413880 BEGIN
	/* 413880 */ uint FUN_00413880(uchar param_1, uchar param_2);
	// !DECL 0x00413880 END
	// !DECL 0x004138b0 BEGIN
	/* 4138B0 */ uchar CGame_NetSendOpposingEvent_t17(uint param_1, uint param_2, uint param_3);
	// !DECL 0x004138b0 END
	// !DECL 0x00413b90 BEGIN
	/* 413B90 */ uchar NetSendChat(uchar param_1, int* param_2, void* param_3);
	// !DECL 0x00413b90 END
	// !DECL 0x004144f0 BEGIN
	/* 4144F0 */ uchar FUN_004144f0(uchar param_1, uchar param_2, uint param_3, char param_4);
	// !DECL 0x004144f0 END
	// !DECL 0x00414550 BEGIN
	/* 414550 */ uchar CGame_NetSendTeamScoreEvent_t16(uint param_1, uint param_2, uint param_3);
	// !DECL 0x00414550 END
	// !DECL 0x00414c60 BEGIN
	/* 414C60 */ uchar* FUN_00414c60();
	// !DECL 0x00414c60 END
	// !DECL 0x00414c70 BEGIN
	/* 414C70 */ uchar FUN_00414c70(uchar param_1);
	// !DECL 0x00414c70 END
	// !DECL 0x00414c80 BEGIN
	/* 414C80 */ uchar FUN_00414c80(uchar param_1);
	// !DECL 0x00414c80 END
	// !DECL 0x00414c90 BEGIN
	/* 414C90 */ uchar* FUN_00414c90();
	// !DECL 0x00414c90 END
	// !DECL 0x00414ca0 BEGIN
	/* 414CA0 */ uchar FUN_00414ca0(uint* param_1);
	// !DECL 0x00414ca0 END
	// !DECL 0x00415220 BEGIN
	/* 415220 */ uchar FUN_00415220(uint* param_1, int param_2);
	// !DECL 0x00415220 END
	// !DECL 0x00415290 BEGIN
	/* 415290 */ uchar CGame_ProcessNetMessage(int param_1, int* param_2);
	// !DECL 0x00415290 END
	// !DECL 0x00415f60 BEGIN
	/* 415F60 */ void* FUN_00415f60(uchar param_1);
	// !DECL 0x00415f60 END
	// !DECL 0x00416030 BEGIN
	/* 416030 */ uchar FUN_00416030(uint param_1);
	// !DECL 0x00416030 END
	// !DECL 0x00416407 BEGIN
	/* 416407 */ uint Catch_00416407();
	// !DECL 0x00416407 END
	// !DECL 0x004169a0 BEGIN
	/* 4169A0 */ uchar FUN_004169a0(uchar param_1, uchar param_2, uchar param_3);
	// !DECL 0x004169a0 END
	// !DECL 0x00416a10 BEGIN
	/* 416A10 */ uchar FUN_00416a10(uchar param_1, uchar param_2, uchar param_3);
	// !DECL 0x00416a10 END
	// !DECL 0x004180c0 BEGIN
	/* 4180C0 */ uchar FUN_004180c0(uchar param_1);
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
	/* 41F010 */ uchar CGame_OnNetMsg_t19_PlayerPickedUpWorldObj(uchar param_1, ODSImage param_2, uchar param_3);
	// !DECL 0x0041f010 END
	// !DECL 0x0041f030 BEGIN
	/* 41F030 */ uchar CGame_OnNetMsg_t14_PlayerPickedUpSpecial(uchar param_1);
	// !DECL 0x0041f030 END
	// !DECL 0x0041f210 BEGIN
	/* 41F210 */ uchar FUN_0041f210(uchar param_1, ODSImage param_2);
	// !DECL 0x0041f210 END
	// !DECL 0x00420510 BEGIN
	/* 420510 */ uchar FUN_00420510(uchar param_1, uchar param_2, int* param_3);
	// !DECL 0x00420510 END
	// !DECL 0x004209b0 BEGIN
	/* 4209B0 */ uint FUN_004209b0(uchar param_1, uchar param_2, char param_3);
	// !DECL 0x004209b0 END
	// !DECL 0x004209f0 BEGIN
	/* 4209F0 */ uchar FUN_004209f0(uchar param_1, uchar param_2, int* param_3);
	// !DECL 0x004209f0 END
	// !DECL 0x00420a70 BEGIN
	/* 420A70 */ uchar CGame_OnNetMsg_t0e_PrimaryAction(uchar param_1);
	// !DECL 0x00420a70 END
	// !DECL 0x00428e20 BEGIN
	/* 428E20 */ uchar FUN_00428e20(int param_1);
	// !DECL 0x00428e20 END
	// !DECL 0x0042eff0 BEGIN
	/* 42EFF0 */ uchar BroadcastEvent(ushort param_1, uint param_2, uint param_3, void* param_4);
	// !DECL 0x0042eff0 END
	// !DECL 0x0043ab70 BEGIN
	/* 43AB70 */ uchar CDSDirectPlay_Receive(uint param_1, uint param_2, uint param_3);
	// !DECL 0x0043ab70 END
	// !DECL 0x0043ac00 BEGIN
	/* 43AC00 */ uchar CDSDirectPlay_EnumSessions(int param_1, uint param_2);
	// !DECL 0x0043ac00 END
	// !DECL 0x0043ca40 BEGIN
	/* 43CA40 */ uchar FUN_0043ca40(int param_1);
	// !DECL 0x0043ca40 END
};

#endif
