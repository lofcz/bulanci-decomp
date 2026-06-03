#ifndef _C_D_S_RESOURCE_EXCEPTION
#define _C_D_S_RESOURCE_EXCEPTION

#include <globals.h>

class CDSResourceException {
public:
	// !DECL 0x00434350 BEGIN
	/* 434350 */ char* CDSResourceException_What(wchar_t* param_1);
	// !DECL 0x00434350 END
	// !DECL 0x00434430 BEGIN
	/* 434430 */ uchar* CDSResourceException_GetClassTable();
	// !DECL 0x00434430 END
	// !DECL 0x00434440 BEGIN
	/* 434440 */ void CDSResourceException_dtor(uchar param_1);
	// !DECL 0x00434440 END
	// !DECL 0x004344a0 BEGIN
	/* 4344A0 */ void* CDSResourceException_ctor_default(void* param_1);
	// !DECL 0x004344a0 END
	// !DECL 0x004348e0 BEGIN
	/* 4348E0 */ void* CDSResourceException_DtorScalar(uchar param_1);
	// !DECL 0x004348e0 END

	// !DECL 0x00434400 BEGIN
	/* 434400 */ void CDSResourceException_ctor(uint param_1);
	// !DECL 0x00434400 END
};

#endif
