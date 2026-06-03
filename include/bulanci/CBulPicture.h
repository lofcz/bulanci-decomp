#ifndef _C_BUL_PICTURE
#define _C_BUL_PICTURE

#include <globals.h>

class CBulPicture {
public:
	// !DECL 0x0040aa70 BEGIN
	/* 40AA70 */ uchar CBulPicture_ApplyTeamPalette(uchar param_1);
	// !DECL 0x0040aa70 END
	// !DECL 0x0040aaa0 BEGIN
	/* 40AAA0 */ uchar CBulPicture_OnRecolorEvent(short param_1, uint param_2);
	// !DECL 0x0040aaa0 END
	// !DECL 0x0040b050 BEGIN
	/* 40B050 */ uchar CBulPicture_DrawSurface(void* param_1);
	// !DECL 0x0040b050 END
	// !DECL 0x0040b8b0 BEGIN
	/* 40B8B0 */ uchar* CBulPicture_GetClassTable();
	// !DECL 0x0040b8b0 END
	// !DECL 0x0040b8c0 BEGIN
	/* 40B8C0 */ uchar CBulPicture_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x0040b8c0 END
	// !DECL 0x0040b8d0 BEGIN
	/* 40B8D0 */ uchar CBulPicture_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x0040b8d0 END
	// !DECL 0x0040b8e0 BEGIN
	/* 40B8E0 */ uchar CBulPicture_ScalarDeletingDtor_thunk_Sub10(uchar param_1);
	// !DECL 0x0040b8e0 END
	// !DECL 0x0040b8f0 BEGIN
	/* 40B8F0 */ void CBulPicture_dtor(uchar param_1);
	// !DECL 0x0040b8f0 END
	// !DECL 0x0040eb30 BEGIN
	/* 40EB30 */ void* CBulPicture_Create();
	// !DECL 0x0040eb30 END
	// !DECL 0x0040ebd0 BEGIN
	/* 40EBD0 */ void* CBulPicture_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0040ebd0 END

	// !DECL 0x004101d0 BEGIN
	/* 4101D0 */ void* CBulPicture_ctor(int param_1, int param_2, uint param_3, uint param_4);
	// !DECL 0x004101d0 END
};

#endif
