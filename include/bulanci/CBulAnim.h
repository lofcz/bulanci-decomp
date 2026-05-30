#ifndef _C_BUL_ANIM
#define _C_BUL_ANIM

#include <globals.h>

class CBulAnim {
public:
	// !DECL 0x0040ad20 BEGIN
	/* 40AD20 */ void CleanupBody(void* param_1);
	// !DECL 0x0040ad20 END
	// !DECL 0x0040afd0 BEGIN
	/* 40AFD0 */ uchar ApplyTeamPalette(uchar param_1);
	// !DECL 0x0040afd0 END
	// !DECL 0x0040b020 BEGIN
	/* 40B020 */ uchar OnTeamPaletteEvt(short param_1, uint param_2);
	// !DECL 0x0040b020 END
	// !DECL 0x0040b820 BEGIN
	/* 40B820 */ uchar* GetTypeInfo_Sub68();
	// !DECL 0x0040b820 END
	// !DECL 0x0040b830 BEGIN
	/* 40B830 */ uchar* GetTypeInfo_Sub78();
	// !DECL 0x0040b830 END
	// !DECL 0x0040b840 BEGIN
	/* 40B840 */ uchar PrimaryTick_thunk(int param_1);
	// !DECL 0x0040b840 END
	// !DECL 0x0040b850 BEGIN
	/* 40B850 */ uchar PrimaryRender_thunk(char param_1);
	// !DECL 0x0040b850 END
	// !DECL 0x0040b870 BEGIN
	/* 40B870 */ uchar* PrimaryDtor_thunk();
	// !DECL 0x0040b870 END
	// !DECL 0x0040b880 BEGIN
	/* 40B880 */ uchar Sub78_CDSObjectAdjust_thunk(int param_1);
	// !DECL 0x0040b880 END
	// !DECL 0x0040b890 BEGIN
	/* 40B890 */ uchar Sub78Dtor_thunk(uchar param_1);
	// !DECL 0x0040b890 END
	// !DECL 0x0040b8a0 BEGIN
	/* 40B8A0 */ uchar Sub68Dtor_thunk(uchar param_1);
	// !DECL 0x0040b8a0 END
	// !DECL 0x0040eaa0 BEGIN
	/* 40EAA0 */ uchar CreateCBulAnim();
	// !DECL 0x0040eaa0 END
	// !DECL 0x00438f20 BEGIN
	/* 438F20 */ uchar IDSAnim_NotifyEvents(uint param_1, int param_2, int param_3);
	// !DECL 0x00438f20 END
	// !DECL 0x00438f60 BEGIN
	/* 438F60 */ uchar IDSAnim_BindUserData(uint param_1, int* param_2);
	// !DECL 0x00438f60 END
	// !DECL 0x00439010 BEGIN
	/* 439010 */ void SetPalette(uint* param_1);
	// !DECL 0x00439010 END
	// !DECL 0x004391d0 BEGIN
	/* 4391D0 */ uchar IDSAnim_SetSequence_thunk(uint param_1, int* param_2);
	// !DECL 0x004391d0 END
	// !DECL 0x004392a0 BEGIN
	/* 4392A0 */ uchar IDSAnim_SelectRandomTrack(uint param_1, short param_2);
	// !DECL 0x004392a0 END
	// !DECL 0x00439530 BEGIN
	/* 439530 */ uchar CBulAnim_Sub10Dtor_thunk(uchar param_1);
	// !DECL 0x00439530 END
	// !DECL 0x00439540 BEGIN
	/* 439540 */ uchar Sub18Dtor_thunk(uchar param_1);
	// !DECL 0x00439540 END
	// !DECL 0x00439550 BEGIN
	/* 439550 */ uchar Sub78ChainOp_thunk(int param_1);
	// !DECL 0x00439550 END
	// !DECL 0x004396f0 BEGIN
	/* 4396F0 */ void* ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004396f0 END
};

#endif
