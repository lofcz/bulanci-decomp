#ifndef _C_D_S_FILE_STREAM
#define _C_D_S_FILE_STREAM

#include <globals.h>

class CDSFileStream {
public:
	// !DECL 0x00401540 BEGIN
	/* 401540 */ uchar CDSFileStream_Ctor(int param_1, uchar* param_2);
	// !DECL 0x00401540 END
	// !DECL 0x00401600 BEGIN
	/* 401600 */ uchar* CDSFileStream_GetClassTable();
	// !DECL 0x00401600 END
	// !DECL 0x00401610 BEGIN
	/* 401610 */ uchar* CDSFileStream_GetClassRegistry();
	// !DECL 0x00401610 END
	// !DECL 0x00401620 BEGIN
	/* 401620 */ uchar* IDSStream_GetClassRegistry();
	// !DECL 0x00401620 END
	// !DECL 0x00401630 BEGIN
	/* 401630 */ uchar* CDSFileStream_GetTypeInfo();
	// !DECL 0x00401630 END
	// !DECL 0x00401640 BEGIN
	/* 401640 */ void CloseStream(int param_1);
	// !DECL 0x00401640 END
	// !DECL 0x00401660 BEGIN
	/* 401660 */ uchar IDSStream_ReleaseRefcount(int param_1);
	// !DECL 0x00401660 END
	// !DECL 0x00401690 BEGIN
	/* 401690 */ uchar CDSFileStream_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x00401690 END
	// !DECL 0x004016a0 BEGIN
	/* 4016A0 */ uchar CDSFileStream_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x004016a0 END
	// !DECL 0x004016b0 BEGIN
	/* 4016B0 */ uchar ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004016b0 END
	// !DECL 0x004016c0 BEGIN
	/* 4016C0 */ uchar CDSFileStream_dtor(uint* param_1);
	// !DECL 0x004016c0 END
	// !DECL 0x00401740 BEGIN
	/* 401740 */ uint* GetStreamName(uint* param_1);
	// !DECL 0x00401740 END
	// !DECL 0x00401770 BEGIN
	/* 401770 */ uchar CDSFileStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00401770 END
	// !DECL 0x00409260 BEGIN
	/* 409260 */ uchar IDSStream_ReleaseRefcount_thunk_Sub4(int param_1);
	// !DECL 0x00409260 END
	// !DECL 0x00409450 BEGIN
	/* 409450 */ uchar IDSStream_AddRef(int param_1);
	// !DECL 0x00409450 END
	// !DECL 0x00409490 BEGIN
	/* 409490 */ uchar IDSStream_Release(int param_1);
	// !DECL 0x00409490 END
	// !DECL 0x00409970 BEGIN
	/* 409970 */ uchar CDSFileStream_ReleaseViaChained(int param_1);
	// !DECL 0x00409970 END
	// !DECL 0x00429300 BEGIN
	/* 429300 */ uchar CDSFileStream_AdjustThisOffset(int param_1);
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
	/* 433750 */ uchar CDSFileStream_Open(int param_1, DWORD param_2);
	// !DECL 0x00433750 END
	// !DECL 0x004338d0 BEGIN
	/* 4338D0 */ uchar CDSFileStream_CreateInstance(int param_1);
	// !DECL 0x004338d0 END
};

#endif
