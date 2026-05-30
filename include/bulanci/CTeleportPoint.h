#ifndef _C_TELEPORT_POINT
#define _C_TELEPORT_POINT

#include <globals.h>

class CTeleportPoint {
public:
	// !DECL 0x0041a920 BEGIN
	/* 41A920 */ uchar CTeleportPoint_InitVtables(uchar param_1);
	// !DECL 0x0041a920 END
	// !DECL 0x0041a970 BEGIN
	/* 41A970 */ uchar* CTeleportPoint_GetClassTable();
	// !DECL 0x0041a970 END
	// !DECL 0x0041c9a0 BEGIN
	/* 41C9A0 */ void* CTeleportPoint_Ctor(int* param_1, uint param_2, int param_3);
	// !DECL 0x0041c9a0 END
	// !DECL 0x0041fca0 BEGIN
	/* 41FCA0 */ void TriggerTeleportFX(void* param_1);
	// !DECL 0x0041fca0 END
	// !DECL 0x0041fed0 BEGIN
	/* 41FED0 */ uchar OnEvent(uint param_1, short param_2);
	// !DECL 0x0041fed0 END
};

#endif
