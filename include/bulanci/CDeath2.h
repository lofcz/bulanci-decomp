#ifndef _C_DEATH2
#define _C_DEATH2

#include <globals.h>

class CDeath2 {
public:
	// !DECL 0x00417af0 BEGIN
	/* 417AF0 */ uchar CDeath2_UpdateStateFromParams(uint param_1, uint param_2, int param_3);
	// !DECL 0x00417af0 END
	// !DECL 0x00417b10 BEGIN
	/* 417B10 */ void CDeath2_HideViewIfParentShown(CDSUpdatedItem* param_1);
	// !DECL 0x00417b10 END
	// !DECL 0x00419b00 BEGIN
	/* 419B00 */ uchar* CDeath2_GetClassTable();
	// !DECL 0x00419b00 END
	// !DECL 0x0041a8c0 BEGIN
	/* 41A8C0 */ void* CDeath2_SubobjectCtor();
	// !DECL 0x0041a8c0 END

	// !DECL 0x00419aa0 BEGIN
	/* 419AA0 */ void* CDeath2_ctor(CBulanek* param_1);
	// !DECL 0x00419aa0 END
};

#endif
