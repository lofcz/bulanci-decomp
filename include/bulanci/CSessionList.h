#ifndef _C_SESSION_LIST
#define _C_SESSION_LIST

#include <globals.h>

class CSessionList {
public:
	// !DECL 0x0040adc0 BEGIN
	/* 40ADC0 */ uchar CSessionList_OnEvent(short param_1, uint param_2, uint param_3);
	// !DECL 0x0040adc0 END
	// !DECL 0x0040c2d0 BEGIN
	/* 40C2D0 */ CWindow* CSessionList_BuildDialog(CWindow* param_1);
	// !DECL 0x0040c2d0 END
	// !DECL 0x0040c530 BEGIN
	/* 40C530 */ uchar* CSessionList_GetTypeDescriptor();
	// !DECL 0x0040c530 END
};

#endif
