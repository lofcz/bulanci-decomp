#ifndef _C_HISTORY_SCRIPT
#define _C_HISTORY_SCRIPT

#include <globals.h>

class CHistoryScript {
public:
	// !DECL 0x004226c0 BEGIN
	/* 4226C0 */ void* ctor(void* param_1);
	// !DECL 0x004226c0 END
	// !DECL 0x00422760 BEGIN
	/* 422760 */ uchar* CHistoryScript_GetClassTable();
	// !DECL 0x00422760 END
	// !DECL 0x00422780 BEGIN
	/* 422780 */ uchar CHistoryScript_ScalarDeletingDtor_thunk(uchar param_1);
	// !DECL 0x00422780 END
	// !DECL 0x00422790 BEGIN
	/* 422790 */ uchar CHistoryScript_ScalarDeletingDtor_thunk_Sub438(uchar param_1);
	// !DECL 0x00422790 END
	// !DECL 0x004227a0 BEGIN
	/* 4227A0 */ uchar CHistoryScript_AdjustOffsetDtor(uchar param_1);
	// !DECL 0x004227a0 END
	// !DECL 0x004227b0 BEGIN
	/* 4227B0 */ void CHistoryScript_dtor(void* param_1);
	// !DECL 0x004227b0 END
	// !DECL 0x00422ae0 BEGIN
	/* 422AE0 */ void* CHistoryScript_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00422ae0 END
	// !DECL 0x00422b00 BEGIN
	/* 422B00 */ static uchar HhBuildStaticTextAuto(int param_1);
	// !DECL 0x00422b00 END
	// !DECL 0x00422bc0 BEGIN
	/* 422BC0 */ static uchar HhBuildStaticTextSized(int param_1);
	// !DECL 0x00422bc0 END
	// !DECL 0x00423530 BEGIN
	/* 423530 */ static CDSBitmap* HhBuildHistoryBitmapWidget(int param_1);
	// !DECL 0x00423530 END
	// !DECL 0x00423820 BEGIN
	/* 423820 */ static uint HBuildHistoryMovieWidget(int param_1);
	// !DECL 0x00423820 END
};

#endif
