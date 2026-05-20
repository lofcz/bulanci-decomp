#ifndef _C_GAMING
#define _C_GAMING

#include <globals.h>

class CGaming {
public:
	// !DECL 0x004033b0 BEGIN
	/* 4033B0 */ bool FUN_004033b0(uint param_1);
	// !DECL 0x004033b0 END
	// !DECL 0x00412520 BEGIN
	/* 412520 */ uchar FUN_00412520(int param_1);
	// !DECL 0x00412520 END
	// !DECL 0x004129c0 BEGIN
	/* 4129C0 */ uchar FUN_004129c0(int param_1);
	// !DECL 0x004129c0 END
	// !DECL 0x00412fa0 BEGIN
	/* 412FA0 */ uchar FUN_00412fa0(void* param_1);
	// !DECL 0x00412fa0 END
	// !DECL 0x00413180 BEGIN
	/* 413180 */ uchar CGame_NetSendRoundResult_t0b(char param_1, short param_2);
	// !DECL 0x00413180 END
	// !DECL 0x00413390 BEGIN
	/* 413390 */ uchar FUN_00413390(char param_1, char param_2);
	// !DECL 0x00413390 END
	// !DECL 0x00415f80 BEGIN
	/* 415F80 */ uchar CGaming_DrainRecordedNetMessages(void* param_1);
	// !DECL 0x00415f80 END
	// !DECL 0x00416810 BEGIN
	/* 416810 */ uint CGaming_GetObjectAtSlotSafe(uchar param_1);
	// !DECL 0x00416810 END
	// !DECL 0x00416860 BEGIN
	/* 416860 */ uchar FUN_00416860(char param_1);
	// !DECL 0x00416860 END
	// !DECL 0x004168a0 BEGIN
	/* 4168A0 */ uchar FUN_004168a0(char param_1);
	// !DECL 0x004168a0 END
	// !DECL 0x004168e0 BEGIN
	/* 4168E0 */ bool FUN_004168e0(int param_1);
	// !DECL 0x004168e0 END
	// !DECL 0x004168f0 BEGIN
	/* 4168F0 */ uchar FUN_004168f0(int param_1);
	// !DECL 0x004168f0 END
	// !DECL 0x00416940 BEGIN
	/* 416940 */ int FUN_00416940(int param_1);
	// !DECL 0x00416940 END
	// !DECL 0x00419ca0 BEGIN
	/* 419CA0 */ uchar CGaming_UnregisterAndRemoveObject(int* param_1);
	// !DECL 0x00419ca0 END
	// !DECL 0x00419d20 BEGIN
	/* 419D20 */ uchar CGaming_ClearAllEntities(void* param_1);
	// !DECL 0x00419d20 END
	// !DECL 0x00419e40 BEGIN
	/* 419E40 */ uchar FUN_00419e40(void* param_1);
	// !DECL 0x00419e40 END
	// !DECL 0x0041a270 BEGIN
	/* 41A270 */ uchar FUN_0041a270(uint* param_1);
	// !DECL 0x0041a270 END
	// !DECL 0x0041a2a0 BEGIN
	/* 41A2A0 */ uchar CGaming_CleanupInactiveBullets(void* param_1);
	// !DECL 0x0041a2a0 END
	// !DECL 0x0041ab70 BEGIN
	/* 41AB70 */ uint* FUN_0041ab70(uint* param_1);
	// !DECL 0x0041ab70 END
	// !DECL 0x0041aca0 BEGIN
	/* 41ACA0 */ uchar* FUN_0041aca0();
	// !DECL 0x0041aca0 END
	// !DECL 0x0041acb0 BEGIN
	/* 41ACB0 */ uchar FUN_0041acb0(uchar param_1);
	// !DECL 0x0041acb0 END
	// !DECL 0x0041acc0 BEGIN
	/* 41ACC0 */ uchar FUN_0041acc0(uchar param_1);
	// !DECL 0x0041acc0 END
	// !DECL 0x0041acd0 BEGIN
	/* 41ACD0 */ uchar FUN_0041acd0(uchar param_1);
	// !DECL 0x0041acd0 END
	// !DECL 0x0041ace0 BEGIN
	/* 41ACE0 */ uchar FUN_0041ace0(uchar param_1);
	// !DECL 0x0041ace0 END
	// !DECL 0x0041b3b0 BEGIN
	/* 41B3B0 */ uchar FUN_0041b3b0(int param_1);
	// !DECL 0x0041b3b0 END
	// !DECL 0x0041b500 BEGIN
	/* 41B500 */ uchar CGaming_TickAmbientAnimations(void* param_1);
	// !DECL 0x0041b500 END
	// !DECL 0x0041b5a0 BEGIN
	/* 41B5A0 */ uchar FUN_0041b5a0(int* param_1);
	// !DECL 0x0041b5a0 END
	// !DECL 0x0041b620 BEGIN
	/* 41B620 */ uchar FUN_0041b620(void* param_1);
	// !DECL 0x0041b620 END
	// !DECL 0x0041b850 BEGIN
	/* 41B850 */ uchar CGaming_dtor(uint* param_1);
	// !DECL 0x0041b850 END
	// !DECL 0x0041bab0 BEGIN
	/* 41BAB0 */ uchar FUN_0041bab0(void* param_1);
	// !DECL 0x0041bab0 END
	// !DECL 0x0041bee0 BEGIN
	/* 41BEE0 */ uint* FUN_0041bee0(uchar param_1);
	// !DECL 0x0041bee0 END
	// !DECL 0x0041c140 BEGIN
	/* 41C140 */ uchar CGaming_OnResumeOrStartGame(char param_1);
	// !DECL 0x0041c140 END
	// !DECL 0x0041c290 BEGIN
	/* 41C290 */ uchar FUN_0041c290(void* param_1);
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
	/* 420480 */ uchar FUN_00420480(int* param_1);
	// !DECL 0x00420480 END
	// !DECL 0x004206a0 BEGIN
	/* 4206A0 */ uchar CGaming_OnCustomEvent(ushort param_1, int* param_2, uint param_3);
	// !DECL 0x004206a0 END
	// !DECL 0x00429d90 BEGIN
	/* 429D90 */ uchar FUN_00429d90(int param_1);
	// !DECL 0x00429d90 END
	// !DECL 0x00429f70 BEGIN
	/* 429F70 */ uchar FUN_00429f70(uint* param_1);
	// !DECL 0x00429f70 END
	// !DECL 0x00439880 BEGIN
	/* 439880 */ uchar FUN_00439880(int param_1);
	// !DECL 0x00439880 END
	// !DECL 0x00439970 BEGIN
	/* 439970 */ uchar FUN_00439970(int param_1);
	// !DECL 0x00439970 END
	// !DECL 0x0043aa70 BEGIN
	/* 43AA70 */ uchar FUN_0043aa70(void* param_1);
	// !DECL 0x0043aa70 END
};

#endif
