#ifndef _C_D_S_FILTER_STREAM
#define _C_D_S_FILTER_STREAM

#include <globals.h>

class CDSFilterStream {
public:
	// !DECL 0x0042ff90 BEGIN
	/* 42FF90 */ uchar FlushStream(int param_1);
	// !DECL 0x0042ff90 END
	// !DECL 0x0042ffa0 BEGIN
	/* 42FFA0 */ longlong GetSize(void* param_1);
	// !DECL 0x0042ffa0 END
	// !DECL 0x00430080 BEGIN
	/* 430080 */ uchar* CDSFilterStream_GetTypeInfo();
	// !DECL 0x00430080 END
	// !DECL 0x00430090 BEGIN
	/* 430090 */ int TellPosition(int param_1);
	// !DECL 0x00430090 END
	// !DECL 0x004300a0 BEGIN
	/* 4300A0 */ uchar CDSFilterStream_ScalarDeletingDtor_thunk_Sub14(uchar param_1);
	// !DECL 0x004300a0 END
	// !DECL 0x004300b0 BEGIN
	/* 4300B0 */ uchar CDSFilterStream_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x004300b0 END
	// !DECL 0x004300c0 BEGIN
	/* 4300C0 */ uchar CDSFilterStream_ScalarDeletingDtor_thunk_004300c0(uchar param_1);
	// !DECL 0x004300c0 END
	// !DECL 0x004300d0 BEGIN
	/* 4300D0 */ uint GetStreamName(uint param_1);
	// !DECL 0x004300d0 END
	// !DECL 0x00430400 BEGIN
	/* 430400 */ uchar CDSFilterStream_ReleaseInnerStream(int param_1);
	// !DECL 0x00430400 END
	// !DECL 0x00430420 BEGIN
	/* 430420 */ void ReadBytes(void* param_1, uint param_2);
	// !DECL 0x00430420 END
	// !DECL 0x00430490 BEGIN
	/* 430490 */ void WriteBytes(void* param_1, uint param_2);
	// !DECL 0x00430490 END
	// !DECL 0x00430500 BEGIN
	/* 430500 */ uchar SeekPosition(uint param_1, int param_2, int param_3);
	// !DECL 0x00430500 END
	// !DECL 0x004305c0 BEGIN
	/* 4305C0 */ uchar LockRegion(uint param_1, int param_2, uint param_3, int param_4);
	// !DECL 0x004305c0 END
	// !DECL 0x00430640 BEGIN
	/* 430640 */ uchar UnlockRegion(uint param_1, int param_2, uint param_3, int param_4);
	// !DECL 0x00430640 END
	// !DECL 0x00430b90 BEGIN
	/* 430B90 */ uchar CDSFilterStream_dtor(uint* param_1);
	// !DECL 0x00430b90 END
	// !DECL 0x00430c10 BEGIN
	/* 430C10 */ uchar CloseStream(int param_1);
	// !DECL 0x00430c10 END
	// !DECL 0x00430c30 BEGIN
	/* 430C30 */ void* CDSFilterStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00430c30 END
	// !DECL 0x00430c90 BEGIN
	/* 430C90 */ uchar RaiseUnsupportedOperation(uint param_1);
	// !DECL 0x00430c90 END
	// !DECL 0x00430ca0 BEGIN
	/* 430CA0 */ uchar CDSFilterStream_BindSource(int* param_1, uint param_2, uint param_3, uint param_4, uint param_5);
	// !DECL 0x00430ca0 END
	// !DECL 0x00430d00 BEGIN
	/* 430D00 */ uchar SetStreamSize(uint param_1, uint param_2);
	// !DECL 0x00430d00 END
	// !DECL 0x00430dd0 BEGIN
	/* 430DD0 */ void* CDSFilterStream_Ctor(int* param_1, uint param_2, uint param_3, uint param_4, uint param_5);
	// !DECL 0x00430dd0 END
	// !DECL 0x00430f40 BEGIN
	/* 430F40 */ void* CDSFilterStream_ChainedNewInstance(int param_1);
	// !DECL 0x00430f40 END
};

#endif
