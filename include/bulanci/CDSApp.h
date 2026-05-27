#ifndef _C_D_S_APP
#define _C_D_S_APP

#include <globals.h>

class CDSApp {
public:
	// !DECL 0x004033a0 BEGIN
	/* 4033A0 */ uint CDSView_GetDataKindStubZero(int param_1);
	// !DECL 0x004033a0 END
	// !DECL 0x00429db0 BEGIN
	/* 429DB0 */ uchar CDSApp_DispatchInputEvent(int param_1);
	// !DECL 0x00429db0 END
	// !DECL 0x0042a210 BEGIN
	/* 42A210 */ uchar CDSApp_OnCreate(uchar param_1);
	// !DECL 0x0042a210 END
	// !DECL 0x0042afd0 BEGIN
	/* 42AFD0 */ uint* FUN_0042afd0(uint* param_1);
	// !DECL 0x0042afd0 END
	// !DECL 0x0042b130 BEGIN
	/* 42B130 */ uchar* CDSApp_GetClassTable();
	// !DECL 0x0042b130 END
	// !DECL 0x0042b140 BEGIN
	/* 42B140 */ uchar FUN_0042b140(uchar param_1);
	// !DECL 0x0042b140 END
	// !DECL 0x0042b150 BEGIN
	/* 42B150 */ uchar FUN_0042b150(uchar param_1);
	// !DECL 0x0042b150 END
	// !DECL 0x0042b160 BEGIN
	/* 42B160 */ uchar FUN_0042b160(uchar param_1);
	// !DECL 0x0042b160 END
	// !DECL 0x0042b3d0 BEGIN
	/* 42B3D0 */ uchar FUN_0042b3d0(uint* param_1);
	// !DECL 0x0042b3d0 END
	// !DECL 0x0042b477 BEGIN
	/* 42B477 */ uchar* Catch_0042b477();
	// !DECL 0x0042b477 END
	// !DECL 0x0042b48c BEGIN
	/* 42B48C */ uchar FUN_0042b48c();
	// !DECL 0x0042b48c END
	// !DECL 0x0042b560 BEGIN
	/* 42B560 */ uchar CDSApp_dtor(int* param_1);
	// !DECL 0x0042b560 END
	// !DECL 0x0042b8e0 BEGIN
	/* 42B8E0 */ uchar FUN_0042b8e0(int param_1);
	// !DECL 0x0042b8e0 END
	// !DECL 0x0042b8f0 BEGIN
	/* 42B8F0 */ uchar FUN_0042b8f0(int param_1);
	// !DECL 0x0042b8f0 END
	// !DECL 0x0042b900 BEGIN
	/* 42B900 */ uchar FUN_0042b900(int param_1);
	// !DECL 0x0042b900 END
	// !DECL 0x0042b980 BEGIN
	/* 42B980 */ void* CDSApp_DtorScalar(uchar param_1);
	// !DECL 0x0042b980 END
	// !DECL 0x0042c7d0 BEGIN
	/* 42C7D0 */ uchar FUN_0042c7d0(ushort param_1);
	// !DECL 0x0042c7d0 END
	// !DECL 0x0042ccf0 BEGIN
	/* 42CCF0 */ uchar CDSView_RenderChildrenClipped(int param_1);
	// !DECL 0x0042ccf0 END
	// !DECL 0x0042e770 BEGIN
	/* 42E770 */ uchar CDSApp_InitClock();
	// !DECL 0x0042e770 END
	// !DECL 0x0042f530 BEGIN
	/* 42F530 */ uchar FUN_0042f530();
	// !DECL 0x0042f530 END
	// !DECL 0x00437fb0 BEGIN
	/* 437FB0 */ uchar RegWriteDword(int param_1);
	// !DECL 0x00437fb0 END
	// !DECL 0x0043c9f0 BEGIN
	/* 43C9F0 */ static uchar FUN_0043c9f0(ushort* param_1, ushort param_2, int param_3, ushort param_4);
	// !DECL 0x0043c9f0 END
	// !DECL 0x0043cb20 BEGIN
	/* 43CB20 */ int FUN_0043cb20(ushort param_1, int param_2, ushort param_3, uint param_4, uint param_5);
	// !DECL 0x0043cb20 END
	// !DECL 0x0043cbc0 BEGIN
	/* 43CBC0 */ uchar CDSDirectSound_InitPrimary(uint param_1, ushort param_2, int param_3, ushort param_4);
	// !DECL 0x0043cbc0 END
	// !DECL 0x004465b0 BEGIN
	/* 4465B0 */ uchar FUN_004465b0();
	// !DECL 0x004465b0 END
	// !DECL 0x00467430 BEGIN
	/* 467430 */ void CDSApp_PreCreateHook();
	// !DECL 0x00467430 END
};

#endif
