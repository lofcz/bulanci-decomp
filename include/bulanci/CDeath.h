#ifndef _C_DEATH
#define _C_DEATH

#include <globals.h>

class CDeath {
public:
	// !DECL 0x00416650 BEGIN
	/* 416650 */ uchar CDeath_UpdateLastManStandingState(int param_1);
	// !DECL 0x00416650 END
	// !DECL 0x00417a40 BEGIN
	/* 417A40 */ uchar FUN_00417a40(uint param_1, int param_2, int param_3);
	// !DECL 0x00417a40 END
	// !DECL 0x00417a70 BEGIN
	/* 417A70 */ uchar CDeath_UpdateStateFromParams(uint param_1, uint param_2, int param_3);
	// !DECL 0x00417a70 END
	// !DECL 0x00417a90 BEGIN
	/* 417A90 */ uchar FUN_00417a90(int param_1);
	// !DECL 0x00417a90 END
	// !DECL 0x00419a80 BEGIN
	/* 419A80 */ uchar* CDeath_GetClassTable();
	// !DECL 0x00419a80 END
	// !DECL 0x0041a860 BEGIN
	/* 41A860 */ uchar CDeath_SubobjectCtor(uchar param_1);
	// !DECL 0x0041a860 END
};

#endif
