#ifndef _C_D_S_REG_KEY_EXCEPTION
#define _C_D_S_REG_KEY_EXCEPTION

#include <globals.h>

class CDSRegKeyException {
public:
	// !DECL 0x00437b20 BEGIN
	/* 437B20 */ uchar* CDSRegKeyException_GetClassTable();
	// !DECL 0x00437b20 END
	// !DECL 0x00437ba0 BEGIN
	/* 437BA0 */ void* CDSRegKeyException_Ctor(uint param_1, int param_2);
	// !DECL 0x00437ba0 END
	// !DECL 0x00437c40 BEGIN
	/* 437C40 */ wchar_t* CDSRegKeyException_What(wchar_t* param_1);
	// !DECL 0x00437c40 END
	// !DECL 0x004380c0 BEGIN
	/* 4380C0 */ uint* CDSRegKeyException_AllocDefault();
	// !DECL 0x004380c0 END
	// !DECL 0x00438140 BEGIN
	/* 438140 */ void* CDSRegKeyException_DtorScalar(uchar param_1);
	// !DECL 0x00438140 END
};

#endif
