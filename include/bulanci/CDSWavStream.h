#ifndef _C_D_S_WAV_STREAM
#define _C_D_S_WAV_STREAM

#include <globals.h>

class CDSWavStream {
public:
	// !DECL 0x0041a5c0 BEGIN
	/* 41A5C0 */ uchar* CDSWavStream_GetTypeInfo();
	// !DECL 0x0041a5c0 END
	// !DECL 0x0041a5d0 BEGIN
	/* 41A5D0 */ uchar CDSWavStream_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x0041a5d0 END
	// !DECL 0x0041a5e0 BEGIN
	/* 41A5E0 */ uchar CDSWav_ReleaseChild_thunk_Sub30(int param_1);
	// !DECL 0x0041a5e0 END
	// !DECL 0x0041a5f0 BEGIN
	/* 41A5F0 */ uchar CDSWav_ReleaseChild_thunk_Sub34(int param_1);
	// !DECL 0x0041a5f0 END
	// !DECL 0x0041a600 BEGIN
	/* 41A600 */ uchar CDSWavStream_ScalarDeletingDtor_thunk_Sub30(uchar param_1);
	// !DECL 0x0041a600 END
	// !DECL 0x0041a610 BEGIN
	/* 41A610 */ uchar CDSChain_AdjustThisOffset_ThisMinus30(int param_1);
	// !DECL 0x0041a610 END
	// !DECL 0x0041a620 BEGIN
	/* 41A620 */ uchar CDSWavStream_ScalarDeletingDtor_thunk_Sub34(uchar param_1);
	// !DECL 0x0041a620 END
	// !DECL 0x0041a630 BEGIN
	/* 41A630 */ uchar CDSWavStream_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x0041a630 END
	// !DECL 0x0041a640 BEGIN
	/* 41A640 */ void CDSWavStream_dtor();
	// !DECL 0x0041a640 END
	// !DECL 0x0041bc00 BEGIN
	/* 41BC00 */ void* CDSWavStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0041bc00 END
	// !DECL 0x00428dd0 BEGIN
	/* 428DD0 */ uchar CDSChain_AdjustThisOffset_ThisMinus2c(int param_1);
	// !DECL 0x00428dd0 END
	// !DECL 0x0043bab0 BEGIN
	/* 43BAB0 */ uchar CDSWavStream_SaveToStream(int* param_1);
	// !DECL 0x0043bab0 END
	// !DECL 0x0043bb50 BEGIN
	/* 43BB50 */ uchar CDSWavStream_AttachStreamStorage(int* param_1);
	// !DECL 0x0043bb50 END
};

#endif
