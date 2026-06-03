#ifndef _C_D_S_MEM_QUEUE
#define _C_D_S_MEM_QUEUE

#include <globals.h>

class CDSMemQueue {
public:
	// !DECL 0x0043c3a0 BEGIN
	/* 43C3A0 */ uint CDSMemQueue_InitDefault(uint* param_1);
	// !DECL 0x0043c3a0 END
	// !DECL 0x0043c3c0 BEGIN
	/* 43C3C0 */ uchar* CDSMemQueue_GetTypeInfo();
	// !DECL 0x0043c3c0 END
	// !DECL 0x0043c3d0 BEGIN
	/* 43C3D0 */ uchar CDSMemQueue_dtor(uint* param_1);
	// !DECL 0x0043c3d0 END
	// !DECL 0x0043c570 BEGIN
	/* 43C570 */ void* CDSMemQueue_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0043c570 END
};

#endif
