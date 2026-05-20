#ifndef _C_LEVEL_SCORE
#define _C_LEVEL_SCORE

#include <globals.h>

class CLevelScore {
public:
	// !DECL 0x00409050 BEGIN
	/* 409050 */ uchar Deserialize(int* param_1);
	// !DECL 0x00409050 END
	// !DECL 0x00409310 BEGIN
	/* 409310 */ uchar* GetClassIdentifier();
	// !DECL 0x00409310 END
	// !DECL 0x00409320 BEGIN
	/* 409320 */ uchar DeletingDestructorThunk_4(uchar param_1);
	// !DECL 0x00409320 END
	// !DECL 0x00409330 BEGIN
	/* 409330 */ uchar CLevelScore_dtor(uint* param_1);
	// !DECL 0x00409330 END
	// !DECL 0x00409690 BEGIN
	/* 409690 */ uchar Serialize(void* param_1);
	// !DECL 0x00409690 END
	// !DECL 0x00409a40 BEGIN
	/* 409A40 */ uint* CLevelScore_scalar_deleting_dtor(uchar param_1);
	// !DECL 0x00409a40 END
};

#endif
