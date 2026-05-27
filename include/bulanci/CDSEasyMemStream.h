#ifndef _C_D_S_EASY_MEM_STREAM
#define _C_D_S_EASY_MEM_STREAM

#include <globals.h>

class CDSEasyMemStream {
public:
	// !DECL 0x00409170 BEGIN
	/* 409170 */ void* FUN_00409170(uchar* param_1, uint param_2);
	// !DECL 0x00409170 END
	// !DECL 0x004091f0 BEGIN
	/* 4091F0 */ uchar* FUN_004091f0();
	// !DECL 0x004091f0 END
	// !DECL 0x00409200 BEGIN
	/* 409200 */ uchar CloseStream(int param_1);
	// !DECL 0x00409200 END
	// !DECL 0x00409220 BEGIN
	/* 409220 */ longlong GetSize();
	// !DECL 0x00409220 END
	// !DECL 0x00409230 BEGIN
	/* 409230 */ longlong TellPosition();
	// !DECL 0x00409230 END
	// !DECL 0x00409240 BEGIN
	/* 409240 */ uchar FUN_00409240(uchar param_1);
	// !DECL 0x00409240 END
	// !DECL 0x00409250 BEGIN
	/* 409250 */ uchar FUN_00409250(uchar param_1);
	// !DECL 0x00409250 END
	// !DECL 0x00409270 BEGIN
	/* 409270 */ uchar FUN_00409270(uint* param_1);
	// !DECL 0x00409270 END
	// !DECL 0x004092f0 BEGIN
	/* 4092F0 */ void* FUN_004092f0(uchar param_1);
	// !DECL 0x004092f0 END
	// !DECL 0x004306c0 BEGIN
	/* 4306C0 */ uchar ReleaseBackingBuffer(int param_1);
	// !DECL 0x004306c0 END
	// !DECL 0x004306e0 BEGIN
	/* 4306E0 */ uchar FUN_004306e0(int param_1);
	// !DECL 0x004306e0 END
	// !DECL 0x004307f0 BEGIN
	/* 4307F0 */ uchar ReadBytes(void* param_1, size_t param_2);
	// !DECL 0x004307f0 END
	// !DECL 0x004308c0 BEGIN
	/* 4308C0 */ uchar WriteBytes(void* param_1, size_t param_2);
	// !DECL 0x004308c0 END
	// !DECL 0x00430980 BEGIN
	/* 430980 */ uchar SeekPosition(int param_1, uint param_2, int param_3);
	// !DECL 0x00430980 END
	// !DECL 0x004309f0 BEGIN
	/* 4309F0 */ uchar SetStreamSize(uint param_1, uint param_2);
	// !DECL 0x004309f0 END
	// !DECL 0x00430a40 BEGIN
	/* 430A40 */ uchar GetStreamName(uchar param_1);
	// !DECL 0x00430a40 END
	// !DECL 0x00430d60 BEGIN
	/* 430D60 */ uchar FUN_00430d60(uchar* param_1, uint param_2);
	// !DECL 0x00430d60 END
	// !DECL 0x00430db0 BEGIN
	/* 430DB0 */ uchar LockRegion(CDSFilterStream* param_1);
	// !DECL 0x00430db0 END
	// !DECL 0x00430dc0 BEGIN
	/* 430DC0 */ uchar UnlockRegion(CDSFilterStream* param_1);
	// !DECL 0x00430dc0 END
};

#endif
