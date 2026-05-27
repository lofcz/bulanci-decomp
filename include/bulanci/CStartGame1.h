#ifndef _C_START_GAME1
#define _C_START_GAME1

#include <globals.h>

class CStartGame1 {
public:
	// !DECL 0x00401280 BEGIN
	/* 401280 */ int FUN_00401280();
	// !DECL 0x00401280 END
	// !DECL 0x0040a860 BEGIN
	/* 40A860 */ uchar CStartGame1_ApplyHostJoinVisibility(int param_1);
	// !DECL 0x0040a860 END
	// !DECL 0x0040a970 BEGIN
	/* 40A970 */ uchar CStartGame1_ApplyJoinModeVisibility(char param_1);
	// !DECL 0x0040a970 END
	// !DECL 0x0040ae00 BEGIN
	/* 40AE00 */ uchar FUN_0040ae00(int param_1);
	// !DECL 0x0040ae00 END
	// !DECL 0x0040ae30 BEGIN
	/* 40AE30 */ uchar CStartGame1_OnRadioChange(short param_1, int param_2, uint param_3);
	// !DECL 0x0040ae30 END
	// !DECL 0x0040ae90 BEGIN
	/* 40AE90 */ uchar CStartGame1_OnEvent(ushort param_1);
	// !DECL 0x0040ae90 END
	// !DECL 0x0040c650 BEGIN
	/* 40C650 */ uchar CStartGame1_BuildUi(uchar param_1);
	// !DECL 0x0040c650 END
	// !DECL 0x0040f400 BEGIN
	/* 40F400 */ CWindow* CStartGame1_ctor(CWindow* param_1);
	// !DECL 0x0040f400 END
	// !DECL 0x0040f4b0 BEGIN
	/* 40F4B0 */ uchar* CStartGame1_GetSingleton();
	// !DECL 0x0040f4b0 END
	// !DECL 0x0040f4c0 BEGIN
	/* 40F4C0 */ uchar FUN_0040f4c0(uchar param_1);
	// !DECL 0x0040f4c0 END
	// !DECL 0x0040f4d0 BEGIN
	/* 40F4D0 */ uchar FUN_0040f4d0(uchar param_1);
	// !DECL 0x0040f4d0 END
	// !DECL 0x0040f4e0 BEGIN
	/* 40F4E0 */ uchar FUN_0040f4e0(uchar param_1);
	// !DECL 0x0040f4e0 END
	// !DECL 0x0040f4f0 BEGIN
	/* 40F4F0 */ uchar FUN_0040f4f0(uchar param_1);
	// !DECL 0x0040f4f0 END
	// !DECL 0x0040f500 BEGIN
	/* 40F500 */ uchar FUN_0040f500(uchar param_1);
	// !DECL 0x0040f500 END
	// !DECL 0x0040f560 BEGIN
	/* 40F560 */ void* FUN_0040f560(uchar param_1);
	// !DECL 0x0040f560 END
};

#endif
