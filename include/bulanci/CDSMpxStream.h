#ifndef _C_D_S_MPX_STREAM
#define _C_D_S_MPX_STREAM

#include <globals.h>

class CDSMpxStream {
public:
	// !DECL 0x00432eb0 BEGIN
	/* 432EB0 */ uchar SaveMpxFile(int* param_1);
	// !DECL 0x00432eb0 END
	// !DECL 0x00432fe0 BEGIN
	/* 432FE0 */ uchar* CDSMpxStream_GetTypeInfo();
	// !DECL 0x00432fe0 END
	// !DECL 0x00432ff0 BEGIN
	/* 432FF0 */ uchar CDSMpxStream_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x00432ff0 END
	// !DECL 0x00433000 BEGIN
	/* 433000 */ uchar CDSMpxStream_ScalarDeletingDtor_thunk_Sub38(uchar param_1);
	// !DECL 0x00433000 END
	// !DECL 0x00433020 BEGIN
	/* 433020 */ uchar CDSMpxStream_ScalarDeletingDtor_thunk_Sub3c(uchar param_1);
	// !DECL 0x00433020 END
	// !DECL 0x00433030 BEGIN
	/* 433030 */ uchar CDSMpxStream_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x00433030 END
	// !DECL 0x00433080 BEGIN
	/* 433080 */ uchar CDSWav_ReleaseChild_thunk_Sub38(int param_1);
	// !DECL 0x00433080 END
	// !DECL 0x00433090 BEGIN
	/* 433090 */ uchar CDSWav_ReleaseChild_thunk_Sub3c(int param_1);
	// !DECL 0x00433090 END
	// !DECL 0x004330b0 BEGIN
	/* 4330B0 */ void CDSMpxStream_dtor(void* param_1);
	// !DECL 0x004330b0 END
	// !DECL 0x00433160 BEGIN
	/* 433160 */ void* CDSMpxStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00433160 END
	// !DECL 0x00433180 BEGIN
	/* 433180 */ uchar LoadMpxFile(int* param_1);
	// !DECL 0x00433180 END
};

#endif
