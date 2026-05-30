#ifndef _C_D_S_DIRECT_PLAY
#define _C_D_S_DIRECT_PLAY

#include <globals.h>

class CDSDirectPlay {
public:
	// !DECL 0x0043b270 BEGIN
	/* 43B270 */ void* CDSDirectPlay_ctor(void* param_1);
	// !DECL 0x0043b270 END
	// !DECL 0x0043b2d0 BEGIN
	/* 43B2D0 */ uchar* CDSDirectPlay_GetClassTable();
	// !DECL 0x0043b2d0 END
	// !DECL 0x0043b2e0 BEGIN
	/* 43B2E0 */ void CDSDirectPlay_dtor_body(void* param_1);
	// !DECL 0x0043b2e0 END
	// !DECL 0x0043b600 BEGIN
	/* 43B600 */ void* CDSDirectPlay_scalar_deleting_dtor(uchar param_1);
	// !DECL 0x0043b600 END
};

#endif
