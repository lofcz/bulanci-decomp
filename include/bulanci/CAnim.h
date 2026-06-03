#ifndef _C_ANIM
#define _C_ANIM

#include <globals.h>

class CAnim {
public:
	// !DECL 0x004164f0 BEGIN
	/* 4164F0 */ void CAnim_RenderAnimFrame(int param_1);
	// !DECL 0x004164f0 END
	// !DECL 0x00419870 BEGIN
	/* 419870 */ uchar CAnim_SubobjectCtor(uchar param_1);
	// !DECL 0x00419870 END
	// !DECL 0x00419920 BEGIN
	/* 419920 */ uchar* CAnim_GetClassTable();
	// !DECL 0x00419920 END
	// !DECL 0x00419930 BEGIN
	/* 419930 */ uchar CAnim_ReleaseViaVtable_thunk_Sub98(int param_1);
	// !DECL 0x00419930 END
	// !DECL 0x00419940 BEGIN
	/* 419940 */ void* CAnim_ctor(int param_1, int param_2, int* param_3, int param_4);
	// !DECL 0x00419940 END
	// !DECL 0x00419a90 BEGIN
	/* 419A90 */ uchar CAnim_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x00419a90 END
	// !DECL 0x00419b10 BEGIN
	/* 419B10 */ uchar CAnim_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x00419b10 END
	// !DECL 0x00419b20 BEGIN
	/* 419B20 */ uchar CAnim_ScalarDeletingDtor_thunk_Sub98(uchar param_1);
	// !DECL 0x00419b20 END
	// !DECL 0x00419b30 BEGIN
	/* 419B30 */ uchar CAnim_ScalarDeletingDtor_thunk_Sub88(uchar param_1);
	// !DECL 0x00419b30 END
	// !DECL 0x0041ab30 BEGIN
	/* 41AB30 */ uchar CAnim_ScalarDeletingDtor_thunk_Sub10(uchar param_1);
	// !DECL 0x0041ab30 END
	// !DECL 0x0041ab40 BEGIN
	/* 41AB40 */ uchar CDSChain_AdjustThisOffset_ThisMinus94(int param_1);
	// !DECL 0x0041ab40 END
	// !DECL 0x0041ab50 BEGIN
	/* 41AB50 */ void* CAnim_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0041ab50 END
	// !DECL 0x0041b190 BEGIN
	/* 41B190 */ uchar CAnim_OnSchedulerEnqueueSlot0(void* param_1);
	// !DECL 0x0041b190 END
};

#endif
