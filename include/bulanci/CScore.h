#ifndef _C_SCORE
#define _C_SCORE

#include <globals.h>

class CScore {
public:
	// !DECL 0x0040abe0 BEGIN
	/* 40ABE0 */ uint OnKeyPress(char param_1, char param_2);
	// !DECL 0x0040abe0 END
	// !DECL 0x0040bdd0 BEGIN
	/* 40BDD0 */ uchar* GetClassIdentifier();
	// !DECL 0x0040bdd0 END
	// !DECL 0x0040bde0 BEGIN
	/* 40BDE0 */ uchar DeletingDestructorThunk_18(uchar param_1);
	// !DECL 0x0040bde0 END
	// !DECL 0x0040bdf0 BEGIN
	/* 40BDF0 */ uchar DeletingDestructorThunk_4(uchar param_1);
	// !DECL 0x0040bdf0 END
	// !DECL 0x0040be00 BEGIN
	/* 40BE00 */ uchar DeletingDestructorThunk_10(uchar param_1);
	// !DECL 0x0040be00 END
	// !DECL 0x0040e120 BEGIN
	/* 40E120 */ uchar Destructor(uint* param_1);
	// !DECL 0x0040e120 END
	// !DECL 0x0040efd0 BEGIN
	/* 40EFD0 */ uint* Create();
	// !DECL 0x0040efd0 END
	// !DECL 0x0040f070 BEGIN
	/* 40F070 */ uint* ScalarDeletingDestructor(uchar param_1);
	// !DECL 0x0040f070 END
};

#endif
