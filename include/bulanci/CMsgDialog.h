#ifndef _C_MSG_DIALOG
#define _C_MSG_DIALOG

#include <globals.h>

class CMsgDialog {
public:
	// !DECL 0x0040b760 BEGIN
	/* 40B760 */ ClassRegEntry* CMsgDialog_GetClassTable();
	// !DECL 0x0040b760 END
	// !DECL 0x0040be80 BEGIN
	/* 40BE80 */ void* CMsgDialog_ctor(int param_1, uint* param_2);
	// !DECL 0x0040be80 END
	// !DECL 0x0040e780 BEGIN
	/* 40E780 */ uint* CMsgDialog_Allocate();
	// !DECL 0x0040e780 END
};

#endif
