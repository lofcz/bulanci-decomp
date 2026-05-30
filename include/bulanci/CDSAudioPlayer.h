#ifndef _C_D_S_AUDIO_PLAYER
#define _C_D_S_AUDIO_PLAYER

#include <globals.h>

class CDSAudioPlayer {
public:
	// !DECL 0x0043a4f0 BEGIN
	/* 43A4F0 */ void CDSAudioPlayer_ctor(void* param_1);
	// !DECL 0x0043a4f0 END
	// !DECL 0x0043a570 BEGIN
	/* 43A570 */ uchar* CDSAudioPlayer_GetTypeInfo();
	// !DECL 0x0043a570 END
	// !DECL 0x0043a580 BEGIN
	/* 43A580 */ uchar CDSAudioPlayer_ScalarDeletingDtor_thunk_Sub4(uchar param_1);
	// !DECL 0x0043a580 END
	// !DECL 0x0043a650 BEGIN
	/* 43A650 */ void CDSAudioPlayer_dtor(void* param_1);
	// !DECL 0x0043a650 END
	// !DECL 0x0043a740 BEGIN
	/* 43A740 */ void* CDSAudioPlayer_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0043a740 END
};

#endif
