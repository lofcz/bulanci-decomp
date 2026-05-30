#ifndef _C_DIRECT_KEYB
#define _C_DIRECT_KEYB

#include <globals.h>

class CDirectKeyb {
public:
	// !DECL 0x00412302 BEGIN
	/* 412302 */ uchar Catch_00412302();
	// !DECL 0x00412302 END
	// !DECL 0x00412340 BEGIN
	/* 412340 */ uchar* CDirectKeyb_GetClassTable();
	// !DECL 0x00412340 END
	// !DECL 0x00412350 BEGIN
	/* 412350 */ uchar CDirectKeyb_dtor(uint* param_1);
	// !DECL 0x00412350 END
	// !DECL 0x00412440 BEGIN
	/* 412440 */ void* CDirectKeyb_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00412440 END
};

#endif
