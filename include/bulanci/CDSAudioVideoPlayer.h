#ifndef _C_D_S_AUDIO_VIDEO_PLAYER
#define _C_D_S_AUDIO_VIDEO_PLAYER

#include <globals.h>

class CDSAudioVideoPlayer {
public:
	// !DECL 0x0043bba0 BEGIN
	/* 43BBA0 */ uchar SetupTrack(int* param_1, int* param_2, uint param_3, uchar* param_4);
	// !DECL 0x0043bba0 END
	// !DECL 0x0043bbf0 BEGIN
	/* 43BBF0 */ uchar Play(uchar param_1);
	// !DECL 0x0043bbf0 END
	// !DECL 0x0043bc10 BEGIN
	/* 43BC10 */ uchar FUN_0043bc10(uint* param_1);
	// !DECL 0x0043bc10 END
	// !DECL 0x0043bc90 BEGIN
	/* 43BC90 */ uchar* FUN_0043bc90();
	// !DECL 0x0043bc90 END
	// !DECL 0x0043bca0 BEGIN
	/* 43BCA0 */ uint* Constructor(uint* param_1);
	// !DECL 0x0043bca0 END
	// !DECL 0x0043bd40 BEGIN
	/* 43BD40 */ uint* FUN_0043bd40(uchar param_1);
	// !DECL 0x0043bd40 END
};

#endif
