#ifndef _C_LEVEL_LIST
#define _C_LEVEL_LIST

#include <globals.h>

class CLevelList {
public:
	// !DECL 0x0040b0c0 BEGIN
	/* 40B0C0 */ CListBox* CLevelList_ctor(CListBox* param_1);
	// !DECL 0x0040b0c0 END
	// !DECL 0x0040b120 BEGIN
	/* 40B120 */ uchar* CLevelList_GetTypeDescriptor();
	// !DECL 0x0040b120 END
	// !DECL 0x0040d490 BEGIN
	/* 40D490 */ uchar CLevelList_RenderItem(int* param_1, int param_2);
	// !DECL 0x0040d490 END
};

#endif
