#ifndef _C_D_S_WORKING_THREAD
#define _C_D_S_WORKING_THREAD

#include <globals.h>

class CDSWorkingThread {
public:
	// !DECL 0x00447210 BEGIN
	/* 447210 */ uchar* CDSWorkingThread_GetClassTable();
	// !DECL 0x00447210 END
	// !DECL 0x00447220 BEGIN
	/* 447220 */ uchar CDSWorkingThread_OnTimer(int param_1);
	// !DECL 0x00447220 END
	// !DECL 0x00447230 BEGIN
	/* 447230 */ uchar CDSWorkingThread_OnChainEvent(uchar param_1);
	// !DECL 0x00447230 END
	// !DECL 0x00447240 BEGIN
	/* 447240 */ uchar CDSWorkingThread_dtor(uint* param_1);
	// !DECL 0x00447240 END
	// !DECL 0x004472e0 BEGIN
	/* 4472E0 */ void* CDSWorkingThread_DtorScalar(uchar param_1);
	// !DECL 0x004472e0 END
};

#endif
