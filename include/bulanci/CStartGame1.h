#ifndef _C_START_GAME1
#define _C_START_GAME1

#include <globals.h>

class CStartGame1 {
public:
	// !DECL 0x00401280 BEGIN
	/* 401280 */ int CStartGame1_GetEmbeddedCGamePtr();
	// !DECL 0x00401280 END
	// !DECL 0x0040a860 BEGIN
	/* 40A860 */ void CStartGame1_ApplyHostJoinVisibility(void* param_1);
	// !DECL 0x0040a860 END
	// !DECL 0x0040a970 BEGIN
	/* 40A970 */ uchar CStartGame1_ApplyJoinModeVisibility(char param_1);
	// !DECL 0x0040a970 END
	// !DECL 0x0040ae00 BEGIN
	/* 40AE00 */ uchar CStartGame1_UpdateModalChildVisibility(int param_1);
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
	/* 40F400 */ void* CStartGame1_ctor(void* param_1);
	// !DECL 0x0040f400 END
	// !DECL 0x0040f4b0 BEGIN
	/* 40F4B0 */ uchar* CStartGame1_GetSingleton();
	// !DECL 0x0040f4b0 END
	// !DECL 0x0040f4c0 BEGIN
	/* 40F4C0 */ uchar CStartGame1_ScalarDeletingDtor_thunk_Sub10(uchar param_1);
	// !DECL 0x0040f4c0 END
	// !DECL 0x0040f4d0 BEGIN
	/* 40F4D0 */ uchar CStartGame1_ScalarDeletingDtor_thunk_Sub70(uchar param_1);
	// !DECL 0x0040f4d0 END
	// !DECL 0x0040f4e0 BEGIN
	/* 40F4E0 */ uchar CStartGame1_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x0040f4e0 END
	// !DECL 0x0040f4f0 BEGIN
	/* 40F4F0 */ uchar CStartGame1_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x0040f4f0 END
	// !DECL 0x0040f500 BEGIN
	/* 40F500 */ uchar CStartGame1_dtor(uchar param_1);
	// !DECL 0x0040f500 END
	// !DECL 0x0040f560 BEGIN
	/* 40F560 */ uchar CStartGame1_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0040f560 END

	// !DECL 0x0040a8c0 BEGIN
	/* 40A8C0 */ uchar ApplyLocalPlayersConstraints(int param_1);
	// !DECL 0x0040a8c0 END
	// !DECL 0x0040a9d0 BEGIN
	/* 40A9D0 */ uchar OnTotalPlayersChange(int param_1);
	// !DECL 0x0040a9d0 END
	// !DECL 0x0040a9f0 BEGIN
	/* 40A9F0 */ uchar OnLocalPlayersChange(int param_1);
	// !DECL 0x0040a9f0 END
};


class CRadio {
public:
	// !DECL 0x00403d20 BEGIN
	/* 403D20 */ uchar SetItemDisabled(int param_1, char param_2);
	// !DECL 0x00403d20 END
};

#endif
