#ifndef _C_BLACK_VIEW
#define _C_BLACK_VIEW

#include <globals.h>

class CBlackView {
public:
	// !DECL 0x00403520 BEGIN
	/* 403520 */ uchar* CBlackView_GetClassTable();
	// !DECL 0x00403520 END
	// !DECL 0x00404590 BEGIN
	/* 404590 */ void OnDraw(void* param_1);
	// !DECL 0x00404590 END
	// !DECL 0x00404e00 BEGIN
	/* 404E00 */ uint* CreateObject();
	// !DECL 0x00404e00 END
};

#endif
