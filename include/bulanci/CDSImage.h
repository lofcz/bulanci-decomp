#ifndef _C_D_S_IMAGE
#define _C_D_S_IMAGE

#include <globals.h>

class CDSImage {
public:
	// !DECL 0x00425580 BEGIN
	/* 425580 */ uchar CDSImage_InitDefaults(uint* param_1);
	// !DECL 0x00425580 END
	// !DECL 0x004255e0 BEGIN
	/* 4255E0 */ uchar* CDSImage_GetTypeInfo();
	// !DECL 0x004255e0 END
	// !DECL 0x004255f0 BEGIN
	/* 4255F0 */ uchar* CDSImage_GetTypeInfo_IDSReferenced();
	// !DECL 0x004255f0 END
	// !DECL 0x00425600 BEGIN
	/* 425600 */ uchar CDSImage_AdjustThisOffset(int param_1);
	// !DECL 0x00425600 END
	// !DECL 0x00425610 BEGIN
	/* 425610 */ uchar CDSFont_ScalarDeletingDtor_thunk_Sub4_CDSImage(uchar param_1);
	// !DECL 0x00425610 END
	// !DECL 0x00431d90 BEGIN
	/* 431D90 */ uchar CDSChain_AdjustThisOffset_ThisMinus48(int param_1);
	// !DECL 0x00431d90 END
	// !DECL 0x00431dc0 BEGIN
	/* 431DC0 */ uchar CDSImage_ReleaseRefcount_thunk_Sub4(int param_1);
	// !DECL 0x00431dc0 END
	// !DECL 0x00431dd0 BEGIN
	/* 431DD0 */ uchar CDSImage_ReleaseRefThunk(int param_1);
	// !DECL 0x00431dd0 END
	// !DECL 0x004322d0 BEGIN
	/* 4322D0 */ uchar CDSChain_AdjustThisOffset_ThisMinus54(int param_1);
	// !DECL 0x004322d0 END
	// !DECL 0x004322f0 BEGIN
	/* 4322F0 */ uchar CDSImage_ReleaseRefcount(int param_1);
	// !DECL 0x004322f0 END
	// !DECL 0x00432320 BEGIN
	/* 432320 */ uchar CDSImage_ReleaseRefcount_thunk_Sub58(int param_1);
	// !DECL 0x00432320 END
	// !DECL 0x00436c60 BEGIN
	/* 436C60 */ void CDSImage_Save(CDSStreamStorage* param_1);
	// !DECL 0x00436c60 END
	// !DECL 0x00437160 BEGIN
	/* 437160 */ void CDSImage_Load(CDSStreamStorage* param_1);
	// !DECL 0x00437160 END
	// !DECL 0x00437520 BEGIN
	/* 437520 */ uchar CDSImage_ScalarDeletingDtorThunk(uchar param_1);
	// !DECL 0x00437520 END
	// !DECL 0x00437530 BEGIN
	/* 437530 */ uchar CDSImage_ScalarDeletingDtor_thunk_Sub58(uchar param_1);
	// !DECL 0x00437530 END
	// !DECL 0x00437540 BEGIN
	/* 437540 */ uchar CDSFont_ScalarDeletingDtor_thunk_Sub4c(uchar param_1);
	// !DECL 0x00437540 END
	// !DECL 0x00437550 BEGIN
	/* 437550 */ uchar CDSImage_ReleaseRefcount_thunk_Sub4c(int param_1);
	// !DECL 0x00437550 END
	// !DECL 0x004375c0 BEGIN
	/* 4375C0 */ void* CDSImage_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004375c0 END

	// !DECL 0x00435fe0 BEGIN
	/* 435FE0 */ int ComputeBufferSize(char param_1);
	// !DECL 0x00435fe0 END
	// !DECL 0x004362f0 BEGIN
	/* 4362F0 */ int ComputeAllocationSize(void* param_1);
	// !DECL 0x004362f0 END

	// !DECL 0x00436d50 BEGIN
	/* 436D50 */ uchar FreeBuffers(int param_1);
	// !DECL 0x00436d50 END
	// !DECL 0x00436f40 BEGIN
	/* 436F40 */ uchar Allocate(int param_1, uint param_2, int param_3, char param_4, int param_5);
	// !DECL 0x00436f40 END
};

#endif
