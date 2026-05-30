#ifndef _C_D_S_DIRECT_X_EXCEPTION
#define _C_D_S_DIRECT_X_EXCEPTION

#include <globals.h>

class CDSDirectXException {
public:
	// !DECL 0x0043b6f0 BEGIN
	/* 43B6F0 */ static uint CDSDirectXException_LookupStringIdForHresult(int param_1);
	// !DECL 0x0043b6f0 END
	// !DECL 0x0043b720 BEGIN
	/* 43B720 */ static int FUN_0043b720(int param_1, int param_2);
	// !DECL 0x0043b720 END
	// !DECL 0x0043b750 BEGIN
	/* 43B750 */ uchar* CDSDirectXException_GetClassTable();
	// !DECL 0x0043b750 END
	// !DECL 0x0043b760 BEGIN
	/* 43B760 */ uchar CDSDirectXException_dtor(uint* param_1);
	// !DECL 0x0043b760 END
	// !DECL 0x0043b7c0 BEGIN
	/* 43B7C0 */ uchar* CDSDirectXException_What(wchar_t* param_1);
	// !DECL 0x0043b7c0 END
	// !DECL 0x0043b820 BEGIN
	/* 43B820 */ uchar CDSDirectXException_ThrowFromHresult(uint param_1, uint param_2);
	// !DECL 0x0043b820 END
	// !DECL 0x0043b8b0 BEGIN
	/* 43B8B0 */ uint* FUN_0043b8b0();
	// !DECL 0x0043b8b0 END
	// !DECL 0x0043b930 BEGIN
	/* 43B930 */ void* CDSDirectXException_DtorScalar(uchar param_1);
	// !DECL 0x0043b930 END
};

#endif
