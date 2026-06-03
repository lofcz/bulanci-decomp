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
	// !DECL 0x0040a680 BEGIN
	/* 40A680 */ CDSChain_full* CDSChain_ctor(CDSChain_full* param_1);
	// !DECL 0x0040a680 END
	// !DECL 0x0041aa40 BEGIN
	/* 41AA40 */ uchar CDSChain_ReleaseChild(int param_1);
	// !DECL 0x0041aa40 END
	// !DECL 0x0042ac90 BEGIN
	/* 42AC90 */ int CDSChain_AdjustThisOffset();
	// !DECL 0x0042ac90 END
	// !DECL 0x0042fc30 BEGIN
	/* 42FC30 */ uchar CDSChain_Remove(int* param_1);
	// !DECL 0x0042fc30 END
	// !DECL 0x0042fcd0 BEGIN
	/* 42FCD0 */ void CDSChain_dtor(void* param_1);
	// !DECL 0x0042fcd0 END

	// !DECL 0x0042f800 BEGIN
	/* 42F800 */ void CDSChain_ReleaseAuxHeap(void* param_1);
	// !DECL 0x0042f800 END
	// !DECL 0x0042f880 BEGIN
	/* 42F880 */ uchar CDSChained_InsertBeforeAnchor(void* param_1, int param_2);
	// !DECL 0x0042f880 END
	// !DECL 0x0042f890 BEGIN
	/* 42F890 */ uchar CDSChained_UnlinkAndSpliceNode(int param_1);
	// !DECL 0x0042f890 END
	// !DECL 0x0042f8b0 BEGIN
	/* 42F8B0 */ void* CDSChain_GetChildAtIndex(int param_1);
	// !DECL 0x0042f8b0 END
	// !DECL 0x0042f940 BEGIN
	/* 42F940 */ void CDSChain_RemoveListNode(void* param_1, void* param_2, char param_3);
	// !DECL 0x0042f940 END
	// !DECL 0x0042fa20 BEGIN
	/* 42FA20 */ uchar CDSChained_InsertBeforeWithHeadFixup(CBulanek* param_1, void* param_2);
	// !DECL 0x0042fa20 END
	// !DECL 0x0042fa50 BEGIN
	/* 42FA50 */ uchar CDSChained_RemoveWithHeadFixup(CBulanek* param_1, int param_2);
	// !DECL 0x0042fa50 END
	// !DECL 0x0042fab0 BEGIN
	/* 42FAB0 */ void CDSChained_ClearChildren(char param_1);
	// !DECL 0x0042fab0 END
};

#endif
