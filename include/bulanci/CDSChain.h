#ifndef _C_D_S_CHAIN
#define _C_D_S_CHAIN

#include <globals.h>

class CDSChain {
public:
	// !DECL 0x004011f0 BEGIN
	/* 4011F0 */ uchar* CDSChain_GetClassTable();
	// !DECL 0x004011f0 END
	// !DECL 0x00401200 BEGIN
	/* 401200 */ uchar* CDSChain_Referenced_GetClassTable();
	// !DECL 0x00401200 END
	// !DECL 0x00401210 BEGIN
	/* 401210 */ void* CDSChain_Referenced_DtorScalar(uchar param_1);
	// !DECL 0x00401210 END
	// !DECL 0x00401260 BEGIN
	/* 401260 */ uchar CDSChain_OnChainEvent(uchar param_1);
	// !DECL 0x00401260 END
	// !DECL 0x0040a440 BEGIN
	/* 40A440 */ void CDSChain_LoadConfigFromRegistry();
	// !DECL 0x0040a440 END
	// !DECL 0x0040a62f BEGIN
	/* 40A62F */ uchar* Catch_0040a62f();
	// !DECL 0x0040a62f END
	// !DECL 0x0040a680 BEGIN
	/* 40A680 */ void* CDSChain_ctor(void* param_1);
	// !DECL 0x0040a680 END
	// !DECL 0x0041aa40 BEGIN
	/* 41AA40 */ uchar CDSChain_ReleaseChild(int param_1);
	// !DECL 0x0041aa40 END
	// !DECL 0x0042ac90 BEGIN
	/* 42AC90 */ int CDSChain_AdjustThisOffset();
	// !DECL 0x0042ac90 END
	// !DECL 0x0042fb70 BEGIN
	/* 42FB70 */ uchar CDSChain_Append(int param_1);
	// !DECL 0x0042fb70 END
	// !DECL 0x0042fc30 BEGIN
	/* 42FC30 */ uchar CDSChain_Remove(int* param_1);
	// !DECL 0x0042fc30 END
	// !DECL 0x0042fcd0 BEGIN
	/* 42FCD0 */ uchar CDSChain_dtor(uint* param_1);
	// !DECL 0x0042fcd0 END
};

#endif
