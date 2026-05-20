#ifndef _C_D_S_DSM_FILE
#define _C_D_S_DSM_FILE

#include <globals.h>

class CDSDsmFile {
public:
	// !DECL 0x004289a0 BEGIN
	/* 4289A0 */ uint* InitializeQueueStreamView(int param_1, int param_2, int param_3);
	// !DECL 0x004289a0 END
	// !DECL 0x00428ad0 BEGIN
	/* 428AD0 */ uchar HandleResourceRead(int* param_1, int param_2, uint param_3);
	// !DECL 0x00428ad0 END
	// !DECL 0x00428c40 BEGIN
	/* 428C40 */ uchar HandleRecordRead(int* param_1, void* param_2);
	// !DECL 0x00428c40 END
	// !DECL 0x00428d10 BEGIN
	/* 428D10 */ uchar Catch_00428d10();
	// !DECL 0x00428d10 END
	// !DECL 0x00428da0 BEGIN
	/* 428DA0 */ uchar* GetClassMetaA();
	// !DECL 0x00428da0 END
	// !DECL 0x00428db0 BEGIN
	/* 428DB0 */ uchar* GetClassRegistry();
	// !DECL 0x00428db0 END
	// !DECL 0x00428dc0 BEGIN
	/* 428DC0 */ uchar FUN_00428dc0(uchar param_1);
	// !DECL 0x00428dc0 END
	// !DECL 0x00428de0 BEGIN
	/* 428DE0 */ uchar FUN_00428de0(uchar param_1);
	// !DECL 0x00428de0 END
	// !DECL 0x00428df0 BEGIN
	/* 428DF0 */ uchar FUN_00428df0(uchar param_1);
	// !DECL 0x00428df0 END
	// !DECL 0x00428e00 BEGIN
	/* 428E00 */ uchar FUN_00428e00(uchar param_1);
	// !DECL 0x00428e00 END
	// !DECL 0x00428e10 BEGIN
	/* 428E10 */ uchar FUN_00428e10(int param_1);
	// !DECL 0x00428e10 END
	// !DECL 0x00428e30 BEGIN
	/* 428E30 */ uchar FUN_00428e30(uchar param_1);
	// !DECL 0x00428e30 END
	// !DECL 0x00428e40 BEGIN
	/* 428E40 */ uchar HandleRefcountRelease(int param_1);
	// !DECL 0x00428e40 END
	// !DECL 0x00428e70 BEGIN
	/* 428E70 */ uchar FUN_00428e70(int param_1);
	// !DECL 0x00428e70 END
	// !DECL 0x00428e80 BEGIN
	/* 428E80 */ uchar FUN_00428e80(int param_1);
	// !DECL 0x00428e80 END
	// !DECL 0x00428e90 BEGIN
	/* 428E90 */ uchar FUN_00428e90(int param_1);
	// !DECL 0x00428e90 END
	// !DECL 0x00428ea0 BEGIN
	/* 428EA0 */ uchar FUN_00428ea0(int param_1);
	// !DECL 0x00428ea0 END
	// !DECL 0x00428eb0 BEGIN
	/* 428EB0 */ uchar FUN_00428eb0(int param_1);
	// !DECL 0x00428eb0 END
	// !DECL 0x00428ec0 BEGIN
	/* 428EC0 */ uchar HandleDestructInstance(uint* param_1);
	// !DECL 0x00428ec0 END
	// !DECL 0x00428f80 BEGIN
	/* 428F80 */ uchar HandleOpenStream(int* param_1);
	// !DECL 0x00428f80 END
	// !DECL 0x00428ff0 BEGIN
	/* 428FF0 */ uchar HandleReleaseResource(uchar* param_1, uchar param_2);
	// !DECL 0x00428ff0 END
	// !DECL 0x00429060 BEGIN
	/* 429060 */ uint* HandleScalarDelete(uchar param_1);
	// !DECL 0x00429060 END
	// !DECL 0x00429080 BEGIN
	/* 429080 */ uchar HandleReleaseReadThunk(uchar* param_1);
	// !DECL 0x00429080 END
	// !DECL 0x004290a0 BEGIN
	/* 4290A0 */ uchar HandleReleaseWriteThunk(uchar* param_1);
	// !DECL 0x004290a0 END
	// !DECL 0x004290c0 BEGIN
	/* 4290C0 */ uchar* HandleAcquireResource(uchar param_1);
	// !DECL 0x004290c0 END
	// !DECL 0x00429190 BEGIN
	/* 429190 */ uchar HandleAcquireReadThunk(int param_1);
	// !DECL 0x00429190 END
	// !DECL 0x004291b0 BEGIN
	/* 4291B0 */ uchar HandleAcquireWriteThunk(int param_1);
	// !DECL 0x004291b0 END
	// !DECL 0x00433010 BEGIN
	/* 433010 */ uchar FUN_00433010(int param_1);
	// !DECL 0x00433010 END
	// !DECL 0x00439ac0 BEGIN
	/* 439AC0 */ uint* InitializeChildObject(int param_1);
	// !DECL 0x00439ac0 END
	// !DECL 0x0043bde0 BEGIN
	/* 43BDE0 */ uchar InitializeSourceRegion(int param_1, int param_2, int param_3);
	// !DECL 0x0043bde0 END
	// !DECL 0x0043be78 BEGIN
	/* 43BE78 */ uchar Catch_0043be78();
	// !DECL 0x0043be78 END
	// !DECL 0x0043c2e0 BEGIN
	/* 43C2E0 */ uchar HandleQueueSkip(int param_1);
	// !DECL 0x0043c2e0 END
	// !DECL 0x0043c360 BEGIN
	/* 43C360 */ uchar Catch_0043c360();
	// !DECL 0x0043c360 END
	// !DECL 0x0043c450 BEGIN
	/* 43C450 */ uchar HandleQueueResize(int param_1);
	// !DECL 0x0043c450 END
	// !DECL 0x0043c620 BEGIN
	/* 43C620 */ uchar HandleQueueWrite(void* param_1, void* param_2);
	// !DECL 0x0043c620 END
	// !DECL 0x0043c6d2 BEGIN
	/* 43C6D2 */ uchar Catch_0043c6d2();
	// !DECL 0x0043c6d2 END
	// !DECL 0x0043c6f0 BEGIN
	/* 43C6F0 */ uchar HandleQueueAppendStream(int* param_1, int param_2);
	// !DECL 0x0043c6f0 END
	// !DECL 0x0043c79a BEGIN
	/* 43C79A */ uchar Catch_0043c79a();
	// !DECL 0x0043c79a END
};

#endif
