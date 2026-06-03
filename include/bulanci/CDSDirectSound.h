#ifndef _C_D_S_DIRECT_SOUND
#define _C_D_S_DIRECT_SOUND

#include <globals.h>

class CDSDirectSound {
public:
	// !DECL 0x0043a483 BEGIN
	/* 43A483 */ uchar Catch_0043a483();
	// !DECL 0x0043a483 END
	// !DECL 0x0043c8a0 BEGIN
	/* 43C8A0 */ uchar* CDSDirectSound_GetTypeInfo();
	// !DECL 0x0043c8a0 END
	// !DECL 0x0043c8b0 BEGIN
	/* 43C8B0 */ uchar ~CDSDirectSound(uint* param_1);
	// !DECL 0x0043c8b0 END
	// !DECL 0x0043ca50 BEGIN
	/* 43CA50 */ uchar CDSDirectSound_DtorScalar_minus04(uchar param_1);
	// !DECL 0x0043ca50 END
	// !DECL 0x0043ca60 BEGIN
	/* 43CA60 */ uchar CDSDirectSound_ScalarDeletingDtor_thunk_Sub24(uchar param_1);
	// !DECL 0x0043ca60 END
	// !DECL 0x0043ca70 BEGIN
	/* 43CA70 */ uchar CDSObject_ReleaseViaVtable_ThisMinus24(int param_1);
	// !DECL 0x0043ca70 END
	// !DECL 0x0043ca80 BEGIN
	/* 43CA80 */ uchar CDSDirectSound_DtorScalar_minus1c(uchar param_1);
	// !DECL 0x0043ca80 END
	// !DECL 0x0043cb00 BEGIN
	/* 43CB00 */ void* CDSDirectSound_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0043cb00 END
	// !DECL 0x0043ccb0 BEGIN
	/* 43CCB0 */ void CDSDirectSound_SignalEventIfVoicesActive(void* param_1);
	// !DECL 0x0043ccb0 END
	// !DECL 0x0043cd00 BEGIN
	/* 43CD00 */ void CDSDirectSound_WorkerThreadLoop(void* param_1);
	// !DECL 0x0043cd00 END
	// !DECL 0x0043cdc0 BEGIN
	/* 43CDC0 */ uchar CDSDirectSound_OnPlaybackCompleteMessage(int* param_1);
	// !DECL 0x0043cdc0 END

	// !DECL 0x0043cbc0 BEGIN
	/* 43CBC0 */ void CDSDirectSound_InitPrimary(void* param_1, ushort param_2, int param_3, ushort param_4);
	// !DECL 0x0043cbc0 END
};

#endif
