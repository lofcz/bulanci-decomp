#ifndef _C_D_S_EASY_MEM_STREAM
#define _C_D_S_EASY_MEM_STREAM

#include <globals.h>

class CDSEasyMemStream {
public:
	// !DECL 0x00409170 BEGIN
	/* 409170 */ void* CDSEasyMemStream_ctor(uchar* param_1, uint param_2);
	// !DECL 0x00409170 END
	// !DECL 0x004091f0 BEGIN
	/* 4091F0 */ uchar* CDSEasyMemStream_GetTypeInfo();
	// !DECL 0x004091f0 END
	// !DECL 0x00409240 BEGIN
	/* 409240 */ uchar CDSEasyMemStream_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x00409240 END
	// !DECL 0x00409250 BEGIN
	/* 409250 */ uchar CDSEasyMemStream_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x00409250 END
	// !DECL 0x00409270 BEGIN
	/* 409270 */ uchar CDSEasyMemStream_dtor(uint* param_1);
	// !DECL 0x00409270 END
	// !DECL 0x004092f0 BEGIN
	/* 4092F0 */ void* CDSEasyMemStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004092f0 END
	// !DECL 0x004306c0 BEGIN
	/* 4306C0 */ uchar ReleaseBackingBuffer(int param_1);
	// !DECL 0x004306c0 END
	// !DECL 0x004306e0 BEGIN
	/* 4306E0 */ void CDSEasyMemStream_EnsureCapacity(int param_1);
	// !DECL 0x004306e0 END
	// !DECL 0x00430a40 BEGIN
	/* 430A40 */ uchar GetStreamName(uchar param_1);
	// !DECL 0x00430a40 END
	// !DECL 0x00430d60 BEGIN
	/* 430D60 */ void CDSEasyMemStream_InitBackingBuffer(uchar* param_1, uint param_2);
	// !DECL 0x00430d60 END
	// !DECL 0x00430db0 BEGIN
	/* 430DB0 */ uchar LockRegion(CDSFilterStream* param_1);
	// !DECL 0x00430db0 END
	// !DECL 0x00430dc0 BEGIN
	/* 430DC0 */ uchar UnlockRegion(CDSFilterStream* param_1);
	// !DECL 0x00430dc0 END
};

#endif
