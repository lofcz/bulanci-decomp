#ifndef _C_D_S_STREAM_STORAGE
#define _C_D_S_STREAM_STORAGE

#include <globals.h>

class CDSStreamStorage {
public:
	// !DECL 0x00401850 BEGIN
	/* 401850 */ uchar* CDSStreamStorage_GetTypeInfo();
	// !DECL 0x00401850 END
	// !DECL 0x00401860 BEGIN
	/* 401860 */ uchar CDSStreamStorage_CloseStream(uint param_1);
	// !DECL 0x00401860 END
	// !DECL 0x00401870 BEGIN
	/* 401870 */ void* CDSStreamStorage_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00401870 END
	// !DECL 0x00401890 BEGIN
	/* 401890 */ uchar CDSStreamStorage_ScalarDeletingDtor_00401890(uchar param_1);
	// !DECL 0x00401890 END
	// !DECL 0x004018a0 BEGIN
	/* 4018A0 */ uchar CDSStreamStorage_ScalarDeletingDtor_thunk_Sub8(uchar param_1);
	// !DECL 0x004018a0 END
	// !DECL 0x004018b0 BEGIN
	/* 4018B0 */ uchar* CDSStreamStorage_GetClassTable();
	// !DECL 0x004018b0 END
	// !DECL 0x004018c0 BEGIN
	/* 4018C0 */ uchar CDSStreamStorage_ReleaseRefcount(int param_1);
	// !DECL 0x004018c0 END
	// !DECL 0x004018f0 BEGIN
	/* 4018F0 */ uchar CDSStreamStorage_DtorBody(int param_1);
	// !DECL 0x004018f0 END
	// !DECL 0x00401900 BEGIN
	/* 401900 */ uchar CDSStreamStorage_ReleaseChild_thunk_Sub8(int param_1);
	// !DECL 0x00401900 END
	// !DECL 0x00431170 BEGIN
	/* 431170 */ int CDSStreamStorage_FindKeyIndex(int param_1, uchar* param_2);
	// !DECL 0x00431170 END
	// !DECL 0x004339c0 BEGIN
	/* 4339C0 */ uint CDSStreamStorage_GetStreamCount();
	// !DECL 0x004339c0 END
	// !DECL 0x004339d0 BEGIN
	/* 4339D0 */ uint CDSStreamStorage_GetStreamEntry(int param_1);
	// !DECL 0x004339d0 END
	// !DECL 0x00433cb0 BEGIN
	/* 433CB0 */ uint CDSStreamStorage_CloseStreamByKey(uint param_1);
	// !DECL 0x00433cb0 END
	// !DECL 0x00433e60 BEGIN
	/* 433E60 */ void CDSStreamStorage_dtor(void* param_1);
	// !DECL 0x00433e60 END
	// !DECL 0x00433f00 BEGIN
	/* 433F00 */ uint* FUN_00433f00(int param_1);
	// !DECL 0x00433f00 END
	// !DECL 0x00433f70 BEGIN
	/* 433F70 */ void* FUN_00433f70(void* param_1, void* param_2);
	// !DECL 0x00433f70 END
	// !DECL 0x00434110 BEGIN
	/* 434110 */ uchar CDSStreamStorage_OpenStream(uint param_1, void* param_2);
	// !DECL 0x00434110 END
	// !DECL 0x00434140 BEGIN
	/* 434140 */ uchar CDSStreamStorage_GetStreamByIndex(int param_1, void* param_2);
	// !DECL 0x00434140 END
	// !DECL 0x00434380 BEGIN
	/* 434380 */ uint CDSStreamStorage_AddRefHeldObject(int param_1);
	// !DECL 0x00434380 END
	// !DECL 0x00434400 BEGIN
	/* 434400 */ void* CDSResourceException_ctor(uint param_1);
	// !DECL 0x00434400 END
	// !DECL 0x00434600 BEGIN
	/* 434600 */ uchar CDSChain_AdjustThisOffset_ThisMinus4(int param_1);
	// !DECL 0x00434600 END
	// !DECL 0x004346f0 BEGIN
	/* 4346F0 */ uchar FUN_004346f0(uint param_1);
	// !DECL 0x004346f0 END
	// !DECL 0x00434760 BEGIN
	/* 434760 */ void* FUN_00434760(int* param_1, uint param_2, int param_3, uint param_4, int param_5, uint param_6);
	// !DECL 0x00434760 END
	// !DECL 0x004348ca BEGIN
	/* 4348CA */ uchar Catch_004348ca();
	// !DECL 0x004348ca END
};

#endif
