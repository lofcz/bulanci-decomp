#ifndef _C_D_S_EXCEPTION
#define _C_D_S_EXCEPTION

#include <globals.h>

class CDSException {
public:
	// !DECL 0x0042fff0 BEGIN
	/* 42FFF0 */ uchar CDSException_ReleaseViaFlag(int* param_1);
	// !DECL 0x0042fff0 END
	// !DECL 0x00434a10 BEGIN
	/* 434A10 */ uchar CDSException_InitBaseFields(uint* param_1);
	// !DECL 0x00434a10 END
	// !DECL 0x00434a30 BEGIN
	/* 434A30 */ uchar* CDSException_GetTypeInfo();
	// !DECL 0x00434a30 END
	// !DECL 0x00434a40 BEGIN
	/* 434A40 */ void CDSException_InitFields(uint param_1, uint param_2, uchar param_3);
	// !DECL 0x00434a40 END
	// !DECL 0x00434ae0 BEGIN
	/* 434AE0 */ void* CDSException_DtorScalar(uchar param_1);
	// !DECL 0x00434ae0 END
	// !DECL 0x00434b80 BEGIN
	/* 434B80 */ wchar_t* CDSException_GetMessageW();
	// !DECL 0x00434b80 END
	// !DECL 0x00448a6d BEGIN
	/* 448A6D */ uchar _purecall();
	// !DECL 0x00448a6d END
	// !DECL 0x0044d39d BEGIN
	/* 44D39D */ static uint _set_abort_behavior(uint param_1, uint param_2);
	// !DECL 0x0044d39d END
};

#endif
