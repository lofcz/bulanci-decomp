#ifndef _C_SESSION_ITEM
#define _C_SESSION_ITEM

#include <globals.h>

class CSessionItem {
public:
	// !DECL 0x0040b770 BEGIN
	/* 40B770 */ uchar* CSessionItem_GetTypeDescriptor();
	// !DECL 0x0040b770 END
	// !DECL 0x0040e8b0 BEGIN
	/* 40E8B0 */ void* CSessionItem_Initialize(DPEnumSessionInfo* param_1);
	// !DECL 0x0040e8b0 END
};

#endif
