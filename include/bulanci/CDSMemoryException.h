#ifndef _C_D_S_MEMORY_EXCEPTION
#define _C_D_S_MEMORY_EXCEPTION

#include <globals.h>

class CDSMemoryException {
public:
	// !DECL 0x004349c0 BEGIN
	/* 4349C0 */ void* CDSMemoryException_What(wchar_t* param_1);
	// !DECL 0x004349c0 END
	// !DECL 0x00434a70 BEGIN
	/* 434A70 */ CDSException* CDSMemoryException_ctor(CDSException* param_1);
	// !DECL 0x00434a70 END
	// !DECL 0x00434a90 BEGIN
	/* 434A90 */ uchar* CDSMemoryException_GetClassTable();
	// !DECL 0x00434a90 END
};

#endif
