#ifndef _C_NUM_COUNTER
#define _C_NUM_COUNTER

#include <globals.h>

class CNumCounter {
public:
	// !DECL 0x00427020 BEGIN
	/* 427020 */ uchar CNumCounter_UpdateDigit(int param_1, uchar* param_2);
	// !DECL 0x00427020 END
	// !DECL 0x004270a0 BEGIN
	/* 4270A0 */ uchar CNumCounter_OnTimerTick(void* param_1);
	// !DECL 0x004270a0 END
	// !DECL 0x00427340 BEGIN
	/* 427340 */ uchar CNumCounter_ctor(uchar param_1);
	// !DECL 0x00427340 END
	// !DECL 0x004273e0 BEGIN
	/* 4273E0 */ uchar* CNumCounter_GetTypeDescriptor();
	// !DECL 0x004273e0 END
	// !DECL 0x004273f0 BEGIN
	/* 4273F0 */ uchar CNumCounter_AdjustorThunk04_Dtor(uchar param_1);
	// !DECL 0x004273f0 END
	// !DECL 0x00427400 BEGIN
	/* 427400 */ uchar CNumCounter_AdjustorThunk10_Dtor(uchar param_1);
	// !DECL 0x00427400 END
	// !DECL 0x00427410 BEGIN
	/* 427410 */ uchar CNumCounter_AdjustorThunk18_Dtor(uchar param_1);
	// !DECL 0x00427410 END
	// !DECL 0x00427420 BEGIN
	/* 427420 */ uchar CNumCounter_AdjustorThunk68_Dtor(uchar param_1);
	// !DECL 0x00427420 END
	// !DECL 0x00427430 BEGIN
	/* 427430 */ void CNumCounter_dtor(uchar param_1);
	// !DECL 0x00427430 END
	// !DECL 0x00427740 BEGIN
	/* 427740 */ void CNumCounter_Render(uchar param_1);
	// !DECL 0x00427740 END
	// !DECL 0x00427af0 BEGIN
	/* 427AF0 */ uchar CNumCounter_vDtor(uchar param_1);
	// !DECL 0x00427af0 END
};

#endif
