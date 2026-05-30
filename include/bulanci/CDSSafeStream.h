#ifndef _C_D_S_SAFE_STREAM
#define _C_D_S_SAFE_STREAM

#include <globals.h>

class CDSSafeStream {
public:
	// !DECL 0x00433ab0 BEGIN
	/* 433AB0 */ void* CDSSafeStream_ctor(int* param_1);
	// !DECL 0x00433ab0 END
	// !DECL 0x00433b60 BEGIN
	/* 433B60 */ uchar* CDSSafeStream_GetTypeInfo();
	// !DECL 0x00433b60 END
	// !DECL 0x00433b70 BEGIN
	/* 433B70 */ uchar CDSSafeStream_Close(int param_1);
	// !DECL 0x00433b70 END
	// !DECL 0x00433b90 BEGIN
	/* 433B90 */ uchar CDSSafeStream_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00433b90 END
	// !DECL 0x00433ba0 BEGIN
	/* 433BA0 */ uchar CDSSafeStream_ScalarDeletingDtor_thunk_Sub0c(uchar param_1);
	// !DECL 0x00433ba0 END
	// !DECL 0x00433bb0 BEGIN
	/* 433BB0 */ uchar CDSSafeStream_ScalarDeletingDtor_thunk_Sub14(uchar param_1);
	// !DECL 0x00433bb0 END
	// !DECL 0x00433bc0 BEGIN
	/* 433BC0 */ void CDSSafeStream_dtor(void* param_1);
	// !DECL 0x00433bc0 END
	// !DECL 0x00433c90 BEGIN
	/* 433C90 */ void* CDSSafeStream_ScalarDeletingDtor_00433c90(uchar param_1);
	// !DECL 0x00433c90 END
	// !DECL 0x00433d70 BEGIN
	/* 433D70 */ uchar CDSSafeStream_ReleaseViaChained(int param_1);
	// !DECL 0x00433d70 END
	// !DECL 0x00446c00 BEGIN
	/* 446C00 */ uchar CDSSafeStream_Write(CDSFilterStream* param_1);
	// !DECL 0x00446c00 END
	// !DECL 0x00446ca0 BEGIN
	/* 446CA0 */ int CDSSafeStream_AddRef(int param_1);
	// !DECL 0x00446ca0 END
	// !DECL 0x00446d30 BEGIN
	/* 446D30 */ uint* CDSSafeStream_InitBase(void* param_1);
	// !DECL 0x00446d30 END
	// !DECL 0x00446d90 BEGIN
	/* 446D90 */ int FUN_00446d90(int param_1);
	// !DECL 0x00446d90 END
	// !DECL 0x00446f90 BEGIN
	/* 446F90 */ uchar CDSSafeStream_Read(int param_1);
	// !DECL 0x00446f90 END
	// !DECL 0x00446fb0 BEGIN
	/* 446FB0 */ uchar CDSSafeStream_Seek(uint param_1, uint param_2, uint param_3);
	// !DECL 0x00446fb0 END
	// !DECL 0x00446fe0 BEGIN
	/* 446FE0 */ uchar CDSSafeStream_Tell(int param_1);
	// !DECL 0x00446fe0 END
	// !DECL 0x00447000 BEGIN
	/* 447000 */ uchar CDSSafeStream_GetSize(int param_1);
	// !DECL 0x00447000 END
	// !DECL 0x00447020 BEGIN
	/* 447020 */ uint* CDSSafeStream_GetName(uint* param_1);
	// !DECL 0x00447020 END
};

#endif
