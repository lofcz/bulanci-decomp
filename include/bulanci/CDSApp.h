#ifndef _C_D_S_APP
#define _C_D_S_APP

#include <globals.h>

class CDSApp {
public:
	// !DECL 0x004033a0 BEGIN
	/* 4033A0 */ uint CDSView_GetDataKindStubZero(int param_1);
	// !DECL 0x004033a0 END
	// !DECL 0x00429db0 BEGIN
	/* 429DB0 */ uchar CDSApp_DispatchInputEvent(CDSEventRecord* param_1);
	// !DECL 0x00429db0 END
	// !DECL 0x0042afd0 BEGIN
	/* 42AFD0 */ void* CDSApp_InitCreateObjectShell();
	// !DECL 0x0042afd0 END
	// !DECL 0x0042b130 BEGIN
	/* 42B130 */ uchar* CDSApp_GetClassTable();
	// !DECL 0x0042b130 END
	// !DECL 0x0042b140 BEGIN
	/* 42B140 */ uchar CDSApp_Referenced_GetTypeInfo(uchar param_1);
	// !DECL 0x0042b140 END
	// !DECL 0x0042b150 BEGIN
	/* 42B150 */ uchar CDSApp_EventHandler_GetTypeInfo(uchar param_1);
	// !DECL 0x0042b150 END
	// !DECL 0x0042b160 BEGIN
	/* 42B160 */ uchar CDSApp_Chain_GetTypeInfo(uchar param_1);
	// !DECL 0x0042b160 END
	// !DECL 0x0042b3d0 BEGIN
	/* 42B3D0 */ uchar CDSApp_ShutdownFromScalarDtor(uint* param_1);
	// !DECL 0x0042b3d0 END
	// !DECL 0x0042b477 BEGIN
	/* 42B477 */ uchar* Catch_0042b477();
	// !DECL 0x0042b477 END
	// !DECL 0x0042b48c BEGIN
	/* 42B48C */ void CDSApp_ReleaseMembers();
	// !DECL 0x0042b48c END
	// !DECL 0x0042b560 BEGIN
	/* 42B560 */ void CDSApp_dtor(uchar param_1);
	// !DECL 0x0042b560 END
	// !DECL 0x0042b8e0 BEGIN
	/* 42B8E0 */ uchar CDSApp_ChainDtorBody(int param_1);
	// !DECL 0x0042b8e0 END
	// !DECL 0x0042b8f0 BEGIN
	/* 42B8F0 */ uchar CDSApp_ReferencedDtorBody(int param_1);
	// !DECL 0x0042b8f0 END
	// !DECL 0x0042b900 BEGIN
	/* 42B900 */ uchar CDSApp_EventHandlerDtorBody(int param_1);
	// !DECL 0x0042b900 END
	// !DECL 0x0042b980 BEGIN
	/* 42B980 */ void* CDSApp_DtorScalar(uchar param_1);
	// !DECL 0x0042b980 END
	// !DECL 0x0042e770 BEGIN
	/* 42E770 */ uchar CDSApp_InitClock();
	// !DECL 0x0042e770 END
	// !DECL 0x0042f530 BEGIN
	/* 42F530 */ uchar CBulanci_ReleaseResourceIndexSlots();
	// !DECL 0x0042f530 END
	// !DECL 0x00437fb0 BEGIN
	/* 437FB0 */ uchar RegWriteDword(int param_1);
	// !DECL 0x00437fb0 END
	// !DECL 0x0043c9f0 BEGIN
	/* 43C9F0 */ static uchar CDSApp_FillPCMWaveFormat(ushort* param_1, ushort param_2, int param_3, ushort param_4);
	// !DECL 0x0043c9f0 END
	// !DECL 0x0043cb20 BEGIN
	/* 43CB20 */ int CDSApp_CreateSoundBuffer(ushort param_1, int param_2, ushort param_3, uint param_4, uint param_5);
	// !DECL 0x0043cb20 END
	// !DECL 0x004465b0 BEGIN
	/* 4465B0 */ uchar CDSApp_FreeAlphaBlendLut();
	// !DECL 0x004465b0 END
	// !DECL 0x00467430 BEGIN
	/* 467430 */ void CDSApp_PreCreateHook();
	// !DECL 0x00467430 END

	// !DECL 0x00429bd0 BEGIN
	/* 429BD0 */ bool CDSApp_RestoreLostSurfaces();
	// !DECL 0x00429bd0 END
	// !DECL 0x0042b170 BEGIN
	/* 42B170 */ void CDSApp_ctor(short* param_1, short* param_2);
	// !DECL 0x0042b170 END
	// !DECL 0x0042b5a0 BEGIN
	/* 42B5A0 */ uchar CDSApp_AddDirtyRectCoalesced(int* param_1, int* param_2);
	// !DECL 0x0042b5a0 END
	// !DECL 0x0042b8a0 BEGIN
	/* 42B8A0 */ uchar CDSApp_AddDirtyRect(int* param_1);
	// !DECL 0x0042b8a0 END
};

#endif
