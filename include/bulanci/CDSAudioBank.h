#ifndef _C_D_S_AUDIO_BANK
#define _C_D_S_AUDIO_BANK

#include <globals.h>

class CDSAudioBank {
public:
	// !DECL 0x00429240 BEGIN
	/* 429240 */ void CDSAudioBank_ReleaseSampleSlots(uchar param_1);
	// !DECL 0x00429240 END
	// !DECL 0x004292f0 BEGIN
	/* 4292F0 */ uchar* CDSAudioBank_GetTypeInfo();
	// !DECL 0x004292f0 END
	// !DECL 0x00429310 BEGIN
	/* 429310 */ uchar CDSAudioBank_ScalarDeletingDtor_thunk_Sub0c(uchar param_1);
	// !DECL 0x00429310 END
	// !DECL 0x00429320 BEGIN
	/* 429320 */ uchar CDSAudioBank_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x00429320 END
	// !DECL 0x00429330 BEGIN
	/* 429330 */ uchar CDSAudioBank_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x00429330 END
	// !DECL 0x004293e0 BEGIN
	/* 4293E0 */ uchar CDSAudioBank_dtor(uint* param_1, uint param_2);
	// !DECL 0x004293e0 END
	// !DECL 0x004294b0 BEGIN
	/* 4294B0 */ void* CDSAudioBank_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004294b0 END
	// !DECL 0x0042985a BEGIN
	/* 42985A */ uchar Catch_0042985a();
	// !DECL 0x0042985a END

	// !DECL 0x00429480 BEGIN
	/* 429480 */ void CDSAudioBank_Ctor(void* param_1, void* param_2);
	// !DECL 0x00429480 END
};

#endif
