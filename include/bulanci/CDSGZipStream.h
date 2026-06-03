#ifndef _C_D_S_G_ZIP_STREAM
#define _C_D_S_G_ZIP_STREAM

#include <globals.h>

class CDSGZipStream {
public:
	// !DECL 0x00409030 BEGIN
	/* 409030 */ uchar CloseStream(int param_1);
	// !DECL 0x00409030 END
	// !DECL 0x004098a0 BEGIN
	/* 4098A0 */ uchar CDSGZipStream_dtor(uint* param_1);
	// !DECL 0x004098a0 END
	// !DECL 0x00409960 BEGIN
	/* 409960 */ uchar* CDSGZipStream_GetTypeInfo();
	// !DECL 0x00409960 END
	// !DECL 0x00409980 BEGIN
	/* 409980 */ uchar CDSGZipStream_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x00409980 END
	// !DECL 0x00409990 BEGIN
	/* 409990 */ uchar CDSGZipStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00409990 END
	// !DECL 0x004099a0 BEGIN
	/* 4099A0 */ uchar CDSGZipStream_ScalarDeletingDtor_thunk_Sub14(uchar param_1);
	// !DECL 0x004099a0 END
	// !DECL 0x004099b0 BEGIN
	/* 4099B0 */ uint GetStreamName(uint param_1);
	// !DECL 0x004099b0 END
	// !DECL 0x004099d0 BEGIN
	/* 4099D0 */ void* CDSGZipStream_ScalarDeletingDtor_004099d0(uchar param_1);
	// !DECL 0x004099d0 END
	// !DECL 0x00434f90 BEGIN
	/* 434F90 */ uchar IDSStream_UnlockRegion_ThrowUnsupported(CDSFilterStream* param_1);
	// !DECL 0x00434f90 END
	// !DECL 0x00435220 BEGIN
	/* 435220 */ uchar ReadBytes(void* param_1, uint param_2);
	// !DECL 0x00435220 END
	// !DECL 0x004352e0 BEGIN
	/* 4352E0 */ uchar WriteBytes(void* param_1, uint param_2);
	// !DECL 0x004352e0 END
	// !DECL 0x00435360 BEGIN
	/* 435360 */ int GetSize(uint param_1);
	// !DECL 0x00435360 END
	// !DECL 0x00435390 BEGIN
	/* 435390 */ int TellPosition(uint param_1);
	// !DECL 0x00435390 END
	// !DECL 0x004353c0 BEGIN
	/* 4353C0 */ uchar SeekPosition(uchar* param_1, uint param_2, int param_3);
	// !DECL 0x004353c0 END
	// !DECL 0x00435a20 BEGIN
	/* 435A20 */ uchar CDSGZipStream_InitFromOpenInfo(int param_1);
	// !DECL 0x00435a20 END
	// !DECL 0x00435ae0 BEGIN
	/* 435AE0 */ void* CDSGZipStream_ctor(int param_1);
	// !DECL 0x00435ae0 END
	// !DECL 0x00435b6d BEGIN
	/* 435B6D */ uchar Catch_00435b6d();
	// !DECL 0x00435b6d END
	// !DECL 0x00435ba0 BEGIN
	/* 435BA0 */ void* CDSGZipStream_ChainedNewInstance(int param_1);
	// !DECL 0x00435ba0 END
	// !DECL 0x0043bff0 BEGIN
	/* 43BFF0 */ uchar IDSStream_LockRegion_ThrowUnsupported(CDSFilterStream* param_1);
	// !DECL 0x0043bff0 END
	// !DECL 0x00446c10 BEGIN
	/* 446C10 */ uchar FlushStream(CDSFilterStream* param_1);
	// !DECL 0x00446c10 END
	// !DECL 0x00446c20 BEGIN
	/* 446C20 */ uchar SetStreamSize(CDSFilterStream* param_1);
	// !DECL 0x00446c20 END
};

#endif
