#ifndef _C_D_S_SIMPLE_EXCEPTION
#define _C_D_S_SIMPLE_EXCEPTION

#include <globals.h>

class CDSSimpleException {
public:
	// !DECL 0x00434ac0 BEGIN
	/* 434AC0 */ uchar* CDSSimpleException_GetClassTable();
	// !DECL 0x00434ac0 END
	// !DECL 0x00434ad0 BEGIN
	/* 434AD0 */ uint CDSSimpleException_What(uint param_1);
	// !DECL 0x00434ad0 END
	// !DECL 0x00434c20 BEGIN
	/* 434C20 */ uchar CDSSimpleException_Throw(uint param_1, uint param_2);
	// !DECL 0x00434c20 END
};

#endif
