#ifndef _C_D_S_BACK_BUFFER
#define _C_D_S_BACK_BUFFER

#include <globals.h>

class CDSBackBuffer {
public:
	// !DECL 0x0042ad20 BEGIN
	/* 42AD20 */ uchar* CDSBackBuffer_GetClassTable();
	// !DECL 0x0042ad20 END
	// !DECL 0x0042ad30 BEGIN
	/* 42AD30 */ uchar CDSBackBuffer_OnEvent(uchar param_1);
	// !DECL 0x0042ad30 END
	// !DECL 0x0042adb0 BEGIN
	/* 42ADB0 */ void* CDSBackBuffer_DtorScalar(uchar param_1);
	// !DECL 0x0042adb0 END
};

#endif
