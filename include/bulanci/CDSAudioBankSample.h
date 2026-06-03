#ifndef _C_D_S_AUDIO_BANK_SAMPLE
#define _C_D_S_AUDIO_BANK_SAMPLE

#include <globals.h>

class CDSAudioBankSample {
public:
	// !DECL 0x00429340 BEGIN
	/* 429340 */ uchar* CDSAudioBankSample_GetTypeInfo();
	// !DECL 0x00429340 END
	// !DECL 0x00429350 BEGIN
	/* 429350 */ uchar CDSAudioBankSample_ScalarDeletingDtorThunk(uchar param_1);
	// !DECL 0x00429350 END
	// !DECL 0x00429360 BEGIN
	/* 429360 */ uchar CDSAudioBankSample_ScalarDeletingDtor_thunk_Sub18(uchar param_1);
	// !DECL 0x00429360 END
	// !DECL 0x00429370 BEGIN
	/* 429370 */ uchar CDSAudioBankSample_dtor(uint* param_1);
	// !DECL 0x00429370 END
	// !DECL 0x00429510 BEGIN
	/* 429510 */ void* CDSAudioBankSample_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00429510 END

	// !DECL 0x00429530 BEGIN
	/* 429530 */ void* CDSAudioBankSample_ctor(IDSAudioSource* param_1, uint param_2, uchar* param_3);
	// !DECL 0x00429530 END
};

#endif
