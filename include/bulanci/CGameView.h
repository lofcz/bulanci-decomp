#ifndef _C_GAME_VIEW
#define _C_GAME_VIEW

#include <globals.h>

class CGameView {
public:
	// !DECL 0x00416f30 BEGIN
	/* 416F30 */ uchar* CGameView_GetClassTable();
	// !DECL 0x00416f30 END
	// !DECL 0x00416f50 BEGIN
	/* 416F50 */ int CGameView_GetSpatialBucketKey();
	// !DECL 0x00416f50 END
	// !DECL 0x00417030 BEGIN
	/* 417030 */ uchar CShot_SubobjectCtor(uchar param_1);
	// !DECL 0x00417030 END
	// !DECL 0x00417210 BEGIN
	/* 417210 */ uchar CGameView_GetWorldCollisionRect(int* param_1);
	// !DECL 0x00417210 END
	// !DECL 0x00417f40 BEGIN
	/* 417F40 */ uint FUN_00417f40(uchar param_1, int param_2, uchar param_3);
	// !DECL 0x00417f40 END
	// !DECL 0x00418290 BEGIN
	/* 418290 */ uchar CLevelScript_FireOnSlotPlaced_FromView(uchar param_1, uchar param_2, uchar param_3);
	// !DECL 0x00418290 END
	// !DECL 0x004182d0 BEGIN
	/* 4182D0 */ uchar CGameView_FireOnSlotDisplaced(uchar param_1, uchar param_2);
	// !DECL 0x004182d0 END
	// !DECL 0x00418c60 BEGIN
	/* 418C60 */ uint* CGameView_CreateObject();
	// !DECL 0x00418c60 END
	// !DECL 0x00418d70 BEGIN
	/* 418D70 */ uint* FUN_00418d70(uint* param_1);
	// !DECL 0x00418d70 END
	// !DECL 0x00419010 BEGIN
	/* 419010 */ uchar CGameView_Update(int* param_1);
	// !DECL 0x00419010 END
	// !DECL 0x004191a0 BEGIN
	/* 4191A0 */ void* CGameView_ctor(uint param_1, uint param_2, int* param_3);
	// !DECL 0x004191a0 END
	// !DECL 0x0041acf0 BEGIN
	/* 41ACF0 */ uchar CGameView_OnEvent(short param_1, uint param_2, uint* param_3);
	// !DECL 0x0041acf0 END
	// !DECL 0x0041e4b0 BEGIN
	/* 41E4B0 */ void* CBulanekCtor(uchar* param_1, int param_2, void* param_3, int param_4, void* param_5, int param_6, int param_7, int param_8);
	// !DECL 0x0041e4b0 END
	// !DECL 0x00439720 BEGIN
	/* 439720 */ void TM_SetFrameDelayOverrideMs(int param_1);
	// !DECL 0x00439720 END
};

#endif
