#ifndef _C_D_S_SAFE_STREAM_INFO
#define _C_D_S_SAFE_STREAM_INFO

#include <globals.h>

class CDSSafeStreamInfo {
public:
	// !DECL 0x00446cb0 BEGIN
	/* 446CB0 */ uchar* CDSSafeStreamInfo_GetTypeInfo();
	// !DECL 0x00446cb0 END
	// !DECL 0x00446cc0 BEGIN
	/* 446CC0 */ uchar CDSSafeStreamInfo_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x00446cc0 END
	// !DECL 0x00446cd0 BEGIN
	/* 446CD0 */ uchar CDSSafeStreamInfo_dtor(uint* param_1);
	// !DECL 0x00446cd0 END
	// !DECL 0x00446ea0 BEGIN
	/* 446EA0 */ void CDSSafeStream_RegisterThreadSlice(int* param_1);
	// !DECL 0x00446ea0 END
	// !DECL 0x00447080 BEGIN
	/* 447080 */ void* CDSSafeStreamInfo_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00447080 END
};

#endif
