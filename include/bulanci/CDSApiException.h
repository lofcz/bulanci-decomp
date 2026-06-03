#ifndef _C_D_S_API_EXCEPTION
#define _C_D_S_API_EXCEPTION

#include <globals.h>

class CDSApiException {
public:
	// !DECL 0x00434b00 BEGIN
	/* 434B00 */ uchar* CDSApiException_GetClassTable();
	// !DECL 0x00434b00 END
	// !DECL 0x00434b20 BEGIN
	/* 434B20 */ uchar CDSApiException_dtor(uint* param_1);
	// !DECL 0x00434b20 END
	// !DECL 0x00434c70 BEGIN
	/* 434C70 */ wchar_t* CDSApiException_What(wchar_t* param_1);
	// !DECL 0x00434c70 END
	// !DECL 0x00434d00 BEGIN
	/* 434D00 */ void CDSApiException_ThrowFromGetLastError();
	// !DECL 0x00434d00 END
	// !DECL 0x00434d50 BEGIN
	/* 434D50 */ uchar CDSApiException_ThrowFromWin32(uint param_1);
	// !DECL 0x00434d50 END
	// !DECL 0x00434e10 BEGIN
	/* 434E10 */ void* CDSApiException_DtorScalar(uchar param_1);
	// !DECL 0x00434e10 END
};

#endif
