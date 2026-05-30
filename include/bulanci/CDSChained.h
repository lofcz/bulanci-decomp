#ifndef _C_D_S_CHAINED
#define _C_D_S_CHAINED

#include <globals.h>

class CDSChained {
public:
	// !DECL 0x004032c0 BEGIN
	/* 4032C0 */ uchar* CDSChained_GetTypeInfo();
	// !DECL 0x004032c0 END
	// !DECL 0x004032d0 BEGIN
	/* 4032D0 */ uint* CDSChained_ctor(uint* param_1);
	// !DECL 0x004032d0 END
	// !DECL 0x0040b560 BEGIN
	/* 40B560 */ void* FUN_0040b560(uint* param_1);
	// !DECL 0x0040b560 END
	// !DECL 0x0042beb0 BEGIN
	/* 42BEB0 */ uchar CDSChained_ResetChainCounters(int param_1);
	// !DECL 0x0042beb0 END
	// !DECL 0x0042f8a0 BEGIN
	/* 42F8A0 */ uchar CDSChained_InitIDSReferencedVtable(uint* param_1);
	// !DECL 0x0042f8a0 END
	// !DECL 0x00433dc0 BEGIN
	/* 433DC0 */ uchar CDSChained_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x00433dc0 END
	// !DECL 0x00433dd0 BEGIN
	/* 433DD0 */ void* CDSChained_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00433dd0 END
};

#endif
