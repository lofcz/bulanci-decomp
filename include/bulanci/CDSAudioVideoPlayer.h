#ifndef _C_D_S_AUDIO_VIDEO_PLAYER
#define _C_D_S_AUDIO_VIDEO_PLAYER

#include <globals.h>

class CDSAudioVideoPlayer {
public:
	// !DECL 0x0043bba0 BEGIN
	/* 43BBA0 */ void CDSAudioVideoPlayer_SetupTrack(void* param_1, int* param_2, void* param_3, uchar* param_4);
	// !DECL 0x0043bba0 END
	// !DECL 0x0043bbf0 BEGIN
	/* 43BBF0 */ void CDSAudioVideoPlayer_Play(uchar param_1);
	// !DECL 0x0043bbf0 END
	// !DECL 0x0043bc10 BEGIN
	/* 43BC10 */ void CDSAudioVideoPlayer_dtor(void* param_1);
	// !DECL 0x0043bc10 END
	// !DECL 0x0043bc90 BEGIN
	/* 43BC90 */ uchar* CDSAudioVideoPlayer_GetTypeInfo();
	// !DECL 0x0043bc90 END
	// !DECL 0x0043bca0 BEGIN
	/* 43BCA0 */ void* CDSAudioVideoPlayer_Constructor(void* param_1, int param_2);
	// !DECL 0x0043bca0 END
	// !DECL 0x0043bd40 BEGIN
	/* 43BD40 */ void* CDSAudioVideoPlayer_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x0043bd40 END

	// !DECL 0x0043bc00 BEGIN
	/* 43BC00 */ void CDSAudioVideoPlayer_Stop(void* param_1);
	// !DECL 0x0043bc00 END
};

#endif
