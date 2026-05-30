#ifndef _C_D_S_OBJECT
#define _C_D_S_OBJECT

#include <globals.h>

class CDSObject {
public:
	// !DECL 0x004011e0 BEGIN
	/* 4011E0 */ char** CDSObject_GetClassTable();
	// !DECL 0x004011e0 END
	// !DECL 0x00403510 BEGIN
	/* 403510 */ uchar CDSObject_AssignIDSReferencedVftable(uint* param_1);
	// !DECL 0x00403510 END
	// !DECL 0x00414a80 BEGIN
	/* 414A80 */ CGame* CGame_ctor(CGame* param_1);
	// !DECL 0x00414a80 END
	// !DECL 0x004245c0 BEGIN
	/* 4245C0 */ uchar CDSObject_ReleaseViaVtable(int* param_1);
	// !DECL 0x004245c0 END
	// !DECL 0x00425620 BEGIN
	/* 425620 */ void* CDSObject_CtorWithImage(int* param_1, int param_2, char param_3);
	// !DECL 0x00425620 END
	// !DECL 0x004256c0 BEGIN
	/* 4256C0 */ void CDSObject_dtor(void* param_1);
	// !DECL 0x004256c0 END
	// !DECL 0x0042eb00 BEGIN
	/* 42EB00 */ uchar CDSUpdatedItem_ReleaseSchedulerAndAudioBank(uchar param_1);
	// !DECL 0x0042eb00 END
	// !DECL 0x0042f140 BEGIN
	/* 42F140 */ void CDSUpdatedItem_dtor(CDSUpdatedItem* param_1);
	// !DECL 0x0042f140 END
	// !DECL 0x00439c70 BEGIN
	/* 439C70 */ void* ConstructTrackManager(void* param_1);
	// !DECL 0x00439c70 END
	// !DECL 0x0043b060 BEGIN
	/* 43B060 */ CDSDirectPlaySender* CDSDirectPlaySender_ctor(CDSDirectPlaySender* param_1);
	// !DECL 0x0043b060 END
	// !DECL 0x0043c7c0 BEGIN
	/* 43C7C0 */ uint* CDSDirectSound_ctor(CDSDirectSound* param_1);
	// !DECL 0x0043c7c0 END
	// !DECL 0x00447170 BEGIN
	/* 447170 */ uchar CDSObject_ClearRefcountFields(int param_1);
	// !DECL 0x00447170 END
	// !DECL 0x004471d0 BEGIN
	/* 4471D0 */ uchar FUN_004471d0(char param_1);
	// !DECL 0x004471d0 END
};

#endif
