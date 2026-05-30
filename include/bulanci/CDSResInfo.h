#ifndef _C_D_S_RES_INFO
#define _C_D_S_RES_INFO

#include <globals.h>

class CDSResInfo {
public:
	// !DECL 0x004339f0 BEGIN
	/* 4339F0 */ uchar* CDSResInfo_GetTypeInfo();
	// !DECL 0x004339f0 END
	// !DECL 0x00433a00 BEGIN
	/* 433A00 */ uchar CDSResInfo_ScalarDeletingDtorThunk(uchar param_1);
	// !DECL 0x00433a00 END
	// !DECL 0x00433a10 BEGIN
	/* 433A10 */ uchar CDSResInfo_dtor(uint* param_1);
	// !DECL 0x00433a10 END
	// !DECL 0x00433a70 BEGIN
	/* 433A70 */ void* CDSResInfo_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00433a70 END
	// !DECL 0x00434270 BEGIN
	/* 434270 */ uchar CDSResInfo_ReleaseEmbeddedResource(int param_1);
	// !DECL 0x00434270 END
	// !DECL 0x00434290 BEGIN
	/* 434290 */ uchar CDSResInfo_Load(int* param_1);
	// !DECL 0x00434290 END
	// !DECL 0x004342c0 BEGIN
	/* 4342C0 */ uchar CDSResInfo_Save(int* param_1);
	// !DECL 0x004342c0 END
};

#endif
