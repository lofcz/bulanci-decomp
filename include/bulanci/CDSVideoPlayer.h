#ifndef _C_D_S_VIDEO_PLAYER
#define _C_D_S_VIDEO_PLAYER

#include <globals.h>

class CDSVideoPlayer {
public:
	// !DECL 0x00439b90 BEGIN
	/* 439B90 */ uchar CDSVideoPlayer_AdvanceFrameAndPauseIfDone(int param_1);
	// !DECL 0x00439b90 END
	// !DECL 0x00439d10 BEGIN
	/* 439D10 */ uchar* CDSVideoPlayer_GetTypeInfo();
	// !DECL 0x00439d10 END
	// !DECL 0x00439d20 BEGIN
	/* 439D20 */ uchar CDSVideoPlayer_DtorScalar(uchar param_1);
	// !DECL 0x00439d20 END
	// !DECL 0x00439d30 BEGIN
	/* 439D30 */ uchar CDSVideoPlayer_TM_Destructor(uint* param_1);
	// !DECL 0x00439d30 END
	// !DECL 0x00439fc0 BEGIN
	/* 439FC0 */ void* CDSVideoPlayer_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00439fc0 END
};

#endif
