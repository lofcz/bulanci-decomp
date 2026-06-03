#ifndef _C_D_S_WAV
#define _C_D_S_WAV

#include <globals.h>

class CDSWav {
public:
	// !DECL 0x0041a510 BEGIN
	/* 41A510 */ uchar* CDSWav_GetClassMeta();
	// !DECL 0x0041a510 END
	// !DECL 0x0041a520 BEGIN
	/* 41A520 */ uchar* CDSWav_GetTypeInfo();
	// !DECL 0x0041a520 END
	// !DECL 0x0041a530 BEGIN
	/* 41A530 */ uchar CDSWav_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x0041a530 END
	// !DECL 0x0041a540 BEGIN
	/* 41A540 */ uchar CDSWav_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x0041a540 END
	// !DECL 0x0041bbe0 BEGIN
	/* 41BBE0 */ void* CDSWav_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0041bbe0 END
	// !DECL 0x00433040 BEGIN
	/* 433040 */ void CDSWav_ReleaseRefcount();
	// !DECL 0x00433040 END
	// !DECL 0x00433070 BEGIN
	/* 433070 */ uchar CDSMpxStream_ReleaseRefcount_thunk_Sub18(int param_1);
	// !DECL 0x00433070 END
	// !DECL 0x004330a0 BEGIN
	/* 4330A0 */ uchar CDSMpxStream_ReleaseRefcount_thunk_Sub4(int param_1);
	// !DECL 0x004330a0 END
	// !DECL 0x0043b950 BEGIN
	/* 43B950 */ uint CDSWav_HandleAcquireReadThunk();
	// !DECL 0x0043b950 END
};

#endif
