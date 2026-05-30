#ifndef _C_SHOT_COUNTER
#define _C_SHOT_COUNTER

#include <globals.h>

class CShotCounter {
public:
	// !DECL 0x00426e10 BEGIN
	/* 426E10 */ void Render(uchar param_1);
	// !DECL 0x00426e10 END
	// !DECL 0x00427210 BEGIN
	/* 427210 */ void* Constructor(void* param_1);
	// !DECL 0x00427210 END
	// !DECL 0x00427290 BEGIN
	/* 427290 */ uchar* GetClassIdentifier();
	// !DECL 0x00427290 END
	// !DECL 0x004272a0 BEGIN
	/* 4272A0 */ uchar DeletingDestructorThunk_10(uchar param_1);
	// !DECL 0x004272a0 END
	// !DECL 0x004272b0 BEGIN
	/* 4272B0 */ uchar DeletingDestructorThunk_18(uchar param_1);
	// !DECL 0x004272b0 END
	// !DECL 0x004272d0 BEGIN
	/* 4272D0 */ void Destructor(uchar param_1);
	// !DECL 0x004272d0 END
	// !DECL 0x00427a60 BEGIN
	/* 427A60 */ uchar ScalarDeletingDestructor(uchar param_1);
	// !DECL 0x00427a60 END
	// !DECL 0x00448b50 BEGIN
	/* 448B50 */ ulonglong FUN_00448b50(uint param_1, uint param_2);
	// !DECL 0x00448b50 END
};

#endif
