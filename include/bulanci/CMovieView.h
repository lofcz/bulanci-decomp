#ifndef _C_MOVIE_VIEW
#define _C_MOVIE_VIEW

#include <globals.h>

class CMovieView {
public:
	// !DECL 0x00422630 BEGIN
	/* 422630 */ void StopPlayback(void* param_1);
	// !DECL 0x00422630 END
	// !DECL 0x00422840 BEGIN
	/* 422840 */ uchar OnMovieStop(char param_1);
	// !DECL 0x00422840 END
	// !DECL 0x00422860 BEGIN
	/* 422860 */ uchar OnEvent(short param_1, void* param_2);
	// !DECL 0x00422860 END
	// !DECL 0x00422a20 BEGIN
	/* 422A20 */ uchar* GetClassTable();
	// !DECL 0x00422a20 END
	// !DECL 0x00422a40 BEGIN
	/* 422A40 */ uchar ScalarDeletingDtor_Thunk_104(uchar param_1);
	// !DECL 0x00422a40 END
	// !DECL 0x00422a50 BEGIN
	/* 422A50 */ uchar ScalarDeletingDtor_Thunk_16(uchar param_1);
	// !DECL 0x00422a50 END
	// !DECL 0x00422a60 BEGIN
	/* 422A60 */ uchar ScalarDeletingDtor_Thunk_24(uchar param_1);
	// !DECL 0x00422a60 END
	// !DECL 0x00422cb0 BEGIN
	/* 422CB0 */ uchar Cleanup(uchar param_1);
	// !DECL 0x00422cb0 END
	// !DECL 0x00422d50 BEGIN
	/* 422D50 */ uchar StartPlayback(uchar param_1);
	// !DECL 0x00422d50 END
	// !DECL 0x00422e30 BEGIN
	/* 422E30 */ uchar CMovieView_CreateObject();
	// !DECL 0x00422e30 END
	// !DECL 0x00422ee0 BEGIN
	/* 422EE0 */ void* ScalarDeletingDtor(uchar param_1);
	// !DECL 0x00422ee0 END
	// !DECL 0x004231a0 BEGIN
	/* 4231A0 */ uchar TogglePlayback(void* param_1);
	// !DECL 0x004231a0 END
	// !DECL 0x004236a0 BEGIN
	/* 4236A0 */ uchar Constructor(uint param_1, uint param_2, uint param_3, uint param_4, uint param_5);
	// !DECL 0x004236a0 END
	// !DECL 0x00423787 BEGIN
	/* 423787 */ uchar* Catch_00423787();
	// !DECL 0x00423787 END
	// !DECL 0x004237b0 BEGIN
	/* 4237B0 */ uchar InitTrackSequence();
	// !DECL 0x004237b0 END
};

#endif
