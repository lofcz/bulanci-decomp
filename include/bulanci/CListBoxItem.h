#ifndef _C_LIST_BOX_ITEM
#define _C_LIST_BOX_ITEM

#include <globals.h>

class CListBoxItem {
public:
	// !DECL 0x00404c70 BEGIN
	/* 404C70 */ uchar* CListBoxItem_GetTypeInfo();
	// !DECL 0x00404c70 END
	// !DECL 0x00404cb0 BEGIN
	/* 404CB0 */ uchar CListBoxItem_dtor(uint* param_1);
	// !DECL 0x00404cb0 END
	// !DECL 0x004066c0 BEGIN
	/* 4066C0 */ void* CListBoxItem_ScalarDeletingDtor(uchar param_1);
	// !DECL 0x004066c0 END
	// !DECL 0x0040b640 BEGIN
	/* 40B640 */ void* CListBoxItem_ctor(wchar_t* param_1);
	// !DECL 0x0040b640 END
};

#endif
