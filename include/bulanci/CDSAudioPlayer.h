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
	/* 43A650 */ void CDSAudioPlayer_dtor(uchar param_1);
	// !DECL 0x0043a650 END
	// !DECL 0x0043a740 BEGIN
	/* 43A740 */ void* CDSAudioPlayer_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0043a740 END

	// !DECL 0x0043a060 BEGIN
	/* 43A060 */ void CDSAudioPlayer_ApplyEffectiveVolume(char param_1);
	// !DECL 0x0043a060 END
	// !DECL 0x0043a0f0 BEGIN
	/* 43A0F0 */ bool CDSAudioPlayer_RefillDirectSoundBuffer();
	// !DECL 0x0043a0f0 END
	// !DECL 0x0043a1f0 BEGIN
	/* 43A1F0 */ bool CDSAudioPlayer_OnPlaybackTick();
	// !DECL 0x0043a1f0 END
	// !DECL 0x0043a350 BEGIN
	/* 43A350 */ void CDSAudioPlayer_FillDirectSoundBuffer(void* param_1);
	// !DECL 0x0043a350 END
	// !DECL 0x0043a4a0 BEGIN
	/* 43A4A0 */ uchar CDSAudioPlayer_Stop(uchar param_1);
	// !DECL 0x0043a4a0 END
	// !DECL 0x0043a760 BEGIN
	/* 43A760 */ void CDSAudioPlayer_Init(IDSAudioSource* param_1, uint param_2, void* param_3, uchar* param_4);
	// !DECL 0x0043a760 END
	// !DECL 0x0043a9d0 BEGIN
	/* 43A9D0 */ uchar CDSAudioPlayer_Play(uchar param_1);
	// !DECL 0x0043a9d0 END
};

#endif
