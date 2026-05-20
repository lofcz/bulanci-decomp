#ifndef _C_D_S_FILE_STREAM
#define _C_D_S_FILE_STREAM

#include <globals.h>

class CDSFileStream {
public:
	// !DECL 0x00401540 BEGIN
	/* 401540 */ uint* FUN_00401540(int param_1, uchar* param_2);
	// !DECL 0x00401540 END
	// !DECL 0x00401600 BEGIN
	/* 401600 */ uchar* FUN_00401600();
	// !DECL 0x00401600 END
	// !DECL 0x00401610 BEGIN
	/* 401610 */ uchar* FUN_00401610();
	// !DECL 0x00401610 END
	// !DECL 0x00401620 BEGIN
	/* 401620 */ uchar* FUN_00401620();
	// !DECL 0x00401620 END
	// !DECL 0x00401630 BEGIN
	/* 401630 */ uchar* FUN_00401630();
	// !DECL 0x00401630 END
	// !DECL 0x00401640 BEGIN
	/* 401640 */ void CloseStream(int param_1);
	// !DECL 0x00401640 END
	// !DECL 0x00401660 BEGIN
	/* 401660 */ uchar FUN_00401660(int param_1);
	// !DECL 0x00401660 END
	// !DECL 0x00401690 BEGIN
	/* 401690 */ uchar FUN_00401690(uchar param_1);
	// !DECL 0x00401690 END
	// !DECL 0x004016a0 BEGIN
	/* 4016A0 */ uchar FUN_004016a0(uchar param_1);
	// !DECL 0x004016a0 END
	// !DECL 0x004016b0 BEGIN
	/* 4016B0 */ uchar ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004016b0 END
	// !DECL 0x004016c0 BEGIN
	/* 4016C0 */ uchar FUN_004016c0(uint* param_1);
	// !DECL 0x004016c0 END
	// !DECL 0x00401740 BEGIN
	/* 401740 */ uint* GetStreamName(uint* param_1);
	// !DECL 0x00401740 END
	// !DECL 0x00401770 BEGIN
	/* 401770 */ uint* FUN_00401770(uchar param_1);
	// !DECL 0x00401770 END
	// !DECL 0x00409260 BEGIN
	/* 409260 */ uchar FUN_00409260(int param_1);
	// !DECL 0x00409260 END
	// !DECL 0x00409450 BEGIN
	/* 409450 */ uchar FUN_00409450(int param_1);
	// !DECL 0x00409450 END
	// !DECL 0x00409490 BEGIN
	/* 409490 */ uchar FUN_00409490(int param_1);
	// !DECL 0x00409490 END
	// !DECL 0x00409970 BEGIN
	/* 409970 */ uchar FUN_00409970(int param_1);
	// !DECL 0x00409970 END
	// !DECL 0x00429300 BEGIN
	/* 429300 */ uchar FUN_00429300(int param_1);
	// !DECL 0x00429300 END
	// !DECL 0x004333e0 BEGIN
	/* 4333E0 */ void CloseFileHandle(int param_1);
	// !DECL 0x004333e0 END
	// !DECL 0x00433400 BEGIN
	/* 433400 */ void ReadBytes(void* param_1, DWORD param_2);
	// !DECL 0x00433400 END
	// !DECL 0x00433470 BEGIN
	/* 433470 */ void WriteBytes(void* param_1, DWORD param_2);
	// !DECL 0x00433470 END
	// !DECL 0x004334c0 BEGIN
	/* 4334C0 */ void LockRegion(DWORD param_1, DWORD param_2, DWORD param_3, DWORD param_4);
	// !DECL 0x004334c0 END
	// !DECL 0x00433510 BEGIN
	/* 433510 */ void UnlockRegion(DWORD param_1, DWORD param_2, DWORD param_3, DWORD param_4);
	// !DECL 0x00433510 END
	// !DECL 0x00433560 BEGIN
	/* 433560 */ void SeekPosition(long param_1, long param_2, DWORD param_3);
	// !DECL 0x00433560 END
	// !DECL 0x004335e0 BEGIN
	/* 4335E0 */ void SetStreamSize(uint param_1, uint param_2);
	// !DECL 0x004335e0 END
	// !DECL 0x00433660 BEGIN
	/* 433660 */ longlong GetSize();
	// !DECL 0x00433660 END
	// !DECL 0x004336c0 BEGIN
	/* 4336C0 */ longlong TellPosition();
	// !DECL 0x004336c0 END
	// !DECL 0x00433720 BEGIN
	/* 433720 */ void FlushStream();
	// !DECL 0x00433720 END
	// !DECL 0x00433750 BEGIN
	/* 433750 */ uchar FUN_00433750(int param_1, DWORD param_2);
	// !DECL 0x00433750 END
	// !DECL 0x004338d0 BEGIN
	/* 4338D0 */ uchar FUN_004338d0(int param_1);
	// !DECL 0x004338d0 END
};

#endif
