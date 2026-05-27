#ifndef _C_SCORE_ITEM
#define _C_SCORE_ITEM

#include <globals.h>

class CScoreItem {
public:
	// !DECL 0x00408f50 BEGIN
	/* 408F50 */ uchar Deserialize(int* param_1);
	// !DECL 0x00408f50 END
	// !DECL 0x00408f90 BEGIN
	/* 408F90 */ uchar Serialize(int* param_1);
	// !DECL 0x00408f90 END
	// !DECL 0x004093a0 BEGIN
	/* 4093A0 */ uchar* GetClassIdentifier();
	// !DECL 0x004093a0 END
	// !DECL 0x004093b0 BEGIN
	/* 4093B0 */ uchar DeletingDestructorThunk_4(uchar param_1);
	// !DECL 0x004093b0 END
	// !DECL 0x004093c0 BEGIN
	/* 4093C0 */ uchar Destructor(uint* param_1);
	// !DECL 0x004093c0 END
	// !DECL 0x00409a90 BEGIN
	/* 409A90 */ void* ScalarDeletingDestructor(uchar param_1);
	// !DECL 0x00409a90 END
};

#endif
